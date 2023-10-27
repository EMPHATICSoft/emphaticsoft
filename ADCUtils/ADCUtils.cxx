////////////////////////////////////////////////////////////////////////
/// \brief   ADC class for functions involving waveforms from V1720s
/// \author  jmirabit@bu.ed
/// \date
////////////////////////////////////////////////////////////////////////
#include "ADCUtils/ADCUtils.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>

namespace emph{
namespace adcu{

  //----------------------------------------------------------------------
  
  ADCUtils::ADCUtils(const emph::rawdata::WaveForm& wvfm, const emph::st::SignalTime& stmap)
  {
    _v1720index = wvfm.Board()*8 + wvfm.Channel();
    _exptime = stmap.SigTime(_v1720index);
    _baseline = this->CalcBaseline(wvfm);
    _time = this->CalcTime(wvfm);
    _charge = this->CalcCharge(wvfm);
  }

  //----------------------------------------------------------------------
  float ADCUtils::CalcBaseline(const emph::rawdata::WaveForm& wvfm) const
  {
      int tmin = _exptime - 15;
      int tmax = _exptime + 30;
      if (tmin<0) tmin=0;
      if (tmax>108) tmax=108;

      float bl;
      if (tmin>20) bl = wvfm.Baseline(0,20); //use first 20 if signal is not here
      else if (tmax<88) bl = wvfm.Baseline(88,20); //use last 20 if signal is not here
      else bl = (wvfm.Baseline(0,10) + wvfm.Baseline (98,10))/2.; //use first 10 and last 10 if necessary

      return bl;
  }

  //......................................................................
  int ADCUtils::CalcTimeMax(const emph::rawdata::WaveForm& wvfm) const
  {
      //Calculate time of max ADC
      int maxVal = -99999;
      int s = -1;
      int tm = 0;
      for (size_t i=0; i<wvfm.NADC(); ++i){
          if (wvfm.ADC(i)*s>maxVal) {
              maxVal = wvfm.ADC(i)*s;
              tm = int(i);
          }
      }
      return tm;
  }
  //......................................................................
  float ADCUtils::CalcTime(const emph::rawdata::WaveForm& wvfm) const
  {
      //returns time of hit according to steepest slope
      int timemax = this->CalcTimeMax(wvfm);
      int time=0;
      int steepest_slope=0;
      for (size_t i=0; i<wvfm.NADC(); ++i){
          if (i==0) continue;
          else {
              int slope = wvfm.ADC(i)-wvfm.ADC(i-1);
              //Only record time if it is steepest slope and at most 5 ticks before  maximum ADC
              if (slope<steepest_slope && timemax-i<=5 && timemax-i>=0){
                  steepest_slope=slope;
                  time = int(i);
              }
          }
      }
      return 4.*time; //convert to ns
  }

  //......................................................................
  float ADCUtils::CalcCharge(const emph::rawdata::WaveForm& wvfm) const
  {
      //Fixed Window Integration
      int tmin = _exptime - 15;
      int tmax = _exptime + 30;
      if (tmin<0) tmin=0;
      if (tmax>108) tmax=108;

      float sum=0;
      for ( size_t i=tmin; i<size_t(tmax) && i<wvfm.NADC(); ++i) sum += wvfm.ADC(i)-_baseline;
      //convert to pC
      float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
      return charge;
  }
} //end namespace adcu
} //end namespace emph



  //////////////////////////////////////////////////////////////////////////////
