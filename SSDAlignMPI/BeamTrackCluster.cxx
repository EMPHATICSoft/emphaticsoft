////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a compact SSD cluster, used for SSD 2nd order 
///          alignment.  We only keep the average strip and its RMS. 
///          Used by BTAlignInput class  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>

#include "BeamTrackCluster.h"

namespace emph {
  namespace rbal {
  
    BeamTrackCluster::BeamTrackCluster() : 
    fSpill(-1),  fEvtNum(-1), fKeep(true)
    { ; } 
    
// 
    
    BeamTrackCluster::BeamTrackCluster(int spill, int evt, const std::vector<double> &XViewData, 
                const std::vector<double> &YViewData, const std::vector<double> &UViewData, const std::vector<double> &VViewData) : 
    fSpill(spill),  fEvtNum(evt), fKeep(true),
    fXAvStrips(XViewData.size()/2, 0.), fXRmsStrips(XViewData.size()/2, 1.0e9),
    fYAvStrips(YViewData.size()/2, 0.), fYRmsStrips(YViewData.size()/2, 1.0e9),
    fUAvStrips(UViewData.size()/2, 0.), fURmsStrips(UViewData.size()/2, 1.0e9),
    fVAvStrips(VViewData.size()/2, 0.), fVRmsStrips(VViewData.size()/2, 1.0e9)
    { 
      for (size_t kk=0; kk!=fXAvStrips.size(); kk++) {
        size_t k = 2*kk;
        fXAvStrips[kk] = XViewData[k]; fXRmsStrips[kk] = XViewData[k+1];
      }
      for (size_t kk=0; kk!=fYAvStrips.size(); kk++) {
        size_t k = 2*kk;
        fYAvStrips[kk] = YViewData[k]; fYRmsStrips[kk] = YViewData[k+1];
      }
      for (size_t kk=0; kk!=fUAvStrips.size(); kk++) {
        size_t k = 2*kk;
        fUAvStrips[kk] = UViewData[k]; fURmsStrips[kk] = UViewData[k+1];
      }
      for (size_t kk=0; kk!=fVAvStrips.size(); kk++) {
        size_t k = 2*kk;
        fVAvStrips[kk] = VViewData[k]; fVRmsStrips[kk] = VViewData[k+1];
      }
    }  
  } 
} 
