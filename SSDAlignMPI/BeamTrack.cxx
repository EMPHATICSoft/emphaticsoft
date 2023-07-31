////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Beam tracks, either 2D or 3D  
///          Requires Minuit2 
///          Used by BTFit2D and BTFit3D  classes  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>

#include "BeamTrack.h"
#include "BTAlignGeom.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnContours.h"

namespace emph {
  namespace rbal {
  
    BeamTrack::BeamTrack() : 
      myGeo(emph::rbal::BTAlignGeom::getInstance()),
      fDebugIsOn(false), fDoMigrad(false), fAlignMode(true), fNoMagnet(false),
      fNumSensorsXorY(myGeo->NumSensorsXorY()),
      fNumSensorsU(myGeo->NumSensorsU()),
      fNumSensorsV(myGeo->NumSensorsV()),
      fSpill(-1), fEvtNum(-1), fType("Undef"),
      fx0(DBL_MAX), fy0(DBL_MAX), fslx0(DBL_MAX), fsly0(DBL_MAX),
      fx0Err(DBL_MAX, DBL_MAX), fy0Err(DBL_MAX, DBL_MAX), fmom(DBL_MAX), fNominalMomentum(29.0), 
      fslx0Err(DBL_MAX, DBL_MAX), fsly0Err(DBL_MAX, DBL_MAX), fmomErr(DBL_MAX, DBL_MAX), 
      fresids(2*fNumSensorsXorY + fNumSensorsU + fNumSensorsV, DBL_MAX) // assume 3D, a bit of wast of memory if only 2D    
      {  ; } 
    
     double BeamTrack::doFit2D(char view, std::vector<BeamTrackCluster>::const_iterator it) {
     
       fSpill = it->Spill();   fEvtNum = it->EvtNum();
       fType = (view  == 'X') ? std::string("2DX") : std::string("2DY"); 
       if (fDebugIsOn) std::cerr << " BeamTrack::doFit2D for event " << it->EvtNum() << " Spill " << it->Spill() 
                                 << " view " << view << " Station 0, strip " << it->TheAvStrip(view, 0) << std::endl;
       fFcn2D.SetView(view); 	fFcn2D.SetClusterPtr(it);		 
       ROOT::Minuit2::MnUserParameters uPars;
       std::pair<double, double> initVals = (view  == 'X') ? this->SetInitValuesX(view, it) :  this->SetInitValuesY(view, it);
       if (initVals.first == DBL_MAX) return DBL_MAX; // not enough data. 
       std::string nameT0(1, view); nameT0 += std::string("Offset"); std::string nameSlT0(1, view); nameSlT0 += std::string("Slope");
       uPars.Add(nameT0, initVals.first, 0.5); uPars.Add(nameSlT0, initVals.second, 1.0e-3);
       std::vector<double> initValsV{initVals.first, initVals.second};
       std::vector<double> initValsE{0.5, 0.001};
       if (fDebugIsOn) std::cerr << " ... Initial Xoffset " <<initVals.first << " Slope " << initVals.second << std::endl; 
       //
       std::vector<double> finalParams(2, 0);
       double minFValM = 0.;
       if (fDoMigrad) {
         ROOT::Minuit2::MnMigrad migrad(fFcn2D, uPars);
       //
         ROOT::Minuit2::FunctionMinimum min = migrad();
         if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       
         ROOT::Minuit2::MnMinos minos(fFcn2D, min);
       
         if (view == 'X') {
           fx0 = min.UserState().Value(nameT0); fslx0 = min.UserState().Value(nameSlT0);
	   finalParams[0] = fx0; finalParams[1] = fslx0;
	   fx0Err = minos(0); fslx0Err = minos(1);
         } else { 
           fy0 = min.UserState().Value(nameT0); fsly0 = min.UserState().Value(nameSlT0);
	   finalParams[0] = fy0; finalParams[1] = fsly0;
	   fy0Err = minos(0); fsly0Err = minos(1);
         }
	 minFValM = min.Fval();
       } else { 
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 ROOT::Minuit2::FunctionMinimum min = theMinimizer.Minimize(fFcn2D, initValsV, initValsE);
 	 const std::vector<double> myErrs = min.UserParameters().Errors();
 	 const std::vector<double> myValues = min.UserParameters().Params();
        if (fDebugIsOn) std::cerr << " Minimize minimum " << min << std::endl; 
         if (view == 'X') {
           fx0 = myValues[0];  fslx0 = myValues[1];;
	   finalParams[0] = fx0; finalParams[1] = fslx0;
	   fx0Err = std::pair<double, double> (myErrs[0], myErrs[0]); 
	   fslx0Err =std::pair<double, double> (myErrs[1], myErrs[1]); 
         } else { 
	   if (fDebugIsOn) std::cerr << " .... Getting Y Values and errors " << std::endl;
           fy0 =  myValues[0];
	   if (fDebugIsOn) std::cerr << " .... Getting Y Values fy0 " << fy0 <<  std::endl;
           fsly0 =  myValues[1];
	   if (fDebugIsOn) std::cerr << " .... Getting Y Values slope " << fsly0 <<  std::endl;
	   finalParams[0] = fy0; finalParams[1] = fsly0;
	   fy0Err = std::pair<double, double> (myErrs[0], myErrs[0]); 
	   fsly0Err =std::pair<double, double> (myErrs[1], myErrs[1]); 
	   if (fDebugIsOn) std::cerr << " .... Getting errs as well " << fy0Err.first << " slope " << fsly0Err.first <<  std::endl;
        }
	 minFValM = min.Fval();
       } 
       //
       // Get the residuals
       //
       double finalChi = fFcn2D(finalParams);
       fchiSq = finalChi;
       if (std::abs(finalChi - minFValM)/finalChi >  1.0e-6) {
         std::cerr << " BeamTrack::doFit2D for view " << view 
	            << " gave non-reproducible chi-Square.. " << minFValM <<  " vs finalChi " 
		    << finalChi << "  Something wrong, quit here and now " << std::endl;
		    return DBL_MAX;
       }
       for (size_t kSe = 0; kSe != fNumSensorsXorY; kSe++) fresids[kSe] = fFcn2D.Resid(kSe);  
       if (fDebugIsOn) {
          std::cerr << " .... BeamTrack::doFit2D for view " << view << " is done, and quit here... " << std::endl;
	  exit(2);
	} 
       return minFValM;
     }
     double BeamTrack::doFit3D(std::vector<BeamTrackCluster>::const_iterator it) {
       fSpill = it->Spill();   fEvtNum = it->EvtNum();
//       std::cerr <<  " BeamTrack::doFit3D, at event " << fEvtNum << std::endl;
//       fDebugIsOn = fEvtNum > 63;
// Debug missing low statistics on V4b, V5b views.. 
//       const double rmsV4b = it->TheRmsStrip('V', 1);
//       std::cerr << " BeamTrack::doFit3D evt " << fEvtNum << " V4b info, strip number " << it->TheAvStrip('V', 1) << " rms " << rmsV4b << std::endl;
//       fDebugIsOn = ((rmsV4b < 100.) && (fEvtNum > 42)); 
//       if (fDebugIsOn) 
//         std::cerr << " BeamTrack::doFit3D evt " << fEvtNum << " V4b info, strip number " << it->TheAvStrip('V', 1) << " rms " << rmsV4b << std::endl;
       fDebugIsOn = false;
//         fDebugIsOn = ((fEvtNum == 48)  && (fSpill == 100));
	 if (fDebugIsOn) std::cerr << " BeamTrack::doFit3D, debugging evt " << fEvtNum<< " Real run xxxx, spill  " << fSpill << std::endl;
       fFcn3D.SetDebugOn(fDebugIsOn);
       // 
       fType = std::string("3D");
       fFcn3D.SetClusterPtr(it);
       fchiSq = -1.0;
       const double pMonStartVal = fAlignMode ? fNominalMomentum :  50.; // Hardcoded intial value!  Need to get it from the main..
       fFcn3D.SetNominalMomentum(fNominalMomentum);
       // we start slightly off momentum.. to force the minimizer to do some work..  	
       if (fDebugIsOn) std::cerr << " BeamTrack::doFit3D for event " << it->EvtNum() << " Spill " << it->Spill() << std::endl;
       ROOT::Minuit2::MnUserParameters uPars;
       std::pair<double, double> initValsX = this-> SetInitValuesX('X', it);
       if (initValsX.first == DBL_MAX) {
          if (fDebugIsOn) std::cerr << " Not enough data on the X view, give up.. " << std::endl;
          return -1.0; // not enough data. 
       }
       std::string nameT0X("XOffset"); std::string nameSlT0X("XSlope");
       uPars.Add(nameT0X, initValsX.first, 0.250, -50., 50.); uPars.Add(nameSlT0X, 5e-4, 0.001, -0.1, 0.1);
       if (fDebugIsOn) std::cerr << " ... Initial Xoffset " <<initValsX.first << " Slope " << initValsX.second << std::endl;
       // Same for Y 
        std::pair<double, double> initValsY = this-> SetInitValuesY('Y', it);
       if (initValsY.first == DBL_MAX) {
          if (fDebugIsOn) std::cerr << " Not enough data on the Y view, give up.. " << std::endl;
          return -1.0; // not enough data.
       } 
       std::string nameT0Y("YOffset"); std::string nameSlT0Y("YSlope");
       uPars.Add(nameT0Y, initValsY.first, 0.250, -50., 50.); uPars.Add(nameSlT0Y, 2.0e-4, 0.001, -0.1, 0.1);
       size_t numPars = 4;
       double integrationStep = myGeo->IntegrationStepSize();
       if (!fNoMagnet) { 
          uPars.Add(std::string("PMom"), pMonStartVal, 20., -250., 250.);
	  if (fDebugIsOn) std::cerr << " ...  We will fit Using the field map,  pMonStartVal " << pMonStartVal << std::endl;
          numPars++;
       }
       if (fDebugIsOn) std::cerr << " ... Initial Yoffset " <<initValsY.first << " Slope " << initValsY.second << std::endl;
       double minFValM = 0.;
       std::vector<double> finalParams(numPars, 0);
       bool okFit = true;
         if (fDoMigrad) {
       //
         ROOT::Minuit2::MnMigrad migrad(fFcn3D, uPars);
	 if ((!fNoMagnet) && fAlignMode) migrad.Fix("PMom");
       //
        ROOT::Minuit2::FunctionMinimum min = migrad(1000, 0.1);
	 if (!min.IsValid() && (numPars == 5) && (!fNoMagnet)) { //  try again, flipping the sign of slopes, whatever they are in this first minimization. 
           if (fDebugIsOn) std::cerr << " Migrad minimum, first  " << min << std::endl; 
	   // No limits, this around.. 
	   const double ax0 = min.UserState().Value(nameT0X);
	   const double ay0 = min.UserState().Value(nameT0Y);
	   const double aslx0 = min.UserState().Value(nameSlT0X);
	   const double asly0 = min.UserState().Value(nameSlT0Y);
	   const double anewP = min.UserState().Value(std::string("PMom"));
           ROOT::Minuit2::MnUserParameters uPars2nd;
	   uPars2nd.Add(nameT0X, ax0, 0.150); uPars2nd.Add(nameSlT0X, -1.0*aslx0, 0.001);
	   uPars2nd.Add(nameT0Y, ay0, 0.150); uPars2nd.Add(nameSlT0Y, -1.0*asly0, 0.001);
	   if (!fNoMagnet) uPars2nd.Add(std::string("PMom"), anewP + 5.0, 10.);
           ROOT::Minuit2::MnMigrad migrad2nd(fFcn3D, uPars2nd);
	   if ((!fNoMagnet) && fAlignMode) migrad2nd.Fix("PMom");
 	   min = migrad2nd(1000, 0.1);
	 }
         if (fDebugIsOn) std::cerr << " Migrad minimum, 2nd of good first " << min << std::endl; 
       
//         ROOT::Minuit2::MnMinos minos(fFcn3D, min);
       
         fx0 = min.UserState().Value(nameT0X); fslx0 = min.UserState().Value(nameSlT0X);
         fy0 = min.UserState().Value(nameT0Y); fsly0 = min.UserState().Value(nameSlT0Y);
	 fx0Err.first = min.UserState().Error(nameT0X); fx0Err.second = fx0Err.first; // since we skip Minos 
	 fy0Err.first = min.UserState().Error(nameT0Y); fx0Err.second = fx0Err.first; // since we skip Minos 
	 fslx0Err.first = min.UserState().Error(nameSlT0X); fslx0Err.second = fslx0Err.first; // since we skip Minos 
	 fsly0Err.first = min.UserState().Error(nameSlT0Y); fsly0Err.second = fsly0Err.first; // since we skip Minos 
         finalParams[0] = fx0; finalParams[1] = fslx0;
         finalParams[2] = fy0; finalParams[3] = fsly0;
	 if (numPars == 5) {
	    finalParams[4] = min.UserState().Value(std::string("PMom"));
	    fmom = finalParams[4];
	    fmomErr.first = min.UserState().Error(std::string("PMom")); fmomErr.second = fmomErr.first;
	 }
 	 minFValM = min.Fval();
	 okFit = min.IsValid();
	 if (fDebugIsOn) { 
	   if ( okFit) std::cerr << " Migrad fit is valid, fit values " << std::endl;
	   else std::cerr << " Migrad Fit is invalid " << std::endl;
	   std::cerr << min << std::endl;
	 }
      } else {
         std::vector<double>initValsV{initValsX.first, initValsX.second, initValsY.first, initValsY.second};
         std::vector<double>initValsE{0.5, 0.001, 0.5, 0.001};
	 if (numPars == 5) { 
	   initValsV.push_back(pMonStartVal); initValsE.push_back(5.0);
	 }
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 // set a maximum number of calls to FCN.  Strategy is the default one. 
	 ROOT::Minuit2::FunctionMinimum min = theMinimizer.Minimize(fFcn3D, initValsV, initValsE, 1, 200, 0.1);
 	 const std::vector<double> myErrs = min.UserParameters().Errors();
         if (fDebugIsOn) std::cerr << " Minimize minimum " << min << std::endl; 
//         fx0 = min.UserParameters().Value(nameT0X); fslx0 = min.UserParameters().Value(nameSlT0X);
         fx0 = min.UserParameters().Value(0); fslx0 = min.UserParameters().Value(1);
	 finalParams[0] = fx0; finalParams[1] = fslx0;
	 fx0Err = std::pair<double, double> (myErrs[0], myErrs[0]); 
	 fslx0Err =std::pair<double, double> (myErrs[1], myErrs[1]); 
         fy0 = min.UserState().Value(2); fsly0 = min.UserState().Value(3);
	 finalParams[2] = fy0; finalParams[3] = fsly0;
	 fy0Err = std::pair<double, double> (myErrs[2], myErrs[2]); 
	 fsly0Err =std::pair<double, double> (myErrs[3], myErrs[3]); 
	 if (numPars == 5) finalParams[4] = min.UserState().Value(4);
	 minFValM = min.Fval();
	 if (fDebugIsOn) { 
	   if ( okFit) std::cerr << " Simplex fit is valid, fit values " << std::endl;
	   else std::cerr << " Simplex Fit is invalid " << std::endl;
	   std::cerr << min << std::endl;
	 }
       } 
       double finalChi = fFcn3D(finalParams);
       fchiSq = finalChi;
       if (std::abs(finalChi - minFValM )/finalChi >  1.0e-6) {
         std::cerr << " BeamTrack::doFit3D gave non-reproducible chi-Square.. " << minFValM << " vs  finalChi " 
		    << finalChi << "  Something wrong, quit here and now " << std::endl;
		    exit(2);
       }
       for (size_t kSe = 0; kSe != fresids.size(); kSe++) fresids[kSe] = fFcn3D.Resid(kSe);
       if ((!okFit) || (std::abs(fx0) > 150.) || (std::abs(fy0) > 150.)) {
          std::cerr << " Detecting funky x0 or y0 value " << fx0 << " fy0 " << fy0 << " quit here and now .. " << std::endl; exit(2);
          fchiSq *=- 1.0;
       }
       if (fDebugIsOn) std::cerr << " BeamTrack::doFit3D done, finalChi.. " << fchiSq << std::endl; 
       if (fDebugIsOn) { std::cerr << " ................ After a 3D fit, that's enough  " << std::endl; exit(2); }
       return fchiSq;
     }
     
     
     std::pair<double, double> BeamTrack::SetInitValuesX(char view, std::vector<BeamTrackCluster>::const_iterator it) {
     
       double t0_init = 0.;
       if (it->TheRmsStrip(view, 0) < 1000.) { 
         double pitch0 = myGeo->Pitch(view, 0);
         t0_init = -1.0*it->TheAvStrip(view, 0) * pitch0 + myGeo->TrPos(view, 0);
       } else if (it->TheRmsStrip(view, 1) < 1000.){
          double pitch1 = myGeo->Pitch(view, 1);
          t0_init = -1.0*it->TheAvStrip(view, 1) * pitch1 + myGeo->TrPos(view, 1); // assume, inplicitly, that the slope is small.. 
       } else {
         return std::pair<double, double>(DBL_MAX, DBL_MAX); // we need at one cluster in station 0 or 1. 
       }
       // Locate the other point, choose station
       // Need to update this for the momentum kick
       /*  
       double t5_StripRMS_A = it->TheRmsStrip(view, 6); double t5_StripRMS_B = it->TheRmsStrip(view, 7);
       double slt0_init = 0.;
       if (t5_StripRMS_A < 1000.) {
         double pitch5 = myGeo->Pitch(view, 6);
         double t5_init =  it->TheAvStrip(view, 6) * pitch5 - myGeo->TrPos(view, 6); 
         slt0_init =  (t5_init - t0_init)/myGeo->ZPos(view, 6); 
	 if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesX ..case A  t5_init "  
	                          << t5_init << " TrPos " << myGeo->TrPos(view, 6)  << " Z Pos " << myGeo->ZPos(view, 6) << std::endl;
       } else if (t5_StripRMS_B < 1000.) {
         double pitch5 = myGeo->Pitch(view, 7);
         double t5_init =  it->TheAvStrip(view, 7) * pitch5 - myGeo->TrPos(view, 7); 
         slt0_init =  (t5_init - t0_init)/myGeo->ZPos(view, 7); 
	 if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesX ..case A  t5_init " 
	                          << t5_init << " TrPos " << myGeo->TrPos(view, 7)  << " Z Pos " << myGeo->ZPos(view, 7) << std::endl;
       } else {
         double t4_StripRMS_A = it->TheRmsStrip(view, 4); double t4_StripRMS_B = it->TheRmsStrip(view, 5);
         if (t4_StripRMS_A < 1000.) {
           double pitch4 = myGeo->Pitch(view, 4);
           double t4_init =  it->TheAvStrip(view, 4) * pitch4 - myGeo->TrPos(view, 4); 
           slt0_init =  (t4_init - t0_init)/myGeo->ZPos(view, 4); 
	   if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesX Using Station 4 ..case A  t5_init "  
	                          << t4_init << " TrPos " << myGeo->TrPos(view, 4)  << " Z Pos " << myGeo->ZPos(view, 4) << std::endl;
         } else if (t4_StripRMS_B < 1000.) {
           double pitch4 = myGeo->Pitch(view, 5);
           double t4_init =  it->TheRmsStrip(view, 5) * pitch4 - myGeo->TrPos(view, 5); 
           slt0_init =  (t4_init - t0_init)/myGeo->ZPos(view, 5); 
 	   if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesX Using Station 4 ..case B  t5_init " 
	                          << t4_init << " TrPos " << myGeo->TrPos(view, 5)  << " Z Pos " << myGeo->ZPos(view, 5) << std::endl;
        } else {
           return std::pair<double, double>(DBL_MAX, DBL_MAX); // we need at one cluster in station 0 or 1. 
	 }
       }
       */  
       double slt0_init = 0.;  
       return std::pair<double, double>(t0_init, slt0_init);
    }
     
     std::pair<double, double> BeamTrack::SetInitValuesY(char view, std::vector<BeamTrackCluster>::const_iterator it) {
     
       double t0_init = 0.;
       if (it->TheRmsStrip(view, 0) < 1000.) { 
         double pitch0 = myGeo->Pitch(view, 0);
         t0_init = it->TheAvStrip(view, 0) * pitch0 + myGeo->TrPos(view, 0);
       } else if (it->TheRmsStrip(view, 1) < 1000.){
          double pitch1 = myGeo->Pitch(view, 1);
          t0_init = it->TheAvStrip(view, 1) * pitch1 + myGeo->TrPos(view, 1); // assume, inplicitly, that the slope is small.. 
       } else {
         return std::pair<double, double>(DBL_MAX, DBL_MAX); // we need at one cluster in station 0 or 1. 
       }
       // Locate the other point, choose station 
       /*
       ** This code is used only for the aligner, and typical slope is tens of micro radians, at most, at 120 GeV.  Therefore, we can savely 
       * assume zero slope. 
       *
       double t5_StripRMS_A = it->TheRmsStrip(view, 6); double t5_StripRMS_B = it->TheRmsStrip(view, 7);
       double slt0_init = 0.;
       if (t5_StripRMS_A < 1000.) {
         double pitch5 = myGeo->Pitch(view, 6);
         double t5_init =  -1.0*it->TheAvStrip(view, 6) * pitch5 + myGeo->TrPos(view, 6); 
         slt0_init =  (t5_init - t0_init)/myGeo->ZPos(view, 6); 
	 if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesY ..case A  t5_init "  
	                          << t5_init << " TrPos " << myGeo->TrPos(view, 6)  << " Z Pos " << myGeo->ZPos(view, 6) << std::endl;
       } else if (t5_StripRMS_B < 1000.) {
         double pitch5 = myGeo->Pitch(view, 7);
         double t5_init =  -1.0*it->TheAvStrip(view, 7) * pitch5 + myGeo->TrPos(view, 7); 
         slt0_init =  (t5_init - t0_init)/myGeo->ZPos(view, 7); 
	 if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesY ..case A  t5_init " 
	                          << t5_init << " TrPos " << myGeo->TrPos(view, 7)  << " Z Pos " << myGeo->ZPos(view, 7) << std::endl;
       } else {
         double t4_StripRMS_A = it->TheRmsStrip(view, 4); double t4_StripRMS_B = it->TheRmsStrip(view, 5);
         if (t4_StripRMS_A < 1000.) {
           double pitch4 = myGeo->Pitch(view, 4);
           double t4_init =  -1.0*it->TheAvStrip(view, 4) * pitch4 + myGeo->TrPos(view, 4); 
           slt0_init =  (t4_init - t0_init)/myGeo->ZPos(view, 4); 
	   if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesY Using Station 4 ..case A  t5_init "  
	                          << t4_init << " TrPos " << myGeo->TrPos(view, 4)  << " Z Pos " << myGeo->ZPos(view, 4) << std::endl;
         } else if (t4_StripRMS_B < 1000.) {
           double pitch4 = myGeo->Pitch(view, 5);
           double t4_init =  -1.0*it->TheRmsStrip(view, 5) * pitch4 + myGeo->TrPos(view, 5); 
           slt0_init =  (t4_init - t0_init)/myGeo->ZPos(view, 5); 
 	   if (fDebugIsOn) std::cerr << " BeamTrack::SetInitValuesY Using Station 4 ..case B  t5_init " 
	                          << t4_init << " TrPos " << myGeo->TrPos(view, 5)  << " Z Pos " << myGeo->ZPos(view, 5) << std::endl;
        } else {
           return std::pair<double, double>(DBL_MAX, DBL_MAX); // we need at one cluster in station 0 or 1. 
	 }
       }
       */
       double slt0_init = 0.;  
       return std::pair<double, double>(t0_init, slt0_init);
    } 
    
    int BeamTrack::Serialize(std::vector<double> &data) const {  // For MPI transfer. 
      data.clear();
      const double nW = 4 + 5 + 10 + 1 + fresids.size();
      data.push_back(nW);
      data.push_back(static_cast<double>(fSpill));
      data.push_back(static_cast<double>(fEvtNum));
      if (fType.find("Un") == 0)  data.push_back(0.);
      if (fType.find("2DX") == 0)  data.push_back(1.);
      if (fType.find("2DY") == 0)  data.push_back(2.);
      if (fType.find("3D") == 0)  data.push_back(3.);
     // A waste of memore if 2D.. but simplifies the code.. 
      data.push_back(fx0); data.push_back(fy0); data.push_back(fslx0); data.push_back(fsly0); data.push_back(fmom);
      data.push_back(fx0Err.first); data.push_back(fx0Err.second);
      data.push_back(fy0Err.first); data.push_back(fy0Err.second);
      data.push_back(fslx0Err.first); data.push_back(fslx0Err.second);
      data.push_back(fsly0Err.first); data.push_back(fsly0Err.second);
      data.push_back(fmomErr.first); data.push_back(fsly0Err.second);
      data.push_back(fchiSq);
      for (size_t kSe = 0; kSe != fresids.size(); kSe++) data.push_back(fresids[kSe]);
      return static_cast<int>(nW);
    }
// 
    
    void BeamTrack::DeSerialize(const std::vector<double> &data) { // For MPI transfer. 
      std::vector<double>::const_iterator it=data.cbegin();
      size_t nW = static_cast<size_t>(*it); it++;
      size_t nResids =  nW - 20; // See above
      fSpill = *it; it++; fEvtNum=*it, it++;
      int ffType = static_cast<int>(*it); it++;
      if (ffType = 1) fType=std::string("2DX"); 
      if (ffType = 2) fType=std::string("2DY"); 
      if (ffType = 3) fType=std::string("3D");
      fx0  = *it; it++; fy0  = *it; it++; fslx0  = *it; it++; fsly0  = *it; it++; fmom = *it; it++;
      fx0Err.first = *it; it++; fx0Err.second = *it; it++;
      fy0Err.first = *it; it++; fy0Err.second = *it; it++;
      fslx0Err.first = *it; it++; fslx0Err.second = *it; it++;
      fsly0Err.first = *it; it++; fsly0Err.second = *it; it++;
      fmomErr.first = *it; it++; fmomErr.second = *it; it++;
      fchiSq = *it; it++;
      fresids.clear();
      for (size_t kSe = 0; kSe != nResids; kSe++) { fresids.push_back(*it); it++; }
    }
  } // name space.. 
}     
