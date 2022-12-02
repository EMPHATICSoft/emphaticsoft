////////////////////////////////////////////////////////////////////////
/// \brief   A short block characterizing the trigger waveform and the T0 
///          data, to pre-select single track events is a possibly noisy 
///          environment 
///          It is intended to be used prior to tracking 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/TrigToT0.h"

#include <iomanip>
#include <iostream>
#include <cmath>
#include <climits>

namespace rb {
  
  //----------------------------------------------------------------------
  
  TrigToT0::TrigToT0(int nPeakTrig, double sumPMTTrig, int numSegT0, 
                      int segT0, double sumSigUpT0, double sumSigDownT0) :
    fnPeakTrig(nPeakTrig), 
    fsumPMTTrig(sumPMTTrig),
    fnumSegT0(numSegT0),
    fsegT0(segT0),
    fsumSigUpT0(sumSigUpT0),
    fsumSigDownT0(sumSigDownT0)
 {
 ;
  }
  TrigToT0::TrigToT0() :
    fnPeakTrig(0), 
    fsumPMTTrig(0.),
    fnumSegT0(0),
    fsegT0(INT_MAX),
    fsumSigUpT0(0.),
    fsumSigDownT0(0.)
 {
 ;
  }
  
  //------------------------------------------------------------
  bool TrigToT0::isPrettyGood(double trigAmplCut[2], double T0AmplCut[2]) 
  {
    if (fnPeakTrig != 4) return false;
    if (fnumSegT0 != 1) return false;
    if (fsumPMTTrig < trigAmplCut[0]) return false;
    if (fsumPMTTrig > trigAmplCut[1]) return false;
    double fSumT0 = 0.5*(fsumSigUpT0 + fsumSigDownT0);
    if (fSumT0 < T0AmplCut[0]) return false;
    if (fSumT0 > T0AmplCut[1]) return false;
    return true;
    
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const rb::TrigToT0& h)
  {
    o << "Number of Peak found in trigger counter " << h.NPeakTrig() << ", To Unipolar pulses  " <<h.NumSegT0() << std::endl;
    o << " ... Amplitude Trigger, 1rst peak " << h.SumPMTTrig() <<  std::endl;
    o << " ... Segment number "<< h.SegT0() << " Amplitude, Up " << h.SumSigUpT0() << " Down " << h.SumSigDownT0() <<std::endl;

    return o;
  }
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
