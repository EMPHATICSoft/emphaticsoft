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
    
    BeamTrack3DFCN::BeamTrack3DFCN() :
    myGeo(emph::rbal::BTAlignGeom::getInstance()), fIsMC(true), fNoMagnet(false),
    fNumSensorsTotal(2*myGeo->NumSensorsXorY() + myGeo->NumSensorsU() + myGeo->NumSensorsV()),
    FCNBase(),
    fItCl(NULL), fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), fNominalMomentum(120.),
    fOneOSqrt12(std::sqrt(1.0/12.)), fNCalls(0), fResids(fNumSensorsTotal, DBL_MAX), fMagField(nullptr) { ; }  
      
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
      const bool neglectFringeFieldDown = true;
// 06i, d2d sequence instead of c2d      
//      const bool neglectFringeFieldDown = false;
//      const bool debugIsOn = ((pars[0] > 0.25) && (pars[2] < -0.25)); 
      if (debugIsOn) std::cerr << "BeamTrack3DFCN::operator, number of track parameters " << pars.size() << std::endl;  
      if (debugIsOn && fIsMC) std::cerr << " ... Assume Monte Carlo sign conventions  " << std::endl;
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
         std::cerr  << std::endl; 
      }
      std::vector<double> xPredAtSt(myGeo->NumSensorsXorY(), 0.); 
      std::vector<double> yPredAtSt(myGeo->NumSensorsXorY(), 0.);
      const size_t numSensorInStraightLine = (fNoMagnet) ? myGeo->NumSensorsXorY() : 4; // Phase1b 
      for (size_t kSt=0; kSt != numSensorInStraightLine; kSt++) { 
        xPredAtSt[kSt] = x0 + slx0*(myGeo->ZPos('X', kSt) - myGeo->ZPos('X', 0));
        yPredAtSt[kSt] = y0 + sly0*(myGeo->ZPos('Y', kSt) - myGeo->ZPos('Y', 0));
        if (debugIsOn) 
	  std::cerr << " ..... Before the magnet, X and Y predictions for  kSe " << kSt 
	  	      << " xPred " << xPredAtSt[kSt] << " yPred " <<  yPredAtSt[kSt] 
		      << " ZYKst " << myGeo->ZPos('Y', kSt) << " ZXKst " << myGeo->ZPos('X', kSt) << std::endl; 
      }
      if (!fNoMagnet) { 	
        if (integrationStepSize > 1.0e-6) {
         // Compute the expected kick, define intercepts and slopes downstream of the magnet, based on the Magneticfield integrator class. 
	 // Take into account the effect of the fringe field.. 
	  if (fMagField == nullptr) fMagField = emph::rbal::BTMagneticField::getInstance();
          const int Q = (pars[4] > 0.) ? -1 : 1 ; // Let us flipt the sign of the charge.. The X coordinate has been flipped, perhaps... 
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
         for (size_t kSe=4; kSe != myGeo->NumSensorsXorY(); kSe++) {
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
      std::vector<char> views{'X', 'Y', 'U', 'V'};
      size_t kSeT = 0;
      for (int iView = 0; iView !=4; iView++) { 
        char aView = views[iView];
        size_t numS = myGeo->NumSensorsXorY();
	if (aView == 'U') numS = myGeo->NumSensorsU(); 
	if (aView == 'V') numS = myGeo->NumSensorsV();
	if (debugIsOn) std::cerr << " .... At View " << aView << std::endl;
        for (size_t kSe = 0; kSe != numS; kSe++) {
	  fResids[kSeT] = DBL_MAX;
          const double rmsStr = std::max(0.1, fItCl->TheRmsStrip(aView, kSe)); // some very small RMS reappaered.. To be investigated. 
	  if (debugIsOn) std::cerr << " .... At Sensor " << kSe << " Strip " << fItCl->TheAvStrip(aView, kSe) << " RMS " << rmsStr << std::endl;
	  if (rmsStr > 1000.) { kSeT++; continue; } // no measurement. 
	  const double rmsStrN = rmsStr/fOneOSqrt12;
	  const double strip = fItCl->TheAvStrip(aView, kSe);
          const double pitch = myGeo->Pitch(aView, kSe); 
	  const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!
	  const double multScatErr =  myGeo->MultScatUncert(aView, kSe) * 120.0/fNominalMomentum;
	  if ((kSe != 0) && (multScatErr < 1.0e-26)) {
	   std::cerr << " BeamTrack3DFCN::operator(),Too low mult scatt err at kSe  " << kSe <<  " view " << aView << " quit here and now " << std::endl; 
	   exit(2); 
	  }
	  const double unknownErr = myGeo->UnknownUncert(aView, kSe);
	  const double z = myGeo->ZPos(aView, kSe);
	  double tPred = 0.; double tMeas = 0.; double uPred, vPred;
	  const double angleRoll = myGeo->Roll(aView, kSe);
	  const double angleRollCenter = myGeo->RollCenter(aView, kSe);
	  if (aView == 'X') {
	    tPred = xPredAtSt[kSe]; 
	    tPred += (yPredAtSt[kSe] - angleRollCenter) * angleRoll;
	  } else if (aView == 'Y') {
	    tPred = yPredAtSt[kSe]; 
	    tPred += (xPredAtSt[kSe] - angleRollCenter) * angleRoll;
	  } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	    size_t kSxy = 0; // very clumsy.. Phase 1b only, 
	    if (aView == 'U') {
	      kSxy = (kSe == 0) ? 2 : 3;
	    } else {
	      kSxy = (kSe < 2) ? 4 : 6;
	    }
	    if (fIsMC) { //  X to minus, hyterical reason
	       uPred = fOneOverSqrt2 * ( -xPredAtSt[kSxy] + yPredAtSt[kSxy]);
	       vPred = -1.0*fOneOverSqrt2 * ( xPredAtSt[kSxy] + yPredAtSt[kSxy]);
	    } else { // Debugging for real data.. Run 1043, May 29 
	      uPred = fOneOverSqrt2 * ( xPredAtSt[kSxy] + yPredAtSt[kSxy]);
	      vPred = -1.0*fOneOverSqrt2 * ( -xPredAtSt[kSxy] + yPredAtSt[kSxy]);
	    }
	    tPred = (aView == 'U') ? uPred + ( vPred - angleRollCenter) * angleRoll :  
	                             vPred + ( uPred  - angleRollCenter) * angleRoll ;
	  }
	  if (!fIsMC) { 
	    if (aView == 'X') {
	      tMeas =  ( -1.0*strip*pitch + myGeo->TrPos(aView, kSe));
	      if ((kSe >= 4) && ((kSe % 2) == 1)) tMeas *= -1.0;
	    } else if (aView == 'Y') {
	      tMeas =  ( strip*pitch + myGeo->TrPos(aView, kSe));
	      if (kSe >= 4) tMeas =  ( -1.0*strip*pitch + myGeo->TrPos(aView, kSe));
//	      if (std::abs(tMeas) > 60.) {
//	        std::cerr << " BeamTrack3DFCN::operator(), Y view, anomalously large coordinate, " 
//		          << kSe  << " tMeas " << tMeas << " and quit here.. " <<  std::endl; exit(2);
//	      }
	    } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	     /// To be reviewed.. for data.. 
	      tPred = (aView == 'U') ? uPred + (vPred - angleRollCenter) * angleRoll :  
	                               vPred + (uPred - angleRollCenter) * angleRoll ;
	      if (aView == 'U') { 
	        tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	      } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	        if (kSe == 0) tMeas = (-strip*pitch - myGeo->TrPos(aView, kSe)); // Tested, on run 1274, June 5  
	        else if (kSe == 1) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	        else if (kSe == 2) tMeas = (-strip*pitch - myGeo->TrPos(aView, kSe)); //  Jun 5 2023, better than the June 3 option, 
//	        else if (kSe == 2) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));  // June  3 
	        else if (kSe == 3) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe)); // exploring... 
	      }
	    }
	    if (debugIsOn)     std::cerr << " ..... So, pred is now " << tPred << " and .. for real data,.. for this view " 
	                                 << aView << " tMeas " << tMeas  << std::endl; 
	  } else { // Monte Carlo Convention 
	    if (aView == 'X') {
	      tMeas = ( -1.0 * strip*pitch + myGeo->TrPos(aView, kSe));
	      if (debugIsOn && (kSe == 7)) std::cerr << " ....X view confirming sensor kSe 7  strip " 
	                << strip << " TrPos " << myGeo->TrPos(aView, kSe) << std::endl;
	      if ((kSe > 3) && (kSe % 2) == 1) tMeas *=-1;      
	    } else if (aView == 'Y') {
	      tMeas = (kSe < 4) ? ( strip*pitch + myGeo->TrPos(aView, kSe)) :
	                      ( strip*pitch - myGeo->TrPos(aView, kSe)) ;
	      if ((kSe > 3) && (kSe % 2) == 1) tMeas *=-1;      
	    } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	      if (aView == 'U') { 
	        tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	      } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics.
	       // But this is MC convention, we should know!!!!  
	        tMeas = (strip*pitch + myGeo->TrPos(aView, kSe)); // Unknown, this is a place holder. 
	        if ((kSe % 2) ==  1) tMeas *= -1.0;
		
	      }
	    }
	    if (debugIsOn)     std::cerr << " .....So, pred is now " << tPred 
	                  << " and for MC data, for  this view " << aView << " tMeas " << tMeas  << std::endl; 
	    }
	    const double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr + unknownErr*unknownErr;
	    const double dt = (tPred - tMeas);
	    fResids[kSeT] = dt;
	    kSeT++;
	    const double deltaChiSq = (dt * dt )/tMeasErrSq;
	    chi2 += deltaChiSq;
	    if (debugIsOn) std::cerr << " ....... Meas err " << std::sqrt(pitch*pitch*stripErrSq) << "  multScatErr " << multScatErr 
	                << " unknownErr " << unknownErr << "  dt  " << dt <<  " Current delta chi2 " << deltaChiSq << std::endl;
//          if ((fItCl->EvtNum() == 16) && (fItCl->Spill() == 10)) 
//	    std::cerr << " Debugging bad offsets for evt 16, spill 10 View " << aView 
//	              << " kSe " << kSe << " tPred " <<  tPred << " tMeas " << tMeas 
//		      << " tMeasErrSq " << tMeasErrSq << " chi2 " << chi2 << std::endl;
//	  
	} // on the sensors. 
      } // on the views 
      if (std::isnan(chi2)) return 2.5e9; // Should not need this protection.. 
      if (debugIsOn) std::cerr << " BeamTrack3DFCN::operator(), done chiSq " << chi2 << std::endl;
//      if (debugIsOn) { std::cerr << " ......Chi Sq is " << chi2 << " And enough work for now " << std::endl; exit(2); }
      return chi2;
    }
     

    
    
  }
}  
 
