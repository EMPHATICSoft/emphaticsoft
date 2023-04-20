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

emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr;

namespace emph {
  namespace ssdr {
    
    SSD3DTrackFitFCNAlgo1::SSD3DTrackFitFCNAlgo1(int runNum) :
    FCNBase(),
    fRunHistory(new runhist::RunHistory(runNum)),   
    fEmgeo(new emph::geo::Geometry(fRunHistory->GeoFile())),
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), 
    fMagField(nullptr), fIsMC(false), fDebugIsOn(false), fIntegrationStep(2.0), // last parameter might need some tuning.. This is the value at 120 GeV.
    fNumSensorsTotal(2*fEmVolAlP->NumSensorsXorY() + fEmVolAlP->NumSensorsU() + fEmVolAlP->NumSensorsV()),
    fData(), fZPos(),
    fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), 
    fOneOSqrt12(std::sqrt(1.0/12.)), fResids(fNumSensorsTotal, DBL_MAX),
    fZLocUpstreamMagnet(DBL_MAX), fZLocDownstrMagnet(DBL_MAX) {
     
       art::ServiceHandle<emph::MagneticFieldService> bField;
       fMagField = bField->Field();
//
    }
    SSD3DTrackFitFCNAlgo1::~SSD3DTrackFitFCNAlgo1() {
      if (fFOutResids.is_open()) fFOutResids.close();
    }  
    double SSD3DTrackFitFCNAlgo1::operator()(const std::vector<double> &pars) const {
    
      if (fData.size() <  6) return 2.0e10; // require at lease 6 planes, one dgree of freedom if so... 
      if (std::abs(pars[4]) < 0.5) return 5.0e10; 
       // if Mininuit attemps to change the sign of momentum, or jumps too far, assign  a very large chi-square. 
      if (fZPos.size() == 0) this->getZPos();
//      const bool fDebugIsOn = ((pars[0] > 0.25) && (pars[2] < -0.25)); 
      if (fDebugIsOn) std::cerr << "SSD3DTrackFitFCNAlgo1::operator, number of track parameters " << pars.size() << std::endl;  
      if (fDebugIsOn && fIsMC) std::cerr << " ... Assume Monte Carlo sign conventions  " << std::endl;  
      assert(pars.size() == 5);
//       std::cerr << " SSD3DTrackFitFCNAlgo1::operator(), begin, x0 " << pars[0] << std::endl;
      const double zMag = fEmVolAlP->ZCoordsMagnetCenter();
      const double kick = fEmVolAlP->MagnetKick120GeV(); // only for debugging, we use the magnetic field intgrator. 
      if (fDebugIsOn) std::cerr << " .... Check Coordinate Z Mag " << zMag 
                               << " Upstream " << fZLocUpstreamMagnet << " Downstream " << fZLocDownstrMagnet << std::endl;
      double chi2 = 0.;
      const double x0 = pars[0]; 
      const double slx0 = pars[1];
      const double y0 = pars[2]; 
      const double sly0 = pars[3];
      // Compute the expected kick, define intercepts and slopes downstream of the magnet, based on the Magneticfield integrator class. 
      std::vector<double> startMag(6, 0.); 
      startMag[0] = x0 + slx0*fZLocUpstreamMagnet; // assume station 0 is the origin. 
      startMag[1] = y0 + sly0*fZLocUpstreamMagnet; // assume station 0 is the origin. 
      startMag[2] = fZLocUpstreamMagnet; 
      startMag[3] = slx0*pars[4]; // 
      startMag[4] = sly0*pars[4];; // assume small slope, sin(theta) = theta.. 
      startMag[5] = std::abs(pars[4]) * std::sqrt(1.0 - slx0*slx0 - sly0*sly0); 
      std::vector<double> endMag(6, 0.); endMag[2] = fZLocDownstrMagnet; 
      const int Q = (pars[5] > 0.) ? -1 : 1 ; // Let us flipt the sign of the charge.. The X coordinate has been flipped, perhaps... 
      const double stepAlongZ = fIntegrationStep * (std::abs(pars[4])/120.); 
      if (fDebugIsOn) std::cerr << " Step along z for integration " << stepAlongZ << std::endl;
      fMagField->Integrate(0, Q, stepAlongZ, startMag, endMag);
      const double x1 = endMag[0]; const double y1 = endMag[1];
      const double slx1 = endMag[3]/pars[4]; const double sly1 = endMag[4]/pars[4]; 
      if (fDebugIsOn) std::cerr << "... x0 " << x0 << " slx0 " << slx0 << " y0 " 
                               << y0 << " sly0 " << sly0 << " slx1 " << slx1 << " sly1 " << sly1 << " kick " << kick << std::endl;
      for (size_t kSe=0; kSe != fResids.size(); kSe++) fResids[kSe] = DBL_MAX;  
      // For standalone SSDAlignMPI, no art.. 
//      std::vector<char> views{'X', 'Y', 'U', 'V'};
//      size_t kSeT = 0;
//      for (int iView = 0; iView !=4; iView++) { 
//        char aView = views[iView];
//        size_t numS = fEmGeo->NumSensorsXorY();
//	if (aView == 'U') numS = fEmGeo->NumSensorsU(); 
//	if (aView == 'V') numS = fEmGeo->NumSensorsV();
//	if (fDebugIsOn) std::cerr << " .... At View " << aView << std::endl;
//        for (size_t kSe = 0; kSe != numS; kSe++) {
       size_t kD = 0;
       for (std::vector<myItCl>::const_iterator itCl = fData.cbegin(); itCl != fData.cend(); itCl++, kD++) {
          std::vector<rb::SSDCluster>::const_iterator it = *itCl;
 	  const int kSt = it->Station();
 	  const int kSe = it->Sensor();
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
          // Let us 
//	  fResids[kSeT] = DBL_MAX;
          const double rmsStr = std::max(0.1, it->WgtRmsStrip()); // protect against some zero values for the RMS 
	  const double strip = it->WgtAvgStrip();
	  if (fDebugIsOn) std::cerr << " .... At Station " << kSt << " Sensor " << kSe << " Strip " << strip << " RMS " << rmsStr << std::endl;
	  if (rmsStr > 1000.) { continue; } // no measurement. 
	  const double rmsStrN = rmsStr/fOneOSqrt12;
          const double pitch = fEmVolAlP->Pitch(aView, kSt, kSe); 
	  const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!  Suspicious.. 
	  const double multScatErr =  fEmVolAlP->MultScatUncert(aView, kSt, kSe) * 120.0/std::abs(pars[4]);
	  const double unknownErr = fEmVolAlP->UnknownUncert(aView, kSt, kSe);
//	  const double zUgly = this->fZPos[kD];
	  const double zV = fEmVolAlP->ZPos(aView, kSt, kSe); 
	  if (fDebugIsOn) std::cerr << "  ....  So, at z " << zV << " for view " << aView <<  std::endl;
//	  if (std::abs(zV - zUgly) > 1.0) {
//	    std::cerr << " SSD3DTrackFitFCNAlgo1::operator(), problem with accessing Z coordinates, zUgly " 
//	              << zUgly << " kD " << kD << " Z Volatile " << zV <<  " fatal .. quit here and now...  " << std::endl; exit(2);
//	  }  
	  double xPred = 0.; double yPred = 0.; double tPred = 0.; double tMeas = 0.;
	  if (zV < zMag ) {
	    xPred = x0 + slx0*zV;
	    yPred = y0 + sly0*zV;
	  } else { 
	    xPred = x1 + slx1*(zV - fZLocDownstrMagnet);
	    yPred = y1 + sly1*(zV - fZLocDownstrMagnet);
	  }
	  const double uPred = fOneOverSqrt2 * ( -xPred + yPred);
	  const double vPred = -1.0*fOneOverSqrt2 * ( xPred + yPred);
	  // T coordinate (measuring 
	  switch (aView) {
	    case emph::geo::X_VIEW : {
	      tPred = xPred ; 
	      tPred += yPred * fEmVolAlP->Roll(aView, kSt, kSe);
	      break;
	    }
	    case emph::geo::Y_VIEW : {
	      tPred = yPred ; 
	      tPred += xPred * fEmVolAlP->Roll(aView, kSt, kSe);
	      break;
	    }
	    case emph::geo::U_VIEW : {
	      tPred = uPred ; 
	      tPred += vPred * fEmVolAlP->Roll(aView, kSt, kSe);
	      break;
	    }
	    case emph::geo::W_VIEW : {
	      tPred = vPred ; 
	      tPred += uPred * fEmVolAlP->Roll(aView, kSt, kSe);
	      break;
	    }
	    default : { continue; }
	    
	  } // end of transverse coordinate prediction. 
	  if (!fIsMC) { 
	    if (aView == emph::geo::X_VIEW) {
	      tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	      if (kSe >= 4) tMeas *= -1.0;
	      if (fDebugIsOn) 
	        std::cerr << " ..... X View " << " kSe " << kSe 
	  	      << " yPred " << yPred << " tPred " << tPred << " tMeas " << tMeas  << std::endl; 
	    } else if (aView == emph::geo::Y_VIEW) {
	      tMeas =  ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	      if (kSe >= 4) tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
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
	      if (fDebugIsOn) 
	        std::cerr << " ..... Coords  View " << aView << " kSe " << kSe 
	  	      << " xPred " << xPred << " yPred " <<  yPred << " uPred " << uPred << " vPred " << vPred << std::endl; 
	    }
	  } else { // Monte Carlo Convention 
	    if (aView == emph::geo::X_VIEW) {
	      tMeas = ( -1.0 * strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	      if ((kSt > 3) && (kSe % 2) == 1) tMeas *=-1;    // for now.. Need to keep checking this.. Shameful.   
	      if (fDebugIsOn) 
	        std::cerr << " ..... X View " << " kSe " << kSe 
	  	      << " xPred " << yPred << " tPred " << tPred << " tMeas " << tMeas  << std::endl; 
	    } else if (aView == emph::geo::Y_VIEW) {
	      tMeas = (kSe < 4) ? ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)) :
	                      ( strip*pitch - fEmVolAlP->TrPos(aView, kSt, kSe)) ;
	      if ((kSt > 3) && (kSe % 2) == 1) tMeas *=-1;      
	      if (fDebugIsOn) 
	        std::cerr << " ..... Y View " << " kSe " << kSe 
	  	      << " yPred " << yPred << " tPred " << tPred << " tMeas " << tMeas  << std::endl; 
	    } else if ((aView == emph::geo::U_VIEW) || (aView == emph::geo::W_VIEW))  { // V is a.k.a. W 
	      if (aView == emph::geo::U_VIEW) { 
	        tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	      } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	        tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)); // Unknown, this is a place holder. 
	        if ((kSe % 2) ==  1) tMeas *= -1.0;
	      }
	      if (fDebugIsOn) 
	        std::cerr << " ..... Coords  View " << aView << " kSe " << kSe 
	  	      << " xPred " << xPred << " yPred " <<  yPred << " uPred " 
		      << uPred << " vPred " << vPred << " tPred " << tPred <<  " tMeas " 
		      << tMeas << " trPos " << fEmVolAlP->TrPos(aView, kSt, kSe) << std::endl; 
	      }
	    }
	    double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr + unknownErr*unknownErr;
	    const double dt = (tPred - tMeas);
	    fResids[kSeT] = dt;
	    chi2 += (dt * dt )/tMeasErrSq;
            if (fDebugIsOn) std::cerr << " ....   ..... Index kSeT " << kSeT << " dt " << dt << " +- " 
	                             << std::sqrt(tMeasErrSq) << " current Chi2 " << chi2 << std::endl;
        } // on the SSD Clusters 
//      std::cerr << " SSD3DTrackFitFCNAlgo1::operator(), done chiSq " << chi2 << std::endl;
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
 	const int kPl = static_cast<size_t>(it->Sensor());
        emph::geo::SSDStation aStation = fEmgeo->GetSSDStation(kSt);
        TVector3 posSt = aStation.Pos();
        emph::geo::Detector aPlane = aStation.GetSSD(kPl);
        TVector3 posPl = aPlane.Pos();
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
 
