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
#include "SSDReco/SSDRecUpstreamTgtAlgo1.h"
#include "SSDReco/SSDRecStationDwnstrAlgo1.h"
#include "SSDReco/SSDRecDwnstrTracksAlgo1.h" 
#include "SSDReco/SSDRecBrickTracksAlgo1.h"
#include "SSDReco/SSDRecVertexAlgo1.h" 
//


// emph::ssdr::VolatileAlignmentParams*   emph::ssdr::VolatileAlignmentParams::instancePtr=NULL;

namespace emph {
  class StudyAllTrial1Algo1;
}
  //
    
    class emph::StudyAllTrial1Algo1 : public art::EDAnalyzer {
    
    public:
      
      explicit StudyAllTrial1Algo1(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void analyze(const art::Event& evt);
      
  // Plugins should not be copied or assigned.
      StudyAllTrial1Algo1(StudyAllTrial1Algo1 const&) = delete;
      StudyAllTrial1Algo1(StudyAllTrial1Algo1&&) = delete;
      StudyAllTrial1Algo1& operator=(StudyAllTrial1Algo1 const&) = delete;
      StudyAllTrial1Algo1& operator=(StudyAllTrial1Algo1&&) = delete;


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
      double fSetMCRMomentum; // The momentum for a given run..
      double fPrelimMomentumSecondaries; // for estimate of the multiple scattering uncertainties, pre-fit.  
// Cuts
//
//   Usptream of the target. 
//
     double fChiSqCutXYUVStAlgo1;
     double fMaxUpstreamSlopeX, fMaxUpstreamSlopeY; 
     double fMaxDistFrom120Beam; 
     int fMaxNumBeamTracks;
     int fMinNumDwnstrTracks;
     int fMaxNumDwnstrTracks;
     bool fDoAntiSt2;
     bool fDoIronBrick;
     bool fDoFirstAndLastStrips;
//
//   Downstream of the target. 
//
     double fDwnstrChiSqCut, fDwnstrChiSqCutPreArb, fDwnstrVertChiSqCut; 
     double fConfirmBrickChiSqCut; 
//
// access to the geometry.   
//
      runhist::RunHistory *fRunHistory;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
//
//    Real work, reconstruction algorithms, done here
//
      ssdr::SSDRecUpstreamTgtAlgo1 fUpStreamBeamTrRec;  // Reconstruct the upstream of the target, Station 0 and 1 (Phase1b)     
      ssdr::SSDRecDwnstrTracksAlgo1 fDwnstrTrRec;  // Downstream, station 2, through 5   
      ssdr::SSDRecBrickTracksAlgo1 fBrickTrRec;  // Downstream, station 2, through 5   
      ssdr::SSDRecVertexAlgo1 fRecVert; // The vertice, done via a non-linear MINUIT2 fit.
//
// Internal stuff 
//
     const double fXPencil120, fYPencil120; // The coordinate, in our Volatile Coord. system of the centroid of the 120 GeV, 
//
// Variable I think I know.. Valid only for the MinValues_Simplex_OK_Try3D_R1274_1h1_1.txt alignment set. 
// 
    double fMeanUpstreamXSlope, fMeanUpstreamYSlope;
     // where the SSD alignment could be considered valid. 
     int fNumClUpstr, fNumClDwnstr, fNumBeamTracks, fNumVertices, fNumVertDwn;
     std::vector<int> fStationsRecMult;
//
// CSV tuple output..
// 
      std::ofstream fFOutMultSum, fFOutVertices, fFOutVertDwn;
      
//
// access to input data..   
//
      std::vector<art::Ptr<rb::SSDCluster> > fSSDclPtrs; // This is what I got from art, see analyze method. 
      std::vector<rb::SSDCluster> fSSDcls; // we will do a deep copy, as my first attempt at using the above vector failed.. lower case c
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr; // This works, but use the deprecated art interface.. Upper case C
      
      double InvariantMass(std::vector<rb::DwnstrTrackAlgo1>::const_iterator it1, 
                           std::vector<rb::DwnstrTrackAlgo1>::const_iterator it2) const;
          // assumeing negligible rest mass particle associted with Downstream tracks. 
      
      void openOutputCsvFiles();
      void dumpSummaryMultiplicities(); 
            
    }; 
    
// .....................................................................................
    emph::StudyAllTrial1Algo1::StudyAllTrial1Algo1(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fSSDClsLabel("?"), fSSDAlignmentResult("none"),
    fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0), 
    fSetMCRMomentum(30.), fPrelimMomentumSecondaries(5.0), fChiSqCutXYUVStAlgo1(20.), 
    fMaxUpstreamSlopeX(0.005), fMaxUpstreamSlopeY(0.005), fMaxDistFrom120Beam(100.),
    fMaxNumBeamTracks(10), fMinNumDwnstrTracks(1), fMaxNumDwnstrTracks(20), fDoAntiSt2(false),
    fDwnstrChiSqCut(10.), fDwnstrChiSqCutPreArb(20.), fDwnstrVertChiSqCut(3.0), 
    fConfirmBrickChiSqCut(500),   
    fRunHistory(nullptr), fEmgeo(nullptr), 
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fUpStreamBeamTrRec(), 
    fXPencil120(-3.8), fYPencil120(4.5), // Obtained in the analysis of 120 GeV runs 1043 and 1055.  Phase1b
    fMeanUpstreamXSlope(0.0004851102), fMeanUpstreamYSlope(0.002182192), // Only for run 1066, 1067... (31 GeV). 
    fNumClUpstr(0), fNumClDwnstr(0), fNumBeamTracks(0), fNumVertices(0), fNumVertDwn(0), fStationsRecMult(4, 0)
    {
       std::cerr << " Constructing StudyAllTrial1Algo1 " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;       
    }
    
    void emph::StudyAllTrial1Algo1::reconfigure(const fhicl::ParameterSet& pset)
    {
      std::cerr << " emph::StudyAllTrial1Algo1::reconfigure ... " <<std::endl;
      fSSDClsLabel = pset.get<std::string>("SSDClsLabel");   
      fSSDAlignmentResult = pset.get<std::string>("SSDAlignmentResult", "none");   
      std::cerr << "  ... fSSDClsLabel " << fSSDClsLabel << std::endl;   
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fMaxUpstreamSlopeX = pset.get<double>("maxUpstreamSlopeX", 0.005);
      fMaxUpstreamSlopeY = pset.get<double>("maxUpstreamSlopeY", 0.005);
      fMaxDistFrom120Beam = pset.get<double>("maxDistFrom120Beam", 100.);
      fMaxNumBeamTracks = pset.get<int>("maxNumBeamTracks", 20);
      fMinNumDwnstrTracks = pset.get<int>("minNumDwnstrTracks", 1);
      fMaxNumDwnstrTracks = pset.get<int>("maxNumDwnstrTracks", 20);
      fDoAntiSt2 = pset.get<bool>("doAntiSt2", false);
      fDoIronBrick = pset.get<bool>("doIronBrick", false);
      fDoFirstAndLastStrips = pset.get<bool>("doFirstAndLastStrips", false);
      fConfirmBrickChiSqCut = pset.get<double>("confirmBrickChiSqCut", 500.);
      fPrelimMomentumSecondaries = pset.get<double>("prelimMomentum", 5.);
      fDwnstrChiSqCut = pset.get<double>("dwnstrChiSqCut", 10.);
      fDwnstrChiSqCutPreArb = pset.get<double>("dwnstrChiSqCutPreArb", 100.);
      fDwnstrVertChiSqCut = pset.get<double>("dwnstrVertChiSqCutPreArb", 3.);
      fChiSqCutXYUVStAlgo1 = pset.get<double>("chiSqCutXYUVStAlgo1", 1000.);
      // 
      // Transfering this info to algorithms.. 
      //
      fUpStreamBeamTrRec.SetTokenJob(fTokenJob);
      if (!fDoIronBrick) {
        fDwnstrTrRec.SetTokenJob(fTokenJob);
	fDwnstrTrRec.SetChiSqCutPreArb(fDwnstrChiSqCutPreArb);
      } else { 
        fBrickTrRec.SetTokenJob(fTokenJob);
	fBrickTrRec.SetDistFromBeamCenterCut(fMaxDistFrom120Beam);
	fBrickTrRec.SetChiSqCut(fConfirmBrickChiSqCut);
      }
      // Upload the latest alignment 
      const std::string alignParamsStr = pset.get<std::string>("alignParamFileName");
      const char *pathHere = std::getenv("CETPKG_BUILD");
      const std::string ffName(pathHere + std::string("/") + alignParamsStr) ;
      std::cerr << " StudyAllTrial1Algo1::reconfigure, uploading alignment data from file " << ffName << std::endl;
      if (ffName.find("none") == std::string::npos) fEmVolAlP->SetGeomFromSSDAlign(ffName); 
           
      std::cerr << " .... O.K. done with reconfigure,  keep going ....  " << std::endl; 
    }
    void emph::StudyAllTrial1Algo1::beginRun(art::Run const &run)
    {
      std::cerr << " StudyAllTrial1Algo1::beginRun, run " << run.id() << std::endl;
      fRun = static_cast<int>(run.run());
      fRunHistory = new runhist::RunHistory(fRun);
      fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
      art::ServiceHandle<emph::MagneticFieldService> bField;
      emph::EMPHATICMagneticField *fMagField = bField->Field();
      fMagField->G4GeomAlignIt(fEmgeo);
      fUpStreamBeamTrRec.SetRun(fRun);
      //
      // My own convention..Ugly MC/real data difference.. 
      //
      if (fRun == 1293) fDwnstrTrRec.SetForMC(true);
      if (!fDoIronBrick) { 
        fDwnstrTrRec.SetRun(fRun);
        fDwnstrTrRec.SetPreliminaryMomentum(fPrelimMomentumSecondaries);
        for(size_t kSt=2; kSt != 6; kSt++)  {
          fDwnstrTrRec.SetChiSqCutRecStation(kSt, fChiSqCutXYUVStAlgo1); // should be done 
        }
      } else { 
        fBrickTrRec.SetRun(fRun);
        fBrickTrRec.SetAssumedMomentum(fPrelimMomentumSecondaries);
	fBrickTrRec.SetDoFirstAndLastStrips(fDoFirstAndLastStrips);
        for(size_t kSt=2; kSt != 6; kSt++)  {
          fBrickTrRec.SetChiSqCutRecStation(kSt, fChiSqCutXYUVStAlgo1); // should be done 
        }
       }
       
       fEmVolAlP->UpdateNominalFromStandardGeom(fEmgeo);
       	
       std::cerr  << std::endl << " ------------- End of StudyAllTrial1Algo1::beginRun ------------------" << std::endl << std::endl;
    }
    
    void emph::StudyAllTrial1Algo1::beginJob()
    {
    }
    void emph::StudyAllTrial1Algo1::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " StudyAllTrial1Algo1::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      std::ostringstream fNameMultStrStr; fNameMultStrStr << "./StudyAllTrial1Algo1_Mult_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameMultStr(fNameMultStrStr.str());
      fFOutMultSum.open(fNameMultStr.c_str());
      fFOutMultSum << " spill evt numClUp nClDwnstr nUpstrTr nBeamTr nStPt2 nStPt3 nStPt4 nStPt5 nTrsDwn nVert nVertDwn " << std::endl;
      //
      std::ostringstream fNameVertStrStr; fNameVertStrStr << "./StudyAllTrial1Algo1_Vertices_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameVertStr(fNameVertStrStr.str());
      fFOutVertices.open(fNameVertStr.c_str());
      fFOutVertices << " spill evt iV  itUp itDwn zx zxErr zy zyErr xVUp yVUp " << std::endl;  
      std::ostringstream fNameVertDStrStr; fNameVertDStrStr << "./StudyAllTrial1Algo1_VertDwn_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameVertDStr(fNameVertDStrStr.str());
      fFOutVertDwn.open(fNameVertDStr.c_str());
      fFOutVertDwn << " spill evt iV  itDwn1 itDwn2 zx zxErr zy zyErr xVDwn yVDw xVUp yVUp  " << std::endl;  
      fFilesAreOpen = true;
    }
    
    void emph::StudyAllTrial1Algo1::endJob() {
      std::cerr << " StudyAllTrial1Algo1::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      fFOutVertices.close(); fFOutMultSum.close(); fFOutVertDwn.close();
    }
    
    void emph::StudyAllTrial1Algo1::dumpSummaryMultiplicities() { 
    
//        fFOutMultSum << " subRun evt numClUp nClD nUpstrTr nStPt2  nStPt3 nStPt4 nStPt5 nStPt2 nTrsDwn " << std::endl;
        fFOutMultSum << " " << fSubRun << " " << fEvtNum << " " << fNumClUpstr << " " << fNumClDwnstr;
	if ((!fDoIronBrick) && (fUpStreamBeamTrRec.Size() == 0)) {
	  fFOutMultSum << " 0 0 0 0 0 0 0 0 0 " << std::endl;
	  return;
	} else {
	  fFOutMultSum << " " << fUpStreamBeamTrRec.Size() << " " << fNumBeamTracks;  
	}
	for (size_t k = 0; k != fStationsRecMult.size(); k++) fFOutMultSum << " " << fStationsRecMult[k]; 
	if (!fDoIronBrick) {
	   fFOutMultSum << " " << fDwnstrTrRec.Size() << " " << fNumVertices <<  " " << fNumVertDwn << std::endl;
	} else {
	   fFOutMultSum << " " << fBrickTrRec.Size() << " 0 0 " << std::endl;
	}   
    }
    
    void emph::StudyAllTrial1Algo1::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
//      const bool debugIsOn = ((fSubRun == 100) && (fEvtNum == 700)) ; // Real data, for Iron Brick run 
//      const bool debugIsOn = ((fEvtNum == 51) || (fEvtNum == 106) || (fEvtNum == 226) || (fEvtNum == 736)); // Multi track events, MC run 
//      const bool debugIsOn = ((fEvtNum == 106) || (fEvtNum == 226) || (fEvtNum == 736)); // Multi track events, MC run 
//     const bool debugIsOn = (fEvtNum > 81717); // root-Minuit crash at evt 81719 fixed by eliminating very back prelim chi-sq 
//     const bool debugIsOn = ((fSubRun == 100) && ((fEvtNum == 48 ) || (fEvtNum == 56 ) || (fEvtNum == 142 ) || (fEvtNum == 92 )));
       // For spill 100, good track from RScript, check loading 
       const bool debugIsOn = (fEvtNum == 9);
    //
    // Get the data. This is supposed the best way, but... 
      auto hdlCls = evt.getHandle<std::vector<rb::SSDCluster>>(fSSDClsLabel);
      art::fill_ptr_vector(fSSDclPtrs, hdlCls);
      
      if (debugIsOn) std::cerr << " Debugging.. StudyAllTrial1Algo1::analyze , event " << fEvtNum << "  on " 
                               << fSSDclPtrs.size() <<  " clusters " <<   std::endl;
			       
//      if (fEvtNum > 100) { std::cerr << " 100 evt is enough, quit here and now " << std::endl; exit(2); }		       
			        
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
      
      if (fSSDcls.size() == 0) { this->dumpSummaryMultiplicities();  return; } // nothing to do, no data. 
      fNumClUpstr = 0; fNumClDwnstr = 0; fNumVertices = 0; fNumVertDwn = 0;
      // 
      // Tally the number of clusters..
      //
      for (std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) { 
        if (itCl->Station() < 2) fNumClUpstr++;
	else fNumClDwnstr++; 
      }
      
      if (fNumClUpstr == 0) { this->dumpSummaryMultiplicities();  return; } // No upstream data. 
      //
      // Beam Tracks 
      //
      fNumBeamTracks = 0;
      if (!fDoIronBrick) { 
        fUpStreamBeamTrRec.SetSubRun(fSubRun); fUpStreamBeamTrRec.SetEvtNum(fEvtNum); 
        fUpStreamBeamTrRec.recoXY(fSSDClsPtr);
	if ((fRun == 1293) && (fEvtNum < 10000)) fUpStreamBeamTrRec.dumpXYInforR(INT_MAX);
        //
        // Cuts on beam track here. 
        //
        if (fUpStreamBeamTrRec.Size() == 0) { this->dumpSummaryMultiplicities();  return; }
        if (debugIsOn) std::cerr << " ... Back to Analyze, got " << fUpStreamBeamTrRec.Size() << " upstream track(s) " << std::endl;
        size_t kTr=0; 
       
        for(std::vector<rb::BeamTrackAlgo1>::const_iterator itUpTr = fUpStreamBeamTrRec.CBegin();  
	   itUpTr != fUpStreamBeamTrRec.CEnd(); itUpTr++, kTr++) {
          const double deltaX120 = itUpTr->XOffset() - fXPencil120; const double deltaY120 = itUpTr->YOffset() - fYPencil120;
	  const double dist120 = std::sqrt(deltaX120*deltaX120 + deltaY120*deltaY120);
	  if (debugIsOn) std::cerr << " .... Distance from 120 GeV Beam center " << dist120 << " Track slopes " << itUpTr->XSlope() 
	          << " , " << itUpTr->YSlope() << std::endl;
	  if (dist120 > fMaxDistFrom120Beam) { if (debugIsOn) std::cerr << " ... rejected, too far from beam center " << std::endl; continue; }
	  if ((std::abs(itUpTr->XSlope() - fMeanUpstreamXSlope) > fMaxUpstreamSlopeX) || 
	    (std::abs(itUpTr->YSlope() - fMeanUpstreamYSlope) > fMaxUpstreamSlopeY)) {
	     if (debugIsOn) std::cerr << " ... rejected, too large slope  " << std::endl;
	     continue;
	  }
	  itUpTr->SetUserFlag(1);
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
        fDwnstrTrRec.SetDebugOn(debugIsOn); 
        for(size_t kSt=2; kSt != 6; kSt++)  {
	  fStationsRecMult[kSt-2] = fDwnstrTrRec.RecStation(kSt, evt, fSSDClsPtr);
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
      //
      // Cut on track multiplities 
      // 
      if (debugIsOn) std::cerr << " .... After downstream reconstruction, number of Dwnstr tracks " << fDwnstrTrRec.Size() << std::endl;
      if(fDwnstrTrRec.Size() < static_cast<size_t>(fMinNumDwnstrTracks)) { this->dumpSummaryMultiplicities();  return; }
      if(fDwnstrTrRec.Size() > static_cast<size_t>(fMaxNumDwnstrTracks)) { this->dumpSummaryMultiplicities();  return; }
      if (debugIsOn) std::cerr << " .... O.K., Number of downstream tracks...  " << fDwnstrTrRec.Size() << std::endl;
      
      if (fUpStreamBeamTrRec.Size() > 0) fUpStreamBeamTrRec.dumpXYInforR(1);
      fDwnstrTrRec.dumpStInfoForR();
      fDwnstrTrRec.dumpInfoForR();
      if (debugIsOn) std::cerr << " Check evt number.. after dumping Dwn tracks   " << fEvtNum << std::endl;
      //
      // Temporary stop 
      //
      //if (debugIsOn) { std::cerr << " .... O.K., Back to Analyze of art module, enough is enough .. " << std::endl; exit(2); }
      // Vertex analysis donw here... This should be in a SSD vertex reconstruction package. 
      
      // Vertex Analysis, done better.. We keep the code below for compare..
      //
      if (fUpStreamBeamTrRec.Size() == 1) {
        fRecVert.SetDebugOn(debugIsOn);
	fRecVert.SetTokenJob(fTokenJob);
	std::vector<rb::BeamTrackAlgo1>::const_iterator itUp =  fUpStreamBeamTrRec.CBegin();
        fRecVert.RecAndFitIt(evt, itUp, fDwnstrTrRec);
	fRecVert.dumpInfoForR();
      }
      // Limit the multiplicities.. 
      /*
      ** Obsolete.. 
      size_t kTrUp = 0; 
      if (fUpStreamBeamTrRec.Size() > 2) { this->dumpSummaryMultiplicities();  return; } // cut on the very noisy event, upstream. 
      const double dz20 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0);
      if (debugIsOn) std::cerr << "---------------------------- " << std::endl 
         << " Starting Ustream/Downstream vertexin..on evt " << fEvtNum <<  std::endl; 
      double xVUpstr = DBL_MAX; double yVUpstr = DBL_MAX;
      for(std::vector<rb::BeamTrackAlgo1>::const_iterator itUp =  fUpStreamBeamTrRec.CBegin();  itUp != fUpStreamBeamTrRec.CEnd(); itUp++, kTrUp++) {
        const double x0u = itUp->XOffset(); const double y0u = itUp->YOffset();
	const double slx0u = itUp->XSlope(); const double sly0u = itUp->YSlope();
        const double x0uErr = itUp->XOffsetErr(); const double y0uErr = itUp->YOffsetErr();
	const double slx0uErr = itUp->XSlopeErr(); const double sly0uErr = itUp->YSlopeErr();
	size_t kTrDwn = 0;
	if (debugIsOn) std::cerr << "   At Usptream Track  xOdu " << x0u << " +- " << x0uErr << " y0u " << y0u << " +- " << y0uErr << std::endl;
	
        for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator itDwn = fDwnstrTrRec.CBegin();  itDwn != fDwnstrTrRec.CEnd(); itDwn++, kTrDwn++) {
	  if (itDwn->ChiSq() > fDwnstrVertChiSqCut) continue;
	  if (std::abs(itDwn->Momentum()) < 2.0) continue; 
	  const double slx0d = itDwn->XSlope(); const double sly0d = itDwn->YSlope();
	// Ignoring correlation between X and Y, via the U and W measurements.. 
	  const double slx0dErr = itDwn->XSlopeErr(); const double sly0dErr = itDwn->YSlopeErr();
          const double x0d = itDwn->XOffset() - dz20*slx0d; const double y0d = itDwn->YOffset() - dz20*sly0d;
	  const double x0dErr = std::sqrt(itDwn->XOffsetErr() * itDwn->XOffsetErr() + dz20*dz20*slx0dErr*slx0dErr);
	  const double y0dErr = std::sqrt(itDwn->YOffsetErr() * itDwn->YOffsetErr() + dz20*dz20*sly0dErr*sly0dErr);
	  if (debugIsOn) std::cerr << "   At Track " << itDwn->ID() << " x0d " << x0d << " +- " 
	                            << x0dErr << " y0d " << y0d << " +- " << y0dErr << std::endl;
	  //
	  const double zxNum = (x0u - x0d); const double zxDenom = (slx0d - slx0u); const double zx = zxNum/zxDenom;
	  if (std::isnan(zx)) continue;
	  const double zxNumErr = std::sqrt(x0uErr*x0uErr + x0dErr*x0dErr);
	  const double zxDenomErr = std::sqrt(slx0uErr*slx0uErr + slx0dErr*slx0dErr);
	  // Again, ingore correlation between offsets and slopes.. 
	  const double zxErr = std::abs(zx) * std::sqrt(zxNumErr*zxNumErr/(zxNum*zxNum) + zxDenomErr*zxDenomErr/(zxDenom*zxDenom)); 

	  const double zyNum = (y0u - y0d); const double zyDenom = (sly0d - sly0u); const double zy = zyNum/zyDenom;
	  if (std::isnan(zy)) continue;
	  const double zyNumErr = std::sqrt(y0uErr*y0uErr + y0dErr*y0dErr);
	  const double zyDenomErr = std::sqrt(sly0uErr*sly0uErr + sly0dErr*sly0dErr);
	  const double zyErr = std::abs(zy) * std::sqrt(zyNumErr*zyNumErr/(zyNum*zyNum) + zyDenomErr*zyDenomErr/(zyDenom*zyDenom)); 
	  if (debugIsOn) std::cerr << " Z vertex, X-Z plane " << zx << " +- " << zxErr << " Z vertex, Y-Z plane " << zy << " +- " << zyErr << std::endl;
	  const double zz = 0.5*(zx+zy); // arbitrary.. 
	  xVUpstr = x0u + zz * itUp->XSlope(); yVUpstr = y0u + zz * itUp->YSlope();
 	  if (std::abs(zz - 250.) < 5000.) fNumVertices++;
	  else continue;
//          fFOutVertices << " subRun evt iV itUp itDwn zx zxErr zyErr  zyErr " << std::endl;  
	   fFOutVertices << " " << fSubRun << " " << fEvtNum << " " << fNumVertices << " " << kTrUp << " " << kTrDwn 
	                 << " " << zx << " " << zxErr << " " << zy << " " << zyErr << " " << xVUpstr << " " <<  yVUpstr <<  std::endl; 
        } //  on dwonstream Tracks
      } // on Upstream tracks.. 
//      if (fNEvents > 200) {
//        std::cerr << " Analyze... Stop here.. check after 200 events " << std::endl; exit(2);
//      }
      
      if (debugIsOn) std::cerr << "---------------------------- " << std::endl 
         << " Starting Dwonstream/Downstream vertexin.. on evt " << fEvtNum <<  std::endl; 
	const double z2 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2);
      // Loop for the stiffest track, to estimate best transverse position. 
      int itIDBest = INT_MAX; double highestP = 0.;
      for(std::vector<rb::DwnstrTrackAlgo1>::const_iterator it1 =  fDwnstrTrRec.CBegin();  it1 != fDwnstrTrRec.CEnd(); it1++) {
	if (it1->ChiSq() > fDwnstrVertChiSqCut) continue;
	if (std::abs(it1->Momentum()) < 1.0) continue; 
        if (std::abs(it1->Momentum()) > highestP) { highestP = std::abs(it1->Momentum()); itIDBest = it1->ID(); }
      }
      size_t kTrDwn1 = 0;
      double xVDwnstr = DBL_MAX; double yVDwnstr = DBL_MAX;
      for(std::vector<rb::DwnstrTrackAlgo1>::const_iterator it1 =  fDwnstrTrRec.CBegin();  it1 != fDwnstrTrRec.CEnd(); it1++, kTrDwn1++) {
	if (it1->ChiSq() > fDwnstrVertChiSqCut) continue;
	if (std::abs(it1->Momentum()) < 1.0) continue; 
        const double xd1 = it1->XOffset(); const double yd1 = it1->YOffset();	
	const double slxd1 = it1->XSlope(); const double slyd1 = it1->YSlope();
	// Ignoring correlation between X and Y, via the U and W measurements.. 
	const double slxd1Err = it1->XSlopeErr(); const double slyd1Err = it1->YSlopeErr();
	const double xd1Err = it1->XOffsetErr(); const double yd1Err = it1->YOffsetErr();
	if (debugIsOn) std::cerr << "   At Track " << it1->ID() << " xd1 " << xd1 << " +- " 
	                            << xd1Err << " y0d " << yd1 << " +- " << yd1Err << std::endl;
	size_t kTrDwn2 = 0;
        for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator it2 = fDwnstrTrRec.CBegin();  it2 != fDwnstrTrRec.CEnd(); it2++, kTrDwn2++) { 
	  if (it2->ChiSq() > fDwnstrVertChiSqCut) continue;
	  if (std::abs(it2->Momentum()) < 1.0) continue; 
	  if (kTrDwn2 < kTrDwn1) continue;
	  if (it2 == it1) continue;
	  const double aMass = this->InvariantMass(it1, it2);
	  if (aMass < 0.0001) continue; // Too small invariant mass (neglecting rest mass of the pair) 
          const double xd2 = it2->XOffset(); const double yd2 = it2->YOffset();	
	  const double xd2Err = it2->XOffsetErr(); const double yd2Err = it2->YOffsetErr();
	  const double slxd2 = it2->XSlope(); const double slyd2 = it2->YSlope();
	// Ignoring correlation between X and Y, via the U and W measurements.. 
	  const double slxd2Err = it2->XSlopeErr(); const double slyd2Err = it2->YSlopeErr();
	  //
	  if (debugIsOn) std::cerr << "   At Track " << it2->ID() << " xd2 " << xd2 << " +- " 
	                            << xd2Err << " y0d " << yd2 << " +- " << yd2Err << " Invariant masss " <<  aMass << std::endl;
	  const double zxNum = (xd1 - xd2); const double zxDenom = (slxd1 - slxd2); 
	  const double zx = z2 - zxNum/zxDenom;  
	  if (std::isnan(zx)) continue;
	  const double zxNumErr = std::sqrt(xd1Err*xd1Err + xd2Err*xd2Err);
	  const double zxDenomErr = std::sqrt(slxd1Err*slxd1Err + slxd2Err*slxd2Err);
	  // Again, ingore correlation between offsets and slopes.. 
	  const double zxErr = std::abs(zx) * std::sqrt(zxNumErr*zxNumErr/(zxNum*zxNum) + zxDenomErr*zxDenomErr/(zxDenom*zxDenom)); 

	  const double zyNum = (yd1 - yd2); const double zyDenom = (slyd1 - slyd2); 
	  const double zy = z2 - zyNum/zyDenom; // Z are in the G4 world coordinates 
	  if (std::isnan(zy)) continue;
	  const double zyNumErr = std::sqrt(yd1Err*yd1Err + yd2Err*yd2Err);
	  const double zyDenomErr = std::sqrt(slyd1Err*slyd1Err + slyd2Err*slyd2Err);
	  // Again, ingore correlation between offsets and slopes.. 
	  const double zyErr = std::abs(zy) * std::sqrt(zyNumErr*zyNumErr/(zyNum*zyNum) + zyDenomErr*zyDenomErr/(zyDenom*zyDenom)); 
	  const double zz = 0.5*(zx+zy); // arbitrary.. 
	  if (itIDBest == it1->ID()) { 
	    xVDwnstr = xd1 + slxd1 * (zz - z2);
	    yVDwnstr = yd1 + slyd1 * (zz - z2);
	  } else if (itIDBest == it2->ID()) {
	    xVDwnstr = xd2 + slxd2 * (zz - z2);
	    yVDwnstr = yd2 + slyd2 * (zz - z2);
	  }
	  if (debugIsOn)  {
	     std::cerr << " Z vertex, X-Z plane " << zx << " +- " << zxErr << " Z vertex, Y-Z plane " << zy << " +- " << zyErr << std::endl;
	     std::cerr << " Check evt number.. Within vertex loop  " << fEvtNum << std::endl;
	  }
 	  if (std::abs(zz - 250.) < 5000.) fNumVertDwn++;
//          fFOutVertices << " subRun evt iV itUp itDwn zx zxErr zyErr  zyErr " << std::endl;  
	   fFOutVertDwn << " " << fSubRun << " " << fEvtNum << " " << fNumVertDwn << " " << it1->ID() << " " << it2->ID() 
	                 << " " << zx << " " << zxErr << " " << zy << " " << zyErr 
			 << " " << xVDwnstr << " " <<  yVDwnstr << " " << xVUpstr << " " << yVUpstr << std::endl; 
        } //  on dwonstream Tracks
      } // on Upstream tracks.. 
      */
      this->dumpSummaryMultiplicities();  
//      if (debugIsOn) { 
//          std::cerr << " .... O.K., End of Analyze of art module, enough is enough .. " << std::endl; 
//	  fFOutVertDwn.close(); fFOutVertices.close(); fFOutMultSum.close();
//          exit(2); 
//      }
      return;
	
    } // end of Analyze, event by events.  
    double emph::StudyAllTrial1Algo1::InvariantMass(std::vector<rb::DwnstrTrackAlgo1>::const_iterator it1, 
                                              std::vector<rb::DwnstrTrackAlgo1>::const_iterator it2) const {
					      
					      
       const double px1 = it1->XSlope()*std::abs(it1->Momentum()); 
       const double py1 = it1->YSlope()*std::abs(it1->Momentum()); 
       const double pz1 = std::sqrt(it1->Momentum()*it1->Momentum() - px1*px1 - py1*py1); 
       const double px2 = it2->XSlope()*std::abs(it2->Momentum()); 
       const double py2 = it2->YSlope()*std::abs(it2->Momentum()); 
       const double pz2 = std::sqrt(it2->Momentum()*it2->Momentum() - px2*px2 - py2*py2);
       const double deltaPx = px2-px1; const double deltaPy = py2-py1;   double deltaPz = pz2-pz1;
//       std::cerr << " StudyAllTrial1Algo1::InvariantMass,  px1 " << px1 << " px2 " << px2 
//                 << " py1 " << py1 <<  " py2 " << py2 <<  " pz1 " << pz1 << " pz2 " << pz2 << std::endl;
       return std::sqrt(deltaPx*deltaPx + deltaPy*deltaPy + deltaPz*deltaPz); 
       				      
    }				      
DEFINE_ART_MODULE(emph::StudyAllTrial1Algo1)
