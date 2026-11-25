#ifndef RAWDATA_UNPACKER
#define RAWDATA_UNPACKER

#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "canvas/Persistency/Provenance/Timestamp.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "artdaq-core/Data/RawEvent.hh"
#include "artdaq-core/Utilities/TimeUtils.hh"
#include "artdaq-core/Data/Fragment.hh"
#include "fhiclcpp/types/Atom.h"

#include "TTree.h"
#include "TH1I.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TDirectory.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <unordered_map>
#include <tuple>

#include "ChannelMap/ChannelMap.h"
#include "DataQuality/SyncQuality.h"
#include "RunHistory/RunHistory.h"

namespace emph {
  namespace rawdata {

    class Unpacker
    {
    public:
      Unpacker(Unpacker const&) = delete;
      Unpacker& operator=(Unpacker const&) = delete;

      explicit Unpacker(fhicl::ParameterSet const& ps,
			art::ProductRegistryHelper& help,
			art::SourceHelper const& pm);

      virtual ~Unpacker() {};
      void closeCurrentFile();
      void readFile(std::string const& name, art::FileBlock*& fb);

      //      bool hasMoreData() const { return !shutdownMsgReceived_; }

      bool readNext(art::RunPrincipal* const& inR,
		    art::SubRunPrincipal* const& inSR,
		    art::RunPrincipal*& outR,
		    art::SubRunPrincipal*& outSR,
		    art::EventPrincipal*& outE);

    private:
      bool    createDigitsFromArtdaqEvent();
      bool    createSSDDigits();
      void    readTRB3Containers(TTree& events,
				 std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::TRB3RawDigit> > >& digits,
				 std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> >& timestamps,
				 std::vector<artdaq::Fragment::fragment_id_t>& boards);
      bool    readNeighbourTRB3(size_t ineighbour);
      void    makeTDiffHistos();
      void    makeDiffDiffHistos(artdaq::Fragment::fragment_id_t idA, artdaq::Fragment::fragment_id_t idB);

      bool    fixSSDTimestamps();
      bool    fixCAENTimestamps();
      bool    determineGrandfather();
      bool    seedTimeSync();
      void    dropUnsyncedBoards();
      void    setBoardDetectors();
      void    prepareEventSync();
      bool    findMatches(artdaq::Fragment::fragment_id_t idChild, int timeUncertainty);
      bool    findMatches(int timeUncertainty);
      void    calcTimeWalkCorr();
      void    calcTimeWalkCorr(artdaq::Fragment::fragment_id_t idChild);

      bool    fIsFirst;
      bool    fCreateArtEvents;
      bool    fMakeTDiffHistos;
      bool    fMakeTimeWalkHistos;
      bool    fMakeBenchmarkPlots;
      bool    fReadSSDData;
      bool    fReadTRB3Data;
      bool    fReadCAENData;
      bool    fSeedTimeSync;
      double  fSeedMinMatchFraction;
      double  fMinMatchFraction;
      bool    fSSDSynced;
      int     fVerbosity;
      int     fNumWaveFormPlots;
      int     fRun;
      int     fSubrun;
      int     fNFER;
      uint64_t fNEvents;
      uint64_t fEvtCount;
      uint64_t fTimeOffset;
      uint64_t fTimeWindow;
      uint64_t fPrevTS;
      double   fBCOx;

      art::Timestamp fSpillTime;

      emph::cmap::ChannelMap* fChannelMap;
		runhist::RunHistory* fRunHistory;

      std::string fSSDFilePrefix;
      //      std::vector<std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > >
      std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > fSSDRawDigits;

      std::unordered_map<artdaq::Fragment::fragment_id_t,uint64_t> fT0;
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<int>> masks;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWCorr0;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWCorr1;

      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWErr0;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWErr1;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWEdgeErr; // error of the corrected time at the spill edges (ns)

      art::SourceHelper const& fSourceHelper;
      std::string   fDAQDataLabel;
      std::string   fCurrentFilename;

      std::vector<artdaq::Fragment::fragment_id_t> fFragId;
      std::vector<artdaq::Fragment::fragment_id_t> fCAENFragIds; // V1720 boards (their timestamps wrap, see fixCAENTimestamps)
      std::vector<artdaq::Fragment::fragment_id_t> fTRB3FragIds; // TRB3 boards

      // TRB3 data of the neighbouring art files (previous, next): after beamless spills the DAQ
      // files a spill's TRB3 data into the wrong subrun, so they are read only if needed (see seedTimeSync)
      struct TRB3Neighbour {
	std::string label;
	int fileOffset; // subrun of the file minus this subrun
	std::string fileName;
	bool isRead = false;
	std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::TRB3RawDigit> > > digits;
	std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > timestamps;
	std::vector<artdaq::Fragment::fragment_id_t> boards;
      };
      std::vector<TRB3Neighbour> fTRB3Neighbours;

      emph::dq::SyncQuality fSyncQuality; // time-sync problems met in this spill; subrun product ("raw","TimeSync")
      // per-event time-sync record (EventSync, event product ("raw","TimeSync"))
      std::vector<bool> fSSDMasked; // SSD events whose timestamp kept only its lower 32 bits (fixSSDTimestamps)
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::pair<int64_t,int64_t> > > fSuspectSpans; // board time: reset gaps, unpinned blocks
      std::vector<std::tuple<int64_t,int64_t,uint32_t> > fSuspectWindows; // the same in grandfather time, with the board's detectors
      uint32_t fLostDetectors;    // no readout in this spill
      uint32_t fPartialDetectors; // part of the readout missing in this spill
      artdaq::Fragment::fragment_id_t fragIdGrandfather;

      std::unordered_map<artdaq::Fragment::fragment_id_t,size_t> fFragCounter;
      std::unordered_map<artdaq::Fragment::fragment_id_t,size_t> fFragOffset;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > fFragTimestamps;
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint32_t> > fCAENEventCounter; // V1720 header event counter (restarts at 1 when a board is reset)
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > fCAENHostTime;     // readout PC clock per event (ns), from the fragment metadata
		std::vector<std::pair<uint64_t, std::vector<std::pair<uint64_t,uint64_t>>>> eventStack;

      //std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > fFragTimestampsCorrected;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::WaveForm> > > fWaveForms;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::TRB3RawDigit> > > fTRB3RawDigits;

      std::unordered_map<artdaq::Fragment::fragment_id_t,TH2D*> fdTvsT;
      std::unordered_map<artdaq::Fragment::fragment_id_t,TGraph*> fTvsT;
      std::unordered_map<artdaq::Fragment::fragment_id_t,TH2D*> fTvsFrag;
      std::unordered_map<artdaq::Fragment::fragment_id_t,TH1D*> fTHist;
      TH2D* fSSDdTvsT;
      TH2D* fSSDTvsFrag;
      TH1D* fSSDTHist;
      TH2D* fSSDdTHist;

      TTree* fTRB3Tree;
      std::vector<uint32_t> fTRB3_HeaderWord;
      std::vector<uint32_t> fTRB3_Measurement;
      std::vector<uint32_t> fTRB3_Channel;
      std::vector<uint32_t> fTRB3_FineTime;
      std::vector<uint32_t> fTRB3_EpochTime;
      std::vector<uint32_t> fTRB3_CoarseTime;

      std::unordered_map<int, int> fC1720_HistCount;

      std::vector<std::unique_ptr<std::ifstream>> ssd_file_handles;
      std::string fSSDPath;

    };
  }
}

#endif
