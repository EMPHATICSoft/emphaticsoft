////////////////////////////////////////////////////////////////////////
/// \brief   3D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef SSD3DTRACKFITFCNALGO1_H
#define SSD3DTRACKFITFCNALGO1_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "Minuit2/FCNBase.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "MagneticField/MagneticField.h"
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/VolatileAlignmentParams.h" 

namespace emph{ 
  namespace ssdr {

    typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    
    class SSD3DTrackFitFCNAlgo1 : public ROOT::Minuit2::FCNBase {
    
    public:
      explicit SSD3DTrackFitFCNAlgo1(int aRunNumber);
      ~SSD3DTrackFitFCNAlgo1();
      
    private:
      runhist::RunHistory *fRunHistory;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
      emph::EMPHATICMagneticField *fMagField;
      bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. 
      bool fDebugIsOn;
      double fIntegrationStep;
      double fExpectedMomentum; 
      size_t fNumSensorsTotal;
      std::vector<myItCl> fData;
      bool fNoMagnet; 
      mutable std::vector<double> fZPos;
      mutable std::vector<double> fMagShift;
      double fErrorDef; // for Minuit. 
      double fOneOverSqrt2, fOneOSqrt12;
      mutable std::vector<double> fResids;
      mutable double fZLocUpstreamMagnet, fZLocDownstrMagnet;
      mutable double fLastChi2;
      mutable std::ofstream fFOutResids; 
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (x,x', y, y' and momentum in our case.)
     
      inline void SetInputClusters(std::vector<myItCl> &data) { fData = data; } // No deep copy of clusters, but less readable code..  
      inline void ResetZpos() { fZPos.clear(); fZLocUpstreamMagnet = DBL_MAX; fZLocDownstrMagnet = DBL_MAX; }
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline void SetMCFlag(bool v) { fIsMC = v; }
      inline void SetNoMagnet(bool v=true) { fNoMagnet = v; }
      inline void SetMagnetShift(std::vector<double> v) { fMagShift = v; } 
      inline void SetExpectedMomentum(double v) { fExpectedMomentum = v; } 
      inline void SetDebugOn(bool v = true) { fDebugIsOn = v; }
      inline void SetIntegrationStep(double s) { fIntegrationStep = s; }
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
      void OpenOutResids(const std::string &fNameStr); 
      void SpitOutResids(int spill, int evt); 
    
    private:
    
       void getZPos() const ; // to be called at the beginning of a fit, when we know the list of SSDCluster.  
      
     };
   }
}
#endif
    
