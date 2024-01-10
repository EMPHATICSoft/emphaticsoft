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
      fGeoService(art::ServiceHandle<emph::geo::GeometryService>()), fEmgeo(nullptr),        
      fDetGeoMapService(art::ServiceHandle<emph::dgmap::DetGeoMapService>()), fDetGeoMap(fDetGeoMapService->Map()),        
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), 
      fCoordConvert('A'),
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0), fStationNum(static_cast<int>(kSt)), 
      fNEvents(0), fDebugIsOn(false),
      fIsMC(false), // Ugly, we are still working on the sign convention and rotation angles signs.. 
      fDoFirstAndLastStrips(false),
      fInterchangeStXandWStation6(false), fInterchangeStYandWStation6(false), 
      fChiSqCut(5.0), fChiSqCutPreArb(DBL_MAX),// for XYU (or XYW) cut. 
      fXWindowWidth(40.), fYWindowWidth(40.0), fXWindowCenter(0.), fYWindowCenter(0.),
      fPrelimMomentum(5.0),
      fTokenJob("undef"), fStPoints(), fFOutSt(nullptr), fFOutStYFirst(nullptr), fFOutStYLast(nullptr), 
      fClUsages(), fNxCls(0), fNyCls(0), fNuCls(0), fInViewOverlapCut(30.) {
      // Overlap cut A bit oversized.. Need to study this a bit later, for multiple track or noise...
	 if ((fStationNum < 2) || (fStationNum > 6)) {
	      std::cerr << " SSDRecStationDwnstrAlgo1 Station number, value " << fStationNum 
	                << " is wrong for phase1b or phase1c data, quit here and now " << std::endl;
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
       }
       fCoordConvert.SetDebugOn(fDebugIsOn);
       if (fEmgeo == nullptr) 	fEmgeo = fGeoService->Geo();		 
	// This should be part of the Geometry package.. 
	if (!fCoordConvert.IsReadyToGo()) {
	 const size_t kStMax = (fRunNum < 2000) ? 6 : 8;
	 // We use the nominal Z position, for now.. 
	 std::vector<double> zPosStations;
	 std::cerr << " SSDRecStationDwnstrAlgo1::RecIt, Station " << fStationNum << " Storing Z Positions ..." << std::endl; 
	 for (size_t kSt=0; kSt != kStMax; kSt++) {
	   TVector3 tmpPos = fEmgeo->GetSSDStation(kSt)->Pos(); 
	   double zz = tmpPos[2];
	   zPosStations.push_back(zz); 
	   if ((fRunNum < 2000) && (kSt > 3)) zPosStations.push_back(zz); // double sensors for Phase1b 
	   if ((fRunNum > 1999) && (kSt > 4)) zPosStations.push_back(zz); // double sensors for Phase1c 
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
       if ((fNxCls2nd == 0) && (fNyCls2nd == 0)) {this->arbitrateOverlapping (fInViewOverlapCut); return this->Size(); }
       if ((fNxCls2nd == 0) && (fNuCls2nd == 0)) {this->arbitrateOverlapping (fInViewOverlapCut); return this->Size(); }
       if ((fNyCls2nd == 0) && (fNuCls2nd == 0)) {this->arbitrateOverlapping (fInViewOverlapCut); return this->Size(); }
       size_t nAdded = 0; 
       if ((fNxCls2nd != 0) && (fNyCls2nd != 0)) nAdded = this->recoXY(aSSDClsPtr);
       if ((fNxCls2nd != 0) && (fNuCls2nd != 0)) nAdded = this->recoXUorW(aSSDClsPtr);
       if ((fNyCls2nd != 0) && (fNuCls2nd != 0)) nAdded = this->recoYUorW(aSSDClsPtr);
       if (fDebugIsOn) {
//          std::cerr << " .... Adding  " << nAdded << " Tentative 3D points " << " ... And quit right here and now " << std::endl;
//	  exit(2);
          std::cerr << " .... Adding  " << nAdded << " Tentative 3D points " << " ... And keep going " << std::endl;
       }
       this->arbitrateOverlapping (fInViewOverlapCut);  
       return this->Size(); 
     }
     bool SSDRecStationDwnstrAlgo1::HasUniqueYW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) const {
       int nW=0; int nY=0;
       for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
         if (itCl->Station() != fStationNum) continue;
	 if (itCl->View() == emph::geo::Y_VIEW) nY++;
	 if (itCl->View() == emph::geo::W_VIEW) nW++;
       }
       if ((nY != 1) || (nW != 1)) return false;
       return true;
     }
     //
     // Now the details..
     //
     size_t SSDRecStationDwnstrAlgo1::recoXYUorW(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr){
       size_t kux = 0;
       std::vector<int> fClUsagesDbl(fClUsages); 
       size_t nStart = fStPoints.size();
       if (fDebugIsOn) {
          std::cerr << " Starting SSDRecStationDwnstrAlgo1::recoXYUorW, Station " << fStationNum 
                        << "  with a total of " << aSSDClsPtr->size() << " clusters " << std::endl;
       }	  
       fCoordConvert.SetDebugOn(fDebugIsOn);
       const size_t kSt = static_cast<size_t>(fStationNum);
       bool hasXOverlaps = false; bool hasYOverlaps = false; bool hasWOverlaps = false;
       if (fStationNum > 4) {
          if (fNxCls > 1) { 
            for(std::vector<rb::SSDCluster>::const_iterator itClX1 = aSSDClsPtr->cbegin(); itClX1 != aSSDClsPtr->cend(); itClX1++) {
              if (itClX1->Station() != fStationNum) continue;
	      if (itClX1->View() != emph::geo::X_VIEW) continue;
	      std::vector<rb::SSDCluster>::const_iterator itClX2Start = itClX1;  itClX2Start++; 
              for(std::vector<rb::SSDCluster>::const_iterator itClX2 = itClX2Start; itClX2 != aSSDClsPtr->cend(); itClX2++) {
                if (itClX2->Station() != fStationNum) continue;
	        if (itClX2->View() != emph::geo::X_VIEW) continue;
	        if (this->hasOverlappingClusters(itClX1, itClX2, fInViewOverlapCut)) {
		   if (fDebugIsOn) std::cerr <<  " ... Overlapping Clusters on X View "  << (*itClX1) 
		                            <<  " with " << (*itClX2) << std::endl;
		   hasXOverlaps = true;
	           break;
	        }
	      }
	      if (hasXOverlaps) break; 
	    }  
          }
          if (fNyCls > 1) { 
            for(std::vector<rb::SSDCluster>::const_iterator itClY1 = aSSDClsPtr->cbegin(); itClY1 != aSSDClsPtr->cend(); itClY1++) {
              if (itClY1->Station() != fStationNum) continue;
	      if (itClY1->View() != emph::geo::Y_VIEW) continue;
	      std::vector<rb::SSDCluster>::const_iterator itClY2Start = itClY1;  itClY2Start++; 
              for(std::vector<rb::SSDCluster>::const_iterator itClY2 = itClY2Start; itClY2 != aSSDClsPtr->cend(); itClY2++) {
                if (itClY2->Station() != fStationNum) continue;
	        if (itClY2->View() != emph::geo::Y_VIEW) continue;
	        if (this->hasOverlappingClusters(itClY1, itClY2, fInViewOverlapCut)) {
		   if (fDebugIsOn) std::cerr <<  " ... Overlapping Clusters on Y View "  << (*itClY1) 
		                            <<  " with " << (*itClY2) << std::endl;
		   hasYOverlaps = true;
	           break;
	        }
	      }
	      if (hasYOverlaps) break; 
	    }  
          }
          if (fNuCls > 1) { 
            for(std::vector<rb::SSDCluster>::const_iterator itClW1 = aSSDClsPtr->cbegin(); itClW1 != aSSDClsPtr->cend(); itClW1++) {
              if (itClW1->Station() != fStationNum) continue;
	      if (itClW1->View() != emph::geo::W_VIEW) continue;
	      std::vector<rb::SSDCluster>::const_iterator itClW2Start = itClW1;  itClW2Start++; 
              for(std::vector<rb::SSDCluster>::const_iterator itClW2 = itClW2Start; itClW2 != aSSDClsPtr->cend(); itClW2++) {
                if (itClW2->Station() != fStationNum) continue;
	        if (itClW2->View() != emph::geo::W_VIEW) continue;
	        if (this->hasOverlappingClusters(itClW1, itClW2, fInViewOverlapCut)) {
		   if (fDebugIsOn) std::cerr <<  " ... Overlapping Clusters on W View "  << (*itClW1) 
		                            <<  " with " << (*itClW2) << std::endl;
		   hasWOverlaps = true;
	           break;
	        }
	      }
	      if (hasWOverlaps) break; 
	    }  
          }
       } // Seeting the overlaps bits.. 
       for(std::vector<rb::SSDCluster>::const_iterator itClX = aSSDClsPtr->cbegin(); itClX != aSSDClsPtr->cend(); itClX++, kux++) {
         if (fClUsages[kux] != 0) continue;
         if (itClX->Station() != fStationNum) continue;
	 if ((!fInterchangeStXandWStation6) || (fStationNum != 6)) { 
	   if (itClX->View() != emph::geo::X_VIEW) continue;
	 } else { 
	   if (itClX->View() != emph::geo::W_VIEW) continue;
	 }
	 size_t kSeX = static_cast<size_t>(itClX->Sensor());
	 if (fDebugIsOn) std::cerr << " ...X-View,  Before fCoordConvert.getTrCoord " << std::endl;
	 const std::pair<double, double> xDat = fCoordConvert.getTrCoord(itClX, fPrelimMomentum);
	 if (fDebugIsOn) std::cerr << " ...X-View,  After  fCoordConvert.getTrCoord " << std::endl;
	 const double xDatGeoMap =  (itClX->WgtAvgStrip() < 639.) ? fCoordConvert.getTrCoordRoot(itClX) : -9999.; // bug protect, DetGeomMap screws up is 
	 // if problem with estimate of average strip number is wrong.. 
	 if (std::abs(xDat.first - fXWindowCenter) > fXWindowWidth) {
	   if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected X location  " 
	                              << fXWindowCenter << " delta " << xDat.first - fXWindowCenter << std::endl;
	   continue;
	 }
	 if (fDebugIsOn) {
	   std::cerr << " At cluster on X view, station " << itClX->Station() << " Sensor  " 
	             << kSeX << " weighted strip " << itClX->WgtAvgStrip() 
		     << " RMS " << itClX->WgtRmsStrip() << " ClID " << itClX->ID() << " X (no Roll) " 
		     << xDat.first << " +- " << std::sqrt(xDat.second) << " check GeoMap " << xDatGeoMap << std::endl;
	 }
         size_t kuy = 0;
	 const double angleRollX = fEmVolAlP->Roll(emph::geo::X_VIEW, kSt, kSeX);
	 const double angleRollCenterX = fEmVolAlP->RollCenter(emph::geo::X_VIEW, kSt, kSeX);
	 if (fDebugIsOn) std::cerr << " .... xDat " << xDat.first << " +- " << std::sqrt(xDat.second) 
	                           <<  " angleRollX " << angleRollX << " center " << angleRollCenterX <<  std::endl; 
         for(std::vector<rb::SSDCluster>::const_iterator itClY = aSSDClsPtr->cbegin(); itClY != aSSDClsPtr->cend(); itClY++, kuy++) {
           if (fClUsages[kuy] != 0) continue;
           if (itClY->Station() != fStationNum) continue;
	   if ((!fInterchangeStYandWStation6) || (fStationNum != 6)) { 
	     if (itClY->View() != emph::geo::Y_VIEW) continue;
	   } else { 
	     if (itClY->View() != emph::geo::W_VIEW) continue;
	   }
	   size_t kSeY = static_cast<size_t>(itClY->Sensor());
	   const std::pair<double, double> yDat = fCoordConvert.getTrCoord(itClY, fPrelimMomentum);
	   const double yDatGeoMap =  (itClY->WgtAvgStrip() < 639.) ? fCoordConvert.getTrCoordRoot(itClX) : -9999.; // bug protect, DetGeomMap screws up is 
	 // if problem with estimate of average strip number is wrong.. 
	   if (std::abs(yDat.first - fYWindowCenter) > fYWindowWidth) {
	     if (fDebugIsOn) std::cerr << " Skip, out side the search window in Projected Y location  " 
	                              << fYWindowCenter << " delta " << yDat.first - fYWindowCenter << std::endl;
	     continue;
	   }
	   if (fDebugIsOn) {
	     std::cerr << " ... At cluster on Y view, station " << itClY->Station() << " Sensor  " 
	             << kSeY << " weighted strip " << itClY->WgtAvgStrip() << " RMS " 
		     << itClY->WgtRmsStrip() << " ClID " << itClY->ID() << " Y (no Roll) " 
		     << yDat.first << " +- " << std::sqrt(yDat.second) << " check GeoMap " << yDatGeoMap  << std::endl;
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
	   const double rollCorrX = (yDat.first - angleRollCenterX) * angleRollX;
	   const double xValCorr = xDat.first - rollCorrX; // sign change with respect to the fit.. we want the model value. 
	   const double rollCorrY = (xDat.first - angleRollCenterY) * angleRollY;
	   const double yValCorr = yDat.first - rollCorrY;
//	   std::cerr << " ... ... rollCorrX " << rollCorrX << " rollCorrY " << rollCorrY << " .. so, x now " << xValCorr << " y " << yValCorr << std::endl;
//	   const double xValCorr = xDat.first + (yDat.first - angleRollCenterX) * angleRollX; 
//	   const double yValCorr = yDat.first + (xDat.first - angleRollCenterY) * angleRollY;
//   Now presumably correct.  Sept 8 2023. token job Run_1274_NoTgt31Gev_ClSept_A1e_1o1, or _c7
	   const double uPred = fOneOverSqrt2 * ( xValCorr + yValCorr);
	   const double vPred = -1.0*fOneOverSqrt2 * ( -xValCorr + yValCorr);
//   Wrong, presumably, tokenjob Run_1274_NoTgt31Gev_ClSept_A1e_1o1_W1
//	   const double vPred = fOneOverSqrt2 * ( xValCorr + yValCorr);
//	   const double uPred = -1.0*fOneOverSqrt2 * ( -xValCorr + yValCorr);
//   Wrong, presumably, tokenjob Run_1274_NoTgt31Gev_ClSept_A1e_1o1_W2  Chenge the sign of 
//	   const double uPred = -1.0*fOneOverSqrt2 * ( xValCorr + yValCorr);
//	   const double vPred = fOneOverSqrt2 * ( -xValCorr + yValCorr);
//   Wrong, swap U and W, October 27, token job  
//	   const double vPred = fOneOverSqrt2 * ( xValCorr + yValCorr);
//	   const double uPred = -1.0*fOneOverSqrt2 * ( -xValCorr + yValCorr);
           size_t kuu = 0;
	   if (fDebugIsOn) std::cerr << " ... uPred " << uPred << " vPred " << vPred << std::endl; 
           for(std::vector<rb::SSDCluster>::const_iterator itClUorV = aSSDClsPtr->cbegin(); itClUorV != aSSDClsPtr->cend(); itClUorV++, kuu++) {
             if (fClUsages[kuu] != 0) continue;
             if (itClUorV->Station() != fStationNum) continue;
	     if (itClUorV->View() ==  emph::geo::INIT) continue;
	     if (((!fInterchangeStYandWStation6) && (!fInterchangeStYandWStation6)) || (fStationNum != 6)) { 
	       if ((itClUorV->View() ==  emph::geo::X_VIEW) ||(itClUorV->View() ==  emph::geo::Y_VIEW)) continue;
	     } else { 
	       if (fInterchangeStXandWStation6 && (itClUorV->View() != emph::geo::X_VIEW)) continue;
	       if (fInterchangeStYandWStation6 && (itClUorV->View() != emph::geo::Y_VIEW)) continue;
	     }
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
	     const double uorvDatGeoMap =  (itClUorV->WgtAvgStrip() < 639.) ? fCoordConvert.getTrCoordRoot(itClX) : -9999.; // bug protect, DetGeomMap screws up is 
	     const double angleRollUorV = fEmVolAlP->Roll(itClUorV->View(), kSt, kSeU);  
	     const double angleRollCenterUorV = fEmVolAlP->RollCenter(itClUorV->View(), kSt, kSeU); 
	     double uorvPred = (kSt > 3) ? vPred + ( uPred - angleRollCenterUorV) * angleRollUorV :  
	                                         uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV ; // December 6, but wrong for station 6.. 
	     if (fRunNum > 1999)  {  // Very, very messy
	         uorvPred =  (uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV) ; // All stereo angle are U type... 
		 // Note the sign for the correction... 
//		 std::cerr << " .. Roll correction for U " << (vPred  - angleRollCenterUorV) * angleRollUorV << std::endl;
// 
//    December 2023 
//	         uorvPred = -1.0 * (uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV) ; // Partial improvement... 
//	         if ((xValCorr > 0. ) && (yValCorr < 0.) && (uorvPred < 0.)) uorvPred *= -1.0; // big diff.. Not it. 
//	         if ((xValCorr < 0. ) && (yValCorr > 0.) && (uorvPred < 0.)) uorvPred *= -1.0;
//	         if ((xValCorr > 0. ) && (yValCorr < 0.) && (uorvPred < 0.)) uorvPred *= vPred + ( uPred - angleRollCenterUorV) * angleRollUorV ; // big diff.. Not it. 
//	         if ((xValCorr < 0. ) && (yValCorr > 0.) && (uorvPred < 0.)) uorvPred *= vPred + ( uPred - angleRollCenterUorV) * angleRollUorV ;
	     }				 
	     const double deltaXYU = uorvPred - uorvDat.first;
	     if (fDebugIsOn) std::cerr << " .... After correction for Rolls, uorvPred " << uorvPred  
	                               << " Data " << uorvDat.first << " Diff " << deltaXYU << " check geodata " << uorvDatGeoMap << std::endl;
	     const double uPredErrSq = 0.5 * (xDat.second + yDat.second);
	     // Blow-up the erro for the V view, given the lack of accuracy in the alignment Assume 1 mm to 1 nanometer, to test.  
	     const double deltaErrSq = (itClUorV->View() == emph::geo::W_VIEW) ? (1.0e-9 +  uPredErrSq + uorvDat.second) : 
	                                                                          (uPredErrSq + uorvDat.second);
	     const double aChiSq = (deltaXYU * deltaXYU)/deltaErrSq;
	     if (fDebugIsOn) std::cerr << " .... uDat " << uorvDat.first << " +- " << std::sqrt(uorvDat.second) 
	                             <<  " deltaXYU  " << deltaXYU << " +-  " << std::sqrt(deltaErrSq) 
				     << " chiSq " << aChiSq << " chiSq cut " << fChiSqCut << std::endl;
//	     if (fDebugIsOn && (fStationNum == 5)) { std::cerr << "  check chiSqCut, and there it is " << std::endl; exit(2); }		      
	     if (aChiSq > fChiSqCut) continue;
	     // tag and store 
	     if (!(hasXOverlaps || hasYOverlaps))  fClUsages[kuu] = 1; 
	     if (!(hasWOverlaps || hasYOverlaps))  fClUsages[kux] = 1; 
	     if (!(hasWOverlaps || hasXOverlaps))  fClUsages[kuy] = 1; 
	     if (hasXOverlaps) { fClUsagesDbl[kuu] = 1;  fClUsagesDbl[kuy] = 1; }
	     if (hasYOverlaps) { fClUsagesDbl[kuu] = 1;  fClUsagesDbl[kux] = 1; }
	     if (hasWOverlaps) { fClUsagesDbl[kux] = 1;  fClUsagesDbl[kuy] = 1; }
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
	     rb::LineSegment lineUV; fDetGeoMap->SSDClusterToLineSegment(*itClUorV, lineUV);
	     aStPt.SetLineUorV(lineUV);
	     fStPoints.push_back(aStPt);
	   } // on confirming U or W view   
	 } // onY view
       } // on X view
        
       if (hasXOverlaps || hasYOverlaps || hasWOverlaps) {
         for (size_t k=0; k != fClUsages.size(); k++) fClUsages[k] += fClUsagesDbl[k];
       }
       if (fDebugIsOn) {
         std::cerr << " ... .... Usage flag info " << std::endl;
	 size_t kut= 0; 
         for(std::vector<rb::SSDCluster>::const_iterator itClt = aSSDClsPtr->cbegin(); itClt != aSSDClsPtr->cend(); itClt++, kut++) {
	   if (itClt->Station() != fStationNum) continue;
	   std::cerr << " View " << itClt->View() << " Sensor " << itClt->Sensor() 
	                         << " fClUsage " << fClUsages[kut] << " fClUsagesDbl " << fClUsagesDbl[kut]  << std::endl; 
	 }
       } 
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
	             << kSeX << " weighted strip " << itClX->WgtAvgStrip() 
		     << " RMS " << itClX->WgtRmsStrip() << " xDat " << xDat.first <<  std::endl;
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
	   const double xValCorr = xDat.first - (yDat.first - angleRollCenterX) * angleRollX; 
	   const double yValCorr = yDat.first - (xDat.first - angleRollCenterY) * angleRollY;
	   if (fDebugIsOn) {
	     std::cerr << " ...  At cluster on Y view, station " << itClY->Station() << " Sensor  " 
	             << kSeY << " weighted strip " 
		     << itClY->WgtAvgStrip() << " RMS " << itClY->WgtRmsStrip() << " ydat " << yDat.first  << std::endl;
	     std::cerr << " ... angleRollX " <<  angleRollX  << " angleRollY " <<  angleRollY 
	               << "  xValCorr " <<  xValCorr << " yValCorr " << yValCorr << std::endl;
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
       const size_t kStMaxW = (fRunNum < 2000) ? 4 : 5;
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
	   if ((kSt < 4) && (itClUorV->View() != emph::geo::U_VIEW)) continue;
	   if ((kSt > 3) && (itClUorV->View() != emph::geo::W_VIEW)) continue;
	   size_t kSeUorV = static_cast<size_t>(itClUorV->Sensor());
	   const std::pair<double, double> uorvDat = fCoordConvert.getTrCoord(itClUorV, fPrelimMomentum);
	   const double angleRollUorV = (kSt < kStMaxW) ? fEmVolAlP->Roll(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                        fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);  
	   const double angleRollCenterUorV = (kSt < kStMaxW) ? fEmVolAlP->RollCenter(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                              fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);
	   double yPred, uPred, vPred;
	   if (fDebugIsOn) {
	       std::cerr << " At cluster on UorV view, station " << itClUorV->Station() << " Sensor  " 
	             << kSeUorV << " weighted strip " << itClUorV->WgtAvgStrip() << " RMS " << itClUorV->WgtRmsStrip() << std::endl;
	   }
	   // To be checked, depends on Delta Roll definition Sept 5 2023. 
	   yPred = (kSt > (kStMaxW-1)) ?  (-fSqrt2 * uorvDat.first + xDat.first) : (fSqrt2 * uorvDat.first - xDat.first);
	   uPred = fOneOverSqrt2 * ( xDat.first + yPred);
	    vPred = -1.0*fOneOverSqrt2 * ( -xDat.first + yPred);
	   double uorvValCorr = (kSt > (kStMaxW-1)) ? vPred + ( uPred - angleRollCenterUorV) * angleRollUorV :  
	                                          uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV ;
	   if (fRunNum > 1999) 	uorvValCorr = uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV ;				  
	   const double xValCorr = xDat.first + (yPred - angleRollCenterX) * angleRollX; 
	   double yValCorr = (kSt > 3) ?  (-fSqrt2 * uorvValCorr + xDat.first) : (fSqrt2 * uorvValCorr - xDat.first);
	   // December found the hard way that all stereo-angle views are U type, by reaching semi decent alignment results. 
	   if (fRunNum > 1999) yValCorr = (fSqrt2 * uorvValCorr - xDat.first); 
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
       const size_t kStMaxW = (fRunNum < 2000)  ? 4 : 5;
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
	   if ((kSt < 4) && (itClUorV->View() != emph::geo::U_VIEW)) continue;
	   if ((kSt > 3) && (itClUorV->View() != emph::geo::W_VIEW)) continue;
	   size_t kSeUorV = static_cast<size_t>(itClUorV->Sensor());
	   if (fDebugIsOn) {
	       std::cerr << " At cluster on UorV view, station " << itClUorV->Station() << " Sensor  " 
	             << kSeUorV << " weighted strip " << itClUorV->WgtAvgStrip() << " RMS " << itClUorV->WgtRmsStrip() << std::endl;
	   }
	   const std::pair<double, double> uorvDat = fCoordConvert.getTrCoord(itClUorV, fPrelimMomentum);
	   const double angleRollUorV = (kSt < kStMaxW) ? fEmVolAlP->Roll(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                        fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);  
	   const double angleRollCenterUorV = (kSt < kStMaxW) ? fEmVolAlP->RollCenter(emph::geo::U_VIEW, kSt, kSeUorV) : 
	                                              fEmVolAlP->Roll(emph::geo::W_VIEW, kSt, kSeUorV);
	   const double   xPred = (kSt > (kStMaxW-1)) ?  (fSqrt2 * uorvDat.first + yDat.first) : (fSqrt2 * uorvDat.first - yDat.first);
	   const double  uPred = fOneOverSqrt2 * ( yDat.first + xPred);
	   const double vPred = -1.0*fOneOverSqrt2 * ( yDat.first - xPred);
	   double uorvValCorr = (kSt > (kStMaxW-1)) ? vPred + ( uPred - angleRollCenterUorV) * angleRollUorV :  
	                                          uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV ;
	   if (fRunNum > 1999) 	uorvValCorr = uPred + ( vPred  - angleRollCenterUorV) * angleRollUorV ;				  
	   const double yValCorr = yDat.first + (xPred - angleRollCenterY) * angleRollY; 
	   double  xValCorr = (kSt > 3) ?  (fSqrt2 * uorvValCorr + yDat.first) : (fSqrt2 * uorvValCorr - yDat.first);
	   if (fRunNum > 1999) xValCorr = (fSqrt2 * uorvValCorr - yDat.first);
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
       (*fFOutSt) << " spill evt nPts iPt type sIdX sIdY sIdUorV x xErr y yErr uvPred uvObsRaw uvobsCorr chiSq" << 
                     " xUVStrip0 xUVStrip1 yUVStrip0 yUVStrip1 " << std::endl;
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
		 << " " << it->UorWPred() << " "  << it->UorWObsRaw() << " " << it->UorWObsCorr() << " " << it->ChiSq();
         rb::LineSegment ls = it->LineStripUorV();
         (*fFOutSt) << " " << ls.X0()[0] << " " << ls.X1()[0] << " " << ls.X0()[1] << " " << ls.X1()[1] << std::endl; 
		 
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
     
     bool ssdr::SSDRecStationDwnstrAlgo1::flagDuplicateOverlapping (
                                        emph::geo::sensorView aView, 
                                        std::vector<rb::SSDStationPtAlgo1>::iterator itPt1, 
	                                std::vector<rb::SSDStationPtAlgo1>::iterator itPt2,
	                                  double inViewCut){ // argument is the difference in average strip numbers. 
	std::vector<myItCl> itCls1; std::vector<myItCl> itCls2; 
	itPt1->fillItClusters(itCls1); 	itPt2->fillItClusters(itCls2);
	for (size_t k1=0; k1 != itCls1.size(); k1++) {
	  if (itCls1[k1]->Station() !=  fStationNum) continue; // Should not happen.. 
	  if (itCls1[k1]->View() != aView) continue; 
	  for (size_t k2=0; k2 != itCls2.size(); k2++) {
	    if (itCls2[k2]->Station() !=  fStationNum) continue; // Should not happen.. 
	    if (itCls2[k2]->View() != aView) continue;
	    if (itCls2[k2]->Sensor() == itCls1[k1]->Sensor()) continue;  // Could be close by on the same sensor.. 
	    if (std::abs(itCls1[k1]->WgtAvgStrip() - itCls2[k2]->WgtAvgStrip()) < inViewCut) return true; 
	  }
	}
	return false; 			  
     } 
     bool ssdr::SSDRecStationDwnstrAlgo1::hasOverlappingClusters (
                                        std::vector<rb::SSDCluster>::const_iterator itCl1, 
	                                std::vector<rb::SSDCluster>::const_iterator itCl2,
	                                  double inViewCut){ // argument is the difference in average strip numbers. 
					  
	if ((itCl1->Sensor() != itCl2->Sensor()) && 
	    (std::abs(itCl1->WgtAvgStrip() - itCl2->WgtAvgStrip()) < inViewCut)) return true; 
	return false; 			  
     } 
     bool ssdr::SSDRecStationDwnstrAlgo1::arbitrateOverlapping (double inViewCut) {
       if (fDebugIsOn) {
         std::cerr << " SSDRecStationDwnstrAlgo1::arbitrateOverlapping, on " << fStPoints.size() << " space points, cut " << inViewCut << std::endl;
       }
       if (fStPoints.size() < 2) return false;
       bool hasChanged = false;
       size_t numClCurrent = 0; 
       while (numClCurrent != fStPoints.size()) {
         numClCurrent =  fStPoints.size();      
         for (std::vector<rb::SSDStationPtAlgo1>::iterator itPt1 = fStPoints.begin(); itPt1 != fStPoints.end(); itPt1++) { 
	   std::vector<rb::SSDStationPtAlgo1>::iterator itPt2Start = itPt1;  itPt2Start++;
	   bool didInnerChanged= false; 
	   for (std::vector<rb::SSDStationPtAlgo1>::iterator itPt2 = itPt2Start; itPt2 != fStPoints.end(); itPt2++) {
	     // 
	     // Check each view.. A bit of a waste in transfering pointers in flagDuplicateOverlapping, but a bit cleaner (may be) 
	     //
	     if (this->flagDuplicateOverlapping(emph::geo::X_VIEW, itPt1, itPt2, inViewCut)) {
	       didInnerChanged = true;
	       const double x1 = itPt1->X(); const double x1Err = itPt1->XErr(); const double w1 = 1.0/(x1Err*x1Err); 
	       const double x2 = itPt2->X(); const double x2Err = itPt2->XErr(); const double w2 = 1.0/(x2Err*x2Err); 
	       const double sumW = w1 + w2; 
	       const double xx = (x1*w1 + x2*w2)/sumW;
//	       const double xxErr = std::sqrt(1.0/sumW);  IIn doing so, we artificially, and wrongly, subtract multiple scattering error. 
	       const double xxErr = std::min(x1Err, x2Err);  
	       itPt1->SetX(xx, xxErr); 
	       std::vector<myItCl> itCls2; itPt2->fillItClusters(itCls2);
               for(size_t k2 = 0; k2 != itCls2.size(); k2++) {
	         if (itCls2[k2]->View() == emph::geo::X_VIEW) { 
		   itPt1->addOverlapClusters(itCls2[k2], itPt2->ClusterCorrMeasurement(0), itPt2->ClusterMeasurementError(0));
	           if (fDebugIsOn) {
	             std::cerr << " ....,  Removal of Station point X View " << std::endl;
	             std::cerr << " " << (*itCls2[k2]) << std::endl; 
	             std::cerr << " ....,  Keeping Improved  Space Point...  X View " << std::endl;
	             std::cerr << " " << (*itPt1) << std::endl; 
		     
	            }
		 }
	       }
	       fStPoints.erase(itPt2);
	       break; 
	     }   
	     if (this->flagDuplicateOverlapping(emph::geo::Y_VIEW, itPt1, itPt2, inViewCut)) {
	       didInnerChanged = true;
	       const double y1 = itPt1->Y(); const double y1Err = itPt1->YErr(); const double w1 = 1.0/(y1Err*y1Err); 
	       const double y2 = itPt2->Y(); const double y2Err = itPt2->YErr(); const double w2 = 1.0/(y2Err*y2Err); 
	       const double sumW = w1 + w2; 
	       const double yy = (y1*w1 + y2*w2)/sumW;
//	       const double yyErr = std::sqrt(1.0/sumW);
	       const double yyErr = std::min(y1Err, y2Err); 
	       itPt1->SetY(yy, yyErr); 
	       std::vector<myItCl> itCls2; itPt2->fillItClusters(itCls2);
               for(size_t k2 = 0; k2 != itCls2.size(); k2++) {
	         if (itCls2[k2]->View() == emph::geo::Y_VIEW) { 
		   itPt1->addOverlapClusters(itCls2[k2], itPt2->ClusterCorrMeasurement(1), itPt2->ClusterMeasurementError(1));
	           if (fDebugIsOn) {
	             std::cerr << " SSDRecStationDwnstrAlgo1::arbitrateOverlapping,  Removal of Station point Y View " << std::endl;
	             std::cerr << " " << (*itCls2[k2]) << std::endl; 
	            }
		 }
	       }
	       fStPoints.erase(itPt2);
	       break; 
	     }   
	     if (this->flagDuplicateOverlapping(emph::geo::W_VIEW, itPt1, itPt2, inViewCut)) {
	       didInnerChanged = true;
	       const double x1 = itPt1->X(); const double x1Err = itPt1->XErr(); const double wx1 = 1.0/(x1Err*x1Err); 
	       const double x2 = itPt2->X(); const double x2Err = itPt2->XErr(); const double wx2 = 1.0/(x2Err*x2Err); 
	       const double y1 = itPt1->Y(); const double y1Err = itPt1->YErr(); const double wy1 = 1.0/(y1Err*y1Err); 
	       const double y2 = itPt2->Y(); const double y2Err = itPt2->YErr(); const double wy2 = 1.0/(y2Err*y2Err); 
	       const double sumWx = wx1 + wx2; const double sumWy = wy1 + wy2; 
	       const double xx = (x1*wx1 + x2*wx2)/sumWx;
	       const double xxErr = std::min(x1Err, x2Err); // a bit more conservative.. 
	       const double yy = (y1*wy1 + y2*wy2)/sumWy;
	       const double yyErr = std::min(y1Err, y2Err);
	       itPt1->SetX(xx, xxErr); 
	       itPt1->SetY(yy, yyErr); 
	       std::vector<myItCl> itCls2; itPt2->fillItClusters(itCls2);
               for(size_t k2 = 0; k2 != itCls2.size(); k2++) {
	         if (itCls2[k2]->View() == emph::geo::W_VIEW) { 
	           if (fDebugIsOn) {
	             std::cerr << " SSDRecStationDwnstrAlgo1::arbitrateOverlapping,  Removal of Station point W View " << std::endl;
	             std::cerr << " " << (*itCls2[k2]) << std::endl; 
	            }
		   itPt1->addOverlapClusters(itCls2[k2], itPt2->ClusterCorrMeasurement(2), itPt2->ClusterMeasurementError(2));
		 }
	       }
	       fStPoints.erase(itPt2);
	       break; 
	     }   
	   } // on itPt2
	   if (didInnerChanged) break;
         }// loop over all non-ordered pair of point, and add info and remove 2nd one.. 
       } // Keep looking, possibly multiple removals.. 
      return hasChanged;  
    } // arbitrate.. 
  } // namespace ssdr
} //  namespace emph 
