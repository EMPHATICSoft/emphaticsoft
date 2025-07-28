///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessor
// Author:      fnicolas@fnal.gov
// Description: Main class for LAPPD signal processing
//              Receives LAPPD raw digits,
//              and returns LAPPDADCReco objects
//              Options for processing are set in the constructor
//              - Pre-calibrated baseline subtraction
//              - Clock shift correction
//              - Signal processing: waveform smoothing
//              - Signal processing: baseline subtraction with zero-frequency removal
//              - Signal processing: waveform deconvolution - TO DO
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDSignalProcessor
#define LAPPD_LAPPDSignalProcessor

// C/C++ includes
#include <vector>
#include <limits>

// ROOT includes
#include <TMinuit.h>
#include <TMath.h>

// LAPPD includes
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDObj/LAPPDADCReco.h"
#include "LAPPD/LAPPDReco/LAPPDSignalProcessorConfig.hh"
#include "LAPPD/LAPPDReco/Alg/LAPPDBaselineCalibrator.hh"
#include "LAPPD/LAPPDReco/Alg/LAPPDCircularBufferShifter.hh"

namespace lappd {

  class LAPPDSignalProcessor {
  public:

    // Constructor
    LAPPDSignalProcessor( const lappd::LAPPDSignalProcessorConfig& config);
    // Destructor
    ~LAPPDSignalProcessor() = default;

    lappd::LAPPDADCReco ProcessLAPPDRawDigits(const lappd::LAPPDRawDigit& rawDigit);

  private:
  
    // Configuration parameters
    lappd::LAPPDSignalProcessorConfig fConfig;

    // Baseline calibrator
    lappd::LAPPDBaselineCalibrator fBaselineCalibrator;

    // Circular buffer shifter
    lappd::LAPPDCircularBufferShifter fCircularBufferShifter;
    
  };

} // namespace lappd


#endif // LAPPD_LAPPDSignalProcessor