////////////////////////////////////////////////////////////////////////
/// \brief   ADC class for functions involving waveforms from V1720s
/// \author  jmirabit@bu.ed
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/ADC.h"
#include "../RawData/WaveForm.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>

namespace rb{

  //----------------------------------------------------------------------
  
  ADC::ADC()
  {
  }

  //--------------------------------------------------
  float ADC::Charge(const emph::rawdata::WaveForm* wvfm, int adcOffset, int nhits, int start, int nsamp) const
  {
    int x1=start;
    float bl = wvfm->Baseline(adcOffset,nhits);
    float sum=0;
    for ( size_t i=x1; i<size_t(x1+nsamp) && i<wvfm->NADC(); ++i) sum += (wvfm->ADC(i)-bl);
    //convert to pC
    float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
    return charge;
  }

  //--------------------------------------------------
  float ADC::BACkovCharge(const emph::rawdata::WaveForm* wvfm, int adcOffset, int nhits, int start, int nsamp, int win_size, float ADC_thresh) const
  {
    //value returned is in pC
    int x1=start;
    float bl = wvfm->Baseline(adcOffset,nhits);  
    int  s = -1;
    int  maxVal = -99999;
    for ( size_t i=x1; i<size_t(x1+nsamp); ++i) {
      if (wvfm->ADC(i)*s > maxVal) {
        maxVal = wvfm->ADC(i)*s;
      }
    }
    float sum=0;

    if (maxVal>10){
      for ( size_t i=x1; i<size_t(x1+nsamp) && i<wvfm->NADC(); ++i){ 
        sum += (wvfm->ADC(i)-bl);
      }
    }
    else{
      float win_sum = 0;
      for (size_t i=x1; i<size_t(x1+20) && i<wvfm->NADC(); ++i){
        if (-1*(wvfm->ADC(i)-bl) >= ADC_thresh){
	  for (size_t j=i-2; j<size_t(i-2+win_size); ++j){
            win_sum += (wvfm->ADC(j)-bl);
          }
          break;
	}
      }
      sum = win_sum;
    }
    //convert to pC 
    float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
    return charge;

  }
} //end namespace rb  
//////////////////////////////////////////////////////////////////////////////
