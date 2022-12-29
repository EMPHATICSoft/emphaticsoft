////////////////////////////////////////////////////////////////////////
/// \brief   3D aligner, U and V sensor offset setting. 
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
#include "SSDReco/SSDAlign3DUVAlgo1.h" 

 using namespace emph;

namespace emph {
  namespace ssdr {

     SSDAlign3DUVAlgo1::SSDAlign3DUVAlgo1() :
       fSqrt2(std::sqrt(2.)), fOneOverSqrt2(1.0/std::sqrt(2.)), 
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fFilesAreOpen(false),
       fView('?'), fStation(2), fAlt45(false), fSensor(-1),
       fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10), fChiSqCut(20.), fChiSqCutXY(100.),
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoordXs(fNumStations, 0.), fZCoordYs(fNumStations, 0.), fZCoordUs(4, 0.), fZCoordVs(2, 0.),
       fNominalOffsetsX(fNumStations, 0.), fNominalOffsetsY(fNumStations, 0.), 
       fNominalOffsetsXAlt45(fNumStations, 0.), fNominalOffsetsYAlt45(fNumStations, 0.), 
       fNominalOffsetsU(4, 0.), fNominalOffsetsV(2, 0.),
       fFittedResidualsX(fNumStations, 0.), fFittedResidualsY(fNumStations, 0.),
       fPitchAngles(fNumStations, 0.), fPitchAnglesAlt(fNumStations, 0.), 
       fYawAngles(fNumStations, 0.), fYawAnglesAlt(fNumStations, 0.), 
       fRollAngles(fNumStations, 0.), fRollAnglesAlt(fNumStations, 0.),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.), fTrType(ssdr::NONE), 
       fTrXOffset(DBL_MAX), fTrYOffset(DBL_MAX), fTrXSlope(DBL_MAX), fTrYSlope(DBL_MAX), 
       fTrXOffsetErr(DBL_MAX), fTrYOffsetErr(DBL_MAX), fTrXSlopeErr(DBL_MAX), fTrYSlopeErr(DBL_MAX), 
       fTrXCovOffSl(DBL_MAX), fTrYCovOffSl(DBL_MAX), fChiSqX(DBL_MAX), fChiSqY(DBL_MAX),
       fNHitsXView(fNumStations, 0), fNHitsYView(fNumStations, 0) 
     { 
        ; 
     }
     SSDAlign3DUVAlgo1::SSDAlign3DUVAlgo1(char aView, int aStation, bool alt45) : 
       fSqrt2(std::sqrt(2.)), fOneOverSqrt2(1.0/std::sqrt(2.)), 
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fFilesAreOpen(false),
       fView(aView), fStation(aStation), fAlt45(alt45), 
       fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10),fChiSqCut(20.), fChiSqCutXY(100.),
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoordXs(fNumStations, 0.), fZCoordYs(fNumStations, 0.), fZCoordUs(4, 0.), fZCoordVs(2, 0.),
       fNominalOffsetsX(fNumStations, 0.), fNominalOffsetsY(fNumStations, 0.), 
       fNominalOffsetsXAlt45(fNumStations, 0.), fNominalOffsetsYAlt45(fNumStations, 0.), 
       fNominalOffsetsU(4, 0.), fNominalOffsetsV(2, 0.),
       fFittedResidualsX(fNumStations, 0.),  fFittedResidualsY(fNumStations, 0.),
       fPitchAngles(fNumStations, 0.), fPitchAnglesAlt(fNumStations, 0.), 
       fYawAngles(fNumStations, 0.), fYawAnglesAlt(fNumStations, 0.), 
       fRollAngles(fNumStations, 0.), fRollAnglesAlt(fNumStations, 0.),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.), fTrType(ssdr::NONE),
       fTrXOffset(DBL_MAX), fTrYOffset(DBL_MAX), fTrXSlope(DBL_MAX), fTrYSlope(DBL_MAX), 
       fTrXOffsetErr(DBL_MAX), fTrYOffsetErr(DBL_MAX), fTrXSlopeErr(DBL_MAX), fTrYSlopeErr(DBL_MAX), 
       fTrXCovOffSl(DBL_MAX), fTrYCovOffSl(DBL_MAX), fChiSqX(DBL_MAX), fChiSqY(DBL_MAX),
       fNHitsXView(fNumStations, 0), fNHitsYView(fNumStations, 0) 
       
     { 
        if ((aView != 'U') && (aView != 'V')) {
	     std::cerr << " SSDAlign3DUVAlgo1, setting an unknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	}
	if (aStation < 2 ) {
	     std::cerr << " SSDAlign3DUVAlgo1, Station " << aStation << " has no U nor V views,  fatal, quit here " << std::endl; 
	     exit(2);
	}
	switch (aStation) {
	  case 2:
	    fSensor = 2; // I think need to check.. 
	    break;
	  case 3:
	    fSensor = 2; // I think need to check.. 
	    break;
	  case 4:
	    fSensor = 5; // to be determined. 
	    if (fAlt45) fSensor = 4;
	    break;
	  case 5:
	    fSensor = 5; // same  
	    if (fAlt45) fSensor = 4;
	    break;
	 }   
     }
     void SSDAlign3DUVAlgo1::initTrackParams() {
            fTrXOffset=DBL_MAX; fTrYOffset=DBL_MAX; fTrXSlope=DBL_MAX; fTrYSlope=DBL_MAX; 
       fTrXOffsetErr=DBL_MAX; fTrYOffsetErr=DBL_MAX; fTrXSlopeErr=DBL_MAX; fTrYSlopeErr=DBL_MAX; 
       fChiSqX=DBL_MAX; fChiSqY=DBL_MAX; fTrXCovOffSl= DBL_MAX; fTrYCovOffSl= DBL_MAX;
     }
     void SSDAlign3DUVAlgo1::InitializeCoords(bool lastIs4, const std::vector<double> &zCoordXs, const std::vector<double> &zCoordYs,
	                                     const std::vector<double> &zCoordUs, const std::vector<double> &zCoordVs)
      {
     
        if (lastIs4)  {
	  std::cerr << " SSDAlign3DUVAlgo1::InitializeCoords, unsupported option alt45, quit here and now " << std::endl;
	  exit(2);
       }
       if ((zCoordXs.size() != 8) || (zCoordYs.size() != 8)) {
         std::cerr << "  SSDAlign3DUVAlgo1::InitailizeCoords Unexpected number of ZCoords, " 
	           << zCoordXs.size() << " (or Y)  fatal, and that is it! " << std::endl; exit(2); 
       }
       for (size_t k=0; k != 5; k++) fZCoordXs[k] = zCoordXs[k]; 
       fZCoordXs[5] = zCoordXs[7];
       for (size_t k=0; k != 5; k++) fZCoordYs[k] = zCoordYs[k]; 
       fZCoordYs[5] = zCoordYs[7];
       myLinFitX.SetZCoords(fZCoordXs);
       myLinFitY.SetZCoords(fZCoordYs);
       if (zCoordUs.size() != 4) {
         std::cerr <<  " SSDAlign3DUVAlgo1::InitializeCoords,, unexpected number of U planes. should be 4 of them, we have " 
	           <<  zCoordUs.size()  << " fatal, here and now " << std::endl;
		   exit(2);
       }
       if (zCoordVs.size() != 2) {
         std::cerr <<  " SSDAlign3DUVAlgo1::InitializeCoords,, unexpected number of V planes. should be 2 of them, we have " 
	           <<  zCoordVs.size()  << " fatal, here and now " << std::endl;
		   exit(2);
       }
       for (size_t k = 0; k != fZCoordUs.size(); k++) fZCoordUs[k] = zCoordUs[k];  
       for (size_t k = 0; k != fZCoordVs.size(); k++) fZCoordVs[k] = zCoordVs[k];  
       
       // Nominal Offsets for X and Y are identical.  We will have to define something equivalen for U and V 
       // mean while 
       
       for (size_t k=0; k != 4; k++) {
        fNominalOffsetsX[k] = fHalfWaferWidth;
        fNominalOffsetsY[k] = -fHalfWaferWidth;
      } 
       for (size_t k=4; k != 6; k++) {
        fNominalOffsetsX[k] = 2.0*fHalfWaferWidth;
        fNominalOffsetsY[k] = 2.0*fHalfWaferWidth;
       } 
       for (size_t k=0; k != 4; k++) {
        fNominalOffsetsU[k] = -fHalfWaferWidth;
        if (k < 2) fNominalOffsetsV[k] = -2.0*fHalfWaferWidth; // To be checked.. 
      } 
       
       
//       std::cerr << " SSDAlign3DUVAlgo1::InitailizeCoords  " << std::endl;
       /*
       * To be reviewd.. for wide angle tracks, on needs to Z position of the sensors. 
       * 
       for (size_t k=0; k != fNumStations; k++) {
        if (std::abs(fZLocShifts[k]) > 1.0e-10) {
	   std::cerr << " ... For Station  " << k <<  " Shifting the Z position by " << fZLocShifts[k] << " from " << fZCoords[k];
	  fZCoords[k] += fZLocShifts[k]; 
	  std::cerr << " to " << fZCoords[k] << std::endl;
	}
	if (std::abs(fOtherUncert[k]) > 1/0e-10) {
	  std::cerr << " ....  For Station " << k <<  " Adding (in quadrature) transverse position uncertainty of  " 
	  << fOtherUncert[k] << " from " << fZCoords[k] << std::endl;
	}
       }
       for ( 
         case 'X' :
           for (size_t k=0; k != 4; k++) { 
             fNominalOffsets[k] = fHalfWaferWidth; // xcoord is proportional to - strip number..
//             fNominalOffsetsAlt45[k] = fHalfWaferWidth; // xcoord is proportional to - strip number..Not sure.. 
	   }
	   for (size_t k=4; k != 6; k++) { 
	       fNominalOffsets[k] = 2.0*fHalfWaferWidth; // To be checked.. 
//               fNominalOffsetsAlt45[k] = -2.0*fHalfWaferWidth; // xcoord is proportional to - strip number.. To be checked.. 
	    }
	    break;
	 case 'Y' :
           for (size_t k=0; k != 4; k++) { 
             fNominalOffsets[k] = -fHalfWaferWidth; // ycoord is proportional to  strip number.. 
             fNominalOffsetsAlt45[k] = fHalfWaferWidth; // ycoord is proportional to - strip number..
	   }
	   for (size_t k=4; k != 6; k++)  {
	     fNominalOffsets[k] = 2.0*fHalfWaferWidth; // Using the 2nd sensor. Called Sensor 3 for Y I think... Y coord to trip number is flipped. 
             fNominalOffsetsAlt45[k] = -2.0*fHalfWaferWidth; // ycoord is proportional to +  strip number.. Down sensor 
	   }
           // 
           // Min amd maximum window.. To be defined.. Based on histogramming cluster positions.. 
           //
	   fMinStrips[0] = 250.; fMaxStrips[0] = 500.; 
	   fMinStrips[1] = 260.; fMaxStrips[1] = 510.; 
	   fMinStrips[2] = 275.; fMaxStrips[2] = 525.; 
	   fMinStrips[3] = 275.; fMaxStrips[3] = 525.; 
	   fMinStrips[4] = 525.; fMaxStrips[4] = 700.; // There are dead channels at lower strip count, distribution of strip choppy. 
	   fMinStrips[5] = 490.; fMaxStrips[5] = 700.; 
	   break;
       }
       // Setting of the uncertainties.  Base on G4EMPH, see g4gen_jobC.fcl, Should be valid for X and Y  
       */
       
       fMultScatUncert[1] =  0.003201263;   
       fMultScatUncert[2] =  0.02213214;   
       fMultScatUncert[3] =  0.03676218;   
       fMultScatUncert[4] =  0.1022451;   
       fMultScatUncert[5] =  0.1327402;  
    
     }
     
     SSDAlign3DUVAlgo1::~SSDAlign3DUVAlgo1() {
      if (fFOutXY.is_open()) fFOutXY.close();
      if (fFOutXYU.is_open()) fFOutXYU.close();
      if (fFOutXYV.is_open()) fFOutXYV.close();
     }
     
     void ssdr::SSDAlign3DUVAlgo1::openOutputCsvFiles() {
       std::ostringstream fNameXYStrStr, fNameXYUStrStr, fNameXYVStrStr; 
       fNameXYStrStr << "SSDAlign3DXY_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameXYStr(fNameXYStrStr.str());
       fFOutXY.open(fNameXYStr.c_str());
       fFOutXY<< " spill evt trType nHitsT ";
       for (size_t kSt=0; kSt != fNumStations; kSt++) fFOutXY << "nHX" << kSt << " ";
       for (size_t kSt=0; kSt != fNumStations; kSt++) fFOutXY << "nHY" << kSt << " ";
       fFOutXY << "xOff xOffErr xSl xSlErr chiSqX yOff yOffErr ySl ySlErr chiSqY " << std::endl;
       //
       // Similar, fFoutXYU and V .. We skip the track definition.. 
       //
       fNameXYUStrStr << "SSDAlign3DXYU_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameXYUStr(fNameXYUStrStr.str());
       fFOutXYU.open(fNameXYUStr.c_str());
       fFOutXYU << " spill evt kSt iHU  xPred xPredErr yPred yPredErr xObs yObs chiSq " << std::endl;
       fNameXYVStrStr << "SSDAlign3DXYV_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameXYVStr(fNameXYVStrStr.str());
       fFOutXYV.open(fNameXYVStr.c_str());
       fFOutXYV << " spill evt kSt iHV xPred xPredErr yPred yPredErr xObs yObs chiSq " << std::endl;
     } 

     //
     // This is cloned code!.. See SSDAlign2DXY to be cleaned up. 
     //
     double ssdr::SSDAlign3DUVAlgo1::GetTsFromCluster(char aView, size_t kStation,  double strip, bool getX) const 
     {
          const bool alternate45 = false; // we will use only 
	  double aVal = 0.;
	   switch (aView) { // see SSDCalibration/SSDCalibration_module 
	     case 'X' :
	     {
	       if (kStation < 4) {
	         aVal =  ( -1.0*strip*fPitch + fNominalOffsetsX[kStation] + fFittedResidualsX[kStation]);
	       } else {
	         if (!alternate45) aVal =  strip*fPitch - fNominalOffsetsX[kStation] +  fFittedResidualsX[kStation];
		 else aVal =  -1.0*strip*fPitch + fNominalOffsetsXAlt45[kStation] +  fFittedResidualsX[kStation];
		 // Momentum correction, for 120 GeV primary beam  
		 aVal += fMagnetKick120GeV * (fZCoordXs[kStation] - fZCoordsMagnetCenter);
		 // Yaw Correction 
	       }
	       return aVal;
	       break;
	     } 
	     case 'Y' : 
	      {
	       if (kStation < 4) {  
	         aVal =  (strip*fPitch + fNominalOffsetsY[kStation]  + fFittedResidualsY[kStation]);
	       } else {
	         if (!alternate45) aVal =  ( -1.0*strip*fPitch + fNominalOffsetsY[kStation] + fFittedResidualsY[kStation]);
		 else aVal =  ( strip*fPitch + fNominalOffsetsYAlt45[kStation] + fFittedResidualsY[kStation]);
	       } 
//	       const double aValC = this->correctTsForPitchOrYawAngle(kStation, aVal);  To be implemente later 
	       return aVal;
	       break; 
	      }
	      case 'U' :
	      {
	       if (kStation < 4) { 
	         if (kStation == 2) { // no fitted Residual in this version, as this is what this class is about to determine  
		    if (getX) aVal = fOneOverSqrt2 * (2.3483*strip*fPitch + fNominalOffsetsU[kStation-2]) - 21.7863; 
		    else aVal = fOneOverSqrt2 * (2.1722*strip*fPitch + fNominalOffsetsU[kStation-2]) - 12.69688; 
		 } if (kStation == 3) {
		    if (getX) aVal = fOneOverSqrt2 * (2.40*strip*fPitch + fNominalOffsetsU[kStation-2]) - 22.401; 
		    else aVal = fOneOverSqrt2 * (2.199*strip*fPitch + fNominalOffsetsU[kStation-2]) - 12.9302; 
		 }
	       } else {
	         aVal =  fOneOverSqrt2 *  ( strip*fPitch + fNominalOffsetsU[kStation-2]); // sign convention to be reviewed. Irrelevant, no data ( for 1055 at least..)  
	       }
	       return aVal;
	      } 
	      case 'V' :
	      {
	         if (kStation == 4) { // no fitted Residual in this version, Adding constant to get the right relation.. For run 1055 only! 
	           if (getX) aVal =  fOneOverSqrt2 *  ( 1.9937*strip*fPitch + fNominalOffsetsV[kStation-2]) - 49.6982; // sign convention to be reviewed. 
	           else aVal = fOneOverSqrt2 *  ( -1.9756*strip*fPitch - fNominalOffsetsV[kStation-2]) + 49.5511; 
		 }
	         if (kStation == 5) { // Crazy values for station 5.. 
	           if (getX) aVal =  fOneOverSqrt2 *  ( 6.0910*strip*fPitch + fNominalOffsetsV[kStation-2]) - 162.516; // sign convention to be reviewed. 
	           else aVal = fOneOverSqrt2 *  ( -10.6832*strip*fPitch - fNominalOffsetsV[kStation-2]) + 282.8606; 
		 }
		                                                                      // sign convention to be reviewed.
	        return aVal;
	      } 
	      default :
	        std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected view, " 
		<< fView << " kStation " << kStation << 
		 " internal error, fatal " << std::endl; exit(2);
	   }
	   return 0.; // should not happen  
       }
       
      bool ssdr::SSDAlign3DUVAlgo1::recoXY(rb::planeView theView, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
//
        if ((theView != rb::X_VIEW) && (theView != rb::Y_VIEW)) {
	  std::cerr << " SSDAlign3DUVAlgo1::recoXY, unexpected view enum rb::planeView " << theView << " expect X or Y, fatal, I said it " << std::endl;
	  exit(2);
	}
        const int numMaxH2345 = 3; // Could become a run time parameter. Not yet!.. loops below don't allow it
        bool debugIsOn = (fEvtNum < 25); 
        std::vector<int> nHits(fNumStations, 0); int nHitsTotal = aSSDClsPtr->size();
	if (debugIsOn && (theView == rb::X_VIEW)) std::cerr << " SSDAlign3DUVAlgo1::recoXX. X view, at event " 
	                         << fEvtNum << " number of clusters " << nHitsTotal <<  std::endl;
	if (debugIsOn && (theView == rb::Y_VIEW)) std::cerr << " SSDAlign3DUVAlgo1::recoXX. Y view, at event " 
	                         << fEvtNum << " number of clusters " << nHitsTotal <<  std::endl;
//
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsFirst(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs2nd(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs3rd(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs4rth(fNumStations, aSSDClsPtr->cend());
	if (theView == rb::X_VIEW) for(size_t kSt=0; kSt != fNumStations; kSt++) fNHitsXView[kSt] = 0;
	else for(size_t kSt=0; kSt != fNumStations; kSt++) fNHitsYView[kSt] = 0;  	 
        for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != theView) continue;
	  const size_t kSt = static_cast<size_t>(itCl->Station());
	  nHits[kSt]++;
	  if (kSt >= fNumStations ) continue; // should not happen  
	  if (theView == rb::X_VIEW) fNHitsXView[kSt]++; 
	  else fNHitsYView[kSt]++; 
	  // Require on and only one cluster for the first two station. 
	  if (itCl->Station() < 2) {
	    if (fNHitsXView[kSt] > 1) {
	      if (debugIsOn) std::cerr << " More than one hits for station " << kSt << ", return false " << std::endl;
	      return false;
	    }
	  }
	  if (nHits[kSt] > numMaxH2345) {
	      if (debugIsOn) std::cerr << " More than " << numMaxH2345 << "  hits for station " << kSt << ", return false " << std::endl;
	      return false;
	  }
	  if (mySSDClsPtrsFirst[kSt] == aSSDClsPtr->cend()) mySSDClsPtrsFirst[kSt] = itCl;
	  else {
	    if (mySSDClsPtrs2nd[kSt] == aSSDClsPtr->cend()) mySSDClsPtrs2nd[kSt] = itCl;
	    else {
	      if (mySSDClsPtrs3rd[kSt] == aSSDClsPtr->cend()) mySSDClsPtrs3rd[kSt] = itCl; // etc... 
	    }
	  }  
        }
	int numMiss = 0;
	for (size_t kSt=0; kSt != fNumStations; kSt++) {
	  if (mySSDClsPtrsFirst[kSt] == aSSDClsPtr->cend()) numMiss++;
	}
	if (numMiss > 1) {
	  if (debugIsOn) std::cerr << " .... Too many missing hits =  " << numMiss << std::endl;
	  return false;
	}
        if (theView == rb::X_VIEW) myLinFitX.resids = std::vector<double>(fNumStations, 0.);
        else myLinFitY.resids = std::vector<double>(fNumStations, 0.);
	std::vector<double> tsData(fNumStations, 0.);
        std::vector<double> tsDataErr(fNumStations, 3.0*fPitch); //

        // Assume we have one and only hits in the first two stations. 
	
	for (size_t kSt = 0; kSt != fNumStations; kSt++) {
	  if (mySSDClsPtrsFirst[kSt] == aSSDClsPtr->cend()) {
	    tsDataErr[kSt] = 100.0;
	    if (debugIsOn) std::cerr << " .... No hits for station " << kSt << std::endl;
	  } else {
	    double aStrip = mySSDClsPtrsFirst[kSt]->WgtAvgStrip();
	    if (theView == rb::X_VIEW) tsData[kSt] = GetTsFromCluster('X', kSt, aStrip);
	    else tsData[kSt] = GetTsFromCluster('Y', kSt, aStrip);
	    tsDataErr[kSt] = GetTsUncertainty(kSt, mySSDClsPtrsFirst[kSt]);
	    if (debugIsOn) {
	       if (theView == rb::X_VIEW) std::cerr << " .... For station " << kSt << " strip " << aStrip 
	                                   << " ts " << tsData[kSt] << " +- " << tsDataErr[kSt] << " fitted Resid " << fFittedResidualsX[kSt] <<  std::endl;
	       else  std::cerr << " .... For station " << kSt << " strip " << aStrip 
	                                   << " ts " << tsData[kSt] << " +- " << tsDataErr[kSt] << " fitted Resid " << fFittedResidualsY[kSt] <<  std::endl;
	    }
	  }
	}
	if (theView == rb::X_VIEW) { 
          myLinFitX.fitLin(false, tsData, tsDataErr);
          if (myLinFitX.chiSq > fChiSqCutXY) {
	    if (debugIsOn) {
	       std::cerr << " Unacceptable chi-Sq = " << myLinFitX.chiSq << " need to look at second hits.. We will try it out  " << std::endl;
//	     return false; See blow.. 
	    } 
          } else {
	    fTrXOffset = myLinFitX.offset;
	    fTrXOffsetErr = myLinFitX.sigmaOffset;
	    fTrXSlope =  myLinFitX.slope;
	    fTrXSlopeErr =  myLinFitX.sigmaSlope;
	    fChiSqX = myLinFitX.chiSq;
	    fTrXCovOffSl = myLinFitX.covOffsetSlope;
	    if (debugIsOn) 
	       std::cerr << " Acceptable chi-Sq = " << myLinFitX.chiSq << " Track Offset " << fTrXOffset << "  Slope " << fTrXSlope << std::endl;
	    return true;
	  }
	} else {
	
          myLinFitY.fitLin(false, tsData, tsDataErr);
          if (myLinFitY.chiSq > fChiSqCutXY) {
	    if (debugIsOn) {
	       std::cerr << " Unacceptable chi-Sq = " << myLinFitY.chiSq << " need to look at second hits.. We will try it out  " << std::endl;
//	     return false; See blow.. 
	    } 
          } else {
	    fTrYOffset = myLinFitY.offset;
	    fTrYOffsetErr = myLinFitY.sigmaOffset;
	    fTrYSlope =  myLinFitY.slope;
	    fTrYSlopeErr =  myLinFitY.sigmaSlope;
	    fChiSqY = myLinFitY.chiSq;
	    fTrYCovOffSl = myLinFitY.covOffsetSlope;
	    if (debugIsOn) 
	       std::cerr << " Acceptable chi-Sq = " << myLinFitY.chiSq << " Track Offset " << fTrYOffset << "  Slope " << fTrYSlope << std::endl;
	    return true;
	  }
	}
	// Looking for alternate hits. 
	if (debugIsOn) std::cerr << " Looking for 2nd and 3rd hit, Possibly big combinatorial problem " << std::endl;
	std::vector<double> tsDataAlt(tsData); 
	std::vector<double> tsDataAltErr(tsDataErr);
	double chiSqBest = DBL_MAX;
	//
	for (int i2 = 0; i2 != numMaxH2345; i2++) {
	  std::vector<rb::SSDCluster>::const_iterator itcl2 = mySSDClsPtrsFirst[2];	 
	  if (i2 == 1) itcl2 = mySSDClsPtrs2nd[2];
	  if (i2 == 2) itcl2 = mySSDClsPtrs3rd[2];
	  if (itcl2 == aSSDClsPtr->cend()) continue; //do not allow for missing hits... 
	  double aStrip2 = itcl2->WgtAvgStrip();
	  if (theView == rb::X_VIEW) tsDataAlt[2] = GetTsFromCluster('X', 2, aStrip2);
	  else tsDataAlt[2] = GetTsFromCluster('Y', 2, aStrip2);
	  tsDataAltErr[2] = GetTsUncertainty(2, itcl2);
	  
	 
	  for (int i3 = 0; i3 != numMaxH2345; i3++) {
	    std::vector<rb::SSDCluster>::const_iterator itcl3 = mySSDClsPtrsFirst[3];	 
	    if (i3 == 1) itcl3 = mySSDClsPtrs2nd[3];
	    if (i3 == 2) itcl3 = mySSDClsPtrs3rd[3];
	    if (itcl3 == aSSDClsPtr->cend()) continue;
	    double aStrip3 = itcl3->WgtAvgStrip();
	    if (theView == rb::X_VIEW) tsDataAlt[3] = GetTsFromCluster('X', 3, aStrip3);
	    else tsDataAlt[3] = GetTsFromCluster('X', 3, aStrip3);
	    tsDataAltErr[3] = GetTsUncertainty(3, itcl3);
	    
	    for (int i4 = 0; i4 != numMaxH2345; i4++) {
	      std::vector<rb::SSDCluster>::const_iterator itcl4 = mySSDClsPtrsFirst[4];	 
	      if (i4 == 1) itcl4 = mySSDClsPtrs2nd[4];
	      if (i4 == 2) itcl4 = mySSDClsPtrs3rd[4];
	      if (itcl4 == aSSDClsPtr->cend()) continue;
	      double aStrip4 = itcl4->WgtAvgStrip();
	      if (theView == rb::X_VIEW) tsDataAlt[4] = GetTsFromCluster('X', 4, aStrip4);
	      else tsDataAlt[4] = GetTsFromCluster('Y', 4, aStrip4);
	      tsDataAltErr[4] = GetTsUncertainty(4, itcl4);
	    
	      for (int i5 = 0; i5 != numMaxH2345; i5++) {
	        std::vector<rb::SSDCluster>::const_iterator itcl5 = mySSDClsPtrsFirst[5];	 
	        if (i5 == 1) itcl5 = mySSDClsPtrs2nd[5];
	        if (i5 == 2) itcl5 = mySSDClsPtrs3rd[5];
	        if (itcl5 == aSSDClsPtr->cend()) continue;
	        double aStrip5 = itcl5->WgtAvgStrip();
	        if (theView == rb::X_VIEW) tsDataAlt[5] = GetTsFromCluster('X', 5, aStrip5);
		else tsDataAlt[5] = GetTsFromCluster('Y', 5, aStrip5);
	        tsDataAltErr[5] = GetTsUncertainty(5, itcl5);
		
		if (theView == rb::X_VIEW) {  
                  myLinFitX.fitLin(false, tsDataAlt, tsDataAltErr);		
                  if (myLinFitX.chiSq < chiSqBest) {
	            fTrXOffset = myLinFitX.offset;
	            fTrXOffsetErr = myLinFitX.sigmaOffset;
	            fTrXSlope =  myLinFitX.slope;
	            fTrXSlopeErr =  myLinFitX.sigmaSlope;
	            fTrXCovOffSl = myLinFitX.covOffsetSlope;
	            if (debugIsOn) 
	               std::cerr << "Best chi-Sq = " << myLinFitX.chiSq << " so fat.. with Track Offset " 
		                 << fTrXOffset << "  Slope " << fTrXSlope << std::endl;
		  }
	        } else {
                  myLinFitY.fitLin(false, tsDataAlt, tsDataAltErr);		
                  if (myLinFitY.chiSq < chiSqBest) {
	            fTrYOffset = myLinFitY.offset;
	            fTrYOffsetErr = myLinFitY.sigmaOffset;
	            fTrYSlope =  myLinFitY.slope;
	            fTrYSlopeErr =  myLinFitY.sigmaSlope;
	            fTrYCovOffSl = myLinFitY.covOffsetSlope;
	            if (debugIsOn) 
	               std::cerr << "Best chi-Sq = " << myLinFitY.chiSq << " so fat.. with Track Offset " 
		                 << fTrYOffset << "  Slope " << fTrYSlope << std::endl;
		
		  }
	        } 
	      } // on hits from Station 5   
	   
	    } // on hits from Station 4   
	   
	 } // on hits from Station 3   
	   
	} // on hits from station 2. 
      
       if (chiSqBest < fChiSqCutXY) {
          if (theView == rb::X_VIEW) { 
	    fChiSqX = chiSqBest;
	    if (debugIsOn) 
	       std::cerr << " Acceptable X fit chi-Sq = " << chiSqBest << " Track Offset " << fTrXOffset << "  Slope " << fTrXSlope << std::endl;
	    return true;
	  } else {
	    fChiSqY = chiSqBest;
	    if (debugIsOn) 
	       std::cerr << " Acceptable Y fit chi-Sq = " << chiSqBest << " Track Offset " << fTrYOffset << "  Slope " << fTrYSlope << std::endl;
	    return true;
	  }
       }
    
       return false;
     }

     bool ssdr::SSDAlign3DUVAlgo1::checkUV(rb::planeView view, size_t kStation, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
       bool debugIsOn = (fEvtNum < 25);    
     
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::checkUV, view Index " << view << " Station " << kStation 
                 << " nunH " << aSSDClsPtr->size() << "  working on it.. " << std::endl;
	
       if (kStation < 2) {
         std::cerr << " SSDAlign3DUVAlgo1::checkUV, " << view << " Unexpected Station number = " << kStation << " fatal mistake! " << std::endl; 
	 exit(2);
       } 
       if ((view  == rb::W_VIEW) && (kStation < 4)) {
         std::cerr << " SSDAlign3DUVAlgo1::checkUV, " << view << " Unexpected Station number = " << kStation << " fatal mistake! " << std::endl; 
	 exit(2);
       } 

       size_t kStEff = kStation - 2;
       if (view  == rb::W_VIEW) kStEff = kStation - 4;
       	 
       double xPred, yPred, xPredErrSq, yPredErrSq;
       char cView = '?';
       switch (view) {
         case rb::U_VIEW :
	 {
	   xPred = fTrXOffset + fZCoordUs[kStEff]*fTrXSlope;
	   xPredErrSq = fTrXOffsetErr*fTrXOffsetErr + fZCoordUs[kStEff]*fTrXSlopeErr*fZCoordUs[kStEff]*fTrXSlopeErr 
	                + fZCoordUs[kStEff]*fTrXCovOffSl;
	   yPred = fTrYOffset + fZCoordUs[kStEff]*fTrYSlope;
	   yPredErrSq = fTrYOffsetErr*fTrYOffsetErr + fZCoordUs[kStEff]*fTrYSlopeErr*fZCoordUs[kStEff]*fTrYSlopeErr 
	                + fZCoordUs[kStEff]*fTrYCovOffSl;
	   cView = 'U';
	   break;
	 } 
	 case rb::W_VIEW :
	 {
	   xPred = fTrXOffset + fZCoordVs[kStEff]*fTrXSlope;
	   xPredErrSq = fTrXOffsetErr*fTrXOffsetErr + fZCoordVs[kStEff]*fTrXSlopeErr*fZCoordVs[kStEff]*fTrXSlopeErr 
	                + fZCoordVs[kStEff]*fTrXCovOffSl;
	   yPred = fTrYOffset + fZCoordVs[kStEff]*fTrYSlope;
	   yPredErrSq = fTrYOffsetErr*fTrYOffsetErr + fZCoordVs[kStEff]*fTrYSlopeErr*fZCoordVs[kStEff]*fTrYSlopeErr 
	                + fZCoordVs[kStEff]*fTrYCovOffSl;
	 
	   cView = 'V';
	   break;
	 }
	 default:
	 { 
           std::cerr << " SSDAlign3DUVAlgo1::checkUV, Unexpected " << view << " Station number = " << kStation << " fatal mistake! " << std::endl; 
	   exit(2);
         } 
       }
       if (debugIsOn) {
           if (xPredErrSq < 0.) std::cerr << " .... ???? Negative predicted uncertainty for X , problem with covariance of linFit " << std::endl;
           if (yPredErrSq < 0.) std::cerr << " .... ???? Negative predicted uncertainty for Y , problem with covariance of linFit " << std::endl;
           std::cerr << " .... xPred " << xPred << " +- " << std::sqrt(std::abs(xPredErrSq)) 
                                << "  yPred " << yPred << " +- " << std::sqrt(std::abs(yPredErrSq)) << std::endl;
       }
       double chiBest = DBL_MAX; int ihSelBest = INT_MAX;
       double xObsBest = DBL_MAX; double yObsBest = DBL_MAX;
       int ihSel = 0;
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != view) continue;
	  const size_t kSt = static_cast<size_t>(itCl->Station());
          if (kSt != kStation) continue;
          double aStrip = itCl->WgtAvgStrip();
	  double xObs = this->GetTsFromCluster(cView, kStation, aStrip, true);  // Twice th strip  strip ? 
	  double yObs = this->GetTsFromCluster(cView, kStation, aStrip, false);
	  double tsUncert = this->GetTsUncertainty(kStation, itCl);
	  double tsXErrSq = xPredErrSq + tsUncert*tsUncert; 	   
	  double tsYErrSq = yPredErrSq + tsUncert*tsUncert; 	   
          double dx = xObs - xPred;
	  double dy = yObs - yPred;
	  double chi= (dx*dx/tsXErrSq + dy*dy/tsYErrSq);
	  if (debugIsOn) std::cerr << " ... strip " << aStrip << " XObs " << xObs 
	                          << " dx " << dx << " YObs " << yObs <<  " dy " << dy << " chiSq " << chi << std::endl;
	  if (chi < chiBest) {
	    xObsBest = xObs;
	    yObsBest = yObs;
	    chiBest = chi;
	    ihSelBest = ihSel;
	  }
	  ihSel++;
	  
       }
       if (ihSelBest == INT_MAX) return false;
       std::ostringstream headStrStr; 
       headStrStr << " " << fSubRunNum << " " << fEvtNum << " " << kStation << " " << ihSelBest
                  << " " << xPred << " " << std::sqrt(std::abs(xPredErrSq)) << " " << yPred << " " << std::sqrt(std::abs(yPredErrSq));
       std::string headStr(headStrStr.str());
       if (cView == 'U') {
         fFOutXYU << headStr << " " << xObsBest << " " << yObsBest << " " << chiBest << std::endl;
       } else {
         fFOutXYV << headStr << " " << xObsBest << " " << yObsBest << " " << chiBest << std::endl;
       } 
       return true;	 
     } 
     
     void ssdr::SSDAlign3DUVAlgo1::dumpXYInfo(int nHitsT) {
       fFOutXY << " " << fSubRunNum << " " << fEvtNum << " " << fTrType << " "  << nHitsT;
       for (size_t kSt=0; kSt != fNumStations; kSt++) fFOutXY << " " << fNHitsXView[kSt];
       for (size_t kSt=0; kSt != fNumStations; kSt++) fFOutXY << " " << fNHitsYView[kSt];
       fFOutXY << " " << fTrXOffset << " " << fTrXOffsetErr<< " " << fTrXSlope << " " << fTrXSlopeErr << " " << fChiSqX;
       fFOutXY << " " << fTrYOffset << " " << fTrYOffsetErr<< " " << fTrYSlope << " " << fTrYSlopeErr << " " << fChiSqY << std::endl;
       
     }
     void  ssdr::SSDAlign3DUVAlgo1::alignIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       
       fEvtNum = evt.id().event();
       fSubRunNum = evt.subRun();
       fRunNum = evt.run();
       this->initTrackParams();
       if (!fFOutXY.is_open()) this->openOutputCsvFiles();
       fTrType = ssdr::NONE;
       bool debugIsOn = fEvtNum < 15;
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::alignIt, at event " << fEvtNum << std::endl;
      
       bool gotX = this->recoXY(rb::X_VIEW, aSSDClsPtr);
       if (gotX) {
          if (debugIsOn) std::cerr << " Got a 2d XZ track... for evt " << fEvtNum << std::endl;
	  fTrType = ssdr::XONLY;
       }  else  {
           if (debugIsOn) std::cerr << " Got No 2d XZ track... for evt " << fEvtNum << std::endl;
       }   
	   
       bool gotY = this->recoXY(rb::Y_VIEW, aSSDClsPtr);
       if (gotY) {
          if (debugIsOn) std::cerr << " Got a 2d YZ track... for evt " << fEvtNum << std::endl;
	  if (fTrType == ssdr::XONLY) fTrType = ssdr::XYONLY;
	  if (fTrType == ssdr::NONE) fTrType = ssdr::YONLY;
       }  else  {
           if (debugIsOn) std::cerr << " Got No 2d XZ track... for evt " << fEvtNum << std::endl;
       }   
       if (gotX || gotY) this->dumpXYInfo(static_cast<int>(aSSDClsPtr->size()));
       if (gotX && gotY) { 
         for (size_t kStU=2; kStU != fNumStations; kStU++) {
           this->checkUV(rb::U_VIEW, kStU, aSSDClsPtr);
         }
         for (size_t kStV=4; kStV != fNumStations; kStV++) {
           this->checkUV(rb::W_VIEW, kStV, aSSDClsPtr);
         }
       }
       if(fEvtNum > 250000000) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::alignIt, quit here and at event " << fEvtNum << " quit now ! " << std::endl;
	 exit(2); 
       }
    }
   } // ssdr 
} //emph
 
