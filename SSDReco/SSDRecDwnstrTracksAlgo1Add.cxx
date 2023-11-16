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
    //
    // Fit the (unique) upstream and downstream SSDCluster to a beam track, 
    // thereby assuming no hadronic interaction took place. 
    // 
  
    bool SSDRecDwnstrTracksAlgo1::doUpDwn3DClFitAndStore( double pStart) {
      fUpStrDwnStrTrack.Reset();
      bool isOKMult  = ((fInputSt2Pts.Size() == 1) && (fInputSt3Pts.Size() == 1) && (fInputSt4Pts.Size() == 1) 
			    && (fInputSt5Pts.Size() == 1) );  //  NoTgt31Gev_ClSept_A2e_6a   

      if ((!isOKMult) || (!fDoUseUpstreamTrack)) return false;
      
      // 
      //Further more.. 
      //
      if (fInputSt2Pts.NumTriplets() != 1) return false;
      if (fInputSt3Pts.NumTriplets() != 1) return false;
      // We skip St4, because of the three dead readout chips.. 
      if (fInputSt5Pts.NumTriplets() != 1) return false;

      std::vector<myItCl> dataIn; 
      dataIn.push_back(fItUpstrTr->ItClX0()); // assume the iterator is valid. 
      dataIn.push_back(fItUpstrTr->ItClX1());
      dataIn.push_back(fItUpstrTr->ItClY0());
      dataIn.push_back(fItUpstrTr->ItClY1());
      // Now the dostream station.. Copy SSDClusters const_iterators..
      std::vector<myItCl> itClsTmp; 
      fInputSt2Pts.CBegin()->fillItClusters(itClsTmp);
      for (size_t k=0; k != itClsTmp.size(); k++)  dataIn.push_back(itClsTmp[k]);
      fInputSt3Pts.CBegin()->fillItClusters(itClsTmp);
      for (size_t k=0; k != itClsTmp.size(); k++)  dataIn.push_back(itClsTmp[k]);
      fInputSt4Pts.CBegin()->fillItClusters(itClsTmp);
      for (size_t k=0; k != itClsTmp.size(); k++)  dataIn.push_back(itClsTmp[k]);
      fInputSt5Pts.CBegin()->fillItClusters(itClsTmp);
      for (size_t k=0; k != itClsTmp.size(); k++)  dataIn.push_back(itClsTmp[k]);
      //
       if (fDebugIsOn) std::cerr << " SSDRecDwnstrTracksAlgo1::doUpDwn3DClFitAndStore, evt " 
                                 << fEvtNum <<  " Number of SSDClusters  " << dataIn.size() << std::endl;
       if (fDebugIsOn) {
         std::cerr << " Input Data.. for " << dataIn.size() << " clusters...  " << std::endl;
         for (size_t k = 0; k != dataIn.size(); k++) {
	   myItCl aItCl = dataIn[k];
	   std::cerr << " Station  " << aItCl->Station() << " View " << aItCl->View() 
	             << " Sensor " << aItCl->Sensor() << " strip " << aItCl->WgtAvgStrip() << std::endl; 
         }
	 std::cerr << " ----------------- " << std::endl;  std::cerr << std::endl;
       }
       if (fUpDownFitterFCN == nullptr) {
          fUpDownFitterFCN = new SSD3DTrackFitFCNAlgo1(fRunNum);
//          fUpDownFitterFCN->SetMagnetShift(fMagShift); 
       }
       fUpDownFitterFCN->SetDebugOn(false);
       fUpDownFitterFCN->SetInputClusters(dataIn);
       fUpDownFitterFCN->ResetZpos();
       fUpDownFitterFCN->SetExpectedMomentum(pStart); 
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsOut, parsOutErr;
       double x0Start =  fItUpstrTr->XOffset(); double y0Start =  fItUpstrTr->YOffset();
       double x0StartSlope = fItUpstrTr->XSlope(); double y0StartSlope = fItUpstrTr->YSlope();
       if (fDebugIsOn) {
         std::cerr <<  " Rough XY fit, x0Start " << x0Start << " xSlope " << fItUpstrTr->XSlope() << " y0 Start " 
	           <<  y0Start << " YSlope " << fItUpstrTr->YSlope()  << std::endl;
       }
       uPars.Add(std::string("X_0"), x0Start, 0.1, -20., 20.);  
       uPars.Add(std::string("Slx_0"), x0StartSlope, 0.001, -0.1, 0.1);  // for Misligned 7c. No magnet Misalignament flag 25000025, Dgap = 3.0 mm  
       uPars.Add(std::string("Y_0"), y0Start, 0.1, -20., 20.);  
       uPars.Add(std::string("Sly_0"), y0StartSlope, 0.001, -0.1, 0.1); 
       unsigned int nPars = 4; 
       if (!fNoMagnet) {
          nPars++;
	  double pMin = (pStart > 0.) ? 0.01*pStart : -1.0*std::min(-5.0*pStart, 250.);
	  double pMax = (pStart > 0.) ? std::min(5.0*pStart, 250.) : 0.01*pStart;
	  double pStart = pStart; 
	  if (fEvtNum == 5) pStart = 31.0;  
          fUpDownFitterFCN->SetExpectedMomentum(pStart); 
          uPars.Add(std::string("Mom"), 0.5*pStart, 2., pMin, pMax); 
	  if (fDebugIsOn) std::cerr << " Adding momentum parameter , start val " << pStart << " Limits " << pMin << ", " << pMax << std::endl;
       } 
       std::vector<double> initValsV, initValsE; // for use in the Simple Minimizer.. 
       for (unsigned int k=0; k != nPars; k++) { initValsV.push_back(uPars.Value(k)); initValsE.push_back(uPars.Error(k)); } 
       // Testing the FCN, once .. 
       if (fDebugIsOn) { 
         std::cerr << " ....... About to test the FCN with " << initValsV.size() << " parameters.. " << std::endl;
         double aChiSqOnce = (*fUpDownFitterFCN)(initValsV);
//         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that is enough for now " << std::endl; exit(2);
         std::cerr << " chi-Square Once.. " << aChiSqOnce << " .... and that we keep going... " << std::endl;
       }
       ROOT::Minuit2::MnMigrad migrad((*fUpDownFitterFCN), uPars);
       if (fDebugIsOn) std::cerr << " ..... About to call migrad... " << std::endl;
       //
       ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
       if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
       const bool isMigradValid = min.IsValid(); 
       bool isSimplexValid = true;
       int flagValid = 0; // 0 nothing worked, 1 MiGrad is O.K., 2, Simplex is Ok.  
       double chiSq = DBL_MAX;
       int numCallFCN = 0; 
       
       if (!isMigradValid) {
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize(*fUpDownFitterFCN, initValsV, initValsE);
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
       if (flagValid == 0) return false;
       if (fDebugIsOn) {  
          std::cerr << " Got a fit, chiSq " << chiSq << " number of FCN calls " << numCallFCN <<  " check again.. " << std::endl;
//	  fUpDownFitterFCN->SetDebugOn(true);
//	  (*fUpDownFitterFCN)(parsOut);
       }
       fUpStrDwnStrTrack.SetType(rb::XYUW5ST);
       fUpStrDwnStrTrack.SetTrParams(parsOut[0], parsOut[2], parsOut[1], parsOut[3]);
       fUpStrDwnStrTrack.SetTrParamsErrs(parsOutErr[0], parsOutErr[2], parsOutErr[1], parsOutErr[3], 0., 0.);  // Covariance matrices needs to be studied. 
       // Interface a bit incomplete.. 
       fUpStrDwnStrTrack.SetXChiSq(chiSq); fUpStrDwnStrTrack.SetYChiSq(chiSq);
       if (!fNoMagnet) fUpStrDwnStrTrack.SetMomentum(parsOut[4], parsOutErr[4]);
      // 
//       if (fDebugIsOn) { 
//          std::cerr << " .. O.K., enough of SSDRecDwnstrTracksAlgo1::doUpDwn3DClFitAndStore, for now, quit!....  " 
//                    << std::endl; exit(2); 
//       } 
       return true ;
    }
    void SSDRecDwnstrTracksAlgo1::dumpBeamTracksCmp() const {
        
       if (fTrs.size() == 0) return;
       if (!fFOutCmpBeamTracks.is_open()) { 
         std::ostringstream aFOutTmpStrStr; 
         aFOutTmpStrStr << "./BeamTrack3Ways_" << fRunNum  << "_" << fTokenJob << "_V1.txt";
         std::string aFOutTmpStr(aFOutTmpStrStr.str());
         fFOutCmpBeamTracks.open(aFOutTmpStr.c_str());
         fFOutCmpBeamTracks << " spill evt x0 y0 slx0 slx0Err sly0 sly0Err slx0BTH slx0BTHErr sly0BTH sly0BTHErr pBTH pErrBTH chiSqBTH " << 
                   "x2 xsl2 xsl2Err y2  y2sl y2slErr p pErr chiSq thetaX thetaXErr thetaY thetaYErr theta thetaErr t tErr " << std::endl;
       }
       fFOutCmpBeamTracks << " " << fEvtNum << " " << fSubRunNum;
       fFOutCmpBeamTracks << " " << fItUpstrTr->XOffset() << " " << fItUpstrTr->YOffset() 
        		  << " " << fItUpstrTr->XSlope() << " " << fItUpstrTr->XSlopeErr() 
        		  << " " << fItUpstrTr->YSlope() << " " << fItUpstrTr->YSlopeErr(); 
       fFOutCmpBeamTracks << " " << fUpStrDwnStrTrack.XSlope() << " " << fUpStrDwnStrTrack.XSlopeErr() 
	                   << " " << fUpStrDwnStrTrack.YSlope() << " " << fUpStrDwnStrTrack.YSlopeErr() 
			   << " " << fUpStrDwnStrTrack.Momentum() << " " << fUpStrDwnStrTrack.MomentumErr()
			   << " " << fUpStrDwnStrTrack.XChiSq(); // X and Y chiSq, no difference.. see above. 
			   
       std::vector<rb::DwnstrTrackAlgo1>::const_iterator itSel = fTrs.cbegin(); 
       // Take the one wit hthe smalled chi-Sq.  Should not happen as we have one Space Point in each of the downstream stations.. 
       double minChiSq = DBL_MAX;
       if (fTrs.size() > 1) {
         for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it=fTrs.cbegin();  it!=fTrs.cend(); it++) { 
	   if (it->ChiSq() < minChiSq) { itSel = it; minChiSq = it->ChiSq(); }
         }
       }
       fFOutCmpBeamTracks << " " << itSel->XOffset()  << " " << itSel->XSlope() << " " << itSel->XSlopeErr() 
                          << " " << itSel->YOffset()  << " " << itSel->YSlope() << " " << itSel->YSlopeErr() 
			  << " " << itSel->Momentum() << " " << itSel->MomentumErr() <<  " " << itSel->ChiSq();
			  
	const double thetaX = itSel->XSlope() - fItUpstrTr->XSlope(); 		  
	const double thetaY = itSel->YSlope() - fItUpstrTr->YSlope(); 		  
	const double thetaXErrSq = itSel->XSlopeErr() * itSel->XSlopeErr()  +  fItUpstrTr->XSlopeErr() * fItUpstrTr->XSlopeErr(); 		  
	const double thetaYErrSq = itSel->YSlopeErr() * itSel->YSlopeErr()  +  fItUpstrTr->YSlopeErr() * fItUpstrTr->YSlopeErr();
	const double thetaSq = thetaX*thetaX + thetaY*thetaY;
	const double thetaSqErrSq = 4.0*(thetaX*thetaX*thetaXErrSq + thetaY*thetaY*thetaYErrSq);
	const double t = itSel->Momentum()*itSel->Momentum()*thetaSq; 
	const double tErr = itSel->Momentum()* std::sqrt(4.0*itSel->MomentumErr()*itSel->MomentumErr()*thetaSq*thetaSq
	                               + itSel->Momentum()*itSel->Momentum()*thetaSqErrSq); 
	fFOutCmpBeamTracks << " " << thetaX << " " << std::sqrt(std::abs(thetaXErrSq))
	                   << " " << thetaY << " " << std::sqrt(std::abs(thetaYErrSq));
	fFOutCmpBeamTracks << " " << std::sqrt(thetaSq) << " " << std::pow(thetaSqErrSq, 0.25) << " " << t << " " << tErr << std::endl;		   		  
   }			   
	
  }
}
//
