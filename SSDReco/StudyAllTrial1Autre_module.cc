////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to understand our SSD clusters,, in particular, the relative alignment 
///          between views.  
/// \author  $Author: lebrun $
/*
 We should probably look at 120 GeV first.. 
  Note from Teresa, Thursday, Dec 1 
   to Jack Mirabito, and others, since this is probably useful for everyone looking at the SSDs) 120 GeV runs with probably usable SSD data: 
               1039, 1040, 1042, 1043, 1054, 1055, (Al brick in front of magnet: 1058, 1060, 1062).
The issue with all of these runs was that we saw significantly fewer (~3-6k) triggers on the SSDs than the other detectors, 
so the event matching is in question. It's possible all of the data is usable, 
but until we investigate the event matching we won't know. 
Either way, if you're only looking at individual detectors they should be okay
*/
////////////////////////////////////////////////////////////////////////
// C/C++ includes
// C++ includes
#include <cstddef>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <climits>
#include <cfloat>
#include <cstdlib>
//
// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
//
// simulation results per say.. stuff
// 
#include "RunHistory/RunHistory.h"
#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "RecoBase/SSDCluster.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "MagneticField/test/TestEmphMagneticField.h"
#include "Geometry/service/GeometryService.h"
#include "SSDReco/SSDRecUpstreamTgtAutre.h"
#include "SSDReco/SSDRecStationDwnstrAutre.h"
#include "SSDReco/SSDRecDwnstrTracksAutre.h" 
#include "SSDReco/SSDRecBrickTracksAutre.h"
#include "SSDReco/SSDRecVertexAutre.h" 
//


// emph::ssdr::VolatileAlignmentParams*   emph::ssdr::VolatileAlignmentParams::instancePtr=NULL;

namespace emph {
  class StudyAllTrial1Autre;
}
  //
    
    class emph::StudyAllTrial1Autre : public art::EDAnalyzer {
    
    public:
      
      explicit StudyAllTrial1Autre(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void analyze(const art::Event& evt);
      
  // Plugins should not be copied or assigned.
      StudyAllTrial1Autre(StudyAllTrial1Autre const&) = delete;
      StudyAllTrial1Autre(StudyAllTrial1Autre&&) = delete;
      StudyAllTrial1Autre& operator=(StudyAllTrial1Autre const&) = delete;
      StudyAllTrial1Autre& operator=(StudyAllTrial1Autre&&) = delete;


      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const&);
//      void endRun(art::Run const&); // not needed.. 
//      void endSubRun(art::SubRun const&);
      void endJob();

   private:
//
// Some utilities.. 
//      
      bool fFilesAreOpen;
      std::string fTokenJob;
      std::string fSSDClsLabel;
      std::string fSSDAlignmentResult;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
//      double fSetMCRMomentum; // The momentum for a given run..No longer used, March 2025.. 
      double fPrelimMomentumSecondaries; // for estimate of the multiple scattering uncertainties, pre-fit.  
// Cuts
//
//   Usptream of the target. 
//
     double fChiSqCutXYUVStAutre;
     double fMaxUpstreamSlopeX, fMaxUpstreamSlopeY; 
     double fMaxDistFrom120Beam;
     int fRunSel; // for debugging 
     int fEvtSel; // for debugging 
     int fSpillSel; 
     int fMaxDwnstrStation; 
     int fMaxNumBeamTracks;
     int fMinNumDwnstrTracks;
     int fMaxNumDwnstrTracks;
     bool fDoAntiSt2;
     bool fDoIronBrick;
     bool fDoFirstAndLastStrips;
     bool fDoUseDownstrKalmanTracks; 
     double fXCoeffAlignUncert, fYCoeffAlignUncert;
//
//   Downstream of the target. 
//
     double fDwnstrChiSqCut, fDwnstrChiSqCutPreArb, fDwnstrVertChiSqCut; 
     double fConfirmBrickChiSqCut; 
//
// access to the geometry.   
//
      runhist::RunHistory *fRunHistory;
      art::ServiceHandle<emph::geo::GeometryService> fGeoService;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
//
//    Real work, reconstruction algorithms, done here
//
      ssdr::SSDRecUpstreamTgtAutre fUpStreamBeamTrRec;  // Reconstruct the upstream of the target, Station 0 and 1 (Phase1b)     
      ssdr::SSDRecDwnstrTracksAutre fDwnstrTrRec;  // Downstream, station 2, through 5   
      ssdr::SSDRecBrickTracksAutre fBrickTrRec;  // Downstream, station 2, through 5   
      ssdr::SSDRecVertexAutre fRecVert; // The vertice, done via a non-linear MINUIT2 fit.
//
// Internal stuff 
//
     const double fXPencil120, fYPencil120; // The coordinate, in our Volatile Coord. system of the centroid of the 120 GeV, 
//
// Variable I think I know.. Valid only for the MinValues_Simplex_OK_Try3D_R1274_1h1_1.txt alignment set. 
// 
    double fMeanUpstreamXSlope, fMeanUpstreamYSlope;
     // where the SSD alignment could be considered valid. 
     int fNumClUpstr, fNumClDwnstr, fNumBeamTracks, fNumVertices, fNumVertDwn, fNumDwnsTrkOK;
     std::vector<int> fStationsRecMult;
//
// CSV tuple output..
// 
      std::ofstream fFOutMultSum, fFOutVertices, fFOutVertDwn, fOutScattAngle;
      
//
// access to input data..   
//
      std::vector<art::Ptr<rb::SSDCluster> > fSSDclPtrs; // This is what I got from art, see analyze method. 
      std::vector<rb::SSDCluster> fSSDcls; // we will do a deep copy, as my first attempt at using the above vector failed.. lower case c
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr; // This works, but use the deprecated art interface.. Upper case C
      
      double InvariantMass(std::vector<rb::DwnstrTrackAutre>::const_iterator it1, 
                           std::vector<rb::DwnstrTrackAutre>::const_iterator it2) const;
          // assumeing negligible rest mass particle associted with Downstream tracks. 
      
      void openOutputCsvFiles();
      void dumpSummaryMultiplicities(); 
      
      void studyPrelimScattAngle(const art::Event& evt);
            
    }; 
    
// .....................................................................................
    emph::StudyAllTrial1Autre::StudyAllTrial1Autre(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fSSDClsLabel("?"), fSSDAlignmentResult("none"),
    fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0), 
//    fSetMCRMomentum(30.), 
    fPrelimMomentumSecondaries(5.0), fChiSqCutXYUVStAutre(20.), 
    fMaxUpstreamSlopeX(0.005), fMaxUpstreamSlopeY(0.005), fMaxDistFrom120Beam(100.),
    fRunSel(INT_MAX), fEvtSel(INT_MAX), fSpillSel(INT_MAX), fMaxDwnstrStation(6),
    fMaxNumBeamTracks(10), fMinNumDwnstrTracks(1), fMaxNumDwnstrTracks(20), fDoAntiSt2(false),
    fDwnstrChiSqCut(10.), fDwnstrChiSqCutPreArb(1000.), fDwnstrVertChiSqCut(3.0), 
    fConfirmBrickChiSqCut(500),   
    fRunHistory(nullptr), fEmgeo(nullptr), 
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fUpStreamBeamTrRec(), 
    fXPencil120(-3.8), fYPencil120(4.5), // Obtained in the analysis of 120 GeV runs 1043 and 1055.  Phase1b
//    fMeanUpstreamXSlope(0.0004851102), fMeanUpstreamYSlope(0.002182192), // Only for run 1066, 1067... (31 GeV). 
//    fMeanUpstreamXSlope(0.000811), fMeanUpstreamYSlope(0.002182192), // Only for run 1274 (31 GeV). After retuning TransShift_X_1 from 0.72 to 2.0  
    fMeanUpstreamXSlope(-0.00306), fMeanUpstreamYSlope(0.008883417), // Only for run 2113, aligment 7s1* 
    fNumClUpstr(0), fNumClDwnstr(0), fNumBeamTracks(0), fNumVertices(0), fNumVertDwn(0),
    fNumDwnsTrkOK(0), fStationsRecMult(6, 0) // Downstream station 
    {
       std::cerr << " Constructing StudyAllTrial1Autre " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;       
    }
    
    void emph::StudyAllTrial1Autre::reconfigure(const fhicl::ParameterSet& pset)
    {
      std::cerr << " emph::StudyAllTrial1Autre::reconfigure ... " <<std::endl;
      fSSDClsLabel = pset.get<std::string>("SSDClsLabel");   
      fSSDAlignmentResult = pset.get<std::string>("SSDAlignmentResult", "none");   // Will be optionally overwritten, see below.. 
      std::cerr << "  ... fSSDClsLabel " << fSSDClsLabel << std::endl;   
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fEvtSel = pset.get<int>("EventSelected", INT_MAX);
      fSpillSel = pset.get<int>("SpillSelected", INT_MAX);
      fRunSel = pset.get<int>("RunSelected", INT_MAX);
      fMaxDwnstrStation = pset.get<int>("MaxDwnstrStation", 6);
      fMaxUpstreamSlopeX = pset.get<double>("maxUpstreamSlopeX", 0.005);
      fMaxUpstreamSlopeY = pset.get<double>("maxUpstreamSlopeY", 0.005);
      fMaxDistFrom120Beam = pset.get<double>("maxDistFrom120Beam", 100.);
      fMaxNumBeamTracks = pset.get<int>("maxNumBeamTracks", 20);
      fMinNumDwnstrTracks = pset.get<int>("minNumDwnstrTracks", 1);
      fMaxNumDwnstrTracks = pset.get<int>("maxNumDwnstrTracks", 20);
      fDoAntiSt2 = pset.get<bool>("doAntiSt2", false);
      fDoIronBrick = pset.get<bool>("doIronBrick", false);
      fDoFirstAndLastStrips = pset.get<bool>("doFirstAndLastStrips", false);
      fDoUseDownstrKalmanTracks = pset.get<bool>("doUseDownstrKalmanTracks", false);
      fConfirmBrickChiSqCut = pset.get<double>("confirmBrickChiSqCut", 500.);
      fPrelimMomentumSecondaries = pset.get<double>("prelimMomentum", 5.);
      fDwnstrChiSqCut = pset.get<double>("dwnstrChiSqCut", 25.);
      fDwnstrChiSqCutPreArb = pset.get<double>("dwnstrChiSqCutPreArb", 1000.);
      fDwnstrVertChiSqCut = pset.get<double>("dwnstrVertChiSqCutPreArb", 3.);
      fChiSqCutXYUVStAutre = pset.get<double>("chiSqCutXYUVStAutre", 20.);
      fXCoeffAlignUncert = pset.get<double>("XCoeffAlignUncert", 0.05);
      fYCoeffAlignUncert = pset.get<double>("YCoeffAlignUncert", 0.025);
      // 
      // Transfering this info to algorithms.. 
      //
      fUpStreamBeamTrRec.SetTokenJob(fTokenJob);
      if (!fDoIronBrick) {
        fDwnstrTrRec.SetTokenJob(fTokenJob);
	fDwnstrTrRec.SetChiSqCutPreArb(fDwnstrChiSqCutPreArb);
	fDwnstrTrRec.SetChiSqCut(fDwnstrChiSqCut);
      } else { 
        fBrickTrRec.SetTokenJob(fTokenJob);
	fBrickTrRec.SetDistFromBeamCenterCut(fMaxDistFrom120Beam);
	fBrickTrRec.SetChiSqCut(fConfirmBrickChiSqCut);
      }
      // Upload the latest alignment 
      const std::string alignParamsStr = pset.get<std::string>("alignParamFileName");
      const char *pathHere = std::getenv("CETPKG_BUILD");
      const std::string ffName(pathHere + std::string("/") + alignParamsStr) ;
      fSSDAlignmentResult = ffName;     
      std::cerr << " .... O.K. done with reconfigure,  keep going ....  " << std::endl; 
    }
    void emph::StudyAllTrial1Autre::beginRun(art::Run const &run)
    {
      std::cerr << " StudyAllTrial1Autre::beginRun, run " << run.id() << std::endl;
      if (fRunHistory != nullptr) return; // Do this once ! 
      fRun = static_cast<int>(run.run());
      fRunHistory = new runhist::RunHistory(fRun);
      std::cerr << " ..... Before accessing the geometry service  " << std::endl;
      fEmgeo = fGeoService->Geo();
      std::cerr << " ... after accessing the geometry service... Checking the Views assignment.. Phase1b or phase1c... " << std::endl;
      for (int kSt = 0; kSt != fEmgeo->NSSDStations(); kSt++) {
        const emph::geo::SSDStation* aSt = fEmgeo-> GetSSDStation(kSt); 
	for (int kPl = 0; kPl != aSt->NPlanes(); kPl++) { 
	  const emph::geo::Plane* aPl = aSt->GetPlane(kPl);
	  std::cerr << " ... Station " << kSt << ", ( " << aSt->Name() << "), " << " Plane " 
	            << kPl << " View " << aPl->View() << std::endl;
	  for (int kSSD=0; kSSD != aPl->NSSDs(); kSSD++) {
	    const emph::geo::Detector *aDet = aPl->SSD(kSSD); 
	    std::cerr << " ... ... Sensor " << kSSD << " View " << aDet->View() << std::endl;
	  } // on sensors; 
	} // on planes;; 
      } // on stations... 
      art::ServiceHandle<emph::MagneticFieldService> bField;
      emph::MagneticField *fMagField = bField->Field();
//      fMagField->SetVerbosity(1);
//      emph::TestEmphMagneticField testMag(fMagField);
//       testMag.test2(); 
//      std::cerr << " And quit after testing integration.. " << std::endl; exit(2);
//      
      // October 12, need to check the new alignment.. In Z.. Jonathan looked at at it Probably, very likely, O.K... 
//      fMagField->G4GeomAlignIt(fEmgeo);

      fUpStreamBeamTrRec.SetRun(fRun);
      //
      // My own convention..Ugly MC/real data difference.. 
      //
      if (fRun == 1293) fDwnstrTrRec.SetForMC(true);
      const size_t maxKst = (fRun < 2000) ? 6 : 7; // Station 7 appears to be empty on run 2098, 2113 
      if (!fDoIronBrick) { 
        std::cerr << " Not doing Iron brick... " << std::endl;
        if (fRun == 1274) fUpStreamBeamTrRec.SetNominalMomentum(31.);
        fDwnstrTrRec.SetRun(fRun);
        fDwnstrTrRec.SetPreliminaryMomentum(fPrelimMomentumSecondaries);
	fDwnstrTrRec.SetMaxDwnstrStation(fMaxDwnstrStation);
        for(size_t kSt=2; kSt != maxKst; kSt++)  {
          fDwnstrTrRec.SetChiSqCutRecStation(kSt, fChiSqCutXYUVStAutre); // it is done...  
//          if (kSt == 6) fDwnstrTrRec.SetChiSqCutRecStation(kSt, 1.0e6); // Understanding Station 6 V view ??? 
// Fixed, January 4 2024 
        }
// January 16, study systematic uncertainties due to misalignment
        fUpStreamBeamTrRec.SetCoeffsAlignUncert(fXCoeffAlignUncert, fYCoeffAlignUncert);
        fDwnstrTrRec.SetCoeffsAlignUncert(fXCoeffAlignUncert, fYCoeffAlignUncert);
	std::cerr << " .... SetCoeffsAlignUncert, fXCoeffAlignUncert " << fXCoeffAlignUncert << std::endl;
	std::cerr << " Finished setting up Downstream Tracker.. " << std::endl;
      } else { 
      // Not applicable for Phase1c.. 
        fBrickTrRec.SetRun(fRun);
        fBrickTrRec.SetAssumedMomentum(fPrelimMomentumSecondaries);
	fBrickTrRec.SetDoFirstAndLastStrips(fDoFirstAndLastStrips);
        for(size_t kSt=2; kSt != 6; kSt++)  {
          fBrickTrRec.SetChiSqCutRecStation(kSt, fChiSqCutXYUVStAutre); // should be done 
        }
       }
       std::cerr << " StudyAllTrial1Autre::beginRun, before SetPhase1X, fRun " << fRun << std::endl;
       fEmVolAlP->SetPhase1X(fRun);
       std::cerr << " StudyAllTrial1Autre::beginRun, uploading alignment data from file " << fSSDAlignmentResult << std::endl;
        fEmVolAlP->UpdateNominalFromStandardGeom(fEmgeo);
       if (fSSDAlignmentResult.find("none") == std::string::npos) fEmVolAlP->SetGeomFromSSDAlign(fSSDAlignmentResult); 
       	
       std::cerr  << std::endl << " ------------- End of StudyAllTrial1Autre::beginRun ------------------" << std::endl << std::endl;
    }
    
    void emph::StudyAllTrial1Autre::beginJob()
    {
    }
    void emph::StudyAllTrial1Autre::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " StudyAllTrial1Autre::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      std::ostringstream fNameMultStrStr; fNameMultStrStr << "./StudyAllTrial1Autre_Mult_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameMultStr(fNameMultStrStr.str());
      fFOutMultSum.open(fNameMultStr.c_str());
      fFOutMultSum << " spill evt numClUp nClDwnstr nUpstrTr nBeamTr nStPt2 nStPt3 nStPt4 nStPt5 nStPt6 nTrsDwn nVert nVertDwn " << std::endl;
      //
      std::ostringstream fNameVertStrStr; fNameVertStrStr << "./StudyAllTrial1Autre_Vertices_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameVertStr(fNameVertStrStr.str());
      fFOutVertices.open(fNameVertStr.c_str());
      fFOutVertices << " spill evt iV  itUp itDwn zx zxErr zy zyErr xVUp yVUp " << std::endl;  
      std::ostringstream fNameVertDStrStr; fNameVertDStrStr << "./StudyAllTrial1Autre_VertDwn_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameVertDStr(fNameVertDStrStr.str());
      fFOutVertDwn.open(fNameVertDStr.c_str());
      fFOutVertDwn << " spill evt iV  itDwn1 itDwn2 zx zxErr zy zyErr xVDwn yVDw xVUp yVUp  " << std::endl;  
      fFilesAreOpen = true;
    }
    
    void emph::StudyAllTrial1Autre::endJob() {
      std::cerr << " StudyAllTrial1Autre::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      fFOutVertices.close(); fFOutMultSum.close(); fFOutVertDwn.close();
    }
    
    void emph::StudyAllTrial1Autre::dumpSummaryMultiplicities() { 
    
//        fFOutMultSum << " subRun evt numClUp nClD nUpstrTr nStPt2  nStPt3 nStPt4 nStPt5 nStPt2 nTrsDwn " << std::endl;
        fFOutMultSum << " " << fSubRun << " " << fEvtNum << " " << fNumClUpstr << " " << fNumClDwnstr;
	if ((!fDoIronBrick) && (fUpStreamBeamTrRec.Size() == 0)) {
	  for (int k=0; k!= 10; k++) fFOutMultSum << " 0"; 
	  fFOutMultSum << std::endl;
	  return;
	} else {
	  fFOutMultSum << " " << fUpStreamBeamTrRec.Size() << " " << fNumBeamTracks;  
	}
	for (size_t k = 0; k != 5; k++) fFOutMultSum << " " << fStationsRecMult[k]; 
	if (!fDoIronBrick) {
	   fFOutMultSum << " " << fNumDwnsTrkOK << " " << fNumVertices <<  " " << fNumVertDwn << std::endl;
	} else {
	   fFOutMultSum << " " << fBrickTrRec.Size() << " " << fNumVertices <<  " 0 "  << std::endl;
	}   
    }
    
    void emph::StudyAllTrial1Autre::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      if (fEvtNum > 14460) std::cerr << " emph::StudyAllTrial1Autre::analyze, at event " << fEvtNum << " spill " << fSubRun << std::endl; 
      
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
//      const bool debugIsOn = ((fSubRun == 100) && (fEvtNum == 700)) ; // Real data, for Iron Brick run 
//      const bool debugIsOn = ((fEvtNum == 51) || (fEvtNum == 106) || (fEvtNum == 226) || (fEvtNum == 736)); // Multi track events, MC run 
//      const bool debugIsOn = ((fEvtNum == 106) || (fEvtNum == 226) || (fEvtNum == 736)); // Multi track events, MC run 
//     const bool debugIsOn = (fEvtNum > 81717); // root-Minuit crash at evt 81719 fixed by eliminating very back prelim chi-sq 
//     const bool debugIsOn = ((fSubRun == 100) && ((fEvtNum == 48 ) || (fEvtNum == 56 ) || (fEvtNum == 142 ) || (fEvtNum == 92 )));
       // For spill 100, good track from RScript, check loading 
//       const bool debugIsOn = (fEvtNum < 20);
//       bool debugIsOn = false;
// Run 1274, sorting out Station 4 and 5, negative Y... Running on Spill 10 only
//       debugIsOn = ((fRun == 1274) && ((fEvtNum == 1209) || (fEvtNum == 3462) || (fEvtNum == 3472) || 
//                               (fEvtNum == 4244) || (fEvtNum == 4476) || (fEvtNum == 6191)));
//	debugIsOn = ((fRun == 1293) && (fEvtNum < 20));		       
//	debugIsOn = ((fRun == 1274) && (fEvtNum < 20));		       
//	debugIsOn = ((fRun == 1274) && (fSubRun == 10) && ((fEvtNum == 183) || (fEvtNum == 671)) );		       
    //
//      bool debugIsOn = ((fRun == 1274) && (fSubRun == 10) && (fEvtNum == 5));
//      bool debugIsOn = ((fRun == 1274) && (fSubRun == 10) && 
//          ((fEvtNum == 52) || (fEvtNum == 59) || (fEvtNum == 63) || (fEvtNum == 71) || (fEvtNum == 98)) );
//      bool debugIsOn = ((fRun == 1274) && (fSubRun == 2) && (fEvtNum == 9584));
//      if (fEvtNum > 10000) { std::cerr << " evt 10000... Crash investigation.. hold off " <<std::endl; exit(2); } 
    // Get the data. This is supposed the best way, but... 
//      bool debugIsOn = ((fRun == 1274) && (fSubRun == 18) && (fEvtNum == 12139)); // Bad event Root 300 error, Nov 25 2023. 
//      bool debugIsOn = ((fRun == 1274) && (fSubRun == 36) && (fEvtNum == 3539)); // Bad event Root 300 error, Nov 25 2023. 
// This is no longer needed, after the Minuit2 exception handle upgrade, late November 2023.. 
//      
//      bool debugIsOn = ((fRun == 2113) && (fSubRun == 10) && (fEvtNum == 2977));
      bool debugIsOn = ((static_cast<int>(fRun) == fRunSel) && (static_cast<int>(fSubRun) == fSpillSel) && (static_cast<int>(fEvtNum) == fEvtSel));
//      bool debugIsOn = false;
//        bool debugIsOn = ((fRun == 2142) && (fEvtNum < 20));	
//      bool debugIsOn = ((fRun == 2113) && (fSubRun == 10) && 
//                        ((fEvtNum == 1) || (fEvtNum == 2 ) || (fEvtNum == 4 )|| (fEvtNum == 5 ) || (fEvtNum == 9 ) || (fEvtNum == 10 )));
//      if (fEvtNum > 25) { std::cerr << " ... ... only first few event, quit now.. " << std::endl; exit(2); }
//      if (!debugIsOn) return ; // skip, got for the first clean event.. 
      if ((fRun == 1274) && (fSubRun == 36) && (fEvtNum == 3539)) return ; // Skip fit fails..       
      if ((fRun == 1274) && (fSubRun == 31) && (fEvtNum == 10320)) return ; // Skip fit fails..       
//      if ((fRun == 1274) && (fSubRun == 3) && (fEvtNum == 10298)) return ; // Skip fit fails..       
      if ((fRun == 1274) && (fSubRun == 39) && (fEvtNum == 16060)) return ; // Skip fit fails..       
      if ((fRun == 1274) && (fSubRun == 5) && (fEvtNum == 8582)) return ; // Skip fit fails..       
      if ((fRun == 1274) && (fSubRun == 9) && (fEvtNum == 4612)) return ; // Skip fit fails..       
      if ((fRun == 1066) && (fSubRun == 11) && (fEvtNum == 11489)) return ; // Skip fit fails..       
      if ((fRun == 1066) && (fSubRun == 42) && (fEvtNum == 8119)) return ; // Skip fit fails..       
      if ((fRun == 1066) && (fSubRun == 32) && (fEvtNum == 1309)) return ; // Skip fit fails..       
      if ((fRun == 1066) && (fSubRun == 21) && (fEvtNum == 2835)) return ; // Skip fit fails..       
      if ((fRun == 1066) && (fSubRun == 49) && (fEvtNum == 4143)) return ; // Skip fit fails..       
      if ((fRun == 1066) && (fSubRun == 48) && (fEvtNum == 9454)) return ; // Skip fit fails..       
      auto hdlCls = evt.getHandle<std::vector<rb::SSDCluster>>(fSSDClsLabel);
      art::fill_ptr_vector(fSSDclPtrs, hdlCls);
      
      if (debugIsOn) std::cerr << " Debugging.. StudyAllTrial1Autre::analyze, run " << fRun 
                               << "  spill " << fSubRun << " event " <<  fEvtNum << "  on " 
                               << fSSDclPtrs.size() <<  " clusters " <<   std::endl;
			       
      fNumBeamTracks = 0; fNumVertices = 0; fNumVertDwn = 0; fNumDwnsTrkOK=0;
      for (size_t k=0; k != fStationsRecMult.size(); k++) fStationsRecMult[k] = 0;
      
//      if (fEvtNum > 10) { std::cerr << " 10 evt is enough, quit here and now " << std::endl; exit(2); }
// Only 20000 events, should see some interaction in the target.. 		       
//	if (fEvtNum > 20000) return;
      if ((fSpillSel < 1000) && (!debugIsOn)) return;	
	
      fUpStreamBeamTrRec.Reset();
      
      //
      evt.getByLabel (fSSDClsLabel, fSSDClsPtr);
      
      if (fNEvents < 20){
         std::cerr << " Number SSDClusters, deprecated interface (with deep copy) " << fSSDcls.size() << std::endl;
         fSSDcls = (*fSSDClsPtr); // deep copy here. Probably can be avoided.. Unless we want to make changes to the cluster, which we 
         if (fSSDcls.size() != 0) {
          std::vector<rb::SSDCluster>::const_iterator itClsTest = fSSDcls.cbegin(); 
          std::cerr << " Station for the first cluster, old interface   " << itClsTest->Station() << std::endl;
          std::vector<rb::SSDCluster>::const_iterator itClsTest2 = fSSDClsPtr->cbegin(); 
          std::cerr << " Station for the first cluster, old interface no deep copy   " << itClsTest2->Station() << std::endl;
	  
        }
      }
      
      if (fSSDclPtrs.size() == 0) { this->dumpSummaryMultiplicities();  return; } // nothing to do, no data. 
      fNumClUpstr = 0; fNumClDwnstr = 0; fNumVertices = 0; fNumVertDwn = 0;
      // 
      // Tally the number of clusters..
      //
      for (std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) { 
        if (itCl->Station() < 2) fNumClUpstr++;
	else fNumClDwnstr++; 
      }
      if (debugIsOn) std::cerr << " ... Num Upstream Clusters.. " << fNumClUpstr << std::endl;
      if (fNumClUpstr == 0) { this->dumpSummaryMultiplicities();  return; } // No upstream data. 
      //
      // Beam Tracks 
      //
      std::vector<rb::BeamTrack>::const_iterator itUpTrSel; 
      if (!fDoIronBrick) { 
        fUpStreamBeamTrRec.SetSubRun(fSubRun); fUpStreamBeamTrRec.SetEvtNum(fEvtNum);
	if (debugIsOn) std::cerr << " about to call fUpStreamBeamTrRec.recoXY... " << std::endl;
        fUpStreamBeamTrRec.recoXY(fSSDClsPtr);
//	if ((fNEvents < 25000) && (fNumClUpstr < 6) ) fUpStreamBeamTrRec.dumpXYInforR(INT_MAX);
        //
        // Cuts on beam track here. 
        //
        if (fUpStreamBeamTrRec.Size() == 0) { this->dumpSummaryMultiplicities();  return; }
        fUpStreamBeamTrRec.dumpXYInforR(INT_MAX);
        if (debugIsOn) std::cerr << " ... Back to Analyze, got " << fUpStreamBeamTrRec.Size() << " upstream track(s) " << std::endl;
        size_t kTr=0; 
        itUpTrSel = fUpStreamBeamTrRec.CEnd(); 
        for(std::vector<rb::BeamTrack>::const_iterator itUpTr = fUpStreamBeamTrRec.CBegin();  
	   itUpTr != fUpStreamBeamTrRec.CEnd(); itUpTr++, kTr++) {
          const double deltaX120 = itUpTr->XOffset() - fXPencil120; const double deltaY120 = itUpTr->YOffset() - fYPencil120;
	  const double dist120 = std::sqrt(deltaX120*deltaX120 + deltaY120*deltaY120);
	  if (debugIsOn) std::cerr << " .... X0 " << itUpTr->XOffset() << " Y0 " << itUpTr->YOffset() 
	                           << "  Distance from 120 GeV Beam center " << dist120 << " Track slopes " << itUpTr->XSlope() 
	          << " , " << itUpTr->YSlope() << std::endl;
	  if (dist120 > fMaxDistFrom120Beam) { if (debugIsOn) std::cerr << " ... rejected, too far from beam center " << std::endl; continue; }
	  if ((std::abs(itUpTr->XSlope() - fMeanUpstreamXSlope) > fMaxUpstreamSlopeX) || 
	    (std::abs(itUpTr->YSlope() - fMeanUpstreamYSlope) > fMaxUpstreamSlopeY)) {
	     if (debugIsOn) std::cerr << " ... rejected, too large slope  " << std::endl;
	     continue;
	  }
	  itUpTr->SetUserFlag(1);
	  itUpTrSel = itUpTr;
	  fNumBeamTracks++;
        }
        if (debugIsOn) std::cerr << " ... Beam Track tally " << fNumBeamTracks << " max is " << fMaxNumBeamTracks << std::endl;
        if (fNumBeamTracks == 0) { 
          if (debugIsOn) std::cerr << " ... No Beam track, rejected," << std::endl; 
          this->dumpSummaryMultiplicities();  return; 
        }
        if (fNumBeamTracks > fMaxNumBeamTracks || (fUpStreamBeamTrRec.Size() > static_cast<size_t>(fMaxNumBeamTracks))) { 
          if (debugIsOn) std::cerr << " ... Rejected Too many beam tracks," << fNumBeamTracks << " max is " << fMaxNumBeamTracks << std::endl;
          this->dumpSummaryMultiplicities();  return; 
        }
      } // default reconstruction, upstream of the real target, starting with Upstream (so called Beam) track(s). 
      //
      // Now reconstructed the Downstream (of the target) Space points. If we have the Iron Brick in front of Station 0, then 
      // Special operation, as our buddy Vladimir Putin would say.. 
      //
      bool isCleanSingleTrack = (fNumClUpstr < 5); // hopefully, clean Station 1 and 0 
      if (!fDoIronBrick) {
        fDwnstrTrRec.SetItUpstreamTrack(itUpTrSel);
        if ((fRun == 2113) || (fRun == 2098)) fDwnstrTrRec.SetDoUseUpstreamTrack(); 
	  // Only for checking alignment run, if search for multi track evts, open the search to all possibilities.  
        fDwnstrTrRec.SetDebugOn(debugIsOn);
	size_t numStsDwnstrMax = (fRun < 2000) ? 6 : 7;
	if (fRun > 1999) 
        for(size_t kSt=2; kSt != numStsDwnstrMax; kSt++)  {
	  fStationsRecMult[kSt-2] = fDwnstrTrRec.RecStation(kSt, evt, fSSDClsPtr);
	  if (debugIsOn) std::cerr << std::endl << " Station " << kSt << " done, with " << fStationsRecMult[kSt-2] 
	                           << " reconstructed space points "  << std::endl << std::endl;
	  if ((kSt == 3)  &&  (fDwnstrTrRec.NumTripletsSt2and3() == 0)) {
	    if (debugIsOn) std::cerr << " Neither Station 2 not 3 have a triplet.. Suspicious event, abandon.. " << std::endl;
            fDwnstrTrRec.dumpStInfoForR();
	    this->dumpSummaryMultiplicities();  
	    return; 
	  }
        }
	if (fDwnstrTrRec.NumTripletsSt5and6() == 0) {
	    if (debugIsOn) std::cerr << " Neither Station 5 not 6 have a triplet.. junk event, abandon.. " << std::endl;
            fDwnstrTrRec.dumpStInfoForR();
	    this->dumpSummaryMultiplicities();  
	    return; 
	}
      } else {
        fBrickTrRec.SetDebugOn(debugIsOn); 
        for(size_t kSt=2; kSt != 6; kSt++)  {
	  fStationsRecMult[kSt-2] = fBrickTrRec.RecStation(kSt, evt, fSSDClsPtr);
	  if (fStationsRecMult[kSt-2] != 1) isCleanSingleTrack = false;
        }
      }
      if ((fDoAntiSt2) && (fStationsRecMult[0] > 0))  {
	  this->dumpSummaryMultiplicities();  
	  return; 
      }
//      if (fDoAntiSt2) std::cerr << " No XY hits in Station2, and aintiSt2 is on for evt " << fEvtNum << std::endl; 
      //
      // Now downstream tracks.. 
      //
      if (fDoIronBrick) {
        if ((!isCleanSingleTrack) && (fMaxNumDwnstrTracks == 1)) { this->dumpSummaryMultiplicities(); return; } 
        fBrickTrRec.RecAndFitIt(evt, fSSDClsPtr);     
	fBrickTrRec.dumpStInfoForR(fSSDClsPtr);
        fBrickTrRec.dumpInfoForR();
	this->dumpSummaryMultiplicities();  
//        if (fEvtNum > 20) { std::cerr << " Event " << fEvtNum << " done , quit here and now .. " << std::endl; exit(2); }
        return ; 
      } 
      fDwnstrTrRec.RecAndFitIt(evt);
      fNumDwnsTrkOK = fDwnstrTrRec.SizeOK(); 
      // 
      // Saving for alignment ? 
      //
       if (((fRun == 2113) ||(fRun == 2098)) && fDwnstrTrRec.IsPerfectForAlignment()) {
      	    if (debugIsOn)  std::cerr << " Saving compact format for alignment..... " << std::endl;
           fDwnstrTrRec.dumpCompactEvt(fSSDClsPtr);
       }
       if ((fRun == 2144) && fDwnstrTrRec.HasGoodHighPForAlignment(50.)) {
      	    if (debugIsOn)  std::cerr << " Saving compact format for alignment..... " << std::endl;
           fDwnstrTrRec.dumpCompactEvt(fSSDClsPtr);
       }
      
      if (debugIsOn) std::cerr << std::endl<<  " ... ... Back to art analyze... After dumpCompactEvt " << std::endl;
      fDwnstrTrRec.dumpStInfoForR();
      if (debugIsOn) std::cerr << std::endl<<  " ... ... Back to art analyze... After dumpStInfoForR " << std::endl;
      // To be re-implemented, if need be... 
//      bool gotBeamOrScatter = fDwnstrTrRec.doUpDwn3DClFitAndStore();
      bool gotBeamOrScatter = (fDwnstrTrRec.Size() == 1);
      if ( gotBeamOrScatter && (!fDoUseDownstrKalmanTracks)) fDwnstrTrRec.dumpBeamTracksCmp(false);
      // 
      // Kalman filter results, if any.. 
      //
      if (debugIsOn) std::cerr << std::endl<<  " ... ... Back to art analyze... Before dumpBeamTracksCmpKlm  " << std::endl;
      fDwnstrTrRec.dumpBeamTracksCmpKlm();
      if (fDoUseDownstrKalmanTracks) {
         fDwnstrTrRec.dumpBeamTracksCmp(false);
      }
      
      //
      // Cut on track multiplities 
      // 
      if (debugIsOn) std::cerr << " .... After downstream reconstruction, number of Dwnstr tracks " << fDwnstrTrRec.Size() << std::endl;
      if(fDwnstrTrRec.Size() < static_cast<size_t>(fMinNumDwnstrTracks)) { this->dumpSummaryMultiplicities();  return; }
      if(fDwnstrTrRec.Size() > static_cast<size_t>(fMaxNumDwnstrTracks)) { this->dumpSummaryMultiplicities();  return; }
      if (debugIsOn) std::cerr << " .... O.K., Number of downstream tracks...  " << fDwnstrTrRec.Size() << std::endl;
      
      fDwnstrTrRec.dumpInfoForR();
      if (debugIsOn) std::cerr << " Check evt number.. after dumping Dwn tracks, evtNum =   " << fEvtNum << std::endl;
      //
      // Temporary stop 
      //
//      if (debugIsOn) { std::cerr << " .... O.K., Back to Analyze of art module, enough is enough .. " << std::endl; exit(2); }
      // Vertex analysis donw here... This should be in a SSD vertex reconstruction package. 
      
      // Vertex Analysis, done better.. We keep the code below for compare..
      //
      if (fUpStreamBeamTrRec.Size() == 1) {
        if (fDoUseDownstrKalmanTracks) fDwnstrTrRec.transferKlmToClFit();
        fRecVert.SetDebugOn(debugIsOn);
	fRecVert.SetTokenJob(fTokenJob);
	fRecVert.SetChiSqCut(fDwnstrChiSqCut);
	std::vector<rb::BeamTrack>::const_iterator itUp =  fUpStreamBeamTrRec.CBegin();
	if (debugIsOn) std::cerr << " .... Ready for Vertex analysis,  one upstream track, " << fDwnstrTrRec.Size() << " downstream tracks " << std::endl;
        fRecVert.RecAndFitIt(evt, itUp, fDwnstrTrRec);
	fNumVertices = 1;
	fRecVert.dumpInfoForR();
//	std::cerr << " ... ... And after vertexing gove up, stop cold here and now " << std::endl; exit(2);
      }
      // Limit the multiplicities.. 
      this->dumpSummaryMultiplicities();  
//      if (debugIsOn) { 
//          std::cerr << " .... O.K., End of Analyze of art module, enough is enough .. " << std::endl; 
//	  fFOutVertDwn.close(); fFOutVertices.close(); fFOutMultSum.close();
//          exit(2); 
//      }
      return;
	
    } // end of Analyze, event by events.  
    double emph::StudyAllTrial1Autre::InvariantMass(std::vector<rb::DwnstrTrackAutre>::const_iterator it1, 
                                              std::vector<rb::DwnstrTrackAutre>::const_iterator it2) const {
					      
					      
       const double px1 = it1->XSlope()*std::abs(it1->Momentum()); 
       const double py1 = it1->YSlope()*std::abs(it1->Momentum()); 
       const double pz1 = std::sqrt(it1->Momentum()*it1->Momentum() - px1*px1 - py1*py1); 
       const double px2 = it2->XSlope()*std::abs(it2->Momentum()); 
       const double py2 = it2->YSlope()*std::abs(it2->Momentum()); 
       const double pz2 = std::sqrt(it2->Momentum()*it2->Momentum() - px2*px2 - py2*py2);
       const double deltaPx = px2-px1; const double deltaPy = py2-py1;   double deltaPz = pz2-pz1;
//       std::cerr << " StudyAllTrial1Autre::InvariantMass,  px1 " << px1 << " px2 " << px2 
//                 << " py1 " << py1 <<  " py2 " << py2 <<  " pz1 " << pz1 << " pz2 " << pz2 << std::endl;
       return std::sqrt(deltaPx*deltaPx + deltaPy*deltaPy + deltaPz*deltaPz); 
       				      
    }
    void emph::StudyAllTrial1Autre::studyPrelimScattAngle(const art::Event& evt) {
      
    
    }
    				      
DEFINE_ART_MODULE(emph::StudyAllTrial1Autre)
