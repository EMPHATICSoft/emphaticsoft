////////////////////////////////////////////////////////////////////////
/// \brief   2D aligner, X-Z view or Y-Z view, indepedently from each others. 
///          Algorithm one.  Could beong to SSDCalibration, but, this aligner 
///          requires some crude track reconstruction, as it is based on track residuals, i
///          of SSD strip that are on too often. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>
//

#include "SSDReco/SSDHotChannelList.h" 
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/ConvertDigitToWCoordAutre.h" 

namespace emph {
  namespace ssdr {
  
    const double ConvertDigitToWCoordAutre::fOneOverSqrt12 = 1.0/std::sqrt(12.);
    const double ConvertDigitToWCoordAutre::fOneOverSqrt2 = 1.0/std::sqrt(2.);
     
     ConvertDigitToWCoordAutre::ConvertDigitToWCoordAutre(char aView) : 
       fDetGeoMapService(art::ServiceHandle<emph::dgmap::DetGeoMapService>()), fDetGeoMap(fDetGeoMapService->Map()),        
       fGeoService(art::ServiceHandle<emph::geo::GeometryService>()), fEmgeo(fGeoService->Geo()),        
       fIsMC(false), fAddAlignUncert(true), fIsReadyToGo(false), fView(aView), fDebugIsOn(false), 
       fMomentumIsSet(false), fEffMomentum(120.), fPitch(0.06), fHalfWaferHeight(0.5*static_cast<int>(fNumStrips)*fPitch), 
       fXBeamSpot(5.096333), fYBeamSpot(-9.179789), fSlXBeamSpot(-0.003039212), fSlYBeamSpot(0.009157169),
       // Mean values from alignment run 2113, run 7s1104_5. 
       fXCoeffAlignUncert(0.05), fYCoeffAlignUncert(0.025),
       fZCoords(fNumStations+4, 0.), fZLocShifts(fNumStations+4, 0.), 
       fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fNominalOffsets(fNumStations, 0.), fDownstreamGaps(2, 0.), fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0),
       fMultScatUncert120( fNumStations, 0.), fMultScatUncert(fNumStations, 0.), fOtherUncert(fNumStations, 0.),
       fPitchOrYawAngles(fNumStations, 0.),
       fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()),
       fLastXEncountered(0.), fLastYEncountered(0.) 
     { 
         if (aView == 'W') aView = 'V';
         if ((aView != 'X') && (aView != 'Y') && (aView != 'U') && (aView != 'V') && (aView != 'A')) {
	     std::cerr << " ConvertDigitToWCoordAutre, setting an unknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
        }
     }
     ConvertDigitToWCoordAutre::ConvertDigitToWCoordAutre(int aRunNum, char aView) : 
       fDetGeoMapService(art::ServiceHandle<emph::dgmap::DetGeoMapService>()), fDetGeoMap(fDetGeoMapService->Map()),        
       fGeoService(art::ServiceHandle<emph::geo::GeometryService>()), fEmgeo(fGeoService->Geo()),        
       fIsMC(false), fAddAlignUncert(true), fIsReadyToGo(false), fView(aView), fDebugIsOn(false), 
       fMomentumIsSet(false), fEffMomentum(120.), fPitch(0.06), fHalfWaferHeight(0.5*static_cast<int>(fNumStrips)*fPitch), 
       fXBeamSpot(5.096333), fYBeamSpot(-9.179789), fSlXBeamSpot(-0.003039212), fSlYBeamSpot(0.009157169),
       fZCoords(fNumStations+4, 0.), fZLocShifts(fNumStations+4, 0.), 
       fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fNominalOffsets(fNumStations, 0.), fDownstreamGaps(2, 0.), fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0),
       fMultScatUncert120( fNumStations, 0.), fMultScatUncert(fNumStations, 0.), fOtherUncert(fNumStations, 0.),
       fPitchOrYawAngles(fNumStations, 0.),
       fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), 
       fLastXEncountered(0.), fLastYEncountered(0.) 
     {
         fIsPhase1c =  (aRunNum > 1999);  
         if (aView == 'W') aView = 'V';
         if ((aView != 'X') && (aView != 'Y') && (aView != 'U') && (aView != 'V') && (aView != 'A')) {
	     std::cerr << " ConvertDigitToWCoordAutre, setting an unknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
        }
     }
     void ConvertDigitToWCoordAutre::SetForPhase1X() {
      if (!fIsPhase1c) return;
      // Place holder... 
     }
     void ConvertDigitToWCoordAutre::SetForMomentum(double p) {
       if (fMomentumIsSet || (std::abs(std::abs(fEffMomentum) - std::abs(p)) < 1.0e-3) ) {
//         std::cerr << " ssdr::SSDAlign3DUVAutre::SetForMomentum, already called, skip!!! " << std::endl;
	 return;
       }
       const double pRatio = 120.0 / p;
       fMagnetKick120GeV *= pRatio;
       for (size_t k=0; k != fMultScatUncert.size(); k++) fMultScatUncert[k] = fMultScatUncert120[k] * std::abs(pRatio);
       fEffMomentum = std::abs(p); 
       fMomentumIsSet = true;
     }
     void  ConvertDigitToWCoordAutre::InitializeAllCoords(const std::vector<double> &zCoords) {
       const size_t kStMax = (fIsPhase1c) ? 11 : 8; // kStMax is the number of X view (or Y view) of sensors. 
       if (zCoords.size() > fZCoords.size()) {
         std::cerr << "  ConvertDigitToWCoordAutre::InitailizeCoords Unexpected number of ZCoords " 
	           << zCoords.size() << " declared here " << fZCoords.size() << " fatal, and that is it! " << std::endl; exit(2); 
       }
       for (size_t k=0; k != kStMax; k++) fZCoords[k] = zCoords[k]; // check fZCoords size Which is oversized.. 
       std::cerr << " ConvertDigitToWCoordAutre::InitializeCoords ";
       for (size_t kSt=0; kSt != fZCoords.size(); kSt++) std::cerr << " " << fZCoords[kSt];
        std::cerr << std::endl;
       for (size_t k=0; k != fNumStations; k++) {
        if (std::abs(fZLocShifts[k]) > 1.0e-10) {
	   std::cerr << " ... For Station  " << k <<  " Shifting the Z position by " << fZLocShifts[k] << " from " << fZCoords[k];
	  fZCoords[k] += fZLocShifts[k]; 
	  std::cerr << " to " << fZCoords[k] << std::endl;
	}
	if (std::abs(fOtherUncert[k]) > 1.0e-10) {
	  std::cerr << " ....  For Station " << k <<  " Adding (in quadrature) transverse position uncertainty of  " 
	  << fOtherUncert[k] << " from " << fZCoords[k] << std::endl;
	}
       }
       switch (fView) { 
         case 'X' :
           for (size_t k=0; k != 4; k++) { 
//             fNominalOffsets[k] = fHalfWaferHeight +  fPitch; // xcoord is proportional to - strip number..v2b 
//             fNominalOffsets[k] = fHalfWaferHeight + 2.0*fPitch; // xcoord is proportional to - strip number.. v2c
             fNominalOffsets[k] = fHalfWaferHeight - fPitch; // xcoord is proportional to - strip number.. v2c
	   }
//	   fNominalOffsets[4] = 2.0*fHalfWaferHeight - fPitch; // To be checked.. v2b
//	   fNominalOffsets[5] = 2.0*fHalfWaferHeight + fPitch; // To be checked.. 
//	   fNominalOffsets[4] = 2.0*fHalfWaferHeight + 2.0*fPitch; // To be checked.. v2c Seems O.K. on one event 
//	   fNominalOffsets[5] = 2.0*fHalfWaferHeight + 2.0*fPitch; // To be checked.. v2c No good, too low.. 
//	   fNominalOffsets[5] = 2.0*fHalfWaferHeight + 4.0*fPitch; // To be checked.. v2d ???? Weird.. high statistics need.  No field. 
           fNominalOffsets[4] = 2.0*fHalfWaferHeight; fNominalOffsets[5] = 2.0*fHalfWaferHeight;
	    break;
	 case 'Y' :
           for (size_t k=0; k != 4; k++) { 
             fNominalOffsets[k] = -fHalfWaferHeight; // ycoord is proportional to  strip number.. 
	   }
	   for (size_t k=4; k != 6; k++)  {
	     fNominalOffsets[k] = 2.0*fHalfWaferHeight; // Using the 2nd sensor. Called Sensor 3 for Y I think... Y coord to trip number is flipped. 
	   }
           // 
           // Min amd maximum window.. To be defined.. Based on histogramming cluster positions.. 
           //
	   // Comment this out, that was for data, trial and error.. We should use the methos Select by View in the main module.. 
	   // 
	   // fMinStrips[0] = 250.; fMaxStrips[0] = 500.; 
	  // fMinStrips[1] = 260.; fMaxStrips[1] = 510.; 
	  // fMinStrips[2] = 275.; fMaxStrips[2] = 525.; 
	  // fMinStrips[3] = 275.; fMaxStrips[3] = 525.; 
	  // fMinStrips[4] = 525.; fMaxStrips[4] = 700.; // There are dead channels at lower strip count, distribution of strip choppy. 
	  // fMinStrips[5] = 490.; fMaxStrips[5] = 700.; 
	   break;
	   //
	   // Need to implement U & V 
	 case 'U' : 
	   fNominalOffsets[2] = -fHalfWaferHeight; // Sign to be checked !  
 	   fNominalOffsets[3] = -fHalfWaferHeight; // Sign to be checked ! 
	   break;
	 case 'V' : 
	   for (size_t k=4; k != 6; k++)  {
	     fNominalOffsets[k] = 2.0*fHalfWaferHeight; // 
	   }
	   break;
	   
      }
      //
      // fNominalOffsets is obsolete (August 2023), check this by setting them to DBL_MAX
      //
      for (size_t k=0; k!=fNominalOffsets.size(); k++)  fNominalOffsets[k] = DBL_MAX;
      
       // Setting of the uncertainties.  Base on G4EMPH, see g4gen_jobC.fcl, Should be valid for X and Y  But it does includes the target.
       //
       // Obtained in Dec 2022, with target in. 
       //
       /*
       fMultScatUncert[1] =  0.003201263;   
       fMultScatUncert[2] =  0.02213214;   
       fMultScatUncert[3] =  0.03676218;   
       fMultScatUncert[4] =  0.1022451;   
       fMultScatUncert[5] =  0.1327402; 
       */ 
       // fMultScatUncert[1] =  0.003830147;   
       // fMultScatUncert[2] =  0.01371613;	
       //fMultScatUncert[3] =  0.01947578;	
       // fMultScatUncert[4] =  0.05067243;  
       // fMultScatUncert[5] =  0.06630287;
       /*
       ** This is momentum dependent... Carefull..At 120 GeV, no target, one has: (after rejection of wide scattering event, presumably hadronic.) 
       */
       fMultScatUncert120[1] =  0.003201263;   
       fMultScatUncert120[2] =  0.00512;   
       fMultScatUncert120[3] =  0.0092;   
       fMultScatUncert120[4] =  0.0212;   
       fMultScatUncert120[5] =  0.0264; 
       if (fMultScatUncert120.size() > 5) fMultScatUncert120[6] =  0.04; //  
       for (size_t kSt=0; kSt != fMultScatUncert120.size(); kSt++) fMultScatUncert[kSt] = fMultScatUncert120[kSt];
       fIsReadyToGo = true;
        
     }
     
     std::pair<double, double> ConvertDigitToWCoordAutre::getTrCoord(std::vector<rb::SSDCluster>::const_iterator itCl, double pMomMultScatErr) const { 
        if (fIsPhase1c) return this->getTrCoord1c(itCl, pMomMultScatErr); 
 	const double strip = itCl->WgtAvgStrip();
        const double rmsStr = std::max(0.1, itCl->WgtRmsStrip()); // protect against some zero values for the RMS 
	if (rmsStr > 1000.) { return std::pair<double, double>(DBL_MAX, DBL_MAX); } // no measurement. 
	const double rmsStrN = rmsStr/fOneOverSqrt12;
	const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!  Suspicious.. 
	const size_t kSt = static_cast<size_t>(itCl->Station());
	const emph::geo::sensorView aView = itCl->View();
	const size_t kSe = static_cast<size_t> (itCl->Sensor()); // local to the station.  
        const double pitch = fEmVolAlP->Pitch(aView, kSt, kSe);	
	double tMeas = DBL_MAX; 
	const double multScatErr = fMultScatUncert[kSt]*120./pMomMultScatErr;
	const double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr;
	// The Alignment parameters are organized by view, 
	                 // with indics ranging from 0 to 7 (X & Y views) , 0 an1 (U) and 0-3 for W views. 
			 // Clone code.. This should belong to the converter.. 
	const int kS = (kSt > 3) ? 
	               (4 + (kSt-4)*2 + kSe % 2) : kSt; // in dex ranging from 0 to 7 (X or Y), inclusive, for Phase1b, list of sensors by view. 
	if (fDebugIsOn) std::cerr << " ConvertDigitToWCoordAutre::getTrCoord, for Station " 
	                          << itCl->Station() << " kse " << kSe  << " kS " << kS << " View " 
				  << aView << " strip " << strip
				  << " TrShift " << fEmVolAlP->TrPos(aView, kSt, kSe) << std::endl;
	if (aView == emph::geo::X_VIEW) {
	  tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	  if ((kSt == 2) || (kSt == 3)) tMeas =  -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe);
	  if ((kSt > 3) && (kSe % 2) == 1)  tMeas *= -1.0;
	} else if (aView == emph::geo::Y_VIEW) {
	  tMeas = (kS < 4) ? ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)) :
			     ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)); // Corrected, Sept 9, token NoTgt31Gev_ClSept_A1e_1o1_c10
	  if (((kSe % 2) == 0) && (kSt > 3)) tMeas *= -1.0;
	} else if ((aView == emph::geo::U_VIEW) || (aView == emph::geo::W_VIEW))  { // V is a.k.a. W 
	  if (kSt < 4) { // Sept 1- Sep5  attempt at sorting out orientations.. 
	    tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    if (fDebugIsOn) std::cerr << "  .... kSt " << kSt << "  pitch " << pitch << " strip " 
	                              << strip << " TrShift, again " << fEmVolAlP->TrPos(aView, kSt, kSe) << std::endl;
	  } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	  if (kS == 4) tMeas = (-strip*pitch - fEmVolAlP->TrPos(aView, kSt, kSe)); // Based on the analsis of run 1274. 
	  else if (kS == 5) tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	  else if (kS == 6) tMeas = (-strip*pitch - fEmVolAlP->TrPos(aView, kSt, kSe));
	  else if (kS == 7) tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)); // exploring... 
	  }
	}
	return std::pair<double, double>(tMeas, tMeasErrSq);
     } 
     //
     // clone for Phase1c.. Ugly, wasted code, but I need some slack,  code modifying will be easier. 
      std::pair<double, double> ConvertDigitToWCoordAutre::getTrCoord1c(std::vector<rb::SSDCluster>::const_iterator itCl, double pMomMultScatErr) const { 
	const double strip = itCl->WgtAvgStrip();
        const double rmsStr = std::max(0.1, itCl->WgtRmsStrip()); // protect against some zero values for the RMS 
	if (rmsStr > 1000.) { return std::pair<double, double>(DBL_MAX, DBL_MAX); } // no measurement. 
	const double rmsStrN = rmsStr/fOneOverSqrt12;
	const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!  Suspicious.. 
	const size_t kSt = static_cast<size_t>(itCl->Station());
        emph::geo::sensorView aView = itCl->View();
	if (aView == emph::geo::U_VIEW) aView = emph::geo::W_VIEW; //  February 2024..after discussing with Teresa.. 
	const size_t kSe = static_cast<size_t> (itCl->Sensor()); // local to the station.  
        const double pitch = fEmVolAlP->Pitch(aView, kSt, kSe);	
	double tMeas = DBL_MAX; 
	const double multScatErr = fMultScatUncert[kSt]*120./pMomMultScatErr;
	double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr;
	// The Alignment parameters are organized by view, 
	                 // with indics ranging from 0 to 7 (X & Y views) , 0 an1 (U) and 0-3 for W views. 
			 // Clone code.. This should belong to the converter.. 
	const int kS = (kSt > 4) ? 
	               (5 + (kSt-5)*2 + kSe % 2) : kSt; // in dex ranging from 0 to 7 (X or Y), inclusive, for Phase1b, list of sensors by view. 
	if (fDebugIsOn) std::cerr << " ConvertDigitToWCoordAutre::getTrCoord1c, for Station " 
	                          << itCl->Station() << " kse " << kSe  << " kS " << kS << " View " 
				  << aView << " strip " << strip
				  << " TrShift " << fEmVolAlP->TrPos(aView, kSt, kSe) << std::endl;
	if (aView == emph::geo::X_VIEW) {
	  tMeas =  ( -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	  if ((kSt == 2) || (kSt == 3)) tMeas =  -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe); // Same as above!. ? 
	  if (kSt > 4) {
	    if ((kSe % 2) == 0) tMeas =  strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe); 
	    else  tMeas =  -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe); 
	  }
	} else if (aView == emph::geo::Y_VIEW) {
	  tMeas = (kS < 5) ? ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)) :
			     ( strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe)); // Corrected, again, Jan 2 
	  if (((kSt > 4) && ((kSe % 2)) == 0)) tMeas *= -1.0; 
	} else if ((aView == emph::geo::U_VIEW) || (aView == emph::geo::W_VIEW))  { // V is a.k.a. W 
	  if (kSt < 4) { // Sept 1- Sep5  attempt at sorting out orientations.. 
	    tMeas = (strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe));
	    if (fDebugIsOn) std::cerr << "  .... kSt " << kSt << "  pitch " << pitch << " strip " 
	                              << strip << " TrShift, again " << fEmVolAlP->TrPos(aView, kSt, kSe) << std::endl;
	  } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	    // Assume W view, double sensor.  Opposite convention as X or Y  Dec. 1 2025.. 
	    if ((kSe % 2) == 0) tMeas =  -1.0*strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe); 
	    // Temporary kludge to study why Station 6 has bad 3pt Station chisq... 
//	    if ((kSt == 6) && (kSe % 2) == 1) tMeas +=  -0.4; // by eye, on 4 bad events..
	    else  tMeas =  strip*pitch + fEmVolAlP->TrPos(aView, kSt, kSe); 
	    	  
	  }
	}
	if (fAddAlignUncert) {
	 if (aView == emph::geo::X_VIEW) { 
	   const double tSys = this->getTrXAlignUncert(fZCoords[kSt], tMeas);
	   tMeasErrSq += tSys*tSys; fLastXEncountered = tMeas;
	 } else if (aView == emph::geo::X_VIEW) { 
	   const double tSys = this->getTrYAlignUncert(fZCoords[kSt], tMeas);
	   tMeasErrSq += tSys*tSys; fLastYEncountered = tMeas;
	 } else {
	   const double tSys = this->getTrUAlignUncert(fZCoords[kSt], fLastXEncountered, fLastYEncountered); // Very, very sloppy
	   tMeasErrSq += tSys*tSys;
	 }
	}
	return std::pair<double, double>(tMeas, tMeasErrSq);
     } 
     double ConvertDigitToWCoordAutre::getTrCoordRoot(std::vector<rb::SSDCluster>::const_iterator itCl) { 
      
      // Assumes no Rolls !... 
        //  Transverse position, nominal.. 
	 
 	const double strip = itCl->WgtAvgStrip();
        const double rmsStr = std::max(0.1, itCl->WgtRmsStrip()); // protect against some zero values for the RMS 
	if (rmsStr > 1000.) { return DBL_MAX; } // no measurement. 
	const emph::geo::sensorView aView = itCl->View();
	rb::LineSegment ls;
	// Got to do a copy.. 
        fDetGeoMap->SSDClusterToLineSegment(*itCl, ls);
	if (fDebugIsOn) std::cerr << " ConvertDigitToWCoordAutre::getTrCoordRoot line is " << ls << std::endl;
	switch (aView) {
	case emph::geo::X_VIEW : { return ls.X0().X(); }  
	case emph::geo::Y_VIEW :  { return ls.X0().Y() ; }  
	case emph::geo::U_VIEW : { 
	  const double xx0 = ls.X0().X(); const double yy0 = ls.X0().Y();
	  const double xx1 = ls.X1().X();  const double yy1 = ls.X1().Y();
	  const double x0 = fOneOverSqrt2 * ( xx0 - yy0); 
	  const double y0 = fOneOverSqrt2 * ( xx0 + yy0); 
	  const double x1 = fOneOverSqrt2 * ( xx1 - yy1); 
	  const double y1 = fOneOverSqrt2 * ( xx1 + yy1); 
	  if (fDebugIsOn) std::cerr << " ....  Rotate by -45 degrees ..pt 0  " << x0 << ", y0 " << y0 
				    << " pt1 " << x1 << ", y1 " << y1 << std::endl;
	  return 0.5*(x1+x0);		      
	}  
	case emph::geo::W_VIEW : { 
	  const double xx0 = ls.X0().X(); const double yy0 = ls.X0().Y();
	  const double xx1 = ls.X1().X();  const double yy1 = ls.X1().Y();
	  const double x0 = fOneOverSqrt2 * ( xx0 + yy0); 
	  const double y0 = fOneOverSqrt2 * ( -xx0 + yy0); 
	  const double x1 = fOneOverSqrt2 * ( xx1 + yy1); 
	  const double y1 = fOneOverSqrt2 * ( -xx1 + yy1); 
	  if (fDebugIsOn) std::cerr << " ....  Rotate by +45 degrees ..pt 0  " << x0 << ", y0 " << y0 
				    << " pt1 " << x1 << ", y1 " << y1 << std::endl;
	  return 0.5*(x1+x0);		      
	}  
	default :  { return emph::geo::INIT; } 
	} //On Views..  
	
     } 
  } // ssdr
} // emph
