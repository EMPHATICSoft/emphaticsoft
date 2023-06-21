#ifndef SSD_UNPACKER
#define SSD_UNPACKER

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
#include "fhiclcpp/types/Atom.h"

#include "TTree.h"
#include "TH1I.h"
#include "TDirectory.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"

namespace emph {
  namespace rawdata {

    class SSDUnpacker
    {
    public:
      SSDUnpacker(SSDUnpacker const&) = delete;
      SSDUnpacker& operator=(SSDUnpacker const&) = delete;

      explicit SSDUnpacker(fhicl::ParameterSet const& ps,
			art::ProductRegistryHelper& help,
			art::SourceHelper const& pm);
      
      virtual ~SSDUnpacker() {};
      void closeCurrentFile();
      void readFile(std::string const& name, art::FileBlock*& );
      
      bool readNext(art::RunPrincipal* const& inR,
		    art::SubRunPrincipal* const& inSR,
		    art::RunPrincipal*& outR,
		    art::SubRunPrincipal*& outSR,
		    art::EventPrincipal*& outE);
      
    private:
      bool    createSSDDigits();
      bool    fIsFirst;
      int     fVerbosity;
      int     fRun;
      int     fSubrun;
      int64_t fNEvents;
      int64_t fEvtCount;

      emph::cmap::ChannelMap* fChannelMap;
      runhist::RunHistory* fRunHistory;
      
      size_t  fSSDEvtIdx;

      std::vector<std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit> > > fSSDRawDigits;
      
      art::SourceHelper const& fSourceHelper;
      std::string   fCurrentFilename;

      TH1I* fSSDVecSizeHist;

    };
  }
}

#endif
