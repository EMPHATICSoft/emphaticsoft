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
#include "SSDReco/SSDRecDwnstrTracksAlgo1.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"

namespace emph { 
  namespace ssdr {
  
    const double SSDRecDwnstrTracksAlgo1::fSqrt2 = std::sqrt(2.0);
    const double SSDRecDwnstrTracksAlgo1::fOneOverSqrt2 = std::sqrt(0.5); 
				
    SSDRecDwnstrTracksAlgo1::SSDRecDwnstrTracksAlgo1() :
      fRunHistory(nullptr),   
      fEmgeo(nullptr),
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), 
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0),
      fNEvents(0), fDebugIsOn(false), fDoMigrad(true), fNoMagnet(false), fChiSqCut(5.0), fPrelimMomentum(50.0), 
      fTokenJob("undef"), fFitterFCN(nullptr),
      fInputSt2Pts(2), fInputSt3Pts(3), fInputSt4Pts(4), fInputSt5Pts(5), 
      fTrs()
      {
      ;
      
    }
    // 
    SSDRecDwnstrTracksAlgo1::~SSDRecDwnstrTracksAlgo1() {
      if (fFOutTrs.is_open()) fFOutTrs.close();
      delete fFitterFCN;
    }
    //
    size_t SSDRecDwnstrTracksAlgo1::RecStation(size_t kSt, const art::Event &evt, 
                                const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
	switch (kSt) {
	  case 2 : { return fInputSt2Pts.RecIt(evt, aSSDClsPtr); }
	  case 3 : { return fInputSt3Pts.RecIt(evt, aSSDClsPtr); }
	  case 4 : { return fInputSt4Pts.RecIt(evt, aSSDClsPtr); }
	  case 5 : { return fInputSt5Pts.RecIt(evt, aSSDClsPtr); }
	  default : {return 0; } // should not happen 
	}
				
    }
    // 
    size_t SSDRecDwnstrTracksAlgo1::RecAndFitIt(const art::Event &evt) {
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
      if (fDebugIsOn) std::cerr << " SSDRecDwnstrTracksAlgo1::RecIt, starting on spill " 
                                 << fSubRunNum << " evt " << fEvtNum << std::endl;
      if (fEmgeo == nullptr) {
         fRunHistory = new runhist::RunHistory(fRunNum);   
         fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
         fNoMagnet = fEmgeo->MagnetUSZPos() < 0.;
      }   
      fTrs.clear();
      if ((fInputSt2Pts.Size() == 0) || (fInputSt3Pts.Size() == 0)) {
        if (fDebugIsOn) std::cerr << " ...  No data from station 2 or 3, Pointers are null, so, no tracks.. " << std::endl;
	return 0; 
      }
      fInputSt2Pts.ResetUsage(); fInputSt3Pts.ResetUsage(); fInputSt4Pts.ResetUsage();  fInputSt5Pts.ResetUsage(); 
      if ((fInputSt4Pts.Size() > 0) && (fInputSt5Pts.Size() > 0) ) return this->RecAndFitAll4Stations();  
       
      if (fInputSt4Pts.Size() == 0) {
//        std::cerr << " Encountered spill/event " << fSubRunNum << "/" << fEvtNum << " with no Space Points in Station 4 " << std::endl;
        if (fInputSt5Pts.Size() > 0) {
	   return this->RecAndFitStation235();
	} else {
	   if (fDebugIsOn) std::cerr << " ...  No data from station 4 and 5, so, no tracks.. " << std::endl;
	   return 0; 
	}
      } 
      if (fInputSt5Pts.Size()) {
//        std::cerr << " Encountered spill/event " << fSubRunNum << "/" << fEvtNum << " with no Space Points in Station 5 " << std::endl;
        if (fInputSt4Pts.Size() > 0) {
	   return this->RecAndFitStation234();
	} else {
	   if (fDebugIsOn) std::cerr << " ...  No data from station 4 and 5, so, no tracks.. " << std::endl;
	   return 0; 
	}
      }
      if (fDebugIsOn) std::cerr << " ... Confusing case, check logic, meanwhile, no tracks.. " << std::endl;  
      return 0; // Should not reach here.. 
    } //RecAndFitIt
    //
    size_t SSDRecDwnstrTracksAlgo1::RecAndFitAll4Stations() {
    
      const double dz25 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      if (fDebugIsOn) std::cerr << " Starting on RecAndFitAll4Stations, dz25 = " << dz25 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, "  
			       << fInputSt4Pts.Size() << " St4 Pts,  " << fInputSt5Pts.Size() << " St5 Pts " << std::endl;
      for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
        const double x2Start = itSt2->X(); 
        const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
        if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
        for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
          if (itSt3->UserFlag() != 0) continue;
 	  if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
   
          for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++) { 
            if (itSt4->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... At station 4, at x = " << itSt4->X() << " Y " << itSt4->Y() << std::endl; 
    
            for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++) {
              if (itSt5->UserFlag() != 0) continue;
  	      if (fDebugIsOn) std::cerr << " ... ... ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
              const double x5End = itSt5->X();
              const double y5End = itSt5->Y();// To estimate starting value for the track param fit.
	      const double xSlopeStart =  (x5End - x2Start)/dz25; const double ySlopeStart =  (y5End - y2Start)/dz25;
              fFitterFCN->ResetInputPts();
	      fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	      fFitterFCN->AddInputPt(itSt3);
	      fFitterFCN->AddInputPt(itSt4);
	      fFitterFCN->AddInputPt(itSt5);
	      if (this->doFitAndStore(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
	        int kTrCnt = static_cast<int>(fTrs.size());
		itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
		itSt4->SetUserFlag(kTrCnt); itSt5->SetUserFlag(kTrCnt); // no chi-square arbitration for now.. 
	      }
            } // on Space Points in Station 5 
          } // on Space Points in Station 4 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAlgo1::RecAndFitAll4Stations, num Tracks " << fTrs.size();
      return fTrs.size(); 
    }
    //
     size_t SSDRecDwnstrTracksAlgo1::RecAndFitStation234() {
    
     size_t nTrBefore = fTrs.size(); 
     const double dz24 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 4) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
     if (fDebugIsOn) std::cerr << " Starting on RecAndFit,Station234 dz25 = " << dz24 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, "  
			       << fInputSt4Pts.Size() << " St4 Pts,  " << std::endl;
     for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
       const double x2Start = itSt2->X(); 
       const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
       if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
       
       for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
         if (itSt3->UserFlag() != 0) continue;
 	 if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
    
         for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt4 = fInputSt4Pts.CBegin(); itSt4 != fInputSt4Pts.CEnd(); itSt4++) { 
           if (itSt4->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... At station 4, at x = " << itSt4->X() << " Y " << itSt4->Y() << std::endl; 
           const double x4End = itSt4->X();
           const double y4End = itSt4->Y();// To estimate starting value for the track param fit.
	   const double xSlopeStart =  (x4End - x2Start)/dz24; const double ySlopeStart =  (y4End - y2Start)/dz24;
           fFitterFCN->ResetInputPts();
	   fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	   fFitterFCN->AddInputPt(itSt3);
	   fFitterFCN->AddInputPt(itSt4);
	   if (this->doFitAndStore(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
	        int kTrCnt = static_cast<int>(fTrs.size());
		itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
		itSt4->SetUserFlag(kTrCnt); 
	    }
          } // on Space Points in Station 4 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2 
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAlgo1::RecAndFitStation234, num Tracks " << fTrs.size();
      return ( fTrs.size() - nTrBefore);
    }
    //
    size_t SSDRecDwnstrTracksAlgo1::RecAndFitStation235() {
    
      size_t nTrBefore = fTrs.size(); 
      const double dz25 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 5) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      if (fDebugIsOn) std::cerr << " Starting on RecAndFit,Station234 dz25 = " << dz25 << " with " 
                               << fInputSt2Pts.Size() << " St2 Pts,  " << fInputSt3Pts.Size() << " St3 Pts, " 
			       << fInputSt5Pts.Size() << " St5 Pts,  " << std::endl;
      for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt2 = fInputSt2Pts.CBegin(); itSt2 != fInputSt2Pts.CEnd(); itSt2++) { 
        const double x2Start = itSt2->X(); 
        const double y2Start = itSt2->Y(); // To estimate starting value for the track param fit. 
        if (itSt2->UserFlag() != 0) continue;
	if (fDebugIsOn) std::cerr << " ... At station 2, at x = " << itSt2->X() << " Y " << itSt2->Y() << std::endl; 
       
        for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt3 = fInputSt3Pts.CBegin(); itSt3 != fInputSt3Pts.CEnd(); itSt3++) { 
         if (itSt3->UserFlag() != 0) continue;
 	 if (fDebugIsOn) std::cerr << " ... ... At station 3, at x = " << itSt3->X() << " Y " << itSt3->Y() << std::endl; 
	 
         for (std::vector<rb::SSDStationPtAlgo1>::const_iterator itSt5 = fInputSt5Pts.CBegin(); itSt5 != fInputSt5Pts.CEnd(); itSt5++) { 
            if (itSt5->UserFlag() != 0) continue;
  	    if (fDebugIsOn) std::cerr << " ... ... ... At station 5, at x = " << itSt5->X() << " Y " << itSt5->Y() << std::endl; 
            const double x5End = itSt5->X();
            const double y5End = itSt5->Y();// To estimate starting value for the track param fit.
	    const double xSlopeStart =  (x5End - x2Start)/dz25; const double ySlopeStart =  (y5End - y2Start)/dz25;
            fFitterFCN->ResetInputPts();
	    fFitterFCN->AddInputPt(itSt2); // Deep copy in inner loop.. Oh well... 
	    fFitterFCN->AddInputPt(itSt3);
	    fFitterFCN->AddInputPt(itSt5);
	    if (this->doFitAndStore(rb::FOURSTATION, x2Start, y2Start, xSlopeStart, ySlopeStart)) {
	        int kTrCnt = static_cast<int>(fTrs.size());
		itSt2->SetUserFlag(kTrCnt); itSt3->SetUserFlag(kTrCnt); 
		itSt5->SetUserFlag(kTrCnt); 
	    }
          } // on Space Points in Station 5 
        } // on Space Points in Station 3 
      } // on Space Points in Station 2 
      if (fDebugIsOn) std::cerr << " End of SSDRecDwnstrTracksAlgo1::RecAndFitStation235, num Tracks " << fTrs.size();
      return ( fTrs.size() - nTrBefore);
    }
    //
    bool SSDRecDwnstrTracksAlgo1::doFitAndStore(rb::DwnstrTrType type, double xStart, double yStart, double xSlopeStart, double ySlopeStart) {
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsOut, parsOutErr;
//       fFitterFCN->SetDebugOn(fDebugIsOn);
       fFitterFCN->SetDebugOn(false);
       if (fDebugIsOn) {
         std::cerr << " SSDRecDwnstrTracksAlgo1::doFitAndStore, start, X2 = " <<  xStart << " Y2 " << yStart 
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
        if (!fNoMagnet) 
	  uPars.Add(std::string("PInv"), -1.0/fPrelimMomentum, std::abs(2.0/fPrelimMomentum), -5.0, 5.0); // could flip the sign Min. mometum is 0.1 GeV
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
       }
       if (!this->IsAlreadyFound(aTr))  { fTrs.push_back(aTr); return true; }
//       } else {
       // else should do arbitration here.. For now, just count track that not exactly the same.. 
       //  this->ArbitrateAndStore(aTr);
//       }
       return false;
    } // doFitAndStore
    //
    bool SSDRecDwnstrTracksAlgo1::IsAlreadyFound(const rb::DwnstrTrackAlgo1 &aTr) const {  // this needs tuning.. 
      if (fTrs.size() == 0) return false;
      for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it ++) {
        if ((std::abs(aTr.XSlope() - it->XSlope()) < 1.0e-5) && 
	    (std::abs(aTr.YSlope() - it->YSlope()) < 1.0e-5) && (std::abs(aTr.Momentum() - it->Momentum()) < 0.05 )) return true;   
      }
      return false;
    }
    void SSDRecDwnstrTracksAlgo1::openOutputCsvFiles() const {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameStrStr;
       fNameStrStr << "SSDRecDwnstrTracksAlgo1_Run_" << fRunNum << "_" << fTokenJob << "_V1.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutTrs.open(fNameStr.c_str());
       fFOutTrs << " spill evt nPTrs iTr type x2 x2Err xsl xslErr y2 y2Err y2sl y2slErr p pErr chiSq " << std::endl;
       //
       
     }
     //
     void SSDRecDwnstrTracksAlgo1::dumpInfoForR() const {
       if (fTrs.size() == 0) return;
       if (!fFOutTrs.is_open()) this->openOutputCsvFiles(); 
       std::ostringstream headerStrStr; 
       headerStrStr << " " << fSubRunNum << " " << fEvtNum << " " << fTrs.size(); 
       std::string headerStr(headerStrStr.str());
       size_t k=0;
       for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = fTrs.cbegin(); it != fTrs.cend(); it++, k++) { 
         fFOutTrs << headerStr << " " << k << " " << it->Type()  
	         << " " << it->XOffset() << " " << it->XOffsetErr() << " " << it->XSlope() << " " << it->XSlopeErr()
		 << " " << it->YOffset() << " " << it->YOffsetErr() << " " << it->YSlope() << " " << it->YSlopeErr()
		 << " " <<  it->Momentum() << " " << it->MomentumErr() << " " << it->ChiSq() << std::endl; 
       }
     }
  } // namespace ssdr
} // namespace emph
