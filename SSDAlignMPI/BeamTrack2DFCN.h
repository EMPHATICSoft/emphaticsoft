////////////////////////////////////////////////////////////////////////
/// \brief   2D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACK2DFCN_H
#define BEAMTRACK2DFCN_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "BeamTrackCluster.h" 
#include "BTAlignGeom.h"
#include "Minuit2/FCNBase.h"

namespace emph{ 
  namespace rbal {
    
    class BeamTrack2DFCN : public ROOT::Minuit2::FCNBase {
    
    public:
      BeamTrack2DFCN();
      
    private:
      BTAlignGeom* myGeo;
      std::vector<BeamTrackCluster>::const_iterator fItCl;
      char fView; // X or Y 
      double fErrorDef, fOneOSqrt12; // for Minuit. , and for pitch err.. 
      mutable std::vector<double> fResids;
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (x,x' in our case.)
     
      inline void SetClusterPtr(std::vector<BeamTrackCluster>::const_iterator it) { fItCl = it; } 
      inline void SetView(char aView) { fView = aView; }
      inline void SetErrorDef(double e) { fErrorDef = e; }
      
      inline char View() const { return fView; } 
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
    };
  }
}
#endif
    