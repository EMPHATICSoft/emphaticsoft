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

#ifndef SSD3DTrackKlmFitFCNAutre_H
#define SSD3DTrackKlmFitFCNAutre_H

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
#include "SSDReco/ConvertDigitToWCoordAutre.h" 
#include "RecoBase/SSDStationPt.h" 

namespace emph{ 
  namespace ssdr {

    typedef std::vector<rb::SSDCluster>::const_iterator myItCl; 
    typedef std::vector<rbex::SSDStationPt>::const_iterator myItStPt; 
    
    class SSD3DTrackKlmFitFCNAutre : public ROOT::Minuit2::FCNBase {
    
    public:
      explicit SSD3DTrackKlmFitFCNAutre(int aRunNumber);
      ~SSD3DTrackKlmFitFCNAutre();
      
    private:
      int fRunNum, fnSts; // for use to leave option open to support 7 stations instead of 5 
      art::ServiceHandle<emph::geo::GeometryService> fGeoService;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
      emph::ssdr::ConvertDigitToWCoordAutre fMyConvert;
      emph::MagneticField *fMagField;
      bool fPhase1c;
      bool fDebugIsOn;
      double fIntegrationStep;
      int fMaxDwnstrStation; // We can skip Station 6 (as we do for the default track fitter, it turns out.. ) 
      int fNumMaxPropIter; // maximum number of iteration, to get a stable projection. 
      double fDeltaXSlopeMinChange; // if the x slope extimate change by mote than that, we keep iterating.   
      std::vector<myItStPt> fDataSts;
      bool fNoMagnet; 
      mutable std::vector<double> fMagShift;
      double fErrorDef; // for Minuit. 
      double fOneOverSqrt2, fOneOSqrt12;
      double fMultScatterOneStationSq, fPropMagnetErrSq ; // Multiple scattering and guessed uncertainty from Magnet propagation. 
      // Or alignment.. applied in the magent.. 
      mutable double fChiSqX, fChiSqY;  
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
     
      inline void SetInputClusters(const std::vector<myItStPt> &data) { 
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
      inline void SetMaxDwnstrStation(int i) { fMaxDwnstrStation = i; }  
      // 
      // Getting the track parameters.. 
      //
      inline std::pair<double, double> XStart() const { 
         if (fXSts.size() == 0) return std::pair<double,double>(9999., 9999.);
         return fXSts[0];
      }
      inline std::pair<double, double> YStart() const { 
         if (fYSts.size() == 0) return std::pair<double,double>(9999., 9999.);
         return fYSts[0];
      }
      inline std::pair<double, double> SlXStart() const { 
         if (fSlXSts.size() == 0) return std::pair<double,double>(9999., 9999.);
         return fSlXSts[0];
      }
      inline std::pair<double, double> SlYStart() const { 
         if (fSlYSts.size() == 0) return std::pair<double,double>(9999., 9999.);
         return fSlYSts[0];
      }
      // Additional info 
      inline double SumChiSqSts () const {
         double sum = 0.; 
//	 if (fDebugIsOn) std::cerr << " .... SSD3DTrackKlmFitFCNAutre::SumChiSqSts, numSts " << fDataSts.size() << std::endl; 
	 for (size_t k=0; k != fDataSts.size(); k++) {
//	   if (fDebugIsOn) std::cerr << " .... ....  for Station " 
//	                             << fDataSts[k]->Station() << " Type " << fDataSts[k]->Type() << std::endl;  
	   if (fDataSts[k]->NumClusters() != 3) continue;
	   if (fDataSts[k]->Station() > fMaxDwnstrStation) continue;
	   sum += fDataSts[k]->ChiSq(); 
//	   if (fDebugIsOn) std::cerr << " .... ....  ...  current adding  " << fDataSts[k]->ChiSq() << std::endl;  
	 }
	 return sum;
      }
      inline double ChiSqXView() const { return fChiSqX; } 
      inline double ChiSqYView() const { return fChiSqY; } 
    private:
    
       bool setInitSt(size_t kSt) const; // return false if only one SSDCluster.. Should not happen often.. 
       double propagateStsNext(size_t kStStart, size_t kStEnd, double p ) const ; // p is the momentum 
       // Return the chiSq associated with station, after propagation.  Readjust the slope at station 3. Fill the position at Station3.    
     
     };
   }
}
#endif
    
