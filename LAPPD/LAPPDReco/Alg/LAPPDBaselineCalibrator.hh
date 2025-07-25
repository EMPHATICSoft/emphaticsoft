
///////////////////////////////////////////////////////////////////////
// Class:       LAPPDBaselineCalibrator
// Author:      fnicolas@fnal.gov
// Description: Class for LAPPD baseline calibration
//              Requires input file with pre-calibrated baseline values
//              Functions:
//              GetBaseline(unsigned int channel):
//             - Returns vector of baseline values for the specified channel
//              GetBaseline(unsigned int channel, unsigned int bin):
//             - Returns baseline value for the specified channel and bin
//              Calibrate(lappd::LAPPDRawDigit& rawDigit):
//              Calibrate(lappd::LAPPDADCReco& adcReco):
//             - Returns lappd::LAPPDADCReco object with baseline subtracted
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDBaselineCalibrator
#define LAPPD_LAPPDBaselineCalibrator

// C/C++ includes
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

// LAPPD includes
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDObj/LAPPDADCReco.h"
#include "LAPPD/LAPPDObj/LAPPD_format.h"

namespace lappd {

  class LAPPDBaselineCalibrator {
  public:

    // Constructor
    LAPPDBaselineCalibrator( std::string baselineFile);

    // Destructor
    ~LAPPDBaselineCalibrator() = default;

    // Get baseline values for a specific channel
    std::vector<float> GetBaseline(unsigned int channel) const;

    // Get baseline value for a specific channel and bin
    float GetBaseline(unsigned int channel, unsigned int bin) const;

    // Calibrate a raw digit/ADC waveform by subtracting the baseline
    lappd::LAPPDADCReco Calibrate(const lappd::LAPPDRawDigit& rawDigit) const;
    lappd::LAPPDADCReco Calibrate(const lappd::LAPPDADCReco& adcReco) const;
  

  private:

    // Input parameters
    std::string fBaselineFile; // File containing pre-calibrated baseline values
    
    // Pre-stored baseline values
    float fBaselineValues[lappd::kNChannels][lappd::kNTicks];
  };

} // namespace lappd


#endif // LAPPD_LAPPDBaselineCalibrator