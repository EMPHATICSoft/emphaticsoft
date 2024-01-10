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

#include "BeamTrack3DFCN.h"
#include "BTMagneticField.h"

extern emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr;
extern emph::rbal::BTMagneticField* emph::rbal::BTMagneticField::instancePtr;

namespace emph {
  namespace rbal {
  
    BeamTrack3DFCN::inDatafDebug::inDatafDebug(int kSt, char aView, int aSensor, double aStrip, double rollCorr,  double tM, double aPred) :
        fStation(kSt), fView(aView), fSensor(aSensor), fStrip(aStrip), fRollCorr(rollCorr), fTMeas(tM),  fTPred(aPred) { ; }  
 
    
    
    BeamTrack3DFCN::BeamTrack3DFCN() :
    myGeo(emph::rbal::BTAlignGeom::getInstance()), 
    myGeo1c(emph::rbal::BTAlignGeom1c::getInstance()),
    fIsPhase1c(false), 
    fIsMC(true), fNoMagnet(false), fDebugIsOn(false), fSelectedView('A'),
    fNumSensorsTotal(2*myGeo->NumSensorsXorY() + myGeo->NumSensorsU() + myGeo->NumSensorsV()),
    FCNBase(),
    fItCl(NULL), fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), fNominalMomentum(120.),
    fOneOSqrt12(std::sqrt(1.0/12.)), fMaxAngleLin(0.015), 
    fNCalls(0), fResids(fNumSensorsTotal, DBL_MAX), fMagField(nullptr) { 
      ;  
    }  
      
    double BeamTrack3DFCN::operator()(const std::vector<double> &pars) const {

       for (size_t kPar = 0; kPar != pars.size(); kPar++) {
//        if (std::isnan(pars[kPar])) {
//	   std::cerr << " BeamTrack3DFCN::operator(), Nan found at kPar " << kPar <<  " quit here and now " << std::endl; 
//	   exit(2); 
//	}
        if (std::isnan(pars[kPar])) return 2.0e9;
      }
    
    
//    const bool debugIsOn = fNCalls < 150;
      const bool debugIsOn = fDebugIsOn;
      const bool neglectFringeFieldUp = false;
//      const bool neglectFringeFieldDown = true;
// 06i, d2d sequence instead of c2d      
      const bool neglectFringeFieldDown = false;
//      const bool debugIsOn = ((pars[0] > 0.25) && (pars[2] < -0.25)); 
      if (debugIsOn) {
         std::cerr << "BeamTrack3DFCN::operator, number of track parameters " << pars.size() << std::endl; 
	 fInDataDbg.clear();
      } 
      const double integrationStepSize = myGeo->IntegrationStepSize();
      if (!fNoMagnet) {
        // If we bother to integrate the field for a given 4D phase space space at station 0, might as well fit the momentum
	// hence, we must have 5 parameters.  
        assert(pars.size() == 5);
	if (std::abs(pars[4]) < 1.0) return 1.0e6; // one GeV minimum 
      } else {   
        assert(pars.size() == 4);
      }
//       std::cerr << " BeamTrack3DFCN::operator(), begin, x0 " << pars[0] << std::endl;
      const double zMag = myGeo->ZCoordsMagnetCenter();
      const double kick = (pars.size() == 4) ? myGeo->MagnetKick120GeV() : myGeo->MagnetKick120GeV()*120.0/fNominalMomentum;
      double chi2 = 0.;
      const double x0 = pars[0]; 
      const double slx0 = pars[1];
      const double y0 = pars[2]; 
      const double sly0 = pars[3];
      if (debugIsOn) {
         std::cerr << "... x0 " << x0 << " slx0 " << slx0 << " y0 " << y0 << " sly0 " << sly0;
	 if (pars.size() == 5) std::cerr << " pMom " << pars[4]; 
	 if (!fIsPhase1c) { std::cerr << " ... ... Expecting Phase1c.. Not the case.. If so, stop here and now.. " << std::endl; exit(2); }
         std::cerr  << std::endl; 
      }
      const size_t nnXYTmp = (fIsPhase1c) ? myGeo1c->NumSensorsXorY() : myGeo->NumSensorsXorY();
      std::vector<double> xPredAtSt(nnXYTmp, 0.);
      std::vector<double> yPredAtSt(nnXYTmp, 0.);
      size_t numSensorInStraightLine = INT_MAX;
      if (!fIsPhase1c) { 
         numSensorInStraightLine = (fNoMagnet) ? myGeo->NumSensorsXorY() : 4; // Phase1b 
      } else {
         numSensorInStraightLine = (fNoMagnet) ? myGeo1c->NumSensorsXorY() : 5; // Phase1c 
      }   
      for (size_t kSt=0; kSt != numSensorInStraightLine; kSt++) { 
        if (fIsPhase1c) { 
          xPredAtSt[kSt] = x0 + slx0*(myGeo1c->ZPos('X', kSt) - myGeo1c->ZPos('X', 0));
          yPredAtSt[kSt] = y0 + sly0*(myGeo1c->ZPos('Y', kSt) - myGeo1c->ZPos('Y', 0));
          if (debugIsOn) 
	     std::cerr << " ..... Before the magnet, X and Y predictions for  kSe " << kSt 
	  	      << " xPred " << xPredAtSt[kSt] << " yPred " <<  yPredAtSt[kSt] 
		      << " ZYKst " << myGeo1c->ZPos('Y', kSt) << " ZXKst " << myGeo1c->ZPos('X', kSt) << std::endl; 
        } else {
          xPredAtSt[kSt] = x0 + slx0*(myGeo1c->ZPos('X', kSt) - myGeo1c->ZPos('X', 0));
          yPredAtSt[kSt] = y0 + sly0*(myGeo1c->ZPos('Y', kSt) - myGeo1c->ZPos('Y', 0));
          if (debugIsOn) 
	    std::cerr << " ..... Before the magnet, X and Y predictions for  kSe " << kSt 
	  	      << " xPred " << xPredAtSt[kSt] << " yPred " <<  yPredAtSt[kSt] 
		      << " ZYKst " << myGeo->ZPos('Y', kSt) << " ZXKst " << myGeo->ZPos('X', kSt) << std::endl; 
	}
      }
      if (!fNoMagnet) { 	
        if (integrationStepSize > 1.0e-6) {
	  if (fIsPhase1c) { std::cerr << " Not available for Phase1c... " << std::endl; exit(2); }
         // Compute the expected kick, define intercepts and slopes downstream of the magnet, based on the Magneticfield integrator class. 
	 // Take into account the effect of the fringe field.. 
	  if (fMagField == nullptr) fMagField = emph::rbal::BTMagneticField::getInstance();
//          const int Q = (pars[4] > 0.) ? -1 : 1 ; // Let us flipt the sign of the charge.. The X coordinate has been flipped, perhaps... 
          const int Q = (pars[4] > 0.) ? 1 : -1 ; // Reverting back, based on run 1274, and letting the momentum float (fAlignMode = true )
	  // Sept 27, series 3f1, 3g1, 3h1  
//         const double stepAlongZ = integrationStepSize * (std::abs(pars[4])/120.);
          const double stepAlongZ = integrationStepSize; // such we don't introduce suspicious correlations.. 
          std::vector<double> startMag(6, 0.); std::vector<double> endMag(6, 0.); 
	  double zLocUpstreamMagnet = myGeo->ZPos('X', 2);
	  double zLocDownstrMagnet = myGeo->ZPos('X', 3); double slx1, sly1;
	  if (!neglectFringeFieldUp) { 
	    if (debugIsOn) std::cerr << "Upstream Fringe Field zLocUpstreamMagnet " <<  zLocUpstreamMagnet 
	                            << " Downstream " << zLocDownstrMagnet << std::endl;
            startMag[0] = x0 + slx0*zLocUpstreamMagnet; // assume station 0 is the origin. 
            startMag[1] = y0 + sly0*zLocUpstreamMagnet; // assume station 0 is the origin. 
            startMag[2] = zLocUpstreamMagnet; 
            startMag[3] = slx0*pars[4]; // 
            startMag[4] = sly0*pars[4]; // assume small slope, sin(theta) = theta.. 
            startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx0*slx0 - sly0*sly0); 
            endMag[2] = zLocDownstrMagnet; 
            fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	    if (!fMagField->didStayedInMap()) return 3.0e9;
            slx1 = endMag[3]/pars[4]; sly1 = endMag[4]/pars[4]; 
            xPredAtSt[3] = endMag[0]; yPredAtSt[3] = endMag[1];
	   // correction for the small difference of X and Y planes. 
	    double ddZXY = myGeo->ZPos('Y', 3) - myGeo->ZPos('X', 3); yPredAtSt[3] += ddZXY*sly1;  
            if (debugIsOn) 
	      std::cerr << " ..... After the Usptream X and Y Integrated predictions for  for Station 3, xPred " 
	      << xPredAtSt[3] << " yPred " <<  yPredAtSt[3] << " X Slope " << slx1 <<  std::endl; 
	  } else {
	    slx1 = slx0; sly1 = sly0;
	  }
	  //
	  // The Magnet itself, between station 3 and station 4. 
	  //
	  zLocUpstreamMagnet = myGeo->ZPos('X', 3);
	  zLocDownstrMagnet = myGeo->ZPos('X', 4);
	  if (debugIsOn) std::cerr << " Magnet itself zLocUpstreamMagnet " <<  zLocUpstreamMagnet << " Downstream " << zLocDownstrMagnet << std::endl;
          startMag[0] = xPredAtSt[3]; // restart, from station  
          startMag[1] = yPredAtSt[3]; // assume station 0 is the origin. 
          startMag[2] = zLocUpstreamMagnet; 
          startMag[3] = slx1*pars[4]; // 
          startMag[4] = sly1*pars[4];; // assume small slope, sin(theta) = theta.. 
          startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx1*slx1 - sly1*sly1);
	  endMag[2] = zLocDownstrMagnet; 
          fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	  if (!fMagField->didStayedInMap()) return 3.0e9;
           double slx2 = endMag[3]/pars[4]; double sly2 = endMag[4]/pars[4]; 
          xPredAtSt[4] = endMag[0]; yPredAtSt[4] = endMag[1]; xPredAtSt[5] = endMag[0]; yPredAtSt[5] = endMag[1];
	  // correction for the small difference of X and Y planes. 
	  const double ddZXYC = myGeo->ZPos('Y', 4) - myGeo->ZPos('X', 4); yPredAtSt[4] +=  ddZXYC*sly2; yPredAtSt[5] +=  ddZXYC*sly2;
          if (debugIsOn) 
	     std::cerr << " ..... After the magnet X and Y Integrated predictions at station 4, xPred " 
	               << xPredAtSt[4] << " yPred " <<  yPredAtSt[4] 
		      << " X Slope " << slx2 << " Y Slope " << sly2 <<  std::endl; 
	  //
	  // Downstream of the magnet 
	  //
	  if (!neglectFringeFieldDown) { 
	    	      
	    zLocUpstreamMagnet = myGeo->ZPos('X', 4);
	    zLocDownstrMagnet = myGeo->ZPos('X', 6);
	    if (debugIsOn) std::cerr << " Downstream fringe  zLocUpstreamMagnet " <<  zLocUpstreamMagnet << " Downstream " << zLocDownstrMagnet << std::endl;
            startMag[0] = xPredAtSt[4]; // assume station 0 is the origin. 
            startMag[1] = yPredAtSt[4]; // assume station 0 is the origin. 
            startMag[2] = zLocUpstreamMagnet; 
            startMag[3] = slx2*pars[4]; // 
            startMag[4] = sly2*pars[4];; // assume small slope, sin(theta) = theta.. 
            startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx2*slx2 - sly2*sly2);
	    endMag[2] = zLocDownstrMagnet; 
            fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	    if (!fMagField->didStayedInMap()) return 3.0e9;
            double slx3 = endMag[3]/pars[4]; double sly3 = endMag[4]/pars[4];
            xPredAtSt[6] = endMag[0]; yPredAtSt[6] = endMag[1]; xPredAtSt[7] = endMag[0]; yPredAtSt[7] = endMag[1];
	  // correction for the small difference of X and Y planes. 
	    const double ddZXYE = myGeo->ZPos('Y', 6) - myGeo->ZPos('X', 6); yPredAtSt[6] +=  ddZXYE*sly3; yPredAtSt[7] +=  ddZXYE*sly3;
            if (debugIsOn) 
	       std::cerr << " ..... After the Downstream fringe field, xPred " << xPredAtSt[6] << " yPred " <<  yPredAtSt[6] 
		      << " X Slope " << slx3 << " Y Slope " << sly3 <<  std::endl; 
	 } else {
	   xPredAtSt[6] = xPredAtSt[4] + slx2*(myGeo->ZPos('X', 6) - myGeo->ZPos('X', 4)); xPredAtSt[7] = xPredAtSt[6];
	   yPredAtSt[6] = yPredAtSt[4] + sly2*(myGeo->ZPos('Y', 6) - myGeo->ZPos('Y', 4)); yPredAtSt[7] = yPredAtSt[6];
           if (debugIsOn) 
	       std::cerr << " ..... After the magnet, xPred " << xPredAtSt[6] << " yPred " <<  yPredAtSt[6] 
		      << " X Slope " << slx2 << " Y Slope " << sly2 <<  std::endl; 
	 }
       } else { // simple kick..
         const size_t nnXYDwnMagnet = (fIsPhase1c) ? 4 : 5; 
         for (size_t kSe=nnXYDwnMagnet; kSe != nnXYTmp; kSe++) {
	   const double slx1 =  slx0 + kick;
	   const double xx = x0 + slx0*(zMag - myGeo->ZPos('X', 0));
           xPredAtSt[kSe] = xx + slx1*( myGeo->ZPos('X', kSe) - zMag);
           yPredAtSt[kSe] = y0 + sly0*(myGeo->ZPos('Y', kSe) - myGeo->ZPos('Y', 0));
          if (debugIsOn) 
	     std::cerr << " ..... After the magnet X and Y predictions, simple kick for  kSe " << kSe 
	  	      << " kick " << kick << " xPred " << xPredAtSt[kSe] << " yPred " <<  yPredAtSt[kSe] << std::endl; 
          }	
        }
      } // NoMagnet 
      if (fIsPhase1c) { fx6 = xPredAtSt[myGeo1c->NumSensorsXorY()-1]; fy6 = yPredAtSt[myGeo1c->NumSensorsXorY()-1]; } 
      else { fx5 = xPredAtSt[myGeo->NumSensorsXorY()-1]; fy5 = xPredAtSt[myGeo->NumSensorsXorY()-1]; }
      std::vector<char> views{'X', 'Y', 'U', 'V'}; // Switch U and V for data!!! Sept 15 2016. 
      size_t kSeT = 0;
      int numMeasurements = 0;
      int numMeasurementsXY = 0;
      int numMeasurementsX = 0;
      for (int iView = 0; iView !=4; iView++) { 
        char aView = views[iView];
	size_t numS = 0;
	if (fIsPhase1c) {  
           numS = myGeo1c->NumSensorsXorY();
	   if (aView == 'U') numS = myGeo1c->NumSensorsU(); 
	   if (aView == 'V') numS = myGeo1c->NumSensorsV();
	} else  { 
           numS = myGeo->NumSensorsXorY();
	   if (aView == 'U') numS = myGeo->NumSensorsU(); 
	   if (aView == 'V') numS = myGeo->NumSensorsV();
	}   
        for (size_t kSe = 0; kSe != numS; kSe++) {
	  fResids[kSeT] = DBL_MAX;
          const double rmsStr = std::max(0.1, fItCl->TheRmsStrip(aView, kSe)); // some very small RMS reappaered.. To be investigated. 
	  if (rmsStr > 1000.) continue; // no measurement. 
	  numMeasurements++; if (iView < 2) numMeasurementsXY++; if (iView == 0) numMeasurementsX++;
        }
      }
      if (debugIsOn) std::cerr << " numMeasurements " << numMeasurements << " X " << numMeasurementsX << " XY " << numMeasurementsXY << std::endl;
//      if ((fSelectedView == 'A') && (numMeasurementsX < 6)) return 2.0e9;  // Relax this, try to recover Station 4 
      if ((fSelectedView == 'Z') && (numMeasurements < 12)) return 2.0e9; // reject track that are not strict 6 + 6, if TrShiftXYOnly (fSelectedView = 'Z'). 
      if (((fSelectedView == 'U') || (fSelectedView == 'W')) && (numMeasurementsXY < 12)) return 2.0e9; // reject track that are not strict 6 + 6, if TrShiftXYOnly (fSelectedView = 'Z'). 
//   The above selection is not valid for Phase1c.. 
      kSeT = 0;
      for (int iView = 0; iView !=4; iView++) { 
        char aView = views[iView];
	if (debugIsOn) std::cerr << "  ... The selected view is " << fSelectedView << std::endl;
	if (fSelectedView != 'A') { // Complicated and perhaps confusing logic... 
	   if (((fSelectedView == 'X') || (fSelectedView == 'Y')) && (aView != fSelectedView)) continue;
	   if ((fSelectedView == 'Z') && (iView > 1)) continue;
	   if ((fSelectedView == 'W') && (iView == 3)) continue; // For TrShiftXYW (reject the Station 4 & 5 stereo views ) 
	   if ((fSelectedView == 'U') && (iView == 2)) continue; // For TrShiftXYU (reject the Station 2 & 3 stereo views ) 
	}   
        size_t numS;
	if (fIsPhase1c) {  
	  numS = myGeo1c->NumSensorsXorY();
	  if (aView == 'U') numS = myGeo1c->NumSensorsU(); 
	  if (aView == 'V') numS = myGeo1c->NumSensorsV();
	} else { 
	  numS = myGeo->NumSensorsXorY();
	  if (aView == 'U') numS = myGeo->NumSensorsU(); 
	  if (aView == 'V') numS = myGeo->NumSensorsV();
	}  
	if (debugIsOn) std::cerr << " .... At View " << aView << " numS " << numS << std::endl;
        for (size_t kSe = 0; kSe != numS; kSe++) {
	  BeamTrack3DFCN::inDatafDebug aInDbgR(0., 0., 0, 9999., 0., 9999., 9999.);
	  fResids[kSeT] = DBL_MAX;
          const double rmsStr = std::max(0.1, fItCl->TheRmsStrip(aView, kSe)); // some very small RMS reappaered.. To be investigated. 
	  if (debugIsOn) std::cerr << " .... At Sensor " << kSe << " Strip " << fItCl->TheAvStrip(aView, kSe) << " RMS " << rmsStr << std::endl;
	  if (rmsStr > 1000.) { kSeT++; continue; } // no measurement. 
	  numMeasurements++; if (iView < 2) numMeasurementsXY++;
	  const double rmsStrN = rmsStr/fOneOSqrt12;
	  const double strip = fItCl->TheAvStrip(aView, kSe);
          const double pitch = myGeo->Pitch(aView, kSe); // Same for Phase1c and Phase1b
	  const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!
	  const double multScatErr = (fIsPhase1c) ? myGeo1c->MultScatUncert(aView, kSe) * 120.0/std::abs(fNominalMomentum) :
	                                           myGeo->MultScatUncert(aView, kSe) * 120.0/std::abs(fNominalMomentum) ;
	  if ((kSe != 0) && (multScatErr < 1.0e-26)) {
	   std::cerr << " BeamTrack3DFCN::operator(),Too low mult scatt err at kSe  " << kSe <<  " view " << aView << " quit here and now " << std::endl; 
	   exit(2); 
	  }
	  const double unknownErr = (fIsPhase1c) ? myGeo1c->UnknownUncert(aView, kSe) : myGeo->UnknownUncert(aView, kSe);
	  const double z = (fIsPhase1c) ? myGeo1c->ZPos(aView, kSe) : myGeo->ZPos(aView, kSe);
	  double tPred = 0.; double tMeas = 0.; double uPred, vPred;
	  const double angleRoll = (fIsPhase1c) ? myGeo1c->Roll(aView, kSe) : myGeo->Roll(aView, kSe);
	  const double angleRollCenter = (fIsPhase1c) ? myGeo1c->RollCenter(aView, kSe) : myGeo->RollCenter(aView, kSe);
	  double rollCorr = 0.;
	  if (aView == 'X') {
	    tPred = xPredAtSt[kSe]; rollCorr = (yPredAtSt[kSe] - angleRollCenter) * angleRoll;
	    tPred += rollCorr;
	    if (debugIsOn) std::cerr << " ... XView roll correction " << (yPredAtSt[kSe] - angleRollCenter) * angleRoll 
	                             << " angleRollCenter " << angleRollCenter << " angleRoll " << angleRoll << " so tPred = " << tPred << std::endl;
	    if (std::abs(angleRoll) > fMaxAngleLin) {
	       tPred = this->exactXPred(xPredAtSt[kSe], yPredAtSt[kSe], angleRoll, angleRollCenter);
	       if (debugIsOn) std::cerr << " ... XView roll correction, exact trig function, tPred is now " << tPred << std::endl;
	    } 			     
	  } else if (aView == 'Y') {
	    tPred = yPredAtSt[kSe]; rollCorr = (xPredAtSt[kSe] - angleRollCenter) * angleRoll;
	    tPred += rollCorr;
	    if (debugIsOn) std::cerr << " ... YView roll correction " 
	                             << (xPredAtSt[kSe] - angleRollCenter) * angleRoll <<  " so tPred = " << tPred <<std::endl;
	    if (std::abs(angleRoll) > fMaxAngleLin) {
	       tPred = this->exactYPred(xPredAtSt[kSe], yPredAtSt[kSe], angleRoll, angleRollCenter);
	       if (debugIsOn) std::cerr << " ... YView roll correction, exact trig function, tPred is now " << tPred << std::endl;
	    } 			     
	  } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	    size_t kSxy = 0; // very clumsy.. Phase 1b only, 
	    if (!fIsPhase1c) {
	      if (aView == 'V') { // Station 4 and 5, kSe = 0 though 3, kSxy, points to the X or Y position, ranging from 0 to 7 (Sept 19..) 
	      // predicted position at kSxy = 4 is the same as kSxy = 5 (same Z); same for 6 and 7. 
	        kSxy = (kSe < 2) ? 4 : 6;
	      } else {
	        kSxy = (kSe == 0) ? 2 : 3;
	      }
	    } else { 
	      if (aView == 'V') { // Station 4 and 5, kSe = 0 though 3, kSxy, points to the X or Y position, ranging from 0 to 7 (Sept 19..) 
	      // predicted position at kSxy = 4 is the same as kSxy = 5 (same Z); same for 6 and 7. 
	        kSxy = (kSe < 2) ? 5 : 7;
	      } else {
	        kSxy = (kSe == 0) ? 2 : 3;
	      }
	    }
	    uPred = fOneOverSqrt2 * ( xPredAtSt[kSxy] + yPredAtSt[kSxy]); // Sept 19, declared correct, but negated by Linyan, Sept 22. Try3D_R1274_3b2_1, Try3D_R1274_3b2_1
//	    uPred = -1.0*fOneOverSqrt2 * ( -xPredAtSt[kSxy] + yPredAtSt[kSxy]); // False, I suspect.. Not sure, let us try, Try3D_R1274_3b3_1 
	    vPred = -1.0*fOneOverSqrt2 * ( -xPredAtSt[kSxy] + yPredAtSt[kSxy]); // presumed correct 
//	    vPred = 1.0*fOneOverSqrt2 * ( xPredAtSt[kSxy] + yPredAtSt[kSxy]); // False, I suspect.. Not sure, let us try, Try3D_R1274_3b3_1 
            rollCorr =  (aView == 'U') ? ( vPred - angleRollCenter) * angleRoll : ( uPred  - angleRollCenter) * angleRoll; 
	    tPred = (aView == 'U') ? uPred + rollCorr :  vPred + rollCorr;
	    if (fDebugIsOn) std::cerr << " ... U or W view, kSxy " << kSxy << " check uPred " << uPred << " and vPred " << vPred << std::endl;
	    // Station 6 fix, from run 2113 studies, December 8 2013. U and W mixed up, and a change of sign.. Go figure.. 
//	    if (fIsPhase1c && (aView == 'V') && (kSe > 1)) tPred = -1.0*(uPred + ( vPred - angleRollCenter) * angleRoll);  			     
	  }
	  if (!fIsPhase1c) {
	    if (aView == 'X') {
	      tMeas =  ( -1.0*strip*pitch + myGeo->TrPos(aView, kSe));
	      if ((kSe >= 4) && ((kSe % 2) == 1)) tMeas *= -1.0;
	      if (debugIsOn) {
	        int kSt = kSe; if (kSe == 4 || kSe == 5) kSt = 4; if (kSe == 6 || kSe == 7) kSt = 5;
	        int aSensor = 0; if (kSe == 4 || kSe == 5) aSensor = kSe - 4; if (kSe == 6 || kSe == 7) aSensor = kSe - 6; 
	        BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr,  tMeas, tPred);
	        aInDbgR = aInDbg; // clumsy.. 
	      }
	    } else if (aView == 'Y') {
	      tMeas = (kSe < 4) ? ( strip*pitch + myGeo->TrPos(aView, kSe)) :
			     ( -1.0*strip*pitch + myGeo->TrPos(aView, kSe)); // Corrected, Sept 9, token NoTgt31Gev_ClSept_A1e_1o1_c10
	      if (((kSe % 2) == 0) && (kSe > 3)) tMeas *= -1.0; // kse ranges from 0 to 7, not the sensor index within the station. 
	      if (debugIsOn) {
	        int kSt = kSe; if (kSe == 4 || kSe == 5) kSt = 4; if (kSe == 6 || kSe == 7) kSt = 5;
	        int aSensor = 0; if (kSe == 4 || kSe == 5) aSensor = kSe - 4; if (kSe == 6 || kSe == 7) aSensor = kSe - 6; 
	        BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	        aInDbgR = aInDbg; // clumsy.. 
	      }
	    } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	      tPred = (aView == 'U') ? uPred + (vPred - angleRollCenter) * angleRoll :  
	  			     vPred + (uPred - angleRollCenter) * angleRoll ;
	      if (aView == 'U') { // Station 2 & 3 
	        tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	        if (debugIsOn) {
	          int kSt = kSe + 2; 
	          int aSensor = 0;
	          BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	          aInDbgR = aInDbg; // clumsy.. 
	        }
	      } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	      // September 23 - 25 2023, aligning with SSDReco/SSDRecStationDwnstrAlgo1 
	      // November-December.. Settled.. 
	        if (kSe == 0) tMeas = (-strip*pitch - myGeo->TrPos(aView, kSe)); // Tested, on run 1274, June 5  
	        else if (kSe == 1) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	        else if (kSe == 2) tMeas = (-strip*pitch - myGeo->TrPos(aView, kSe)); //  Jun 5 2023, better than the June 3 option, 
//	      else if (kSe == 2) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));  // June  3 
	        else if (kSe == 3) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe)); // exploring... 
	        if (debugIsOn) {
	          int kSt = kSe/2 + 4; 
	          int aSensor = kSe % 2;  
	          BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	          aInDbgR = aInDbg; // clumsy.. 
	        }
	      }
	    }
	  } else {  // Phase1c.
/* Code in art based, ConvertDigitToWcCoord Note kSe is the sensor index within a plane, 0 or 1    
	    if (aView == emph::geo::X_VIEW) {
	  tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	  if ((kSt == 2) || (kSt == 3)) tMeas =  -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe);
	  if (kSt > 4) {
	    if (kSe % 2 == 0) tMeas =  strip*pitch; 
	    else  tMeas =  -1.0*strip*pitch; 
	  }
	} else if (aView == emph::geo::Y_VIEW) {
	  tMeas = (kS < 5) ? ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)) :
			     ( strip*pitch); // Corrected, Sept 9, token NoTgt31Gev_ClSept_A1e_1o1_c10
	  if (((kSt > 4) && ((kSe % 2)) == 0)) tMeas *=  -1.0; // Corrected, December 7 
	} else if ((aView == emph::geo::U_VIEW) || (aView == emph::geo::W_VIEW))  { // V is a.k.a. W 
	  if (kSt < 4) { // Sept 1- Sep5  attempt at sorting out orientations.. 
	    tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    if (fDebugIsOn) std::cerr << "  .... kSt " << kSt << "  pitch " << pitch << " strip " 
	                              << strip << " TrShift, again " << fEmVolAlP->TrPos(aView, kSt, kSe) << std::endl;
	  } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	    // Assume W view, double sensor.  Opposite convention as X or Y  Dec. 1 2025.. 
	    if (kSe % 2 == 1) tMeas =  strip*pitch; 
	    else  tMeas =  -1.0*strip*pitch; 
	    	  
	  }
*/
// Here kSe is the sensor index within a view, rangin from 0 to 8 for th X and Y view... define kSeSXY.. 
            size_t kSeSXY = (kSe > 4) ? ((kSe-5) % 2) : 0; 
	    if (aView == 'X') {
	      tMeas =  ( -1.0*strip*pitch + myGeo1c->TrPos(aView, kSe));
	      if (kSe > 4) {
	       if ((kSeSXY % 2) == 0) tMeas =  strip*pitch + myGeo1c->TrPos(aView, kSe); 
	       else  tMeas =  -1.0*strip*pitch + myGeo1c->TrPos(aView, kSe); 
	      }
	      if (debugIsOn) {
	        int kSt = kSe; if (kSe == 5 || kSe == 6) kSt = 5; if (kSe == 7 || kSe == 8) kSt = 6;
	        int aSensor = 0; if (kSe == 5 || kSe == 6) aSensor = kSe - 5; if (kSe == 7 || kSe == 8) aSensor = kSe - 7; 
	        BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	        aInDbgR = aInDbg; // clumsy.. 
	      }
	    } else if (aView == 'Y') {
	      tMeas = (kSe < 5) ? ( strip*pitch + myGeo1c->TrPos(aView, kSe)) :
			     ( strip*pitch + myGeo1c->TrPos(aView, kSe)); // Corrected, Sept 9, token NoTgt31Gev_ClSept_A1e_1o1_c10
//	      if ((kSeSXY % 2) == 1) tMeas *=  -1.0; 
	      if (((kSe > 4) && ((kSeSXY % 2)) == 0)) tMeas *=  -1.0; // Corrected, December 7 
	      if (debugIsOn) {
	        std::cerr << " .... ... Check Y View kSe " << kSe << " kSeSXY " << kSeSXY << " TrSfhift " << myGeo1c->TrPos(aView, kSe) << " tMeas " << tMeas << std::endl;
	        int kSt = kSe; if (kSe == 5 || kSe == 6) kSt = 5; if (kSe == 7 || kSe == 8) kSt = 6;
	        int aSensor = 0; if (kSe == 5 || kSe == 6) aSensor = kSe - 5; if (kSe == 7 || kSe == 8) aSensor = kSe - 7; 
	        BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	        aInDbgR = aInDbg; // clumsy.. 
	      }
	    } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	       tPred = (aView == 'U') ? uPred + (vPred - angleRollCenter) * angleRoll :  
	  			     vPred + (uPred - angleRollCenter) * angleRoll ;
	       if (debugIsOn) {
	          std::cerr << " ... Before trigonometric correction, uPred = " << uPred << " vPred " << vPred << " tPred " << tPred << std::endl;
		  std::cerr << " ... ... .. Roll correction (for uPred ) = " << (vPred - angleRollCenter) * angleRoll << std::endl;
	       }
	       if (aView == 'U') { // Sept 1- Sep5  attempt at sorting out orientations.. 
                  if (std::abs(angleRoll) > fMaxAngleLin) {
		    tPred = this->exactUPred(xPredAtSt[kSe], yPredAtSt[kSe], angleRoll, angleRollCenter);
	            if (debugIsOn) std::cerr << " ... After trig correction, tPred " << tPred << std::endl;
		  }
	          tMeas = (strip*pitch + myGeo1c->TrPos(aView, kSe));
		  if (debugIsOn) { 
	            int kSt = kSe + 2; 
	            int aSensor = 0;
	            BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	            aInDbgR = aInDbg; // clumsy.. 
		  }
	       } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	    // Assume W view, double sensor.  Opposite convention as X or Y  Dec. 1 2025.. 
//   Switch U and V ????? 
                 rollCorr = (vPred - angleRollCenter) * angleRoll;
                 tPred = uPred + rollCorr; // Dec 18, run 7p67, 
                  if (std::abs(angleRoll) > fMaxAngleLin) {
		    tPred = this->exactUPred(xPredAtSt[kSe], yPredAtSt[kSe], angleRoll, angleRollCenter);
	            if (debugIsOn) std::cerr << " ... !! Using U orientation for the Station 5 After trig correction, tPred " << tPred << std::endl;\
		 }  
//	         if ((kSe % 2) == 1) { // 7p65_ and lower... December 17 - 18.  
//	         if ((kSe % 2) == 0) { // Trying 7p66_  December 18.  Also run   7p67_
	         if ((kSe % 2) == 1) { // Trying 7p68_  December 18.  With U assumption... 
		   tMeas =  strip*pitch + myGeo1c->TrPos(aView, kSe); 
	         } else   { tMeas =  -1.0*strip*pitch + myGeo1c->TrPos(aView, kSe); }
	         if (debugIsOn) {
	           int kSt = kSe/2 + 5; 
	           int aSensor = kSe % 2;  
	           BeamTrack3DFCN::inDatafDebug aInDbg(kSt, aView, aSensor, strip, rollCorr, tMeas, tPred);
	           aInDbgR = aInDbg; // clumsy.. 
	         }
	       } // Single sensor ( Station 2 or 3 ) vs double sensor, station 5 or 6 
	    } // U or W (a.k.a. V) 	  
	  } // Phase1b vs Phase1c 
	  if (debugIsOn) std::cerr << " ..... So, pred is now " << tPred << " and .. for MC or real data,.. for this view " 
	                                 << aView << " tMeas " << tMeas  << std::endl; 
	  const double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr + unknownErr*unknownErr;
	  const double dt = (tPred - tMeas);
	  fResids[kSeT] = dt;
	  if (debugIsOn) std::cerr << " .......... so, so, dt = " << dt <<  " resid " << fResids[kSeT] 
	                           << " for kSeT = " << kSeT << " size of resid " << fResids.size() << std::endl;
	  kSeT++;
	  const double deltaChiSq = (dt * dt )/tMeasErrSq;
	  chi2 += deltaChiSq;
	  if (debugIsOn) {
	      aInDbgR.SetDeltaChiSq(deltaChiSq);
	      aInDbgR.SetMeasErr(std::sqrt(std::abs(tMeasErrSq)));
	      fInDataDbg.push_back(aInDbgR); 
	      std::cerr << " ....... Meas err " << std::sqrt(pitch*pitch*stripErrSq) << "  multScatErr " << multScatErr 
	  	      << " unknownErr " << unknownErr << "  dt  " << dt <<  " Current delta chi2 " << deltaChiSq << std::endl;
//        if ((fItCl->EvtNum() == 16) && (fItCl->Spill() == 10)) 
//	  std::cerr << " Debugging bad offsets for evt 16, spill 10 View " << aView 
//	  	    << " kSe " << kSe << " tPred " <<  tPred << " tMeas " << tMeas 
//	            << " tMeasErrSq " << tMeasErrSq << " chi2 " << chi2 << std::endl;
          }
//	  
	} // on the sensors. 
      } // on the views 
      if (std::isnan(chi2)) return 2.5e9; // Should not need this protection.. 
      if (debugIsOn) std::cerr << " BeamTrack3DFCN::operator(), Selected View " << fSelectedView << " done chiSq " << chi2 << std::endl;
      if (debugIsOn) { std::cerr << " ......Chi Sq is " << chi2 << " And enough work for now " << std::endl; this->DumpfInDataDbg(false); exit(2); }
      return chi2;
    }
     
    void BeamTrack3DFCN::DumpfInDataDbg(bool byStation) const {
    
      std::ofstream fOut("./DumpfInDataDbg_V1.txt");
      if (byStation) { 
        fOut << " Station View Sensor strip tMeas tMeasErr tPred deltaChiSq " << std::endl;
        for (int kSt = 0; kSt != 6; kSt++) {
        // X 
          for (std::vector<inDatafDebug>::const_iterator itIn = fInDataDbg.cbegin(); itIn != fInDataDbg.cend(); itIn++) {
	    if ((itIn->fStation != kSt) || (itIn->fView != 'X')) continue;
	    fOut << " " << kSt << " X " << itIn->fSensor << " " << itIn->fStrip << " " 
	                << itIn->fTMeas << " " << itIn->fTMeasErr << " " << itIn->fTPred << " " << itIn->fDeltaChi <<  std::endl;  
	  }
	// Y 
           for (std::vector<inDatafDebug>::const_iterator itIn = fInDataDbg.cbegin(); itIn != fInDataDbg.cend(); itIn++) {
	     if ((itIn->fStation != kSt) || (itIn->fView != 'Y')) continue;
	     fOut << " " << kSt << " Y " << itIn->fSensor << " " << itIn->fStrip << " 0 " 
	          << itIn->fTMeas << " " << itIn->fTMeasErr << " " << itIn->fTPred << " " << itIn->fDeltaChi << std::endl;  
	   }
        // U or W 
           for (std::vector<inDatafDebug>::const_iterator itIn = fInDataDbg.cbegin(); itIn != fInDataDbg.cend(); itIn++) {
	     if (itIn->fStation != kSt) continue;
	     if ((itIn->fView == 'Y') || (itIn->fView == 'X')) continue;
	     fOut << " " << kSt << " " << itIn->fView << " " << itIn->fSensor << " " << itIn->fStrip 
	       << " " << itIn->fTMeas << " " << itIn->fTMeasErr << " " << itIn->fTPred << " " << itIn->fDeltaChi << std::endl;  
	  }
        }
      } else {
         fOut << " Station View Sensor strip rollCorr tMeas tMeasErr tPred deltaChiSq " << std::endl;
         std::vector<char> views{'X', 'Y', 'U', 'V'}; 
          for (int iView = 0; iView != 4; iView++) {	  
            for (std::vector<inDatafDebug>::const_iterator itIn = fInDataDbg.cbegin(); itIn != fInDataDbg.cend(); itIn++) {
               if (itIn->fView != views[iView]) continue;           
	       fOut << " " << itIn->fStation << " " << itIn->fView << " " << itIn->fSensor << " " << itIn->fStrip << " " <<  itIn->fRollCorr 
	       << " " << itIn->fTMeas << " " << itIn->fTMeasErr << " " << itIn->fTPred << " " << itIn->fDeltaChi << std::endl; 
	    } 
          }
      }
      fOut.close();
    }
    
    
  }
}  
 
