#ifndef LAPPD_LAPPDEventGen
#define LAPPD_LAPPDEventGen

#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "canvas/Persistency/Provenance/Timestamp.h"
#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Persistency/Provenance/RunAuxiliary.h"
#include "canvas/Persistency/Provenance/SubRunAuxiliary.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "fhiclcpp/types/Atom.h"

#include "TTree.h"
#include "TFile.h"
#include "TH1I.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDUnpacker/LAPPDBinaryUnpacker.h"

namespace emph {
  namespace rawdata {

    class LAPPDEventGen
    {
    public:
      LAPPDEventGen(LAPPDEventGen const&) = delete;
      LAPPDEventGen& operator=(LAPPDEventGen const&) = delete;

      explicit LAPPDEventGen(fhicl::ParameterSet const& ps,
                        art::ProductRegistryHelper& help,
                        art::SourceHelper const& pm);

      void closeCurrentFile();
      void readFile(std::string const& name, art::FileBlock*& fb);

      bool readNext(art::RunPrincipal* const& inR,
		    art::SubRunPrincipal* const& inSR,
		    art::RunPrincipal*& outR,
		    art::SubRunPrincipal*& outSR,
		    art::EventPrincipal*& outE);
      
    private:
      bool    fIsFirst;
      int     fVerbosity;
      int     fRun;
      int     fSubrun;
      int64_t fNEvents;
      int64_t fEvtCount;

      art::SourceHelper const& fSourceHelper;
      std::string fCurrentFilename;

      // LAPPD unpacker
      lappd::LAPPDBinaryUnpacker fUnpacker;
      int fUnpackerEvtIx;

      TTree* fTree;
     
    };
  }
}

#endif
