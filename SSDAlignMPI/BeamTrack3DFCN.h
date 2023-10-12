////////////////////////////////////////////////////////////////////////
/// \brief   3D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACK3DFCN_H
#define BEAMTRACK3DFCN_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "BeamTrackCluster.h" 
#include "BTAlignGeom.h"
#include "Minuit2/FCNBase.h"
#include "BTMagneticField.h"

namespace emph{ 
  namespace rbal {
    
    class BeamTrack3DFCN : public ROOT::Minuit2::FCNBase {
    
    public:
      BeamTrack3DFCN();
      
    private:
      BTAlignGeom* myGeo;
      bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. 
      bool fNoMagnet; // to align on 120 GeV. 
      bool fDebugIsOn;
      char fSelectedView;
      size_t fNumSensorsTotal;
      std::vector<BeamTrackCluster>::const_iterator fItCl;
      double fErrorDef, fOneOverSqrt2, fOneOSqrt12; // for Minuit. 
      double fNominalMomentum;
      mutable int fNCalls;
      mutable std::vector<double> fResids;
      mutable BTMagneticField *fMagField;
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (x,x' in our case.)
     
      inline void SetClusterPtr(std::vector<BeamTrackCluster>::const_iterator it) { fItCl = it; } 
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline void SetNoMagnet(bool v=true) { fNoMagnet = v; }
      inline void SetMCFlag(bool v) { fIsMC = v; }
      inline void SetNominalMomentum(double v) { fNominalMomentum = v; } 
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
      inline void resetNumCalls() { fNCalls = 0; }
      inline void SetSelectedView(char v) { fSelectedView = v; } 
      
      inline void SetDebugOn(bool v=true) { fDebugIsOn = v; }
     };
   }
}
#endif
    
