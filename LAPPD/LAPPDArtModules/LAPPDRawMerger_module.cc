///////////////////////////////////////////////////////////////////////
// Class:       LAPPDRawMerger
// Module Type: art::Producer
// Author:      fnicolas@fnal.gov
// Description: Source file for LAPPDRawMerger module
////////////////////////////////////////////////////////////////////////

#include "LAPPD/LAPPDArtModules/LAPPDRawMerger_module.hh"

// Constructor
emph::rawdata::LAPPDRawMerger::LAPPDRawMerger(fhicl::ParameterSet const& pset)
  : art::EDProducer{pset},
    fLAPPDBinaryFiles(pset.get<std::vector<std::string>>("LAPPDBinaryFiles")),
    fVerbosity(pset.get<int>("Verbosity", 0))
{

  // Initialize the unpacker with the first file
  if (fLAPPDBinaryFiles.empty()) {
    throw art::Exception(art::errors::Configuration) << "No LAPPD binary files specified in the configuration.";
  }
  fUnpacker.setFileHandle( fLAPPDBinaryFiles[0] );
  fUnpackerEvtIx = 0; // Reset event index for unpacker
  fCurrentFileIx = 0; // Start with the first file
  if (fVerbosity > 0) {
    mf::LogInfo("LAPPDEventGen") << "  LAPPDEventGen::readFile initialized unpacker for file: " << fLAPPDBinaryFiles[0]
                                  << " with NEvents = " << fUnpacker.getNEvents();
  }
  // Register the LAPPDRawDigit product
  produces< std::vector<lappd::LAPPDRawDigit > >();

  // Initialize the number of events processed
  fNEventsProcessed = 0;
}

// Method to update the input binary file
void emph::rawdata::LAPPDRawMerger::updateInputBinaryFile(){

  // Check if we have reached the end of the unpacker events
  if (fUnpackerEvtIx >= fUnpacker.getNEvents()) {
    if (fVerbosity > 0) {
      mf::LogInfo("LAPPDRawMerger") << "Reached the end of the LAPPD binary file. No more events to process.";
    }

    // Read the next file if available
    fCurrentFileIx++;
    if (fCurrentFileIx < fLAPPDBinaryFiles.size()) {
      fUnpacker.setFileHandle(fLAPPDBinaryFiles[fCurrentFileIx]);
      fUnpackerEvtIx = 0; // Reset event index for the new file
      if (fVerbosity > 0) {
        mf::LogInfo("LAPPDRawMerger") << "Switched to next LAPPD binary file: " << fLAPPDBinaryFiles[fCurrentFileIx];
      }
    } else {
      if (fVerbosity > 0) {
        mf::LogInfo("LAPPDRawMerger") << "No more LAPPD binary files to process.";
      }
      return; // No more files to process
    }

  }

}


// Produce method
void emph::rawdata::LAPPDRawMerger::produce(art::Event& evt)
{
  // Create a collection to hold the LAPPDRawDigit objects
  auto lappdRawDigits = std::make_unique< std::vector<lappd::LAPPDRawDigit> >();

  // Process the next event from the unpacker - upstream LAPPD
  lappd::LAPPDRawDigit rawDigit = fUnpacker.readNextEvent();
  lappdRawDigits->push_back(rawDigit);
  fNEventsProcessed++;
  fUnpackerEvtIx++;
  updateInputBinaryFile(); // Check if we need to switch files

  // Process the next event from the unpacker - downstream LAPPD
  lappd::LAPPDRawDigit rawDigitDownstream = fUnpacker.readNextEvent();
  lappdRawDigits->push_back(rawDigitDownstream);
  fNEventsProcessed++;
  fUnpackerEvtIx++;
  updateInputBinaryFile(); // Check if we need to switch files

  // Put the collection into the event
  evt.put(std::move(lappdRawDigits));
}