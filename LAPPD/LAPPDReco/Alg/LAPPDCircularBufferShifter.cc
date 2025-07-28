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
#include "TROOT.h"

// _____________________________________________________________________________  
// Define the fitting function: A * sin( a * x + phi) + offset
double sineFunction(double *x, double *par) {
    //return par[0] * sin(2 * TMath::Pi() * par[1] * x[0] + par[2]) + par[3]; // A*sin(2*pi*f*x + phi) + offset
    return par[0] * sin( par[1] * x[0] + par[2]) + par[3]; // A*sin(2*pi*f*x + phi) + offset
}


// _____________________________________________________________________________
// Constructor for LAPPDCircularBufferShifter
lappd::LAPPDCircularBufferShifter::LAPPDCircularBufferShifter(lappd::LAPPDCircularBufferShifterConfig config)
    : fConfig(config)
      , fClockSignalHist( new TH1F("ClockSignalHist", "Clock Signal Histogram", lappd::kNTicks, 0, lappd::kNTicks) )
      , fFitTF1 ( new TF1("fit_func", sineFunction, 0, lappd::kNTicks, 4) )
      , fBestShift(0) // Initialize best shift to 0
{
    if (config.verbosity > 0) {
        std::cout << "LAPPDCircularBufferShifter constructor called with parameters: "
                << "ClockOffset: " << config.clockOffset
                << ", ClockChannel: " << config.clockChannel
                << ", NClockCycles: " << config.nClockCycles
                << ", Verbosity: " << config.verbosity
                << std::endl;
    }

    // Initialize dimensions of the ADC vector
    fADCs.reserve(lappd::kNTicks); // Reserve space for 256 ticks
    fShiftedADCs.reserve(lappd::kNTicks); // Reserve space for 256 ticks
    
    // Initialize the sinusoidal fit parameters    
    // Set parameter names
    fFitTF1->SetParNames("Amplitude", "Frequency", "Phase", "Offset");
    fPhase = 0.0; // Dummy initial phase set to 0
    // Offset set to 0
    fOffset = 0.0; // Dummy initial offset set to 0
    fAmplitude = 1.0; // Dummy initial amplitude set to 0
    // !!!! TO DO: Make clock frequency a fhicl parameter?
    double tickConversionFactor = 25.0 / lappd::kNTicks; // Conversion factor for ticks to ns
    fFrequency = 2 * TMath::Pi() * 0.250 * tickConversionFactor; // Frequency in radians per tick

}


// _____________________________________________________________________________
// Initialize the clock signal for the new event
void lappd::LAPPDCircularBufferShifter::InitializeClockSignal(lappd::LAPPDRawDigit const& rawDigit) {
    
    // Clear the ADC vector
    fADCs.clear();

    // Get the clock channel data
    const LAPPDReadoutChannelRawDigit& clockChannel = rawDigit.GetChannel(fConfig.clockChannel);

    // Fill the ADC vector with the clock channel data
    for (int i = 0; i < clockChannel.NADCs(); ++i) {
        fADCs.push_back(static_cast<float>(clockChannel.ADC(i))); // Store ADC values as floats
    }

    // Reset the best shift to 0
    fBestShift = 0;

    if (fConfig.verbosity > 0) {
        std::cout << "Clock signal initialized with " << fADCs.size() << " ADC values." << std::endl;
    }

    // Update initial sinusoidal fit parameters
    // Offset set to the mean of the clock signal
    fOffset = std::accumulate(fADCs.begin(), fADCs.end(), 0.0) / fADCs.size(); // Calculate mean of the clock signal
    // Amplitude set to max - min of the clock signal, divided by 2
    fAmplitude = (*std::max_element(fADCs.begin(), fADCs.end()) - *std::min_element(fADCs.begin(), fADCs.end())) / 2.0;

    if(fConfig.verbosity > 0) {
        std::cout << "LAPPDCircularBufferShifter initialized with parameters: "
                  << "Offset: " << fOffset
                  << ", Amplitude: " << fAmplitude
                  << ", Frequency: " << fFrequency
                  << ", Phase: " << fPhase
                  << std::endl;
    }
}


// _____________________________________________________________________________
// Update the best shift value based on the clock signal
void lappd::LAPPDCircularBufferShifter::UpdateBestShift(lappd::LAPPDRawDigit const& rawDigit) {

    // Initializations
    InitializeClockSignal(rawDigit); // Initialize the clock signal with the new raw digit

    // Reduce range in which trial shifts are made
    // trigger_low_bound = (((sysTime + CLOCK_OFFSET) % NUM_OCTANTS) * OCTANT_LENGTH - (0)) % BUFFER_LENGTH
    uint64_t sysTime = rawDigit.GetTimeStamp();
    int clockCycleLength = lappd::kNTicks / fConfig.nClockCycles; // Length of one clock cycle in ticks

    unsigned int trigger_low_bound = ( ((sysTime + fConfig.clockOffset) % fConfig.nClockCycles) * clockCycleLength ) % lappd::kNTicks;
    unsigned int trigger_high_bound = trigger_low_bound + lappd::kNTicks/fConfig.nClockCycles;
    std::cout << "Trigger low bound: " << trigger_low_bound << ", high bound: " << trigger_high_bound << std::endl;

    // Loop through all possible shifts    
    double minChi2 = std::numeric_limits<double>::max(); // Initialize minimum chi2
    int bestShift = -1; // Initialize best shift
    for (unsigned int shift = trigger_low_bound; shift < trigger_high_bound; ++shift) {
        
        // Set the current shift value
        fCurrentShift = shift; 

        // Apply the circular shift
        CircularShift(shift);
        
        // Make the fit to the shifted data
        double chi2 = MakeFit(); // Fit the shifted data to a sinusoidal function 
        
        // Update the best shift if the current chi2 is lower than the minimum chi2 found so far
        if(chi2 < minChi2) {
            minChi2 = chi2;
            bestShift = shift;
        }       
    }

    if (fConfig.verbosity > 0)
        std::cout << "Best shift found: " << bestShift << " with chi2: " << minChi2 << std::endl;

    fBestShift = bestShift; // Update the best shift value
}


// Circularly shift a vector
void lappd::LAPPDCircularBufferShifter::CircularShift(unsigned int shift) {

    fShiftedADCs.clear(); // Clear the shifted ADC vector

    // Perform circular shift
    for (unsigned int i = 0; i < lappd::kNTicks; ++i) {
        unsigned int newIndex = (i + shift) % lappd::kNTicks; // Calculate new index with wrap-around
        fShiftedADCs.push_back(fADCs[newIndex]); // Store the shifted value
    }

    if (fConfig.verbosity > 0) {
        std::cout << "Circular shift applied with shift value: " << shift << " first ADC: " 
                  << fShiftedADCs[0] << " last ADC: " << fShiftedADCs.back() << std::endl;
    }
   
}



// _____________________________________________________________________________
double lappd::LAPPDCircularBufferShifter::MakeFit(){
    
    // Initialize fit parameters
    //TF1 *fit_func = new TF1("fit_func", sineFunction, 0, lappd::kNTicks, 4); // Create a TF1 object for the fit function
    fFitTF1->SetParameter(0, fAmplitude); // Set initial amplitude
    fFitTF1->SetParameter(1, fFrequency); // Set initial frequency
    fFitTF1->SetParameter(2, fPhase); // Set initial phase
    fFitTF1->SetParameter(3, fOffset); // Set initial offset

    // Prepare the data for fitting
    for (unsigned int i = 0; i < lappd::kNTicks; ++i) {
        fClockSignalHist->SetBinContent(i + 1, fShiftedADCs[i]); // Fill the histogram with shifted ADC values
    }
    
    // Make the fit to the histogram
    TFitResultPtr result = fClockSignalHist->Fit(fFitTF1, "QS"); // Fit the histogram with the fit function
    
    // Debug output
    if (fConfig.verbosity > 1) {
        std::cout << "Fit reuslts! "
                    << " Status = " << result.Get()->Status()
                    << " Chi2 = " << result.Get()->Chi2()
                    << ", Amplitude = " << fFitTF1->GetParameter(0)
                    << ", Frequency = " << fFitTF1->GetParameter(1)
                    << ", Phase = " << fFitTF1->GetParameter(2)
                    << ", Offset = " << fFitTF1->GetParameter(3)
                    << std::endl;

        TCanvas canvas( ("ClockSignalFit"+std::to_string(fCurrentShift)).c_str()
                       , ("Clock Signal Fit " + std::to_string(fCurrentShift)).c_str(),800, 600);
                       
        fClockSignalHist->Draw(); // Draw the histogram
        fFitTF1->Draw("same"); // Draw the fit function on top of the histogram
        canvas.SaveAs("ClockSignalFit.png"); // Save the canvas as an image
        canvas.Update(); // Update the canvas to reflect the changes
        canvas.WaitPrimitive(); // Wait for user interaction
    }

    // Return the chi2 value of the fit
    return result.Get()->Chi2(); // Return the chi2 value of the fit
}




// _____________________________________________________________________________
lappd::LAPPDADCReco lappd::LAPPDCircularBufferShifter::ShiftWaveforms(lappd::LAPPDADCReco const& adcReco){

    std::cout << "Shifting waveforms with best shift: " << fBestShift << std::endl;

    // Create a new LAPPDADCReco object to hold the shifted waveforms
    lappd::LAPPDADCReco shiftedADCReco;


    // Loop through all channels in the ADC reco object
    for (int ch = 0; ch < shiftedADCReco.GetNChannels(); ++ch) {
        
        // !!! TO DO: WHY MINUS 1?
        for (unsigned int i = 0; i < lappd::kNTicks; ++i) {
            unsigned int newIndex = (i + fBestShift - 1) % lappd::kNTicks; // Apply the best shift
            shiftedADCReco.SetADCValue(ch, i, adcReco.GetChannel(ch).ADC(newIndex)); // Set the shifted ADC value
        }
    }

    return shiftedADCReco; // Return the shifted ADC reco object
}