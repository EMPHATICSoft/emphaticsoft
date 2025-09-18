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
#include "RecoBase/Vertex.h"
#include "SSDReco/experimental/SSDRecVertexAutre.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"

namespace emph { 
  namespace ssdr {
  
				
    SSDRecVertexAutre::SSDRecVertexAutre() :
      fEmgeo(nullptr),
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()),
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0),
      fDebugIsOn(false), fChiSqCut(5.0), 
      fTokenJob("undef")
      {
      ;
    }
    // 
    SSDRecVertexAutre::~SSDRecVertexAutre() {
      if (fFOutVert.is_open()) fFOutVert.close();
    }
    //
    bool SSDRecVertexAutre::RecAndFitIt(const art::Event &evt, 
	                   std::vector<rb::BeamTrack>::const_iterator itBeam,
			   const ssdr::SSDRecDwnstrTracksAutre  &dwnstr) {
			   
       fRunNum = evt.run(); fSubRunNum = evt.subRun(); fEvtNum = evt.id().event();
       if (fEmgeo == nullptr) fGeoService->Geo();
       fChiFinal = DBL_MAX; fVert.Reset();
       const double z2 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
       fFitterFCN.SetZ2(z2);
       fFitterFCN.ResetInput();
       fFitterFCN.SetInputUpstrTr(itBeam);
       int numOKDwnTr = 0;
       if (fDebugIsOn) {
         std::cerr << " SSDRecVertexAutre::RecAndFitIt, start, Num Downstream Tracks submitted = " <<  dwnstr.Size() << std::endl;
	 const double zTgt = 380.; // Phase1c... Yak.. 
	 const double xTgt = itBeam->XOffset() + zTgt*itBeam->XSlope(); 
	 const double yTgt = itBeam->YOffset() + zTgt*itBeam->YSlope(); 
         std::cerr << " Upstream Beam track params, at Target, x =   " << xTgt << " Y " << yTgt << 
	                " at Station 2, X " <<  itBeam->XOffset() + z2*itBeam->XSlope() << " Y " 
			<< itBeam->YOffset() + z2*itBeam->YSlope()  << std::endl;
       }
       for (std::vector<rbex::DwnstrTrack>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) {
         if (fDebugIsOn) std::cerr << " ... For track " << it->ID() << " chiSq " << it->ChiSq() << " chiSq cut " << fChiSqCut << std::endl;
         if (it->ChiSq() > fChiSqCut) continue;
	 // Other slope cuts ?
	  if (fDebugIsOn) std::cerr << " ... Chek track Params " << it->ID() << " X offset " 
	                             << it->XOffset() << " Slope " << 1000.*it->XOffset() << " mrad " << std::endl; 
	 fFitterFCN.AddInputDwn(it); numOKDwnTr++;
       }
       fFitterFCN.SetDebugOn(fDebugIsOn);
       if (fDebugIsOn) {
         std::cerr << " .... , Num Downstream Tracks accepted = " << numOKDwnTr  << std::endl;
       }
       ROOT::Minuit2::MnUserParameters uPars;
       uPars.Add(std::string("XV"), itBeam->XOffset(), 0.2, -50., 50.);  
       uPars.Add(std::string("YV"), itBeam->YOffset(), 0.2, -50., 50.);  
       uPars.Add(std::string("ZV"), 150., 2., 0., 600.); 
       ROOT::Minuit2::MnMigrad migrad(fFitterFCN, uPars);
       if (fDebugIsOn) std::cerr << " ..... About to call migrad... But, before, we arbitrate... " << std::endl;
       //
       bool isMigradValid = false;
       // arbitrate first, require distinct slope. 
       for (std::vector<rbex::DwnstrTrack>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) it->SetUserFlag(0);
       bool noChange = false;
       while (!noChange) {
         noChange = true; // temporarely 
         for (std::vector<rbex::DwnstrTrack>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) {
           if (it->UserFlag() != 0) continue;
           const double slxi = it->XSlope();
           const double slyi = it->YSlope();
           for (std::vector<rbex::DwnstrTrack>::const_iterator jt = dwnstr.CBegin(); jt != dwnstr.CEnd(); jt++ ) {
	     if (it == jt) continue;
             if (jt->UserFlag() != 0) continue;
             const double slxj = jt->XSlope();
             const double slyj = jt->YSlope();
	     const double dsl = std::sqrt((slxj - slxi)*(slxj - slxi) + (slyj - slyi)*(slyj - slyi));
	     if (fDebugIsOn) std::cerr << " Atbitrating track ID " << it->ID() << " against " << jt->ID() << " dsl " << dsl << std::endl;
	     if (dsl < 5.0e-4) { //same track, pick the best chi-sq. 
	       if (fDebugIsOn) std::cerr << " .... same track ! , chisq  " << it->ChiSq() << " and " << jt->ChiSq() << std::endl;
	       if (it->ChiSq() > jt->ChiSq()) it->SetUserFlag(-1);
	       else jt->SetUserFlag(-1);
	       noChange = false;
	     }
	   }  
         }
       }
       try {
         ROOT::Minuit2::FunctionMinimum min = migrad(2000, 0.1);
         if (fDebugIsOn) std::cerr << " Migrad minimum " << min << std::endl; 
       //
         isMigradValid = min.IsValid(); 
         if (isMigradValid) {
           fChiFinal = min.Fval();
	   fVert.SetID(0);
           std::vector<double> parsOutErr = min.UserParameters().Errors();
 	   std::vector<double> parsOut = min.UserParameters().Params();
	   fVert.SetPosition(parsOut[0], parsOut[1], parsOut[2]);
	   fVert.SetPositionErr(parsOutErr[0], parsOutErr[1], parsOutErr[2]);
	   fVert.SetChiSq(fChiFinal);
           if (min.HasCovariance()) {
             for (size_t i=0; i != 3; i++) {
               for (size_t j=0; j != 3; j++) {
		 fVert.SetCovarianceMatrix(i,j,min.UserCovariance()(i,j));
	       }
	     }
           }
           for (std::vector<rbex::DwnstrTrack>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) {
             if (it->ChiSq() > fChiSqCut) continue; // Other slope cuts ? 
             if (it->UserFlag() != 0) continue;	 
	     fVert.AddTrackUID(it->ID());
           }
	 
           if (fDebugIsOn) {
	     std::cerr << " Vertex fit valid, Position X = " << parsOut[0] << " +- " << parsOutErr[0] 
	            << " Y " << parsOut[1] << " +- " << parsOutErr[1] 
		    << " Z " << parsOut[2] << " +- " << parsOutErr[2] << " ChiSq " << fChiFinal << std::endl;
	   }
         } else { 
	    if (fDebugIsOn) std::cerr << " No valid Minimum... " << std::endl;
	  // Could do arbitration here, removing one track at a time 
         }
         for (std::vector<rbex::DwnstrTrack>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) it->SetUserFlag(0);
	} catch (...) { 
           for (std::vector<rbex::DwnstrTrack>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) it->SetUserFlag(0);
	   return false; 
        }
       return isMigradValid;
    }
    // 
    void SSDRecVertexAutre::openOutputCsvFiles() const {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameStrStr;
       fNameStrStr << "SSDRecVertexAutre_Run_" << fRunNum << "_" << fTokenJob << "_V3.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutVert.open(fNameStr.c_str());
       fFOutVert << " spill evt nDwntr X XErr Y YErr Z ZErr chiSq " << std::endl;
       //
     }
     //
     void SSDRecVertexAutre::dumpInfoForR() const {
      if (!fFOutVert.is_open()) this->openOutputCsvFiles(); 
       if (fVert.ChiSq() == DBL_MAX) return;
       fFOutVert << " " << fSubRunNum << " " << fEvtNum; 
       fFOutVert << " " << fVert.NumDwnstr() << " " << fVert.X() << " " << fVert.XErr() << " " 
                 << fVert.Y() << " " << fVert.YErr() << " " << fVert.Z() << " " << fVert.ZErr() << " " << fVert.ChiSq() << std::endl;   
     }
  } // namespace ssdr
} // namespace emph
