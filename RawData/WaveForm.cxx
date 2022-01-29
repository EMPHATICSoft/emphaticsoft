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
    fBaseline(-99999),
    fBLWidth(-1),
    fPeakPos(1),
    fIsNeg(true),
    fIsMC(false),
    fFragmentTimestamp(0)
  {
  }
  
  //----------------------------------------------------------------------
  
  WaveForm::WaveForm(int channel, int board,
		     std::vector<uint16_t> adclist,
		     uint32_t tdc, uint64_t fragTS) :
    fADC(adclist), fTstart(tdc), fChannel(channel), fBoard(board),
    fIsNeg(true), fIsMC(false), fFragmentTimestamp(fragTS)
  {
    fBaseline = -99999;
    fBLWidth = -1;
    fPeakPos = adclist.size();    
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

  int WaveForm::Baseline(int adcOffset, int nhits)
  {
    if (fBaseline == -99999) { // baseline and width haven't been calculated yet
      float avg = 0;
      float rms = 0;
      int ic = 0;
      for ( size_t i=adcOffset; i<size_t(adcOffset+nhits) && i<fADC.size();
	    ++i, ++ic) {
	avg += fADC[i];
      }
      avg /= float(ic);
      for ( size_t i=adcOffset; i<size_t(adcOffset+nhits) && i<fADC.size(); ++i) {
	rms += (fADC[i]-avg)*(fADC[i]-avg);
      }
      rms /= float(ic);
      rms = sqrt(rms);

      fBaseline = (int)avg;
      fBLWidth = (int)rms;
    }
    
    return fBaseline;
  }

  //--------------------------------------------------

  int WaveForm::BLWidth(int adcOffset, int nhits)
  {
    if (fBaseline < 0) {  // baseline and width haven't been calculated yet
      this->Baseline(adcOffset, nhits);      
    }
    
    return fBaseline;
  }

  //--------------------------------------------------
  int WaveForm::PeakADC() 
  {
    return 0;
  }

  //--------------------------------------------------
  int WaveForm::PeakTDC() 
  {
    return 0;
  }

  //--------------------------------------------------
  int WaveForm::PeakWidth() 
  {
    return 0;
  }

  //--------------------------------------------------
  int WaveForm::IntegratedADC() 
  {
    return 0;
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
