///////////////////////////////////////////////////////////////////////
// Class:       LAPPDCircularBufferShifter
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

#ifndef LAPPD_LAPPDCircularBufferShifter
#define LAPPD_LAPPDCircularBufferShifter

// C/C++ includes
#include <vector>
#include <limits>
#include <iostream>

// ROOT includes
#include <TMinuit.h>
#include <TMath.h>
#include <TMath.h>

namespace lappd {

  class LAPPDCircularBufferShifter {
  public:

    // Constructor
    LAPPDCircularBufferShifter( std::vector<float> adcs );

    // Destructor
    ~LAPPDCircularBufferShifter() = default;

    // Set new ADC values
    void SetADCs(const std::vector<float>& adcs) {
      fADCs = adcs;
    }

    // Get the best shift value
    unsigned int GetBestShift() const {
      return fBestShift;
    }

    // Update the best shift value
    void UpdateBestShift();

  private:

    // Vector of ADC values
    std::vector<float> fADCs; // Circular buffer of ADC values

    // Best shift value
    unsigned int fBestShift; // Best shift value found

    // Circularly shift a vector
    std::vector<double> CircularShift(unsigned int shift);

    TMinuit* fMinuitPtr;
    
  };

} // namespace lappd


#endif // LAPPD_LAPPDCircularBufferShifter