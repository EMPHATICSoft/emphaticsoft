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

#include "SSDReco/SSD3DTrackFitFCNAlgo1.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr;

namespace emph {
  namespace ssdr {
    
    SSD3DTrackFitFCNAlgo1::SSD3DTrackFitFCNAlgo1(int runNum) :
    FCNBase(),
    fRunHistory(new runhist::RunHistory(runNum)),   
    fEmgeo(new emph::geo::Geometry(fRunHistory->GeoFile())),
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), 
    fMagField(nullptr), fIsMC(false), fDebugIsOn(false), fIntegrationStep(20.0), // last parameter might need some tuning.. This is the value at 120 GeV.
    fExpectedMomentum(30.), // expected momentum, used to compute the mult. scattering uncertainty. 
    fNumSensorsTotal(2*fEmVolAlP->NumSensorsXorY() + fEmVolAlP->NumSensorsU() + fEmVolAlP->NumSensorsV()),
    fData(), fNoMagnet(false), fZPos(), fMagShift(3, 0.),
    fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), 
    fOneOSqrt12(std::sqrt(1.0/12.)), fResids(fNumSensorsTotal, DBL_MAX),
    fZLocUpstreamMagnet(DBL_MAX), fZLocDownstrMagnet(DBL_MAX) {
     
       art::ServiceHandle<emph::MagneticFieldService> bField;
       fMagField = bField->Field();
       art::ServiceHandle<emph::geo::GeometryService> geo;
       fNoMagnet = geo->Geo()->MagnetUSZPos() < 0.;
//
    }
    SSD3DTrackFitFCNAlgo1::~SSD3DTrackFitFCNAlgo1() {
      if (fFOutResids.is_open()) fFOutResids.close();
    }  
    double SSD3DTrackFitFCNAlgo1::operator()(const std::vector<double> &pars) const {
    
      if (fDebugIsOn) std::cerr << "SSD3DTrackFitFCNAlgo1::operator, Start, number of track parameters " 
                                << pars.size() << " Number of data pts " << fData.size() << std::endl;  
      if (fDebugIsOn && fNoMagnet) std::cerr << "  .....   The Magnet has been removed.. " << std::endl;
      if (fData.size() <  6) return 2.0e10; // require at lease 6 planes, one dgree of freedom if so... 
      if ((!fNoMagnet) && (std::abs(pars[4]) < 0.5)) return 5.0e10; 
       // if Mininuit attemps to change the sign of momentum, or jumps too far, assign  a very large chi-square. 
      if (fZPos.size() == 0) this->getZPos();
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
      const bool neglectFringeFieldUp = false; 
      const bool neglectFringeFieldDown = true; 
      if (fDebugIsOn) {
         std::cerr << "... x0 " << x0 << " slx0 " << slx0 << " y0 " << y0 << " sly0 " << sly0;
	 if (pars.size() == 5) std::cerr << " pMom " << pars[4]; 
         std::cerr  << std::endl; 
      }
      std::vector<double> xPredAtSt(6, 0.); // Phase1b 
      std::vector<double> yPredAtSt(6, 0.);
      const size_t numStationInStraightLine = (fNoMagnet) ? 6 : 4; // Phase1b 
      for (size_t kSt=0; kSt != numStationInStraightLine; kSt++) {
        size_t kSe = (kSt > 3) ? (4 + (kSt-4)*2 ) : kSt; // In phase1b, z Position are identical for sensors 4 and 5 , same with 6 == 7 
        xPredAtSt[kSt] = x0 + slx0*(fEmVolAlP->ZPos(emph::geo::X_VIEW, kSt, kSe) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0));
        yPredAtSt[kSt] = y0 + sly0*(fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt, kSe) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 0));
        if (fDebugIsOn) 
	  std::cerr << " ..... Before the magnet, X and Y predictions for  kSt " << kSt << " kSe " << kSe 
	  	      << " xPred " << xPredAtSt[kSt] << " yPred " <<  yPredAtSt[kSt] << " ZYKst " 
		      << fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt, kSe) << std::endl; 
      }
      if (!fNoMagnet) {	
        if (fIntegrationStep > 1.0e-6) {
          // Compute the expected kick, define intercepts and slopes downstream of the magnet, based on the Magneticfield integrator class. 
	  // Take into account the effect of the fringe field.. 
          const int Q = (pars[4] > 0.) ? -1 : 1 ; // Let us flipt the sign of the charge.. The X coordinate has been flipped, perhaps... 
//         const double stepAlongZ = fIntegrationStep * (std::abs(pars[4])/120.);
          const double stepAlongZ = fIntegrationStep; // such we don't introduce suspicious correlations.. 
          std::vector<double> startMag(6, 0.); std::vector<double> endMag(6, 0.); 
	  double zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
	  double zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 3); double slx1, sly1;
	  if (!neglectFringeFieldUp) { 
	    if (fDebugIsOn) std::cerr << "Upstream Fringe Field zLocUpstreamMagnet " <<  zLocUpstreamMagnet 
	                            << " Downstream " << zLocDownstrMagnet << std::endl;
            startMag[0] = x0 + slx0*zLocUpstreamMagnet; // assume station 0 is the origin. 
            startMag[1] = y0 + sly0*zLocUpstreamMagnet; // assume station 0 is the origin. 
            startMag[2] = zLocUpstreamMagnet; 
            startMag[3] = slx0*pars[4]; // 
            startMag[4] = sly0*pars[4]; // assume small slope, sin(theta) = theta.. 
            startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx0*slx0 - sly0*sly0); 
            endMag[2] = zLocDownstrMagnet; 
	    // 
	    // Implement Magnet misalignment.. 
	    startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
            fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	    //
	    // Back shift.. 
	    endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
            slx1 = endMag[3]/pars[4]; sly1 = endMag[4]/pars[4]; 
            xPredAtSt[3] = endMag[0]; yPredAtSt[3] = endMag[1];
	   
	   // correction for the small difference of X and Y planes. 
	    double ddZXY = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 3) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 3); yPredAtSt[3] += ddZXY*sly1;  
            if (fDebugIsOn) 
	      std::cerr << " ..... After the Usptream X and Y Integrated predictions for  for Station 3, xPred " 
	     << xPredAtSt[3] << " yPred " <<  yPredAtSt[3] << " X Slope " << slx1 <<  std::endl; 
	  } else {
	     slx1 = slx0; sly1 = sly0;
	  }
	  //
	  // The Magnet itself, between station 3 and station 4. 
	  //
	  zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 3);
	  zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);
	  if (fDebugIsOn) std::cerr << " Magnet itself zLocUpstreamMagnet " <<  zLocUpstreamMagnet << " Downstream " << zLocDownstrMagnet << std::endl;
          startMag[0] = xPredAtSt[3]; // restart, from station  
          startMag[1] = yPredAtSt[3]; // assume station 0 is the origin. 
          startMag[2] = zLocUpstreamMagnet; 
          startMag[3] = slx1*pars[4]; // 
          startMag[4] = sly1*pars[4];; // assume small slope, sin(theta) = theta.. 
          startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx1*slx1 - sly1*sly1);
	  endMag[2] = zLocDownstrMagnet; 
 	  startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
          fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	  endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
          double slx2 = endMag[3]/pars[4]; double sly2 = endMag[4]/pars[4]; 
          xPredAtSt[4] = endMag[0]; yPredAtSt[4] = endMag[1];
	  // correction for the small difference of X and Y planes. 
	  const double ddZXYC = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 4) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 4); 
	  yPredAtSt[4] +=  ddZXYC*sly2;
          if (fDebugIsOn) 
	     std::cerr << " ..... After the magnet X and Y Integrated predictions at station 4, xPred " 
	               << xPredAtSt[4] << " yPred " <<  yPredAtSt[4] 
		      << " X Slope " << slx2 << " Y Slope " << sly2 <<  std::endl; 
	  //
	  // Downstream of the magnet 
	  //
	  if (!neglectFringeFieldDown) { 
	    	      
	    zLocUpstreamMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4);
	    zLocDownstrMagnet = fEmVolAlP->ZPos(emph::geo::X_VIEW, 6);
	    if (fDebugIsOn) std::cerr << " Downstream fringe  zLocUpstreamMagnet " <<  zLocUpstreamMagnet << " Downstream " << zLocDownstrMagnet << std::endl;
            startMag[0] = xPredAtSt[4];  
            startMag[1] = yPredAtSt[4]; 
            startMag[2] = zLocUpstreamMagnet; 
            startMag[3] = slx2*pars[4]; // 
            startMag[4] = sly2*pars[4];; // assume small slope, sin(theta) = theta.. 
            startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx2*slx2 - sly2*sly2);
	    endMag[2] = zLocDownstrMagnet; 
 	    startMag[0] -= fMagShift[0]; startMag[1] -= fMagShift[1]; startMag[2] -= fMagShift[2];
            fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
	    endMag[0] += fMagShift[0]; endMag[1] += fMagShift[1]; 
            double slx3 = endMag[3]/pars[4]; double sly3 = endMag[4]/pars[4];
            xPredAtSt[5] = endMag[0]; yPredAtSt[5] = endMag[1];
	  // correction for the small difference of X and Y planes. 
	    const double ddZXYE = fEmVolAlP->ZPos(emph::geo::Y_VIEW, 6) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 6); 
	    yPredAtSt[5] +=  ddZXYE*sly3;
            if (fDebugIsOn) 
	       std::cerr << " ..... After the Downstream fringe field, xPred " << xPredAtSt[5] << " yPred " <<  yPredAtSt[5] 
		      << " X Slope " << slx3 << " Y Slope " << sly3 <<  std::endl; 
	 } else {
	   xPredAtSt[5] = xPredAtSt[4] + slx2*(fEmVolAlP->ZPos(emph::geo::X_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 4)); 
	   yPredAtSt[5] = yPredAtSt[4] + sly2*(fEmVolAlP->ZPos(emph::geo::Y_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 4)); 
           if (fDebugIsOn) {
	       std::cerr << " .....Negecting downstream fringe field,  after the magnet, xPred " << xPredAtSt[5] << " yPred " <<  yPredAtSt[5] 
		      << " X Slope " << slx2 << " Y Slope " << sly2 <<  std::endl; 		      
           }
	 }
       } else { // simple kick.. 
         for (size_t kSt=4; kSt != 6; kSt++) {
	   const double slx1 =  slx0 + kick;
	   const double xx = x0 + slx0*(zMag - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0));
           xPredAtSt[kSt] = xx + slx1*( fEmVolAlP->ZPos(emph::geo::X_VIEW, kSt) - zMag);
           yPredAtSt[kSt] = y0 + sly0*(fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 0));
           if (fDebugIsOn) 
	     std::cerr << " ..... After the magnet X and Y predictions, simple kick for  kSe " << kSt 
	  	      << " kick " << kick << " xPred " << xPredAtSt[4 + (kSt-4)*2] << " yPred " <<  yPredAtSt[4 + (kSt-4)*2] << std::endl; 
          }	
        }
      } // With or without the magnet..  
      for (size_t kSe=0; kSe != fResids.size(); kSe++) fResids[kSe] = DBL_MAX;  
       size_t kD = 0;
       for (std::vector<myItCl>::const_iterator itCl = fData.cbegin(); itCl != fData.cend(); itCl++, kD++) {
          std::vector<rb::SSDCluster>::const_iterator it = *itCl;
 	  const int kSt = it->Station();
 	  const int kSe = it->Sensor();
	  const int kS = (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt; // in dex ranging from 0 to 7, inclusive, for Phase1b, list of sensors by view.  
	  emph::geo::sensorView aView = it->View();
	  size_t kSeT = 0; // For the residuals. Easier to plot and understand this way, but messy to code in this context.
	  // Valid for Phase1b  
	  switch (aView) {
	    case emph::geo::X_VIEW : { kSeT = (kSt < 4) ? kSt : 4 + 2*(kSt-4) + kSe % 2; break; } 
	    case emph::geo::Y_VIEW : { kSeT = (kSt < 4) ? kSt : 4 + 2*(kSt-4) + kSe % 2; kSeT += fEmVolAlP->NumSensorsXorY(); break; } 
	    case emph::geo::U_VIEW : { kSeT =  2.0*fEmVolAlP->NumSensorsXorY() + kSt - 2; break; } // Station 2 and 3. , one U sensor perstation. 
	    case emph::geo::W_VIEW : { kSeT =  2.0*fEmVolAlP->NumSensorsXorY() + 2 + 2*(kSt - 4) + kSe % 2; break; } // Station 4 and 5, double sensors.  
	    default : { continue; } // Phase1b only.. 
	  }
	  if (kSeT > 21) {
	    std::cerr << " SSD3DTrackFitFCNAlgo1::operator, Wrong residual indexing " << kSeT 
	                 << " for subsequent anlysis, something wrong, quit here and now " << std::endl; return 3.0e9; 
	  }
          const double rmsStr = std::max(0.1, it->WgtRmsStrip()); // protect against some zero values for the RMS 
	  const double strip = it->WgtAvgStrip();
	  if (fDebugIsOn) std::cerr << " .... At Station " << kSt << " Sensor " << kSe << " Strip " << strip << " RMS " << rmsStr << std::endl;
	  if (rmsStr > 1000.) { continue; } // no measurement. 
	  const double rmsStrN = rmsStr/fOneOSqrt12;
          const double pitch = fEmVolAlP->Pitch(aView, kSt, kSe); 
	  const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!  Suspicious.. 
//	  const double multScatErr =  fEmVolAlP->MultScatUncert(aView, kSt, kSe) * 120.0/std::abs(pars[4]); 
// quite possibly, a bad idea, as it will bias the chi-square.. 
	  const double multScatErr =  fEmVolAlP->MultScatUncert(aView, kSt, kSe) * 120.0/fExpectedMomentum; 
	  const double unknownErr = fEmVolAlP->UnknownUncert(aView, kSt, kSe);
//	  const double zUgly = this->fZPos[kD];
	  const double zV = fEmVolAlP->ZPos(aView, kSt, kSe); 
	  if (fDebugIsOn) std::cerr << "  ....  So, at z " << zV << " for view " << aView <<  " kSeT " << kSeT << " ..... " << std::endl;
//	  if (std::abs(zV - zUgly) > 1.0) {
//	    std::cerr << " SSD3DTrackFitFCNAlgo1::operator(), problem with accessing Z coordinates, zUgly " 
//	              << zUgly << " kD " << kD << " Z Volatile " << zV <<  " fatal .. quit here and now...  " << std::endl; exit(2);
//	  }  
	  double xPred = xPredAtSt[kSt] ; double yPred = yPredAtSt[kSt]; double tPred = 0.; double tMeas = 0.;
	  // Here we need to implement the transverse shift, from fEmVolAlP
	    // Debugging for real data.. Run 1043, May 29 - May 30 
	  const double uPred = fOneOverSqrt2 * ( xPred + yPred);
	  const double vPred = -1.0*fOneOverSqrt2 * ( xPred + yPred);
	  // T coordinate (measuring 
	  const double angleRoll = fEmVolAlP->Roll(aView, kSt, kSe);
	  const double angleRollCenter = fEmVolAlP->RollCenter(aView, kSt, kSe);
	  switch (aView) {
	    case emph::geo::X_VIEW : {
	      tPred = xPred ; 
	      tPred += (yPred  - angleRollCenter) * angleRoll;
	      break;
	    }
	    case emph::geo::Y_VIEW : {
	      tPred = yPred ; 
	      tPred += (xPred - angleRollCenter) * angleRoll;
	      break;
	    }
	    case emph::geo::U_VIEW : {
	      tPred = uPred ; 
	      tPred += (vPred - angleRollCenter) * angleRoll;
	      break;
	    }
	    case emph::geo::W_VIEW : {
	      tPred = vPred ; 
	      tPred += (uPred - angleRollCenter) * angleRoll;
	      break;
	    }
	    default : { continue; }
	    
	  } // end of transverse coordinate prediction. 
	  if (aView == emph::geo::X_VIEW) {
	    tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    if ((kSt > 3) && (kSe % 2) == 1)  tMeas *= -1.0;
	    if (fDebugIsOn) 
	      std::cerr << " ..... X View " << " kSe " << kSe 
	            << " yPred " << yPred << " tPred " << tPred << " tMeas " << tMeas  << std::endl; 
	  } else if (aView == emph::geo::Y_VIEW) {
	    tMeas =  ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    if (kS >= 4) tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    if (fDebugIsOn) 
	      std::cerr << " ..... Y View " << " kSe " << kSe 
	            << " xPred " << xPred << " tPred " << tPred << " tMeas " << tMeas  << std::endl; 
	  } else if ((aView == emph::geo::U_VIEW) || (aView == emph::geo::W_VIEW))  { // V is a.k.a. W 
	    if (aView == emph::geo::U_VIEW) { 
	      tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	      if (kSe == 0) tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)); // Unknown, this is a place holder. 
	      else if (kSe == 1) tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	      else if (kSe == 2) tMeas = (-strip*pitch - fEmVolAlP->TrPos(aView, kSt, kSe));
	      else if (kSe == 3) tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)); // exploring... 
	    }
	  }
	  if (fDebugIsOn) 
	    std::cerr << " ..... Coords  View " << aView << " kSe " << kSe 
	  	  << " xPred " << xPred << " yPred " <<  yPred << " uPred " << uPred << " vPred " << vPred << std::endl; 
	  double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr + unknownErr*unknownErr;
	  const double dt = (tPred - tMeas);
	  fResids[kSeT] = dt;
	  const double deltaChi2 =  (dt * dt )/tMeasErrSq;
	  chi2 += deltaChi2;
          if (fDebugIsOn) std::cerr << " ....	..... Index kSeT " << kSeT << " dt " << dt << " +- " 
	  			   << std::sqrt(tMeasErrSq) << " multScatErr " << multScatErr  << " unknownErr " << unknownErr
	        		   <<  " delta Chi2 " << deltaChi2 << std::endl;
        } // on the SSD Clusters 
//      if (fDebugIsOn) { std::cerr << " ......Chi Sq is " << chi2 << " And enough work for now " << std::endl; exit(2); }
      if (fDebugIsOn) { std::cerr << " ......Chi Sq is " << chi2 << " And we keep going....  " << std::endl; }
      fLastChi2 = chi2;
      return chi2;
    }
     //
     // Ugly here: access to the Z positions differe from the transverse, roll, pitch, yaw angle.. 
     // 
    void SSD3DTrackFitFCNAlgo1::getZPos() const { 
     
      if (fData.size() == 0) return;
      const double zMag = fEmVolAlP->ZCoordsMagnetCenter();
      for (std::vector<myItCl>::const_iterator itCl = fData.cbegin(); itCl != fData.cend(); itCl++) {
        std::vector<rb::SSDCluster>::const_iterator it = *itCl;
 	const int kSt = static_cast<size_t>(it->Station());
 	const int kSe = static_cast<size_t>(it->Sensor());
        const emph::geo::SSDStation *aStation = fEmgeo->GetSSDStation(kSt);
        const emph::geo::Detector *aSensor = fEmgeo->GetSSDSensor(kSe);
        TVector3 posSt = aStation->Pos();
        TVector3 posPl = aSensor->Pos();
	const double zz = posPl[2] + posSt[2];
	fZPos.push_back(zz);
	if (zz < zMag) fZLocUpstreamMagnet = zz;
	if ((zz > zMag) && (fZLocDownstrMagnet == DBL_MAX)) fZLocDownstrMagnet = zz; // in a given view, assume z order for the incoming data. 
      }
    }  
    void SSD3DTrackFitFCNAlgo1::OpenOutResids(const std::string &fNameStr) {
      if (fFOutResids.is_open()) return;
      fFOutResids.open(fNameStr.c_str());
      fFOutResids << " spill evt chiSq";
      for (size_t k=0; k != (2.0*fEmVolAlP->NumSensorsXorY() + fEmVolAlP->NumSensorsU() + fEmVolAlP->NumSensorsV()); k++) 
        fFOutResids << " resid" << k;
      fFOutResids << " " << std::endl;   
    } 
    void SSD3DTrackFitFCNAlgo1::SpitOutResids(int spill, int evtNum) {
      if (!fFOutResids.is_open()) return;
      fFOutResids << " " << spill << " " << evtNum << " " << fLastChi2;
      for (size_t k=0; k != (2.0*fEmVolAlP->NumSensorsXorY() + fEmVolAlP->NumSensorsU() + fEmVolAlP->NumSensorsV()); k++) 
         fFOutResids << " " << fResids[k];
      fFOutResids << " " << std::endl;   
    }
    
  } // namespace 
} // Name space emph  
 
