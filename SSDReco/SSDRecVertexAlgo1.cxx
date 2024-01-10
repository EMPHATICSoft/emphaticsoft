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
#include "RecoBase/VertexAlgo1.h"
#include "SSDReco/SSDRecVertexAlgo1.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"

namespace emph { 
  namespace ssdr {
  
				
    SSDRecVertexAlgo1::SSDRecVertexAlgo1() :
      fEmgeo(nullptr),
      fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()),
      fRunNum(0),  fSubRunNum(INT_MAX), fEvtNum(0),
      fNEvents(0), fDebugIsOn(false), fChiSqCut(5.0), 
      fTokenJob("undef")
      {
      ;
    }
    // 
    SSDRecVertexAlgo1::~SSDRecVertexAlgo1() {
      if (fFOutVert.is_open()) fFOutVert.close();
    }
    //
    bool SSDRecVertexAlgo1::RecAndFitIt(const art::Event &evt, 
	                   std::vector<rb::BeamTrackAlgo1>::const_iterator itBeam,
			   const ssdr::SSDRecDwnstrTracksAlgo1  &dwnstr) {
			   
       fRunNum = evt.run(); fSubRunNum = evt.subRun(); fEvtNum = evt.id().event();
       if (fEmgeo == nullptr) fGeoService->Geo();
       fChiFinal = DBL_MAX; fVert.Reset();
       const double z2 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
       fFitterFCN.SetZ2(z2);
       fFitterFCN.ResetInput();
       fFitterFCN.SetInputUpstrTr(itBeam);
       int numOKDwnTr = 0;
       if (fDebugIsOn) {
         std::cerr << " SSDRecVertexAlgo1::RecAndFitIt, start, Num Downstream Tracks submitted = " <<  dwnstr.Size() << std::endl;
       }
       for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) {
         if (fDebugIsOn) std::cerr << " ... For track " << it->ID() << " chiSq " << it->ChiSq() << " chiSq cut " << fChiSqCut << std::endl;
         if (it->ChiSq() > fChiSqCut) continue;
	 // Other slope cuts ? 
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
       if (fDebugIsOn) std::cerr << " ..... About to call migrad... " << std::endl;
       //
       bool isMigradValid = false;
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
	       const double cc = min.UserCovariance()(i,j);
               fVert.SetCovarianceMatrix((i*3 + j), cc);
	       }
	     }
           }
           for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it = dwnstr.CBegin(); it != dwnstr.CEnd(); it++ ) {
             if (it->ChiSq() > fChiSqCut) continue;
	 // Other slope cuts ? 
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
	} catch (...) { return false; }
       return isMigradValid;
    }
    // 
    void SSDRecVertexAlgo1::openOutputCsvFiles() const {
       //
       // only the UV matching info is here... 
       //
       
       std::ostringstream fNameStrStr;
       fNameStrStr << "SSDRecVertexAlgo1_Run_" << fRunNum << "_" << fTokenJob << "_V2.txt";
       std::string fNameStr(fNameStrStr.str());
       fFOutVert.open(fNameStr.c_str());
       fFOutVert << " spill evt nDwntr X XErr Y YErr Z ZErr chiSq " << std::endl;
       //
     }
     //
     void SSDRecVertexAlgo1::dumpInfoForR() const {
      if (!fFOutVert.is_open()) this->openOutputCsvFiles(); 
       if (fVert.ChiSq() == DBL_MAX) return;
       fFOutVert << " " << fSubRunNum << " " << fEvtNum; 
       fFOutVert << " " << fVert.NumDwnstr() << " " << fVert.X() << " " << fVert.XErr() << " " 
                 << fVert.Y() << " " << fVert.YErr() << " " << fVert.Z() << " " << fVert.ZErr() << " " << fVert.ChiSq() << std::endl;   
     }
  } // namespace ssdr
} // namespace emph
