////////////////////////////////////////////////////////////////////////
/// \brief   SSDRawDigit class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/SSDRawDigit.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rawdata{
  
  //----------------------------------------------------------------------
  
  SSDRawDigit::SSDRawDigit() :   
    fStation(-999),
    fModule(-999),
    fRow(-999),
    fTime(-999),
    fADC(-999),
    fTrigType(-999)
  {
  }
  
  //----------------------------------------------------------------------
  
  SSDRawDigit::SSDRawDigit(int32_t station, int32_t module, int32_t row,
			   int32_t t, int32_t adc, int32_t trigtype) :
    fStation(station), fModule(module), fRow(row), fTime(t),
    fADC(adc), fTrigType(trigtype)
  {
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
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
