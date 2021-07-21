////////////////////////////////////////////////////////////////////////
/// \brief   RawDigit class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/RawDigit.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rawdata{
  
  //----------------------------------------------------------------------
  
  RawDigit::RawDigit() :   
    fADC(0),
    fTDC(1, 0),
    fVersion(0),  
    fChannel(0),
    fIsMC(false)
  {
  }
  
  //----------------------------------------------------------------------
  
  RawDigit::RawDigit(uint32_t channel,
		     std::vector<int16_t> adclist,
		     int32_t tdc) :
    fADC(adclist), 
    fTDC(tdc),
    fVersion(0),  
    fChannel(channel),
    fIsMC(false)
  {
  }
  
  //----------------------------------------------------------------------
  
  //--------------------------------------------------
  int16_t RawDigit::ADC(uint32_t i) const
  {
    if(i > fADC.size()){
      printf("RawDigit::ADC(%d) out of range!\n", i);
    }
    return fADC[i];
  }

  //--------------------------------------------------
  
  void RawDigit::SetADC(uint32_t i, int16_t iADC) {
    if (fADC.size()<i+1) fADC.resize(i+1);
    fADC[i]=iADC;
    return;
  }
  
  //--------------------------------------------------
  
  void RawDigit::SetTDC(int32_t iTDC) {
    assert(fTDC.size() == 1);
    fTDC[0] = iTDC;
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const RawDigit& r)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Channel = "     << std::setw(5) << std::right << r.Channel()
      << " Version = "     << std::setw(5) << std::right << (int)r.Version()
      << " NADC = "        << std::setw(5) << std::right << r.NADC();     
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
