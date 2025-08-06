////////////////////////////////////////////////////////////////////////
/// \brief   3D linear fit FCN function for Beam Track alignment. Or preliminary Physics Studies.    
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

#include "SSDReco/SSDDwnstrTrackFitFCNAlgo1.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

// emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr; defined elsewhere 

namespace emph {
  namespace ssdr {
    
    SSDDwnstrTrackFitFCNAlgo1::SSDDwnstrTrackFitFCNAlgo1(int runNum) :
    FCNBase(),
    fRunNum(runNum),
    fEmgeo(fGeoService->Geo()),
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()),
    fMagField(nullptr), fPhase1c(true), fIsMC(false), fDebugIsOn(false), fMaxDwnstrStation(6), fIntegrationStep(20.0), 
    // last parameter might need some tuning.. This is the value at 120 GeV.
    fStartingMomentum(30.), // expected momentum, used to compute the mult. scattering uncertainty. 
    fNumSensorsTotal(2*fEmVolAlP->NumSensorsXorY() -4 + fEmVolAlP->NumSensorsU() + fEmVolAlP->NumSensorsV()),
    fData(), fNoMagnet(false), fZPos(), fMagShift(3, 0.),
    fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), 
    fOneOSqrt12(std::sqrt(1.0/12.)), fResids(12, DBL_MAX), 
    fZLocUpstreamMagnet(DBL_MAX), fZLocDownstrMagnet(DBL_MAX) {
     
       art::ServiceHandle<emph::MagneticFieldService> bField;
       fMagField = bField->Field();
       art::ServiceHandle<emph::geo::GeometryService> geo;
       fNoMagnet = geo->Geo()->MagnetUSZPos() < 0.;
       std::cerr << " SSDDwnstrTrackFitFCNAlgo1::SSDDwnstrTrackFitFCNAlgo1, Magnet MagnetUSZPos() " <<  geo->Geo()->MagnetUSZPos() << std::endl;
       fPhase1c = (runNum > 1999) ? true : false;
//
    }
    SSDDwnstrTrackFitFCNAlgo1::~SSDDwnstrTrackFitFCNAlgo1() {
      if (fFOutResids.is_open()) fFOutResids.close();
    } 
    void SSDDwnstrTrackFitFCNAlgo1::AddInputUpstreamStation(size_t kSt, double xVal, double xValErr, double yVal, double yValErr) {
      rb::SSDStationPtAlgo1 aPt;
      aPt.SetStationNum(static_cast<int>(kSt)); 
      aPt.SetID(9999); aPt.SetX(xVal, xValErr); aPt.SetY(yVal, yValErr);
      fData.push_back(aPt);
    } 
    double SSDDwnstrTrackFitFCNAlgo1::operator()(const std::vector<double> &pars) const {
      
      if (fDebugIsOn) std::cerr << "SSDDwnstrTrackFitFCNAlgo1::operator, Start, number of track parameters " 
                                << pars.size() << " Number of data pts " << fData.size() << std::endl;  
      if (fDebugIsOn && fNoMagnet) std::cerr << "  .....   The Magnet has been removed.. " << std::endl;
      if (fDebugIsOn) {
         if (fPhase1c) std::cerr << "  .....   Assuming Phase1c geometry .. " << std::endl;
	 else std::cerr << "  .....   Assuming Phase1b geometry .. " << std::endl;
      }
      if (fData.size() <  3) return 2.0e10; // require at least 3 SSD Space Points 
      if ((!fNoMagnet) && (std::abs(1.0/pars[4]) < 0.1)) return 5.0e10; // low momentum cut offf.. To be adjested later on. 
       // if Mininuit attemps to change the sign of momentum, or jumps too far, assign  a very large chi-square. 
      if (fDebugIsOn) std::cerr << " ... Assume Monte Carlo sign conventions Expected value for momentum  " << fStartingMomentum << std::endl; 
      if (fNoMagnet) {  
	 assert(pars.size() == 4);
      } else {
	 assert(pars.size() == 5);
      }
      const double zMag = fEmVolAlP->ZCoordsMagnetCenter();
      const double kick = fEmVolAlP->MagnetKick120GeV(); // only for debugging, we use the magnetic field intgrator. 
      double chi2 = 0.;
      const double x0 = pars[0]; 
      const double slx0 = pars[1];
      const double y0 = pars[2]; 
      const double sly0 = pars[3];
      double pMomTmp = (1.0/pars[4]); 
      
      const bool neglectFringeFieldUp = false; 
      const bool neglectFringeFieldDown = false; 
      if (fDebugIsOn) {
         std::cerr << "... x0 " << x0 << " slx0 " << slx0 << " y0 " << y0 << " sly0 " << sly0;
	 if (pars.size() == 5) std::cerr << " pMom " << 1.0/pars[4]; 
         std::cerr  << std::endl; 
      }
      // check the input
      std::vector<double> xPredAtSt(8, 0.); // Phase1b or Phase1c  Over dimensioned 
      std::vector<double> yPredAtSt(8, 0.);
      const size_t numStationLast = (fPhase1c) ? 6 : 5; // In Phase1c, Station 7 is not aligned.. 
      for (size_t kSt=2; kSt != numStationLast; kSt++) { // start a station 2., Phase1b 
        size_t kSe = (kSt > 4) ? (5 + (kSt-5)*2 ) : kSt; // In phase1c, z Position are identical for sensors 5 and 6 , same with 6 == 7 
        if (!fPhase1c) kSe = (kSt > 3) ? (4 + (kSt-4)*2 ) : kSt; // In phase1b, z Position are identical for sensors 4 and 5 , same with 6 == 7 	
        xPredAtSt[kSt-2] = x0 + slx0*(fEmVolAlP->ZPos(emph::geo::X_VIEW, kSt, kSe) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2));
        yPredAtSt[kSt-2] = y0 + sly0*(fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt, kSe) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 2));
        if (fDebugIsOn) 
	  std::cerr << " ..... Before the magnet, X and Y predictions for  kSt " << kSt << " kSe " << kSe 
	  	      << " xPred " << xPredAtSt[kSt-2] << " yPred " <<  yPredAtSt[kSt-2] << " ZYKst " 
		      << fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt, kSe) << std::endl; 
      }
      if (!fNoMagnet) {	
        if (fIntegrationStep > 1.0e-6) {
          // Compute the expected kick, define intercepts and slopes downstream of the magnet, based on the Magneticfield integrator class. 
	  // Take into account the effect of the fringe field.. 
          const int Q = (pars[4] > 0.) ? 1 : -1 ; // Let us flipt the sign of the charge.. The X coordinate has been flipped, perhaps... 
//         const double stepAlongZ = fIntegrationStep * (std::abs(pars[4])/120.);
          const double stepAlongZ = fIntegrationStep; // such we don't introduce suspicious correlations.. 
          std::vector<double> startMag(6, 0.); std::vector<double> endMag(6, 0.); double slx1, sly1;
	  double zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 3);  
	  double zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);   // In Phase1c, station 4 is in front of the magnet. 
	  size_t iiLast = (fPhase1c) ? 2 : 1;
	  if (!fPhase1c) { 
	    zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
	    zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 3); 
	  }
	  if (!neglectFringeFieldUp) { 
	    if (fDebugIsOn) std::cerr << "Upstream Fringe Field zLocUpstreamMagnet " <<  zLocUpstreamMagnet 
	                            << " Downstream " << zLocDownstrMagnet <<  " end Station Index " << iiLast << std::endl;
            startMag[0] = xPredAtSt[iiLast-1]; // assume station 2 is the origin. But, for Phase1c, the upstream fringer fiels starts at stion 3,  
            startMag[1] = yPredAtSt[iiLast-1]; 
            startMag[2] = zLocUpstreamMagnet; 
            startMag[3] = slx0*pMomTmp; // 
            startMag[4] = sly0*pMomTmp; // assume small slope, sin(theta) = theta.. 
            startMag[5] = std::abs(pMomTmp) * std::sqrt(1.0 - slx0*slx0 - sly0*sly0); 
            endMag[2] = zLocDownstrMagnet; 
	    // 
	    // Implement Magnet misalignment.. 
	    startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
	    fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	    // function removed
            //fMagField->IntegrateSt3toSt4(0, Q, startMag, endMag);
	    //
	    // Back shift.. 
	    endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
            slx1 = endMag[3]/pMomTmp; sly1 = endMag[4]/pMomTmp; 
            xPredAtSt[iiLast] = endMag[0]; yPredAtSt[iiLast] = endMag[1];
	   
	   // correction for the small difference of X and Y planes. 
	    double ddZXY = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 4) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 4); 
	    if (!fPhase1c) ddZXY = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 3) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 3); 
	    yPredAtSt[iiLast] += ddZXY*sly1;  
            if (fDebugIsOn) 
	      std::cerr << " ..... After the Usptream fringeX and Y,  from Z= " 
	     << zLocUpstreamMagnet << " to Z = " << zLocDownstrMagnet << " ,  xPred " 
	     << xPredAtSt[iiLast] << " yPred " <<  yPredAtSt[iiLast] 
	     << " X' " << 1000.0*slx1 << " Y' " << 1000.0*sly1 << " mrad"  << std::endl; 
            if (fDebugIsOn) 
	      std::cerr << " ..... .... Shift in Y vs X view corr "  <<  ddZXY*sly1 << " ddZXY " << ddZXY << std::endl;
	  } else {
	     slx1 = slx0; sly1 = sly0;
	  }
	  //
	  // The Magnet itself, between station 3 and station 4. (Phase1b) 
	  // In Phase1c, between station 4 and station 5.
	  //
	  zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);
	  zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 5);
	  if (!fPhase1c) {
	    zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 3);
	    zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);
	  }
	  iiLast++;
	  if (fDebugIsOn) std::cerr << " Magnet itself zLocUpstreamMagnet " 
	                            <<  zLocUpstreamMagnet << " Downstream " << zLocDownstrMagnet 
				    <<  " slx1 " << 1000.0*slx1 <<  " sly1 " << 1000.0*sly1 << " mrad, iiLast " << iiLast <<  std::endl;
          startMag[0] = xPredAtSt[iiLast-1]; // restart, from station  
          startMag[1] = yPredAtSt[iiLast-1]; // assume station 0 is the origin. 
          startMag[2] = zLocUpstreamMagnet; 
          startMag[3] = slx1*pMomTmp; // 
          startMag[4] = sly1*pMomTmp;; // assume small slope, sin(theta) = theta.. 
          startMag[5] = std::abs(pMomTmp) * std::sqrt(1.0 - slx1*slx1 - sly1*sly1);
	  endMag[2] = zLocDownstrMagnet; 
 	  startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
	  fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
          //fMagField->IntegrateSt4toSt5(0, Q, startMag, endMag);
	  endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
          double slx2 = endMag[3]/pMomTmp; double sly2 = endMag[4]/pMomTmp; 
	  
          xPredAtSt[iiLast] = endMag[0]; yPredAtSt[iiLast] = endMag[1];
	  // correction for the small difference of X and Y planes. 
	  
	  double ddZXYC = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 5);
	  if (!fPhase1c)
	     ddZXYC = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 4) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);
	  yPredAtSt[iiLast] +=  ddZXYC*sly2;
          if (fDebugIsOn) 
	     std::cerr << " ..... After the magnet X and Y Integrated predictions, from Z = "
	               << zLocUpstreamMagnet << " to Z = " << zLocDownstrMagnet << " , xPred " 
	               << xPredAtSt[iiLast] << " yPred " <<  yPredAtSt[iiLast] 
		      << " X Slope " << 1000.*slx2 << " delta xSl " << 1.0e3*(slx2-slx1) << 
		      " mrad, Y Slope " << 1000.*sly2 << " mrad" <<  std::endl; 
            if (fDebugIsOn) 
	      std::cerr << " ..... .... Shift in Y vs X view corr "  <<  ddZXYC*sly2 << " ddZXYC " << ddZXYC << std::endl;
	  //
	  // Downstream of the magnet 
	  //
	  if (!neglectFringeFieldDown) { 
	    	      
	    zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 5);
	    zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 6);
	    if (!fPhase1c) {
	      zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);
	      zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 5);
	    }
	    iiLast++;
	    if (fDebugIsOn) std::cerr << " Downstream Fringe zLocUpstreamMagnet " 
	                            <<  zLocUpstreamMagnet << " Downstream " << zLocDownstrMagnet 
				    <<  " slx2 " << 1000.0*slx2 <<  " sly2 " << 1000.0*sly2 << " mrad " <<  std::endl;
            startMag[0] = xPredAtSt[iiLast-1];  
            startMag[1] = yPredAtSt[iiLast-1]; 
            startMag[2] = zLocUpstreamMagnet; 
            startMag[3] = slx2*pMomTmp; // 
            startMag[4] = sly2*pMomTmp;; // assume small slope, sin(theta) = theta.. 
            startMag[5] = std::abs(pMomTmp) * std::sqrt(1.0 - slx2*slx2 - sly2*sly2);
	    endMag[2] = zLocDownstrMagnet; 
 	    startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
            fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
            //fMagField->IntegrateSt5toSt6(0, Q, startMag, endMag);
	    endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
            double slx3 = endMag[3]/pMomTmp; double sly3 = endMag[4]/pMomTmp;
            xPredAtSt[iiLast] = endMag[0]; yPredAtSt[iiLast] = endMag[1];
	  // correction for the small difference of X and Y planes. 
	    double ddZXYE = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 6) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 6); 
	    if (!fPhase1c) ddZXYE = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 5); 
	    yPredAtSt[iiLast] +=  ddZXYE*sly3;
            if (fDebugIsOn) 
	       std::cerr << " ..... After the Downstream fringe field, xPred " << xPredAtSt[iiLast] << " yPred " <<  yPredAtSt[iiLast] 
		      << " X Slope " << 1000.*slx3 << " Y Slope " << 1000.0*sly3 <<  std::endl; 
	 } else {
//	   xPredAtSt[3] = xPredAtSt[2] + slx2*(fEmVolAlP->ZPos(emph::geo::X_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 4)); 
//	   yPredAtSt[3] = yPredAtSt[2] + sly2*(fEmVolAlP->ZPos(emph::geo::Y_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 4)); 
           if (fDebugIsOn) {
	       std::cerr << " .....Negecting downstream fringe field,  after the magnet, xPred " << xPredAtSt[3] << " yPred " <<  yPredAtSt[3] 
		      << " X Slope " << 1000.0*slx2 << " Y Slope " << 1000.0*sly2 <<  std::endl; 		      
           }
	 }
       } else { // simple kick.. 
         size_t iiSimpleKickStart = (fPhase1c) ? 4 : 3;
	 const double slx1 =  slx0 + kick;
         for (size_t kSt=iiSimpleKickStart; kSt != iiSimpleKickStart+2; kSt++) {
	   const double xx = x0 + slx0*(zMag - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0));
           xPredAtSt[kSt-2] = xx + slx1*( fEmVolAlP->ZPos(emph::geo::X_VIEW, kSt) - zMag);
           yPredAtSt[kSt-2] = y0 + sly0*(fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 0));
           if (fDebugIsOn) 
	     std::cerr << " ..... After the magnet X and Y predictions, simple kick for  kSe " << kSt 
	  	      << " kick " << kick << " xPred " << xPredAtSt[kSt-2] << " yPred " <<  yPredAtSt[kSt-2] << std::endl; 
          }	
        }
      } // With or without the magnet..  
      for (size_t kSe=0; kSe != fResids.size(); kSe++) fResids[kSe] = DBL_MAX;  
      size_t kD = 0;
      for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itPt = fData.cbegin(); itPt != fData.cend(); itPt++, kD++) {
        if ((itPt->Station() < 2) || (itPt->Station() > fMaxDwnstrStation)) continue; // January 22 
        size_t kStD = itPt->Station()-2; 
        const double xMeas = itPt->X(); const double xErr = itPt->XErr();
        const double dx = xMeas - xPredAtSt[kStD];
	fResids[kStD] = dx;
        chi2 += dx*dx / (xErr*xErr);
        const double yMeas = itPt->Y(); const double yErr = itPt->YErr();
        const double dy = yMeas - yPredAtSt[kStD]; 
	fResids[kStD+6] = dy; // Numbering only valid only for Phase1c.. 
	if (fDebugIsOn) std::cerr  << " ... ... At Station " << itPt->Station() << " xMeas " << xMeas << " yMeas " << yMeas 
	                           << " xPred " << xPredAtSt[kStD] << " yPred " << yPredAtSt[kStD] 
	                           << " dx " << dx << " +- " << xErr << " dy " << dy << " +- " << yErr << " chi2 " << chi2 << std::endl;
        chi2 += dy*dy / (yErr*yErr);
       }  
      if (fDebugIsOn) { std::cerr << " ......Chi Sq is " << chi2 << " And we keep going....  " << std::endl << std::endl; }
//      if (fDebugIsOn) { std::cerr << " ......Chi Sq is " << chi2 << " And we stop here...  " << std::endl; exit(2);}
      fLastChi2 = chi2;
      return chi2;
    }
    void SSDDwnstrTrackFitFCNAlgo1::OpenOutResids(const std::string &fNameStr) {
      if (fFOutResids.is_open()) return;
      fFOutResids.open(fNameStr.c_str());
      fFOutResids << " spill evt chiSq";
      for (size_t k=0; k != 4; k++)  fFOutResids << " residXSt" << k;
      for (size_t k=0; k != 4; k++)  fFOutResids << " residYSt" << k;
      fFOutResids << " " << std::endl;   
    } 
    void SSDDwnstrTrackFitFCNAlgo1::SpitOutResids(int spill, int evtNum) {
      if (!fFOutResids.is_open()) return;
      fFOutResids << " " << spill << " " << evtNum << " " << fLastChi2;
      for (size_t k=0; k != fResids.size(); k++) 
         fFOutResids << " " << fResids[k];
      fFOutResids << " " << std::endl;   
    }
    void SSDDwnstrTrackFitFCNAlgo1::printInputData() const { 
      std::cerr << " SSDDwnstrTrackFitFCNAlgo1::printInputData, number of data points " << fData.size() << std::endl;
      for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itPt = fData.cbegin(); itPt != fData.cend(); itPt++) {
        std::cerr << " Station " << itPt->Station() << " X = " << itPt->X() << " +- " 
	                                            << itPt->XErr() << " Y = " << itPt->Y() << " +- " << itPt->YErr() << std::endl; 
      }
      std::cerr << std::endl;
    }
  } // namespace 
} // Name space emph  
 
