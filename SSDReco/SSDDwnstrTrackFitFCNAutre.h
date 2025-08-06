////////////////////////////////////////////////////////////////////////
/// \brief   3D non-linear fit FCN function for Beam Track alignment, downstream of the target. 
///          Depends on the magnetic field map.
///          Input data is a vector of  rbex::SSDStationPt 
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef SSDDWNSTRTRACKFITFCNAUTRE_H
#define SSDDWNSTRTRACKFITFCNAUTRE_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "Minuit2/FCNBase.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "MagneticField/MagneticField.h"
#include "RecoBase/SSDStationPt.h" 
#include "SSDReco/VolatileAlignmentParams.h" 

namespace emph{ 
  namespace ssdr {

    typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    
    class SSDDwnstrTrackFitFCNAutre : public ROOT::Minuit2::FCNBase {
    
    public:
      explicit SSDDwnstrTrackFitFCNAutre(int aRunNumber);
      ~SSDDwnstrTrackFitFCNAutre();
      
    private:
      int fRunNum;
      art::ServiceHandle<emph::geo::GeometryService> fGeoService;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
      emph::MagneticField *fMagField;
      bool fPhase1c;
      bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. But... nasty correction done in reconstruction space points,
      //  So, this is obsolete. I hope.. 
      bool fDebugIsOn;
      int fMaxDwnstrStation;
      double fIntegrationStep;
      double fStartingMomentum; 
      size_t fNumSensorsTotal;
      std::vector<rbex::SSDStationPt> fData; // Deep copy required here.. 
      bool fNoMagnet; 
      mutable std::vector<double> fZPos;
      mutable std::vector<double> fMagShift;
      double fErrorDef; // for Minuit. 
      double fOneOverSqrt2, fOneOSqrt12;
      mutable std::vector<double> fResids; // with respect to the 3D SSD reconstructed Space points. Phase1b : 8 of them, 4 for X, 4 for Y 
      //  (U and W view data is include in the Space points.. 
      mutable double fZLocUpstreamMagnet, fZLocDownstrMagnet;
      mutable double fLastChi2;
      mutable std::ofstream fFOutResids; 
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (x,x', y, y' and momentum in our case.)
     
      inline void SetInputPts(std::vector<rbex::SSDStationPt> &data) { fData = data; } // Deep Copy  
      inline void AddInputPt(std::vector<rbex::SSDStationPt>::const_iterator it) { fData.push_back(*it); } // again..   
      inline void ResetInputPts() { fData.clear();  }
      inline void ResetZpos() { fZPos.clear(); fZLocUpstreamMagnet = DBL_MAX; fZLocDownstrMagnet = DBL_MAX; }
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline void SetMCFlag(bool v) { fIsMC = v; }
      inline void SetPhase1x(int aRun) { fPhase1c = (aRun > 1999) ? true : false; }
      inline void SetNoMagnet(bool v=true) { fNoMagnet = v; }
      inline void SetMagnetShift(std::vector<double> v) { fMagShift = v; } 
      inline void SetStartingMomentum(double v) { fStartingMomentum = v; } 
      inline void SetDebugOn(bool v = true) { fDebugIsOn = v; }
      inline void SetIntegrationStep(double s) { fIntegrationStep = s; }
      inline void SetMaxDwnstrStation(int i) { fMaxDwnstrStation = i; } 
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
      // Adding  upstream of the nominal target, but downstream of the iron brick
      void AddInputUpstreamStation(size_t kSt, double xVal, double xValErr, double yVal, double yValErr); 
      //  
      //  Getter 
      //
      inline int RunNumber() const {return fRunNum; }
      
      void OpenOutResids(const std::string &fNameStr); 
      void SpitOutResids(int spill, int evt); 
      void printInputData() const; 
    
    private:
    
       void getZPos() const ; // to be called at the beginning of a fit, when we know the list of SSDCluster.  
      
     };
   }
}
#endif
    
