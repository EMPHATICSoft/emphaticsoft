////////////////////////////////////////////////////////////////////////
/// \brief   3D linear fit FCN function for Downstream track, based on Kalman 
///           filter based set of propagation.  The FCNBase derived class.
///           There is only one free paramter, the momentum. 
///           The track slopes at Station 2 are solely derived from Station 2 & 3, 
///           and the position is derived from the Station data point.    
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef SSD3DTrackKlmFitFCNAlgo1_H
#define SSD3DTrackKlmFitFCNAlgo1_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "Minuit2/FCNBase.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "MagneticField/MagneticField.h"
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/VolatileAlignmentParams.h" 
#include "SSDReco/ConvertDigitToWCoordAlgo1.h" 

namespace emph{ 
  namespace ssdr {

    typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    
    class SSD3DTrackKlmFitFCNAlgo1 : public ROOT::Minuit2::FCNBase {
    
    public:
      explicit SSD3DTrackKlmFitFCNAlgo1(int aRunNumber);
      ~SSD3DTrackKlmFitFCNAlgo1();
      
    private:
      int fRunNum, fnSts; // for use to leave option open to support 7 stations instead of 5 
      art::ServiceHandle<emph::geo::GeometryService> fGeoService;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
      emph::ssdr::ConvertDigitToWCoordAlgo1 fMyConvert;
      emph::MagneticField *fMagField;
      bool fDebugIsOn;
      double fIntegrationStep;
      int fNumMaxPropIter; // maximum number of iteration, to get a stable projection. 
      double fDeltaXSlopeMinChange; // if the x slope extimate change by mote than that, we keep iterating.   
      std::vector<myItCl> fDataSts;
      bool fNoMagnet; 
      mutable std::vector<double> fMagShift;
      double fErrorDef; // for Minuit. 
      double fOneOverSqrt2, fOneOSqrt12;
      double fMultScatterOneStationSq; 
      mutable int fNDGF; // The number of degrees of freedom.. 
      mutable std::vector<std::pair<double, double> > fXSts, fSlXSts, fYSts, fSlYSts; 
        // first is the coordinate, second is the uncertainty. We do not propagate the full covariance matrix.. 
      // Volatile data, to avoid repeating mantra.. See fillVolatileCl
      mutable double fExpectedMomentum; // 
      mutable int fkStV, fkSeV; 
      mutable double fzV, ftMeasV, ftMeasErrV; 
      mutable std::vector<double> fChiSqSts; // on stations.. 
      
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the 1D parameter vector. Only the momentum P
     
      inline void SetInputClusters(std::vector<myItCl> &data) { 
       fDataSts= data;
      } // No deep copy of clusters, but less readable code..  
      inline void SetRunNum(int r) { fRunNum = r; }
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline void SetMagnetShift(std::vector<double> v) { fMagShift = v; } 
      inline void SetExpectedMomentum(double v) { fExpectedMomentum = v; } 
      inline void SetDebugOn(bool v = true) { fDebugIsOn = v; }
      inline void SetIntegrationStep(double s) { fIntegrationStep = s; }
      inline void SetMultScatterErr(double s) { fMultScatterOneStationSq = s*s; }
      inline void SetNumMaxPropIter (int n) { fNumMaxPropIter = n; } 
      inline void SetDeltaXSlopeMinChange (double d) {fDeltaXSlopeMinChange = d; }  
    
    private:
    
       bool setInitSt(size_t kSt) const; // return false if only one SSDCluster.. Should not happen often.. 
       double propagateStsNext(size_t kStStart, size_t kStEnd, double p ) const ; // p is the momentum 
       // Return the chiSq associated with station, after propagation.  Readjust the slope at station 3. Fill the position at Station3.    
       void fillVolatileCl(std::vector<rb::SSDCluster>::const_iterator it) const ;   
     
     };
   }
}
#endif
    
