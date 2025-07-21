///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessorConfig
// Author:      fnicolas@fnal.gov
// Description: Configuration class for LAPPD signal processing
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDSignalProcessorConfig
#define LAPPD_LAPPDSignalProcessorConfig


namespace lappd {

  struct LAPPDSignalProcessorConfig {
    bool doBaselineSubtraction; // Enable baseline subtraction
    bool doClockShiftCorrection; // Enable clock shift correction
    bool doSmoothing; // Enable waveform smoothing
    bool doZeroFrequencyRemoval; // Enable zero-frequency removal
    bool doDeconvolution; // Enable waveform deconvolution
    int verbosity; // Verbosity level for logging

    // Default constructor
    
    LAPPDSignalProcessorConfig(){};

    // Constructor with parameters
    LAPPDSignalProcessorConfig(bool _baselineSubtraction,
                                bool _clockShiftCorrection,
                                bool _smoothing,
                                bool _zeroFrequencyRemoval,
                                bool _deconvolution,
                                int _verbosity = 0)
      : doBaselineSubtraction(_baselineSubtraction),
        doClockShiftCorrection(_clockShiftCorrection),
        doSmoothing(_smoothing),
        doZeroFrequencyRemoval(_zeroFrequencyRemoval),
        doDeconvolution(_deconvolution),
        verbosity(_verbosity) {}
  }; // struct LAPPDSignalProcessorConfig

} // namespace lappdreco


#endif // LAPPD_LAPPDSignalProcessorConfig