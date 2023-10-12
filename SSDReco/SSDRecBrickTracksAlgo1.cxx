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
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "SSDReco/SSDRecBrickTracksAlgo1.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"

namespace emph { 
  namespace ssdr {
  
    const double SSDRecBrickTracksAlgo1::fSqrt2 = std::sqrt(2.0);
    const double SSDRecBrickTracksAlgo1::fOneOverSqrt2 = std::sqrt(0.5); 
				
    SSDTrPreArbitrationAlgo1::SSDTrPreArbitrationAlgo1(int itId, int ix0, int iy0, int ix1, 
                                                       int iy1, int i2, int i3, int i4, int i5) : 
      fTrId(itId), fIdClX0(ix0), fIdClY0(iy0), fIdClX1(ix1), fIdClY1(iy1),
      fIdSt2(i2), fIdSt3(i3), fIdSt4(i4), fIdSt5(i5) { ; }  
					       
						       

    SSDRecBrickTracksAlgo1::SSDRecBrickTracksAlgo1() :
      fRunHistory(nullptr),   
      fEmgeo(nullptr),
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fCoordConvert('A'),
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0),
      fNEvents(0), fNumCompactSaved(0), fDebugIsOn(false), fDoMigrad(true), fNoMagnet(false), 
      fDistFromBeamCenterCut(10.), fBeamCenterX(-3.5), fBeamCenterY(4.5),  fTrackSlopeCut(0.050), 
      fChiSqCut(1000.0), fAssumedMomentum(5.0), fMaxNumTrComb(50000), fMaxNumSpacePts(25),
      fDeltaZX(6, DBL_MAX), fDeltaZY(6, DBL_MAX), // Phase1b only. 
      fDeltaZSqX(6, DBL_MAX), fDeltaZSqY(6, DBL_MAX), // Phase1b only. 
      fTokenJob("undef"), fFitterFCN(nullptr),
      fInputStPts(), 
      fTrs()
      {
	for (size_t kkSt=0; kkSt != 4; kkSt++) { // Phase1b 
	    SSDRecStationDwnstrAlgo1 aStRec(kkSt+2);
	    fInputStPts.push_back(aStRec);
	}
    }
    // 
    SSDRecBrickTracksAlgo1::~SSDRecBrickTracksAlgo1() {
      if (fFOutTrs.is_open()) fFOutTrs.close();
      if (fFOutCompact.is_open()) fFOutCompact.close();
      std::cerr << " SSDRecBrickTracksAlgo1 destructor, number of saved Compact events for alignment " << fNumCompactSaved << std::endl;
      delete fFitterFCN;
    }
    //
    size_t SSDRecBrickTracksAlgo1::RecStation(size_t kSt, const art::Event &evt, 
                                const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
	
	if ((kSt < 2) || ((kSt-2) > fInputStPts.size())) { // Phase1b 
	  std::cerr << " SSDRecBrickTracksAlgo1::RecStation, illegal station number kSt " << kSt << " fatal... I mean it.. " << std::endl;
	  exit(2);
	}
	return fInputStPts[kSt-2].RecIt(evt, aSSDClsPtr);			
    }
    // 
    size_t SSDRecBrickTracksAlgo1::RecAndFitIt(const art::Event &evt, 
                const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
      fRunNum = evt.run(); fSubRunNum = evt.subRun(); fEvtNum = evt.id().event();
      // One need a fitter. 
      if (fFitterFCN == nullptr) {
        fFitterFCN = new SSDDwnstrTrackFitFCNAlgo1(fRunNum);
      } else {
        if (fFitterFCN->RunNumber() != fRunNum) {
	  delete fFitterFCN;
	  fFitterFCN = new SSDDwnstrTrackFitFCNAlgo1(fRunNum);
	}
      }
      if (fDebugIsOn) std::cerr << " SSDRecBrickTracksAlgo1::RecIt, starting on spill " 
                                 << fSubRunNum << " evt " << fEvtNum << std::endl;
      if (fEmgeo == nullptr) {
         fRunHistory = new runhist::RunHistory(fRunNum);   
         fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
         fNoMagnet = fEmgeo->MagnetUSZPos() < 0.;
	 for (size_t kSt=0; kSt != fDeltaZX.size(); kSt++) { 
	   fDeltaZX[kSt] = fEmVolAlP->ZPos(emph::geo::X_VIEW, kSt) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0);
	   fDeltaZSqX[kSt] = fDeltaZX[kSt]*fDeltaZX[kSt];
	 }
	 for (size_t kSt=0; kSt != fDeltaZY.size(); kSt++) {
	   fDeltaZY[kSt] = fEmVolAlP->ZPos(emph::geo::Y_VIEW, kSt) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 0);
	   fDeltaZSqY[kSt] = fDeltaZY[kSt]*fDeltaZY[kSt];
	 }	 
	 if (!fNoMagnet) {
	   std::cerr << " SSDRecBrickTracksAlgo1::RecAndFitIt ..The magnet seems to be in place, should not be.. " 
	                << " Assume it is not, 4 track params only. " << std::endl;  
	 }
      }   
      fTrs.clear();
      if ((fInputStPts[0].Size() == 0) || (fInputStPts[1].Size() == 0)) {
        if (fDebugIsOn) std::cerr << " ...  No data from station 2 or 3, so, no tracks.. " << std::endl;
	return 0; 
      }
      if ((fInputStPts[2].Size() == 0) || (fInputStPts[3].Size() == 0)) {
        if (fDebugIsOn) std::cerr << " ...  No data from station 4 or 5, so, no tracks.. " << std::endl;
	return 0; 
      }
      for (size_t kkSt=0; kkSt!= fInputStPts.size(); kkSt++) fInputStPts[kkSt].ResetUsage();
      fTrsForArb.clear();
      size_t nNoArbTrs = this->RecAndFitAll6Stations(aSSDClsPtr);
      
      size_t nArbs = this->Arbitrate(); 
      if (nArbs > 0) {
//        this->dumpInfoForR(); done in the main art module 
        this->dumpCompactEvt(aSSDClsPtr); // move also there? 
      }

      return nArbs;  
      
       
       
    } //RecAndFitIt
    //
    size_t SSDRecBrickTracksAlgo1::RecAndFitAll6Stations(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
    

      size_t nx0=0; size_t ny0=0; size_t nx1=0; size_t ny1=0;
      std::vector<std::pair<double, double> > x0Data; std::vector<std::pair<double, double> > y0Data;
      std::vector<std::pair<double, double> > x1Data; std::vector<std::pair<double, double> > y1Data;
      std::vector<int> x0Indices; std::vector<int> y0Indices; // These array are parallel to x0Data... 
      std::vector<int> x1Indices; std::vector<int> y1Indices; // These array are parallel to x0Data... 
      for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
        if (itCl->Station() == 0) {
	  if (itCl->View() == emph::geo::X_VIEW) {
	     nx0++; x0Data.push_back(fCoordConvert.getTrCoord(itCl, fAssumedMomentum)); x0Indices.push_back(itCl->ID());
	  } 
	  if (itCl->View() == emph::geo::Y_VIEW) {
	      ny0++; y0Data.push_back(fCoordConvert.getTrCoord(itCl, fAssumedMomentum)); y0Indices.push_back(itCl->ID());
	  }
	}
        if (itCl->Station() == 1) {
	  if (itCl->View() == emph::geo::X_VIEW) {
	      nx1++; x1Data.push_back(fCoordConvert.getTrCoord(itCl, fAssumedMomentum)); x1Indices.push_back(itCl->ID());
	  }
	  if (itCl->View() == emph::geo::Y_VIEW) {
	      ny1++; y1Data.push_back(fCoordConvert.getTrCoord(itCl, fAssumedMomentum)); y1Indices.push_back(itCl->ID());
	  }
	}
      }
      if (fDebugIsOn) std::cerr << " Starting on RecAndFitAll6Stations, spill " << fSubRunNum << " evt " << fEvtNum << " with " 
                               << nx0 << " X St0 clusters,  " << ny0 << " Y St0, " << nx1 << " X St1 " << ny1 
			       << " Y St1, and .. " << std::endl << "          "   
                               << fInputStPts[0].Size() << " St2 Pts,  " << fInputStPts[1].Size() << " St3 Pts, "  
			       << fInputStPts[2].Size() << " St4 Pts,  " << fInputStPts[3].Size() << " St5 Pts " << std::endl;
      if ((nx0 == 0) || (ny0 == 0)) {
        if (fDebugIsOn) std::cerr << " ... Not clusters in Station 0, presume that this is a false trigger, no tracks " << std::endl;
	return 0;
      } 			       
      if ((nx1 == 0) && (ny1 == 0)) {
        if (fDebugIsOn) std::cerr << " ... Both X and Y number of cluster are 0 at Station 1, so, no tracks  " << std::endl;
	return 0;
      }
      if (nx1 == 0) { 
        x1Data.push_back(std::pair<double, double>(0., 1.0e6)); x1Indices.push_back(INT_MAX);  nx1++; 
	if (fDebugIsOn) std::cerr << " Added dummy point at Station 1, XView, check size " << x1Data.size() << std::endl;
      }
      if (ny1 == 0) { 
         y1Data.push_back(std::pair<double, double>(0., 1.0e6)); y1Indices.push_back(INT_MAX); ny1++; 
	 if (fDebugIsOn) std::cerr << " Added dummy point at Station 1, YView, check size " << y1Data.size() << std::endl;
      }
      size_t nSpacePtsTot = 0;
      for (size_t kSt = 0; kSt != fInputStPts.size(); kSt++) nSpacePtsTot += fInputStPts[kSt].Size();
      if (nSpacePtsTot > fMaxNumSpacePts) {
        if (fDebugIsOn) std::cerr << " ... Too many Space Points (" << nSpacePtsTot << ")  in station 2 through 5.. geve up  " << std::endl;
	return 0;
      } 			       
      for (size_t kSt = 0; kSt != fInputStPts.size(); kSt++) fInputStPts[kSt].ResetUsage();
      //     
      // preload the data, to put some multiplicity cuts in stations, and avoid too many inner loops in this pattern recognition. 
      //
      size_t kx0=0;
      const double angleRollX0 = fEmVolAlP->Roll(emph::geo::X_VIEW, 0, 0);
      const double angleRollCenterX0 = fEmVolAlP->RollCenter(emph::geo::X_VIEW, 0, 0);
      const double angleRollY0 = fEmVolAlP->Roll(emph::geo::Y_VIEW, 0, 0);
      const double angleRollCenterY0 = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 0, 0);
      
     // 8 embedded loops, X0, Y0, then St5, then confirming data, Station 1 (two ), St2 through 4 
      for (std::vector<std::pair<double, double> >::const_iterator itX0 = x0Data.cbegin(); itX0 != x0Data.cend(); itX0++, kx0++) {
        
        size_t ky0=0;
        for (std::vector<std::pair<double, double> >::const_iterator itY0 = y0Data.cbegin(); itY0 != y0Data.cend(); itY0++, ky0++) {
	   // Apply rotation.. 
	  const double xVal0Corr = itX0->first + (itY0->first - angleRollCenterX0) * angleRollX0; 
	  const double yVal0Corr = itY0->first + (itX0->first - angleRollCenterY0) * angleRollY0;
	 // Apply distance from the center of the 120 GeV beam 
	  const double distToBeamCenterX = xVal0Corr - fBeamCenterX;
	  const double distToBeamCenterY = yVal0Corr - fBeamCenterY;
	  const double distToBeamCenter = std::sqrt(distToBeamCenterX*distToBeamCenterX + distToBeamCenterY*distToBeamCenterY);
	  if (fDebugIsOn) std::cerr << " At Station 0, at " << xVal0Corr << " yVal0Corr " 
	                            << yVal0Corr << " Dist from Beam Center " << distToBeamCenter << std::endl;
	  if (distToBeamCenter > fDistFromBeamCenterCut) continue;
	 // On to Station 5. 
          size_t kSt5=0;
          for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt5 = fInputStPts[3].CBegin(); 
	      itSt5 != fInputStPts[3].CEnd(); itSt5++, kSt5++) {
	   
	    // Station point X and Y are already expressec in global coord, Rolls corrected.
	    const double xSlope = (itSt5->X() - xVal0Corr)/fDeltaZX[5];
	    const double ySlope = (itSt5->Y() - yVal0Corr)/fDeltaZY[5];
	   // Cuts..
	    if (fDebugIsOn) std::cerr << " ....  At Station 5, X " << itSt5->X() << " Xslope " << xSlope 
	          << " and Y = " << itSt5->Y() << "  ySlope " << ySlope << std::endl;
	    
	    if (std::abs(xSlope) > fTrackSlopeCut) continue; 
	    if (std::abs(ySlope) > fTrackSlopeCut) continue; 
	   
	    const double xSlopeErrSq = (itSt5->XErr()*itSt5->XErr() +  itX0->second*itX0->second)/(fDeltaZSqX[5]); 		       
	    const double ySlopeErrSq = (itSt5->YErr()*itSt5->YErr() +  itY0->second*itY0->second)/(fDeltaZSqY[5]);
	   //interpolate  Station 1 
	    const double xSt1Pred = xVal0Corr + xSlope * fDeltaZX[1];		       
	    const double ySt1Pred = yVal0Corr + ySlope * fDeltaZY[1];
	    const double xSt1PredErrSq = itX0->second*itX0->second + xSlopeErrSq * fDeltaZSqX[1]; // no correlation taken into account. 
	    const double ySt1PredErrSq = itY0->second*itY0->second + ySlopeErrSq * fDeltaZSqY[1]; // no correlation taken into account. 
	   // ask for confirmation at station 1. 
            const double angleRollX1 = fEmVolAlP->Roll(emph::geo::X_VIEW, 1, 0);
            const double angleRollCenterX1 = fEmVolAlP->RollCenter(emph::geo::X_VIEW, 1, 0);
            const double angleRollY1 = fEmVolAlP->Roll(emph::geo::Y_VIEW, 1, 0);
            const double angleRollCenterY1 = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 1, 0);
	    size_t kx1 = 0;
	    if (fDebugIsOn) std::cerr << " ... Getting to Station 1, x1Data.size() " << x1Data.size() 
	                              << " Y " <<  y1Data.size() <<  std::endl; 
            for (std::vector<std::pair<double, double> >::const_iterator itX1 = x1Data.cbegin(); itX1 != x1Data.cend(); itX1++, kx1++) {
	      if (fDebugIsOn) std::cerr << " ... ... O.K., kx1 " << kx1 << " Check y1 size " << y1Data.size() <<  std::endl;
	      size_t ky1 = 0;
              for (std::vector<std::pair<double, double> >::const_iterator itY1 = y1Data.cbegin(); itY1 != y1Data.cend(); itY1++, ky1++) {
	        if (fDebugIsOn) std::cerr << " ... ... ... O.K., ky1 " << ky1 << std::endl;
	        const double xVal1Corr = itX1->first + (itY1->first - angleRollCenterX1) * angleRollX1; 
	        const double yVal1Corr = itY1->first + (itX1->first - angleRollCenterY1) * angleRollY1;
	        const double dX1Sq = (xVal1Corr -  xSt1Pred ) *  (xVal1Corr -  xSt1Pred);
	        const double chiX1 = dX1Sq/(itX1->second * itX1->second + xSt1PredErrSq);
		if (fDebugIsOn) std::cerr << " ... .... At Station 1, at " << xVal1Corr << " yVal1Corr " 
		                          << yVal1Corr << " chiX1 " << chiX1 <<  std::endl; 
	        if (chiX1 > fChiSqCut) continue;
	        const double dY1Sq = (yVal1Corr -  ySt1Pred ) *  (yVal1Corr -  ySt1Pred );
	        const double chiY1 = dY1Sq/(itY1->second * itY1->second + ySt1PredErrSq); 
		if (fDebugIsOn) std::cerr << " ... ... O.K. chiSq cut in X and in Y   "  << chiY1 <<  std::endl; 
	        if (chiY1 > fChiSqCut) continue;
	       // Confirmation on Station 2. 
	        size_t kSt2 = 0;
	        const double xSt2Pred = xVal0Corr + xSlope * fDeltaZX[2];		       
	        const double ySt2Pred = yVal0Corr + ySlope * fDeltaZY[2];
	        const double xSt2PredErrSq = itX0->second*itX0->second + xSlopeErrSq	* fDeltaZSqX[2]; // no correlation taken into account. 
	        const double ySt2PredErrSq = itY0->second*itY0->second + ySlopeErrSq	* fDeltaZSqY[2]; // no correlation taken into account. 
                for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt2 = fInputStPts[0].CBegin(); 
	                   itSt2 != fInputStPts[0].CEnd(); itSt2++, kSt2++) { 
	          if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
                  const double xVal2Corr = itSt2->X(); // Already corrected for rotation.. 
                  const double yVal2Corr = itSt2->Y(); // To estimate starting value for the track param fit.
	          const double dX2Sq = (xVal2Corr -  xSt2Pred ) *  (xVal2Corr -  xSt2Pred);
	          const double chiX2 = dX2Sq/(itX0->second * itX0->second + xSt2PredErrSq); 
	          if (chiX2 > fChiSqCut) continue;
	          const double dY2Sq = (yVal2Corr -  ySt2Pred ) *  (yVal2Corr -  ySt2Pred );
	          const double chiY2 = dY2Sq/(itY0->second * itY0->second + ySt2PredErrSq); 
	          if (chiY2 > fChiSqCut) continue;		 
		  // Station 3, same ... 
 	          size_t kSt3 = 0;
	          const double xSt3Pred = xVal0Corr + xSlope * fDeltaZX[3];		       
	          const double ySt3Pred = yVal0Corr + ySlope * fDeltaZY[3];
	          const double xSt3PredErrSq = itX0->second*itX0->second + xSlopeErrSq	* fDeltaZSqX[3]; // no correlation taken into account. 
	          const double ySt3PredErrSq = itY0->second*itY0->second + ySlopeErrSq	* fDeltaZSqY[3]; // no correlation taken into account. 
                  for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt3 = fInputStPts[1].CBegin(); 
	                   itSt3 != fInputStPts[1].CEnd(); itSt3++, kSt3++) { 
 	            if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
                    const double xVal3Corr = itSt3->X(); // Already corrected for rotation.. 
                    const double yVal3Corr = itSt3->Y(); // To estimate starting value for the track param fit.
	            const double dX3Sq = (xVal3Corr -  xSt3Pred ) *  (xVal3Corr -  xSt3Pred);
	            const double chiX3 = dX3Sq/(itX0->second * itX0->second + xSt3PredErrSq); 
	            if (chiX3 > fChiSqCut) continue;
	            const double dY3Sq = (yVal3Corr -  ySt3Pred ) *  (yVal3Corr -  ySt3Pred );
	            const double chiY3 = dY3Sq/(itY0->second * itY0->second + ySt3PredErrSq); 
	            if (chiY3 > fChiSqCut) continue;		
		  // Station 4, same ... 
		 
 	            size_t kSt4 = 0;
	            const double xSt4Pred = xVal0Corr + xSlope * fDeltaZX[4];		       
	            const double ySt4Pred = yVal0Corr + ySlope * fDeltaZY[4];
	            const double xSt4PredErrSq = itX0->second*itX0->second + xSlopeErrSq	* fDeltaZSqX[4]; // no correlation taken into account. 
	            const double ySt4PredErrSq = itY0->second*itY0->second + ySlopeErrSq	* fDeltaZSqY[4]; // no correlation taken into account. 
                    for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt4 = fInputStPts[2].CBegin(); 
	          	  itSt4 != fInputStPts[2].CEnd(); itSt4++, kSt4++) { 
  	              if (fDebugIsOn) std::cerr << " ... ... ... At station 4, at x = " << itSt4->X() << " Y " << itSt4->Y() << std::endl; 
                      const double xVal4Corr = itSt4->X(); // Already corrected for rotation.. 
                      const double yVal4Corr = itSt4->Y(); // To estimate starting value for the track param fit.
	              const double dX4Sq = (xVal4Corr -  xSt4Pred ) *  (xVal4Corr -  xSt4Pred);
	              const double chiX4 = dX4Sq/(itX0->second * itX0->second + xSt4PredErrSq); 
	              if (chiX4 > fChiSqCut) continue;
	              const double dY4Sq = (yVal4Corr -  ySt4Pred ) *  (yVal4Corr -  ySt4Pred );
	              const double chiY4 = dY4Sq/(itY0->second * itY0->second + ySt4PredErrSq); 
	              if (chiY4 > fChiSqCut) continue;		
//		 
//  Now perform a linear fit.  4 Parameters, up to 36 data points. ( One view might be missing from Station 1 to 5. ). 
//
                      fFitterFCN->ResetInputPts();
		      fFitterFCN->AddInputUpstreamStation(0, xVal0Corr, itX0->second, yVal0Corr, itY0->second);
		      fFitterFCN->AddInputUpstreamStation(1, xVal1Corr, itX1->second, yVal1Corr, itY1->second);
	              fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	              fFitterFCN->AddInputPt(itSt3);
	              fFitterFCN->AddInputPt(itSt4);
	              fFitterFCN->AddInputPt(itSt5);
	              if (this->doFitAndStore(rb::SIXSTATION, xVal0Corr, yVal0Corr, xSlope, ySlope)) {
		    //  Good track.. Might be a duplicate, to b sorted out later. 
		        if (fDebugIsOn) std::cerr << " Got a track, numTrack now " << fTrs.size() << std::endl;
	                int kTrCnt = static_cast<int>(fTrs.size());
			std::vector<rb::DwnstrTrackAlgo1>::reverse_iterator itNewTr= fTrs.rbegin(); itNewTr->SetID(kTrCnt);
			int uFlagInc = 1000 * (kTrCnt-1);
		        itSt2->SetUserFlag(uFlagInc + itSt2->UserFlag()); itSt3->SetUserFlag(uFlagInc + itSt3->UserFlag()); 
		        itSt4->SetUserFlag(uFlagInc + itSt4->UserFlag()); itSt5->SetUserFlag(uFlagInc + itSt5->UserFlag()); 
		        //
		        // Get and Store the Cluster information for re-alignment.
			//
			SSDTrPreArbitrationAlgo1 aNewTrAA(itNewTr->ID(), x0Indices[kx0], y0Indices[ky0], x1Indices[kx1], y1Indices[ky1], 
			                            itSt2->ID(), itSt3->ID(), itSt4->ID(), itSt5->ID());
			fTrsForArb.push_back(aNewTrAA); // Not triple A quality.. 
			// Limit the number of combinations.. Brute force, no physics done here.. JUst picking up somewide angle tracks. 
			if (fTrsForArb.size() > 50000) break;			    
		      }
	              if (fTrsForArb.size() > fMaxNumTrComb) break;			    
		    } // on Space Points in Station 4 
		    if (fTrsForArb.size() > fMaxNumTrComb) break;			    
	          } // on Space Points in Station 3
		  if (fTrsForArb.size() > fMaxNumTrComb) break;			    
                } // on Space Points in Station 2
		if (fTrsForArb.size() > fMaxNumTrComb) break;			    
              } // On Y coordinate, Station 1  
	      if (fTrsForArb.size() > fMaxNumTrComb) break;			    
            } // On X coordinate, Station 1 
	    if (fTrsForArb.size() > fMaxNumTrComb) break;			    
          } // On Space Points Station 5 
	  if (fTrsForArb.size() > fMaxNumTrComb) break;			    
        } // on Y coordinate, Station 0  
        if (fTrsForArb.size() > fMaxNumTrComb) break;			    
      } // on X coordinate, Station 0
      // 
      // Place holder for arbitation., and dumping the Cluster for re-alignment.    
      if (fDebugIsOn) std::cerr << " End of SSDRecBrickTracksAlgo1::RecAndFitAll6Stations, num Tracks " << fTrs.size() << std::endl;
      return fTrs.size(); 
    }
    //
    bool SSDRecBrickTracksAlgo1::doFitAndStore(rb::DwnstrTrType type, double xStart, double yStart, 
                               double xSlopeStart, double ySlopeStart) {
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsOut, parsOutErr;
//       fFitterFCN->SetDebugOn(fDebugIsOn);
       fFitterFCN->SetDebugOn(false);
       if (fDebugIsOn) {
         std::cerr << " SSDRecBrickTracksAlgo1::doFitAndStore, start, X0 = " <<  xStart << " Y0 " << yStart 
	           << " slopes " << xSlopeStart << ", " << ySlopeStart << " pAssumed " << fAssumedMomentum << std::endl;
	 fFitterFCN->printInputData();	   
       }
       uPars.Add(std::string("X_0"), xStart, 0.1, -20., 20.);  
       uPars.Add(std::string("Slx_0"), xSlopeStart, 0.001, -0.05, 0.05);  // for Misligned 7c. No magnet Misalignament flag 25000025, Dgap = 3.0 mm  
       uPars.Add(std::string("Y_0"), yStart, 0.1, -20., 20.);  
       uPars.Add(std::string("Sly_0"), ySlopeStart, 0.001, -0.05, 0.05); 
        if (!fNoMagnet) 
	  uPars.Add(std::string("PInv"), -1.0/fAssumedMomentum, std::abs(2.0/fAssumedMomentum), -5.0, 5.0); 
       unsigned int nPars = (fNoMagnet) ? 4 : 5; 
       std::vector<double> initValsV, initValsE; // for use in the Simple Minimizer.. 
       for (unsigned int k=0; k != nPars; k++) { initValsV.push_back(uPars.Value(k)); initValsE.push_back(uPars.Error(k)); } 
       // Testing the FCN, once .. 
       if (fDebugIsOn) { 
         std::cerr << " ....... About to test the FCN with " << initValsV.size() << " parameters.. " << std::endl;
         double aChiSqOnce = (*fFitterFCN)(initValsV);
//         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that is enough for now " << std::endl; exit(2);
         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that we keep going... " << std::endl;
       }
       ROOT::Minuit2::MnMigrad migrad((*fFitterFCN), uPars);
       if (fDebugIsOn) std::cerr << " ..... About to call migrad... " << std::endl;
       //
       ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
       if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
       const bool isMigradValid = min.IsValid(); 
       //
       bool isSimplexValid = true;
       int flagValid = 0; // 0 nothing worked, 1 MiGrad is O.K., 2, Simplex is Ok.  
       double chiSq = DBL_MAX;
       if (!isMigradValid) {
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize((*fFitterFCN), initValsV, initValsE);
 	 parsOutErr = minS.UserParameters().Errors();
 	 parsOut = minS.UserParameters().Params();
         isSimplexValid = minS.IsValid();
	 chiSq = minS.Fval();
	 if (isSimplexValid) flagValid = 2;
       } else {
	 chiSq = min.Fval();
 	 parsOutErr = min.UserParameters().Errors();
 	 parsOut = min.UserParameters().Params();
	 flagValid = 1;
       }
       if (flagValid == 0) {
         if (fDebugIsOn) std::cerr << " No valid fit, so, no new tracks " << std::endl;
	 return false;
       }
       if (fDebugIsOn && (flagValid == 1)) std::cerr <<  " Migrad fit succeeded, chiSq " << chiSq << std::endl;
       if (fDebugIsOn && (flagValid == 2)) std::cerr <<  " Simplex fit succeeded, chiSq " << chiSq << std::endl;
       rb::DwnstrTrackAlgo1 aTr; 
       aTr.SetType(type); 
       if (parsOut.size() == 5) { 
         aTr.SetTrParams(parsOut[0], parsOut[1], parsOut[2], parsOut[3], 1.0/parsOut[4]);
         aTr.SetTrParamsErrs(parsOutErr[0], parsOutErr[1], parsOutErr[2], parsOutErr[3], parsOutErr[4]/(parsOut[4]*parsOut[4]));
       } else {
         aTr.SetTrParams(parsOut[0], parsOut[1], parsOut[2], parsOut[3], DBL_MAX);
         aTr.SetTrParamsErrs(parsOutErr[0], parsOutErr[1], parsOutErr[2], parsOutErr[3], DBL_MAX);
       }
       aTr.SetChiSq(chiSq);
       if (flagValid == 1 && min.HasCovariance()) {
         for (size_t i=0; i != static_cast<size_t>(nPars); i++) {
           for (size_t j=0; j != static_cast<size_t>(nPars); j++) {
	     const double cc = min.UserCovariance()(i,j);
             aTr.SetCovarianceMatrix((i*(nPars-1) + j), cc);
	   }
	 }
	 fTrs.push_back(aTr);
	 return true;
       } // Asking for a valid Migrad fit.. 
       return false;
    } // doFitAndStore
    //
    bool SSDRecBrickTracksAlgo1::IsAlreadyFound(const rb::DwnstrTrackAlgo1 &aTr) const {  // this needs tuning.. 
      if (fTrs.size() == 0) return false;
      for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it ++) {
        if ((std::abs(aTr.XSlope() - it->XSlope()) < 1.0e-5) && 
	    (std::abs(aTr.YSlope() - it->YSlope()) < 1.0e-5)) return true;   
      }
      return false;
    }
    
    size_t SSDRecBrickTracksAlgo1::Arbitrate() {
      if (fDebugIsOn)  std::cerr << " Entering SSDRecBrickTracksAlgo1::Arbitrate, spill " 
                                 << fSubRunNum << " evt " << fEvtNum << " with " << fTrs.size() << " tracks " << std::endl;
      if (fTrs.size() < 2 ) return fTrs.size();
      if (fTrsForArb.size() == 0) {
        std::cerr << " SSDRecBrickTracksAlgo1::Arbitrate Logic error, no back pointer information, fatal, quit here and now " << std::endl;
	exit(2); 
      }
      if (fTrsForArb.size() < 2 ) {
        std::cerr << " SSDRecBrickTracksAlgo1::Arbitrate Logic error, t enough  back pointer information, fatal...  " << std::endl;
	exit(2); 
      }
      int nItrBigLoop = 0; size_t numDeprecated = 0;
      while (true) {
        if (fDebugIsOn) std::cerr << " .... nItrBigLoop " << nItrBigLoop << std::endl;
        bool hasChanged = false;
	for (size_t kArbA = 0; kArbA != fTrsForArb.size() - 1; kArbA++) {
	  std::vector<rb::DwnstrTrackAlgo1>::iterator itA = this->GetTrackPtr(fTrsForArb[kArbA].fTrId); 
	  if (itA == fTrs.end()) {
            std::cerr << " SSDRecBrickTracksAlgo1::Arbitrate Logic error, unknown trackid " 
	             << fTrsForArb[kArbA].fTrId << std::endl;
	    exit(2); 
          }
	  if (itA->ChiSq() >= DBL_MAX) continue;  // Already out of the picture. 
	  const double x5A = itA->XOffset() + fDeltaZX[5]*itA->XSlope();
	  const double y5A = itA->YOffset() + fDeltaZY[5]*itA->YSlope();
	  for (size_t kArbB = kArbA+1; kArbB != fTrsForArb.size(); kArbB++) { 
	    std::vector<rb::DwnstrTrackAlgo1>::iterator itB = this->GetTrackPtr(fTrsForArb[kArbB].fTrId); 
	    if (itB == fTrs.end()) {
              std::cerr << " SSDRecBrickTracksAlgo1::Arbitrate Logic error, 2nd loop, unknown trackid " 
	             << fTrsForArb[kArbB].fTrId << std::endl;
	      exit(2); 
            }
	    if (itB->ChiSq() >= DBL_MAX) continue;  // Already out of the picture.
	    bool isSame = false; 
	    if  (fTrsForArb[kArbA].fIdSt5 == fTrsForArb[kArbB].fIdSt5) isSame = true;  // pointing to two different Space Point  of Station5. 
	    const double x5B = itB->XOffset() + fDeltaZX[5]*itB->XSlope();
	    const double y5B = itB->YOffset() + fDeltaZY[5]*itB->YSlope();
	    const double dd5Sq = (x5A - x5B)*(x5A - x5B) + (y5A - y5B)*(y5A - y5B);
	    // but too close to each other.. Say 2 mm 
	    if (dd5Sq < 4.0) isSame = true; 
            // 
	    // In addtiion, Require at least 2 distinct space points not to be arbitrated away  
	    //
	    int n234Shared = 0;
	    if (fTrsForArb[kArbA].fIdSt2 == fTrsForArb[kArbB].fIdSt2) n234Shared++;
	    if (fTrsForArb[kArbA].fIdSt3 == fTrsForArb[kArbB].fIdSt3) n234Shared++;
	    if (fTrsForArb[kArbA].fIdSt4 == fTrsForArb[kArbB].fIdSt4) n234Shared++;
	    if (n234Shared > 1) isSame = true;
           // pick the smallest chiSq
	    if (!isSame) continue; 
	    hasChanged = true;
	    numDeprecated++;
	    if ( itB->ChiSq() > itA->ChiSq()) { 
	      if (fDebugIsOn) std::cerr << " .... Deprecating track id " << itB->ID() << " chiSq " 
	                      << itB->ChiSq() << " is larger than for track Id " << itA->ID() 
			      << " whose chi sq is " << itA->ChiSq() << std::endl;   
	      itB->SetChiSq(DBL_MAX);
	    } else {
	      if (fDebugIsOn) std::cerr << " .... Deprecating track id " << itA->ID() << " chiSq " 
	                      << itA->ChiSq() << " is larger than for track Id " << itB->ID() 
			      << " whose chi sq is " << itB->ChiSq() << std::endl;   
	      itA->SetChiSq(DBL_MAX); 
	    }
	    break;
	  } // on kArbB
        } // on kArba 
        if (!hasChanged) break;
	nItrBigLoop++;
	if (nItrBigLoop > 100) { // kill this event.. too messy... 
	  for (std::vector<rb::DwnstrTrackAlgo1>::iterator it=fTrs.begin(); it != fTrs.end(); it++) it->SetChiSq(DBL_MAX);
	  std::cerr << " SSDRecBrickTracksAlgo1::Arbitrate, spill " << fSubRunNum << " evt " 
	            << fEvtNum << " too messy, reject this event.. " << std::endl;
	  return 0;
	}
      } // Loop on possible change. 
      if (fDebugIsOn) std::cerr << " End of arbitration, after " << nItrBigLoop 
                                << " kill-loops, num Deprecated  " << numDeprecated << " out of " << fTrs.size() << std::endl;
      return (fTrs.size() - numDeprecated); 
    }
    void SSDRecBrickTracksAlgo1::openOutputCsvFiles() const {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameStrStr;
       fNameStrStr << "SSDRecBrickTracksAlgo1_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutTrs.open(fNameStr.c_str());
       fFOutTrs << " spill evt nTrsT nTrs iTr type x0 x0Err x0sl xslErr y0 y0Err y0sl y0slErr chiSq " << std::endl;
       //
       
     }
     //
     void SSDRecBrickTracksAlgo1::dumpInfoForR() const {
       if (fTrs.size() == 0) return;
       if (!fFOutTrs.is_open()) this->openOutputCsvFiles(); 
       std::ostringstream headerStrStr; 
       headerStrStr << " " << fSubRunNum << " " << fEvtNum << " " << fTrs.size(); 
       std::string headerStr(headerStrStr.str());
       int nTrOK = 0; 
       for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it++)  
         if (it->ChiSq() < DBL_MAX) nTrOK++;

       size_t k=0;
       for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it++, k++) { 
         if (it->ChiSq() >= DBL_MAX) continue;
         fFOutTrs << headerStr << " " << nTrOK << " " << it->ID() << " " << it->Type()  
	         << " " << it->XOffset() << " " << it->XOffsetErr() << " " << it->XSlope() << " " << it->XSlopeErr()
		 << " " << it->YOffset() << " " << it->YOffsetErr() << " " << it->YSlope() << " " << it->YSlopeErr()
		 << " " << it->ChiSq() << std::endl; 
       }
     }
     void SSDRecBrickTracksAlgo1::dumpCompactEvt(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       if (!fFOutCompact.is_open()) {
         std::ostringstream aFOutCompactStrStr; 
         aFOutCompactStrStr << "./CompactAlgo1Data_" << fRunNum  << "_" << fTokenJob << "_V1g.dat";
         std::string aFOutCompactStr(aFOutCompactStrStr.str());
         fFOutCompact.open(aFOutCompactStr.c_str(),  std::ios::binary | std::ios::out);
         std::cerr << " Opening CompactEvt file " << aFOutCompactStr << " andkeep going  .. " << std::endl; 
       }
       const double maxClustSep = 75.;
       if (fDebugIsOn) std::cerr << " SSDRecBrickTracksAlgo1::dumpCompactEvt, at spill " << fSubRunNum << " evt " << fEvtNum << std::endl;
       int key=687503; // fixed, distinct from similar code found in SSDAlign3DUVAlgo1Add
       int numDouble = ( 2 * (2*(8) + 2 + 4)) ; // for 2 is mean & RMS, then, X + Y + U + V // Phase1b 
       // Version 1g:  Consider only the single track events.. And wide angle.. 
       int numOKTracks = 0;
        for (std::vector<SSDTrPreArbitrationAlgo1>::const_iterator itTArb = fTrsForArb.cbegin(); itTArb != fTrsForArb.cend(); itTArb++) { 
         std::vector<rb::DwnstrTrackAlgo1>::const_iterator itTr = GetTrackPtrConst(itTArb->fTrId);
	 if (itTr == fTrs.cend()) continue; // Should not happen.. 
	 if (itTr->ChiSq() > 1.0e9) continue; // Could happen, duplicated or really bad track.
	 const double scatAngle = std::sqrt(itTr->XSlope() * itTr->XSlope() +  itTr->YSlope() * itTr->YSlope());
	 if (scatAngle < 0.01) continue; 
	 numOKTracks++; 
       }
       if (fDebugIsOn) std::cerr << " ... Number of good arbitrated tracks .. " << numOKTracks << std::endl;
       if (numOKTracks != 1) return;
       int numClReal = 0;
       const double stripAvNone = 0.; const double stripRmsNone = 1.0e9; 
       for (std::vector<SSDTrPreArbitrationAlgo1>::const_iterator itTArb = fTrsForArb.cbegin(); itTArb != fTrsForArb.cend(); itTArb++) { 
         std::vector<rb::DwnstrTrackAlgo1>::const_iterator itTr = GetTrackPtrConst(itTArb->fTrId);
	 if (itTr == fTrs.cend()) continue; // Should not happen.. 
	 if (itTr->ChiSq() > 1.0e9) continue; // Could happen, duplicated or really bad track.
	 const double scatAngle = std::sqrt(itTr->XSlope() * itTr->XSlope() +  itTr->YSlope() * itTr->YSlope());
	 if (scatAngle < 0.01) continue;   
         // a bit of memory waste if strictY6 is true, 4 doubles being either 0 or DBL_MAX. 
      // but keep the code relatively simple. 
      	 if (fDebugIsOn) std::cerr  << " ..... At track " << itTr->ID() << std::endl;
         std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsX(8, aSSDClsPtr->cend());  // station 0 to 5, Phase1b
         std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsY(8, aSSDClsPtr->cend());// with two double sensor 
         std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsU(2, aSSDClsPtr->cend());  // station 2 nd 3 
         std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsV(4, aSSDClsPtr->cend());// station 4 and 5, two adjacent sensors.
	 // Now fill those.. Not the most efficient way...  
	 for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	   if ((itCl->Station() == 0) && (itCl->View() == emph::geo::X_VIEW) 
	      && (itCl->ID() == itTArb->fIdClX0) && (this->isClusterIsolated(itCl, aSSDClsPtr, maxClustSep))) { 
	        {mySSDClsPtrsX[0] = itCl; numClReal++; break; } 
	      }  
	 } // on clusters, any of them 
	 for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	   if ((itCl->Station() == 1) && (itCl->View() == emph::geo::X_VIEW) 
	      && (itCl->ID() == itTArb->fIdClX1) &&  (this->isClusterIsolated(itCl, aSSDClsPtr, maxClustSep))) {
	         mySSDClsPtrsX[1] = itCl; numClReal++;break; 
		 } 
	 } // on clusters, any of them 
	 for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	   if ((itCl->Station() == 0) && (itCl->View() == emph::geo::Y_VIEW) 
	      && (itCl->ID() == itTArb->fIdClY0) && (this->isClusterIsolated(itCl, aSSDClsPtr, maxClustSep))) { 
	         mySSDClsPtrsY[0] = itCl; numClReal++;break; 
		 }
	 } // on clusters, any of them 
	 for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	   if ((itCl->Station() == 1) && (itCl->View() == emph::geo::Y_VIEW) 
	      && (itCl->ID() == itTArb->fIdClY1) && (this->isClusterIsolated(itCl, aSSDClsPtr, maxClustSep))) {
	         mySSDClsPtrsY[1] =  itCl; numClReal++; break; 
	         }
	 } // on clusters, any of them 
	 if (fDebugIsOn) std::cerr  << " ..... ... done station 0 and 1  " << std::endl;
	 // Now the Space Point Stations.. Tedious, the pointer have a different layout..
	 for (size_t kkSt = 0; kkSt != fInputStPts.size(); kkSt++) { // on downstream stations (with respect to empty target !) 
	   const size_t kSt = kkSt + 2;
	   int idStPt = INT_MAX; 
	   switch (kSt) {
	     case 2 : {idStPt = itTArb->fIdSt2; break; }  
	     case 3 : {idStPt = itTArb->fIdSt3; break; }  
	     case 4 : {idStPt = itTArb->fIdSt4; break; }  
	     case 5 : {idStPt = itTArb->fIdSt5; break; }
	     default : {
	       std::cerr << "  SSDRecBrickTracksAlgo1::dumpCompactEvt Space Point Station indices error, kSt " 
	                 << kSt << " fatal " << std::endl; exit(2); 
	     }  
	   }
	   std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt = fInputStPts[kkSt].GetStationPointPtr(idStPt); 
	   if (itSt == fInputStPts[kkSt].CEnd()) continue; // Should not happen.. 
	   size_t numClInSpSt = itSt->NumClusters();
	   if (itSt->Station() != static_cast<int>(kSt)) {
	     std::cerr << "  SSDRecBrickTracksAlgo1::dumpCompactEvt Space Point Station number indices error, kSt " 
	   	       << kSt << " fatal " << " Station num " << itSt->Station() << std::endl; exit(2); 
	   } 
	   for (size_t kCl=0; kCl != numClInSpSt; kCl++) {
	      emph::geo::sensorView aView =  itSt->ClusterView(kCl); 
	      int aSensor = itSt->ClusterSensorId(kCl);
	      int aID = itSt->ClusterID(kCl);
	      if (fDebugIsOn) std::cerr << " ... ... ... Inspecting Cluster station " << kSt 
	                                << " View " << aView << " sensor " << aSensor << " ID " << aID << std::endl; 
	      for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	   	if ((itCl->Station() == static_cast<int>(kSt)) && (itCl->View() == aView) 
	   	     && (itCl->ID() == aID)) {
		  if (!this->isClusterIsolated(itCl, aSSDClsPtr, maxClustSep)) {
		    if (fDebugIsOn) std::cerr << " Cluster is not isolated,.. skip .. " << std::endl; 
		    continue;
		  }  
		  if (fDebugIsOn) std::cerr << " ... Isolated Cluster, found it mean strip " << itCl->WgtAvgStrip() << std::endl;
		  numClReal++;
	          if (kSt < 4) { 
	            if (aView == emph::geo::X_VIEW) mySSDClsPtrsX[kSt] = itCl; 
	            if (aView == emph::geo::Y_VIEW) mySSDClsPtrsY[kSt] = itCl; 
	            if (aView == emph::geo::U_VIEW) mySSDClsPtrsU[kSt-2] = itCl; //Phase1b
	   	  } else { // double sensors. 
	            size_t kDSt = 4 + 2*(kSt-4) + (aSensor % 2);
	            if (aView == emph::geo::X_VIEW) mySSDClsPtrsX[kDSt] = itCl; 
	            if (aView == emph::geo::Y_VIEW) mySSDClsPtrsY[kDSt] = itCl; 
	            if (aView == emph::geo::W_VIEW) mySSDClsPtrsV[kDSt-4] = itCl; 
	          }
	        } // a match, original list of cluster, and clusters found in Space Points.  
	      } // on original list of clusters, any of them 
	    } // On Downstream Stations.. 
	   if (fDebugIsOn) std::cerr  << " ..... ... done Station   " << kSt << std::endl;
	 } // On stations, upload Cluster pointers. 
	 //
	 // Write stuff out.. 
	 //  
	 if (fDebugIsOn) std::cerr << " ... Total number of good clusters " << numClReal << std::endl;
	 if (numClReal < 10) return;
	 fNumCompactSaved++;
	 std::cerr << " SSDRecBrickTracksAlgo1::dumpCompactEvt, saving alignment event " << fEvtNum << std::endl;
         fFOutCompact.write(reinterpret_cast<char*>(&key), sizeof(int)); 
         fFOutCompact.write(reinterpret_cast<char*>(&numDouble), sizeof(int));
         fFOutCompact.write(reinterpret_cast<char*>(&fSubRunNum), sizeof(int)); 
         fFOutCompact.write(reinterpret_cast<char*>(&fEvtNum), sizeof(int));
	 int trUID = itTr->ID(); 
         fFOutCompact.write(reinterpret_cast<char*>(&trUID), sizeof(int)); // Additional word, compare to first version in SSDAlign3DUVAlgo1Add.cxx
	 
         double stripInfo[2];
         for (size_t kSt=0; kSt != 8; kSt++) { //Phase1b
	   stripInfo[0] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsX[kSt]->WgtAvgStrip(); 
	   stripInfo[1] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsX[kSt]->WgtRmsStrip();
           fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   if (fDebugIsOn) std::cerr << " X view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
         }
         for (size_t kSt=0; kSt != 8; kSt++) {//Phase1b
	   stripInfo[0] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsY[kSt]->WgtAvgStrip(); 
	   stripInfo[1] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsY[kSt]->WgtRmsStrip();
           fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   if (fDebugIsOn) std::cerr << " Y view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
         }
         for (size_t kSt=0; kSt != 2; kSt++) {
	   stripInfo[0] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsU[kSt]->WgtAvgStrip(); 
	   stripInfo[1] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsU[kSt]->WgtRmsStrip();
           fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   if (fDebugIsOn) std::cerr << " U view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
          }
         for (size_t kSt=0; kSt != 4; kSt++) {
	   stripInfo[0] = (mySSDClsPtrsV[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsV[kSt]->WgtAvgStrip(); 
	   stripInfo[1] = (mySSDClsPtrsV[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsV[kSt]->WgtRmsStrip();
           fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	   if (fDebugIsOn) std::cerr << " W view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
         }
       } // On tracks 
     } // end of dumpCompactEvt
     // 
     bool SSDRecBrickTracksAlgo1::isClusterIsolated(std::vector<rb::SSDCluster>::const_iterator itClSel, 
	                        const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr, double nStripCut) {
	 int numNeighbors = 0; // includes itself... 
	 const double stripNumSel = itClSel->WgtAvgStrip();			
	 for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	   if ((itCl->Station() != itClSel->Station()) || (itCl->Sensor() != itClSel->Sensor())) continue; 
	   const double stripNumTmp = itCl->WgtAvgStrip();
	   if (std::abs(stripNumTmp - stripNumSel) > nStripCut) continue; // too far away, won't bother us.. 
	   numNeighbors++;
	 } // on clusters, any of them 
	 if (numNeighbors == 0) return false; // should not happen
	 return (numNeighbors == 1); 			
     }
     
  } // namespace ssdr
} // namespace emph
