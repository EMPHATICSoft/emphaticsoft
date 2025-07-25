///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessor
// Author:      fnicolas@fnal.gov
// Description: Source file for LAPPD signal processing
////////////////////////////////////////////////////////////////////////


#include "LAPPD/LAPPDReco/LAPPDSignalProcessor.hh"

// ------- Constructor ---------------------------------------------------
lappd::LAPPDSignalProcessor::LAPPDSignalProcessor( const lappd::LAPPDSignalProcessorConfig& config ) :
  fConfig(config),
  fBaselineCalibrator(config.baselineFile)
{
  if(config.verbosity > 0) {
    std::cout << "LAPPDSignalProcessor constructor called with parameters: "
              << "DoBaselineSubtraction: " << config.doBaselineSubtraction
              << ", DoClockShiftCorrection: " << config.doClockShiftCorrection
              << ", DoSmoothing: " << config.doSmoothing
              << ", DoZeroFrequencyRemoval: " << config.doZeroFrequencyRemoval
              << ", DoDeconvolution: " << config.doDeconvolution
              << ", BaselineFile: " << config.baselineFile
              << ", Verbosity: " << config.verbosity
              << std::endl;   
  }



}

// ------- ProcessLAPPDRawDigits ------------------------------------------
lappd::LAPPDADCReco lappd::LAPPDSignalProcessor::ProcessLAPPDRawDigits(const lappd::LAPPDRawDigit& rawDigit)
{

  // --- Create a new LAPPDADCReco object
  lappd::LAPPDADCReco adcReco(rawDigit);

  // --- Apply baseline calibration if enabled
  if (fConfig.doBaselineSubtraction) {
    std::cout << "Applying baseline calibration..." << std::endl;
    adcReco = fBaselineCalibrator.Calibrate(adcReco);
  }

  // --- Apply clock shift correction if enabled
  if(fConfig.doClockShiftCorrection){
    std::cout << "Applying clock shift correction..." << std::endl;
  }

  return adcReco; // Return the processed ADC waveform
}