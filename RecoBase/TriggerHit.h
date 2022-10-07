////////////////////////////////////////////////////////////////////////
/// \brief   Summary of the Trigger Counter data, developed in ../TOF
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TRIGGERHIT_H
#define TRIGGERHIT_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>

namespace rb {
  
  class TriggerHit {
  public: 
    explicit TriggerHit(double sum4PMTs, const std::vector<int> &numPeaks, const std::vector<double> &deltaTs);
    
    
    
  private:
    const double _cutSum4PMTSLow, _cutSum4PMTSHigh; // Tuned for Phase 1b, to be reviewed for Phase 1 .. Or, alternatively, 
                                                    // Remove the const anad implementer setters. 
    bool _isSingleParticle; // a flag stating the analysis of the 4 PMT waveform, via the TOF/PeakInWaveform class,  give only one particle 
                            // in the 400 ns window
    double _sum4PMTs; // the sum of the PMT ADCs, baseline subracted. 
    std::vector<int> _numPeaks; // the number of peaks found in each PMT waveform of the trigger counter.  
    std::vector<double> _deltaTs; // The difference in time, 2nd peak - first peak, Set to DBL_MAX is only one peak is found. 
    
  public:
  
    inline bool isSingleBeamParticle() const { return  _isSingleParticle;}
    inline double Sum4PMTs() const { return _sum4PMTs; }
    inline int NumberOfPeaks(size_t iPMT) const { 
      if (iPMT >=  _numPeaks.size()) return 0; // protection.. 
      return _numPeaks[iPMT];
    } 
    inline double DeltaTs(size_t iPMT) const { 
      if (iPMT >=  _deltaTs.size()) return DBL_MAX;
      return _deltaTs[iPMT];
    } 
    
    friend std::ostream& operator << (std::ostream& o, const rb::TriggerHit& r);
  
  };
  
} // namespace 
  
#endif // TRIGGERHIT_H
