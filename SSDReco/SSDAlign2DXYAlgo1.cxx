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
#include "SSDReco/SSDAlign2DXYAlgo1.h" 

 using namespace emph;

namespace emph {
  namespace ssdr {

     SSDAlign2DXYAlgo1::SSDAlign2DXYAlgo1() :
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fAlign0to4(false), fNumStationsEff(fNumStations), fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fMomentumIsSet(false), fFilesAreOpen(false),
       fView('?'), fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10), fChiSqCut(20.), 
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoords(fNumStations, 0.), fNominalOffsets(fNumStations, 0.), 
       fNominalOffsetsAlt45(fNumStations, 0.), fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0), fRMSResiduals(fNumStations, 0),
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.), fZLocShifts(fNumStations, 0.),
       fPitchOrYawAngles(fNumStations, 0.) 
     { 
        ; 
     }
     SSDAlign2DXYAlgo1::SSDAlign2DXYAlgo1(char aView) : 
       fOneOverSqrt12(1.0/std::sqrt(12.)),  
       fAlign0to4(false), fNumStationsEff(fNumStations), fRunNum(0), fSubRunNum(0), fEvtNum(0), fNEvents(0), fMomentumIsSet(false), fFilesAreOpen(false),
       fView(aView), fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNumIterMax(10),fChiSqCut(20.), 
       fTokenJob("undef"), fZCoordsMagnetCenter(757.7), fMagnetKick120GeV(-0.612e-3), 
       fZCoords(fNumStations, 0.), fNominalOffsets(fNumStations, 0.), 
       fNominalOffsetsAlt45(fNumStations, 0.), fResiduals(fNumStations, 0.), fMeanResiduals(fNumStations, 0), fRMSResiduals(fNumStations, 0),  
       fMinStrips(fNumStations, -1), fMaxStrips(fNumStations, fNumStrips+1), 
       fMultScatUncert( fNumStations, 0.), fOtherUncert(fNumStations, 0.), fZLocShifts(fNumStations, 0.),
       fPitchOrYawAngles(fNumStations, 0.) 
       
     { 
        if ((aView != 'X') && (aView != 'Y')) {
	     std::cerr << " SSDAlign2DXYAlgo1, setting an unknow view " << aView << " fatal, quit here " << std::endl; 
	     exit(2);
	}
     }
     SSDAlign2DXYAlgo1::~SSDAlign2DXYAlgo1() {
      if (fFOutA1.is_open()) fFOutA1.close();
     }
     void  SSDAlign2DXYAlgo1::InitializeCoords(bool lastIs4, const std::vector<double> &zCoords) {
       fAlign0to4 = lastIs4;
       if (zCoords.size() != 8) {
         std::cerr << "  SSDAlign2DXYAlgo1::InitailizeCoords Unexpected number of ZCoords, " 
	           << zCoords.size() << " fatal, and that is it! " << std::endl; exit(2); 
       }
       for (size_t k=0; k != 5; k++) fZCoords[k] = zCoords[k]; 
       fZCoords[5] = zCoords[7];  // Check this !!!! weird, but I think, correct... 
       std::cerr << " SSDAlign2DXYAlgo1::InitializeCoords, check for weird Z ";
       for (size_t kSt=0; kSt != fZCoords.size(); kSt++) std::cerr << " " << fZCoords[kSt]; 
        myLinFit.SetZCoords(fZCoords);
       std::cerr << " SSDAlign2DXYAlgo1::InitailizeCoords  " << std::endl;
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
       switch (fView) { 
         case 'X' :
           for (size_t k=0; k != 4; k++) { 
             fNominalOffsets[k] = fHalfWaferWidth; // xcoord is proportional to - strip number..
             fNominalOffsetsAlt45[k] = fHalfWaferWidth; // xcoord is proportional to - strip number..Not sure.. 
	   }
	   for (size_t k=4; k != 6; k++) { 
	       fNominalOffsets[k] = 2.0*fHalfWaferWidth; // To be checked.. 
               fNominalOffsetsAlt45[k] = -2.0*fHalfWaferWidth; // xcoord is proportional to - strip number.. To be checked.. 
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
       fMultScatUncert[1] =  0.003830147;   
       fMultScatUncert[2] =  0.01371613;	
       fMultScatUncert[3] =  0.01947578;	
       fMultScatUncert[4] =  0.05067243;  
       fMultScatUncert[5] =  0.06630287;
        
     }
     void ssdr::SSDAlign2DXYAlgo1::SetForMomentum(double p) {
       if (fMomentumIsSet) {
         std::cerr << " ssdr::SSDAlign3DUVAlgo1::SetForMomentum, already called, skip!!! " << std::endl;
	 return;
       }
       const double pRatio = 120.0 / p;
       fMagnetKick120GeV *= pRatio;
       for (size_t k=0; k != fMultScatUncert.size(); k++) fMultScatUncert[k] *= std::abs(pRatio); 
       fMomentumIsSet = true;
     }
     void  SSDAlign2DXYAlgo1::openOutputCsvFiles() {
       std::ostringstream fNameStrStr, fNameDbgStrStr; 
       fNameStrStr << "SSDAlign2D_" << fView << "_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutA1.open(fNameStr.c_str());
       fFOutA1 << " spill evt nHitsT kTr nIter ";
       for (size_t kSt=0; kSt != fNumStationsEff; kSt++) fFOutA1 << fView << kSt << " ";
       fFOutA1 << " trOff trOffErr trSl trSlErr chi ";
       for (size_t kSt=0; kSt != fNumStationsEff; kSt++) fFOutA1 << fView << "r" << kSt << " ";
       fFOutA1 << std::endl;
       // Only a few events, commisioning.. 
       fNameDbgStrStr << "SSDAlign2D_Dbg_" << fView << "_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameDbgStr(fNameDbgStrStr.str());
       fFOutA1Dbg.open(fNameDbgStr.c_str());
       fFOutA1Dbg << " spill evt kTr ";
       for (size_t kSt=0; kSt != fNumStationsEff; kSt++) fFOutA1Dbg << fView << "s" << kSt << " ";
       for (size_t kSt=0; kSt != fNumStationsEff; kSt++) fFOutA1Dbg << fView << kSt << " ";
       fFOutA1Dbg << " trOff trOffErr trSl trSlErr chi ";
       for (size_t kSt=0; kSt != fNumStationsEff; kSt++) fFOutA1Dbg << fView << "r" << kSt << " ";
       fFOutA1Dbg << std::endl;
       fFilesAreOpen = true;
     }
     void  SSDAlign2DXYAlgo1::alignIt(const art::Event &evt, const std::vector<rb::SSDCluster> &aSSDcls) {
       fRunNum = evt.run();
       if (!fFilesAreOpen) this->openOutputCsvFiles();
       fSubRunNum = evt.subRun(); 
       fEvtNum = evt.id().event();
       int kTrSeqNum = 0;
       std::ostringstream headerStrStr; headerStrStr << " "  << fSubRunNum << " " << fEvtNum;
       std::string headerStr(headerStrStr.str());
       fNEvents++; 
       bool debugIsOn = (fNEvents < 15) || (fEvtNum == 4) || (fEvtNum == 9);
       if (debugIsOn) std::cerr <<  " SSDAlign2DXYAlgo1::alingIt, " << " evtNum " << fEvtNum  
                                <<  " number of Cluster " << aSSDcls.size() << std::endl;
       if (aSSDcls. size() < fNumStationsEff) {
         if (debugIsOn) std::cerr <<  " ... Not enough data for a " << fNumStationsEff << " hit fit.. " << std::endl;
	 return;
       }
       std::vector<size_t> nHits(fNumStationsEff, 0);
       for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDcls.cbegin(); itCl != aSSDcls.cend(); itCl++) {
         size_t kSt = itCl->Station();
         if (kSt >=  fNumStationsEff) continue; // Should not happen, when fAlign0to4 is false  
	 if ((itCl->WgtAvgStrip() < fMinStrips[kSt]) || (itCl->WgtAvgStrip() >= fMaxStrips[kSt])) continue;
	 nHits[kSt]++;
       }
       if (debugIsOn) {
          if (!fAlign0to4) std::cerr << " Number of Hits " <<  nHits[0] << ", " << nHits[1] 
                                << ", " <<  nHits[2]  << ", " << nHits[3] << ","  << nHits[4]<< ","  << nHits[5] << std::endl;
	  else 	std::cerr << " Number of Hits " <<  nHits[0] << ", " << nHits[1] 
                                << ", " <<  nHits[2]  << ", " << nHits[3] << ", "  << nHits[4] << std::endl;
       }
       size_t nHitsTotal = 0;		
        for (size_t kSt=0; kSt != fNumStationsEff; kSt++) {
          if (nHits[kSt] == 0) {
	   if (debugIsOn) std::cerr <<  " ... No enough clusters at all for station " << kSt << " So, No fits " << std::endl;
	   return;
	  }
          if (nHits[kSt] > 3) {
	   if (debugIsOn) std::cerr <<  " ... Too many Clusters ( " << nHits[kSt] << " ) for station  " << kSt << " So, No fits " << std::endl;
	   return;
	 }
	 nHitsTotal += nHits[kSt]; 
       }
       
       myLinFit.resids = std::vector<double>(fNumStationsEff, 0.);
       std::vector<double> tsData(fNumStationsEff, 0.); 
       std::vector<double> tsDataErr(fNumStationsEff, 3.0*fPitch); // At start.. to be reviewed based on sims, and prelim results. And use cluster RMS. 
       if (debugIsOn) {
         if (fAlign0to4) std::cerr << " .... Checksize , 0to 4 case, fNumStationsEff " << fNumStationsEff << std::endl;
	 else std::cerr << " .... Checksize , 0to 5 case, fNumStationsEff " << fNumStationsEff << std::endl;

       } 
       double prevChiSq = DBL_MAX;
       double prevPChiSq = DBL_MAX; 
       for (std::vector<rb::SSDCluster>::const_iterator itCl0 = aSSDcls.cbegin(); itCl0 != aSSDcls.cend(); itCl0++) {
	 if (itCl0->Station() != 0) continue;
	 if ((itCl0->WgtAvgStrip() < fMinStrips[0]) || (itCl0->WgtAvgStrip() >= fMaxStrips[0])) continue;
	 tsDataErr[0] = this->GetTsUncertainty(0, itCl0);
	 tsData[0] = getTsFromCluster(0, itCl0->Sensor(), false, itCl0->WgtAvgStrip()); 
	 if (debugIsOn) std::cerr << " ... Stations 0 weighted strip number " 
	                << itCl0->WgtAvgStrip() << " tsData " << tsData[0] << " NominalOff set " << fNominalOffsets[0] 
			<< " assumed Residual " << fResiduals[0] << std::endl;
         for (std::vector<rb::SSDCluster>::const_iterator itCl1 = aSSDcls.cbegin(); itCl1 != aSSDcls.cend(); itCl1++) {
	   if (itCl1->Station() != 1) continue;
	   if ((itCl1->WgtAvgStrip() < fMinStrips[1]) || (itCl1->WgtAvgStrip() >= fMaxStrips[1])) continue;
	   tsData[1] = getTsFromCluster(1, itCl0->Sensor(), false, itCl1->WgtAvgStrip()); 
	   tsDataErr[1] = this->GetTsUncertainty(1, itCl1);
	   if (debugIsOn) std::cerr << " ... Stations 1 weighted strip number " 
	                << itCl1->WgtAvgStrip() << " tsData " << tsData[1] << " NominalOff set " << fNominalOffsets[1] 
			<< " assumed Residual " << fResiduals[1] << std::endl;
          for (std::vector<rb::SSDCluster>::const_iterator itCl2 = aSSDcls.cbegin(); itCl2 != aSSDcls.cend(); itCl2++) {
	     if (itCl2->Station() != 2) continue;
	     if ((itCl2->WgtAvgStrip() < fMinStrips[2]) || (itCl2->WgtAvgStrip() >= fMaxStrips[2])) continue;
	     tsData[2] = getTsFromCluster(2, itCl2->Sensor(),  false, itCl2->WgtAvgStrip()); 
	     tsDataErr[2] = this->GetTsUncertainty(2, itCl2);
	     if (debugIsOn) std::cerr << " ... Stations 2 weighted strip number " 
	                << itCl2->WgtAvgStrip() << " tsData " << tsData[2] << " NominalOff set " << fNominalOffsets[2] 
			<< " assumed Residual " << fResiduals[2] << std::endl;
             for (std::vector<rb::SSDCluster>::const_iterator itCl3 = aSSDcls.cbegin(); itCl3 != aSSDcls.cend(); itCl3++) {
	       if (itCl3->Station() != 3) continue;
	       if ((itCl3->WgtAvgStrip() < fMinStrips[3]) || (itCl3->WgtAvgStrip() >= fMaxStrips[3])) continue;
	       tsData[3] = getTsFromCluster(3, itCl3->Sensor(),   false, itCl3->WgtAvgStrip()); 
	       tsDataErr[3] = this->GetTsUncertainty(3, itCl3);
	       if (debugIsOn) std::cerr << " ... Stations 3 weighted strip number " 
	                << itCl3->WgtAvgStrip() << " tsData " << tsData[3] << " NominalOff set " << fNominalOffsets[3] 
			<< " assumed Residual " << fResiduals[3] << std::endl;
               for (std::vector<rb::SSDCluster>::const_iterator itCl4 = aSSDcls.cbegin(); itCl4 != aSSDcls.cend(); itCl4++) {
	         if (itCl4->Station() != 4) continue;
	         tsData[4] = getTsFromCluster(4, itCl4->Sensor(), false, itCl4->WgtAvgStrip()); 
	         tsDataErr[4] = this->GetTsUncertainty(4, itCl4);
	         if ((itCl4->WgtAvgStrip() < fMinStrips[4]) || (itCl4->WgtAvgStrip() >= fMaxStrips[4])) continue;
	         if (debugIsOn) std::cerr << " .. Stations 4 weighted strip number " 
	                << itCl4->WgtAvgStrip() << " tsData " << tsData[4] << " NominalOff set " << fNominalOffsets[4] 
			<< " assumed Residual " << fResiduals[4] << std::endl;
		 if (fAlign0to4) { // This is mostly repeated code.. Need to create a small private method. 
		     myLinFit.chiSq = DBL_MAX; std::vector<double> tsDataStart(tsData); int nIter = 0;
		     bool didConverged = false;
                     while  ((nIter < fNumIterMax) && (!didConverged)) { // chi-square cut a user parameter, to be tuned.. 
		        if (debugIsOn) std::cerr << " ... .... Case 0 to 4 nIter " << nIter << " before fitLin " << std::endl;
		        myLinFit.fitLin(fAlign0to4, tsData, tsDataErr);
		        if (nIter > 5) {
			  if ((std::abs(prevChiSq - myLinFit.chiSq) < 0.25) && (std::abs(prevChiSq - prevPChiSq) < 0.1)) {
			    if (debugIsOn) std::cerr << " Convergence reached, final chiSq " 
			                             << myLinFit.chiSq << "Previous " << prevChiSq << " previous, previous " << prevPChiSq << std::endl;
			    didConverged = true;			     
			  }
			}
			for(size_t kSt=1; kSt != 4; kSt++) tsData[kSt] -= 0.5*myLinFit.resids[kSt];
			prevPChiSq = prevChiSq; 
			prevChiSq = myLinFit.chiSq; 
			nIter++;
			 if (fEvtNum < 20) {
		           fFOutA1Dbg << headerStr << " " << kTrSeqNum;
			   fFOutA1Dbg << " " << itCl0->WgtAvgStrip() << " " << itCl1->WgtAvgStrip() << " " << itCl2->WgtAvgStrip();
			   fFOutA1Dbg << " " << itCl3->WgtAvgStrip() << " " << itCl4->WgtAvgStrip();
			   for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1Dbg << " " << tsData[kSt];
			   fFOutA1Dbg << " " << myLinFit.offset << " " << myLinFit.sigmaOffset << " " 
			         << myLinFit.slope << " " << myLinFit.sigmaSlope << " " << myLinFit.chiSq;
			   for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1Dbg << " " << myLinFit.resids[kSt];
			   fFOutA1Dbg << std::endl;
			 } 
		       }
		      if ((nIter < fNumIterMax) && (myLinFit.chiSq < fChiSqCut)) {
		         fFOutA1 << headerStr << " " << nHitsTotal << " " << kTrSeqNum << " " << nIter;
			 for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1 << " " << tsDataStart[kSt];
			 fFOutA1 << " " << myLinFit.offset << " " << myLinFit.sigmaOffset << " " 
			         << myLinFit.slope << " " << myLinFit.sigmaSlope << " " << myLinFit.chiSq;
			 for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) 
			    fFOutA1 << " " << tsDataStart[kSt] - tsData[kSt] ;
			 fFOutA1 << std::endl;
			 kTrSeqNum++;
		       }
		 } else { 	
                   for (std::vector<rb::SSDCluster>::const_iterator itCl5 = aSSDcls.cbegin(); itCl5 != aSSDcls.cend(); itCl5++) {
	             if (itCl5->Station() != 5) continue;
	             if ((itCl5->WgtAvgStrip() < fMinStrips[5]) || (itCl5->WgtAvgStrip() >= fMaxStrips[5])) continue;
	             tsData[5] = getTsFromCluster(5, itCl5->Sensor(), false , itCl5->WgtAvgStrip()); 
	             tsDataErr[5] = this->GetTsUncertainty(5, itCl5);
		     // Now fit.. a few times keep transfering the residuals 
		     myLinFit.chiSq = DBL_MAX; std::vector<double> tsDataStart(tsData); int nIter = 0;
		     bool didConverged = false;
                     while  ((nIter < fNumIterMax)  && (!didConverged)) { // chi-square cut a user parameter, to be tuned.. 
		       myLinFit.fitLin(fAlign0to4, tsData, tsDataErr);
		       if (nIter > 5) {
			 if ((std::abs(prevChiSq - myLinFit.chiSq) < 0.25) && (std::abs(prevChiSq - prevPChiSq) < 0.1)) {
			     if (debugIsOn) std::cerr << " Convergence reached, final chiSq " 
			                             << myLinFit.chiSq << "Previous " << prevChiSq << " previous, previous " << prevPChiSq << std::endl;
			     didConverged = true;			     
			  }
			}
			for(size_t kSt=1; kSt !=5; kSt++) tsData[kSt] -= 0.5*myLinFit.resids[kSt];
			prevPChiSq = prevChiSq; 
			prevChiSq = myLinFit.chiSq; 
			nIter++;
			if (fEvtNum < 20) {
		           fFOutA1Dbg << headerStr << " " << kTrSeqNum;
			   fFOutA1Dbg << " " << itCl0->WgtAvgStrip() << " " << itCl1->WgtAvgStrip() << " " << itCl2->WgtAvgStrip();
			   fFOutA1Dbg << " " << itCl3->WgtAvgStrip() << " " << itCl4->WgtAvgStrip() << " " << itCl5->WgtAvgStrip();
			   for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1Dbg << " " << tsData[kSt];
			   fFOutA1Dbg << " " << myLinFit.offset << " " << myLinFit.sigmaOffset << " " 
			         << myLinFit.slope << " " << myLinFit.sigmaSlope << " " << myLinFit.chiSq;
			   for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1Dbg << " " << myLinFit.resids[kSt];
			   fFOutA1Dbg << std::endl;
			} 
		      } // keep adjusting offsets and linear fits. 
		      if ((nIter < fNumIterMax) && (myLinFit.chiSq < fChiSqCut)) {
		         fFOutA1 << headerStr << " " << nHitsTotal << " " << kTrSeqNum << " " << nIter;
			 for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1 << " " << tsDataStart[kSt];
			 fFOutA1 << " " << myLinFit.offset << " " << myLinFit.sigmaOffset << " " 
			         << myLinFit.slope << " " << myLinFit.sigmaSlope << " " << myLinFit.chiSq;
			 for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) 
			    fFOutA1 << " " << tsDataStart[kSt] - tsData[kSt]; 
			 fFOutA1 << std::endl;
			 kTrSeqNum++;
		      }
		    } // itCl5
		  } // 5 or 6 stations hits.. 
	        } // itCl4 
	       } // itCl3      
	     }  // itCl2
	   } // itCl1
         } // itCl0
/*	 
	 if (debugIsOn)   {
	    std::cerr << " ... ... And quit after first event .. " << std::endl;
	    fFOutA1.close(); fFOutA1Dbg.close(); exit(2);
	 }
*/
     } 
     void  SSDAlign2DXYAlgo1::alignItAlt45(const bool skipStation4, const art::Event &evt, const std::vector<rb::SSDCluster> &aSSDcls) {
       fRunNum = evt.run();
       if (!fFilesAreOpen) this->openOutputCsvFiles();
       fSubRunNum = evt.subRun(); 
       fEvtNum = evt.id().event();
       int kTrSeqNum = 0;
       std::ostringstream headerStrStr; headerStrStr << " "  << fSubRunNum << " " << fEvtNum;
       std::string headerStr(headerStrStr.str());
       fNEvents++; 
       bool debugIsOn = (fNEvents < 100) || (fEvtNum == 139999999);
       if (debugIsOn) std::cerr <<  " SSDAlign2DXYAlgo1::alingItAlt45,  number of Cluster " << aSSDcls.size() 
                                << " fNumStationsEff " << fNumStationsEff << std::endl;
       if ((aSSDcls. size() < fNumStationsEff) && (!skipStation4)) {
         if (debugIsOn) std::cerr <<  " ... Not enough data for a " << fNumStationsEff << " hit fit.. " << std::endl;
	 return;
       }
       if ((skipStation4) && (aSSDcls. size() < 5)) {
          if (debugIsOn) std::cerr <<  " ... Not enough data for a " << fNumStationsEff << " hit fit.. " << std::endl;
	 return;
       }
       std::vector<size_t> nHits(fNumStationsEff, 0);
       for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDcls.cbegin(); itCl != aSSDcls.cend(); itCl++) {
         size_t kSt = itCl->Station();
         if (kSt >=  fNumStationsEff) continue; // Should not happen, when fAlign0to4 is false  
	 if ((itCl->WgtAvgStrip() < fMinStrips[kSt]) || (itCl->WgtAvgStrip() >= fMaxStrips[kSt])) continue;
	 nHits[kSt]++;
       }
       if (debugIsOn) {
          if (!fAlign0to4) std::cerr << " Number of Hits " <<  nHits[0] << ", " << nHits[1] 
                                << ", " <<  nHits[2]  << ", " << nHits[3] << ","  << nHits[4]<< ","  << nHits[5] << std::endl;
	  else 	std::cerr << " Number of Hits " <<  nHits[0] << ", " << nHits[1] 
                                << ", " <<  nHits[2]  << ", " << nHits[3] << ", "  << nHits[4] << std::endl;
       }
       size_t nHitsTotal = 0;
       if (!skipStation4) { 		
         for (size_t kSt=0; kSt != fNumStationsEff; kSt++) {
            if (nHits[kSt] == 0) {
	      if (debugIsOn) std::cerr <<  " ... No enough clusters at all for station " << kSt << " So, No fits " << std::endl;
	      return;
	   }
           if (nHits[kSt] > 3) {
	     if (debugIsOn) std::cerr <<  " ... Too many Clusters ( " << nHits[kSt] << " ) for station  " << kSt << " So, No fits " << std::endl;
	     return;
	   }
	   nHitsTotal += nHits[kSt]; 
         }
       } else {
         for (size_t kSt=0; kSt != 4; kSt++) {
            if (nHits[kSt] == 0) {
	      if (debugIsOn) std::cerr <<  " ... No enough clusters at all for station " << kSt << " So, No fits " << std::endl;
	      return;
	   }
           if (nHits[kSt] > 3) {
	     if (debugIsOn) std::cerr <<  " ... Too many Clusters ( " << nHits[kSt] << " ) for station  " << kSt << " So, No fits " << std::endl;
	     return;
	   }
	   nHitsTotal += nHits[kSt]; 
         }
	 if (nHits[5] == 0 ) {
	    if (debugIsOn) std::cerr <<  " ... No enough clusters at all for station 5 So, No fits " << std::endl;
	    return;
	 }
 	 if (nHits[5] > 3 ) {
	    if (debugIsOn) std::cerr <<  " ... Too many clusters for station 5 So, No fits " << std::endl;
	    return;
	 }
	 nHitsTotal += nHits[5]; 
       
       }
       myLinFit.resids = std::vector<double>(fNumStationsEff, 0.);
       std::vector<double> tsData(fNumStationsEff, 0.); 
       std::vector<double> tsDataErr(fNumStationsEff, 3.0*fPitch); // At start.. to be reviewed based on sims, and prelim results. And use cluster RMS. 
       if (debugIsOn) {
	 std::cerr << " .... Checksize , 0to 5 case, fNumStationsEff " << fNumStationsEff << std::endl;

       } 
       double prevChiSq = DBL_MAX;
       double prevPChiSq = DBL_MAX; 
       for (std::vector<rb::SSDCluster>::const_iterator itCl0 = aSSDcls.cbegin(); itCl0 != aSSDcls.cend(); itCl0++) {
	 if (itCl0->Station() != 0) continue;
	 if ((itCl0->WgtAvgStrip() < fMinStrips[0]) || (itCl0->WgtAvgStrip() >= fMaxStrips[0])) continue;
	 tsDataErr[0] = this->GetTsUncertainty(0, itCl0);
	 tsData[0] = getTsFromCluster(0, itCl0->Sensor(), false, itCl0->WgtAvgStrip()); 
	 if (debugIsOn) std::cerr << " ... Stations 0 weighted strip number " 
	                << itCl0->WgtAvgStrip() << " tsData " << tsData[0] << " NominalOff set " << fNominalOffsets[0] 
			<< " assumed Mean Residual " << fMeanResiduals[0] << std::endl;
         for (std::vector<rb::SSDCluster>::const_iterator itCl1 = aSSDcls.cbegin(); itCl1 != aSSDcls.cend(); itCl1++) {
	   if (itCl1->Station() != 1) continue;
	   if ((itCl1->WgtAvgStrip() < fMinStrips[1]) || (itCl1->WgtAvgStrip() >= fMaxStrips[1])) continue;
	   tsData[1] = getTsFromCluster(1, itCl1->Sensor(), false, itCl1->WgtAvgStrip()); 
	   tsDataErr[1] = this->GetTsUncertainty(1, itCl1);
	   if (debugIsOn) std::cerr << " ... Stations 1 weighted strip number " 
	                << itCl1->WgtAvgStrip() << " tsData " << tsData[1] << " NominalOff set " << fNominalOffsets[1] 
			<< " assumed Mean Residual " << fMeanResiduals[1] << std::endl;
          for (std::vector<rb::SSDCluster>::const_iterator itCl2 = aSSDcls.cbegin(); itCl2 != aSSDcls.cend(); itCl2++) {
	     if (itCl2->Station() != 2) continue;
	     if ((itCl2->WgtAvgStrip() < fMinStrips[2]) || (itCl2->WgtAvgStrip() >= fMaxStrips[2])) continue;
	     tsData[2] = getTsFromCluster(2, itCl2->Sensor(), false, itCl2->WgtAvgStrip()); 
	     tsDataErr[2] = this->GetTsUncertainty(2, itCl2);
	     if (debugIsOn) std::cerr << " ... Stations 2 weighted strip number " 
	                << itCl2->WgtAvgStrip() << " tsData " << tsData[2] << " NominalOff set " << fNominalOffsets[2] 
			<< " assumed mean Residual " << fMeanResiduals[2] << std::endl;
             for (std::vector<rb::SSDCluster>::const_iterator itCl3 = aSSDcls.cbegin(); itCl3 != aSSDcls.cend(); itCl3++) {
	       if (itCl3->Station() != 3) continue;
	       if ((itCl3->WgtAvgStrip() < fMinStrips[3]) || (itCl3->WgtAvgStrip() >= fMaxStrips[3])) continue;
	       tsData[3] = getTsFromCluster(3, itCl3->Sensor(), false, itCl3->WgtAvgStrip()); 
	       tsDataErr[3] = this->GetTsUncertainty(3, itCl3);
	       bool didSinglePseudoHitStation4 = false;
	       if (debugIsOn) std::cerr << " ... Stations 3 weighted strip number " 
	                << itCl3->WgtAvgStrip() << " tsData " << tsData[3] << " NominalOff set " << fNominalOffsets[3] 
			<< " assumed mean Residual " << fMeanResiduals[3] << std::endl;
               for (std::vector<rb::SSDCluster>::const_iterator itCl4 = aSSDcls.cbegin(); itCl4 != aSSDcls.cend(); itCl4++) {
	         if (!skipStation4) {   
	           if (itCl4->Station() != 4) continue;
	           tsData[4] = getTsFromCluster(4, itCl4->Sensor(), false, itCl4->WgtAvgStrip()); 
	           tsDataErr[4] = this->GetTsUncertainty(4, itCl4);
	           if ((itCl4->WgtAvgStrip() < fMinStrips[4]) || (itCl4->WgtAvgStrip() >= fMaxStrips[4])) continue;
	           if (debugIsOn) std::cerr << " .. Stations 4 weighted strip number " 
	                << itCl4->WgtAvgStrip() << " tsData " << tsData[4] << " NominalOff set " << fNominalOffsets[4] 
			<< " assumed Residual " << fResiduals[4] << std::endl;
	         } else {
		   if (didSinglePseudoHitStation4) continue;
	           tsData[4] = 0.; 
		   tsDataErr[4] = 1.0e4;
		   didSinglePseudoHitStation4 = true;
		 }
                 for (std::vector<rb::SSDCluster>::const_iterator itCl5 = aSSDcls.cbegin(); itCl5 != aSSDcls.cend(); itCl5++) {
	           if (itCl5->Station() != 5) continue;
	           if ((itCl5->WgtAvgStrip() < fMinStrips[5]) || (itCl5->WgtAvgStrip() >= fMaxStrips[5])) continue;
	           tsData[5] = getTsFromCluster(5, itCl5->Sensor(), false , itCl5->WgtAvgStrip()); 
	           tsDataErr[5] = this->GetTsUncertainty(5, itCl5);
		   // Now fit.. a few times keep transfering the residuals 
		   myLinFit.chiSq = DBL_MAX; std::vector<double> tsDataStart(tsData); int nIter = 0;
		   bool didConverged = false;
                   while  ((nIter < fNumIterMax)  && (!didConverged)) { // chi-square cut a user parameter, to be tuned.. 
		     myLinFit.fitLin(fAlign0to4, tsData, tsDataErr);
		     if (nIter > 5) {
		       if ((std::abs(prevChiSq - myLinFit.chiSq) < 0.25) && (std::abs(prevChiSq - prevPChiSq) < 0.1)) {
		           if (debugIsOn) std::cerr << " Convergence reached, final chiSq " 
		        			   << myLinFit.chiSq << "Previous " << prevChiSq << " previous, previous " << prevPChiSq << std::endl;
		           didConverged = true; 			   
		        }
		      }
		      for(size_t kSt=4; kSt !=6; kSt++) tsData[kSt] -= 0.5*myLinFit.resids[kSt];
		      prevPChiSq = prevChiSq; 
		      prevChiSq = myLinFit.chiSq; 
		      nIter++;
		      if (fEvtNum < 200) {
		 	 fFOutA1Dbg << headerStr << " " << kTrSeqNum;
		         fFOutA1Dbg << " " << itCl0->WgtAvgStrip() << " " << itCl1->WgtAvgStrip() << " " << itCl2->WgtAvgStrip();
		         fFOutA1Dbg << " " << itCl3->WgtAvgStrip() << " " << itCl4->WgtAvgStrip() << " " << itCl5->WgtAvgStrip();
		         for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1Dbg << " " << tsData[kSt];
		         fFOutA1Dbg << " " << myLinFit.offset << " " << myLinFit.sigmaOffset << " " 
		               << myLinFit.slope << " " << myLinFit.sigmaSlope << " " << myLinFit.chiSq;
		         for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1Dbg << " " << myLinFit.resids[kSt];
		         fFOutA1Dbg << std::endl;
		      } 
		    } // keep adjusting offsets and linear fits. 
		    if ((nIter < fNumIterMax) && (myLinFit.chiSq < fChiSqCut)) {
		       fFOutA1 << headerStr << " " << nHitsTotal << " " << kTrSeqNum << " " << nIter;
		       for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) fFOutA1 << " " << tsDataStart[kSt];
		       fFOutA1 << " " << myLinFit.offset << " " << myLinFit.sigmaOffset << " " 
		               << myLinFit.slope << " " << myLinFit.sigmaSlope << " " << myLinFit.chiSq;
		       for(size_t kSt=0; kSt != tsDataStart.size(); kSt++) 
		          fFOutA1 << " " << tsDataStart[kSt] - tsData[kSt]; 
		       fFOutA1 << std::endl;
		       kTrSeqNum++;
		    }
		  } // itCl5
	        } // itCl4 
	       } // itCl3      
	     }  // itCl2
	   } // itCl1
         } // itCl0
/*	 
	 if (debugIsOn)   {
	    std::cerr << " ... ... And quit after first event .. " << std::endl;
	    fFOutA1.close(); fFOutA1Dbg.close(); exit(2);
	 }
*/
     } 

  } // ssdr
} // emph
