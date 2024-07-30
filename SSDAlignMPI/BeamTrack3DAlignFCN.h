////////////////////////////////////////////////////////////////////////
/// \brief   3D linear fit FCN function for Beam Track alignment.
///          Fit for the transverse shift, tilts, Z position.    
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACK3DALIGNFCN_H
#define BEAMTRACK3DALIGNFCN_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "BeamTrackCluster.h" 
#include "BTAlignGeom.h"
#include "Minuit2/FCNBase.h"

namespace emph{ 
  namespace rbal {
    
    class BeamTrack3DAlignFCN : public ROOT::Minuit2::FCNBase {
    
    public:
      BeamTrack3DFCN();
      
    private:
      BTAlignGeom* myGeo;
      size_t fNumSensorsXorY;
      size_t fNumSensorsU;
      size_t fNumSensorsV;
      size_t fNumSensorsTotal;
      double fErrorDef, fOneOverSqrt2, fOneOSqrt12; // for Minuit. 
      mutable std::vector<double> fResids;
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (x,x' in our case.)
     
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
      
      
     };
   }
}
#endif
    
