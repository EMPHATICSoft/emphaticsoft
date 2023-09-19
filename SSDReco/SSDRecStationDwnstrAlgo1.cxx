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

#include "RunHistory/RunHistory.h"
#include "SSDReco/SSDHotChannelList.h" // for futur rejection of spurious hits. 
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/SSDRecStationDwnstrAlgo1.h" 
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "RecoBase/SSDStationPtAlgo1.h" 
#include "SSDReco/VolatileAlignmentParams.h" // Our real geometry for now.. 

 using namespace emph;

namespace emph {
  namespace ssdr {
  
    const double SSDRecStationDwnstrAlgo1::fSqrt2 = std::sqrt(2.0);
    const double SSDRecStationDwnstrAlgo1::fOneOverSqrt2= 1.0/std::sqrt(2.0);

    SSDRecStationDwnstrAlgo1::SSDRecStationDwnstrAlgo1(size_t kSt) :
      fRunHistory(nullptr),   
      fEmgeo(nullptr),
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), 
      fCoordConvert('A'),
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0), fStationNum(static_cast<int>(kSt)), 
      fNEvents(0), fDebugIsOn(false),
      fIsMC(false), // Ugly, we are still working on the sign convention and rotation angles signs.. 
      fDoFirstAndLastStrips(false),
      fChiSqCut(5.0), fChiSqCutPreArb(DBL_MAX),// for XYU (or XYW) cut. 
      fXWindowWidth(1.0e4), fYWindowWidth(1.0e4), fXWindowCenter(0.), fYWindowCenter(0.),
      fPrelimMomentum(5.0),
      fTokenJob("undef"), fStPoints(), fFOutSt(nullptr), fFOutStYFirst(nullptr), fFOutStYLast(nullptr), 
      fClUsages(), fNxCls(0), fNyCls(0), fNuCls(0)  {
	 if ((fStationNum < 2) || (fStationNum > 5)) {
	      std::cerr << " SSDRecStationDwnstrAlgo1 Station number, value " << fStationNum 
	                << " is wrong for phase1b data, quit here and now " << std::endl;
			exit(2);
	 }
         fCoordConvert.SetForMC(fIsMC); 
      }
    //
     SSDRecStationDwnstrAlgo1::~SSDRecStationDwnstrAlgo1() {
       if (fFOutSt != nullptr) { 
         if (fFOutSt->is_open()) fFOutSt->close();
         delete fFOutSt;
       } 
       if (fFOutStYFirst != nullptr) { 
         if (fFOutStYFirst->is_open()) fFOutStYFirst->close();
         delete fFOutStYFirst;
       }
         if (fFOutStYLast != nullptr) {
         if (fFOutStYLast->is_open()) fFOutStYLast->close();
         delete fFOutStYLast;
       }
     }
     // 
     size_t SSDRecStationDwnstrAlgo1::RecIt(const art::Event &evt, const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       fIdStPtNow = 0;
       if (fStationNum == INT_MAX) {
         std::cerr <<  " SSDRecStationDwnstrAlgo1::RecIt The station number for this instance is not defined.  ";
	 std::cerr << " One must call SetStationNumber.. Fatal " << std::endl; exit(2);
       }
       fRunNum = evt.run(); fSubRunNum = evt.subRun(); fEvtNum = evt.id().event();
       if (fDebugIsOn) {
           std::cerr << " SSDRecStationDwnstrAlgo1::RecIt, starting on spill " 
                                 << fSubRunNum << " evt " << fEvtNum << " with " << aSSDClsPtr->size() << " Clusters (all Stations) " << std::endl;
//	   fCoordConvert.SetDebugOn(true);
       }			 
       if (fEmgeo == nullptr) {
         fRunHistory = new runhist::RunHistory(fRunNum);   
         fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
        } 
	// This should be part of the Geometry package.. 
	if (!fCoordConvert.IsReadyToGo()) {
	 // We use the nominal Z position, for now.. 
	 std::vector<double> zPosStations;
	 std::cerr << " SSDRecStationDwnstrAlgo1::RecIt, Station " << fStationNum << " Storing Z Positions ..." << std::endl; 
	 for (size_t kSt=0; kSt != 6; kSt++) {
	   TVector3 tmpPos = fEmgeo->GetSSDStation(kSt).Pos(); 
	   double zz = tmpPos[2];
	   zPosStations.push_back(zz); 
	   if (kSt > 3) zPosStations.push_back(zz); 
	   std::cerr << " ..... Z at station " << kSt << " index " << zPosStations.size() -1 
	             << " is " << zPosStations[zPosStations.size() -1]  << std::endl;
	 }
	 fCoordConvert.InitializeAllCoords(zPosStations); 
       }   
       fStPoints.clear(); fClUsages.clear(); fNxCls=0; fNyCls=0; fNuCls=0; 
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
          fClUsages.push_back(0);
          if (itCl->Station() != fStationNum) continue;
	  switch (itCl->View()) {
	    case emph::geo::X_VIEW : { fNxCls++; break;} 
	    case emph::geo::Y_VIEW : { fNyCls++; break;} 
	    case emph::geo::U_VIEW : case emph::geo::W_VIEW : { fNuCls++; break;}
	    default : // should not happen 
	      break; 
	  }
       }
       if (fDebugIsOn) std::cerr << " ..... At Station " << fStationNum 
                                 << "  Got " << fNxCls << ", " << fNyCls << ", " << fNuCls << " X, Y (u or V) clusters " << std::endl;
       if ((fNxCls == 0) && (fNyCls == 0)) return 0; 
       if ((fNxCls == 0) && (fNuCls == 0)) return 0; 
       if ((fNyCls == 0) && (fNuCls == 0)) return 0; 
       
       size_t nCurrent = 0; 
       if ((fNxCls != 0) && (fNyCls != 0) && (fNuCls != 0)) nCurrent = this->recoXYUorW(aSSDClsPtr);
       if (fDebugIsOn) std::cerr << " .... Got " << nCurrent << " Confirmed 3D points " << std::endl; 
       //
       // Second phase, two cluster points.. Tally usage first. 
       //
       int fNxCls2nd=0; int fNyCls2nd=0; int fNuCls2nd=0; // Should we make them class members? 
       size_t ku=0;
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++, ku++) {
          if (fClUsages[ku] != 0) continue;
          if (itCl->Station() != fStationNum) continue;
	  switch (itCl->View()) {
	    case emph::geo::X_VIEW : { fNxCls2nd++; break;} 
	    case emph::geo::Y_VIEW : { fNyCls2nd++; break;} 
	    case emph::geo::U_VIEW : case emph::geo::W_VIEW : { fNuCls2nd++; break;} 
	    default : break;
	  }
       }
       if ((fNxCls2nd == 0) && (fNyCls2nd == 0)) { return this->Size(); }
       if ((fNxCls2nd == 0) && (fNuCls2nd == 0)) { return this->Size(); }
       if ((fNyCls2nd == 0) && (fNuCls2nd == 0)) { return this->Size(); }
       size_t nAdded = 0; 
       if ((fNxCls2nd != 0) && (fNyCls2nd != 0)) nAdded = this->recoXY(aSSDClsPtr);
       if ((fNxCls2nd != 0) && (fNuCls2nd != 0)) nAdded = this->recoXUorW(aSSDClsPtr);
       if ((fNyCls2nd != 0) && (fNuCls2nd != 0)) nAdded = this->recoYUorW(aSSDClsPtr);
       if (fDebugIsOn) {
//          std::cerr << " .... Adding  " << nAdded << " Tentative 3D points " << " ... And quit right here and now " << std::endl;
//	  exit(2);
          std::cerr << " .... Adding  " << nAdded << " Tentative 3D points " << " ... And keep going " << std::endl;
       }
       return this->Size(); 
     }
     //
     // Now the details..
     //
     size_t SSDRecStationDwnstrAlgo1::recoXYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       size_t kux = 0; 
       size_t nStart = fStPoints.size();
       if (fDebugIsOn) {
          std::cerr << " Starting SSDRecStationDwnstrAlgo1::recoXYUorW, Station " << fStationNum 
                        << "  with a total of " << aSSDClsPtr->size() << " clusters " << std::endl;
       }	  
       fCoordConvert.SetDebugOn(fDebugIsOn);
       const size_t kSt = static_cast<size_t>(fStationNum);
       for(std::vector<rb::SSDCluster>::const_iterator itClX = aSSDClsPtr->cbegin(); itClX != aSSDClsPtr->cend(); itClX++, kux++) {
         if (fClUsages[kux] != 0) continue;
         if (itClX->Station() != fStationNum) continue;
	 if (itClX->View() != emph::geo::X_VIEW) continue;
	 size_t kSeX = static_cast<size_t>(itClX->Sensor());
	 const std::pair<double, double> xDat = fCoordConvert.getTrCoord(itClX, fPrelimMomentum);
	 if (std::abs(xDat.first - fXWindowCenter) > fXWindowWidth) {
	   if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected X location  " 
	                              << fXWindowCenter << " delta " << xDat.first - fXWindowCenter << std::endl;
	   continue;
	 }
	 if (fDebugIsOn) {
	   std::cerr << " At cluster on X view, station " << itClX->Station() << " Sensor  " 
	             << kSeX << " weighted strip " << itClX->WgtAvgStrip() 
		     << " RMS " << itClX->WgtRmsStrip() << " ClID " << itClX->ID() << " X (no Roll) " 
		     << xDat.first << " +- " << std::sqrt(xDat.second) << std::endl;
	 }
         size_t kuy = 0;
	 const double angleRollX = fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, kSeX);
	 const double angleRollCenterX = fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, kSeX);
	 if (fDebugIsOn) std::cerr << " .... xDat " << xDat.first << " +- " << std::sqrt(xDat.second) << std::endl; 
         for(std::vector<rb::SSDCluster>::const_iterator itClY = aSSDClsPtr->cbegin(); itClY != aSSDClsPtr->cend(); itClY++, kuy++) {
           if (fClUsages[kuy] != 0) continue;
           if (itClY->Station() != fStationNum) continue;
	   if (itClY->View() != emph::geo::Y_VIEW) continue;
	   size_t kSeY = static_cast<size_t>(itClY->Sensor());
	   const std::pair<double, double> yDat = fCoordConvert.getTrCoord(itClY, fPrelimMomentum);
	   if (std::abs(yDat.first - fYWindowCenter) > fYWindowWidth) {
	     if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected Y location  " 
	                              << fYWindowCenter << " delta " << yDat.first - fYWindowCenter << std::endl;
	     continue;
	   }
	   if (fDebugIsOn) {
	     std::cerr << " ... At cluster on Y view, station " << itClY->Station() << " Sensor  " 
	             << kSeY << " weighted strip " << itClY->WgtAvgStrip() << " RMS " 
		     << itClY->WgtRmsStrip() << " ClID " << itClY->ID() << " Y (no Roll) " 
		     << yDat.first << " +- " << std::sqrt(yDat.second) << std::endl;
	   }
	   if (std::abs(yDat.first) > 60.) { // about 20 mm gap? 
	     if (fDebugIsOn) {
	        std::cerr << " ..... Very large Y coordinate .." << yDat.first << " Investigate.. " << std::endl; 
//		fCoordConvert.SetDebugOn(true); 
		const std::pair<double, double> yDatCheck = fCoordConvert.getTrCoord(itClY, fPrelimMomentum);
		std::cerr << " ... yDatCheck " << yDatCheck.first << "  And quit for now... " << std::endl;
		exit(2); 
	     } else continue;
	   }
	   const double angleRollY = fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, kSeY);
	   const double angleRollCenterY = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, kSt, kSeY);
	   const double xValCorr = xDat.first + (yDat.first - angleRollCenterX) * angleRollX; 
	   const double yValCorr = yDat.first + (xDat.first - angleRollCenterY) * angleRollY;
//   Now presumably correct.  Sept 8 2023. token job Run_1274_NoTgt31Gev_ClSept_A1e_1o1, or _c7
	   const double uPred = fOneOverSqrt2 * ( xValCorr + yValCorr);
	   const double vPred = -1.0*fOneOverSqrt2 * ( -xValCorr + yValCorr);
//   Wrong, presumably, tokenjob Run_1274_NoTgt31Gev_ClSept_A1e_1o1_W1
//	   const double vPred = fOneOverSqrt2 * ( xValCorr + yValCorr);
//	   const double uPred = -1.0*fOneOverSqrt2 * ( -xValCorr + yValCorr);
//   Wrong, presumably, tokenjob Run_1274_NoTgt31Gev_ClSept_A1e_1o1_W2  Chenge the sign of 
//	   const double uPred = -1.0*fOneOverSqrt2 * ( xValCorr + yValCorr);
//	   const double vPred = fOneOverSqrt2 * ( -xValCorr + yValCorr);
           size_t kuu = 0;
	   if (fDebugIsOn) std::cerr << " ... uPred " << uPred << " vPred " << vPred << std::endl; 
           for(std::vector<rb::SSDCluster>::const_iterator itClUorV = aSSDClsPtr->cbegin(); itClUorV != aSSDClsPtr->cend(); itClUorV++, kuu++) {
             if (fClUsages[kuu] != 0) continue;
             if (itClUorV->Station() != fStationNum) continue;
	     if ((itClUorV->View() ==  emph::geo::X_VIEW) ||(itClUorV->View() ==  emph::geo::Y_VIEW)) continue;
	     // To leave some flexibility on what we call U or W.. No X or Y !  
//	     if ((kSt < 4) && (itClUorV->View() != emph::geo::W_VIEW)) continue;
//	     if ((kSt > 3) && (itClUorV->View() != emph::geo::U_VIEW)) continue;
	     size_t kSeU = static_cast<size_t>(itClUorV->Sensor()); // dropping the suffix"orV". 
	     if (fDebugIsOn) {
	       std::cerr << " ... ... At cluster on UorV view, station " << itClUorV->Station() << " Sensor  " 
	             << kSeU << " View " <<  itClUorV->View() << " weighted strip " << itClUorV->WgtAvgStrip() 
		     << " RMS " << itClUorV->WgtRmsStrip() << " ClID " << itClUorV->ID() << std::endl;
	     }
	     const std::pair<double, double> uorvDat = fCoordConvert.getTrCoord(itClUorV, fPrelimMomentum);
	     const double angleRollUorV = fEmVolAlP->Roll(itClUorV->View(), kSt, kSeU);  
	     const double angleRollCenterUorV = fEmVolAlP->RollCenter(itClUorV->View(), kSt, kSeU); 
	     const double uorvPred = (kSt > 3) ? vPred + ( uPred - angleRollCenterUorV) * angleRollUorV :  
	                                         uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV ;
	     const double deltaXYU = uorvPred - uorvDat.first;
	     if (fDebugIsOn) std::cerr << " .... After correction for Rolls, uorvPred " << uorvPred  
	                               << " Data " << uorvDat.first << " Diff " << deltaXYU << std::endl;
	     const double uPredErrSq = 0.5 * (xDat.second + yDat.second);
	     // Blow-up the erro for the V view, given the lack of accuracy in the alignment Assume 1 mm to 1 nanometer, to test.  
	     const double deltaErrSq = (itClUorV->View() == emph::geo::W_VIEW) ? (1.0e-9 +  uPredErrSq + uorvDat.second) : 
	                                                                          (uPredErrSq + uorvDat.second);
	     const double aChiSq = (deltaXYU * deltaXYU)/deltaErrSq;
	     if (fDebugIsOn) std::cerr << " .... uDat " << uorvDat.first << " +- " << std::sqrt(uorvDat.second) 
	                             <<  " deltaXYU  " << deltaXYU << " +-  " << std::sqrt(deltaErrSq) << " chiSq " << aChiSq << std::endl; 
	     if (aChiSq > fChiSqCut) continue;
	     // tag and store 
	     fClUsages[kuu] = 1; fClUsages[kux] = 1; fClUsages[kuy] = 1;
	     rb:: SSDStationPtAlgo1 aStPt;
	     const double errX = std::sqrt(std::abs(xDat.second));  const double errY = std::sqrt(std::abs(yDat.second)); 
	     aStPt.SetX(xValCorr, errX);  // we could do a fit.. Note the 3D Downstream track will use measurement data. 
	     aStPt.SetID(fIdStPtNow);
	     aStPt.SetY(yValCorr, errY);
	     aStPt.SetUorVInfo(uorvPred, 0.06*itClUorV->WgtAvgStrip(), uorvDat.first); 
	     aStPt.SetChiSq(aChiSq);
	     aStPt.SetStation(fStationNum);
	     aStPt.Add(itClX, xValCorr, errX);  
	     aStPt.Add(itClY, yValCorr, errY); 
	     aStPt.Add(itClUorV, uorvPred, std::sqrt(uPredErrSq));
	     fIdStPtNow++;
	     if (fDebugIsOn) std::cerr << " ......  Adding " << std::endl << aStPt << std::endl;; 
	     fStPoints.push_back(aStPt);
	   } // on confirming U or W view   
	 } // onY view
       } // on X view 
       return fStPoints.size() - nStart;
     } // recoXYUorW
     //
     size_t SSDRecStationDwnstrAlgo1::recoXY(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       size_t kux = 0;
       size_t nStart = fStPoints.size();
       const size_t kSt = static_cast<size_t>(fStationNum);
       if (fDebugIsOn) std::cerr << " Starting SSDRecStationDwnstrAlgo1::recoXY, Station " << fStationNum 
                        << "  with a total of " << aSSDClsPtr->size() << " clusters " << std::endl;
       for(std::vector<rb::SSDCluster>::const_iterator itClX = aSSDClsPtr->cbegin(); itClX != aSSDClsPtr->cend(); itClX++, kux++) {
         if (fClUsages[kux] != 0) continue;
         if (itClX->Station() != fStationNum) continue;
	 if (itClX->View() != emph::geo::X_VIEW) continue;
	 size_t kSeX = static_cast<size_t>(itClX->Sensor());
	 const std::pair<double, double> xDat = fCoordConvert.getTrCoord(itClX, fPrelimMomentum);
         size_t kuy = 0;
	 const double angleRollX = fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, kSeX);
	 const double angleRollCenterX = fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, kSeX);
	 if (fDebugIsOn) {
	   std::cerr << " At cluster on X view, station " << itClX->Station() << " Sensor  " 
	             << kSeX << " weighted strip " << itClX->WgtAvgStrip() << " RMS " << itClX->WgtRmsStrip() << std::endl;
	 }
	 if (std::abs(xDat.first - fXWindowCenter) > fXWindowWidth) {
	   if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected X location  " 
	                              << fXWindowCenter << " delta " << xDat.first - fXWindowCenter << std::endl;
	   continue;
	 }
         for(std::vector<rb::SSDCluster>::const_iterator itClY = aSSDClsPtr->cbegin(); itClY != aSSDClsPtr->cend(); itClY++, kuy++) {
           if (fClUsages[kuy] != 0) continue;
           if (itClY->Station() != fStationNum) continue;
	   if (itClY->View() != emph::geo::Y_VIEW) continue;
	   size_t kSeY = static_cast<size_t>(itClY->Sensor());
	   const std::pair<double, double> yDat = fCoordConvert.getTrCoord(itClY, fPrelimMomentum);
	   const double angleRollY = fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, kSeY);
	   const double angleRollCenterY = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, kSt, kSeY);
	   const double xValCorr = xDat.first + (yDat.first - angleRollCenterX) * angleRollX; 
	   const double yValCorr = yDat.first + (xDat.first - angleRollCenterY) * angleRollY;
	   if (fDebugIsOn) {
	     std::cerr << " At cluster on Y view, station " << itClY->Station() << " Sensor  " 
	             << kSeY << " weighted strip " << itClY->WgtAvgStrip() << " RMS " << itClY->WgtRmsStrip() << std::endl;
	   }
	   if (std::abs(yDat.first - fYWindowCenter) > fYWindowWidth) {
	     if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected Y location  " 
	                              << fYWindowCenter << " delta " << yDat.first - fYWindowCenter << std::endl;
	     continue;
	   }
           fClUsages[kux] = 1; fClUsages[kuy] = 1;
	   // constraints, store.. 
	   rb:: SSDStationPtAlgo1 aStPt;
	   const double errX = std::sqrt(std::abs(xDat.second));  const double errY = std::sqrt(std::abs(yDat.second)); 
	   aStPt.SetID(fIdStPtNow);
	   aStPt.SetX(xValCorr, errX);  // we could do a fit.. Note the 3D Downstream track will use measurement data. 
	   aStPt.SetY(yValCorr, errY); 
	   aStPt.SetStation(fStationNum);
	   aStPt.Add(itClX, xValCorr, errX);  
	   aStPt.Add(itClY, yValCorr, errY);  
	   if (fDebugIsOn) std::cerr << " Adding " << std::endl << aStPt << std::endl;; 
	   fStPoints.push_back(aStPt);
	   fIdStPtNow++;
	 } // onY view
       } // on X view 
       return fStPoints.size() - nStart;
     } // recoXY
     
     size_t SSDRecStationDwnstrAlgo1::recoXUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       size_t kux = 0;
       size_t nStart = fStPoints.size();
       const size_t kSt = static_cast<size_t>(fStationNum);
       if (fDebugIsOn) std::cerr << " Starting SSDRecStationDwnstrAlgo1::recoXUorW, Station " << fStationNum 
                        << "  with a total of " << aSSDClsPtr->size() << " clusters " << std::endl;
       for(std::vector<rb::SSDCluster>::const_iterator itClX = aSSDClsPtr->cbegin(); itClX != aSSDClsPtr->cend(); itClX++, kux++) {
         if (fClUsages[kux] != 0) continue;
         if (itClX->Station() != fStationNum) continue;
	 if (itClX->View() != emph::geo::X_VIEW) continue;
	 size_t kSeX = static_cast<size_t>(itClX->Sensor());
	 const std::pair<double, double> xDat = fCoordConvert.getTrCoord(itClX, fPrelimMomentum);
         size_t kuu = 0;
	 const double angleRollX = fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, kSeX);
	 const double angleRollCenterX = fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, kSeX);
	 if (fDebugIsOn) {
	   std::cerr << " At cluster on X view, station " << itClX->Station() << " Sensor  " 
	             << kSeX << " weighted strip " << itClX->WgtAvgStrip() << " RMS " << itClX->WgtRmsStrip() << std::endl;
	 }
	 if (std::abs(xDat.first - fXWindowCenter) > fXWindowWidth) {
	   if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected X location  " 
	                              << fXWindowCenter << " delta " << xDat.first - fXWindowCenter << std::endl;
	   continue;
	 }
         for(std::vector<rb::SSDCluster>::const_iterator itClUorV = aSSDClsPtr->cbegin(); itClUorV != aSSDClsPtr->cend(); itClUorV++, kuu++) {
           if (fClUsages[kuu] != 0) continue;
           if (itClUorV->Station() != fStationNum) continue;
	   if ((kSt < 4) && (itClUorV->View() != emph::geo::W_VIEW)) continue;
	   if ((kSt > 3) && (itClUorV->View() != emph::geo::U_VIEW)) continue;
	   size_t kSeUorV = static_cast<size_t>(itClUorV->Sensor());
	   const std::pair<double, double> uorvDat = fCoordConvert.getTrCoord(itClUorV, fPrelimMomentum);
	   const double angleRollUorV = (kSt <4) ? fEmVolAlP->Roll(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                        fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);  
	   const double angleRollCenterUorV = (kSt <4) ? fEmVolAlP->RollCenter(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                              fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);
	   double yPred, uPred, vPred;
	   if (fDebugIsOn) {
	       std::cerr << " At cluster on UorV view, station " << itClUorV->Station() << " Sensor  " 
	             << kSeUorV << " weighted strip " << itClUorV->WgtAvgStrip() << " RMS " << itClUorV->WgtRmsStrip() << std::endl;
	   }
	   // To be checked, depends on Delta Roll definition Sept 5 2023. 
	   yPred = (kSt > 3) ?  (fSqrt2 * uorvDat.first - xDat.first) : (-fSqrt2 * uorvDat.first + xDat.first);
	   uPred = fOneOverSqrt2 * ( xDat.first + yPred);
	    vPred = -1.0*fOneOverSqrt2 * ( -xDat.first + yPred);
	   const double uorvValCorr = (kSt > 3) ? uPred + ( vPred - angleRollCenterUorV) * angleRollUorV :  
	                                          vPred + ( uPred  - angleRollCenterUorV) * angleRollUorV ;
	   const double xValCorr = xDat.first + (yPred - angleRollCenterX) * angleRollX; 
	   const double yValCorr = (kSt > 3) ?  (fSqrt2 * uorvValCorr - xDat.first) : (-fSqrt2 * uorvValCorr + xDat.first);
           fClUsages[kux] = 1; fClUsages[kuu] = 1;
	   // constraints, store.. 
	   rb:: SSDStationPtAlgo1 aStPt;
	   const double errX = std::sqrt(std::abs(xDat.second));  
	   const double errY = fSqrt2*(std::sqrt(std::abs(uorvDat.second) + xDat.second)); 
	   // assume uncorrelated errors.. Not quite true, aligmnment.  	   
	   aStPt.SetID(fIdStPtNow);
	   aStPt.SetX(xValCorr, errX);  // we could do a fit.. Note the 3D Downstream track will use measurement data. 
	   aStPt.SetY(yValCorr, errY); 
	   aStPt.SetStation(fStationNum);  
	   aStPt.Add(itClX, xValCorr, errX);  
	   aStPt.Add(itClUorV, uorvValCorr, uorvDat.second);  
	   if (fDebugIsOn) std::cerr << " Adding " << std::endl << aStPt << std::endl;; 
	   fStPoints.push_back(aStPt);
	   fIdStPtNow++;
	 } // on U or V view
       } // on X view 
       return fStPoints.size() - nStart;
     } // recoXUorV
     //
     size_t SSDRecStationDwnstrAlgo1::recoYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       size_t kuy = 0;
       size_t nStart = fStPoints.size();
       const size_t kSt = static_cast<size_t>(fStationNum);
       if (fDebugIsOn) std::cerr << " Starting SSDRecStationDwnstrAlgo1::recoYUorW, Station " << fStationNum 
                        << "  with a total of " << aSSDClsPtr->size() << " clusters " << std::endl;
       for(std::vector<rb::SSDCluster>::const_iterator itClY = aSSDClsPtr->cbegin(); itClY != aSSDClsPtr->cend(); itClY++, kuy++) {
         if (fClUsages[kuy] != 0) continue;
         if (itClY->Station() != fStationNum) continue;
	 if (itClY->View() != emph::geo::Y_VIEW) continue;
	 size_t kSeY = static_cast<size_t>(itClY->Sensor());
	 const std::pair<double, double> yDat = fCoordConvert.getTrCoord(itClY, fPrelimMomentum);
         size_t kuu = 0;
	 const double angleRollY = fEmVolAlP->Roll(emph::geo::Y_VIEW, kSt, kSeY);
	 const double angleRollCenterY = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, kSt, kSeY);
	 if (fDebugIsOn) {
	     std::cerr << " At cluster on Y view, station " << itClY->Station() << " Sensor  " 
	             << kSeY << " weighted strip " << itClY->WgtAvgStrip() << " RMS " << itClY->WgtRmsStrip() << std::endl;
	 }
	   if (std::abs(yDat.first - fYWindowCenter) > fYWindowWidth) {
	     if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected Y location  " 
	                              << fYWindowCenter << " delta " << yDat.first - fYWindowCenter << std::endl;
	     continue;
	   }
         for(std::vector<rb::SSDCluster>::const_iterator itClUorV = aSSDClsPtr->cbegin(); itClUorV != aSSDClsPtr->cend(); itClUorV++, kuu++) {
           if (fClUsages[kuu] != 0) continue;
           if (itClUorV->Station() != fStationNum) continue;
	   if ((kSt < 4) && (itClUorV->View() != emph::geo::W_VIEW)) continue;
	   if ((kSt > 3) && (itClUorV->View() != emph::geo::U_VIEW)) continue;
	   size_t kSeUorV = static_cast<size_t>(itClUorV->Sensor());
	   if (fDebugIsOn) {
	       std::cerr << " At cluster on UorV view, station " << itClUorV->Station() << " Sensor  " 
	             << kSeUorV << " weighted strip " << itClUorV->WgtAvgStrip() << " RMS " << itClUorV->WgtRmsStrip() << std::endl;
	   }
	   const std::pair<double, double> uorvDat = fCoordConvert.getTrCoord(itClUorV, fPrelimMomentum);
	   const double angleRollUorV = (kSt > 3) ? fEmVolAlP->Roll(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                        fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);  
	   const double angleRollCenterUorV = (kSt <4) ? fEmVolAlP->RollCenter(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                              fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);
	   const double   xPred = (kSt > 3) ?  (fSqrt2 * uorvDat.first - yDat.first) : (fSqrt2 * uorvDat.first + yDat.first);
	   const double  uPred = fOneOverSqrt2 * ( yDat.first + xPred);
	   const double vPred = -1.0*fOneOverSqrt2 * ( yDat.first - xPred);
	   const double uorvValCorr = (kSt > 3) ? uPred + ( vPred - angleRollCenterUorV) * angleRollUorV :  
	                                          vPred + ( uPred  - angleRollCenterUorV) * angleRollUorV ;
	   const double yValCorr = yDat.first + (xPred - angleRollCenterY) * angleRollY; 
	   const double  xValCorr = (kSt > 3) ?  (fSqrt2 * uorvValCorr - yDat.first) : (fSqrt2 * uorvValCorr + yDat.first);
           fClUsages[kuy] = 1; fClUsages[kuu] = 1;
	   // constraints, store.. 
	   rb:: SSDStationPtAlgo1 aStPt;
	   const double errY = std::sqrt(std::abs(yDat.second));  
	   const double errX = fSqrt2*(std::sqrt(std::abs(uorvDat.second) + yDat.second)); 
	   aStPt.SetID(fIdStPtNow);
	   aStPt.SetX(xValCorr, errX);  // we could do a fit.. Note the 3D Downstream track will use measurement data. 
	   aStPt.SetY(yValCorr, errY); 
	   aStPt.SetStation(fStationNum);  
	   aStPt.Add(itClY, yValCorr, errX);  
	   aStPt.Add(itClUorV, uorvValCorr, uorvDat.second);  
	   if (fDebugIsOn) std::cerr << " Adding " << std::endl << aStPt << std::endl;; 
	   fStPoints.push_back(aStPt);
	   fIdStPtNow++;
	 } // on U or V view
       } // on X view 
       return fStPoints.size() - nStart;
    } // recoYUorV
    //
     void ssdr::SSDRecStationDwnstrAlgo1::openOutputCsvFiles() const {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameStrStr;
       fNameStrStr << "SSDRecStationDwnstrAlgo1_Run_" << fRunNum << "_" << fTokenJob << "_Station" << fStationNum << "_V1.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutSt = new std::ofstream(fNameStr.c_str());
       (*fFOutSt) << " spill evt nPts iPt type sIdX sIdY sIdUorV x xErr y yErr uvPred uvObsRaw uvobsCorr chiSq " << std::endl;
       //
       if (fDoFirstAndLastStrips) {
       
         std::ostringstream fName2StrStr;
         fName2StrStr << "SSDRecStationDwnstrAlgo1Y_FirstStrip_Run_" << fRunNum << "_" << fTokenJob << "_Station" << fStationNum << "_V1.txt";
         std::string fName2Str(fName2StrStr.str());
         fFOutStYFirst = new std::ofstream(fName2Str.c_str());
           (*fFOutStYFirst) << " spill evt nPts iPt type x xErr y yErr chiSq yStripAv yStripRMS " << std::endl;
       
         std::ostringstream fName3StrStr;
         fName3StrStr << "SSDRecStationDwnstrAlgo1Y_LastStrip_Run_" << fRunNum << "_" << fTokenJob << "_Station" << fStationNum << "_V1.txt";
         std::string fName3Str(fName3StrStr.str());
         fFOutStYLast = new std::ofstream(fName3Str.c_str());
           (*fFOutStYLast) << " spill evt nPts iPt type x xErr y yErr chiSq yStripAv yStripRMS " << std::endl;
       
         
         std::ostringstream fName4StrStr;
         fName4StrStr << "SSDRecStationDwnstrAlgo1X_FirstStrip_Run_" << fRunNum << "_" << fTokenJob << "_Station" << fStationNum << "_V1.txt";
         std::string fName4Str(fName4StrStr.str());
         fFOutStXFirst = new std::ofstream(fName4Str.c_str());
           (*fFOutStXFirst) << " spill evt nPts iPt type x xErr y yErr chiSq yStripAv xStripRMS " << std::endl;
       
         std::ostringstream fName5StrStr;
         fName5StrStr << "SSDRecStationDwnstrAlgo1X_LastStrip_Run_" << fRunNum << "_" << fTokenJob << "_Station" << fStationNum << "_V1.txt";
         std::string fName5Str(fName5StrStr.str());
         fFOutStXLast = new std::ofstream(fName5Str.c_str());
           (*fFOutStXLast) << " spill evt nPts iPt type x xErr y yErr chiSq yStripAv xStripRMS " << std::endl;
       
       
       }
       
     }
     void ssdr::SSDRecStationDwnstrAlgo1::dumpInfoForR() const {
       if ((fSubRunNum == 10) && (fEvtNum < 20)) 
             std::cerr << " ssdr::SSDRecStationDwnstrAlgo1::dumpInfoForR, Station " << fStationNum << " spill 10, evt " << fEvtNum << std::endl;
       if (fStPoints.size() == 0) return;
       if (fFOutSt == nullptr)  this->openOutputCsvFiles(); 
       std::ostringstream headerStrStr; 
       headerStrStr << " " << fSubRunNum << " " << fEvtNum << " " << fStPoints.size(); 
       std::string headerStr(headerStrStr.str());
       size_t k=0;
       for (std::vector<rb::SSDStationPtAlgo1>::const_iterator it = fStPoints.cbegin(); it != fStPoints.cend(); it++, k++) { 
       	 size_t numClInSpSt = it->NumClusters();
	 int sIdX=9999;  int sIdY=9999;  int sIdUorV=9999;
	 for (size_t kCl=0; kCl != numClInSpSt; kCl++) {
	    emph::geo::sensorView aView =  it->ClusterView(kCl);
	    int aSensor = it->ClusterSensorId(kCl);
	    switch (aView) {
	        case emph::geo::X_VIEW : {sIdX = aSensor; break; }
	        case emph::geo::Y_VIEW : {sIdY = aSensor; break; }
	        case emph::geo::U_VIEW : {sIdUorV = aSensor; break; }
	        case emph::geo::W_VIEW : {sIdUorV = aSensor; break; }
		default : { 
		  std::cerr << " ssdr::SSDRecStationDwnstrAlgo1::dumpInfoForR, unrecognized SSD Cluster.. keep going.. " << std::endl;
		}
	   }
 	 } // on Clusters, here, also views..  	 
         (*fFOutSt) << headerStr << " " << k << " " << it->Type()  << " " << sIdX << " " << sIdY << " " << sIdUorV
	         << " " << it->X() << " " << it->XErr() << " " << it->Y() << " " << it->YErr() 
		 << " " << it->UorWPred() << " "  << it->UorWObsRaw() << " " << it->UorWObsCorr() << " " << it->ChiSq() << std::endl; 
		 
       } // On Space Points
     }
     //
     
     void ssdr::SSDRecStationDwnstrAlgo1::dumpInfoForRYViewEdges(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const {
       if (!fDoFirstAndLastStrips) return;
       if (fStPoints.size() == 0) return;
       if (fFOutSt == nullptr)  this->openOutputCsvFiles(); 
       std::ostringstream headerStrStr; 
       headerStrStr << " " << fSubRunNum << " " << fEvtNum << " " << fStPoints.size(); 
       std::string headerStr(headerStrStr.str());
       size_t k=0;
       for (std::vector<rb::SSDStationPtAlgo1>::const_iterator it = fStPoints.cbegin(); it != fStPoints.cend(); it++, k++) { 
       	 size_t numClInSpSt = it->NumClusters();
	 if ((it->Type() != 4) && (it->Type() != 7)) continue; 
	 for (size_t kCl=0; kCl != numClInSpSt; kCl++) {
	    emph::geo::sensorView aView =  it->ClusterView(kCl);
	    if (aView != emph::geo::Y_VIEW) continue;
	    int aSIdY = it->ClusterSensorId(kCl);
	    int aClID = it->ClusterID(kCl);
	    // go back and search for the corresponding cluster.. 
            for(std::vector<rb::SSDCluster>::const_iterator itClY = aSSDClsPtr->cbegin(); itClY != aSSDClsPtr->cend(); itClY++) {
              if (itClY->Station() != fStationNum) continue;
	      if (itClY->View() != emph::geo::Y_VIEW) continue;
	      if (itClY->Sensor() != aSIdY) continue;
	      if (itClY->ID() != aClID) continue;
	      if (itClY->MinStrip() == 0) {
                (*fFOutStYFirst) << headerStr << " " << k << " " << it->Type() 
	         << " " << it->X() << " " << it->XErr() << " " << it->Y() << " " << it->YErr() << " " 
		 << it->ChiSq() << " " << itClY->WgtAvgStrip() << " " << itClY->WgtRmsStrip() << std::endl; 
	      }
	      if (itClY->MaxStrip() == 638) {
                (*fFOutStYLast) << headerStr << " " << k << " " << it->Type() 
	         << " " << it->X() << " " << it->XErr() << " " << it->Y() << " " << it->YErr() << " " 
		 << it->ChiSq() << " " << itClY->WgtAvgStrip() << " " << itClY->WgtRmsStrip() << " " << std::endl; 
	      }
	   }       
        } // 
       } // On Space Point 
     } // dumpInfoForRYViewEdges
     void ssdr::SSDRecStationDwnstrAlgo1::dumpInfoForRXViewEdges(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const {
       if (!fDoFirstAndLastStrips) return; 
       if (fStPoints.size() == 0) return;
       if (fFOutSt == nullptr)  this->openOutputCsvFiles(); 
       std::ostringstream headerStrStr; 
       headerStrStr << " " << fSubRunNum << " " << fEvtNum << " " << fStPoints.size(); 
       std::string headerStr(headerStrStr.str());
       size_t k=0;
       for (std::vector<rb::SSDStationPtAlgo1>::const_iterator it = fStPoints.cbegin(); it != fStPoints.cend(); it++, k++) { 
       	 size_t numClInSpSt = it->NumClusters();
	 if ((it->Type() != 4) && (it->Type() != 7)) continue; 
	 for (size_t kCl=0; kCl != numClInSpSt; kCl++) {
	    emph::geo::sensorView aView =  it->ClusterView(kCl);
	    if (aView != emph::geo::X_VIEW) continue;
	    int aSIdX = it->ClusterSensorId(kCl);
	    int aClID = it->ClusterID(kCl);
	    // go back and search for the corresponding cluster.. 
            for(std::vector<rb::SSDCluster>::const_iterator itClX = aSSDClsPtr->cbegin(); itClX != aSSDClsPtr->cend(); itClX++) {
              if (itClX->Station() != fStationNum) continue;
	      if (itClX->View() != emph::geo::X_VIEW) continue;
	      if (itClX->Sensor() != aSIdX) continue;
	      if (itClX->ID() != aClID) continue;
	      if (itClX->MinStrip() == 0) {
                (*fFOutStXFirst) << headerStr << " " << k << " " << it->Type() 
	         << " " << it->X() << " " << it->XErr() << " " << it->Y() << " " << it->YErr() << " " 
		 << it->ChiSq() << " " << itClX->WgtAvgStrip() << " " << itClX->WgtRmsStrip() << std::endl; 
	      }
	      if (itClX->MaxStrip() == 638) {
                (*fFOutStXLast) << headerStr << " " << k << " " << it->Type() 
	         << " " << it->X() << " " << it->XErr() << " " << it->Y() << " " << it->YErr() << " " 
		 << it->ChiSq() << " " << itClX->WgtAvgStrip() << " " << itClX->WgtRmsStrip() << " " << std::endl; 
	      }
	   }       
        } // 
       } // On Space Point 
     } // dumpInfoForRXViewEdges
  } // namespace ssdr
} //  namespace emph 
