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
#include <cstddef>
#include <algorithm>

namespace emph{
namespace adcu{

  //----------------------------------------------------------------------
  ADCUtils::ADCUtils() {
    // constructor implementation
  }
  //----------------------------------------------------------------------
  
  ADCUtils::ADCUtils(const emph::rawdata::WaveForm& wvfm, const emph::st::SignalTime& stmap, const int& tlow, const int& thigh, const int& slopeMin)
  {
    _v1720index = wvfm.Board()*8 + wvfm.Channel();
    _exptime = stmap.SigTime(_v1720index);

    // Locate the hit on the waveform when asked to.  This has to happen before
    // the baseline, because CalcBaseline picks its signal-free region from the
    // hit sample; CalcTimeSample itself needs neither the baseline nor the map.
    // A channel with no qualifying falling edge keeps the map window, so a
    // record with no signal integrates exactly the pedestal it always did.
    int ts = (slopeMin > 0) ? this->CalcTimeSample(wvfm, slopeMin) : -1;
    _selftimed = (ts >= 0);
    _hittime = _selftimed ? ts : _exptime;

    _baseline = this->CalcBaseline(wvfm);
    _time = this->CalcTime(wvfm);
    _charge = this->CalcCharge(wvfm, tlow, thigh);
  }

  //----------------------------------------------------------------------
  float ADCUtils::CalcBaseline(const emph::rawdata::WaveForm& wvfm) const
  {
      int tmin = _hittime - 15;
      int tmax = _hittime + 30;
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
  int ADCUtils::CalcTimeSample(const emph::rawdata::WaveForm& wvfm, const int& slopeMin) const
  {
      //sample of the steepest slope, requiring it to be at least slopeMin deep.
      //slopeMin<=0 reproduces the original "any falling sample counts".
      int timemax = this->CalcTimeMax(wvfm);
      int time=-1;
      int steepest_slope = (slopeMin>0) ? -slopeMin+1 : 0;
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
      return time;
  }

  //......................................................................
  float ADCUtils::CalcTime(const emph::rawdata::WaveForm& wvfm) const
  {
      //returns time of hit according to steepest slope
      int time = this->CalcTimeSample(wvfm,0);
      if (time<0) time=0;
      return 4.*time; //convert to ns
  }

  //......................................................................
  float ADCUtils::CalcCharge(const emph::rawdata::WaveForm& wvfm, const int& tlow, const int& thigh) const
  {
      //Fixed Window Integration
      int tmin = _hittime - tlow;
      int tmax = _hittime + thigh;
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
