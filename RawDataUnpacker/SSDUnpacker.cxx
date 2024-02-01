////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class with static functions to convert SSD raw data to
///          slightly more useful raw data products
/// \author  jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "RawDataUnpacker/SSDUnpacker.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include <bitset>
#include <time.h>

namespace emph {
  namespace rawdata {

    std::pair<uint64_t, std::vector<emph::rawdata::SSDRawDigit>> SSDUnpack::readSSDHitsFromFileStream(std::ifstream& file_handle, bool first) {
      // TODO sanity checks for input file
      
      const uint64_t kOnes = -1;
      const uint32_t kMaxHits = 500;
      const size_t kDataSize = sizeof(unsigned long long);
      uint64_t rawdata_buffer;
      uint64_t bco;
      
      std::vector<emph::rawdata::SSDRawDigit> ssd_hits;
      ssd_hits.reserve(kMaxHits);
      bool start_flag = true;
      bool isFirst = first;
      
      while (!file_handle.eof()) {
        file_handle.read((char*)(&rawdata_buffer), kDataSize);	
        if (rawdata_buffer == kOnes) { // mark the start of a new event
	  if (isFirst) {
	    // at start of file, skip this first marker
	    isFirst = false;
	    continue;
	  }
	  // got to end of the event, so we're done
	  break;
	}
	if (start_flag) {
	  // at the start of an event, so this is the event number
	  // read the bco clock time at the start of each event
	  // we call an extra read here since the first data block of the event is unused
	  start_flag = false;
	  file_handle.read((char*)(&bco), kDataSize);
	  continue;
	}
	else {
	  // extra next SSD raw hit info
	  uint64_t rawdata_tmp = 0;
	  uint64_t rawdata_ordered = 0;
	  for (size_t byte = 0; byte < kDataSize; byte++) {
	    rawdata_tmp = ((rawdata_buffer >> ((kDataSize - byte - 1) * 8)) & 0xff) << (byte * 8);
	    rawdata_ordered += rawdata_tmp;
	  }
	  rawdata_buffer = rawdata_ordered;
	  uint32_t trig_type = (rawdata_buffer >> 32) & 0xfffff;
	  uint32_t module = (rawdata_buffer >> 27) & 0x07;
	  uint32_t chip = (rawdata_buffer >> 24) & 0x07;
	  uint32_t set = (rawdata_buffer >> 12) & 0x1f;
	  uint32_t strip = (rawdata_buffer >> 17) & 0x0f;
	  uint32_t t = (rawdata_buffer >> 4) & 0xff;
	  uint32_t adc = (rawdata_buffer >> 1) & 0x7;
	  uint32_t station = (rawdata_buffer >> 56) & 0xf;
	  
	  rawdata::SSDRawDigit hit(station, module, chip, set, strip, t, adc, trig_type);
	  ssd_hits.push_back(hit);
	}
      }
      return std::make_pair(bco, ssd_hits);
    }    
  } // end namespace rawdata
} // end namespace emph
