///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessor
// Author:      fnicolas@fnal.gov
// Description: Source file for LAPPD signal processing
////////////////////////////////////////////////////////////////////////


#include "LAPPD/LAPPDReco/LAPPDSignalProcessor.hh"

// ------- Constructor ---------------------------------------------------
lappd::LAPPDSignalProcessor::LAPPDSignalProcessor( const lappd::LAPPDSignalProcessorConfig& config ) :
  fConfig(config)
{
  if(config.verbosity > 0) {
    std::cout << "LAPPDSignalProcessor constructor called with parameters: "
              << "DoBaselineSubtraction: " << config.doBaselineSubtraction
              << ", DoClockShiftCorrection: " << config.doClockShiftCorrection
              << ", DoSmoothing: " << config.doSmoothing
              << ", DoZeroFrequencyRemoval: " << config.doZeroFrequencyRemoval
              << ", DoDeconvolution: " << config.doDeconvolution
              << ", Verbosity: " << config.verbosity
              << std::endl;   
  }

}

// ------- ProcessLAPPDRawDigits ------------------------------------------
lappd::LAPPDADCReco lappd::LAPPDSignalProcessor::ProcessLAPPDRawDigits(const lappd::LAPPDRawDigit& rawDigit)
{

  // --- Create a new LAPPDADCReco object
  lappd::LAPPDADCReco adcReco(rawDigit);



  return adcReco; // Return the processed ADC waveform

}