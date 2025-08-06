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
#include "SSDReco/SSDRecDwnstrTracksAutre.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"

namespace emph { 
  namespace ssdr {
  
    const double SSDRecDwnstrTracksAutre::fSqrt2 = std::sqrt(2.0);
    const double SSDRecDwnstrTracksAutre::fOneOverSqrt2 = std::sqrt(0.5); 
				
    SSDRecDwnstrTracksAutre::SSDRecDwnstrTracksAutre() :
      fEmgeo(nullptr),
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fCoordConvert('A'),
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0),
      fNEvents(0), fDebugIsOn(false), fIsMC(false), fIsGoodForAlignment(false),
      fDoMigrad(true), fNoMagnet(false), fChiSqCut(5.0), 
      fPrelimMomentum(50.0), fChiSqCutPreArb(1000.), 
      fDoUseUpstreamTrack(false), 
      fTokenJob("undef"), fFitterFCN(nullptr), fUpDownFitterFCN(nullptr), fFitterKlmFCN(nullptr), 
      fInputSt2Pts(2), fInputSt3Pts(3), fInputSt4Pts(4), fInputSt5Pts(5), fInputSt6Pts(6), fDataItClForFits(),
      fTrs(), fUpStrDwnStrTrack()
      {
      ;
    }
    // 
    SSDRecDwnstrTracksAutre::~SSDRecDwnstrTracksAutre() {
      if (fFOutTrs.is_open()) fFOutTrs.close();
      if (fFOutCompact.is_open()) fFOutCompact.close();
      if (fFOutCmpBeamTracks.is_open()) fFOutCmpBeamTracks.close();
      if (fFOutCompactInfo.is_open()) fFOutCompactInfo.close();
      delete fFitterFCN;
      delete fUpDownFitterFCN;
    }
    //
    size_t SSDRecDwnstrTracksAutre::RecStation(size_t kSt, const art::Event &evt, 
                                const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
       fRunNum = evt.run(); fSubRunNum = evt.subRun(); fEvtNum = evt.id().event();
       if (kSt == 2) { fIsGoodForAlignment = false; fIsPerfectForAlignment = false;}
       if (kSt == 4) fHasUniqueYWSt4 = false;
       if (fDebugIsOn) std::cerr << " SSDRecDwnstrTracksAutre::RecStation, spill " << fSubRunNum << " evt " << fEvtNum << " Station " << kSt << std::endl;
       if (fEmgeo == nullptr) {
         fEmgeo = fGeoService->Geo();
         fNoMagnet = fEmgeo->MagnetUSZPos() < 0.;
	 // We use the nominal Z position, for now.. 
	 if (fDoUseUpstreamTrack && (kSt > 3) ) {
	   fInputSt2Pts.SetXYWindowWidth( 15.0, 15.0); 
	   fInputSt3Pts.SetXYWindowWidth( 15.0, 15.);
	   fInputSt4Pts.SetXYWindowWidth( 25.0, 25.0); 
	   fInputSt5Pts.SetXYWindowWidth( 25.0, 25.);
	   fInputSt6Pts.SetXYWindowWidth( 25.0, 25.);
	   if (fRunNum > 1999) {  // For wide 120 GeV beam... 
	     fInputSt2Pts.SetXYWindowWidth( 30.0, 30.0); 
	     fInputSt3Pts.SetXYWindowWidth( 30.0, 30.);
	     fInputSt4Pts.SetXYWindowWidth( 45.0, 45.0); 
	     fInputSt5Pts.SetXYWindowWidth( 45.0, 45.);
	     fInputSt6Pts.SetXYWindowWidth( 45.0, 45.);
	   }
	   if (fDebugIsOn) std::cerr << " XYWindows all Dwn station are set, as well as chiSq ... " << std::endl;
	 }
       }
       if (!fCoordConvert.IsReadyToGo()) {
	 std::vector<double> zPosStations;
	 std::cerr << " SSDRecStationDwnstrAutre::RecIt Storing Z Positions ..." << std::endl; 
	 const size_t maxKnst = (fRunNum < 2000) ? 6 : 7;
	 for (size_t kSt=0; kSt != maxKnst; kSt++) {
	   TVector3 tmpPos = fEmgeo->GetSSDStation(kSt)->Pos(); 
	   double zz = tmpPos[2];
	   zPosStations.push_back(zz); 
	   if (kSt > 3) zPosStations.push_back(zz); 
	   std::cerr << " ..... Z at station " << kSt << " index " << zPosStations.size() -1 
	             << " is " << zPosStations[zPosStations.size() -1]  << std::endl;
	 }
	 fCoordConvert.InitializeAllCoords(zPosStations); 
       }
       double xAtStation  = 0.; double yAtStation  = 0.; 
       if (fDoUseUpstreamTrack && (kSt > 3)) {  
         TVector3 tmpPos0 = fEmgeo->GetSSDStation(0)->Pos(); 
         const double zzSt0 = tmpPos0[2];
         TVector3 tmpPoskSt = fEmgeo->GetSSDStation(kSt)->Pos(); 
         const double zzStThis = tmpPoskSt[2];
         double dzSt0ForWindow = zzStThis - zzSt0;
	 xAtStation = fItUpstrTr->XOffset() + dzSt0ForWindow * fItUpstrTr->XSlope();
	 yAtStation = fItUpstrTr->YOffset() + dzSt0ForWindow * fItUpstrTr->YSlope();
	 if (fDebugIsOn) std::cerr << " .....  From Upstream track, xAtStation.. " << xAtStation << " yAtStation " << std::endl;
        }
	switch (kSt) {
	  case 2 : { 
	   
	     fInputSt2Pts.SetDebugOn(fDebugIsOn);
	     if (fDoUseUpstreamTrack) fInputSt2Pts.SetXYWindowCenter( xAtStation, yAtStation);
	     return fInputSt2Pts.RecIt(evt, aSSDClsPtr); 
	    }
	  case 3 : { 
	     fInputSt3Pts.SetDebugOn(fDebugIsOn);
	     if (fDoUseUpstreamTrack) fInputSt3Pts.SetXYWindowCenter( xAtStation, yAtStation);
	     return fInputSt3Pts.RecIt(evt, aSSDClsPtr); 
	  }
	  case 4 : { 
	     fInputSt4Pts.SetDebugOn(fDebugIsOn);
//	     if (fDoUseUpstreamTrack) fInputSt4Pts.SetXYWindowCenter( xAtStation, yAtStation);
	    size_t nSt4 = fInputSt4Pts.RecIt(evt, aSSDClsPtr);
	    fHasUniqueYWSt4 = fInputSt4Pts.HasUniqueYW(aSSDClsPtr); 
	    return nSt4;
	  }
	  case 5 : { 
	     fInputSt5Pts.SetDebugOn(fDebugIsOn);
//	     if (fDoUseUpstreamTrack) fInputSt4Pts.SetXYWindowCenter( xAtStation, yAtStation);
	     return fInputSt5Pts.RecIt(evt, aSSDClsPtr); 
	  }
	  case 6 : { 
	     fInputSt6Pts.SetDebugOn(fDebugIsOn);
//	     if (fDoUseUpstreamTrack) fInputSt4Pts.SetXYWindowCenter( xAtStation, yAtStation);
	     return fInputSt6Pts.RecIt(evt, aSSDClsPtr); 
	  }
	  default : {return 0; } // should not happen 
	}
				
    }
    // 
    size_t SSDRecDwnstrTracksAutre::RecAndFitIt(const art::Event &evt) {
    
      fRunNum = evt.run(); fSubRunNum = evt.subRun(); fEvtNum = evt.id().event();
      fDataItClForFits.clear();
      // Assume we have a single Station 0 to Station 1 single track, clean.. 
      const bool atLeastSt2or3 = (((fInputSt2Pts.Size() > 0) && (fInputSt2Pts.NumTriplets() < 2)) || 
                                  ((fInputSt3Pts.Size() > 0) && (fInputSt3Pts.NumTriplets() < 2)));
      const bool atLeastSt4or5 = (((fInputSt4Pts.Size() > 0) && (fInputSt4Pts.NumTriplets() < 2)) || 
                                  ((fInputSt5Pts.Size() > 0) && (fInputSt5Pts.NumTriplets() < 2)));
      const bool atLeastSt5or6 = (((fInputSt5Pts.Size() > 0) && (fInputSt5Pts.NumTriplets() < 2)) || 
                                  ((fInputSt6Pts.Size() > 0) && (fInputSt6Pts.NumTriplets() < 2)));
				   
      if (fRunNum < 2000) fIsGoodForAlignment = atLeastSt2or3 && atLeastSt4or5;
      else  { 
         fIsGoodForAlignment = atLeastSt2or3 && atLeastSt5or6 && (fInputSt4Pts.Size() < 2); 
	 fIsPerfectForAlignment = fIsGoodForAlignment; 
      } 
      if (fRunNum < 2000) { 
        fIsPerfectForAlignment = ((fInputSt2Pts.Size() == 1) && (fInputSt2Pts.NumTriplets() == 1) &&
                             (fInputSt3Pts.Size() == 1) && (fInputSt3Pts.NumTriplets() == 1) &&
			     (fInputSt4Pts.Size() == 1) && (fInputSt4Pts.NumTriplets() == 1) &&
			     (fInputSt5Pts.Size() == 1) && (fInputSt5Pts.NumTriplets() == 1) );  //  NoTgt31Gev_ClSept_A2e_6a   
       // November 11: attempt to recover the missing track at x0 <~ -10. mm due to the loss of 3 read chip on X view station 4. 
        fIsPerfectForAlignment = ((fInputSt2Pts.Size() == 1) && (fInputSt2Pts.NumTriplets() == 1) &&
                             (fInputSt3Pts.Size() == 1) && (fInputSt3Pts.NumTriplets() == 1) &&
			     (fHasUniqueYWSt4) &&
			     (fInputSt5Pts.Size() == 1) && (fInputSt5Pts.NumTriplets() == 1) );  //  NoTgt31Gev_ClSept_A2e_7a   
      } 
      if (fRunNum > 1999) {
        if (fDebugIsOn)  {
	  std::cerr << "  SSDRecDwnstrTracksAutre::RecAndFitIt "; 
	  if (fIsPerfectForAlignment) std::cerr << "  .. O.K. good for alignment " << std::endl;
	  else {
	     std::cerr << "  .. Not good for alignment " << std::endl;
	     std::cerr << "   ... because, for station 2, size " << fInputSt2Pts.Size() 
	               << " numTriplets " << fInputSt2Pts.NumTriplets() << std::endl 
		       << " , for station 3 " << fInputSt3Pts.Size() << "/" << fInputSt3Pts.NumTriplets() 
		       << " , for station 5 " << fInputSt5Pts.Size() << "/" << fInputSt5Pts.NumTriplets() 
		       << " , for station 6 " << fInputSt6Pts.Size() << "/" << fInputSt6Pts.NumTriplets() 
	               << std::endl;
	     if (atLeastSt2or3) std::cerr << " ... atLeastSt2or3 OK " << std::endl;            
	     if (atLeastSt5or6) std::cerr << " ... atLeastSt5or6 OK " << std::endl;
	     std::cerr << std::endl;            
	  }
//	  std::cerr << "  Fits are Not ready for prime time.." << std::endl;
	}
//	return 0;  
      }
      if (fRunNum < 2000) return 0; // If need be, we will restore Phase1b. At a later stage..     

      // One need a fitter. 
      if (fFitterFCN == nullptr) {
        fFitterFCN = new SSDDwnstrTrackFitFCNAutre(fRunNum);
        fFitterKlmFCN = new SSD3DTrackKlmFitFCNAutre(fRunNum);
	fFitterFCN->SetMCFlag(fIsMC);
	fFitterFCN->SetMaxDwnstrStation(fMaxDwnstrStation);
	fFitterKlmFCN->SetMaxDwnstrStation(fMaxDwnstrStation);
      } else {
        if (fFitterFCN->RunNumber() != fRunNum) {
	  delete fFitterFCN;
	  fFitterFCN = new SSDDwnstrTrackFitFCNAutre(fRunNum);
	  delete fFitterKlmFCN;
          fFitterKlmFCN = new SSD3DTrackKlmFitFCNAutre(fRunNum);
	}
      }
      if (fDebugIsOn) std::cerr << " SSDRecDwnstrTracksAutre::RecIt, starting on spill " 
                                 << fSubRunNum << " evt " << fEvtNum << std::endl;
      if (fEmgeo == nullptr) {
         fEmgeo = fGeoService->Geo();
         fNoMagnet = fEmgeo->MagnetUSZPos() < 0.;
      }   
      fTrs.clear(); fTrsKlm.clear();
      if ((fInputSt2Pts.Size() == 0) || (fInputSt3Pts.Size() == 0)) {
        if (fDebugIsOn) std::cerr << " ...  No data from station 2 or 3, Pointers are null, so, no tracks.. " << std::endl;
	return 0; 
      }
      fInputSt2Pts.ResetUsage(); fInputSt3Pts.ResetUsage(); fInputSt4Pts.ResetUsage();  fInputSt5Pts.ResetUsage(); fInputSt6Pts.ResetUsage(); 
      if ((fInputSt4Pts.Size() > 0) && (fInputSt5Pts.Size() > 0) && (fInputSt6Pts.Size() > 0) ) {
         size_t n23456 = this->RecAndFitAll4Stations(); // misnomer for Phase1c, this is in fact a five station (allowing one missing hit for station with 3 views
	 if (fDebugIsOn) std:: cerr << " Back to SSDRecStationDwnstrAutre::RecIt, 5-station number of tracks " << fTrs.size() << std::endl;
      }
       
      size_t n2356 = this->RecAndFitStation2356();
      if (fDebugIsOn) std:: cerr << " Back to SSDRecStationDwnstrAutre::RecIt, num  2356 " << n2356 << " total " << fTrs.size() << std::endl;
      size_t n2345 = this->RecAndFitStation2345();
      if (fDebugIsOn) std:: cerr << " Back to SSDRecStationDwnstrAutre::RecIt, num  2345 " << n2345 << " total " << fTrs.size() << std::endl;
//
// Obsolete, phase1b  
//
      if (fRunNum < 2000) {   
        if (fInputSt5Pts.Size()) {
//        std::cerr << " Encountered spill/event " << fSubRunNum << "/" << fEvtNum << " with no Space Points in Station 5 " << std::endl;
          if (fInputSt4Pts.Size() > 0) {
	     return this->RecAndFitStation234();
	  } else {
	     if (fDebugIsOn) std::cerr << " ...  No data from station 4 and 5, so, no tracks.. " << std::endl;
	     return 0; 
	  }
        }
      } else {
        return fTrs.size(); 
      }
      if (fDebugIsOn) std::cerr << " ... Confusing case, check logic, meanwhile, no tracks.. " << std::endl;  
      return 0; // Should not reach here.. 
    } //RecAndFitIt
    //
    size_t SSDRecDwnstrTracksAutre::RecAndFitAll4Stations() {
    
      const double multScattErrStation2 = fCoordConvert.GetMultScatUncert120(2);
      const double multScattErrStation3 = fCoordConvert.GetMultScatUncert120(3);
      const double multScattErrStation4 = fCoordConvert.GetMultScatUncert120(4);
      const double multScattErrStation5 = fCoordConvert.GetMultScatUncert120(5);
      const double multScattErrStation6 = fCoordConvert.GetMultScatUncert120(6);
    
      const double dz26 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 6) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      if (fDebugIsOn) {
         std::cerr << " " <<  std::endl 
	          <<  " Starting on RecAndFitAll4Stations, evt " << fEvtNum << " dz26 = " << dz26 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, "  
			       << fInputSt4Pts.Size() << " St4 Pts,  " << fInputSt5Pts.Size() << " St5 Pts, " 
			       << fInputSt6Pts.Size() << " St6 Pts " << std::endl;
	 std::cerr << " Dump of XY position for Station 2 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++)  
	   std::cerr << " ID " << itSt2->ID() << " X = " << itSt2->X() << " Y " << itSt2->Y() << " ChiSq " << itSt2->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 3 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++)  
	   std::cerr << " ID " << itSt3->ID() << " X = " << itSt3->X() << " Y " << itSt3->Y() << " ChiSq " << itSt3->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 4 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++)  
	   std::cerr << " ID " << itSt4->ID() << " X = " << itSt4->X() << " Y " << itSt4->Y() << std::endl;
	 std::cerr << " Dump of XY position for Station 5 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++)  
	   std::cerr << " ID " << itSt5->ID() << " X = " << itSt5->X() << " Y " << itSt5->Y() << " ChiSq " << itSt5->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 6 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt6 = fInputSt6Pts.CBegin(); itSt6 != fInputSt6Pts.CEnd(); itSt6++)  
	   std::cerr << " ID " << itSt6->ID() << " X = " << itSt6->X() << " Y " << itSt6->Y() << " ChiSq " << itSt6->ChiSq() << std::endl;
	 std::cerr << " Start Looping .. " << std::endl;  		       
      }
      std::vector<myItStPt> dataInKlm;
      int nComb = 0;	       
      for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
        const double x2Start = itSt2->X(); 
        const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
        if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
        for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
          if (itSt3->UserFlag() != 0) continue;
 	  if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
   
          for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++) { 
            if (itSt4->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... At station 4, at x = " << itSt4->X() << " Y " << itSt4->Y() << std::endl; 
    
            for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++) {
              if (itSt5->UserFlag() != 0) continue;
  	      if (fDebugIsOn) std::cerr << " ... ... ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
	      
              for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt6 = fInputSt6Pts.CBegin(); itSt6 != fInputSt6Pts.CEnd(); itSt6++) {
	        nComb++;
		if (nComb > 10000) {
		  std::cerr << " Starting on RecAndFitAll4Stations, evt " << fEvtNum <<  " spill " << fSubRunNum 
		            << " humongous combinatiroc, gave up " << std::endl;
		  fTrs.clear(); return 0;
		}
                if (itSt5->UserFlag() != 0) continue;
  	        if (fDebugIsOn) std::cerr << " ... ... ... ... ... At station 6, at x = " << itSt6->X() << " Y " << itSt6->Y() << std::endl; 
                const double x6End = itSt6->X();
                const double y6End = itSt6->Y();// To estimate starting value for the track param fit.
	        const double xSlopeStart =  (x6End - x2Start)/dz26; const double ySlopeStart =  (y6End - y2Start)/dz26;
                fFitterFCN->ResetInputPts();
	        fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	        fFitterFCN->AddInputPt(itSt3);
	        fFitterFCN->AddInputPt(itSt4);
	        fFitterFCN->AddInputPt(itSt5);
	        fFitterFCN->AddInputPt(itSt6);
//	        if (this->doPrelimFit(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
                const double fPrelimFitMomPlus = std::abs(fPrelimMomentum); const double fPrelimFitMomMinus = -1.0*std::abs(fPrelimMomentum);
                if (fDebugIsOn) std::cerr << " ... ... ... ... ... Now doing prelim fit assuming positive momentum of " << fPrelimFitMomPlus << std::endl;
                bool goodPrelimPlus = 
	           this->doFitAndStore(rb::FIVESTATIONDWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMomPlus, false);
                if (fDebugIsOn && (!goodPrelimPlus)) std::cerr << " ... ... ... ... ... .. This Prelim fit failed "  << std::endl;
                if (fDebugIsOn && (goodPrelimPlus)) std::cerr << " ... ... ... ... ... .. This Prelim fit suceeded, p "  << fPrelimFitMom << std::endl;
		const double  prelimFitMomPlus  = std::max(fPrelimFitMom, 2.0); // The multiple diverges..  
		const double prelimChiSqPlus = fPrelimFitChiSq;
                if (fDebugIsOn) std::cerr << " ... ... ... ... ... Now doing prelim fit assuming negative momentum of " << fPrelimFitMomMinus << std::endl;
                bool goodPrelimMinus = 
	           this->doFitAndStore(rb::FIVESTATIONDWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMomMinus, false); 
                if (fDebugIsOn && (!goodPrelimMinus)) std::cerr << " ... ... ... ... ... .. This Prelim fit failed "  << std::endl;
                if (fDebugIsOn && (goodPrelimMinus)) std::cerr << " ... ... ... ... ... .. This Prelim fit suceeded, p "  << fPrelimFitMom << std::endl;
		const double  prelimFitMomMinus  = -1.0*std::max(std::abs(fPrelimFitMom), 3.0); 
		const double prelimChiSqMinus = fPrelimFitChiSq;
		if (goodPrelimPlus || goodPrelimMinus) {
		  if (goodPrelimPlus && (!goodPrelimMinus)) fPrelimFitMom = prelimFitMomPlus;
		  if ((!goodPrelimPlus) && goodPrelimMinus) fPrelimFitMom = prelimFitMomMinus;
		  if (goodPrelimPlus && goodPrelimMinus) {
		    if (prelimChiSqPlus < prelimChiSqMinus) { fPrelimFitMom = prelimFitMomPlus; } 
		    else { fPrelimFitMom = prelimFitMomMinus; }
		  }
		  if (std::abs(fPrelimFitMom) < 0.5) continue; // For now!!!   We are not interested in low momentum tracks.. 
                  fFitterFCN->ResetInputPts();
	          itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimMomentum, fPrelimFitMom); 
		  itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimMomentum, fPrelimFitMom);
	          itSt4->ReScaleMultUncert(multScattErrStation4, fPrelimMomentum, fPrelimFitMom); 
		  itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimMomentum, fPrelimFitMom);
		  itSt6->ReScaleMultUncert(multScattErrStation6, fPrelimMomentum, fPrelimFitMom);
  	          if (fDebugIsOn) {
	            std::cerr << " ... ... ... At station 6, after Prelim fit, fPrelimMomentum " 
	                 << fPrelimMomentum << " fPrelimFitMom " << fPrelimFitMom << " XErr " 
			 << itSt5->XErr() << " YErr " << itSt5->YErr() << std::endl;
	          } 
	          fFitterFCN->AddInputPt(itSt2); fFitterFCN->AddInputPt(itSt3); // copy again.. 
	          fFitterFCN->AddInputPt(itSt4); fFitterFCN->AddInputPt(itSt5); fFitterFCN->AddInputPt(itSt6);
                  if (fDebugIsOn) std::cerr << " ... ... ... ... ... Now doing final fit with starting momentum  " << fPrelimFitMom << std::endl;
 	          if (this->doFitAndStore(rb::FIVESTATIONDWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMom, true)) {
		    if (fTrs.rbegin()->ChiSq() < fChiSqCutPreArb) { 
	              int kTrCnt = static_cast<int>(fTrs.size());
  	              if (fDebugIsOn) std::cerr << " ... ... ... ...  ... Setting Used flags on Space Point, kTrCnt " <<  kTrCnt 
		      << " St2 ID " << itSt2->ID() << " St3 " <<  itSt3->ID() << " St4 " 
		      <<  itSt4->ID() << " St5 " <<  itSt5->ID() <<  " St6 " <<  itSt6->ID()  << std::endl; 
		      itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
		      itSt4->SetUserFlag(kTrCnt); itSt5->SetUserFlag(kTrCnt); // no chi-square arbitration for now.. 
		      itSt6->SetUserFlag(kTrCnt);
	            }
	           //
	           // Do the Klm Fits.. 
	           //
		    std::vector<rb::DwnstrTrackAutre>::reverse_iterator itLast = fTrs.rbegin(); 
		    dataInKlm.clear();
		    dataInKlm.push_back(itSt2);  
		    dataInKlm.push_back(itSt3);  
		    dataInKlm.push_back(itSt4);  
		    dataInKlm.push_back(itSt5);  
		    dataInKlm.push_back(itSt6);  
//	            if (fDebugIsOn) { std::cerr << " ... About to call doDwn3DKlmFitAndStore, but I chicken out.. " << std::endl; exit(2); }
	            this->doDwn3DKlmFitAndStore(itLast->ID(), dataInKlm, itLast->Momentum());
		  
		  } //  Good refit...+ storage.
		// One must restore the uncertainties... The whole thing is ugly.. 
	          itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimFitMom, fPrelimMomentum); 
		  itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimFitMom, fPrelimMomentum);
	          itSt4->ReScaleMultUncert(multScattErrStation4, fPrelimFitMom, fPrelimMomentum); 
		  itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimFitMom, fPrelimMomentum);
		  itSt6->ReScaleMultUncert(multScattErrStation6, fPrelimFitMom, fPrelimMomentum);
                } // Preliminary Fit successfull.. 
              } // on Space Points in Station 6 
            } // on Space Points in Station 5 
          } // on Space Points in Station 4 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAutre::RecAndFitAll4Stations, num Tracks " << fTrs.size() << std::endl;;
      return fTrs.size(); 
    }
    size_t SSDRecDwnstrTracksAutre::RecAndFitStation2356() {
    
      const double multScattErrStation2 = fCoordConvert.GetMultScatUncert120(2);
      const double multScattErrStation3 = fCoordConvert.GetMultScatUncert120(3);
      const double multScattErrStation5 = fCoordConvert.GetMultScatUncert120(5);
      const double multScattErrStation6 = fCoordConvert.GetMultScatUncert120(6);
    
      const double dz26 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 6) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      if (fDebugIsOn) {
         std::cerr << " " <<  std::endl 
	          <<  " Starting on RecAndFitStation2356, evt " << fEvtNum << " dz26 = " << dz26 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, "  
			       << fInputSt4Pts.Size() << " St4 Pts,  " << fInputSt5Pts.Size() << " St5 Pts, " 
			       << fInputSt6Pts.Size() << " St6 Pts " << std::endl;
	 std::cerr << " Dump of XY position for Station 2 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++)  
	   std::cerr << " ID " << itSt2->ID() << " X = " << itSt2->X() << " Y " << itSt2->Y() << " ChiSq " << itSt2->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 3 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++)  
	   std::cerr << " ID " << itSt3->ID() << " X = " << itSt3->X() << " Y " << itSt3->Y() << " ChiSq " << itSt3->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 5 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++)  
	   std::cerr << " ID " << itSt5->ID() << " X = " << itSt5->X() << " Y " << itSt5->Y() << " ChiSq " << itSt5->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 6 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt6 = fInputSt6Pts.CBegin(); itSt6 != fInputSt6Pts.CEnd(); itSt6++)  
	   std::cerr << " ID " << itSt6->ID() << " X = " << itSt6->X() << " Y " << itSt6->Y() << " ChiSq " << itSt6->ChiSq() << std::endl;
	 std::cerr << " Start Looping .. " << std::endl;  		       
      }
      int nComb = 0;	       
      for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
        const double x2Start = itSt2->X(); 
        const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
        if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
        for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
          if (itSt3->UserFlag() != 0) continue;
 	  if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
          for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++) {
            if (itSt5->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
            for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt6 = fInputSt6Pts.CBegin(); itSt6 != fInputSt6Pts.CEnd(); itSt6++) {
	      nComb++;
	      if (nComb > 10000) {
	        std::cerr << " Starting on RecAndFitStation2356, evt " << fEvtNum <<  " spill " << fSubRunNum 
	        	  << " humongous combinatiroc, gave up " << std::endl;
	        fTrs.clear(); return 0;
	      }
              if (itSt5->UserFlag() != 0) continue;
  	      if (fDebugIsOn) std::cerr << " ... ... ... ... ... At station 6, at x = " << itSt6->X() << " Y " << itSt6->Y() << std::endl; 
              const double x6End = itSt6->X();
              const double y6End = itSt6->Y();// To estimate starting value for the track param fit.
	      const double xSlopeStart =  (x6End - x2Start)/dz26; const double ySlopeStart =  (y6End - y2Start)/dz26;
              fFitterFCN->ResetInputPts();
	      fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	      fFitterFCN->AddInputPt(itSt3);
	      fFitterFCN->AddInputPt(itSt5);
	      fFitterFCN->AddInputPt(itSt6);
//	      if (this->doPrelimFit(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
              const double fPrelimFitMomPlus = std::abs(fPrelimMomentum); const double fPrelimFitMomMinus = -1.0*std::abs(fPrelimMomentum);
              bool goodPrelimPlus = 
	  	 this->doFitAndStore(rb::STATION2356DWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMomPlus, false);
	      const double  prelimFitMomPlus  = fPrelimFitMom; 
	      const double prelimChiSqPlus = fPrelimFitChiSq;
              bool goodPrelimMinus = 
	  	 this->doFitAndStore(rb::STATION2356DWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMomMinus, false); 
	      const double  prelimFitMomMinus  = fPrelimFitMom; 
	      const double prelimChiSqMinus = fPrelimFitChiSq;
	      if (goodPrelimPlus || goodPrelimMinus) {
	        if (goodPrelimPlus && (!goodPrelimMinus)) fPrelimFitMom = prelimChiSqPlus;
	        if ((!goodPrelimPlus) && goodPrelimMinus) fPrelimFitMom = prelimChiSqMinus;
	        if (goodPrelimPlus && goodPrelimMinus) {
	          if (prelimChiSqPlus < prelimChiSqMinus) { fPrelimFitMom = prelimChiSqPlus; } 
	          else { fPrelimFitMom = prelimChiSqMinus; }
	        }
	        if (std::abs(fPrelimFitMom) < 0.5){  // For now!!!   We are not interested in low momentum tracks.. 
		    if (fDebugIsOn) std::cerr << " ... ... ... ... ... Momentum too low.... skip that one " << std::endl;
		    continue;
		}
          	fFitterFCN->ResetInputPts();
	  	itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimMomentum, fPrelimFitMom); 
	        itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimMomentum, fPrelimFitMom);
	        itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimMomentum, fPrelimFitMom);
	        itSt6->ReScaleMultUncert(multScattErrStation6, fPrelimMomentum, fPrelimFitMom);
  	  	if (fDebugIsOn) {
	  	  std::cerr << " ... ... ... At station 6, after Prelim fit, fPrelimMomentum " 
	  	       << fPrelimMomentum << " fPrelimFitMom " << fPrelimFitMom << " XErr " 
	               << itSt5->XErr() << " YErr " << itSt5->YErr() << std::endl;
	  	} 
	  	fFitterFCN->AddInputPt(itSt2); fFitterFCN->AddInputPt(itSt3); // copy again.. 
	  	fFitterFCN->AddInputPt(itSt5); fFitterFCN->AddInputPt(itSt6);
	  	if (this->doFitAndStore(rb::STATION2356DWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMom, true)) {
	          if (fTrs.rbegin()->ChiSq() < fChiSqCutPreArb) { 
	  	    int kTrCnt = static_cast<int>(fTrs.size());
  	  	    if (fDebugIsOn) std::cerr << " ... ... ... ...  ... Setting Used flags on Space Point, kTrCnt " <<  kTrCnt 
	            << " St2 ID " << itSt2->ID() << " St3 " <<  itSt3->ID() <<  " St5 " <<  itSt5->ID() <<  " St6 " <<  itSt6->ID()  << std::endl; 
	            itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
	            itSt5->SetUserFlag(kTrCnt); // no chi-square arbitration for now.. 
	            itSt6->SetUserFlag(kTrCnt);
	  	  }
	        } //  Good refit...+ storage.
	       //
	       // Do the Klm Fits.. 
	       //
	      // One must restore the uncertainties... The whole thing is ugly.. 
	  	itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimFitMom, fPrelimMomentum); 
	        itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimFitMom, fPrelimMomentum);
	        itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimFitMom, fPrelimMomentum);
	        itSt6->ReScaleMultUncert(multScattErrStation6, fPrelimFitMom, fPrelimMomentum);
	        
              } // Preliminary Fit successfull.. 
            } // on Space Points in Station 6 
          } // on Space Points in Station 5 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAutre::RecAndFitStation2356, num Tracks " << fTrs.size() << std::endl;;
      return fTrs.size(); 
    }
    size_t SSDRecDwnstrTracksAutre::RecAndFitStation2345() {
    
      const double multScattErrStation2 = fCoordConvert.GetMultScatUncert120(2);
      const double multScattErrStation3 = fCoordConvert.GetMultScatUncert120(3);
      const double multScattErrStation4 = fCoordConvert.GetMultScatUncert120(4);
      const double multScattErrStation5 = fCoordConvert.GetMultScatUncert120(5);
    
      const double dz25 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      if (fDebugIsOn) {
         std::cerr << " " <<  std::endl 
	          <<  " Starting on RecAndFitStation2345, evt " << fEvtNum << " dz25 = " << dz25 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, "  
			       << fInputSt4Pts.Size() << " St4 Pts,  " << fInputSt5Pts.Size() << " St5 Pts, " 
			       << fInputSt6Pts.Size() << " St6 Pts " << std::endl;
	 std::cerr << " Dump of XY position for Station 2 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++)  
	   std::cerr << " ID " << itSt2->ID() << " X = " << itSt2->X() << " Y " << itSt2->Y() << " ChiSq " << itSt2->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 3 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++)  
	   std::cerr << " ID " << itSt3->ID() << " X = " << itSt3->X() << " Y " << itSt3->Y() << " ChiSq " << itSt3->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 4 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++)  
	   std::cerr << " ID " << itSt4->ID() << " X = " << itSt4->X() << " Y " << itSt4->Y() << " ChiSq " << itSt4->ChiSq() << std::endl;
	 std::cerr << " Dump of XY position for Station 5 " << std::endl;
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++)  
	   std::cerr << " ID " << itSt5->ID() << " X = " << itSt5->X() << " Y " << itSt5->Y() << " ChiSq " << itSt5->ChiSq() << std::endl;
	 std::cerr << " Start Looping .. " << std::endl;  		       
      }
      int nComb = 0;	       
      for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
        const double x2Start = itSt2->X(); 
        const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
        if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
        for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
          if (itSt3->UserFlag() != 0) continue;
 	  if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
          for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++) {
            if (itSt4->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... ... At station 4, at x = " << itSt4->X() << " Y " << itSt4->Y() << std::endl; 
            for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++) {
	      nComb++;
	      if (nComb > 10000) {
	        std::cerr << " Starting on RecAndFitStation2345, evt " << fEvtNum <<  " spill " << fSubRunNum 
	        	  << " humongous combinatiroc, gave up " << std::endl;
	        fTrs.clear(); return 0;
	      }
              if (itSt5->UserFlag() != 0) continue;
  	      if (fDebugIsOn) std::cerr << " ... ... ... ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
              const double x5End = itSt5->X();
              const double y5End = itSt5->Y();// To estimate starting value for the track param fit.
	      const double xSlopeStart =  (x5End - x2Start)/dz25; const double ySlopeStart =  (y5End - y2Start)/dz25;
              fFitterFCN->ResetInputPts();
	      fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	      fFitterFCN->AddInputPt(itSt3);
	      fFitterFCN->AddInputPt(itSt4);
	      fFitterFCN->AddInputPt(itSt5);
//	      if (this->doPrelimFit(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
              const double fPrelimFitMomPlus = std::abs(fPrelimFitMom); const double fPrelimFitMomMinus = -1.0*std::abs(fPrelimFitMom);
              bool goodPrelimPlus = 
	  	 this->doFitAndStore(rb::STATION2345DWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMomPlus, false);
	      const double  prelimFitMomPlus  = fPrelimFitMom; 
	      const double prelimChiSqPlus = fPrelimFitChiSq;
              bool goodPrelimMinus = 
	  	 this->doFitAndStore(rb::STATION2345DWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMomMinus, false); 
	      const double  prelimFitMomMinus  = fPrelimFitMom; 
	      const double prelimChiSqMinus = fPrelimFitChiSq;
	      if (goodPrelimPlus || goodPrelimMinus) {
	        if (goodPrelimPlus && (!goodPrelimMinus)) fPrelimFitMom = prelimFitMomPlus;
	        if ((!goodPrelimPlus) && goodPrelimMinus) fPrelimFitMom = prelimFitMomMinus;
	        if (goodPrelimPlus && goodPrelimMinus) {
	          if (prelimChiSqPlus < prelimChiSqMinus) { fPrelimFitMom = prelimFitMomPlus; } 
	          else { fPrelimFitMom = prelimFitMomMinus; }
	        }
	        if (std::abs(fPrelimFitMom) < 0.5) {
		    if (fDebugIsOn) std::cerr << " ... ... ... ... ... Momentum too low.... skip that one " << std::endl;
		    continue; // For now!!!   We are not interested in low momentum tracks.. 
		}
          	fFitterFCN->ResetInputPts();
	  	itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimMomentum, fPrelimFitMom); 
	        itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimMomentum, fPrelimFitMom);
	        itSt4->ReScaleMultUncert(multScattErrStation4, fPrelimMomentum, fPrelimFitMom);
	        itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimMomentum, fPrelimFitMom);
  	  	if (fDebugIsOn) {
	  	  std::cerr << " ... ... ... At station 5, after Prelim fit, fPrelimMomentum " 
	  	       << fPrelimMomentum << " fPrelimFitMom " << fPrelimFitMom << " XErr " 
	               << itSt5->XErr() << " YErr " << itSt5->YErr() << std::endl;
	  	} 
	  	fFitterFCN->AddInputPt(itSt2); fFitterFCN->AddInputPt(itSt3); // copy again.. 
	  	fFitterFCN->AddInputPt(itSt5); fFitterFCN->AddInputPt(itSt5);
	  	if (this->doFitAndStore(rb::STATION2345DWN, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMom, true)) {
	          if (fTrs.rbegin()->ChiSq() < fChiSqCutPreArb) { 
	  	    int kTrCnt = static_cast<int>(fTrs.size());
  	  	    if (fDebugIsOn) std::cerr << " ... ... ... ...  ... Setting Used flags on Space Point, kTrCnt " <<  kTrCnt 
	            << " St2 ID " << itSt2->ID() << " St3 " <<  itSt3->ID() << " St4 " 
	            <<  itSt4->ID() << " St5 " <<  itSt5->ID() <<  " St5 " <<  itSt5->ID()  << std::endl; 
	            itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
	            itSt5->SetUserFlag(kTrCnt); // no chi-square arbitration for now.. 
	            itSt5->SetUserFlag(kTrCnt);
	  	  }
	        } //  Good refit...+ storage.
	       //
	       //
	      // One must restore the uncertainties... The whole thing is ugly.. 
	  	itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimFitMom, fPrelimMomentum); 
	        itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimFitMom, fPrelimMomentum);
	        itSt4->ReScaleMultUncert(multScattErrStation4, fPrelimFitMom, fPrelimMomentum);
	        itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimFitMom, fPrelimMomentum);
	        
              } // Preliminary Fit successfull.. 
            } // on Space Points in Station 5 
          } // on Space Points in Station 4 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAutre::RecAndFitStation2345, num Tracks " << fTrs.size() << std::endl;;
      return fTrs.size(); 
    }
    //
     size_t SSDRecDwnstrTracksAutre::RecAndFitStation234() {
    
     size_t nTrBefore = fTrs.size(); 
     const double multScattErrStation2 = fCoordConvert.GetMultScatUncert120(2);
     const double multScattErrStation3 = fCoordConvert.GetMultScatUncert120(3);
     const double multScattErrStation4 = fCoordConvert.GetMultScatUncert120(4);
     const double dz24 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
     if (fDebugIsOn) std::cerr << " Starting on RecAndFit,Station234 dz24 = " << dz24 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, "  
			       << fInputSt4Pts.Size() << " St4 Pts,  " << std::endl;
     for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
       const double x2Start = itSt2->X(); 
       const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
       if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
       
       for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
         if (itSt3->UserFlag() != 0) continue;
 	 if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
    
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++) { 
           if (itSt4->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... At station 4, at x = " << itSt4->X() << " Y " << itSt4->Y() << std::endl; 
           const double x4End = itSt4->X();
           const double y4End = itSt4->Y();// To estimate starting value for the track param fit.
	   const double xSlopeStart =  (x4End - x2Start)/dz24; const double ySlopeStart =  (y4End - y2Start)/dz24;
           fFitterFCN->ResetInputPts();
	   fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	   fFitterFCN->AddInputPt(itSt3);
	   fFitterFCN->AddInputPt(itSt4);
	   if (this->doPrelimFit(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
	     if (fPrelimFitChiSq > 500.) continue; // To be tuned for real data, O.K. for MC. Bogus track.. 
	     if (std::abs(fPrelimFitMom) < 1.0) continue; // For now!!!   We are interested in low momentum tracks.. 
             fFitterFCN->ResetInputPts();
	     itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimMomentum, fPrelimFitMom); 
	     itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimMomentum, fPrelimFitMom);
	     itSt4->ReScaleMultUncert(multScattErrStation4, fPrelimMomentum, fPrelimFitMom); 
	     fFitterFCN->AddInputPt(itSt2); fFitterFCN->AddInputPt(itSt3);
	     fFitterFCN->AddInputPt(itSt4); 
	     if (this->doFitAndStore(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMom)) {
	       if (fTrs.rbegin()->ChiSq() < fChiSqCutPreArb) { 
	   	 int kTrCnt = static_cast<int>(fTrs.size());
	     	 itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
	     	 itSt4->SetUserFlag(kTrCnt); // no chi-square arbitration for now.. 
  	            if (fDebugIsOn) std::cerr << " ... ... ... ...  ... Setting Used flags on Space Point, kTrCnt " <<  kTrCnt 
		    << " St2 ID " << itSt2->ID() << " St3 " <<  itSt3->ID() << " St4 " <<  itSt4->ID() <<  std::endl; 
	       } 
	     }
		// One must restore the uncertainties... The whole thing is ugly.. 
	     itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimFitMom, fPrelimMomentum); 
	     itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimFitMom, fPrelimMomentum);
	     itSt4->ReScaleMultUncert(multScattErrStation4, fPrelimFitMom, fPrelimMomentum); 	     
           } // Preliminary Fit successfull.. 
          } // on Space Points in Station 4 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2 
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAutre::RecAndFitStation234, num Tracks " << fTrs.size() << std::endl;;
      return ( fTrs.size() - nTrBefore);
    }
    
    //  Actually, extend to station 6... No longer works for Phase1c 
    
    size_t SSDRecDwnstrTracksAutre::RecAndFitStation235() {
    
      size_t nTrBefore = fTrs.size(); 
      
      const double multScattErrStation2 = fCoordConvert.GetMultScatUncert120(2);
      const double multScattErrStation3 = fCoordConvert.GetMultScatUncert120(3);
      const double multScattErrStation5 = fCoordConvert.GetMultScatUncert120(5);
      const double multScattErrStation6 = fCoordConvert.GetMultScatUncert120(6);
      const double dz26 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 6) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      if (fDebugIsOn) std::cerr << " Starting on RecAndFit,Station235 dz26 = " << dz26 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, " 
			       << fInputSt5Pts.Size() << " St5 Pts,  " << std::endl;
      for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
        const double x2Start = itSt2->X(); 
        const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
        if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
       
        for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
         if (itSt3->UserFlag() != 0) continue;
 	 if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
	 
         for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++) { 
           if (itSt5->UserFlag() != 0) continue;
 	   if (fDebugIsOn) std::cerr << " ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
           for (std::vector<rb::SSDStationPtAutre>::const_iterator itSt6 = fInputSt6Pts.CBegin(); itSt6 != fInputSt6Pts.CEnd(); itSt6++) { 
             if (itSt6->UserFlag() != 0) continue;
 	     if (fDebugIsOn) std::cerr << " ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
             const double x5End = itSt5->X();
             const double y5End = itSt5->Y();// To estimate starting value for the track param fit.
	     const double xSlopeStart =  (x5End - x2Start)/dz26; const double ySlopeStart =  (y5End - y2Start)/dz26;
             fFitterFCN->ResetInputPts();
	     fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	     fFitterFCN->AddInputPt(itSt3);
	     fFitterFCN->AddInputPt(itSt5);
	     fFitterFCN->AddInputPt(itSt6);
	     if (this->doPrelimFit(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
	     if (fPrelimFitChiSq > 500.) continue; // To be tuned for real data, O.K. for MC. Bogus track.. 
	     if (std::abs(fPrelimFitMom) < 1.0) continue; // For now!!!   We are interested in low momentum tracks.. 
              fFitterFCN->ResetInputPts();
	      itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimMomentum, fPrelimFitMom); 
	      itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimMomentum, fPrelimFitMom);
	      itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimMomentum, fPrelimFitMom);
	      itSt6->ReScaleMultUncert(multScattErrStation6, fPrelimMomentum, fPrelimFitMom);
	      if (fDebugIsOn) std::cerr << " ... ... ... At station 6, after Prelim fit, fPrelimMomentum " 
	                 << fPrelimMomentum << " fPrelimFitMom " << fPrelimFitMom << " XErr " 
			 << itSt6->XErr() << " YErr " << itSt6->YErr() << std::endl;
	      fFitterFCN->AddInputPt(itSt2); fFitterFCN->AddInputPt(itSt3);
	      fFitterFCN->AddInputPt(itSt5); fFitterFCN->AddInputPt(itSt6); 
	      if (this->doFitAndStore(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart, fPrelimFitMom)) {
	       if (fTrs.rbegin()->ChiSq() < fChiSqCutPreArb) { 
	   	 int kTrCnt = static_cast<int>(fTrs.size());
	     	 itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
	     	 itSt5->SetUserFlag(kTrCnt); itSt6->SetUserFlag(kTrCnt);
  	            if (fDebugIsOn) std::cerr << " ... ... ... ...  ... Setting Used flags on Space Point, kTrCnt " <<  kTrCnt 
		    << " St2 ID " << itSt2->ID() << " St2 " <<  itSt3->ID() << " St5 " <<  itSt5->ID() <<  " St6 " <<  itSt6->ID() <<std::endl; 
	       } 
	     }
		// One must restore the uncertainties... The whole thing is ugly.. 
	     itSt2->ReScaleMultUncert(multScattErrStation2, fPrelimFitMom, fPrelimMomentum); 
	     itSt3->ReScaleMultUncert(multScattErrStation3, fPrelimFitMom, fPrelimMomentum);
	     itSt5->ReScaleMultUncert(multScattErrStation5, fPrelimFitMom, fPrelimMomentum); 	     
	     itSt6->ReScaleMultUncert(multScattErrStation6, fPrelimFitMom, fPrelimMomentum); 	     
             } // Preliminary Fit successfull.. 
            } // on Space Points in Station 6 
          } // on Space Points in Station 5 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2 
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAutre::RecAndFitStation235, num Tracks " << fTrs.size() << std::endl;
      return ( fTrs.size() - nTrBefore);
    }
    //
    bool SSDRecDwnstrTracksAutre::doPrelimFit(rb::DwnstrTrType type, double xStart, double yStart, double xSlopeStart, double ySlopeStart) {
       fNoMagnet = fEmgeo->MagnetUSZPos() < 0.;
       if ((fRunNum == 2113) || (fRunNum == 2098)) fNoMagnet = true;
       else  fNoMagnet = false;
      fFitterFCN->SetNoMagnet(fNoMagnet);
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsOut, parsOutErr;
//       fFitterFCN->SetDebugOn(fDebugIsOn);
//       fFitterFCN->SetDebugOn((fEvtNum == 5));
       fFitterFCN->SetDebugOn(false);
       fPrelimFitMom = fPrelimMomentum;
       fPrelimFitChiSq = DBL_MAX; 
       if (fDebugIsOn) {
         std::cerr << " SSDRecDwnstrTracksAutre::doPrelimFit, start, X2 = " <<  xStart << " Y2 " << yStart 
	           << " slopes " << xSlopeStart << ", " << ySlopeStart << " pInit " << fPrelimMomentum << std::endl;
	 fFitterFCN->printInputData();	   
       }
       uPars.Add(std::string("X_2"), xStart, 0.1, -20., 20.);  
       uPars.Add(std::string("Slx_2"), xSlopeStart, 0.001, -0.1, 0.1);  // for Misligned 7c. No magnet Misalignament flag 25000025, Dgap = 3.0 mm  
       uPars.Add(std::string("Y_2"), yStart, 0.1, -20., 20.);  
       uPars.Add(std::string("Sly_2"), ySlopeStart, 0.001, -0.1, 0.1); 
//       uPars.Add(std::string("PInv"), 1.0/fPrelimMomentum, 2.0/fPrelimMomentum, -5.0, 5.0); // could flip the sign Min. mometum is 0.1 GeV
// Start with negative momentum, to (usually), observe a change of sign in this parameter (assuming positive beam 
// 
//        if (!fNoMagnet) 
//	  uPars.Add(std::string("PInv"), 1.0/fPrelimMomentum, std::abs(2.0/fPrelimMomentum), -5.0, 5.0); // could flip the sign Min. mometum is 0.1 GeV
       
        if (!fNoMagnet)  // test this.. 
	  uPars.Add(std::string("PInv"), 1.0/50., std::abs(2.0/50.), -0.0080, 0.008); // could flip the sign Min. mometum is 0.1 GeV
       unsigned int nPars = (fNoMagnet) ? 4 : 5; 
       std::vector<double> initValsV, initValsE; // for use in the Simple Minimizer.. 
       for (unsigned int k=0; k != nPars; k++) { initValsV.push_back(uPars.Value(k)); initValsE.push_back(uPars.Error(k)); } 
       // Testing the FCN, once .. 
       if (fDebugIsOn) { 
//         fFitterFCN->SetDebugOn(true); 
         if (fNoMagnet) std::cerr << " ....... About to test the FCN, No Magnet,  with " << initValsV.size() << " parameters.. " << std::endl;
         else std::cerr << " ....... About to test the FCN, Magnet in place,  with " << initValsV.size() << " parameters.. " << std::endl;
         double aChiSqOnce = (*fFitterFCN)(initValsV);
//         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that is enough for now " << std::endl; exit(2);
         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that we keep going... " << std::endl;
       }
       ROOT::Minuit2::MnMigrad migrad((*fFitterFCN), uPars);
       if (fDebugIsOn) std::cerr << " ..... About to call migrad... " << std::endl;
       //
       int flagValid = 0; // 0 nothing worked, 1 MiGrad is O.K., 2, Simplex is Ok.  
       bool isMigradValid = false;
       double chiSq = DBL_MAX;
       try { 
         ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
         if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
         isMigradValid = min.IsValid(); 
       //
//       if (fDebugIsOn) { std::cerr << " .... Migrad done, and quit here and now " << std::endl; exit(2); }
         bool isSimplexValid = true;
         if (!isMigradValid) {
           // 
           // Try again, flip the sign of the momentum.. We should wrap this in a try/catch again.. 
	   //
	   if (!fNoMagnet) { 
             if (fDebugIsOn)
	      std::cerr << " ..... Migrad failed, with the Magnet in place, bad news. Try again, fliiping the sign of the momentum " << std::endl;
	     uPars.SetValue(4, -0.02);
             ROOT::Minuit2::MnMigrad migradOppS((*fFitterFCN), uPars);
             if (fDebugIsOn) std::cerr << " ..... About to call migrad 2nd time ... " << std::endl;
       //
             ROOT::Minuit2::FunctionMinimum minOppS = migradOppS(2000, 0.25); // Relax a bit the EDM 
             if (fDebugIsOn) std::cerr << " Migrad minimum " << minOppS << std::endl; 
       //
             isMigradValid = minOppS.IsValid(); 
	     if (isMigradValid) { 
	       chiSq = minOppS.Fval();
 	       parsOutErr = minOppS.UserParameters().Errors();
 	       parsOut = minOppS.UserParameters().Params();
	       //
	       // if the momentum is negative, flip the sign of the slopes. 
	       //
	       if (parsOut[4] < 0.) { parsOut[1] *= -1.; parsOut[3] *= -1.; }
	       flagValid = 1;
	     } 
	   } else { // Rare case. 
             if (fDebugIsOn) std::cerr << " ...No Magnet,  Migrad fails, Try Simplex.. " << std::endl; 
             ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	     ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize((*fFitterFCN), initValsV, initValsE);
	     if (fDebugIsOn) std::cerr << " Simplex minimum " << minS << std::endl; 
 	     parsOutErr = minS.UserParameters().Errors();
 	     parsOut = minS.UserParameters().Params();
             isSimplexValid = minS.IsValid();
	     chiSq = minS.Fval();
	     if (isSimplexValid) flagValid = 2;
	   }
         } else { // Migrad success, first try.. 
	   chiSq = min.Fval();
 	   parsOutErr = min.UserParameters().Errors();
 	   parsOut = min.UserParameters().Params();
	   flagValid = 1;
         }

         if (flagValid == 0) {
           if (fDebugIsOn) std::cerr << " No valid fit, so, no new tracks " << std::endl;
	   return false;
         }
       } catch (...) { return false; }
       if (fDebugIsOn && (flagValid == 1)) std::cerr <<  " Migrad fit succeeded, chiSq " << chiSq << std::endl;
       if (fDebugIsOn && (flagValid == 2)) std::cerr <<  " Simplex fit succeeded, chiSq " << chiSq << std::endl;
       fPrelimFitMom = 1.0/parsOut[4];
       if (std::abs(fPrelimFitMom) > 125) fPrelimFitMom = (parsOut[4] > 0.) ? 125.0: -125.;
       fPrelimFitChiSq = chiSq;
       if (fDebugIsOn) std::cerr << " Setting the Preliminary momentum to " << fPrelimFitMom << std::endl;
//       if (fEvtNum == 5) { std::cerr << " SSDRecDwnstrTracksAutre::doPrelimFit Event 5, quit here.. " << std::endl; exit(2); }
       return true;
    } // doPrelimFitFit
    //
    bool SSDRecDwnstrTracksAutre::doFitAndStore(rb::DwnstrTrType type, double xStart, double yStart, 
                                                 double xSlopeStart, double ySlopeStart, double pStart, bool storeIt) {
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsOut, parsOutErr;
//       fFitterFCN->SetDebugOn(fDebugIsOn);
//       fFitterFCN->SetDebugOn((fEvtNum == 17));
       fFitterFCN->SetDebugOn(false);
       if (fDebugIsOn) {
         std::cerr << " SSDRecDwnstrTracksAutre::doFitAndStore, start, X2 = " <<  xStart << " Y2 " << yStart 
	           << " slopes " << xSlopeStart << ", " << ySlopeStart << " pInit " << pStart << std::endl;
	 fFitterFCN->printInputData();	   
       }
       uPars.Add(std::string("X_2"), xStart, 0.1, -20., 20.);  
       uPars.Add(std::string("Slx_2"), xSlopeStart, 0.001, -0.1, 0.1);  // for Misligned 7c. No magnet Misalignament flag 25000025, Dgap = 3.0 mm  
       uPars.Add(std::string("Y_2"), yStart, 0.1, -20., 20.);  
       uPars.Add(std::string("Sly_2"), ySlopeStart, 0.001, -0.1, 0.1); 
//       uPars.Add(std::string("PInv"), 1.0/fPrelimMomentum, 2.0/fPrelimMomentum, -5.0, 5.0); // could flip the sign Min. mometum is 0.1 GeV
// Start with negative momentum, to (usually), observe a change of sign in this parameter (assuming positive beam 
// 
//        if (!fNoMagnet) 
//	  uPars.Add(std::string("PInv"), 1.0/fPrelimMomentum, std::abs(2.0/fPrelimMomentum), -5.0, 5.0); // could flip the sign Min. mometum is 0.1 GeV
        if (!fNoMagnet)  // test this.. 
	  uPars.Add(std::string("PInv"), 1.0/pStart, std::abs(2.0/50.), -5.0, 5.0); // could flip the sign Min. mometum is 0.1 GeV
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
       bool isMigradValid = false;
       int flagValid = 0; // 0 nothing worked, 1 MiGrad is O.K., 2, Simplex is Ok.  
       double chiSq = DBL_MAX;
       try {
         ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
         if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
         isMigradValid = min.IsValid(); 
       //
         bool isSimplexValid = false;
         flagValid = 0;  
         if (!isMigradValid) {
	   if (fDebugIsOn) std::cerr << " ... Trying Simplex... " << std::endl;
           ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	   ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize((*fFitterFCN), initValsV, initValsE);
	   if (fDebugIsOn) std::cerr << " Simplex minimum " << min << std::endl;
 	   parsOutErr = minS.UserParameters().Errors();
 	   parsOut = minS.UserParameters().Params();
           isSimplexValid = minS.IsValid();
	   chiSq = minS.Fval();
	   if (isSimplexValid) flagValid = 2;
         } else {
	   chiSq = min.Fval();
           fFitterFCN->SetDebugOn(false);
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
         if (fDebugIsOn) {
	     std::cerr << " ..... Setting debug on in the FCN function dump of the fit prediction/measurements.... . " << std::endl;
             fFitterFCN->SetDebugOn(true);
	     const double chiCheck = (*fFitterFCN)(parsOut);
             fFitterFCN->SetDebugOn(false);
	 }
	 if (!storeIt) {
	  fPrelimFitChiSq = chiSq; 
	  fPrelimFitMom = 1.0/parsOut[4];
	  return true;
	 }   
         rb::DwnstrTrackAutre aTr; 
         aTr.SetType(type); 
         aTr.SetID(static_cast<int>(fTrs.size()));
         if (parsOut.size() == 5) {
	   //
	   // if the momentum is negative, flip the sign of the slopes. 
	   //
	   if (parsOut[4] < 0.) { parsOut[1] *= -1.; parsOut[3] *= -1.; }
           aTr.SetTrParams(parsOut[0], parsOut[1], parsOut[2], parsOut[3], 1.0/parsOut[4]);
           aTr.SetTrParamsErrs(parsOutErr[0], parsOutErr[1], parsOutErr[2], parsOutErr[3], parsOutErr[4]/(parsOut[4]*parsOut[4]));
         } else {
           aTr.Reset(true); // No Magnet = true
           aTr.SetType(type); 
           aTr.SetID(static_cast<int>(fTrs.size()));
           aTr.SetTrParams(parsOut[0], parsOut[1], parsOut[2], parsOut[3], DBL_MAX);
           aTr.SetTrParamsErrs(parsOutErr[0], parsOutErr[1], parsOutErr[2], parsOutErr[3], DBL_MAX);
         }
         aTr.SetChiSq(chiSq);
         if (flagValid == 1 && min.HasCovariance()) {
           for (size_t i=0; i != static_cast<size_t>(nPars); i++) {
             for (size_t j=0; j != static_cast<size_t>(nPars); j++) {
	       const double cc = min.UserCovariance()(i,j);
               aTr.SetCovarianceMatrix((i*nPars + j), cc);
	     }
	   }
         }
         if (!this->IsAlreadyFound(aTr))  { fTrs.push_back(aTr); return true; }
//       } else {
       // else should do arbitration here.. For now, just count track that not exactly the same.. 
       //  this->ArbitrateAndStore(aTr);
//       }
       } catch (...) { return false; }
       return false;
    } // doFitAndStore
    //
    bool SSDRecDwnstrTracksAutre::IsAlreadyFound(const rb::DwnstrTrackAutre &aTr) const {  // this needs tuning.. 
      if (fTrs.size() == 0) return false;
      for (std::vector<rb::DwnstrTrackAutre>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it ++) {
        if ((std::abs(aTr.XSlope() - it->XSlope()) < 1.0e-5) && 
	    (std::abs(aTr.YSlope() - it->YSlope()) < 1.0e-5) && (std::abs(aTr.Momentum() - it->Momentum()) < 0.05 )) return true;   
      }
      return false;
    }
    void SSDRecDwnstrTracksAutre::openOutputCsvFiles() const {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameStrStr;
       fNameStrStr << "SSDRecDwnstrTracksAutre_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutTrs.open(fNameStr.c_str());
       fFOutTrs << " spill evt nPTrs iTr type x2 x2Err xsl xslErr y2 y2Err y2sl y2slErr p pErr chiSq " << std::endl;
       //
       
     }
     //
     void SSDRecDwnstrTracksAutre::dumpInfoForR() const {
       if (fTrs.size() == 0) return;
       if (!fFOutTrs.is_open()) this->openOutputCsvFiles(); 
       std::ostringstream headerStrStr; 
       headerStrStr << " " << fSubRunNum << " " << fEvtNum << " " << fTrs.size(); 
       std::string headerStr(headerStrStr.str());
       size_t k=0;
       for (std::vector<rb::DwnstrTrackAutre>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it++, k++) { 
         fFOutTrs << headerStr << " " << k << " " << it->Type()  
	         << " " << it->XOffset() << " " << it->XOffsetErr() << " " << it->XSlope() << " " << it->XSlopeErr()
		 << " " << it->YOffset() << " " << it->YOffsetErr() << " " << it->YSlope() << " " << it->YSlopeErr()
		 << " " <<  it->Momentum() << " " << it->MomentumErr() << " " << it->ChiSq() << std::endl; 
       }
     }
     // Inspired by from the Brick (silly) exercise
     
     void SSDRecDwnstrTracksAutre::dumpCompactEvt(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr ) {
       if (!fFOutCompact.is_open()) {
         std::ostringstream aFOutCompactStrStr; 
	 if (fRunNum < 2000)  
            aFOutCompactStrStr << "./CompactAutreData_" << fRunNum  << "_" << fTokenJob << "_V1g.dat";
	 else   aFOutCompactStrStr << "./CompactAutreData_" << fRunNum  << "_" << fTokenJob << "_V2a.dat"; 
         std::string aFOutCompactStr(aFOutCompactStrStr.str());
         fFOutCompact.open(aFOutCompactStr.c_str(),  std::ios::binary | std::ios::out);
         std::cerr << " Opening CompactEvt file " << aFOutCompactStr << std::endl; 
         std::ostringstream aFOutCompactStrStr2;
	 if (fRunNum < 2000)  
           aFOutCompactStrStr2 << "./CompactAutreInfo_" << fRunNum  << "_" << fTokenJob << "_V1g.txt";
	 else 
	   aFOutCompactStrStr2 << "./CompactAutreInfo_" << fRunNum  << "_" << fTokenJob << "_V2a.txt"; 
         std::string aFOutCompactStr2(aFOutCompactStrStr2.str());
         fFOutCompactInfo.open(aFOutCompactStr2.c_str());
	 fFOutCompactInfo << " spill evt nTr itr x0 y0 xSl ySl x2 y2 nCl2 x3 y3 nCl3 x4 y4 nCl4 x5 y5 nCl5 ";
	 if (fRunNum > 1999) fFOutCompactInfo << " x6 y6 nCl6 ";
	 fFOutCompactInfo << std::endl;
         std::cerr << " And corresponding ASCII file " << aFOutCompactStr2 << " andkeep going  .. " << std::endl; 
       }
       const double maxClustSep = 5.;
       if (fDebugIsOn) std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, at spill " << fSubRunNum << " evt " << fEvtNum << std::endl;
       int key= (fRunNum < 1999) ? 687503 : 687603; // fixed, distinct from similar code found in SSDAlign3DUVAutreAdd
       int numDouble = (fRunNum < 2000)  ? ( 2 * (2*(8) + 2 + 4)) :
                                           ( 2 * (2*(9) + 2 + 4)) ; // Phase1b for 2 is mean & RMS, then, X + Y + U + V // Phase1b, 22 sensors 
			// Phase1c : add station 4, with X and Y, and still 2 downstream station downstream of the magnet.  Station 7 has no data.  
       // Version 1g:  Consider only the single track events.. And wide angle.. 
        // Version 2a:  Phase1c , Consider only the single track events.. Skip Station 7, which looks like it is empty..  
      int numClReal = 0;
       const double stripAvNone = 0.; const double stripRmsNone = 1.0e9;
       std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsX(8, aSSDClsPtr->cend());  // station 0 to 5, Phase1b
       std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsY(8, aSSDClsPtr->cend());// with two double sensor 
       if (fRunNum < 2000) {
         mySSDClsPtrsX.push_back(aSSDClsPtr->cend());  mySSDClsPtrsX.push_back(aSSDClsPtr->cend());
         mySSDClsPtrsY.push_back(aSSDClsPtr->cend());  mySSDClsPtrsY.push_back(aSSDClsPtr->cend());
       }
       std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsU(2, aSSDClsPtr->cend());// station 2 and 3, 
       std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsW(4, aSSDClsPtr->cend());  // station 4 nd 5, two adjacent sensors.
       // Save the Station 0 and 1 unique pointers; ( A must..)
       std::vector<int> numXCl = {0, 0};  std::vector<int> numYCl = {0, 0}; 
       for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
         if (itCl->View() == emph::geo::X_VIEW) {
	   if (itCl->Station() < 2) { mySSDClsPtrsX[(size_t) itCl->Station()] = itCl; numXCl[(size_t) itCl->Station()]++; }
	 } else if (itCl->View() == emph::geo::Y_VIEW) {
	   if (itCl->Station() < 2) { mySSDClsPtrsY[itCl->Station()] = itCl; numYCl[(size_t) itCl->Station()]++; }
	 }
       }
       for (size_t kk=0; kk !=2; kk++) {
         if ((numXCl[kk] != 1) || (numYCl[kk] != 1)) {
	    if (fDebugIsOn) std::cerr << " .... Ambiguous set of clusters in station " << kk   
	                              << " numX " << numXCl[kk] << " Y " << numYCl[kk] << std::endl; 
	    return;
	 }
       }
       std::vector<std::string> infoPtSts; 
       // Now extract data from Space points.  Consider all combinations.. To be revised, quite possibly.  
      size_t nSp2 = (fInputSt2Pts.Size() == 0) ? 1 : fInputSt2Pts.Size();       
      size_t nSp3 = (fInputSt3Pts.Size() == 0) ? 1 : fInputSt3Pts.Size();       
      size_t nSp4 = (fInputSt4Pts.Size() == 0) ? 1 : fInputSt4Pts.Size();       
      size_t nSp5 = (fInputSt5Pts.Size() == 0) ? 1 : fInputSt5Pts.Size();       
      size_t nSp6 = (fInputSt6Pts.Size() == 0) ? 1 : fInputSt6Pts.Size();       

      if ((nSp2*nSp3*nSp4*nSp5*nSp6) > 5) {
        if (fDebugIsOn)  std::cerr << " .... Excessive combinatorics in station 2 through 5, give up on this .. " << std::endl;
        return;
      }

      int iTr = 0;
      double x2, x3, x4, x5, x6, y2, y3, y4, y5, y6;
      int nCl2, nCl3, nCl4, nCl5, nCl6; 
      if (fRunNum < 2000) { 
        for (size_t i2 = 0; i2 != nSp2; i2++) {
      	  mySSDClsPtrsX[2] = aSSDClsPtr->cend();
	  mySSDClsPtrsY[2] = aSSDClsPtr->cend();
	  mySSDClsPtrsU[0] = aSSDClsPtr->cend();
	  x2 = DBL_MAX; y2 = DBL_MAX; nCl2 = 0;
          if (fInputSt2Pts.Size() != 0) {
	    const rb::SSDStationPtAutre aPtSt2 = fInputSt2Pts.GetStPoint(i2); x2 = aPtSt2.X(); y2 = aPtSt2.Y(); nCl2 = aPtSt2.NumClusters();
	    if (fDebugIsOn) std::cerr << " ... at Station 2, x = " << x2 << " y " << y2 << " num cl " << aPtSt2.NumClusters() << std::endl; 
	    for (size_t kCl=0; kCl != aPtSt2.NumClusters(); kCl++) {
	      int aId = aPtSt2.ClusterID(kCl); emph::geo::sensorView aView = aPtSt2.ClusterView(kCl); 
	    
              for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	        if ((itCl->Station() != 2) || (itCl->View() != aView) || (itCl->ID() != aId)) continue;
//	      if (fDebugIsOn) std::cerr << " ... setting Cluster " << (*itCl) << std::endl; 
	        if (fDebugIsOn) std::cerr << " ... ... at Station 2, id Cluster " << aId << " View " << aView << std::endl; 
	        if (aView == emph::geo::X_VIEW) mySSDClsPtrsX[2] = itCl;
	        if (aView == emph::geo::Y_VIEW) mySSDClsPtrsY[2] = itCl;
	        if (aView == emph::geo::U_VIEW) mySSDClsPtrsU[0] = itCl;
	        if (aView == emph::geo::W_VIEW) {
	          std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, unexpected view at Station 2 , Space point  is " << std::endl;
		  std::cerr << aPtSt2 << std::endl;
		  std::cerr << " Fatal, quit here .. " << std::endl; exit(2); 
	        }
	      }
	    } // 
	  } // Station 2 has data 
          for (size_t i3 = 0; i3 != nSp3; i3++) {
      	    mySSDClsPtrsX[3] = aSSDClsPtr->cend();
	    mySSDClsPtrsY[3] = aSSDClsPtr->cend();
	    mySSDClsPtrsU[1] = aSSDClsPtr->cend();
	    x3 = DBL_MAX; y3 = DBL_MAX; nCl3 = 0;
            if (fInputSt3Pts.Size() != 0) {
	      const rb::SSDStationPtAutre aPtSt3 = fInputSt3Pts.GetStPoint(i3); x3 = aPtSt3.X(); y3 = aPtSt3.Y(); nCl3 = aPtSt3.NumClusters();
	      if (fDebugIsOn) std::cerr << " ... ... at Station 3, x = " << x3 << " y " << y3 << " num cl " << aPtSt3.NumClusters() << std::endl; 
	      for (size_t kCl=0; kCl != aPtSt3.NumClusters(); kCl++) {
	        int aId = aPtSt3.ClusterID(kCl); emph::geo::sensorView aView = aPtSt3.ClusterView(kCl); 
                for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	          if ((itCl->Station() != 3) || (itCl->View() != aView) || (itCl->ID() != aId)) continue;
//	        if (fDebugIsOn) std::cerr << " ... ... setting Cluster " << (*itCl) << std::endl; 
	          if (fDebugIsOn) std::cerr << " ... ... at Station 3, id Cluster " << aId << " View " << aView << std::endl; 
	          if (aView == emph::geo::X_VIEW) mySSDClsPtrsX[3] = itCl;
	          if (aView == emph::geo::Y_VIEW) mySSDClsPtrsY[3] = itCl;
	          if (aView == emph::geo::U_VIEW) mySSDClsPtrsU[1] = itCl;
	          if (aView == emph::geo::W_VIEW) {
	            std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, unexpected view at Station 3 , Space point  is " << std::endl;
		    std::cerr << aPtSt3 << std::endl;
		    std::cerr << " Fatal, quit here .. " << std::endl; exit(2); 
	          }
	        }
	      } // on the 2 or 3 clusters in Station 3.  
	    } // Station 3 has data 
            for (size_t i4 = 0; i4 != nSp4; i4++) {
      	      mySSDClsPtrsX[4] = aSSDClsPtr->cend(); mySSDClsPtrsX[5] = aSSDClsPtr->cend();
	      mySSDClsPtrsY[4] = aSSDClsPtr->cend(); mySSDClsPtrsY[5] = aSSDClsPtr->cend();
	      mySSDClsPtrsW[0] = aSSDClsPtr->cend(); mySSDClsPtrsW[1] = aSSDClsPtr->cend();
	      x4 = DBL_MAX; y4 = DBL_MAX; nCl4 = 0;
              if (fInputSt4Pts.Size() != 0) {
	        const rb::SSDStationPtAutre aPtSt4 = fInputSt4Pts.GetStPoint(i4); x4 = aPtSt4.X(); y4 = aPtSt4.Y(); nCl4 = aPtSt4.NumClusters();
	        if (fDebugIsOn) std::cerr << " ... ... ... at Station 4, x = " << x4 << " y " << y4 << " num cl " << aPtSt4.NumClusters() << std::endl; 
	        for (size_t kCl=0; kCl != aPtSt4.NumClusters(); kCl++) {
	          int aId = aPtSt4.ClusterID(kCl); emph::geo::sensorView aView = aPtSt4.ClusterView(kCl); 
                  for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	            if ((itCl->Station() != 4) || (itCl->View() != aView) || (itCl->ID() != aId)) continue;
//	            if (fDebugIsOn) std::cerr << " ... ... ... setting Cluster " << (*itCl) << std::endl; 
		    size_t theSensor = static_cast<size_t>(itCl->Sensor());
		    // We could/should check the sensor/view consistency here.. Bare knuckle pointer arithmetic.. 
	            if (fDebugIsOn) std::cerr << " ... ... at Station 4, id Cluster " << aId << " View " << aView  << " cluster Sensor " << theSensor << std::endl; 
		    if (theSensor > 1) {
		      std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, station 4 unexpected sensor index " 
		              << theSensor << " should be 0 or 1, fatal  " << std::endl; exit(2);
		    }
	            if (aView == emph::geo::X_VIEW) mySSDClsPtrsX[4 + theSensor] = itCl;
	            if (aView == emph::geo::Y_VIEW) mySSDClsPtrsY[4 + theSensor] = itCl; // Yb if the Sensor is 1 
	            if (aView == emph::geo::W_VIEW) mySSDClsPtrsW[theSensor] = itCl; // Wb if the Sensor is 1 
	            if (aView == emph::geo::U_VIEW) {
	              std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, unexpected view at Station 4 , Space point  is " << std::endl;
		      std::cerr << aPtSt4 << std::endl;
		      std::cerr << " Fatal, quit here .. " << std::endl; exit(2); 
	            }
	          }
	        } // on the 2 or 3 clusters in Station 4.  
	      } // Station 4 has data 
              for (size_t i5 = 0; i5 != nSp5; i5++) {
      	        mySSDClsPtrsX[6] = aSSDClsPtr->cend(); mySSDClsPtrsX[7] = aSSDClsPtr->cend();
	        mySSDClsPtrsY[6] = aSSDClsPtr->cend(); mySSDClsPtrsY[7] = aSSDClsPtr->cend();
	        mySSDClsPtrsW[2] = aSSDClsPtr->cend(); mySSDClsPtrsW[3] = aSSDClsPtr->cend();
	        x5 = DBL_MAX; y5 = DBL_MAX; nCl5 = 0;
                if (fInputSt5Pts.Size() != 0) {
	          const rb::SSDStationPtAutre aPtSt5 = fInputSt5Pts.GetStPoint(i5); x5 = aPtSt5.X(); y5 = aPtSt5.Y(); nCl5 = aPtSt5.NumClusters();
	          if (fDebugIsOn) std::cerr << " ... ... ... ...  at Station 5, x = " << x5 << " y " << y5 << " num cl " << aPtSt5.NumClusters() << std::endl; 
	          for (size_t kCl=0; kCl != aPtSt5.NumClusters(); kCl++) {
	            int aId = aPtSt5.ClusterID(kCl); emph::geo::sensorView aView = aPtSt5.ClusterView(kCl); 
                    for (std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
	              if ((itCl->Station() != 5) || (itCl->View() != aView) || (itCl->ID() != aId)) continue;
//	            if (fDebugIsOn) std::cerr << " ... ... ... ... setting Cluster " << (*itCl) << std::endl; 
		    size_t theSensor = static_cast<size_t>(itCl->Sensor());
		    // We could/should check the sensor/view consistency here.. Bare knuckle pointer arithmetic.. 
	              if (fDebugIsOn) std::cerr << " ... ... at Station 5, id Cluster " << aId << " View " << aView  << " cluster Sensor " << theSensor << std::endl; 
		      if (theSensor > 1) {
		        std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, station 5 unexpected sensor index " 
		              << theSensor << " should be 0 or 1, fatal  " << std::endl; exit(2);
		      }
	              if (aView == emph::geo::X_VIEW) mySSDClsPtrsX[6 + theSensor] = itCl;
	              if (aView == emph::geo::Y_VIEW) mySSDClsPtrsY[6 + theSensor] = itCl;
	              if (aView == emph::geo::W_VIEW) mySSDClsPtrsW[2 + theSensor] = itCl;
	              if (aView == emph::geo::U_VIEW) {
	                std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, unexpected view at Station 5 , Space point  is " << std::endl;
		        std::cerr << aPtSt5 << std::endl;
		        std::cerr << " Fatal, quit here .. " << std::endl; exit(2); 
	              }
	            }
	          } // on the 2 or 3 clusters in Station 5.  
	        } // Station 5 has data 
                // Write this out.. ( a lot of clone code here.. ) 
	       // Write this out.. AsCII first.. 
	       std::ostringstream infoStsStrStr;
	       infoStsStrStr << " " << x2 << " " << y2 << " " << nCl2 << " " << x3 << " " << y3 << " " << nCl3 
	                           << " " << x4 << " " << y4 << " " << nCl4 << " " << x5 << " " << y5 << " " << nCl5;
	       std::string infoSts(infoStsStrStr.str()); 
	       infoPtSts.push_back(infoSts); 
	       // Now the binary compact, Cluster info. 
	       if (fDebugIsOn) std::cerr << " SSDRecDwnsTracksAutre::dumpCompactEvt, saving alignment event " << fEvtNum << " track " << iTr << std::endl;
               fFOutCompact.write(reinterpret_cast<char*>(&key), sizeof(int)); 
               fFOutCompact.write(reinterpret_cast<char*>(&numDouble), sizeof(int));
               fFOutCompact.write(reinterpret_cast<char*>(&fSubRunNum), sizeof(int)); 
               fFOutCompact.write(reinterpret_cast<char*>(&fEvtNum), sizeof(int));
               fFOutCompact.write(reinterpret_cast<char*>(&iTr), sizeof(int)); // Additional word, compare to first version in SSDAlign3DUVAutreAdd.cxx
	       iTr++;
	      
               double stripInfo[2];
	       const size_t kStMaxXY = (fRunNum < 2000) ? 8 : 9;
               for (size_t kSt=0; kSt != kStMaxXY; kSt++) { //Phase1b or Phase 1c 
	         stripInfo[0] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsX[kSt]->WgtAvgStrip(); 
	         stripInfo[1] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsX[kSt]->WgtRmsStrip();
                 fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	         if (fDebugIsOn) std::cerr << " X view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
               }
               for (size_t kSt=0; kSt != kStMaxXY; kSt++) {//Phase1b or Phase 1c 
	        stripInfo[0] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsY[kSt]->WgtAvgStrip(); 
	        stripInfo[1] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsY[kSt]->WgtRmsStrip();
                fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	        if (fDebugIsOn) std::cerr << " Y view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
               }
               for (size_t kSt=0; kSt != 2; kSt++) {//Phase1b or Phase 1c 
	        stripInfo[0] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsU[kSt]->WgtAvgStrip(); 
	        stripInfo[1] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsU[kSt]->WgtRmsStrip();
                fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	        if (fDebugIsOn) std::cerr << " U view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
               }
               for (size_t kSt=0; kSt != 4; kSt++) {
	         stripInfo[0] = (mySSDClsPtrsW[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsW[kSt]->WgtAvgStrip(); 
	         stripInfo[1] = (mySSDClsPtrsW[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsW[kSt]->WgtRmsStrip();
                 fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	         if (fDebugIsOn) std::cerr << " W view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
               }
	     } // on Station 5 space points, Phase1b  
	   } // on Station 4 space points, Phase1b 
	 } // on Station 3 space points, Phase1b      
       } // on Station 2 space points, Phase1b      


     } else { // Phase1c.  Take advantage of the presence of the const_iterator pointers in the Space Pointers data struct. 
       for (size_t i2 = 0; i2 != nSp2; i2++) {  // station 2 is now signle sensors. 
      	 mySSDClsPtrsX[2] = aSSDClsPtr->cend();
	 mySSDClsPtrsY[2] = aSSDClsPtr->cend(); 
	 mySSDClsPtrsW[0] = aSSDClsPtr->cend(); 
	 x2 = DBL_MAX; y2 = DBL_MAX; nCl2 = 0;
         if (fInputSt2Pts.Size() != 0) {
	   const rb::SSDStationPtAutre aPtSt2 = fInputSt2Pts.GetStPoint(i2); x2 = aPtSt2.X(); y2 = aPtSt2.Y(); nCl2 = aPtSt2.NumClusters();
	   if (fDebugIsOn) std::cerr << " ... ... ... at Station 2, x = " << x2 << " y " << y2 << " num cl " << aPtSt2.NumClusters() << std::endl; 
	   std::vector<myItCl> itClSt2;  aPtSt2.fillItClusters(itClSt2);
           for (size_t k=0; k!=itClSt2.size(); k++) { 
	      if (itClSt2[k]->View() == emph::geo::X_VIEW) mySSDClsPtrsX[2] = itClSt2[k];
	      if (itClSt2[k]->View() == emph::geo::Y_VIEW) mySSDClsPtrsY[2] = itClSt2[k];
	      if (itClSt2[k]->View() == emph::geo::U_VIEW) mySSDClsPtrsU[0] = itClSt2[k];
            }
	  }// Station 2 has data.
          for (size_t i3 = 0; i3 != nSp3; i3++) {  // station 3 is now signle sensors. 
      	    mySSDClsPtrsX[3] = aSSDClsPtr->cend();
	    mySSDClsPtrsY[3] = aSSDClsPtr->cend(); 
	    mySSDClsPtrsW[1] = aSSDClsPtr->cend(); 
	    x3 = DBL_MAX; y3 = DBL_MAX; nCl3 = 0;
            if (fInputSt3Pts.Size() != 0) {
	      const rb::SSDStationPtAutre aPtSt3 = fInputSt3Pts.GetStPoint(i3); x3 = aPtSt3.X(); y3 = aPtSt3.Y(); nCl3 = aPtSt3.NumClusters();
	      if (fDebugIsOn) std::cerr << " ... ... ... at Station 3, x = " << x3 << " y " << y3 << " num cl " << aPtSt3.NumClusters() << std::endl; 
	      std::vector<myItCl> itClSt3;  aPtSt3.fillItClusters(itClSt3);
	      for (size_t k=0; k!=itClSt3.size(); k++) { 
		if (itClSt3[k]->View() == emph::geo::X_VIEW) mySSDClsPtrsX[3] = itClSt3[k];
		if (itClSt3[k]->View() == emph::geo::Y_VIEW) mySSDClsPtrsY[3] = itClSt3[k];
		if (itClSt3[k]->View() == emph::geo::U_VIEW) mySSDClsPtrsU[1] = itClSt3[k];
             }
	   }// Station 3 has data.
	         
           for (size_t i4 = 0; i4 != nSp4; i4++) {  // station 4 is now signle sensors. 
      	     mySSDClsPtrsX[4] = aSSDClsPtr->cend();
	     mySSDClsPtrsY[4] = aSSDClsPtr->cend(); 
	     x4 = DBL_MAX; y4 = DBL_MAX; nCl4 = 0;
             if (fInputSt4Pts.Size() != 0) {
	       const rb::SSDStationPtAutre aPtSt4 = fInputSt4Pts.GetStPoint(i4); x4 = aPtSt4.X(); y4 = aPtSt4.Y(); nCl4 = aPtSt4.NumClusters();
	       if (fDebugIsOn) std::cerr << " ... ... ... at Station 4, x = " << x4 << " y " << y4 << " num cl " << aPtSt4.NumClusters() << std::endl; 
	       std::vector<myItCl> itClSt4;  aPtSt4.fillItClusters(itClSt4);
	       for (size_t k=0; k!=itClSt4.size(); k++) { 
		 if (itClSt4[k]->View() == emph::geo::X_VIEW) mySSDClsPtrsX[4] = itClSt4[k];
		 if (itClSt4[k]->View() == emph::geo::Y_VIEW) mySSDClsPtrsY[4] = itClSt4[k];
	        }
	     }// Station 4 has data.
             for (size_t i5 = 0; i5 != nSp5; i5++) {  // station 5 has double sensors, and W view. 
      	       mySSDClsPtrsX[5] = aSSDClsPtr->cend(); mySSDClsPtrsX[6] = aSSDClsPtr->cend();
	       mySSDClsPtrsY[5] = aSSDClsPtr->cend(); mySSDClsPtrsY[6] = aSSDClsPtr->cend();
	       mySSDClsPtrsW[0] = aSSDClsPtr->cend(); mySSDClsPtrsW[1] = aSSDClsPtr->cend();
	       x5 = DBL_MAX; y5 = DBL_MAX; nCl5 = 0;
               if (fInputSt5Pts.Size() != 0) {
	       const rb::SSDStationPtAutre aPtSt5 = fInputSt5Pts.GetStPoint(i5); x5 = aPtSt5.X(); y5 = aPtSt5.Y(); nCl5 = aPtSt5.NumClusters();
	       if (fDebugIsOn) std::cerr << " ... ... ... at Station 5, x = " << x5 << " y " << y5 << " num cl " << aPtSt5.NumClusters() << std::endl; 
	         std::vector<myItCl> itClSt5;  aPtSt5.fillItClusters(itClSt5);
		 for (size_t k=0; k!=itClSt5.size(); k++) { 
		   if ((itClSt5[k]->Sensor() != 0) && (itClSt5[k]->Sensor() != 1)) {
		       std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt. unexpected sensor number on station5, event " 
		                  << fEvtNum << " Fatal " << std::endl; exit(2);
		   }
		   if (itClSt5[k]->View() == emph::geo::X_VIEW) mySSDClsPtrsX[5 + static_cast<size_t>(itClSt5[k]->Sensor())] = itClSt5[k];
		   if (itClSt5[k]->View() == emph::geo::Y_VIEW) mySSDClsPtrsY[5 + static_cast<size_t>(itClSt5[k]->Sensor())] = itClSt5[k];
		   if (itClSt5[k]->View() == emph::geo::W_VIEW) mySSDClsPtrsW[static_cast<size_t>(itClSt5[k]->Sensor())] = itClSt5[k]; 		   }
	      	}// Station 5 has data.
                for (size_t i6 = 0; i6 != nSp6; i6++) {  // station 6 has double sensors, and W view. 
      	          mySSDClsPtrsX[7] = aSSDClsPtr->cend(); mySSDClsPtrsX[8] = aSSDClsPtr->cend();
	          mySSDClsPtrsY[7] = aSSDClsPtr->cend(); mySSDClsPtrsY[8] = aSSDClsPtr->cend();
	          mySSDClsPtrsW[2] = aSSDClsPtr->cend(); mySSDClsPtrsW[3] = aSSDClsPtr->cend();
	          x6 = DBL_MAX; y6 = DBL_MAX; nCl6 = 0;
                  if (fInputSt6Pts.Size() != 0) {
	           const rb::SSDStationPtAutre aPtSt6 = fInputSt6Pts.GetStPoint(i6); x6 = aPtSt6.X(); y6 = aPtSt6.Y(); nCl6 = aPtSt6.NumClusters();
	           if (fDebugIsOn) std::cerr << " ... ... ... at Station 6, x = " << x6 << " y " << y6 << " num cl " << aPtSt6.NumClusters() << std::endl; 
	           std::vector<myItCl> itClSt6;  aPtSt6.fillItClusters(itClSt6);
		   for (size_t k=0; k!=itClSt6.size(); k++) { 
		     if ((itClSt6[k]->Sensor() != 0) && (itClSt6[k]->Sensor() != 1)) {
		       std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt. unexpected sensor number on station6, event " 
		                  << fEvtNum << " Fatal " << std::endl; exit(2);
		     }
		     if (itClSt6[k]->View() == emph::geo::X_VIEW) mySSDClsPtrsX[7 + static_cast<size_t>(itClSt6[k]->Sensor())] = itClSt6[k];
		     if (itClSt6[k]->View() == emph::geo::Y_VIEW) mySSDClsPtrsY[7 + static_cast<size_t>(itClSt6[k]->Sensor())] = itClSt6[k];
		     if (itClSt6[k]->View() == emph::geo::W_VIEW) mySSDClsPtrsW[2+ static_cast<size_t>(itClSt6[k]->Sensor())] = itClSt6[k]; 		   }
	      	  }// Station 6 has data.
		
	         // Write this out.. AsCII first.. 
	         std::ostringstream infoStsStrStr;
	         infoStsStrStr << " " << x2 << " " << y2 << " " << nCl2 << " " << x3 << " " << y3 << " " << nCl3 
	                           << " " << x4 << " " << y4 << " " << nCl4 << " " << x5 << " " << y5 << " " << nCl5
				   << " " << x6 << " " << y6 << " " << nCl6;
	         std::string infoSts(infoStsStrStr.str()); 
	         infoPtSts.push_back(infoSts); 
	        // Now the binary compact, Cluster info. 
	        if (fDebugIsOn) std::cerr << " SSDRecDwnsTracksAutre::dumpCompactEvt, saving alignment event " << fEvtNum << " track " << iTr << std::endl;
                fFOutCompact.write(reinterpret_cast<char*>(&key), sizeof(int)); 
                fFOutCompact.write(reinterpret_cast<char*>(&numDouble), sizeof(int));
                fFOutCompact.write(reinterpret_cast<char*>(&fSubRunNum), sizeof(int)); 
                fFOutCompact.write(reinterpret_cast<char*>(&fEvtNum), sizeof(int));
                fFOutCompact.write(reinterpret_cast<char*>(&iTr), sizeof(int)); // Additional word, compare to first version in SSDAlign3DUVAutreAdd.cxx
	        iTr++;
	      
                double stripInfo[2];
	        const size_t kStMaxXY = (fRunNum < 2000) ? 8 : 9;
                for (size_t kSt=0; kSt != kStMaxXY; kSt++) { //Phase1b or Phase 1c 
	           stripInfo[0] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsX[kSt]->WgtAvgStrip(); 
	           stripInfo[1] = (mySSDClsPtrsX[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsX[kSt]->WgtRmsStrip();
                   fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	           if (fDebugIsOn) std::cerr << " X view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
                }
                for (size_t kSt=0; kSt != kStMaxXY; kSt++) {//Phase1b or Phase 1c 
	          stripInfo[0] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsY[kSt]->WgtAvgStrip(); 
	          stripInfo[1] = (mySSDClsPtrsY[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsY[kSt]->WgtRmsStrip();
                  fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	          if (fDebugIsOn) std::cerr << " Y view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
                }
                for (size_t kSt=0; kSt != 2; kSt++) {//Phase1b or Phase 1c 
	          stripInfo[0] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsU[kSt]->WgtAvgStrip(); 
	          stripInfo[1] = (mySSDClsPtrsU[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsU[kSt]->WgtRmsStrip();
                  fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	          if (fDebugIsOn) std::cerr << " U view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
                }
                for (size_t kSt=0; kSt != 4; kSt++) {
	          stripInfo[0] = (mySSDClsPtrsW[kSt] == aSSDClsPtr->cend()) ? stripAvNone : mySSDClsPtrsW[kSt]->WgtAvgStrip(); 
	          stripInfo[1] = (mySSDClsPtrsW[kSt] == aSSDClsPtr->cend()) ? stripRmsNone : mySSDClsPtrsW[kSt]->WgtRmsStrip();
                  fFOutCompact.write(reinterpret_cast<char*>(stripInfo), 2*sizeof(double));
	          if (fDebugIsOn) std::cerr << " W view, stationSensor " << kSt << " value " 
	                             << stripInfo[0] << " +- " << stripInfo[1] << std::endl;
		}		     
               }// on Station 6 space points
	     } // on Station 5 space points 
	    } // on Station 4 space points 
	  } // on Station 3 space points      
        } // on Station 2 space points 
	if (fDebugIsOn) std::cerr << "  End  of Phase1c " << std::endl;
      } //Phase1b vs Phase1c     
     // Dump the info..  
     if (iTr != static_cast<int>(infoPtSts.size())) {
         std::cerr << " SSDRecDwnstrTracksAutre::dumpCompactEvt, inconsistent number of info " << infoPtSts.size() 
	           << " vs  numlines " << iTr << " fatal, exit here and now " <<  std::endl; exit(2);
     }      
     if (fDebugIsOn) std::cerr << "  After check on consistency of infoPtSts... iTr " << iTr << std::endl;
     std::ostringstream infoHeaderStrStr;
     infoHeaderStrStr << " " << fSubRunNum << " " << fEvtNum << " " << iTr; 
     std::string infoHeader(infoHeaderStrStr.str());
     for (int il=0; il != iTr; il++) {
         fFOutCompactInfo << infoHeader << " " << il << " " << fItUpstrTr->XOffset() << " " << fItUpstrTr->YOffset() 
	                  << " " <<  fItUpstrTr->XSlope() << " " << fItUpstrTr->YSlope() << infoPtSts[il] << std::endl;
      }
      if (fDebugIsOn) std::cerr << " ... End of dumpCompactEvt ... " << std::endl; 
   } // end of dumpCompactEvt
  } // namespace ssdr
} // namespace emph
