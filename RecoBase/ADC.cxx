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
  
  ADC::ADC() : emph::rawdata::WaveForm()
  {
    //emph::rawdata::WaveForm* wvfm = new emph::rawdata::WaveForm(); //Create Waveform object from RawData 
    //recowvfm = 0;
  }

  //----------------------------------------------------------------------
  
  //ADC::ADC(emph::rawdata::WaveForm wvfm) 
  //{
  //  recowvfm = wvfm
  //}
  

  //--------------------------------------------------
  float ADC::Charge(int adcOffset, int nhits, int start, int nsamp) const
  {
    int x1=start;
    float bl = this->Baseline(adcOffset,nhits);
    float sum=0;
    for ( size_t i=x1; i<size_t(x1+nsamp) && i<fADC.size(); ++i) sum += (fADC[i]-bl);
    //convert to pC
    float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
    return charge;
  }

  //--------------------------------------------------
  float ADC::BACkovCharge(int adcOffset, int nhits, int start, int nsamp, int win_size, float ADC_thresh) const
  {
    //value returned is in pC
    int x1=start;
    float bl = this->Baseline(adcOffset,nhits);  
    int  s = -1;
    int  maxVal = -99999;
    for ( size_t i=x1; i<size_t(x1+nsamp); ++i) {
      if (fADC[i]*s > maxVal) {
        maxVal = fADC[i]*s;
      }
    }
    float sum=0;

    if (maxVal>10){
      for ( size_t i=x1; i<size_t(x1+nsamp) && i<fADC.size(); ++i){ 
        sum += (fADC[i]-bl);
      }
    }
    else{
      float win_sum = 0;
      for (size_t i=x1; i<size_t(x1+20) && i<fADC.size(); ++i){
        if (-1*(fADC[i]-bl) >= ADC_thresh){
	  for (size_t j=i-2; j<size_t(i-2+win_size); ++j){
            win_sum += (fADC[j]-bl);
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
