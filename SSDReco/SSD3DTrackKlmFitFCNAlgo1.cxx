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

#include "SSDReco/SSD3DTrackKlmFitFCNAlgo1.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

// emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr; already defined elsewhere

namespace emph {
  namespace ssdr {
    
    SSD3DTrackKlmFitFCNAlgo1::SSD3DTrackKlmFitFCNAlgo1(int runNum) :
    FCNBase(),
    fRunNum(runNum), fnSts(4), 
    fGeoService(),  
    fEmgeo(fGeoService->Geo()),
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fMyConvert('A'),  
    fMagField(nullptr), fDebugIsOn(false), fIntegrationStep(3.0), // last parameter might need some tuning.. This is the value at 31 GeV.
    fNumMaxPropIter(5), fDeltaXSlopeMinChange(1.0e-5), 
    fDataSts(), fNoMagnet(false), fMagShift(3, 0.),
    fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), 
    fOneOSqrt12(std::sqrt(1.0/12.)), fMultScatterOneStationSq(0.005*0.005) // at 120 GeV.  one station only..
    {
     
       art::ServiceHandle<emph::MagneticFieldService> bField;
       fMagField = bField->Field();
       art::ServiceHandle<emph::geo::GeometryService> geo;
       fNoMagnet = geo->Geo()->MagnetUSZPos() < 0.;
       fnSts = fRunNum < 2000 ? 4 : 6;   // Getting ready for Phase1c (reluctantly... ) 
       fXSts.clear();  fSlXSts.clear(); fYSts.clear(); fSlYSts.clear(); 
       for (size_t k=0; k != static_cast<size_t>(fnSts); k++) {
          std::pair<double,double> blankVal(DBL_MAX, DBL_MAX);
          fXSts.push_back(blankVal); fSlXSts.push_back(blankVal);
          fYSts.push_back(blankVal); fSlYSts.push_back(blankVal);
	  
       }
    }
    SSD3DTrackKlmFitFCNAlgo1::~SSD3DTrackKlmFitFCNAlgo1() {
    }  
    double SSD3DTrackKlmFitFCNAlgo1::operator()(const std::vector<double> &pars) const {
    
      if (fDebugIsOn) std::cerr << "SSD3DTrackKlmFitFCNAlgo1::operator, Start, number of track parameters " 
                                << pars.size() << " Number of data pts " << fDataSts.size() << std::endl;  
      double chiSq = 0.;
      for (size_t kSt = 2; kSt!= static_cast<size_t>(fnSts); kSt++) {
        fChiSqSts[kSt] = 0.;
        if (!this->setInitSt(kSt)) return 1.0e5;
      }
      for (size_t kSt = 2; kSt!= static_cast<size_t>(fnSts); kSt++) {
        chiSq += fChiSqSts[kSt];
        chiSq += this->propagateStsNext(kSt, kSt+1, pars[0]);
      }			
      return chiSq;
    }
    
    
    bool SSD3DTrackKlmFitFCNAlgo1::setInitSt(size_t kSt) const {
      bool hasViewX=false; bool hasViewY=false; bool hasViewU=false; bool hasViewW=false;
      myItCl itClX, itClY, itClU, itClW; 
      size_t iSensorX = 0; size_t iSensorY = 0; size_t iSensorUorW = 0; 
      size_t nData = 0;
      for (size_t k=0; k != fDataSts.size(); k++) {
        if (fDataSts[k]->Station() != static_cast<int>(kSt)) continue; 
	nData++;
	if (fDataSts[k]->View() == emph::geo::X_VIEW) { hasViewX = true; itClX = fDataSts[k]; } 
	if (fDataSts[k]->View() == emph::geo::Y_VIEW) { hasViewY = true; itClY = fDataSts[k]; } 
	if (fDataSts[k]->View() == emph::geo::U_VIEW) { hasViewU = true; itClU = fDataSts[k]; } 
	if (fDataSts[k]->View() == emph::geo::W_VIEW) { hasViewW = true; itClW = fDataSts[k]; } 
      }	
      if (nData < 2) return false; 
      if (nData > 3) return false;  // should not happen, pattern recognition failure. 
      double xCorr=DBL_MAX; double yCorr=DBL_MAX; double xErr=DBL_MAX; double yErr=DBL_MAX;
      double wErr = DBL_MAX; double dd = DBL_MAX; double uErr = DBL_MAX; 
      if (hasViewX && hasViewY) {
        this->fillVolatileCl(itClX); const double xRaw = ftMeasV; xErr =  ftMeasErrV;
        this->fillVolatileCl(itClY); const double yRaw = ftMeasV; yErr =  ftMeasErrV;
        xCorr = xRaw - 
	  (yRaw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, iSensorX))*fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, iSensorX);
        yCorr = yRaw - 
	 (xRaw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 2, iSensorY))*fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, iSensorY);
	// We do not include the U measurement in the estimate of X and Y.. Too convolved.. but not optimum. 
	fXSts[kSt-2].first = xCorr; fYSts[kSt-2].first = yCorr; // O.K. for Phase1b and Phase1c, only 2 stations in front of target..
	fXSts[kSt-2].second = xErr; fYSts[kSt-2].second = yErr;
      }
      if (nData == 3) {
        fNDGF++;
	double deltaSq=0.;
	if (hasViewU) { 
          this->fillVolatileCl(itClU); const double uRaw = ftMeasV; uErr =  ftMeasErrV;
	  const double wEff = fOneOverSqrt2*(xCorr - yCorr); // rough... 
	  const double uCorr = uRaw - 
	    (wEff - fEmVolAlP->RollCenter(emph::geo::U_VIEW, kSt, 0))*fEmVolAlP->Roll(emph::geo::U_VIEW, kSt, 0);
	  dd = (uCorr - fOneOverSqrt2*(xCorr + yCorr));
	} else if (hasViewW) {   
          this->fillVolatileCl(itClW); const double wRaw = ftMeasV; wErr =  ftMeasErrV;
	  const double uEff = fOneOverSqrt2*(xCorr + yCorr); // rough... 
	  const double wCorr = wRaw - 
	    (uEff - fEmVolAlP->RollCenter(emph::geo::W_VIEW, kSt, 0))*fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, 0);
	  dd = (wCorr - fOneOverSqrt2*(xCorr - yCorr));
	}  
	deltaSq=dd*dd;
	fChiSqSts[kSt] += deltaSq/(uErr*uErr + wErr*wErr + 0.5*(xErr*xErr + yErr*yErr));
      } else {
        if (hasViewX && hasViewU) {
          this->fillVolatileCl(itClU); const double uRaw = ftMeasV; uErr =  ftMeasErrV;
          this->fillVolatileCl(itClX); const double xRaw = ftMeasV; xErr =  ftMeasErrV;
	  const double yRaw = (1.0/fOneOverSqrt2) *uRaw - xRaw;
          const double xCorr = xRaw - 
	    (yRaw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, iSensorX))*fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, iSensorX);
          const double yCorr = yRaw - 
	   (xRaw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 2, iSensorY))*fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, iSensorY);
	  fXSts[kSt-2].first = xCorr; fYSts[kSt-2].first = yCorr; // O.K. for Phase1b and Phase1c, only 2 stations in front of target..
          fXSts[kSt-2].second = xErr; fYSts[kSt-2].second = std::sqrt(2.0*uErr*uErr + xErr*xErr); 
	} else if (hasViewX && hasViewW) {
          this->fillVolatileCl(itClW); const double wRaw = ftMeasV; wErr =  ftMeasErrV;
          this->fillVolatileCl(itClX); const double xRaw = ftMeasV; xErr =  ftMeasErrV;
	  const double yRaw = (-1.0/fOneOverSqrt2) *wRaw + xRaw;
          const double xCorr = xRaw - 
	    (yRaw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, iSensorX))*fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, iSensorX);
          const double yCorr = yRaw - 
	   (xRaw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 2, iSensorY))*fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, iSensorY);
	  fXSts[kSt-2].first = xCorr; fYSts[kSt-2].first = yCorr; // O.K. for Phase1b and Phase1c, only 2 stations in front of target..
          fXSts[kSt-2].second = xErr; fYSts[kSt-2].second = std::sqrt(2.0*wErr*wErr + xErr*xErr); 
 	} else if (hasViewY && hasViewU) {
          this->fillVolatileCl(itClU); const double uRaw = ftMeasV;  uErr =  ftMeasErrV;
          this->fillVolatileCl(itClX); const double yRaw = ftMeasV;  yErr =  ftMeasErrV;
	  const double xRaw = (1.0/fOneOverSqrt2) *uRaw - yRaw;
          const double xCorr = xRaw - 
	    (yRaw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, iSensorX))*fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, iSensorX);
          const double yCorr = yRaw - 
	   (xRaw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 2, iSensorY))*fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, iSensorY);
	  fXSts[kSt-2].first = xCorr; fYSts[kSt-2].first = yCorr; // O.K. for Phase1b and Phase1c, only 2 stations in front of target..
          fXSts[kSt-2].second = xErr; fYSts[kSt-2].second = std::sqrt(2.0*uErr*uErr + yErr*yErr); 
	} else if (hasViewY && hasViewW) {
          this->fillVolatileCl(itClW); const double wRaw = ftMeasV; wErr =  ftMeasErrV;
          this->fillVolatileCl(itClX); const double yRaw = ftMeasV; yErr =  ftMeasErrV;
	  const double xRaw = (1.0/fOneOverSqrt2) *wRaw + yRaw;
          const double xCorr = xRaw - 
	    (yRaw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, iSensorX))*fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, iSensorX);
          const double yCorr = yRaw - 
	   (xRaw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 2, iSensorY))*fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, iSensorY);
	  fXSts[kSt-2].first = xCorr; fYSts[kSt-2].first = yCorr; // O.K. for Phase1b and Phase1c, only 2 stations in front of target..
          fXSts[kSt-2].second = xErr; fYSts[kSt-2].second = std::sqrt(2.0*wErr*wErr + yErr*yErr); 
	} // a bit too many repeated nearly identical mantras.. Well, it coded! 
      }
      return true;
    }
//
//  
    double SSD3DTrackKlmFitFCNAlgo1::propagateStsNext(size_t kStStart, size_t kStEnd, double p) const {
      int nIter = 0;
      double deltaXSl = DBL_MAX;
      double chiSq = DBL_MAX;
      // Compute the slopes at station 3, linear propagation. 
      const double deltaZ = fEmVolAlP->ZPos(emph::geo::X_VIEW, kStEnd) - fEmVolAlP->ZPos(emph::geo::X_VIEW, kStStart);
      const double zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, kStStart);
      const double zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, kStEnd);
      double slx = (kStStart == 2) ? (fXSts[kStEnd-2].first - fXSts[kStStart-2].first)/deltaZ :
                                      fSlXSts[kStStart-2].first;
      double sly = (kStStart == 2) ? (fYSts[kStEnd-2].first - fYSts[kStStart-2].first)/deltaZ :
                                      fSlYSts[kStStart-2].first;
      const double slxInit = slx;  const double slyInit = sly;			      
      if (fDebugIsOn) std::cerr << "Upstream Fringe Field zLocUpstreamMagnet " <<  zLocUpstreamMagnet 
				<< " Downstream " << zLocDownstrMagnet << " slx " << slx << " sly " << std::endl;
      double xPredStNext = DBL_MAX; double yPredStNext = DBL_MAX;
      int nMax = (kStStart == 3) ? 2*fNumMaxPropIter : fNumMaxPropIter; // arbitrary.. Phase1b. 
      while ((nIter < nMax) && (deltaXSl > fDeltaXSlopeMinChange)) { 
        const int Q = (p > 0.) ? 1 : -1 ;  // hopefully correct.. 
        const double stepAlongZ = fIntegrationStep; // such we don't introduce suspicious correlations.. 
        std::vector<double> startMag(6, 0.); std::vector<double> endMag(6, 0.); 
        startMag[0] = fXSts[kStStart-2].first; // assume station 2 is the origin. 
        startMag[1] = fYSts[kStStart-2].first;;  
        startMag[2] = zLocUpstreamMagnet; 
        startMag[3] = slx*p; // 
        startMag[4] = sly*p; // assume small slope, sin(theta) = theta.. 
        startMag[5] = std::abs(p) * std::sqrt(1.0 - slx*slx - sly*sly); 
        endMag[2] = zLocDownstrMagnet; 
	// 
	// Implement Magnet misalignment.. 
	startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
        fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	//
	// Back shift.. 
	endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
        const double slxNew = endMag[3]/p; sly = endMag[4]/p;
	deltaXSl = std::abs(slxNew - slx); slx = slxNew; 
        xPredStNext = endMag[0];  yPredStNext = endMag[1];
	// correction for the small difference of X and Y planes. 
	double ddZXY = fEmVolAlP->ZPos(emph::geo::Y_VIEW, kStEnd) - fEmVolAlP->ZPos(emph::geo::X_VIEW, kStEnd); 
	yPredStNext += ddZXY*sly;  
        if (fDebugIsOn) 
	  std::cerr << " ..... After the Usptream X and Y Integrated predictions from Station "  
		   << kStStart << " to Station " << kStEnd << ", xPred " 
	 << xPredStNext << " yPred " <<  yPredStNext << " X Slope " << slx << " Y slope " << sly << std::endl; 
       }
 	 // 
       const double errPosXaSq = fXSts[kStStart-2].second * fXSts[kStStart-2].second  
        			 + fMultScatterOneStationSq*(120./fExpectedMomentum)*(120./fExpectedMomentum);
       const double errPosYaSq = fXSts[kStStart-2].second * fXSts[kStStart-2].second  
        			 + fMultScatterOneStationSq*(120./fExpectedMomentum)*(120./fExpectedMomentum);
       const double errPosXbSq = fXSts[kStEnd-2].second * fXSts[kStEnd-2].second;  
       const double errPosYbSq =  fYSts[kStEnd-2].second * fYSts[kStEnd-2].second; 
       double weightPosXa = (1.0/errPosXaSq); double weightPosXb = 1.0/errPosXbSq;
       const double sumWX = weightPosXa + weightPosXb; weightPosXa /= sumWX;  weightPosXb /= sumWX;
       double weightPosYa = (1.0/errPosYaSq); double weightPosYb = 1.0/errPosYbSq;
       const double sumWY = weightPosYa + weightPosYb; weightPosYa /= sumWY;  weightPosYb /= sumWY;
       const double chiSqX = ((xPredStNext - fXSts[kStEnd-2].first) * (xPredStNext - fXSts[kStEnd-2].first))*sumWX;
       const double chiSqY = ((yPredStNext - fYSts[kStEnd-2].first) * (yPredStNext - fYSts[kStEnd-2].first))*sumWY;
       chiSq = chiSqX + chiSqY; fNDGF += 2;
       fXSts[kStEnd-2].first = xPredStNext*weightPosXa + weightPosXb*fXSts[kStEnd-2].first;
       fYSts[kStEnd-2].first = yPredStNext*weightPosYa + weightPosYb*fYSts[kStEnd-2].first;
       fXSts[kStEnd-2].second = std::sqrt((1.0/sumWX)/2.); // approximate 
       fYSts[kStEnd-2].second = std::sqrt((1.0/sumWY)/2.); // approximate 
       fSlXSts[kStEnd-2].first = slx; // Obvious;; 	 
       fSlYSts[kStEnd-2].first = sly;
       if (kStStart == 2) {
         //
         // Now the slopes.. We refine the slopes at station 2, as they are the second ingredient to the scattering angle. 
	 // Linearize... Assume no uncertainties coming from the magnet. Only fringe field at station 3. 
	 // Now that we have a better position at Station3,  redefine the slope 
	 const double deltaMagSlx = slx - slxInit; const double deltaMagSly = sly - slyInit;
	 const double slxEndNoMag = (fXSts[kStEnd-2].first - fXSts[kStStart-2].first)/deltaZ;
	 const double slyEndNoMag = (fYSts[kStEnd-2].first - fYSts[kStStart-2].first)/deltaZ;
	 fSlXSts[kStStart-2].first = slxEndNoMag - deltaMagSlx; 
	 fSlYSts[kStStart-2].first = slyEndNoMag - deltaMagSly; 
	 // uncertainties for the slope at Station 2. 
	 fSlXSts[kStEnd-2].second = std::sqrt(1.0/sumWX)/deltaZ; // Obvious, but neglect uncertainty in Z...  	 
	 fSlYSts[kStEnd-2].second = std::sqrt(1.0/sumWY)/deltaZ;;
       }
       return chiSq; 
    }
//    
    void SSD3DTrackKlmFitFCNAlgo1::fillVolatileCl(std::vector<rb::SSDCluster>::const_iterator it) const {  
 	  fkStV = it->Station();
 	  fkSeV = it->Sensor();
	  ftMeasV = fMyConvert.getTrCoord(it, fExpectedMomentum).first; 
//	  ftMeasErrV = std::sqrt(fMyConvert.getTrCoord(it, fExpectedMomentum).second;  No ! It includes the wrong estimate of multiple Scattering 
          const double rmsStr = std::max(0.1, it->WgtRmsStrip()); // protect against some zero values for the RMS 
	  const double rmsStrN = rmsStr/fOneOSqrt12;
	  const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!  Suspicious.. 
//	  ftMeasErrV = std::sqrt(stripErrSq + fMultScatterOneStationSq*(120./fExpectedMomentum)*(120./fExpectedMomentum)); 
	  ftMeasErrV = std::sqrt(stripErrSq); // We will add the multiple scattering uncertainty in the propagator. 
	  emph::geo::sensorView aView =  it->View();
	  fzV = fEmVolAlP->ZPos(aView, fkStV, fkSeV);
    }
  } // namespace 
} // Name space emph  
 
