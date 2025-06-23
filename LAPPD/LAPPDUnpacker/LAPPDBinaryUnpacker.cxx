////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class convert LAPPD raw data
/// \author  fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "LAPPD/LAPPDUnpacker/LAPPDBinaryUnpacker.h"


void lappd::LAPPDBinaryUnpacker::setFileHandle(std::string filename){

	// Open the file in binary mode
	fFileHandle.open(filename, std::ios::in | std::ios::binary);
	if (fFileHandle.is_open()) {
		if (fFileHandle.good()) {
			std::cout << "Opened file: " << filename << std::endl;
		} else {
			std::cerr << "File opened but not good: " << filename << std::endl;
			throw std::runtime_error("File opened but not good");
		}
	} else{
		std::cout << "Could not open file: " << filename << std::endl;
		throw std::runtime_error("Could not open file");
	}
	
	// Get the number of bytes in the file
	fFileHandle.seekg(0, std::ios::end);
	int file_size = fFileHandle.tellg();
	// Reset the file pointer to the beginning
	fFileHandle.seekg(0, std::ios::beg);

	// Sanity check for the file size
	int filesize_64b_words = file_size / sizeof(uint64_t);
	// File size in 64-bit words modulo the expected size of a single event must be zero
	if (filesize_64b_words % (kN64BitDataWords + kN64BitHeaderWords) != 0) {
		std::cerr << "File size does not match expected event size: " 
			<< file_size << " bytes (" << filesize_64b_words << " 64-bit words)."
			<< " Expected size is a multiple of " 
			<< (kN64BitDataWords + kN64BitHeaderWords) << " 64-bit words." << std::endl;
		throw std::runtime_error("File size does not match expected event size");
	}
	else{
		std::cout << "File size matches expected event size: " 
			<< file_size << " bytes (" << filesize_64b_words << " 64-bit words)." << std::endl;
		fNEvents = filesize_64b_words / (kN64BitDataWords + kN64BitHeaderWords);
	}
		
}



lappd::LAPPDRawDigit lappd::LAPPDBinaryUnpacker::readNextEvent() {
	if (!fFileHandle.is_open()) {
		std::cerr << "File is not open. Cannot read next event." << std::endl;
		throw std::runtime_error("File is not open");
	}
	if (fCurrentEventIndex >= fNEvents) {
		std::cerr << "No more events to read. Current event index: " 
			<< fCurrentEventIndex << ", Total events: " << fNEvents << std::endl;
		throw std::out_of_range("No more events to read");
	}

	std::cout << "Current position of the get pointer: " 
		<< fFileHandle.tellg() << " bytes." << std::endl;
	// Read the next event from the file
	uint64_t block[kN64BitTotalWords];
	fFileHandle.read(reinterpret_cast<char*>(block), sizeof(block));
	lappd::LAPPDRawDigit rawDigit(block);
	if (!fFileHandle) {
		std::cerr << "Error reading from file at event index: " 
			<< fCurrentEventIndex << std::endl;
		throw std::runtime_error("Error reading from file");	
	}
	fCurrentEventIndex++;

	return rawDigit;

}

