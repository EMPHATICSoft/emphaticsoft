#ifndef RAWDATA_UNPACKER
#define RAWDATA_UNPACKER

#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "artdaq-core/Data/RawEvent.hh"
#include "artdaq-core/Utilities/TimeUtils.hh"
#include "artdaq-core/Data/Fragment.hh"
#include "fhiclcpp/types/Atom.h"

#include "TTree.h"
#include "TH1I.h"
#include "TDirectory.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "ChannelMap/ChannelMap.h"

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
      void    makeTDiffHistos();
      bool    fIsFirst;
      bool    fCreateArtEvents;
      bool    fMakeTDiffHistos;
      bool    fReadSSDData;
      bool    fReadTRB3Data;
      int     fVerbosity;
      int     fNumWaveFormPlots;
      int     fRun;
      int     fSubrun;
      uint64_t fNEvents;
      uint64_t fEvtCount;
      uint64_t fTimeOffset;
      uint64_t fTimeWindow;
      uint64_t fPrevTS;
      std::string fChanMapFileName;

      emph::cmap::ChannelMap* fChannelMap;

      std::string fSSDFilePrefix;
      uint64_t fSSDT0;      
      size_t  fSSDEvtIdx;
      //      std::vector<std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > >
      std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > fSSDRawDigits;
      
      std::unordered_map<artdaq::Fragment::fragment_id_t,uint64_t> fT0;
      
      art::SourceHelper const& fSourceHelper;
      std::string   fDAQDataLabel;
      std::string   fCurrentFilename;

      std::vector<artdaq::Fragment::fragment_id_t> fFragId;

      std::unordered_map<artdaq::Fragment::fragment_id_t,size_t> fFragCounter;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<uint64_t> > fFragTimestamps;

      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::WaveForm> > > fWaveForms;
      
      std::unordered_map<artdaq::Fragment::fragment_id_t,std::vector<std::vector<emph::rawdata::TRB3RawDigit> > > fTRB3RawDigits;

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
