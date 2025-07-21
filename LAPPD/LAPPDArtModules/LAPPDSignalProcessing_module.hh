///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessing
// Module Type: art::Producer
// Author:      fnicolas@fnal.gov
// Description: LAPPD signal processing interface proudcer
//              Reads LAPPD raw digits from the event,
//              calls the LAPPDSignalProcessor class, 
//              and puts LAPPDADCReco objects in the art event
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDSignalProcessing
#define LAPPD_LAPPDSignalProcessing

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"


// ROOT includes
#include "TTree.h"
#include "TFile.h"

// C/C++ includes
#include <string>
#include <vector>

// LAPPD includes
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDObj/LAPPDADCReco.h"
#include "LAPPD/LAPPDReco/LAPPDSignalProcessor.hh"
#include "LAPPD/LAPPDReco/LAPPDSignalProcessorConfig.hh"

namespace emph {
  namespace lappdreco {

    class LAPPDSignalProcessing : public art::EDProducer {
    public:

      explicit LAPPDSignalProcessing(fhicl::ParameterSet const& pset); 
      ~LAPPDSignalProcessing() override = default;
    

      void produce(art::Event& evt);

    private:

      // Input parameters
      std::string fLAPPDRawLabel;
      int fVerbosity;

      // Signal processor instance
      lappd::LAPPDSignalProcessor *fSignalProcessor;

    };
  }
}

DEFINE_ART_MODULE( emph::lappdreco::LAPPDSignalProcessing )

#endif
