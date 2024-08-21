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
#include "canvas/Utilities/Exception.h"


namespace emph { 
  namespace ssdr {
    //
    // Fit the (unique) upstream and downstream SSDCluster to a beam track, 
    // thereby assuming no hadronic interaction took place. 
    // 
  
    bool SSDRecDwnstrTracksAlgo1::doUpDwn3DClFitAndStore( double pStart) {
      if (fRunNum < 2000) return false; // Disable Phase1b . 
      std::cerr << " ... SSDRecDwnstrTracksAlgo1::doUpDwn3DClFitAndStore Need to be re-comissioned... Fatal...  " << std::endl; exit(2); 
      fUpStrDwnStrTrack.Reset();
      bool isOKMult  = ((fInputSt2Pts.Size() == 1) && (fInputSt3Pts.Size() == 1) && (fInputSt4Pts.Size() < 5 ) 
			    && ((fInputSt5Pts.Size() == 1) || fInputSt5Pts.Size() < 3 )  
			    && ((fInputSt6Pts.Size() == 1) || fInputSt6Pts.Size() < 3 ));  //  NoTgt31Gev_ClSept_A2e_6a   

      if ((!isOKMult) || (!fDoUseUpstreamTrack)) return false;
      
      // 
      //Further more.. 
      //
      if (fInputSt2Pts.NumTriplets() != 1) return false;
      if (fInputSt3Pts.NumTriplets() != 1) return false;
      // We skip St4, because of the three dead readout chips.. 
//      if (fInputSt5Pts.NumTriplets() != 1) return false;

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
       try { 
          ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
 //      } catch (art::errors::ErrorCodes FatalRootError const& e) {
         if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
         const bool isMigradValid = min.IsValid(); 
         bool isSimplexValid = true;
         int flagValid = 0; // 0 nothing worked, 1 MiGrad is O.K., 2, Simplex is Ok. 
         double chiSq = DBL_MAX;
         int numCallFCN = 0; 
       
         if (!isMigradValid) {
	   try {
             ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	     ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize(*fUpDownFitterFCN, initValsV, initValsE);
 	     parsOutErr = minS.UserParameters().Errors();
 	     parsOut = minS.UserParameters().Params();
             isSimplexValid = minS.IsValid();
	     chiSq = minS.Fval(); numCallFCN=minS.NFcn();
	     if (isSimplexValid) flagValid = 2;
	   } catch (...) { return false; }
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
         fUpStrDwnStrTrack.SetTrParamsErrs(parsOutErr[0], parsOutErr[2], parsOutErr[1], parsOutErr[3], 0., 0.);  
	 // Covariance matrices needs to be studied. 
         // Interface a bit incomplete.. 
         fUpStrDwnStrTrack.SetXChiSq(chiSq); fUpStrDwnStrTrack.SetYChiSq(chiSq);
         if (!fNoMagnet) fUpStrDwnStrTrack.SetMomentum(parsOut[4], parsOutErr[4]);
      // 
//       if (fDebugIsOn) { 
//          std::cerr << " .. O.K., enough of SSDRecDwnstrTracksAlgo1::doUpDwn3DClFitAndStore, for now, quit!....  " 
//                    << std::endl; exit(2); 
//       } 
         return true ;
      }  catch (...) { return false; }
    }
    void SSDRecDwnstrTracksAlgo1::dumpBeamTracksCmp(bool useKlmTrack) const {
//       if (fDebugIsOn) 
//        { std::cerr << " SSDRecDwnstrTracksAlgo1::dumpBeamTracksCmp, num Dwn tracks " << fTrs.size() << " And quit noe ! " << std::endl; exit(2); } 
       if ((!useKlmTrack) && (fTrs.size() == 0)) return;
       if ((useKlmTrack) && (fTrsKlm.size() == 0)) return;
       if (!fFOutCmpBeamTracks.is_open()) { 
         std::ostringstream aFOutTmpStrStr; 
         aFOutTmpStrStr << "./BeamTrack2Ways_" << fRunNum  << "_" << fTokenJob << "_V1.txt";
         std::string aFOutTmpStr(aFOutTmpStrStr.str());
         fFOutCmpBeamTracks.open(aFOutTmpStr.c_str());
         fFOutCmpBeamTracks << " spill evt useKlm x0 y0 slx0 slx0Err sly0 sly0Err " << 
                   "x2 xsl2 xsl2Err y2  y2sl y2slErr p pErr chiSq thetaX thetaXErr thetaY thetaYErr theta thetaErr t tErr phiDwn phiScat " << std::endl;
       }
       fFOutCmpBeamTracks << " " << fSubRunNum << " " << fEvtNum;
       if (useKlmTrack) fFOutCmpBeamTracks << " " << 1; 
       else fFOutCmpBeamTracks << " " << 0; 
       fFOutCmpBeamTracks << " " << fItUpstrTr->XOffset() << " " << fItUpstrTr->YOffset() 
        		  << " " << fItUpstrTr->XSlope() << " " << fItUpstrTr->XSlopeErr() 
        		  << " " << fItUpstrTr->YSlope() << " " << fItUpstrTr->YSlopeErr(); 
			   
       std::vector<rb::DwnstrTrackAlgo1>::const_iterator itSel = useKlmTrack ? fTrsKlm.cbegin(): fTrs.cbegin(); 
       // Take the one with the smalled chi-Sq.  Should not happen as we have one Space Point in each of the downstream stations.. 
       double minChiSq = DBL_MAX;
       if (useKlmTrack) { 
         if (fTrsKlm.size() > 1) {
           for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it=fTrsKlm.cbegin();  it!=fTrsKlm.cend(); it++) { 
	     if (it->ChiSq() < minChiSq) { itSel = it; minChiSq = it->ChiSq(); }
           }
         }
       } else { 
         if (fTrs.size() > 1) {
           for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it=fTrs.cbegin();  it!=fTrs.cend(); it++) { 
	     if (it->ChiSq() < minChiSq) { itSel = it; minChiSq = it->ChiSq(); }
           }
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
	const double thetaErrSq =(thetaXErrSq + thetaYErrSq);
	const double t = itSel->Momentum()*itSel->Momentum()*thetaSq; 
	const double tErr = itSel->Momentum()* std::sqrt(4.0*itSel->MomentumErr()*itSel->MomentumErr()*thetaSq*thetaSq
	                               + itSel->Momentum()*itSel->Momentum()*thetaErrSq); 
				       
	fFOutCmpBeamTracks << " " << thetaX << " " << std::sqrt(std::abs(thetaXErrSq))
	                   << " " << thetaY << " " << std::sqrt(std::abs(thetaYErrSq));
	fFOutCmpBeamTracks << " " << std::sqrt(thetaSq) << " " << std::sqrt(thetaErrSq) << " " << t << " " << tErr; 
	fFOutCmpBeamTracks << " " << std::atan2(fItUpstrTr->YSlope(), fItUpstrTr->XSlope()) << " " << std::atan2(thetaY, thetaX) <<   std::endl;		   		  
   }			   
    
   bool SSDRecDwnstrTracksAlgo1::doDwn3DKlmFitAndStore(int trID, const std::vector<myItStPt> &dataIn,   double pStartTmp) {
   
      bool isOKMult  = ((fInputSt2Pts.Size() > 0) && (fInputSt3Pts.Size() > 0) 
			    && (fInputSt5Pts.Size() > 0));   

       if (fDebugIsOn) {
         std::cerr << std::endl <<  " SSDRecDwnstrTracksAlgo1::doDwn3DKlmFitAndStore, spill " << fSubRunNum << " evt " 
                   << fEvtNum <<  " Dwnstr Track Id " << trID << " Number of SSDClusters  " << dataIn.size() << " pStartTmp " << pStartTmp << std::endl;
	if (!isOKMult) std::cerr << " ... multiplicity count not quite good enough, skip.. " << std::endl;			 
        for (size_t k = 0; k != dataIn.size(); k++) {
	   std::cerr << " ...  " <<  *(dataIn[k]) << std::endl;
         }
//	 if (fDebugIsOn)  { std::cerr << " .... And quit now... " << std::endl; exit(2); } 
	 std::cerr << " ----------------- " << std::endl;  std::cerr << std::endl;
       }
       if (fNoMagnet) { 
         if (fDebugIsOn) std::cerr << " .............. No magnet!.. Not applicable,... " <<std::endl; 
	 return false;
       }
       if (fFitterKlmFCN == nullptr) {
         std::cerr << " ..... SSDRecDwnstrTracksAlgo1::doDwn3DKlmFitAndStore, constructing a new instance of fFitterKlmFCN " << std::endl; 
         fFitterKlmFCN  = new SSD3DTrackKlmFitFCNAlgo1(fRunNum);
//	 fFitterKlmFCN->SetMagnetShift(fMagShift);  // To be defined in this class, for later.. 
       }
       const bool DoMigrad = true;
       double pStart = pStartTmp;
       if ((std::abs(pStartTmp) > 55.) && (pStartTmp > 0.)) pStart = 55.; // Needs retuning...
       //  Watch for VariableMetricBuilder Initial matrix not pos.def
       if ((std::abs(pStartTmp) > 55.) && (pStartTmp < 0.)) pStart = -55.;
       fFitterKlmFCN->SetDebugOn(fDebugIsOn);
       fFitterKlmFCN->SetInputClusters(dataIn);
       const double aChiSts = fFitterKlmFCN->SumChiSqSts();
//       fFitterKlmFCN->SetExpectedMomentum(pStart);  
// Be conservative, assume 30 GeV/c beam 
       fFitterKlmFCN->SetExpectedMomentum(30.0);  
       ROOT::Minuit2::MnUserParameters uPars;
       std::vector<double> parsInCold, parsOut, parsOutErr;
       unsigned int nPars = 1; 
       const double pMin = (pStart > 0.) ? 0.5*pStart : -1.0*std::max(-2.0*pStart, -125.);
       const double pMax = (pStart > 0.) ? std::min(2.0*pStart, 125.) : 0.5*pStart;
       uPars.Add(std::string("Mom"), pStart, 0.05*std::abs(pStart), pMin, pMax); 
       uPars.SetLimits(0,  pMin, pMax); // Should not have to do this... 
       parsInCold.push_back(pStart); 
       if (fDebugIsOn) std::cerr << " ..... Adding momentum parameter , start val " << pStart << " Limits " << pMin << ", " << pMax << std::endl;
       // run test, to make sure we have a valid chiSq to start with. 
       const double chiSqCold = (*fFitterKlmFCN)(parsInCold);
       if (chiSqCold > 1.0e5) return false;
       bool isMinValid = false; 
       if (DoMigrad) { 
         ROOT::Minuit2::MnMigrad migrad((*fFitterKlmFCN), uPars);
         if (fDebugIsOn) std::cerr << " ..... About to call migrad... " << std::endl;
       //
         try {
           ROOT::Minuit2::FunctionMinimum min = migrad(200, 0.1);
           isMinValid = min.IsValid(); 
           if (fDebugIsOn) std::cerr << " Function  minimum, from Migrad... " << min << std::endl; 
       //
           parsOutErr = min.UserParameters().Errors();
           parsOut = min.UserParameters().Params();
	} catch (...) { return false; }
       } else { 
         std::vector<double> initValsV, initValsE; initValsV.push_back(pStart); initValsE.push_back(std::abs(0.05*pStart));
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 try { 
           if (fDebugIsOn) std::cerr << " ..... About to call Simple Minimizer... " << std::endl;
//	   fFitterKlmFCN->SetDebugOn(true);
	   ROOT::Minuit2::FunctionMinimum minS = theMinimizer.Minimize((*fFitterKlmFCN), initValsV, initValsE);
           if (fDebugIsOn) std::cerr << " Function  minimum from Simple Minimizer..." << minS << std::endl; 
 	   parsOutErr = minS.UserParameters().Errors();
 	   parsOut = minS.UserParameters().Params();
           isMinValid = minS.IsValid();
//	   std::cerr << " .... And quit after this first Simplex.... " << std::endl; exit(2);
        } catch (...) { return false; } 
       }
       if(!isMinValid) {
          if (fDebugIsOn) std::cerr << " ..... Migrad or Simplex Invalid, no Klm track.. " << std::endl;
	  return false;
       }  
       const double finalChiSq = (*fFitterKlmFCN)(parsOut);
       rb::DwnstrTrackAlgo1 aTr;
       aTr.SetID(trID);
       aTr.SetType(rb::FOURSTATION);
       aTr.SetChiSq(finalChiSq);
       aTr.SetChiSqKlmInfo(aChiSts, fFitterKlmFCN->ChiSqXView(), fFitterKlmFCN->ChiSqYView());
       const std::pair<double, double> xSt = fFitterKlmFCN->XStart(); 
       const std::pair<double, double> ySt = fFitterKlmFCN->YStart(); 
       const std::pair<double, double> SlxSt = fFitterKlmFCN->SlXStart(); 
       const std::pair<double, double> SlySt = fFitterKlmFCN->SlYStart(); 
       aTr.SetTrParams(xSt.first, SlxSt.first, ySt.first, SlySt.first, parsOut[0]);
       aTr.SetTrParamsErrs(xSt.second, SlxSt.second, ySt.second, SlySt.second, parsOutErr[0]);
       fTrsKlm.push_back(aTr);
//       if (fDebugIsOn) { std::cerr << " ...SSDRecDwnstrTracksAlgo1::doDwn3DKlmFitAndStore ... and quit for now... " << std::endl; exit(2); }
       return true;
       //
    } 
    void SSDRecDwnstrTracksAlgo1::dumpBeamTracksCmpKlm() const {
        
       if (fTrs.size() == 0) return;
       if (fTrsKlm.size() == 0) return;
       if (!fFOutCmpKlmTracks.is_open()) { 
         std::ostringstream aFOutTmpStrStr; 
         aFOutTmpStrStr << "./SecondaryTrack2Ways_" << fRunNum  << "_" << fTokenJob << "_V1.txt";
         std::string aFOutTmpStr(aFOutTmpStrStr.str());
         fFOutCmpKlmTracks.open(aFOutTmpStr.c_str());
         fFOutCmpKlmTracks << " spill evt ntr itr x2 x2Err xsl2 xsl2Err y2 y2Err y2sl y2slErr p pErr chiSq" 
	                   << " ntrk itrk x2k x2kErr xsl2k xsl2kErr y2k x2kErr y2slk y2slkErr pk pkErr"
			   << " chiSqk chiSqkSts chiSqkX chiSqkY" << std::endl;
       }
       int itNum = 0;
       for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it=fTrs.cbegin();  it!=fTrs.cend(); it++, itNum++) { 
         fFOutCmpKlmTracks << " " << fSubRunNum << " " << fEvtNum << " " << fTrs.size() << " " << itNum;
         fFOutCmpKlmTracks << " " << it->XOffset()  << " " << it->XOffsetErr()  << " " << it->XSlope() << " " << it->XSlopeErr() 
                          << " " << it->YOffset()  << " " << it->YOffsetErr()  << " " << it->YSlope() << " " << it->YSlopeErr() 
			  << " " << it->Momentum() << " " << it->MomentumErr() <<  " " << it->ChiSq();
	 std::vector<rb::DwnstrTrackAlgo1>::const_iterator itkSel=fTrsKlm.cbegin();
	 double rrSqMin = DBL_MAX; int itkNum = 0; int itkNumSel = -1;		  
	 for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator itk=fTrsKlm.cbegin();  itk!=fTrsKlm.cend(); itk++, itkNum++) {
	   if (it->ID() == itk->ID()) { itkSel= itk; itkNumSel = itk->ID(); break; }
	 }
	 fFOutCmpKlmTracks << " " << fTrsKlm.size() << " " << itkNumSel ;
	 if (itkNumSel < 0) {
	   fFOutCmpKlmTracks << " 0. 9.99e4 0. 9.99e4 0. 9.99e4 0. 9.99e4 9.99e4 9.99e4 1.0e6 1.0e6 1.0e6 1.0e6";
         } else { 
	   fFOutCmpKlmTracks << " " << itkSel->XOffset() << " " << itkSel->XOffsetErr()
	                  << " " << itkSel->XSlope() << " " << itkSel->XSlopeErr() 
                          << " " << itkSel->YOffset()  << " " << itkSel->YOffsetErr() 
			  << " " << itkSel->YSlope() << " " << itkSel->YSlopeErr() 
			  << " " << itkSel->Momentum() << " " << itkSel->MomentumErr()
			  <<  " " << itkSel->ChiSq() << " " << itkSel->ChiSqSts() << " " 
			  << itkSel->ChiSqKlmX() << " " << itkSel->ChiSqKlmY() ;
	 }
	 fFOutCmpKlmTracks << std::endl;
       } // On track, fits
   }
   void SSDRecDwnstrTracksAlgo1::transferKlmToClFit() {
     fTrs.clear();
     for (auto it = fTrsKlm.cbegin(); it!= fTrsKlm.cend(); it++) {
       fTrs.push_back(*it); 
     }
   }			   
//
   bool SSDRecDwnstrTracksAlgo1::HasGoodHighPForAlignment(double pMin, double chiSqMax) const {
     if (fTrs.size() != 1) return false; 
     if (fTrs[0].Momentum() < pMin) return false;    
     if (fTrs[0].ChiSq() > chiSqMax) return false;    
     if (fTrs[0].Type() != 12) return false;    
//     if ((std::abs(fTrs[0].XOffset()) + 0.0) > 8.0) return false; //  8s0001 Too broad, diverge to two distinct dsamples.  
     if (fRunNum  < 2700) { 
       if ((std::abs(fTrs[0].XOffset()) + 1.0) > 2.0) return false;  // 9s0001  
       if (std::abs(fTrs[0].YOffset()) > 8.0) return false; // No typo, accidental centering for Station 2, for run 2144 8s0001 Too broad  
       if (std::abs(fTrs[0].YOffset()) > 4.0) return false; // No 9s0001
     } else {
        if (std::abs(fTrs[0].XOffset() + 0.001) > 15.0) return false;  // Broad, for 2716 and 
        if (std::abs(fTrs[0].YOffset() + 2.0) > 15.0) return false; //  
     }
       const bool atLeastSt2or3 = (((fInputSt2Pts.Size() > 0) && (fInputSt2Pts.NumTriplets() < 2)) || 
                                  ((fInputSt3Pts.Size() > 0) && (fInputSt3Pts.NumTriplets() < 2)));
       const bool atLeastSt4or5 = (((fInputSt4Pts.Size() > 0) && (fInputSt4Pts.NumTriplets() < 2)) || 
                                  ((fInputSt5Pts.Size() > 0) && (fInputSt5Pts.NumTriplets() < 2)));
       const bool atLeastSt5or6 = (((fInputSt5Pts.Size() > 0) && (fInputSt5Pts.NumTriplets() < 2)) || 
                                  ((fInputSt6Pts.Size() > 0) && (fInputSt6Pts.NumTriplets() < 2)));
				   
     return (atLeastSt2or3 && atLeastSt5or6 && (fInputSt4Pts.Size() < 2)); 
  }
} 
}
