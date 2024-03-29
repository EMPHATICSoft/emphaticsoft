////////////////////////////////////////////////////////////////////////
/// \brief   SSDRawDigit class
/// \author  jpaley@fnal.gov
/// \date
/// \note:   *** Note that "strip" in the raw digit class is not the same
///              as "strip" in the geometry.  The corresponding variable
///              to the geometry strip is "row".
////////////////////////////////////////////////////////////////////////
#include "RawData/SSDRawDigit.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <fstream>

namespace emph {

  namespace rawdata{
  
  //----------------------------------------------------------------------
  
  SSDRawDigit::SSDRawDigit() :   
    fFER(-999),
    fModule(-999),
    fChip(-999),
    fSet(-999),
    fTime(-999),
    fADC(-999),
    fTrigNum(-999),
    fRow(-999),
    fBco2s(144)
  {
  }
  
  //----------------------------------------------------------------------
  
  rawdata::SSDRawDigit::SSDRawDigit(int32_t station, int32_t module, int32_t chip, int32_t set, int32_t strip, int32_t t, int32_t adc, int32_t trigtype) :
    fFER(station), fModule(module), fChip(chip), fSet(set), fStrip(strip), fTime(t), fADC(adc), fTrigNum(trigtype), fBco2s(144)
  {
    fRow = getSensorRow();
  }
  
  //----------------------------------------------------------------------
  
  rawdata::SSDRawDigit::SSDRawDigit(int32_t station, int32_t module, int32_t row, int32_t t, int32_t adc, int32_t trigtype) :
    fFER(station), fModule(module), fChip(-999), fSet(-999), fStrip(-999), fTime(t), fADC(adc), fTrigNum(trigtype), fRow(row), fBco2s(144)
  {

  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const SSDRawDigit& r)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " FER = "     << std::setw(5) << std::right << r.FER()
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
  
} // end namespace rawdata
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
