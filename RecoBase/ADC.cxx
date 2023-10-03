////////////////////////////////////////////////////////////////////////
/// \brief   ADC class for functions involving waveforms from V1720s
/// \author  jmirabit@bu.ed
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/ADC.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace rb{

  //----------------------------------------------------------------------
  
  ADC::ADC()
  {
    _v1720index = -999;
    _exptime = -999;
    _time = -999;
    _charge = -999;
    _swcharge = -999;
  }

  //--------------------------------------------------

  ADC::ADC(const emph::rawdata::WaveForm& wvfm, const emph::st::SignalTime& stmap)
  {
    _v1720index = wvfm.Board()*8 + wvfm.Channel();
    _exptime = stmap.SigTime(_v1720index);
    _time = this->CalcTime(wvfm);
    _charge = this->CalcCharge(wvfm);
    _swcharge = this->CalcSWCharge(wvfm);
  
  }

  //--------------------------------------------------
  float ADC::CalcTime(const emph::rawdata::WaveForm& wvfm) const
  {
    //returns time of hit according to steepest slope 
    int time=0;
    int steepest_slope=0;
    for (size_t i=0; i<wvfm.NADC(); ++i){
       if (i==0) continue;
       else {
           int slope = wvfm.ADC(i)-wvfm.ADC(i-1);
           if (slope<steepest_slope){
                steepest_slope=slope;
                time = int(i);
           }
       }
    }
    if (time==0){
        //for (size_t i=0; i<wvfm.NADC(); ++i) std::cout<< wvfm.ADC(i)<<std::endl;
        std::cout<<steepest_slope<<std::endl;
    }
    return 4.*time; //convert to ns
  }

  //--------------------------------------------------
  float ADC::CalcCharge(const emph::rawdata::WaveForm& wvfm) const
  {
    //Fixed Window Integration
    int tmin = _exptime - 15;
    int tmax = _exptime + 30;
    if (tmin<0) tmin=0;
    if (tmax>108) tmax=108;
    float bl;
    if (tmin>10) bl = wvfm.Baseline(0,10); //use first 10 if signal is not here
    else bl = wvfm.Baseline(98,10); //else use last 10

    float sum=0;
    for ( size_t i=tmin; i<size_t(tmax) && i<wvfm.NADC(); ++i) sum += wvfm.ADC(i)-bl;
    //convert to pC
    float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
    return charge;
  }
  //--------------------------------------------------
  float ADC::CalcSWCharge(const emph::rawdata::WaveForm& wvfm) const
  {
    //Sliding Window Integration (for small ADC signals)
    int tmin = _exptime - 15;
    int tmax = _exptime + 30;
    if (tmin<0) tmin=0;
    if (tmax>108) tmax=108;
    float bl;
    if (tmin>10) bl = wvfm.Baseline(0,10); //use first 10 if signal is not here
    else bl = wvfm.Baseline(98,10); //else use last 10

    size_t win_size = 10;
    float sum=0;
    std::vector<float> chargevec;
    for (size_t i=tmin; i<size_t(tmax); ++i){
        if (i+win_size>size_t(tmax)) break; //stop integrating window if it extends beyond window size
        float winsum=0;
        for (size_t j=i; j<size_t(i+win_size); ++j){
            winsum += wvfm.ADC(j)-bl;
        }
        //convert to pC
        winsum = (-1*winsum*2*4e-9*1e12)/(50*4096);
        chargevec.push_back(winsum);
    }
    //Get max value of sliding window integration
    float charge = *std::max_element(chargevec.begin(),chargevec.end());
    return charge;
  }

  //--------------------------------------------------
  //float ADC::CalcBACkovCharge(const emph::rawdata::WaveForm &wvfm, int adcOffset, int nhits, int start, int nsamp, int win_size, float ADC_thresh) const
  //{
  //  //value returned is in pC
  //  int x1=start;
  //  float bl = wvfm.Baseline(adcOffset,nhits);  
  //  int  s = -1;
  //  int  maxVal = -99999;
  //  for ( size_t i=x1; i<size_t(x1+nsamp); ++i) {
  //    if (wvfm.ADC(i)*s > maxVal) {
  //      maxVal = wvfm.ADC(i)*s;
  //    }
  //  }
  //  float sum=0;

  //  if (maxVal>10){
  //    for ( size_t i=x1; i<size_t(x1+nsamp) && i<wvfm.NADC(); ++i){ 
  //      sum += (wvfm.ADC(i)-bl);
  //    }
  //  }
  //  else{
  //    float win_sum = 0;
  //    for (size_t i=x1; i<size_t(x1+20) && i<wvfm.NADC(); ++i){
  //      if (-1*(wvfm.ADC(i)-bl) >= ADC_thresh){
  //    for (size_t j=i-2; j<size_t(i-2+win_size); ++j){
  //          win_sum += (wvfm.ADC(j)-bl);
  //        }
  //        break;
  //  }
  //    }
  //    sum = win_sum;
  //  }
  //  //convert to pC 
  //  float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
  //  return charge;

  //}
} //end namespace rb  
//////////////////////////////////////////////////////////////////////////////
