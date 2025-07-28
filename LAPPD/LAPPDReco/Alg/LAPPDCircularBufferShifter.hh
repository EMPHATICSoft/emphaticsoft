///////////////////////////////////////////////////////////////////////
// Class:       LAPPDCircularBufferShifter
// Author:      fnicolas@fnal.gov
// Description: TO DOOOOOOO!!
////////////////////////////////////////////////////////////////////////

#ifndef LAPPD_LAPPDCircularBufferShifter
#define LAPPD_LAPPDCircularBufferShifter

// C/C++ includes
#include <vector>
#include <limits>
#include <iostream>
#include <numeric>

// ROOT includes
#include <TMath.h>
#include <TF1.h>
#include <TH1F.h>
#include <TFitResult.h>
#include "TCanvas.h"

// LAPPD includes
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDObj/LAPPDADCReco.h"
#include "LAPPD/LAPPDObj/LAPPD_format.h"
#include "LAPPD/LAPPDReco/LAPPDSignalProcessorConfig.hh"

namespace lappd {

  class LAPPDCircularBufferShifter {
  public:

    // Constructor
    LAPPDCircularBufferShifter(lappd::LAPPDCircularBufferShifterConfig config);

    // Destructor
    ~LAPPDCircularBufferShifter() = default;

    // Update the best shift value
    void UpdateBestShift(lappd::LAPPDRawDigit const& rawDigit);

    // Shift all waveforms in the event
    lappd::LAPPDADCReco ShiftWaveforms(lappd::LAPPDADCReco const& adcReco);
    
  private:

    // Configuration parameters
    lappd::LAPPDCircularBufferShifterConfig fConfig; // Configuration parameters for the circular buffer shifter

    // Vectors to store the clock signal data
    std::vector<float> fADCs; // original signal
    std::vector<float> fShiftedADCs; // placeholder for shifted signal

    // TH1F for the sinusoidal fit
    TH1F* fClockSignalHist;

    // Fitting function
    TF1 *fFitTF1;

    // Best shift value
    unsigned int fBestShift; // Best shift value found    

    // Current shift value
    unsigned int fCurrentShift; // Current shift value being applied

    // Sinusoidal fit parameters
    float fAmplitude; // Amplitude of the sinusoidal fit
    float fFrequency; // Frequency of the sinusoidal fit
    float fPhase; // Phase of the sinusoidal fit
    float fOffset; // Offset of the sinusoidal fit
    
    // Initialize the clock signal for the new event
    void InitializeClockSignal(lappd::LAPPDRawDigit const& rawDigit);

    // Apply circular shift to the clock signal
    void CircularShift(unsigned int shift);
    
    // Make the fit to the shifted data
    double MakeFit();
    
  };

} // namespace lappd


#endif // LAPPD_LAPPDCircularBufferShifter