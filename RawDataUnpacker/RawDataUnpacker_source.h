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
#include "fhiclcpp/types/ConfigurationTable.h"
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

#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"

namespace emph {
  namespace rawdata {

    class Unpacker
    {
    public:
      struct Config {
        fhicl::Atom<std::string> daqLabel{fhicl::Name("daqLabel"), "daq"};
        fhicl::Atom<bool> createArtEvents{fhicl::Name("createArtEvents"), true};
        fhicl::Atom<int> numWaveFormPlots{fhicl::Name("numWaveFormPlots"), 100};
        fhicl::Atom<uint64_t> timeWindow{fhicl::Name("timeWindow"), 20000};
        fhicl::Atom<uint64_t> nEvents{fhicl::Name("nEvents"), static_cast<uint64_t>(-1)};
        fhicl::Atom<int> verbosity{fhicl::Name("verbosity"), 0};
        fhicl::Atom<std::string> SSDFilePrefix{fhicl::Name("SSDFilePrefix"), "RawDataSaver0FER1_Run"};
        fhicl::Atom<bool> readSSDData{fhicl::Name("readSSDData"), false};
        fhicl::Atom<bool> readCAENData{fhicl::Name("readCAENData"), false};
        fhicl::Atom<bool> readTRB3Data{fhicl::Name("readTRB3Data"), false};
        fhicl::Atom<int> NFER{fhicl::Name("NFER"), 0};
        fhicl::Atom<double> BCOx{fhicl::Name("BCOx"), 151.1515152};
        fhicl::Atom<bool> firstSubRunHasExtraTrigger{fhicl::Name("firstSubRunHasExtraTrigger"), false};
        fhicl::Atom<bool> makeTDiffHistos{fhicl::Name("makeTDiffHistos"), false};
        fhicl::Atom<bool> makeTimeWalkHistos{fhicl::Name("makeTimeWalkHistos"), false};
      };
      using Parameters = fhicl::WrappedTable<Config>;

      Unpacker(Unpacker const&) = delete;
      Unpacker& operator=(Unpacker const&) = delete;

      explicit Unpacker(Parameters const& ps,
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
      void    makeTDiffHistos();
      void    makeDiffDiffHistos(artdaq::Fragment::fragment_id_t idA, artdaq::Fragment::fragment_id_t idB);

      bool    fixSSDTimestamps();
      bool    determineGrandfather();
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
      bool    fFirstSubRunHasExtraTrigger;
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
      uint64_t fSSDT0;
      size_t  fSSDEvtIdx;
      //      std::vector<std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > >
      std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > fSSDRawDigits;

      std::unordered_map<artdaq::Fragment::fragment_id_t,uint64_t> fT0;
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<int>> masks;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWCorr0;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWCorr1;

      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWErr0;
      std::unordered_map<artdaq::Fragment::fragment_id_t,double> fTWErr1;

      art::SourceHelper const& fSourceHelper;
      std::string   fDAQDataLabel;
      std::string   fCurrentFilename;

      std::vector<artdaq::Fragment::fragment_id_t> fFragId;
      artdaq::Fragment::fragment_id_t fragIdGrandfather;

      std::unordered_map<artdaq::Fragment::fragment_id_t,size_t> fFragCounter;
      std::unordered_map<artdaq::Fragment::fragment_id_t,size_t> fFragOffset;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > fFragTimestamps;
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
