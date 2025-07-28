///////////////////////////////////////////////////////////////////////
// Class:       LAPPDSignalProcessorConfig
// Author:      fnicolas@fnal.gov
// Description: Configuration class for LAPPD signal processing
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDSignalProcessorConfig
#define LAPPD_LAPPDSignalProcessorConfig


namespace lappd {


  // _____________________________________________________________________________
  struct LAPPDBaselineCalibratorConfig {
    std::string baselineFile; // File containing pre-calibrated baseline values
    int verbosity; // Verbosity level for logging

    // Default constructor
    LAPPDBaselineCalibratorConfig() : baselineFile(""), verbosity(0) {}

    // Constructor with parameters
    LAPPDBaselineCalibratorConfig(std::string _baselineFile, int _verbosity = 0)
      : baselineFile(_baselineFile), verbosity(_verbosity) {}
  }; // struct LAPPDBaselineCalibratorConfig


  // _____________________________________________________________________________
  struct LAPPDCircularBufferShifterConfig {
    int clockOffset; // Clock offset for circular buffer shifting
    unsigned int clockChannel; // Channel with the clock signal
    unsigned int nClockCycles; // Number of clock cycles to shift
    int verbosity; // Verbosity level for logging

    // Default constructor
    LAPPDCircularBufferShifterConfig() : clockOffset(0),
                                          clockChannel(0), 
                                          nClockCycles(0),
                                          verbosity(0) {}

    // Constructor with parameters
    LAPPDCircularBufferShifterConfig(int _clockOffset,
                                      unsigned int _clockChannel,
                                      unsigned int _nClockCycles,
                                      int _verbosity = 0)
      : clockOffset(_clockOffset),
        clockChannel(_clockChannel),
        nClockCycles(_nClockCycles),
        verbosity(_verbosity) {}
  }; // struct LAPPDCircularBufferShifterConfig


  // _____________________________________________________________________________
  struct LAPPDSignalProcessorConfig {
    bool doBaselineSubtraction; // Enable baseline subtraction
    bool doClockShiftCorrection; // Enable clock shift correction
    bool doSmoothing; // Enable waveform smoothing
    bool doZeroFrequencyRemoval; // Enable zero-frequency removal
    bool doDeconvolution; // Enable waveform deconvolution
    struct LAPPDBaselineCalibratorConfig baselineCalibratorConfig; // Baseline calibrator configuration
    struct LAPPDCircularBufferShifterConfig circularBufferConfig; // Circular buffer shifter configuration
    int verbosity; // Verbosity level for logging

    // Default constructor
    
    LAPPDSignalProcessorConfig(){};

    // Constructor with parameters
    LAPPDSignalProcessorConfig(bool _baselineSubtraction,
                                bool _clockShiftCorrection,
                                bool _smoothing,
                                bool _zeroFrequencyRemoval,
                                bool _deconvolution,
                                struct LAPPDBaselineCalibratorConfig _baselineConfig,
                                struct LAPPDCircularBufferShifterConfig _circularBufferConfig,
                                int _verbosity = 0)
      : doBaselineSubtraction(_baselineSubtraction),
        doClockShiftCorrection(_clockShiftCorrection),
        doSmoothing(_smoothing),
        doZeroFrequencyRemoval(_zeroFrequencyRemoval),
        doDeconvolution(_deconvolution),
        baselineCalibratorConfig(_baselineConfig),
        circularBufferConfig(_circularBufferConfig),
        verbosity(_verbosity) {}
  }; // struct LAPPDSignalProcessorConfig

} // namespace lappdreco


#endif // LAPPD_LAPPDSignalProcessorConfig