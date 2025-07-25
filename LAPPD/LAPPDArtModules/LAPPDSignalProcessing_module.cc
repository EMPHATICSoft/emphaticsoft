///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessing
// Module Type: art::Producer
// Author:      fnicolas@fnal.gov
// Description: Source code for LAPPDSignalProcessing module
////////////////////////////////////////////////////////////////////////


#include "LAPPD/LAPPDArtModules/LAPPDSignalProcessing_module.hh"


/***************************************************************************/
// Constructor
emph::lappdreco::LAPPDSignalProcessing::LAPPDSignalProcessing(fhicl::ParameterSet const& pset)
  : EDProducer(pset)
  , fLAPPDRawLabel( pset.get<std::string>("LAPPDRawLabel") )
  , fVerbosity( pset.get<int>("Verbosity", 0) )
{

  // --- Initialize the signal processor with configuration parameters
  fhicl::ParameterSet signalProcessorFhiclConfig = pset.get<fhicl::ParameterSet>("SignalProcessorConfig");
  const lappd::LAPPDSignalProcessorConfig config(
    signalProcessorFhiclConfig.get<bool>("DoBaselineSubtraction"),
    signalProcessorFhiclConfig.get<bool>("DoClockShiftCorrection"),
    signalProcessorFhiclConfig.get<bool>("DoSmoothing"),
    signalProcessorFhiclConfig.get<bool>("DoZeroFrequencyRemoval"),
    signalProcessorFhiclConfig.get<bool>("DoDeconvolution"),
    signalProcessorFhiclConfig.get<std::string>("BaselineFile", ""), // Default empty string if not provided
    signalProcessorFhiclConfig.get<int>("Verbosity", 0) // Default verbosity level is 0
  );
  fSignalProcessor = new lappd::LAPPDSignalProcessor(config);
  
  // Register the produce function
  produces< std::vector<lappd::LAPPDADCReco> >();
}

/***************************************************************************/
// Main produce function
void emph::lappdreco::LAPPDSignalProcessing::produce(art::Event& evt){

  // --- Define the output collection for ADC waveforms
  std::unique_ptr< std::vector<lappd::LAPPDADCReco> > lappdADCRecoPtr( new std::vector< lappd::LAPPDADCReco > );

  // --- Get the LAPPD raw digits from the event
  art::Handle< std::vector<lappd::LAPPDRawDigit> > rawDigitsHandle;
  if (!evt.getByLabel(fLAPPDRawLabel, rawDigitsHandle)) {
    mf::LogError("LAPPDSignalProcessing") << "Could not find LAPPD raw digits with label: " << fLAPPDRawLabel;
    return;
  }

  // --- Loop over the raw digits and process them
  for(auto const& rawDigit : *rawDigitsHandle) {
    if(fVerbosity > 0) {
      mf::LogInfo("LAPPDSignalProcessing") << "Processing LAPPD raw digit with ACDC number: " 
                                            << rawDigit.GetACDCNumber();
    }

    // Create a vector to hold the ADC waveforms
    lappd::LAPPDADCReco adcWaveforms = fSignalProcessor->ProcessLAPPDRawDigits(rawDigit);

    // Store the processed ADC waveform in the output collection
    lappdADCRecoPtr->emplace_back(adcWaveforms);
  }

  // --- Put into the event
  evt.put( std::move( lappdADCRecoPtr ) );

}


