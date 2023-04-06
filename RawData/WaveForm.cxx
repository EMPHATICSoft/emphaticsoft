////////////////////////////////////////////////////////////////////////
/// \brief   RawDigit class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/WaveForm.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>

namespace emph {
namespace rawdata{
  
  //----------------------------------------------------------------------
  
  WaveForm::WaveForm() :   
    fADC(0),
    fTstart(-1),
    fChannel(-1),
    fBoard(-1),
    fDetChannel(-1),
    fIsMC(false),
    fFragmentTimestamp(0)
  {
  }
  
  //----------------------------------------------------------------------
  
  WaveForm::WaveForm(int channel, int board,
		     std::vector<uint16_t> adclist,
		     uint32_t tdc, uint64_t fragTS) :
    fADC(adclist), fTstart(tdc), fChannel(channel), fBoard(board),
    fDetChannel(-1),
    fIsMC(false), fFragmentTimestamp(fragTS)
  {
  }
  
  //--------------------------------------------------
  uint16_t WaveForm::ADC(size_t i) const
  {
    if(i > fADC.size()){
      std::cerr << "WaveForm::ADC(" << i << ") out of range!" << std::endl;
      return 0;
    }
    return fADC[i];
  }

  //--------------------------------------------------

  float WaveForm::Baseline(int adcOffset, int nhits) const
  {
    float avg = 0;
    int ic = 0;
    for ( size_t i=adcOffset; i<size_t(adcOffset+nhits) && i<fADC.size();
	  ++i, ++ic) {
      avg += fADC[i];
    }
    avg /= float(ic);

    return avg;
    
  }

  //--------------------------------------------------

  float WaveForm::BLWidth(int adcOffset, int nhits) const
  {
    float bl = this->Baseline(adcOffset,nhits);
    float rms = 0;
    int ic = 0;

    for ( size_t i=adcOffset; i<size_t(adcOffset+nhits) && i<fADC.size();
	  ++i, ++ic) {
      rms += (fADC[i]-bl)*(fADC[i]-bl);
    }
    rms /= float(ic);

    return sqrt(rms);
  }

  //--------------------------------------------------
  int WaveForm::PeakADC(bool isNegative) const
  {
    int s = 1;
    int maxVal = 0;
    if (isNegative) {
      s = -1;
      maxVal = -99999;
    }
    for ( size_t i=0; i<fADC.size(); ++i) {
      if (fADC[i]*s > maxVal) {
	maxVal = fADC[i]*s;
      }
    }
    return maxVal*s;
  }

  //--------------------------------------------------
  int WaveForm::PeakTDC(bool isNegative) const
  {
    int s = 1;
    int maxVal = 0;
    size_t peakPos=0;
    if (isNegative) {
      s = -1;
      maxVal = -99999;
    }
    for ( size_t i=0; i<fADC.size(); ++i) {
      if (fADC[i]*s > maxVal) {
	maxVal = fADC[i]*s;
	peakPos = i;
      }
    }
    return (int)peakPos;
  }

  //--------------------------------------------------
  int WaveForm::PeakWidth(bool ) const // isNegative)
  {
    return 0;
  }

  //--------------------------------------------------
  float WaveForm::IntegratedADC(int x1, int nsamp) const
  {
    float sum=0;
    for ( size_t i=x1; i<size_t(x1+nsamp) && i<fADC.size(); ++i) sum += fADC[i];
    return sum;
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const WaveForm& r)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Channel = "     << std::setw(5) << std::right << r.Channel()
      << " Board = "     << std::setw(5) << std::right << (int)r.Board()
      << " NADC = "        << std::setw(5) << std::right << r.NADC();     
    return o;
  }
  
} // end namespace rawdata
} // end namespace emph
  
//////////////////////////////////////////////////////////////////////////////
