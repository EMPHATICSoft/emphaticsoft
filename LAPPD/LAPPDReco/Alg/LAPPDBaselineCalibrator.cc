///////////////////////////////////////////////////////////////////////
// Class:       LAPPDBaselineCalibrator
// Author:      fnicolas@fnal.gov
// Description: Source code for LAPPDBaselineCalibrator
////////////////////////////////////////////////////////////////////////

#include "LAPPD/LAPPDReco/Alg/LAPPDBaselineCalibrator.hh"

// ------ Constructor ------
lappd::LAPPDBaselineCalibrator::LAPPDBaselineCalibrator( std::string baselineFile )
    : fBaselineFile( baselineFile )
{

    // Open the baseline file
    std::ifstream file( fBaselineFile );
    if ( !file.is_open() ) {
        std::cerr << "Error: Could not open baseline file: " << fBaselineFile << std::endl;
        return;
    }
    std::cout << "LAPPDBaselineCalibrator: Reading baseline values from file: " << fBaselineFile << std::endl;

    // Fill in the baseline values
    // Format per line: channel adc1 adc2 adc3 ... adcNTicks
    int currentChannel = -1;
    while ( !file.eof() ) {
        std::string line;
        std::getline( file, line );
    
        currentChannel++;

        // Parse the line
        std::istringstream iss( line );
        int channel;
        iss >> channel;

        // Check if the channel number matches
        if ( channel != currentChannel ) {
            std::cerr << "Error: Channel number mismatch in baseline file. Expected channel " 
                      << currentChannel << ", but found channel " << channel << std::endl;
            continue;
        }

        // Read the baseline values for the channel
        for ( int i = 0; i < lappd::kNTicks; ++i ) {
            if ( !(iss >> fBaselineValues[currentChannel][i]) ) {
                std::cerr << "Error: Not enough baseline values for channel " << currentChannel 
                          << " in line: " << line << std::endl;
                break;
            }
        }

    }

    // Check we read exactly kNChannels channels
    if ( currentChannel + 1 != lappd::kNChannels ) {
        std::cerr << "Error: Expected " << lappd::kNChannels << " channels, but read " 
                  << currentChannel + 1 << " channels from the baseline file." << std::endl;
    } else {
        std::cout << "LAPPDBaselineCalibrator: Successfully read baseline values for all channels." << std::endl;
    }

    // Close the file
    file.close();
    std::cout << "LAPPDBaselineCalibrator: Finished reading baseline values." << std::endl;

    for ( unsigned int ch = 0; ch < lappd::kNChannels; ++ch ) {
        std::cout << "Channel " << ch << " baseline values: ";
        for ( unsigned int bin = 0; bin < lappd::kNTicks; ++bin ) {
            std::cout << fBaselineValues[ch][bin] << " ";
        }
        std::cout << std::endl;
    }
}


// ------ GetBaseline for a specific channel ------
std::vector<float> lappd::LAPPDBaselineCalibrator::GetBaseline( unsigned int channel ) const
{
    if ( channel >= lappd::kNChannels ) {
        std::cerr << "Error: Invalid channel number " << channel << ". Must be less than " 
                  << lappd::kNChannels << "." << std::endl;
        return {};
    }
    return std::vector<float>( fBaselineValues[channel], fBaselineValues[channel] + lappd::kNTicks );
}

// ------ GetBaseline for a specific channel and bin ------
float lappd::LAPPDBaselineCalibrator::GetBaseline( unsigned int channel, unsigned int bin ) const
{
    if ( channel >= lappd::kNChannels ) {
        std::cerr << "Error: Invalid channel number " << channel << ". Must be less than " 
                  << lappd::kNChannels << "." << std::endl;
        return std::numeric_limits<float>::quiet_NaN();
    }
    if ( bin >= lappd::kNTicks ) {
        std::cerr << "Error: Invalid bin number " << bin << ". Must be less than " 
                  << lappd::kNTicks << "." << std::endl;
        return std::numeric_limits<float>::quiet_NaN();
    }
    return fBaselineValues[channel][bin];
}

// ------ Calibrate a raw digit by subtracting the baseline ------
lappd::LAPPDADCReco lappd::LAPPDBaselineCalibrator::Calibrate( const lappd::LAPPDRawDigit& rawDigit ) const
{
    lappd::LAPPDADCReco adcReco( rawDigit );

    // Subtract the baseline from each bin
    for ( unsigned int ch = 0; ch < lappd::kNChannels; ++ch ) {
        for ( unsigned int bin = 0; bin < lappd::kNTicks; ++bin ) {
            adcReco.SetADCValue( ch, bin, adcReco.GetChannel(ch).ADC(bin) - GetBaseline(ch, bin) );
        }
    }

    return adcReco;
}

// ------ Calibrate an ADC waveform by subtracting the baseline ------
lappd::LAPPDADCReco lappd::LAPPDBaselineCalibrator::Calibrate( const lappd::LAPPDADCReco& adcReco ) const
{
    lappd::LAPPDADCReco calibratedADC( adcReco );

    // Subtract the baseline from each bin
    for ( unsigned int ch = 0; ch < lappd::kNChannels; ++ch ) {
        for ( unsigned int bin = 0; bin < lappd::kNTicks; ++bin ) {
            calibratedADC.SetADCValue( ch, bin, adcReco.GetChannel(ch).ADC(bin) - GetBaseline(ch, bin) );
        }
    }

    return calibratedADC;
}


