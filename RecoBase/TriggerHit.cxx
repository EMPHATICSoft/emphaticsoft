////////////////////////////////////////////////////////////////////////
/// \brief   TriggerHit
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <iomanip>

#include "RecoBase/TriggerHit.h"

  namespace rb {
    TriggerHit::TriggerHit(double sum4PMTs, const std::vector<int> &numPeaks, const std::vector<double> &deltaTs):
    _cutSum4PMTSLow(7000.), // Tune for Phase1b, to be reivewed and adjusted for Phase1a .. 
    _cutSum4PMTSHigh(37000.),
    _isSingleParticle(true), 
    _sum4PMTs(sum4PMTs),
    _numPeaks(numPeaks),
    _deltaTs(numPeaks.size(), DBL_MAX) 
    {
     if (_sum4PMTs < _cutSum4PMTSLow) _isSingleParticle = false;
     if (_sum4PMTs > _cutSum4PMTSHigh) _isSingleParticle = false;
     int numSuspicious = 0;
     
     for (size_t k=0; k !=  _numPeaks.size(); k++) {
       if (_numPeaks[k] != 1) numSuspicious++;
       else if (_numPeaks[k] == 1) _deltaTs[k] = deltaTs[k];
     }
     if (numSuspicious >= 3) _isSingleParticle = false;
    } 
    std::ostream& operator<< (std::ostream& o, const  TriggerHit& r)
       {
        auto prevFlags = o.flags(); auto prevPrecision = o.precision();
        o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        o << " Dump from emph::rb::TriggerHit " << std::endl;
        o <<  " ...  Sum4 PMTs signals  " << r.Sum4PMTs() << " Number of Peaks for PMT0 " << r.NumberOfPeaks(0) 
	  << " PMT 1 " << r.NumberOfPeaks(0) << " PMT 2 " << r.NumberOfPeaks(2) 
	  << " PMT 3 " << r.NumberOfPeaks(3) << std::endl;
	for (size_t kPMT=0; kPMT != 4; kPMT++) {
	  if (r.DeltaTs(kPMT) == DBL_MAX) continue;
          o << " PMT" << kPMT  << " delta T = " <<  r.DeltaTs(kPMT) << std::endl;
	}
	o << " ------------------------------------------------------------" << std::endl << std::endl;
        o.setf(prevFlags); o.precision(prevPrecision);
        return o;
       }
    
  }  
