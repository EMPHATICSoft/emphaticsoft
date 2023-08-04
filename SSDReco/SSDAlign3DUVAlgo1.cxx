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
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"
#include "Geometry/service/GeometryService.h"

 using namespace emph;

namespace emph {
  namespace ssdr {

     SSDAlign3DUVAlgo1::SSDAlign3DUVAlgo1() :
       fSqrt2(std::sqrt(2.)), fOneOverSqrt2(1.0/std::sqrt(2.)), 
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fNEvtsCompact(0), fDo3DFit(false), fDoUseTightClusters(false), fIsMC(false), 
       fMomentumIsSet(false), fFilesAreOpen(false),
       fView('?'), fStation(2), fSensor(2),
       fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10), 
       fChiSqCut(20.), fChiSqCutX(500.), fChiSqCutY(100.), fMomentumInit3DFit(120), fNoMagnet(false),
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoordXs(fNumStations, 0.), fZCoordYs(fNumStations, 0.), fZCoordUs(2, 0.), fZCoordVs(4, 0.),
       fFittedResidualsX(fNumStations, 0.), fFittedResidualsY(fNumStations, 0.),
       fPitchAngles(fNumStations, 0.), fPitchAnglesAlt(fNumStations, 0.), 
       fYawAngles(fNumStations, 0.), fYawAnglesAlt(fNumStations, 0.), 
       fRollAngles(fNumStations, 0.), fRollAnglesAlt(fNumStations, 0.),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.), fDataFor3DFit(), 
       fNHitsXView(fNumStations, 0), fNHitsYView(fNumStations, 0), myLinFitX(), myLinFitY(), myNonLin3DFitPtr(nullptr),
       myConvertX('X'), myConvertY('Y'), myConvertU('U'), myConvertV('V'), 
       fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()) 
     { ;
    }
     void SSDAlign3DUVAlgo1::InitializeCoords(bool lastIs4, const std::vector<double> &zCoordXs, const std::vector<double> &zCoordYs,
	                                     const std::vector<double> &zCoordUs, const std::vector<double> &zCoordVs)
      {
     
       art::ServiceHandle<emph::geo::GeometryService> geo;
       if (geo->Geo() == nullptr) {
         std::cerr << " Problem in SSDAlign3DUVAlgo1::InitializeCoords, can not get the Geometry unique pointer... Fatal, quit now " << std::endl; exit(2);
       }
       fNoMagnet = (geo->Geo()->MagnetUSZPos() < 0.);
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
       myConvertX.InitializeAllCoords(zCoordXs); 
       myConvertY.InitializeAllCoords(zCoordYs);
       // This is silly.. but I need to make progress! 
       std::vector<double> zCoordUsStd(fNumStations + 2); zCoordUsStd[2] = fZCoordUs[0]; zCoordUsStd[3] = fZCoordUs[1];
       myConvertU.InitializeAllCoords(zCoordUsStd); 
       std::vector<double> zCoordVsStd(fNumStations + 2); 
       for (size_t k=4; k != fZCoordVs.size(); k++) zCoordVsStd[4+k] = fZCoordVs[k];
       myConvertV.InitializeAllCoords(zCoordVsStd); 
       
     }
     
     SSDAlign3DUVAlgo1::~SSDAlign3DUVAlgo1() {
      if (fFOutXY.is_open()) fFOutXY.close();
      if (fFOutXYU.is_open()) fFOutXYU.close();
      if (fFOutXYV.is_open()) fFOutXYV.close();
      if (fFOut3DFit.is_open()) fFOut3DFit.close();
      if (fFOutCompact.is_open()) {
          std::cerr << " Closing the compact event data with " << fNEvtsCompact << " events selected " << std::endl;
          fFOutCompact.close();
      }
      delete myNonLin3DFitPtr;
     }
     void ssdr::SSDAlign3DUVAlgo1::SetForMomentum(double p) {
       if (fMomentumIsSet) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::SetForMomentum, already called, skip!!! " << std::endl;
	 return;
       }
       myConvertX.SetForMomentum(p); myConvertY.SetForMomentum(p); myConvertU.SetForMomentum(p); myConvertV.SetForMomentum(p);
       fMomentumIsSet = true;
       fMomentumInit3DFit = p;
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
       //
       // And 3D fit 
       //
       std::ostringstream fName3DFitStrStr;
       fName3DFitStrStr << "SSDAlign3DFit_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fName3DFitStr(fName3DFitStrStr.str());
       fFOut3DFit.open(fName3DFitStr.c_str());
       fFOut3DFit << " spill evt validFlag chiSq ndgf nFCN x0 errX0 Slx0 errSlx0 y0 errY0 Sly0 errSly0";
       if (!fNoMagnet) fFOut3DFit << " p errP ";
       fFOut3DFit << std::endl;
       
     } 

     bool ssdr::SSDAlign3DUVAlgo1::recoXY(emph::geo::sensorView theView, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
        fIsMC = fTokenJob.find("SimProton") != std::string::npos;
//
        if ((theView != emph::geo::X_VIEW) && (theView != emph::geo::Y_VIEW)) {
	  std::cerr << " SSDAlign3DUVAlgo1::recoXY, unexpected view enum emph::geo::sensorView " << theView << " expect X or Y, fatal, I said it " << std::endl;
	  exit(2);
	}
        const int numMaxH2345 = 3; // Could become a run time parameter. Not yet!.. loops below don't allow it
        bool debugIsOn = (fEvtNum < 25); 
        std::vector<int> nHits(fNumStations, 0); int nHitsTotal = aSSDClsPtr->size();
	if (debugIsOn && (theView == emph::geo::X_VIEW)) std::cerr << " SSDAlign3DUVAlgo1::recoXX. X view, at event " 
	                         << fEvtNum << " number of clusters " << nHitsTotal <<  std::endl;
	if (debugIsOn && (theView == emph::geo::Y_VIEW)) std::cerr << " SSDAlign3DUVAlgo1::recoXX. Y view, at event " 
	                         << fEvtNum << " number of clusters " << nHitsTotal <<  std::endl;
//
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsFirst(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs2nd(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs3rd(fNumStations, aSSDClsPtr->cend());
        std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrs4rth(fNumStations, aSSDClsPtr->cend());
	if (theView == emph::geo::X_VIEW) for(size_t kSt=0; kSt != fNumStations; kSt++) fNHitsXView[kSt] = 0;
	else for(size_t kSt=0; kSt != fNumStations; kSt++) fNHitsYView[kSt] = 0;  	 
        for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != theView) continue;
	  const size_t kSt = static_cast<size_t>(itCl->Station());
	  nHits[kSt]++;
	  if (kSt >= fNumStations ) continue; // should not happen  
	  if (theView == emph::geo::X_VIEW) fNHitsXView[kSt]++; 
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
        if (theView == emph::geo::X_VIEW) myLinFitX.resids = std::vector<double>(fNumStations, 0.);
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
	    double aStripErr = mySSDClsPtrsFirst[kSt]->WgtRmsStrip();
	    size_t kSe = mySSDClsPtrsFirst[kSt]->Sensor();
	    size_t kS = (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt; // in dex ranging from 0 to 7, inclusive, for Phase1b, list of sensors by view. 
	    if (std::isnan(aStrip) || std::isnan(aStripErr)) continue;
	    fDataFor3DFit.push_back(mySSDClsPtrsFirst[kSt]);
	    if (theView == emph::geo::X_VIEW) { 
//	       tsData[kSt] = myConvertX.GetTsFromCluster(kSt, mySSDClsPtrsFirst[kSt]->Sensor(), aStrip);
// Very clumsy Upgrade to use the new VolatileAlignment data. 
               if (!fIsMC) { 
	          tsData[kSt] =  ( -1.0*aStrip*fPitch + fEmVolAlP->TrPos(theView, kSt, kSe));
	          if (kSt >= 4)  tsData[kSt] *= -1.0; // Incomplete... To be checked again!!!! 
	       } else {
	          tsData[kSt]  = ( -1.0 * aStrip*fPitch + fEmVolAlP->TrPos(theView, kSt, kSe));
	          if ((kSt > 3) && (kSe % 2) == 1) tsData[kSt] *=-1;    // for now.. Need to keep checking this.. Shameful.   
	       }
	       tsDataErr[kSt] = myConvertX.GetTsUncertainty(kSt, mySSDClsPtrsFirst[kSt]);
	    } else {
//	       tsData[kSt] = myConvertY.GetTsFromCluster(kSt, mySSDClsPtrsFirst[kSt]->Sensor(), aStrip);
               if (!fIsMC) { // Most likely wrong by now.. Work in progress... 
	          tsData[kSt]  =  ( aStrip*fPitch + fEmVolAlP->TrPos(theView, kSt, kSe));
	          if (kSt >= 4) tsData[kSt] =  ( -1.0*aStrip*fPitch + fEmVolAlP->TrPos(theView, kSt, kSe));
	       } else {
	          tsData[kSt] = (kS < 4) ? ( aStrip*fPitch + fEmVolAlP->TrPos(theView, kSt, kSe)) :
	                      ( aStrip*fPitch - fEmVolAlP->TrPos(theView, kSt, kSe)) ;
	          if ((kS > 3) && (kS % 2) == 1) tsData[kSt] *=-1;
	       }
	       tsDataErr[kSt] = myConvertY.GetTsUncertainty(kSt, mySSDClsPtrsFirst[kSt]);
	    }
	    if (debugIsOn) {
	       if (theView == emph::geo::X_VIEW) std::cerr << " .... For X station ";
	       else std::cerr << " .... For Y station ";
	       std::cerr << kSt << " strip " << aStrip 
	                                   << " ts " << tsData[kSt] << " +- " << tsDataErr[kSt] << " fitted Resid " << fFittedResidualsX[kSt] <<  std::endl;
	    }
	  }
	}
	
	if (theView == emph::geo::X_VIEW) { 
          myLinFitX.fitLin(false, tsData, tsDataErr);
          if (myLinFitX.chiSq > fChiSqCutX) {
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
          if (myLinFitY.chiSq > fChiSqCutY) {
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
	  if (theView == emph::geo::X_VIEW) {
	    tsDataAlt[2] = myConvertX.GetTsFromCluster(itcl2->Station(), itcl2->Sensor(), aStrip2);
	    tsDataAltErr[2] = myConvertX.GetTsUncertainty(itcl2->Station(), itcl2);
	  } else {
	    tsDataAlt[2] = myConvertY.GetTsFromCluster(itcl2->Station(), itcl2->Sensor(), aStrip2);
	    tsDataAltErr[2] = myConvertY.GetTsUncertainty(itcl2->Station(), itcl2);
	  }
	 
	  for (int i3 = 0; i3 != numMaxH2345; i3++) {
	    std::vector<rb::SSDCluster>::const_iterator itcl3 = mySSDClsPtrsFirst[3];	 
	    if (i3 == 1) itcl3 = mySSDClsPtrs2nd[3];
	    if (i3 == 2) itcl3 = mySSDClsPtrs3rd[3];
	    if (itcl3 == aSSDClsPtr->cend()) continue;
	    double aStrip3 = itcl3->WgtAvgStrip();
	    if (theView == emph::geo::X_VIEW) {
	      tsDataAlt[3] = myConvertX.GetTsFromCluster(itcl3->Station(), itcl3->Sensor(), aStrip3);
	      tsDataAltErr[3] = myConvertX.GetTsUncertainty(itcl3->Station(), itcl2);
	    } else {
	      tsDataAlt[3] = myConvertY.GetTsFromCluster(itcl3->Station(), itcl3->Sensor(), aStrip3);
	      tsDataAltErr[3] = myConvertY.GetTsUncertainty(itcl3->Station(), itcl3);
	    }
	    
	    for (int i4 = 0; i4 != numMaxH2345; i4++) {
	      std::vector<rb::SSDCluster>::const_iterator itcl4 = mySSDClsPtrsFirst[4];	 
	      if (i4 == 1) itcl4 = mySSDClsPtrs2nd[4];
	      if (i4 == 2) itcl4 = mySSDClsPtrs3rd[4];
	      if (itcl4 == aSSDClsPtr->cend()) continue;
	      double aStrip4 = itcl4->WgtAvgStrip();
	      if (theView == emph::geo::X_VIEW) {
	        tsDataAlt[4] = myConvertX.GetTsFromCluster(itcl4->Station(), itcl4->Sensor(), aStrip4);
	        tsDataAltErr[4] = myConvertX.GetTsUncertainty(itcl4->Station(), itcl4);
	      } else {
	        tsDataAlt[4] = myConvertY.GetTsFromCluster(itcl4->Station(), itcl4->Sensor(), aStrip4);
	        tsDataAltErr[4] = myConvertY.GetTsUncertainty(itcl4->Station(), itcl4);
	      }
	    
	      for (int i5 = 0; i5 != numMaxH2345; i5++) {
	        std::vector<rb::SSDCluster>::const_iterator itcl5 = mySSDClsPtrsFirst[5];	 
	        if (i5 == 1) itcl5 = mySSDClsPtrs2nd[5];
	        if (i5 == 2) itcl5 = mySSDClsPtrs3rd[5];
	        if (itcl5 == aSSDClsPtr->cend()) continue;
	        double aStrip5 = itcl5->WgtAvgStrip();
	      if (theView == emph::geo::X_VIEW) {
	        tsDataAlt[5] = myConvertX.GetTsFromCluster(itcl5->Station(), itcl5->Sensor(), aStrip5);
	        tsDataAltErr[5] = myConvertX.GetTsUncertainty(itcl5->Station(), itcl5);
	      } else {
	        tsDataAlt[5] = myConvertY.GetTsFromCluster(itcl5->Station(), itcl5->Sensor(), aStrip5);
	        tsDataAltErr[5] = myConvertY.GetTsUncertainty(itcl5->Station(), itcl5);
	      }
		
		if (theView == emph::geo::X_VIEW) {  
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
      
       if (chiSqBest < std::max(fChiSqCutX, fChiSqCutY)) {
          if (theView == emph::geo::X_VIEW) { 
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
     int ssdr::SSDAlign3DUVAlgo1::recoUV(emph::geo::sensorView view, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
       //
       // For now, we take all U or V hits, for the fit later on.  But, we always take only the first encountered hit.   
       //
       
       bool debugIsOn = (fEvtNum < 2);    
     
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::RecoUV, view Index " << view  
                 << " nunH " << aSSDClsPtr->size() << "  working on it.. " << std::endl;
       std::vector<int> NumHitsPerStation(fNumStations, 0); // valid only for Phase1b 
       int nnT = 0;
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != view) continue;
	  // Room for selection, such rejection of hot channels., or perhaps, crude XY cuts.. 
	  const size_t kSt = static_cast<size_t>(itCl->Station());
	  if (std::isnan(itCl->WgtRmsStrip()) || std::isnan(itCl->WgtAvgStrip())) continue;
	  NumHitsPerStation[kSt]++; nnT++;
       }
       for (size_t kSt=0; kSt != fNumStations; kSt++) if (NumHitsPerStation[kSt] > 1) return 0;
       for (size_t kSt=0; kSt != fNumStations; kSt++) NumHitsPerStation[kSt] = 0;
       //
       // redo, this time store the hit for the 3D fit. 
       //
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          if (itCl->View() != view) continue;
	  // Room for selection, such rejection of hot channels., or perhaps, crude XY cuts.. 
	  const size_t kSt = static_cast<size_t>(itCl->Station());
	  if (std::isnan(itCl->WgtRmsStrip()) || std::isnan(itCl->WgtAvgStrip())) continue;
	  NumHitsPerStation[kSt]++;
	  if (NumHitsPerStation[kSt] == 1) fDataFor3DFit.push_back(itCl);
       }
       if (debugIsOn) std::cerr << " ...... Number of hits in the last 4 stations " 
           << NumHitsPerStation[2] << " " << NumHitsPerStation[3] << " " 
	   << NumHitsPerStation[4] << " " << NumHitsPerStation[5] << " " << std::endl;
       return nnT;
     } 
     bool ssdr::SSDAlign3DUVAlgo1::fit3D(size_t minNumHits) {
       
//       bool debugIsOn = ( fEvtNum == 5 );  
       bool debugIsOn = false;  
     
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::fit3D, evt " << fEvtNum <<  " min num hits " 
                                << minNumHits << " actual number of Hits " << fDataFor3DFit.size() << std::endl;
       if (debugIsOn && fNoMagnet) std::cerr << " ..........  Magnet has been removed... " << std::endl;
       if (debugIsOn && (!fNoMagnet)) std::cerr << " ..........  Magnet is in the beam line.. " << std::endl;
     
       if (fDataFor3DFit.size() < minNumHits) return false; // other cuts possible. 
       if (myNonLin3DFitPtr == nullptr) {
          myNonLin3DFitPtr = new SSD3DTrackFitFCNAlgo1(fRunNum);
          myNonLin3DFitPtr->SetMagnetShift(fMagShift); 
       }
       myNonLin3DFitPtr->SetDebugOn(debugIsOn);
       myNonLin3DFitPtr->SetInputClusters(fDataFor3DFit);
       myNonLin3DFitPtr->ResetZpos();
       myNonLin3DFitPtr->SetExpectedMomentum(fMomentumInit3DFit); 
       if (fRunNum == 1293)  myNonLin3DFitPtr->SetMCFlag(true); // Very, very ugly, but let us move on..  
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsOut, parsOutErr;
       double x0Start =  fTrXY.XOffset(); double y0Start =  fTrXY.YOffset();
       // Debugging the coordinate transform, something is wrong for Y positive, track, few mm above the gap.  
 //      if (fEvtNum == 5) { x0Start = -14.476; y0Start = 16.3339; }
       uPars.Add(std::string("X_0"), x0Start, 0.1, -20., 20.);  
       uPars.Add(std::string("Slx_0"), 0.00037808, 0.001, -0.1, 0.1);  // for Misligned 7c. No magnet Misalignament flag 25000025, Dgap = 3.0 mm  
       uPars.Add(std::string("Y_0"), y0Start, 0.1, -20., 20.);  
       uPars.Add(std::string("Sly_0"), -0.00244194, 0.001, -0.1, 0.1); 
       unsigned int nPars = 4; 
       if (!fNoMagnet) {
          nPars++;
          uPars.Add(std::string("Mom"), fMomentumInit3DFit, 2., 0.5*fMomentumInit3DFit, 1.5*fMomentumInit3DFit); 
       } 
       std::vector<double> initValsV, initValsE; // for use in the Simple Minimizer.. 
       for (unsigned int k=0; k != nPars; k++) { initValsV.push_back(uPars.Value(k)); initValsE.push_back(uPars.Error(k)); } 
       // Testing the FCN, once .. 
       if (debugIsOn) { 
         std::cerr << " ....... About to test the FCN with " << initValsV.size() << " parameters.. " << std::endl;
         double aChiSqOnce = (*myNonLin3DFitPtr)(initValsV);
//         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that is enough for now " << std::endl; exit(2);
         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that we keep going... " << std::endl;
       }
       ROOT::Minuit2::MnMigrad migrad((*myNonLin3DFitPtr), uPars);
       if (debugIsOn) std::cerr << " ..... About to call migrad... " << std::endl;
       //
       ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
       if (debugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
       const bool isMigradValid = min.IsValid(); 
       bool isSimplexValid = true;
       int flagValid = 0; // 0 nothing worked, 1 MiGrad is O.K., 2, Simplex is Ok.  
       double chiSq = DBL_MAX;
       int numCallFCN = 0;
       if (!isMigradValid) {
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize(*myNonLin3DFitPtr, initValsV, initValsE);
 	 parsOutErr = minS.UserParameters().Errors();
 	 parsOut = minS.UserParameters().Params();
         isSimplexValid = minS.IsValid();
	 chiSq = minS.Fval(); numCallFCN=minS.NFcn();
	 if (isSimplexValid) flagValid = 2;
       } else {
	 chiSq = min.Fval();
 	 parsOutErr = min.UserParameters().Errors();
 	 parsOut = min.UserParameters().Params();
	 flagValid = 1; numCallFCN=min.NFcn();
       }
       // Out to CSV file for R studies. 
       fFOut3DFit << " " << fSubRunNum << " " << fEvtNum << " "; 
       fFOut3DFit << " " << flagValid << " " << chiSq 
                  << " " << fDataFor3DFit.size() - 5 << " " << numCallFCN; 
       if ((!isSimplexValid) && (!isMigradValid)) {
         fFOut3DFit << " " << fTrXY.XOffset() << " " << DBL_MAX << " 0. 0. " << fTrXY.YOffset() << " " << DBL_MAX << " 0. 0. 0. 0. ";
         fFOut3DFit << " " << std::endl;
         return false ; 
       }
       for (unsigned int kPar=0; kPar != parsOut.size();  kPar++) {  // Hopefully, all vector lengths do match .
	 fFOut3DFit << " " << parsOut[kPar] << " " << parsOutErr[kPar];
       }
       fFOut3DFit << " " << std::endl;
      // 
       // could be commented out to avoid too many CSV files..
       std::ostringstream fNameTmpStrStr;
       fNameTmpStrStr << "SSDAlign3DFIResids_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameTmpStr(fNameTmpStrStr.str());
       myNonLin3DFitPtr->OpenOutResids(fNameTmpStr); // we won't re-open, I check for that.. 
       (*myNonLin3DFitPtr)(parsOut); 
       myNonLin3DFitPtr->SpitOutResids(fSubRunNum, fEvtNum);
       if (debugIsOn) { std::cerr << " .. O.K., enough for now, quit!....  " << std::endl; exit(2); } 
      return true ;
     
     }
     bool ssdr::SSDAlign3DUVAlgo1::checkUV(emph::geo::sensorView view, size_t kStation, 
                                           const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
     
       bool debugIsOn = (fEvtNum < 25);    
     
       if (debugIsOn) std::cerr << " SSDAlign3DUVAlgo1::checkUV, view Index " << view << " Station " << kStation 
                 << " nunH " << aSSDClsPtr->size() << "  working on it.. " << std::endl;
	
       if (kStation < 2) {
         std::cerr << " SSDAlign3DUVAlgo1::checkUV, " << view << " Unexpected Station number = " << kStation << " fatal mistake! " << std::endl; 
	 exit(2);
       } 
       if ((view  == emph::geo::W_VIEW) && (kStation < 4)) {
         std::cerr << " SSDAlign3DUVAlgo1::checkUV, " << view << " Unexpected Station number = " << kStation << " fatal mistake! " << std::endl; 
	 exit(2);
       } 

       size_t kStEff = kStation - 2;
       if (view  == emph::geo::W_VIEW) kStEff = kStation - 4;
       double aTrXOffset = fTrXY.XOffset(); double aTrYOffset = fTrXY.YOffset(); 
       double aTrXOffsetErr = fTrXY.XOffsetErr(); double aTrYOffsetErr = fTrXY.YOffsetErr();
       double aTrXSlope = fTrXY.XSlope(); double aTrYSlope = fTrXY.YSlope(); 
       double aTrXSlopeErr = fTrXY.XSlopeErr(); double aTrYSlopeErr = fTrXY.YSlopeErr();
       double aTrXCovOffSl = fTrXY.XCovOffSl(); double aTrYCovOffSl = fTrXY.YCovOffSl();
       	 
       double xPred, yPred, xPredErrSq, yPredErrSq;
       char cView = '?';
       switch (view) {
         case emph::geo::U_VIEW :
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
	 case emph::geo::W_VIEW :
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
      const double uPred = fOneOverSqrt2 * (-xPred + yPred);  // + or - ?? Now, check on simulated data.. 
      const double wPred = -1.0*fOneOverSqrt2 * (xPred + yPred);  // + or - 
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
	  const double uvObs = (view == emph::geo::U_VIEW) ?  myConvertU.GetTsFromCluster(kStation, itCl->Sensor(), aStrip)
	                       :  myConvertV.GetTsFromCluster(kStation, itCl->Sensor(), aStrip);  
	  // special case for station 5, sometimes.. 
	  if ((kSt == 5) && (itCl->View() != emph::geo::U_VIEW)) {
	    if (debugIsOn) std::cerr << " ... .... W or V view, Sensor " << itCl->Sensor() <<  std::endl;
	  } 
	  const double uvObsUncert = (view == emph::geo::U_VIEW) ? myConvertU.GetTsUncertainty(kStation, itCl)
	                              : myConvertV.GetTsUncertainty(kStation, itCl);
	  const double uvUncertSq = uvObsUncert*uvObsUncert + 0.5 * (xPredErrSq + yPredErrSq);
	  double duv = (view == emph::geo::U_VIEW) ? (uPred - uvObs) : (wPred - uvObs);
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
      
       bool gotX = this->recoXY(emph::geo::X_VIEW, aSSDClsPtr);
       if (gotX) {
          if (debugIsOn) std::cerr << " Got a 2d XZ track... for evt " << fEvtNum << std::endl;
	  fTrXY.SetType(rb::XONLY);
       }  else  {
           if (debugIsOn) std::cerr << " Got No 2d XZ track... for evt " << fEvtNum << std::endl;
       }   
	   
       bool gotY = this->recoXY(emph::geo::Y_VIEW, aSSDClsPtr);
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
           if (this->checkUV(emph::geo::U_VIEW, kStU, aSSDClsPtr)) numUVCheck++;
         }
	 if (numUVCheck == 1) fTrXY.SetType(rb::XYUCONF1);
	 if (numUVCheck == 2) fTrXY.SetType(rb::XYUCONF2);
	 // Not worth doing.. stereo angle clearly wrong, of mis labeled. Well, try it again.. 
         for (size_t kStV=4; kStV != fNumStations; kStV++) {
           this->checkUV(emph::geo::W_VIEW, kStV, aSSDClsPtr);
         }
	 if (numUVCheck == 3) fTrXY.SetType(rb::XYUCONF3);
	 if (numUVCheck == 4) fTrXY.SetType(rb::XYUCONF4);
	 if (fDo3DFit) {
	   int nnU = this->recoUV(emph::geo::U_VIEW, aSSDClsPtr);
	   int nnV = this->recoUV(emph::geo::W_VIEW, aSSDClsPtr);
	   if (debugIsOn) std::cerr << " .... Considering doing a 3D fit with nnU " << nnU << " and nnV " << nnV << std::endl; 
	   if (nnU + nnV > 0) this->fit3D(10); 
	 }
       }
       if(fEvtNum > 250000000) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::alignIt, quit here and at event " << fEvtNum << " quit now ! " << std::endl;
	 exit(2); 
       }
     }
   } // ssdr 
} //emph
 
