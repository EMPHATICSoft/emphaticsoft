///////////////////////////////////////////////////////////////////////
// Class:       LAPPDRawMerger
// Module Type: art::Producer
// Author:      fnicolas@fnal.gov
// Description: Reads art::Event with other subsystems data products,
//              reads LAPPD binary file (path from fhicl), unpacks
//              LAPPD raw digits, and puts LAPPDRawDigit objects in
//              the art event
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDRawMerger
#define LAPPD_LAPPDRawMerger

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

// C/C++ includes
#include <string>
#include <vector>

// LAPPD includes
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDUnpacker/LAPPDBinaryUnpacker.h"

namespace emph {
  namespace rawdata {

    class LAPPDRawMerger : public art::EDProducer {
    public:

      explicit LAPPDRawMerger(fhicl::ParameterSet const& pset); 
      ~LAPPDRawMerger() override = default;
    

      void produce(art::Event& evt);

    private:

      // Input parameters
      std::vector<std::string> fLAPPDBinaryFiles; // List of LAPPD binary files to read
      int fVerbosity;

      // LAPPD unpacker
      lappd::LAPPDBinaryUnpacker fUnpacker;

      int fUnpackerEvtIx; // Current event index in the unpacker
      unsigned int fCurrentFileIx; // Current file index being processed
      int fNEventsProcessed; // Number of events processed so far

      void updateInputBinaryFile();

    };
  }
}

DEFINE_ART_MODULE( emph::rawdata::LAPPDRawMerger )

#endif
