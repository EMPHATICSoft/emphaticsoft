////////////////////////////////////////////////////////////////////////
/// \brief   SSDRawDigit class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/SSDRawDigit.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <fstream>

namespace rawdata{
  
  //----------------------------------------------------------------------
  
  SSDRawDigit::SSDRawDigit() :   
    fStation(-999),
    fModule(-999),
    fChip(-999),
    fSet(-999),
    fTime(-999),
    fADC(-999),
    fTrigType(-999),
    fRow(-999)
  {
  }
  
  //----------------------------------------------------------------------
  
  rawdata::SSDRawDigit::SSDRawDigit(int32_t station, int32_t module, int32_t chip, int32_t set, int32_t strip, int32_t t, int32_t adc, int32_t trigtype) :
    fStation(station), fModule(module), fChip(chip), fSet(set), fStrip(strip), fTime(t),
    fADC(adc), fTrigType(trigtype)
  {
      fRow = getSensorRow();
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const SSDRawDigit& r)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Station = "     << std::setw(5) << std::right << r.Station()
      << " Module  = "     << std::setw(5) << std::right << r.Module()
      << " Row     = "        << std::setw(5) << std::right << r.Row();
    return o;
  }


  uint32_t rawdata::SSDRawDigit::getSensorRow(int chip, int set, int strip) {
      // TODO bounds checking
      if (chip < 0 || set < 0 || strip < 0 || set >= 32 || strip >= 16) {
          // throw an error...
      }

      static const unsigned char set_number[] = {
          255, 255, 255, 255, 255, 255, 255, 255,
          255, 255,   0,   1,   4,   5,   3,   2,
          255, 255,  12,  13,   8,   9,  11,  10,
          255, 255,  15,  14,   7,   6, 255, 255
      };
      static const unsigned char strip_number[] = {
          255, 255, 255, 255,
          255,   0,   2,   1,
          255,   6,   4,   5,
          255,   7,   3, 255
      };

      return 128 * (chip - 1) + (set_number[set] * 8) + strip_number[strip];
  }
  

  uint32_t rawdata::SSDRawDigit::getSensorRow() const {
      return getSensorRow(fChip, fSet, fStrip);
  }

  std::pair<uint64_t, std::vector<SSDRawDigit>> readSSDHitsFromFileStream(std::ifstream& file_handle) {
    // TODO sanity checks for input file

    const uint64_t kOnes = -1;
    const uint32_t kMaxHits = 500;
    const size_t kDataSize = sizeof(unsigned long long);
    uint64_t rawdata_buffer;
    uint64_t bco;

    std::vector<rawdata::SSDRawDigit> ssd_hits;
    ssd_hits.reserve(kMaxHits);
    bool start_flag = false;

    while (!file_handle.eof()) {
        file_handle.read((char*)(&rawdata_buffer), kDataSize);
        if (rawdata_buffer == kOnes) {
            if (start_flag) {
                // event reading was previously started, now got end marker
                // return as soon as this event is fully read
                start_flag = false;
                break;
            }
            else  {
                // maybe got an extra event separator?
                // TODO better logging
                std::cout << "Warning: Got extra event marker. Check file?\n";
                continue;
            }
        }
        else {
            // event data
            if (!start_flag) {
                start_flag = true;
                // read the bco clock time at the start of each event
                // we call an extra read here since the first data block of the event is unused
                file_handle.read((char*)(&bco), kDataSize);
                continue;
            }
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
//////////////////////////////////////////////////////////////////////////////
