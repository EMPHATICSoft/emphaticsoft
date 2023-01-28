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
      fDebugIsOn(false), fDoMigrad(false),
      fNumSensorsXorY(myGeo->NumSensorsXorY()),
      fNumSensorsU(myGeo->NumSensorsU()),
      fNumSensorsV(myGeo->NumSensorsV()),
      fSpill(-1), fEvtNum(-1), fType("Undef"),
      fx0(DBL_MAX), fy0(DBL_MAX), fslx0(DBL_MAX), fsly0(DBL_MAX),
      fx0Err(DBL_MAX, DBL_MAX), fy0Err(DBL_MAX, DBL_MAX), 
      fslx0Err(DBL_MAX, DBL_MAX), fsly0Err(DBL_MAX, DBL_MAX),
      fresids(2*fNumSensorsXorY + fNumSensorsU + fNumSensorsV, DBL_MAX) // assume 3D, a bit of wast of memory if only 2D    
      { ; } 
    
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
       return minFValM;
     }
     double BeamTrack::doFit3D(std::vector<BeamTrackCluster>::const_iterator it) {
       fSpill = it->Spill();   fEvtNum = it->EvtNum();
       fType = std::string("3D");
       fFcn3D.SetClusterPtr(it);	
       if (fDebugIsOn) std::cerr << " BeamTrack::doFit3D for event " << it->EvtNum() << " Spill " << it->Spill() << std::endl;
       ROOT::Minuit2::MnUserParameters uPars;
       std::pair<double, double> initValsX = this-> SetInitValuesX('X', it);
       if (initValsX.first == DBL_MAX) return DBL_MAX; // not enough data. 
       std::string nameT0X("XOffset"); std::string nameSlT0X("XSlope");
       uPars.Add(nameT0X, initValsX.first); uPars.Add(nameSlT0X, initValsX.second);
       if (fDebugIsOn) std::cerr << " ... Initial Xoffset " <<initValsX.first << " Slope " << initValsX.second << std::endl;
       // Same for Y 
        std::pair<double, double> initValsY = this-> SetInitValuesY('Y', it);
       if (initValsY.first == DBL_MAX) return DBL_MAX; // not enough data. 
       std::string nameT0Y("YOffset"); std::string nameSlT0Y("YSlope");
       uPars.Add(nameT0Y, initValsY.first); uPars.Add(nameSlT0Y, initValsY.second);
       if (fDebugIsOn) std::cerr << " ... Initial Yoffset " <<initValsY.first << " Slope " << initValsY.second << std::endl;
       double minFValM = 0.;
       std::vector<double> finalParams(4, 0);
        if (fDoMigrad) {
       //
         ROOT::Minuit2::MnMigrad migrad(fFcn3D, uPars);
       //
         ROOT::Minuit2::FunctionMinimum min = migrad();
         if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       
         ROOT::Minuit2::MnMinos minos(fFcn3D, min);
       
         fx0 = min.UserState().Value(nameT0X); fslx0 = min.UserState().Value(nameSlT0X);
         fx0Err = minos(0); fslx0Err = minos(1);
         fy0 = min.UserState().Value(nameT0Y); fsly0 = min.UserState().Value(nameSlT0Y);
         fy0Err = minos(2); fsly0Err = minos(3);
         finalParams[0] = fx0; finalParams[1] = fslx0;
         finalParams[2] = fy0; finalParams[3] = fsly0;
 	 minFValM = min.Fval();
      } else {
         std::vector<double>initValsV{initValsX.first, initValsX.second, initValsY.first, initValsY.second};
         std::vector<double>initValsE{0.5, 0.001, 0.5, 0.001};
         ROOT::Minuit2::VariableMetricMinimizer theMinimizer;
	 ROOT::Minuit2::FunctionMinimum min = theMinimizer.Minimize(fFcn2D, initValsV, initValsE);
 	 const std::vector<double> myErrs = min.UserParameters().Errors();
         if (fDebugIsOn) std::cerr << " Minimize minimum " << min << std::endl; 
         fx0 = min.UserParameters().Value(nameT0X); fslx0 = min.UserParameters().Value(nameSlT0X);
	 finalParams[0] = fx0; finalParams[1] = fslx0;
	 fx0Err = std::pair<double, double> (myErrs[0], myErrs[0]); 
	 fslx0Err =std::pair<double, double> (myErrs[1], myErrs[1]); 
         fy0 = min.UserState().Value(nameT0Y); fsly0 = min.UserState().Value(nameSlT0Y);
	 finalParams[2] = fy0; finalParams[3] = fsly0;
	 fy0Err = std::pair<double, double> (myErrs[2], myErrs[2]); 
	 fsly0Err =std::pair<double, double> (myErrs[3], myErrs[3]); 
	 minFValM = min.Fval();
       	 
       } 
       double finalChi = fFcn3D(finalParams);
       fchiSq = finalChi;
       if (std::abs(finalChi - minFValM )/finalChi >  1.0e-6) {
         std::cerr << " BeamTrack::doFit2D gave non-reproducible chi-Square.. " << minFValM << " vs  finalChi " 
		    << finalChi << "  Something wrong, quit here and now " << std::endl;
		    exit(2);
       }
       for (size_t kSe = 0; kSe != fNumSensorsXorY; kSe++) fresids[kSe] = fFcn2D.Resid(kSe);
       return finalChi;
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
       return std::pair<double, double>(t0_init, slt0_init);
    } 
    
    int BeamTrack::Serialize(std::vector<double> &data) const {  // For MPI transfer. 
      data.clear();
      const double nW = 4 + 4 + 8 + 1 + fresids.size();
      data.push_back(nW);
      data.push_back(static_cast<double>(fSpill));
      data.push_back(static_cast<double>(fEvtNum));
      if (fType.find("Un") == 0)  data.push_back(0.);
      if (fType.find("2DX") == 0)  data.push_back(1.);
      if (fType.find("2DY") == 0)  data.push_back(2.);
      if (fType.find("3D") == 0)  data.push_back(3.);
     // A waste of memore if 2D.. but simplifies the code.. 
      data.push_back(fx0); data.push_back(fy0); data.push_back(fslx0); data.push_back(fsly0);
      data.push_back(fx0Err.first); data.push_back(fx0Err.second);
      data.push_back(fy0Err.first); data.push_back(fy0Err.second);
      data.push_back(fslx0Err.first); data.push_back(fslx0Err.second);
      data.push_back(fsly0Err.first); data.push_back(fsly0Err.second);
      data.push_back(fchiSq);
      for (size_t kSe = 0; kSe != fresids.size(); kSe++) data.push_back(fresids[kSe]);
      return nW;
    }
// 
    
    void BeamTrack::DeSerialize(const std::vector<double> &data) { // For MPI transfer. 
      std::vector<double>::const_iterator it=data.cbegin();
      size_t nW = static_cast<size_t>(*it); it++;
      size_t nResids =  nW - 17; // See above
      fSpill = *it; it++; fEvtNum=*it, it++;
      int ffType = static_cast<int>(*it); it++;
      if (ffType = 1) fType=std::string("2DX"); 
      if (ffType = 2) fType=std::string("2DY"); 
      if (ffType = 3) fType=std::string("3D");
      fx0  = *it; it++; fy0  = *it; it++; fslx0  = *it; it++; fsly0  = *it; it++;
      fx0Err.first = *it; it++; fx0Err.second = *it; it++;
      fy0Err.first = *it; it++; fy0Err.second = *it; it++;
      fslx0Err.first = *it; it++; fslx0Err.second = *it; it++;
      fsly0Err.first = *it; it++; fsly0Err.second = *it; it++;
      fchiSq = *it; it++;
      fresids.clear();
      for (size_t kSe = 0; kSe != nResids; kSe++) { fresids.push_back(*it); it++; }
    }
  } // name space.. 
}     
