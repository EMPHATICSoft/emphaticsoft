////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a compact SSD cluster, used for SSD 2nd order 
///          alignment.  We only keep the average strip and its RMS. 
///          Used by BTAlignInput class  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef BEAMTRACKCLUSTER_H
#define BEAMTRACKCLUSTER_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace emph{ 
  namespace rbal {
    
    class BeamTrackCluster {
    
    public:
      BeamTrackCluster();
      explicit BeamTrackCluster(int spill, int evt, const std::vector<double> &XViewData, 
                const std::vector<double> &YViewData, const std::vector<double> &UViewData, const std::vector<double> &VViewData);
     
    private: 
      int fSpill; 
      int fEvtNum;
      mutable bool fKeep; // keep or not... On Rank 0, we do not waste time in deleting events.. Used only in the MPI context. 
      std::vector<double> fXAvStrips, fXRmsStrips, fYAvStrips, fYRmsStrips;
      std::vector<double> fUAvStrips, fURmsStrips, fVAvStrips, fVRmsStrips;
      
    public:
      // Keep or not ? On rank 0, we set the transfered object to do not keep, but avoid wasting time to erase them 
      inline void DoNotUse() const {fKeep = false;} // pseudo const !!! 
      // Getters 
      inline int Spill() const { return fSpill; }
      inline int EvtNum() const { return fEvtNum; }
      inline bool Keep() const {return fKeep;}
      inline double TheAvStrip(char view, size_t kSt) const {  
        switch (view) {
	  case 'X' : { if (kSt < fXAvStrips.size()) return fXAvStrips[kSt]; break; } 
	  case 'Y' :  { if (kSt < fYAvStrips.size()) return fYAvStrips[kSt]; break; } 
	  case 'U' :  { if (kSt < fUAvStrips.size()) return fUAvStrips[kSt]; break; } 
	  case 'V' :  { if (kSt < fVAvStrips.size()) return fVAvStrips[kSt]; break; } 
	  case 'W' : { if (kSt < fVAvStrips.size()) return fVAvStrips[kSt]; break; }
	  default: { return 0.; break; }  
	}
	return 0.;
      } 
      inline double TheRmsStrip(char view, size_t kSt) const {
        switch (view) {
	  case 'X' :  { if (kSt < fXRmsStrips.size()) return fXRmsStrips[kSt]; break; } 
	  case 'Y' :  { if (kSt < fYRmsStrips.size()) return fYRmsStrips[kSt]; break; } 
	  case 'U' :  { if (kSt < fURmsStrips.size()) return fURmsStrips[kSt]; break; } 
	  case 'V' :  { if (kSt < fVRmsStrips.size()) return fVRmsStrips[kSt]; break; } 
	  case 'W' : { if (kSt < fVRmsStrips.size()) return fVRmsStrips[kSt]; break; }
	  default: { return 1.0e9; break; }  
	}
	return 1.0e9;
      } 
       
    };
  } // rbal 
} // emph
#endif // 
