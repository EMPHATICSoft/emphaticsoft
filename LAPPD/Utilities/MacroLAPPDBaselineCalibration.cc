
////////////////////////////////////////////////////////////////////////
/// \brief   Macro to get the baseline calibration from a LAPPD binary file
/// \author  fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>

// LAPPD unpacker
#include "LAPPD/LAPPDUnpacker/LAPPDBinaryUnpacker.h"
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPD/LAPPDObj/LAPPD_format.h"

int main(int argc,char** argv) {

  // Get arguments - minimum 1 argument expected (input binary file)
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " -s <filename>" << std::endl;
    return 1;
  }

  // Parse command line arguments
  std::string inputFileName;
  std::string outputFileName = "baseline_calibration.txt"; // Default output file name
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "-s" && i + 1 < argc) {
      inputFileName = argv[++i];
    }
    else if (std::string(argv[i]) == "-o" && i + 1 < argc) {
      outputFileName = argv[++i];
    }
    else {
      std::cerr << "Unknown argument: " << argv[i] << std::endl;
      return 1;
    }
  }
  std::cout << "Processing input file: " << inputFileName << std::endl;

  // Create an instance of the LAPPDBinaryUnpacker
  lappd::LAPPDBinaryUnpacker unpacker;
  unpacker.setFileHandle(inputFileName);
  int nEvents = unpacker.getNEvents();
  if (nEvents <= 0) {
    std::cerr << "No events found in the input file: " << inputFileName << std::endl;
    return 1;
  }
  else std::cout << "Number of events in the file: " << nEvents << std::endl;

  // Baseline calibration is per channel and per ADC bin
  // Need array 30x256 to make the calibration (mean)
  std::vector<std::vector<float>> baselineCalibration(lappd::kNChannels, std::vector<float>(lappd::kNTicks, 0.));
  
  // Loop over events
  for (int i = 0; i < nEvents; ++i) {

    // Get the LAPPD raw digit for the current event
    lappd::LAPPDRawDigit lappdRawDigit = unpacker.readNextEvent();

    // Loop over channels in the raw digit
    for (int channelIndex = 0; channelIndex < lappdRawDigit.GetNChannels(); ++channelIndex) {
      lappd::LAPPDReadoutChannelRawDigit channel = lappdRawDigit.GetChannel(channelIndex);
      
      // Loop over ADC bins in the channel
      for (int adcBin = 0; adcBin < channel.NADCs(); ++adcBin) {
        // Accumulate the ADC values for baseline calibration
        baselineCalibration[channelIndex][adcBin] += channel.ADC(adcBin);
      }
    }
  }

  // Open the output file for writing
  std::ofstream outputFile(outputFileName);
  if (!outputFile.is_open()) {
    std::cerr << "Error opening output file for writing." << std::endl;
    return 1;
  }

  // Format the output:
  // each line contains channel index, followed by 256 ADC values separated by spaces
  for (int channelIndex = 0; channelIndex < lappd::kNChannels; ++channelIndex) {

    // Output the channel index
    outputFile << channelIndex << " ";

    // Output the baseline calibration values for each ADC bin
    for (int adcBin = 0; adcBin < 256; ++adcBin) {
      outputFile << baselineCalibration[channelIndex][adcBin] / nEvents << " ";
    }

    // End the line for this channel if not the last channel
    if (channelIndex < 29)
      outputFile << std::endl;
  }

  // Close the output file
  outputFile.close();
  std::cout << "Baseline calibration written to: " << outputFileName << std::endl;
  return 0;

}