////////////////////////////////////////////////////////////////////////
/// \brief   2D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
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
#include <cassert>

#include "SSDReco/experimental/SSD3DTrackKlmFitFCNAutre.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

// emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr; already defined elsewhere

namespace emph {
  namespace ssdr {
    
    SSD3DTrackKlmFitFCNAutre::SSD3DTrackKlmFitFCNAutre(int runNum) :
    FCNBase(),
    fRunNum(runNum), fnSts(4), 
    fGeoService(),  
    fEmgeo(fGeoService->Geo()),
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fMyConvert('A'),  
    fMagField(nullptr), fPhase1c(true), 
    fDebugIsOn(false), fIntegrationStep(3.0), fMaxDwnstrStation(5), // last parameter might need some tuning.. This is the value at 31 GeV.
    fNumMaxPropIter(5), fDeltaXSlopeMinChange(1.0e-6), 
    fDataSts(), fNoMagnet(false), fMagShift(3, 0.),
    fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), 
    fOneOSqrt12(std::sqrt(1.0/12.)), fMultScatterOneStationSq(8*3.14e-5 * 3.14e-5), 
      // in micron Sq.., per mm^ sq.  at 120 GeV.  one station only..Doubling the uncertainties.. 
    fPropMagnetErrSq(0.1*0.1)
    {
     
       art::ServiceHandle<emph::MagneticFieldService> bField;
       fMagField = bField->Field();
       art::ServiceHandle<emph::geo::GeometryService> geo;
       fNoMagnet = geo->Geo()->MagnetUSZPos() < 0.;
       fnSts = fRunNum < 2000 ? 4 : 5;   // Getting ready for Phase1c (reluctantly... ) 
       fXSts.clear();  fSlXSts.clear(); fYSts.clear(); fSlYSts.clear(); 
       fExpectedMomentum = 50.0;
       fPhase1c = (runNum > 1999) ? true : false;
       for (size_t k=0; k != static_cast<size_t>(fnSts); k++) {
          std::pair<double,double> blankVal(DBL_MAX, DBL_MAX);
          fXSts.push_back(blankVal); fSlXSts.push_back(blankVal);
          fYSts.push_back(blankVal); fSlYSts.push_back(blankVal);
	  fChiSqSts.push_back(DBL_MAX);
       }
    }
    SSD3DTrackKlmFitFCNAutre::~SSD3DTrackKlmFitFCNAutre() {
    }  
    double SSD3DTrackKlmFitFCNAutre::operator()(const std::vector<double> &pars) const {
    
      if (fDebugIsOn) std::cerr << "SSD3DTrackKlmFitFCNAutre::operator, Start, number of track parameters " 
                                << pars.size() << " Number of data pts " << fDataSts.size() << std::endl; 
      for (size_t kSt = 0; kSt!= static_cast<size_t>(fnSts); kSt++) {
        if (!this->setInitSt(kSt)) return 5.0e5;
      }
      fChiSqX = 0.; fChiSqY = 0.; 
      double chiSq = 0.;
      for (size_t kSt = 0; kSt != static_cast<size_t>(fnSts) - 1; kSt++) {
        if (fDataSts[kSt]->NumClusters() == 3) chiSq += fChiSqSts[kSt]; // If we have missing SSD cluster, got no Space point chiSq 
	if (fDataSts[kSt]->Station() > (fMaxDwnstrStation-1)) continue; 
        const double chiSqProp = this->propagateStsNext(kSt, kSt+1, pars[0]);
	chiSq += chiSqProp;
//	if (fDebugIsOn)  std::cerr << " ... Checking in operator of FCN... Prop chiSq " << chiSqProp << " Current total chiSq " << chiSq << std::endl;
      }			
//      chiSq += fChiSqSts[static_cast<size_t>(fnSts) - 1]; // Why doing this? 
//      if (fDebugIsOn) { std::cerr << " And.. the total chiSq is " << chiSq << " Quit here and now .. " << std::endl; exit(2); }
      if (fDebugIsOn) { std::cerr << " And.. the total chiSq is " << chiSq << " and keep going .. .. " << std::endl; }
      return chiSq;
    }
    
    
    bool SSD3DTrackKlmFitFCNAutre::setInitSt(size_t kStation) const {
      const size_t kSt = kStation; // In case we need to shift indices.. There is always  two stations infront of the target. 
      if (fDebugIsOn) std::cerr << " ....SSD3DTrackKlmFitFCNAutre::setInitSt for kStation " << kStation+2 << " kSt (in this method) " << kSt << std::endl; 
      if (kStation >= fDataSts.size()) return false;
      fXSts[kSt].first = fDataSts[kStation]->X(); fXSts[kSt].second = fDataSts[kStation]->XErr();
      fYSts[kSt].first = fDataSts[kStation]->Y(); fYSts[kSt].second = fDataSts[kStation]->YErr();
      fChiSqSts[kSt] = fDataSts[kStation]->ChiSq(); 
      if (fDebugIsOn) std::cerr << " ....X = " << fXSts[kSt].first << " +- " << fXSts[kSt].second 
                               << "  and Y = " << fYSts[kSt].first << " +- " << fYSts[kSt].second << std::endl; 
      return true;
    }
//
//  
    double SSD3DTrackKlmFitFCNAutre::propagateStsNext(size_t kStStart, size_t kStEnd, double p) const {
      // Note: in this context, kStStart = 0 refers to station 2. 
      int nIter = 0;
      double chiSq = DBL_MAX;
      // Compute the slopes at station 3, linear propagation. 
      if (fDebugIsOn) std::cerr << " .... SSD3DTrackKlmFitFCNAutre::propagateStsNext, kStStart  " 
                                << kStStart << " kStEnd " << kStEnd << " p " << p 
				<<  " from X = " << fXSts[kStStart].first << " to X = " << fXSts[kStEnd].first << std::endl; 
      const double zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, kStStart+2);
      const double zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, kStEnd+2);
      const double deltaZ = zLocDownstrMagnet - zLocUpstreamMagnet;
      double slx = (kStStart == 0) ? (fXSts[kStEnd].first - fXSts[kStStart].first)/deltaZ :
                                      fSlXSts[kStStart].first;
      double sly = (kStStart == 0) ? (fYSts[kStEnd].first - fYSts[kStStart].first)/deltaZ :
                                      fSlYSts[kStStart].first;
      const double slxInit = slx;  const double slyInit = sly;			      
      if (fDebugIsOn) std::cerr << "....  Tracing through  " <<  zLocUpstreamMagnet 
				<< " Downstream " << zLocDownstrMagnet << " slx " << 1.0e3*slx 
				<< " sly " << 1.0e3*sly << " (mrad) " << " delta Z " << deltaZ << std::endl;
      double deltaXSl = 0.; 
      const int Q = (p > 0.) ? 1 : -1 ;  // hopefully correct.. 
      const double stepAlongZ = fIntegrationStep; // such we don't introduce suspicious correlations.. 
      std::vector<double> startMag(6, 0.); std::vector<double> endMag(6, 0.); 
      startMag[0] = fXSts[kStStart].first; // assume station 2 is the origin. 
      startMag[1] = fYSts[kStStart].first;;  
      startMag[2] = zLocUpstreamMagnet; 
      startMag[3] = slx*p; // 
      startMag[4] = sly*p; // assume small slope, sin(theta) = theta.. 
      startMag[5] = std::abs(p) * std::sqrt(1.0 - slx*slx - sly*sly); 
      endMag[2] = zLocDownstrMagnet; 
      // 
      // Implement Magnet misalignment.. 
      startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
      fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
      if (((fRunNum < 2000) && (kStStart == 3)) ||
          ((fRunNum > 1999) && (kStStart == 4))) {
        std::cerr << " ...  SSD3DTrackKlmFitFCNAutre::propagateStsNext , p = " << p << std::endl 
        	  << " ............ Inside the magnet, track swerved outside the magnet.. return bad ChiSq " << std::endl;
        return 1.0e6; 
       } 
       //
	// Back shift.. 
       endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
       const double slxNew = endMag[3]/p; sly = endMag[4]/p;
       deltaXSl = slxNew - slx; slx = slxNew; 
       const double xPredStNext = endMag[0];  double yPredStNext = endMag[1];
       // correction for the small difference of X and Y planes. 
       double ddZXY = fEmVolAlP->ZPos(emph::geo::Y_VIEW, kStEnd+2) - fEmVolAlP->ZPos(emph::geo::X_VIEW, kStEnd+2); 
       yPredStNext += ddZXY*sly;  
       if (fDebugIsOn) 
	  std::cerr << " ..... After the Usptream X and Y Integrated predictions from Station "  
		   << kStStart+2 << " to Station " << kStEnd+2 << ", xPred " 
	 << xPredStNext << " yPred " <<  yPredStNext << " X Slope " << 1.0e3*slx << " Y slope " << 1.0e3*sly 
	 << " deltaXSl " << 1.0e3*deltaXSl << " ( mrad ) " << std::endl;
	 nIter++; 
 	 //
	 
       const double errMultSq = fMultScatterOneStationSq*(120./fExpectedMomentum)*(120./fExpectedMomentum)
                                * deltaZ * deltaZ; 
       double errMagAlignmentSq = 0.; 
       if (((fRunNum < 2000) && (kStStart == 3)) ||
           ((fRunNum > 1999) && (kStStart == 4))) errMagAlignmentSq = fPropMagnetErrSq;  			
       const double errPosXaSq = fXSts[kStStart].second * fXSts[kStStart].second + errMagAlignmentSq; 
       const double errPosYaSq = fXSts[kStStart].second * fXSts[kStStart].second + errMagAlignmentSq; 
       const double errPosXbSq = fXSts[kStEnd].second * fXSts[kStEnd].second + errMultSq ;  
       const double errPosYbSq =  fYSts[kStEnd].second * fYSts[kStEnd].second + errMultSq ; 
       double weightPosXa = (1.0/errPosXaSq); double weightPosXb = 1.0/errPosXbSq;
       const double sumWX = weightPosXa + weightPosXb; weightPosXa /= sumWX;  weightPosXb /= sumWX;
       double weightPosYa = (1.0/errPosYaSq); double weightPosYb = 1.0/errPosYbSq;
       const double sumWY = weightPosYa + weightPosYb; weightPosYa /= sumWY;  weightPosYb /= sumWY;
       const double errPosXT = errPosXaSq + errPosXbSq; const double errPosYT = errPosYaSq + errPosYbSq;
       const double chiSqX = ((xPredStNext - fXSts[kStEnd].first) * (xPredStNext - fXSts[kStEnd].first))/errPosXT;
       const double chiSqY = ((yPredStNext - fYSts[kStEnd].first) * (yPredStNext - fYSts[kStEnd].first))/errPosYT;
       fChiSqX += chiSqX; fChiSqY += chiSqY; 
       chiSq = chiSqX + chiSqY; fNDGF += 2;
       fXSts[kStEnd].first = xPredStNext*weightPosXa + weightPosXb*fXSts[kStEnd].first;
       fYSts[kStEnd].first = yPredStNext*weightPosYa + weightPosYb*fYSts[kStEnd].first;
//       fXSts[kStEnd].second = std::sqrt((1.0/sumWX)/2.); // Wrong, we do not decrease the intrinsic aacuracy.. 
//       fYSts[kStEnd].second = std::sqrt((1.0/sumWY)/2.); // approximate 
       fXSts[kStEnd].second += errMultSq + errMagAlignmentSq; // adding the multiple scattering contribution. 
       fYSts[kStEnd].second += errMultSq + errMagAlignmentSq; // still approximate 
       fSlXSts[kStEnd].first = slx; // Obvious;;
       // The Y slope is badly biased between station 2 and 3.. So, we keep retuning it.  For alignment 7s_1104_5 at least.. 
       // Assume no deflection in the vertical plane for now..
       if (kStStart != 0) {
         const double deltaZEndfr0 = fEmVolAlP->ZPos(emph::geo::Y_VIEW, kStEnd+2) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 2);
         fSlYSts[0].first = (fYSts[kStEnd].first - fYSts[0].first)/deltaZEndfr0 ;
         fSlYSts[kStEnd].first =  fSlYSts[0].first; // Keep the same slope, but refined.. 
         if (fDebugIsOn) std::cerr << " retuning Y slope ... Y slope from prop " << 1.0e3*sly << " new slope " 
	           << 1.0e3*fSlYSts[0].first << " on delta Z " << deltaZEndfr0 << std::endl;
       } else { 
         fSlYSts[kStEnd].first = sly;
       }
       fSlXSts[kStStart].second = 
         std::sqrt(fXSts[kStStart].second * fXSts[kStStart].second + fXSts[kStEnd].second * fXSts[kStEnd].second)/deltaZ;
       fSlYSts[kStStart].second = 
         std::sqrt(fYSts[kStStart].second * fYSts[kStStart].second + fYSts[kStEnd].second * fYSts[kStEnd].second)/deltaZ;
       fSlXSts[kStEnd].second = fSlXSts[kStStart].second; // Intrinsic propagation uncertainties neglected. 
       fSlYSts[kStEnd].second = fSlYSts[kStStart].second; // Intrinsic propagation uncertainties neglected. 
       if (kStStart == 0) {
         //
         // Now the slopes.. We refine the slopes at station 2, as they are the second ingredient to the scattering angle. 
	 // Linearize... Assume no uncertainties coming from the magnet. Only fringe field at station 3. 
	 // Now that we have a better position at Station3,  redefine the slope 
	 const double deltaMagSlx = slx - slxInit; const double deltaMagSly = sly - slyInit;
	 const double slxEndNoMag = (fXSts[kStEnd].first - fXSts[kStStart].first)/deltaZ;
	 const double slyEndNoMag = (fYSts[kStEnd].first - fYSts[kStStart].first)/deltaZ;
	 fSlXSts[kStStart].first = slxEndNoMag + deltaMagSlx; 
	 fSlYSts[kStStart].first = slyEndNoMag + deltaMagSly; 
	 // uncertainties for the slope at Station 2. 
	 fSlXSts[kStEnd].second = std::sqrt(1.0/sumWX)/deltaZ; // Obvious, but neglect uncertainty in Z...  	 
	 fSlYSts[kStEnd].second = std::sqrt(1.0/sumWY)/deltaZ;
         if (fDebugIsOn) 
           std::cerr << "  ... At Station 2, Redefining SlX = " << 1.0e3*fSlXSts[kStStart].first << " +- " << 1.0e3*fSlXSts[kStStart].second 
	           << " Y = " << 1.0e3*fSlYSts[kStStart].first << " +- " << 1.0e3*fSlYSts[kStStart].second << " (mrad ) " << std::endl;
       }
       if (fDebugIsOn) {
         std::cerr << "  .... At the end, X = " << fXSts[kStEnd].first << " +- " << fXSts[kStEnd].second 
	           << " Y = " << fYSts[kStEnd].first << " +- " << fYSts[kStEnd].second << std::endl;
         std::cerr << "  ... SlX = " << 1.0e3*fSlXSts[kStEnd].first << " +- " << 1.0e3*fSlXSts[kStEnd].second 
	           << " Y = " << 1.0e3*fSlYSts[kStEnd].first << " +- " << 1.0e3*fSlYSts[kStEnd].second << " return chiSq " << chiSq << std::endl;
//	 std::cerr << " And quit after such a tour de force .. " << std::endl; exit(2);
       }  
       return chiSq; 
    }
//    
  } // namespace 
} // Name space emph  
 
