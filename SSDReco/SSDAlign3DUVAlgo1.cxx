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
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fNEvtsCompact(0), fMomentumIsSet(false), fFilesAreOpen(false),
       fView('?'), fStation(2), fAlt45(false), fSensor(-1),
       fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10), fChiSqCut(20.), fChiSqCutXY(100.),
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoordXs(fNumStations, 0.), fZCoordYs(fNumStations, 0.), fZCoordUs(2, 0.), fZCoordVs(4, 0.),
       fNominalOffsetsX(fNumStations, 0.), fNominalOffsetsY(fNumStations, 0.), 
       fNominalOffsetsXAlt45(fNumStations, 0.), fNominalOffsetsYAlt45(fNumStations, 0.), 
       fNominalOffsetsU(2, 0.), fNominalOffsetsV(4, 0.),
       fFittedResidualsX(fNumStations, 0.), fFittedResidualsY(fNumStations, 0.),
       fPitchAngles(fNumStations, 0.), fPitchAnglesAlt(fNumStations, 0.), 
       fYawAngles(fNumStations, 0.), fYawAnglesAlt(fNumStations, 0.), 
       fRollAngles(fNumStations, 0.), fRollAnglesAlt(fNumStations, 0.),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.), 
       fNHitsXView(fNumStations, 0), fNHitsYView(fNumStations, 0) 
     { 
        ; 
     }
     // Out dated constructor, to be deleted at some point. 
     SSDAlign3DUVAlgo1::SSDAlign3DUVAlgo1(char aView, int aStation, bool alt45) :  
       fSqrt2(std::sqrt(2.)), fOneOverSqrt2(1.0/std::sqrt(2.)), 
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fNEvtsCompact(0), fMomentumIsSet(false), fFilesAreOpen(false),
       fView(aView), fStation(aStation), fAlt45(alt45), 
       fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10),fChiSqCut(20.), fChiSqCutXY(100.),
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoordXs(fNumStations, 0.), fZCoordYs(fNumStations, 0.), fZCoordUs(2, 0.), fZCoordVs(4, 0.),
       fNominalOffsetsX(fNumStations, 0.), fNominalOffsetsY(fNumStations, 0.), 
       fNominalOffsetsXAlt45(fNumStations, 0.), fNominalOffsetsYAlt45(fNumStations, 0.), 
       fNominalOffsetsU(2, 0.), fNominalOffsetsV(4, 0.),
       fFittedResidualsX(fNumStations, 0.),  fFittedResidualsY(fNumStations, 0.),
       fPitchAngles(fNumStations, 0.), fPitchAnglesAlt(fNumStations, 0.), 
       fYawAngles(fNumStations, 0.), fYawAnglesAlt(fNumStations, 0.), 
       fRollAngles(fNumStations, 0.), fRollAnglesAlt(fNumStations, 0.),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.),
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
       if (zCoordUs.size() != 2) {
         std::cerr <<  " SSDAlign3DUVAlgo1::InitializeCoords,, unexpected number of U planes. should be 2 of them, we have " 
	           <<  zCoordUs.size()  << " fatal, here and now " << std::endl;
		   exit(2);
       }
       if (zCoordVs.size() != 4) {
         std::cerr <<  " SSDAlign3DUVAlgo1::InitializeCoords,, unexpected number of V planes. should be 4 of them, we have " 
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
       for (size_t k=4; k != 6; k++) { // for Station 4 and 5, 
        fNominalOffsetsX[k] = 2.0*fHalfWaferWidth;
        fNominalOffsetsY[k] = 2.0*fHalfWaferWidth;
       } 
       for (size_t k=0; k != 2; k++) { // for Station 2 and 3 
        fNominalOffsetsU[k] = -fHalfWaferWidth;
      } 
        for (size_t k=0; k != 4; k++) { // for Station 4 and 5,  
        fNominalOffsetsV[k] = 2.0*fHalfWaferWidth;
        if ((k == 1) || (k == 2)) fNominalOffsetsV[k] = -2.0*fHalfWaferWidth; // To be checked.. 
        if (k == 3) fNominalOffsetsV[k] = -2.0*fHalfWaferWidth; // To be checked.. 
      } 
      
       
//       std::cerr << " SSDAlign3DUVAlgo1::InitailizeCoords  " << std::endl;
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
       fMultScatUncert[1] =  0.003830147;   
       fMultScatUncert[2] =  0.01371613;	
       fMultScatUncert[3] =  0.01947578;	
       fMultScatUncert[4] =  0.05067243;  
       fMultScatUncert[5] =  0.06630287;    
     }
     
     SSDAlign3DUVAlgo1::~SSDAlign3DUVAlgo1() {
      if (fFOutXY.is_open()) fFOutXY.close();
      if (fFOutXYU.is_open()) fFOutXYU.close();
      if (fFOutXYV.is_open()) fFOutXYV.close();
      if (fFOutCompact.is_open()) {
          std::cerr << " Closing the compact event data with " << fNEvtsCompact << " events selected " << std::endl;
          fFOutCompact.close();
      }
     }
     void ssdr::SSDAlign3DUVAlgo1::SetForMomentum(double p) {
       if (fMomentumIsSet) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::SetForMomentum, already called, skip!!! " << std::endl;
	 return;
       }
       const double pRatio = 120.0 / p;
       fMagnetKick120GeV *= pRatio;
       for (size_t k=0; k != fMultScatUncert.size(); k++) fMultScatUncert[k] *= std::abs(pRatio); 
       fMomentumIsSet = true;
     }
     void ssdr::SSDAlign3DUVAlgo1::openOutputCsvFiles() {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameXYUStrStr, fNameXYVStrStr;
       fNameXYUStrStr << "SSDAlign3DXYU_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameXYUStr(fNameXYUStrStr.str());
       fFOutXYU.open(fNameXYUStr.c_str());
       fFOutXYU << " spill evt kSt iHU  xPred xPredErr yPred yPredErr uPred uObs chiSq " << std::endl;
       fNameXYVStrStr << "SSDAlign3DXYV_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameXYVStr(fNameXYVStrStr.str());
       fFOutXYV.open(fNameXYVStr.c_str());
       fFOutXYV << " spill evt kSt iHV xPred xPredErr yPred yPredErr wPred wObs chiSq sensor " << std::endl;
     } 

     //
     // This is cloned code!.. See SSDAlign2DXY to be cleaned up.  This is a more recent version, better..  
     //
     double ssdr::SSDAlign3DUVAlgo1::GetTsFromCluster(char aView, size_t kStation, size_t sensor,  double strip) const 
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
	      case 'U' : // Station 2 and 3 
	      {
		 aVal =  (strip*fPitch + fNominalOffsetsU[kStation-2]  + 1.0e-10);
	         return aVal;
	      } 
	      case 'V' :// Station 4 and 5, double sensors.  
	      {
	         if (kStation < 4) {
	            std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected station " << kStation 
		              << " for W view internal error, fatal " << std::endl; exit(2);
		 }
	         if (sensor < 4) {
	            std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected sensor " << sensor 
		              << " for W view internal error, fatal " << std::endl; exit(2);
		 }
	         size_t indexNom = 2*(kStation-4) + (sensor-4);
		 aVal =  (strip*fPitch + fNominalOffsetsV[indexNom]  + 1.0e-10);
		 if ((kStation == 5) && (sensor == 4)) aVal *= -1.;
	        return aVal;
	      } 
	      default :
	        std::cerr << " SSDAlign3DUVAlgo1::getTsFromCluster, unexpected view, " 
		<< fView << " kStation " << kStation << 
		 " internal error, fatal " << std::endl; exit(2);
	   }
	   return 0.; // should not happen  
       }
       
      bool ssdr::SSDAlign3DUVAlgo1::recoXY(geo::sensorView theView, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
//
        if ((theView != geo::X_VIEW) && (theView != geo::Y_VIEW)) {
	  std::cerr << " SSDAlign3DUVAlgo1::recoXY, unexpected view enum geo::sensorView " << theView << " expect X or Y, fatal, I said it " << std::endl;
	  exit(2);
	}
        const int numMaxH2345 = 3; // Could become a run time parameter. Not yet!.. loops below don't allow it
        bool debugIsOn = (fEvtNum < 25); 
        std::vector<int> nHits(fNumStations, 0); int nHitsTotal = aSSDClsPtr->size();
	if (debugIsOn && (theView == geo::X_VIEW)) std::cerr << " SSDAlign3DUVAlgo1::recoXX. X view, at event " 
	                         << fEvtNum << " number of clusters " << nHitsTotal <<  std::endl;
	if (debugIsOn && (theView == geo::Y_VIEW)) std::cerr << " SSDAlign3DUVAlgo1::recoXX. Y view, at event " 
	                         << fEvtNum << " number of clusters " << nHitsTotal <<  std::endl;
//
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsFirst(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs2nd(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs3rd(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs4rth(fNumStations, aSSDClsPtr->cend());
	if (theView == geo::X_VIEW) for(size_t kSt=0; kSt != fNumStations; kSt++) fNHitsXView[kSt] = 0;
	else for(size_t kSt=0; kSt != fNumStations; kSt++) fNHitsYView[kSt] = 0;  	 
        for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != theView) continue;
	  const size_t kSt = static_cast<size_t>(itCl->Station());
	  nHits[kSt]++;
	  if (kSt >= fNumStations ) continue; // should not happen  
	  if (theView == geo::X_VIEW) fNHitsXView[kSt]++; 
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
        if (theView == geo::X_VIEW) myLinFitX.resids = std::vector<double>(fNumStations, 0.);
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
	    if (theView == geo::X_VIEW) tsData[kSt] = GetTsFromCluster('X', kSt, 0, aStrip); // sensor number does not matter.. for X view... 
	    else tsData[kSt] = GetTsFromCluster('Y', kSt, 0, aStrip);
	    tsDataErr[kSt] = GetTsUncertainty(kSt, mySSDClsPtrsFirst[kSt]);
	    if (debugIsOn) {
	       if (theView == geo::X_VIEW) std::cerr << " .... For station " << kSt << " strip " << aStrip 
	                                   << " ts " << tsData[kSt] << " +- " << tsDataErr[kSt] << " fitted Resid " << fFittedResidualsX[kSt] <<  std::endl;
	       else  std::cerr << " .... For station " << kSt << " strip " << aStrip 
	                                   << " ts " << tsData[kSt] << " +- " << tsDataErr[kSt] << " fitted Resid " << fFittedResidualsY[kSt] <<  std::endl;
	    }
	  }
	}
	if (theView == geo::X_VIEW) { 
          myLinFitX.fitLin(false, tsData, tsDataErr);
          if (myLinFitX.chiSq > fChiSqCutXY) {
	    if (debugIsOn) {
	       std::cerr << " Unacceptable chi-Sq = " << myLinFitX.chiSq << " need to look at second hits.. We will try it out  " << std::endl;
//	     return false; See blow.. 
	    } 
          } else {
	    fTrXY.SetXTrParams(myLinFitX.offset, myLinFitX.slope);
	    fTrXY.SetXTrParamsErrs(myLinFitX.sigmaOffset, myLinFitX.sigmaSlope, myLinFitX.covOffsetSlope);
	    fTrXY.SetXChiSq(myLinFitX.chiSq);
	    if (debugIsOn) 
	       std::cerr << " Acceptable chi-Sq = " << myLinFitX.chiSq << " Track Offset " << fTrXY.XOffset() << "  Slope " << fTrXY.XSlope() << std::endl;
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
	    fTrXY.SetYTrParams(myLinFitY.offset, myLinFitY.slope);
	    fTrXY.SetYTrParamsErrs(myLinFitY.sigmaOffset, myLinFitY.sigmaSlope, myLinFitY.covOffsetSlope);
	    fTrXY.SetYChiSq(myLinFitY.chiSq);
	    if (debugIsOn) 
	       std::cerr << " Acceptable chi-Sq = " << myLinFitY.chiSq << " Track Offset " << fTrXY.YOffset() << "  Slope " << fTrXY.YSlope() << std::endl;
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
	  if (theView == geo::X_VIEW) tsDataAlt[2] = GetTsFromCluster('X', 2, 0, aStrip2);
	  else tsDataAlt[2] = GetTsFromCluster('Y', 2, 0, aStrip2);
	  tsDataAltErr[2] = GetTsUncertainty(2, itcl2);
	  
	 
	  for (int i3 = 0; i3 != numMaxH2345; i3++) {
	    std::vector<rb::SSDCluster>::const_iterator itcl3 = mySSDClsPtrsFirst[3];	 
	    if (i3 == 1) itcl3 = mySSDClsPtrs2nd[3];
	    if (i3 == 2) itcl3 = mySSDClsPtrs3rd[3];
	    if (itcl3 == aSSDClsPtr->cend()) continue;
	    double aStrip3 = itcl3->WgtAvgStrip();
	    if (theView == geo::X_VIEW) tsDataAlt[3] = GetTsFromCluster('X', 3, 0, aStrip3);
	    else tsDataAlt[3] = GetTsFromCluster('X', 3, 0, aStrip3);
	    tsDataAltErr[3] = GetTsUncertainty(3, itcl3);
	    
	    for (int i4 = 0; i4 != numMaxH2345; i4++) {
	      std::vector<rb::SSDCluster>::const_iterator itcl4 = mySSDClsPtrsFirst[4];	 
	      if (i4 == 1) itcl4 = mySSDClsPtrs2nd[4];
	      if (i4 == 2) itcl4 = mySSDClsPtrs3rd[4];
	      if (itcl4 == aSSDClsPtr->cend()) continue;
	      double aStrip4 = itcl4->WgtAvgStrip();
	      if (theView == geo::X_VIEW) tsDataAlt[4] = GetTsFromCluster('X', 4, 0, aStrip4);
	      else tsDataAlt[4] = GetTsFromCluster('Y', 4, 0, aStrip4);
	      tsDataAltErr[4] = GetTsUncertainty(4, itcl4);
	    
	      for (int i5 = 0; i5 != numMaxH2345; i5++) {
	        std::vector<rb::SSDCluster>::const_iterator itcl5 = mySSDClsPtrsFirst[5];	 
	        if (i5 == 1) itcl5 = mySSDClsPtrs2nd[5];
	        if (i5 == 2) itcl5 = mySSDClsPtrs3rd[5];
	        if (itcl5 == aSSDClsPtr->cend()) continue;
	        double aStrip5 = itcl5->WgtAvgStrip();
	        if (theView == geo::X_VIEW) tsDataAlt[5] = GetTsFromCluster('X', 5, 0, aStrip5);
		else tsDataAlt[5] = GetTsFromCluster('Y', 5, 0, aStrip5);
	        tsDataAltErr[5] = GetTsUncertainty(5, itcl5);
		
		if (theView == geo::X_VIEW) {  
                  myLinFitX.fitLin(false, tsDataAlt, tsDataAltErr);		
                  if (myLinFitX.chiSq < chiSqBest) {
	            fTrXY.SetXTrParams(myLinFitX.offset, myLinFitX.slope);
	            fTrXY.SetXTrParamsErrs(myLinFitX.sigmaOffset, myLinFitX.sigmaSlope, myLinFitX.covOffsetSlope);
	            if (debugIsOn) 
	               std::cerr << "Best chi-Sq = " << myLinFitX.chiSq << " so fat.. with Track Offset " 
		                 << fTrXY.XOffset() << "  Slope " << fTrXY.XSlope() << std::endl;
		  }
	        } else {
                  myLinFitY.fitLin(false, tsDataAlt, tsDataAltErr);		
                  if (myLinFitY.chiSq < chiSqBest) {
	            fTrXY.SetYTrParams(myLinFitY.offset, myLinFitY.slope);
	            fTrXY.SetYTrParamsErrs(myLinFitY.sigmaOffset, myLinFitY.sigmaSlope, myLinFitY.covOffsetSlope);
	            if (debugIsOn) 
	               std::cerr << "Best chi-Sq = " << myLinFitY.chiSq << " so fat.. with Track Offset " 
		                 << fTrXY.YOffset() << "  Slope " << fTrXY.YSlope() << std::endl;
		
		  }
	        } 
	      } // on hits from Station 5   
	   
	    } // on hits from Station 4   
	   
	 } // on hits from Station 3   
	   
	} // on hits from station 2. 
      
       if (chiSqBest < fChiSqCutXY) {
          if (theView == geo::X_VIEW) { 
	    fTrXY.SetXChiSq (chiSqBest);
	    if (debugIsOn) 
	       std::cerr << " Acceptable X fit chi-Sq = " << chiSqBest << " Track Offset " << fTrXY.XOffset() << "  Slope " << fTrXY.XSlope() << std::endl;
	    return true;
	  } else {
	    fTrXY.SetYChiSq (chiSqBest);
	    if (debugIsOn) 
	       std::cerr << " Acceptable Y fit chi-Sq = " << chiSqBest << " Track Offset " << fTrXY.YOffset() << "  Slope " << fTrXY.YSlope() << std::endl;
	    return true;
	  }
       }
    
       return false;
     }

     bool ssdr::SSDAlign3DUVAlgo1::checkUV(geo::sensorView view, size_t kStation, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
       bool debugIsOn = (fEvtNum < 25);    
     
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::checkUV, view Index " << view << " Station " << kStation 
                 << " nunH " << aSSDClsPtr->size() << "  working on it.. " << std::endl;
	
       if (kStation < 2) {
         std::cerr << " SSDAlign3DUVAlgo1::checkUV, " << view << " Unexpected Station number = " << kStation << " fatal mistake! " << std::endl; 
	 exit(2);
       } 
       if ((view  == geo::W_VIEW) && (kStation < 4)) {
         std::cerr << " SSDAlign3DUVAlgo1::checkUV, " << view << " Unexpected Station number = " << kStation << " fatal mistake! " << std::endl; 
	 exit(2);
       } 

       size_t kStEff = kStation - 2;
       if (view  == geo::W_VIEW) kStEff = kStation - 4;
       double aTrXOffset = fTrXY.XOffset(); double aTrYOffset = fTrXY.YOffset(); 
       double aTrXOffsetErr = fTrXY.XOffsetErr(); double aTrYOffsetErr = fTrXY.YOffsetErr();
       double aTrXSlope = fTrXY.XSlope(); double aTrYSlope = fTrXY.YSlope(); 
       double aTrXSlopeErr = fTrXY.XSlopeErr(); double aTrYSlopeErr = fTrXY.YSlopeErr();
       double aTrXCovOffSl = fTrXY.XCovOffSl(); double aTrYCovOffSl = fTrXY.YCovOffSl();
       	 
       double xPred, yPred, xPredErrSq, yPredErrSq;
       char cView = '?';
       switch (view) {
         case geo::U_VIEW :
	 {
	   xPred = aTrXOffset + fZCoordUs[kStEff]*aTrXSlope;
	   xPredErrSq = aTrXOffsetErr*aTrXOffsetErr + fZCoordUs[kStEff]*aTrXSlopeErr*fZCoordUs[kStEff]*aTrXSlopeErr 
	                + fZCoordUs[kStEff]*aTrXCovOffSl;
	   yPred = aTrYOffset + fZCoordUs[kStEff]*aTrYSlope;
	   yPredErrSq = aTrYOffsetErr*aTrYOffsetErr + fZCoordUs[kStEff]*aTrYSlopeErr*fZCoordUs[kStEff]*aTrYSlopeErr 
	                + fZCoordUs[kStEff]*aTrYCovOffSl;
	   cView = 'U';
	   break;
	 } 
	 case geo::W_VIEW :
	 {
	   xPred = aTrXOffset + fZCoordVs[kStEff]*aTrXSlope;
	   xPredErrSq = aTrXOffsetErr*aTrXOffsetErr + fZCoordVs[kStEff]*aTrXSlopeErr*fZCoordVs[kStEff]*aTrXSlopeErr 
	                + fZCoordVs[kStEff]*aTrXCovOffSl;
	   yPred = aTrYOffset + fZCoordVs[kStEff]*aTrYSlope;
	   yPredErrSq = aTrYOffsetErr*aTrYOffsetErr + fZCoordVs[kStEff]*aTrYSlopeErr*fZCoordVs[kStEff]*aTrYSlopeErr 
	                + fZCoordVs[kStEff]*aTrYCovOffSl;
	 
	   cView = 'V';
	   break;
	 }
	 default:
	 { 
           std::cerr << " SSDAlign3DUVAlgo1::checkUV, Unexpected " << view << " Station number = " << kStation << " fatal mistake! " << std::endl; 
	   exit(2);
         } 
       }
      const double uPred = fOneOverSqrt2 * (xPred + yPred);  // + or - 
      const double wPred = fOneOverSqrt2 * (xPred - yPred);  // + or - 
      if (debugIsOn) {
           if (xPredErrSq < 0.) std::cerr << " .... ???? Negative predicted uncertainty for X , problem with covariance of linFit " << std::endl;
           if (yPredErrSq < 0.) std::cerr << " .... ???? Negative predicted uncertainty for Y , problem with covariance of linFit " << std::endl;
           std::cerr << " .... xPred " << xPred << " +- " << std::sqrt(std::abs(xPredErrSq)) 
                                << "  yPred " << yPred << " +- " << std::sqrt(std::abs(yPredErrSq)) << " uPred " << uPred << std::endl;
       }
       double chiBest = DBL_MAX; int ihSelBest = INT_MAX;
       double uvObsBest = DBL_MAX;
       int sensorBest = -1;
       int ihSel = 0;
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != view) continue;
	  const size_t kSt = static_cast<size_t>(itCl->Station());
          if (kSt != kStation) continue;
          const double aStrip = itCl->WgtAvgStrip();
          const double uvObs =  this->GetTsFromCluster(cView, kStation, itCl->Sensor(), aStrip);
	  // special case for station 5, sometimes.. 
	  if ((kSt == 5) && (itCl->View() == geo::W_VIEW)) {
	    if (debugIsOn) std::cerr << " ... .... W view, Sensor " << itCl->Sensor() <<  std::endl;
	  } 
	  const double uvObsUncert = this->GetTsUncertainty(kStation, itCl);
	  const double uvUncertSq = uvObsUncert*uvObsUncert + 0.5 * (xPredErrSq + yPredErrSq);
	  double duv = (cView == 'U') ? (uPred - uvObs) : (wPred - uvObs);
	  double chi= (duv*duv/uvUncertSq);
	  if (debugIsOn) std::cerr << " ... strip " << aStrip << " U or V Obs " << uvObs 
	                          << " duv " << duv << " chiSq " << chi << std::endl;
	  if (chi < chiBest) {
	    uvObsBest = uvObs;
	    chiBest = chi;
	    ihSelBest = ihSel;
	    sensorBest = itCl->Sensor();  
	  }
	  ihSel++;
	  
       }
       if (ihSelBest == INT_MAX) return false;
       std::ostringstream headStrStr; 
       headStrStr << " " << fSubRunNum << " " << fEvtNum << " " << kStation << " " << ihSelBest
                  << " " << xPred << " " << std::sqrt(std::abs(xPredErrSq)) << " " << yPred << " " << std::sqrt(std::abs(yPredErrSq));
       std::string headStr(headStrStr.str());
       if (cView == 'U') {
         fFOutXYU << headStr << " " << uPred << " " << uvObsBest << " " << chiBest << std::endl;
       } else {
         fFOutXYV << headStr << " " << wPred << " " << uvObsBest << " " << chiBest << " " << sensorBest << std::endl;
       } 
       return true;	 
     } 
     
     void  ssdr::SSDAlign3DUVAlgo1::alignIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       
       fEvtNum = evt.id().event();
       fSubRunNum = evt.subRun();
       fRunNum = evt.run();
       fTrXY.Reset();
       if (!fFOutXYU.is_open()) this->openOutputCsvFiles();
        bool debugIsOn = fEvtNum < 25;
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::alignIt, at event " << fEvtNum << std::endl;
      
       bool gotX = this->recoXY(geo::X_VIEW, aSSDClsPtr);
       if (gotX) {
          if (debugIsOn) std::cerr << " Got a 2d XZ track... for evt " << fEvtNum << std::endl;
	  fTrXY.SetType(rb::XONLY);
       }  else  {
           if (debugIsOn) std::cerr << " Got No 2d XZ track... for evt " << fEvtNum << std::endl;
       }   
	   
       bool gotY = this->recoXY(geo::Y_VIEW, aSSDClsPtr);
       if (gotY) {
          if (debugIsOn) std::cerr << " Got a 2d YZ track... for evt " << fEvtNum << std::endl;
	  if (fTrXY.Type() == rb::XONLY) fTrXY.SetType(rb::XYONLY);
	  if (fTrXY.Type() == rb::NONE) fTrXY.SetType(rb::YONLY);
       }  else  {
           if (debugIsOn) std::cerr << " Got No 2d XZ track... for evt " << fEvtNum << std::endl;
       }   
//       if (gotX || gotY) this->dumpXYInfo(static_cast<int>(aSSDClsPtr->size())); Move to RecoBeamTrackAlgo1
       if (gotX && gotY) {
         int numUVCheck = 0; 
         for (size_t kStU=2; kStU != fNumStations-2; kStU++) {
           if (this->checkUV(geo::U_VIEW, kStU, aSSDClsPtr)) numUVCheck++;
         }
	 if (numUVCheck == 1) fTrXY.SetType(rb::XYUCONF1);
	 if (numUVCheck == 2) fTrXY.SetType(rb::XYUCONF2);
	 // Not worth doing.. stereo angle clearly wrong, of mis labeled. Well, try it again.. 
         for (size_t kStV=4; kStV != fNumStations; kStV++) {
           this->checkUV(geo::W_VIEW, kStV, aSSDClsPtr);
         }
	 if (numUVCheck == 3) fTrXY.SetType(rb::XYUCONF3);
	 if (numUVCheck == 4) fTrXY.SetType(rb::XYUCONF4);
       }
       if(fEvtNum > 250000000) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::alignIt, quit here and at event " << fEvtNum << " quit now ! " << std::endl;
	 exit(2); 
       }
    }
   } // ssdr 
} //emph
 
