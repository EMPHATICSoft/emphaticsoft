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

#include "LAPPD/LAPPDReco/Alg/LAPPDCircularBufferShifter.hh"

// Constructor
lappd::LAPPDCircularBufferShifter::LAPPDCircularBufferShifter(std::vector<float> adcs)
    : fADCs(std::move(adcs)), fBestShift(0), fMinuitPtr(nullptr)
{
}



void lappd::LAPPDCircularBufferShifter::UpdateBestShift(){

    // Initializations
    double bestChi2 = std::numeric_limits<double>::max();
    int bestShift = -1;
    std::vector<double> shifted;
    shifted.reserve(fADCs.size());

    for (int shift = 0; shift < 256; ++shift) {
        shifted = CircularShift(shift);

        // Fit the shifted data to a sinusoidal function
        double chi2 = 0.0;

        
    }

    std::cout << "Best shift: " << bestShift << " with chi2 = " << bestChi2 << std::endl;
  
    fBestShift = 0; // Reset best shift to 0
}


// Circularly shift a vector
std::vector<double> lappd::LAPPDCircularBufferShifter::CircularShift(unsigned int shift) {
    std::vector<double> result( fADCs.size() );
    for (size_t i = 0; i < fADCs.size(); ++i)
        result[i] = fADCs[(i + shift) % fADCs.size()];
    return result;
}


