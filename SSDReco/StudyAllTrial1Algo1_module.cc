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
//
//   Downstream of the target. 
//
     double fDwnstrChiSqCut; 
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
//
// Internal stuff 
//
     const double fXPencil120, fYPencil120; // The coordinate, in our Volatile Coord. system of the centroid of the 120 GeV, 
//
// Variable I think I know.. Valid only for the MinValues_Simplex_OK_Try3D_R1274_1h1_1.txt alignment set. 
// 
    double fMeanUpstreamXSlope, fMeanUpstreamYSlope;
     // where the SSD alignment could be considered valid. 
     int fNumClUpstr, fNumClDwnstr, fNumBeamTracks, fNumVertices;
     std::vector<int> fStationsRecMult;
//
// CSV tuple output..
// 
      std::ofstream fFOutMultSum, fFOutVertices;
      
//
// access to input data..   
//
      std::vector<art::Ptr<rb::SSDCluster> > fSSDclPtrs; // This is what I got from art, see analyze method. 
      std::vector<rb::SSDCluster> fSSDcls; // we will do a deep copy, as my first attempt at using the above vector failed.. lower case c
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr; // This works, but use the deprecated art interface.. Upper case C
      
      void openOutputCsvFiles();
      void dumpSummaryMultiplicities(); 
            
    }; 
    
// .....................................................................................
    emph::StudyAllTrial1Algo1::StudyAllTrial1Algo1(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fSSDClsLabel("?"), fSSDAlignmentResult("?"),
    fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0), 
    fSetMCRMomentum(30.), fPrelimMomentumSecondaries(5.0), fChiSqCutXYUVStAlgo1(20.), 
    fMaxUpstreamSlopeX(0.005), fMaxUpstreamSlopeY(0.005), fMaxDistFrom120Beam(100.),
    fMaxNumBeamTracks(10), fMinNumDwnstrTracks(1), fMaxNumDwnstrTracks(20), fDoAntiSt2(false), fDwnstrChiSqCut(10.),   
    fRunHistory(nullptr), fEmgeo(nullptr), 
    fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fUpStreamBeamTrRec(), 
    fXPencil120(-3.8), fYPencil120(4.5), // Obtained in the analysis of 120 GeV runs 1043 and 1055.  Phase1b
    fMeanUpstreamXSlope(0.0004851102), fMeanUpstreamYSlope(0.002182192), // Only for run 1066, 1067... (31 GeV). 
    fNumClUpstr(0), fNumClDwnstr(0), fNumBeamTracks(0), fNumVertices(0), fStationsRecMult(4, 0)
    {
       std::cerr << " Constructing StudyAllTrial1Algo1 " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;       
       //
       // import the alignment parameter, from the SSDAlign project. 
       //
       if (fSSDAlignmentResult != std::string("?")) fEmVolAlP->SetGeomFromSSDAlign(fSSDAlignmentResult); 
    }
    
    void emph::StudyAllTrial1Algo1::reconfigure(const fhicl::ParameterSet& pset)
    {
      std::cerr << " emph::StudyAllTrial1Algo1::reconfigure ... " <<std::endl;
      fSSDClsLabel = pset.get<std::string>("SSDClsLabel");   
      fSSDAlignmentResult = pset.get<std::string>("SSDAlignmentResult", "?");   
      std::cerr << "  ... fSSDClsLabel " << fSSDClsLabel << std::endl;   
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fMaxUpstreamSlopeX = pset.get<double>("maxUpstreamSlopeX", 0.005);
      fMaxUpstreamSlopeY = pset.get<double>("maxUpstreamSlopeY", 0.005);
      fMaxDistFrom120Beam = pset.get<double>("maxDistFrom120Beam", 100.);
      fMaxNumBeamTracks = pset.get<int>("maxNumBeamTracks", 20);
      fMinNumDwnstrTracks = pset.get<int>("minNumDwnstrTracks", 1);
      fMaxNumDwnstrTracks = pset.get<int>("maxNumDwnstrTracks", 20);
      fDoAntiSt2 = pset.get<bool>("doAntiSt2", false);
      fPrelimMomentumSecondaries = pset.get<double>("prelimMomentum", 5.);
      fDwnstrChiSqCut = pset.get<double>("dwnstrChiSqCut", 10.);
      fChiSqCutXYUVStAlgo1 = pset.get<double>("chiSqCutXYUVStAlgo1", 1000.);
      // 
      // Transfering this info to algorithms.. 
      //
      fUpStreamBeamTrRec.SetTokenJob(fTokenJob);
      fDwnstrTrRec.SetTokenJob(fTokenJob);
      // Upload the latest alignment 
      const std::string alignParamsStr = pset.get<std::string>("alignParamFileName");
      const char *pathHere = std::getenv("CETPKG_BUILD");
      const std::string ffName(pathHere + std::string("/") + alignParamsStr) ;
      std::cerr << " StudyAllTrial1Algo1::reconfigure, uploading alignment data from file " << ffName << std::endl;
      fEmVolAlP->SetGeomFromSSDAlign(ffName);      
      std::cerr << " .... O.K. done with reconfugure,  keep going ....  " << std::endl; 
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
      fDwnstrTrRec.SetRun(fRun);
      fDwnstrTrRec.SetPreliminaryMomentum(fPrelimMomentumSecondaries);
      for(size_t kSt=2; kSt != 6; kSt++)  {
        fDwnstrTrRec.SetChiSqCutRecStation(kSt, fChiSqCutXYUVStAlgo1); // should be done 
      }	
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
      fFOutMultSum << " spill evt numClUp nClDwnstr nUpstrTr nBeamTr nStPt2 nStPt3 nStPt4 nStPt5 nTrsDwn nVert " << std::endl;
      //
      std::ostringstream fNameVertStrStr; fNameVertStrStr << "./StudyAllTrial1Algo1_Vertices_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string fNameVertStr(fNameVertStrStr.str());
      fFOutVertices.open(fNameVertStr.c_str());
      fFOutVertices << " spill evt iV  itUp itDwn zx zxErr zy zyErr " << std::endl;  
      fFilesAreOpen = true;
    }
    
    void emph::StudyAllTrial1Algo1::endJob() {
      std::cerr << " StudyAllTrial1Algo1::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      fFOutVertices.close(); fFOutMultSum.close();
    }
    
    void emph::StudyAllTrial1Algo1::dumpSummaryMultiplicities() { 
    
//        fFOutMultSum << " subRun evt numClUp nClD nUpstrTr nStPt2  nStPt3 nStPt4 nStPt5 nStPt2 nTrsDwn " << std::endl;
        fFOutMultSum << " " << fSubRun << " " << fEvtNum << " " << fNumClUpstr << " " << fNumClDwnstr;
	if (fUpStreamBeamTrRec.Size() == 0) {
	  fFOutMultSum << " 0 0 0 0 0 0 0 0 " << std::endl;
	  return;
	} else {
	  fFOutMultSum << " " << fUpStreamBeamTrRec.Size() << " " << fNumBeamTracks;  
	}
	for (size_t k = 0; k != fStationsRecMult.size(); k++) fFOutMultSum << " " << fStationsRecMult[k]; 
	fFOutMultSum << " " << fDwnstrTrRec.Size() << " " << fNumVertices << std::endl;
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
      const bool debugIsOn = ((fSubRun == 10) && (fEvtNum == 30)) ; 
      
      
    //
    // Get the data. This is supposed the best way, but... 
      auto hdlCls = evt.getHandle<std::vector<rb::SSDCluster>>(fSSDClsLabel);
      art::fill_ptr_vector(fSSDclPtrs, hdlCls);
      
      if (debugIsOn) std::cerr << " StudyAllTrial1Algo1::analyze , event " << fEvtNum << "  on " 
                               << fSSDclPtrs.size() <<  " clusters " <<   std::endl;
			       
//      if (fEvtNum > 100) { std::cerr << " 100 evt is enough, quit here and now " << std::endl; exit(2); }		       
			        
      fUpStreamBeamTrRec.Reset();
      
      //
      // This code fragment is simply for debugging.. And is confusing.. 
//      if (fSSDclPtrs.size() != 1) { 
//        std::cerr << " Number of pointers to a given SSDCluster, " << fSSDclPtrs.size() << std::endl;
//      }

//      std::vector<art::Ptr<rb::SSDCluster> >::const_iterator aPtrClIt = fSSDclPtrs.cbegin();
//      art::Ptr<rb::SSDCluster> aPtrCl = *aPtrClIt;
//      std::cerr << " Station for the first cluster  " << aPtrCl->Station() << std::endl;
      // 
      // These, above, will fail, as the sStion number is clearly bogus... for many clusters.  The fill_ptr_vector is not applicable here!  
      // old, deprecated interface, with a deep copy.. But, it works ..  
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
      fNumClUpstr = 0; fNumClDwnstr = 0; fNumVertices = 0;
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
      fUpStreamBeamTrRec.SetSubRun(fSubRun); fUpStreamBeamTrRec.SetEvtNum(fEvtNum); 
      fUpStreamBeamTrRec.recoXY(fSSDClsPtr);
      //
      // Cuts on beam track here. 
      //
      if (fUpStreamBeamTrRec.Size() == 0) { this->dumpSummaryMultiplicities();  return; }
      if (debugIsOn) std::cerr << " ... Back to Analyze, got " << fUpStreamBeamTrRec.Size() << " upstream track(s) " << std::endl;
      fNumBeamTracks = 0; size_t kTr=0;  
      for(std::vector<rb::BeamTrackAlgo1>::const_iterator itUpTr = fUpStreamBeamTrRec.CBegin();  itUpTr != fUpStreamBeamTrRec.CEnd(); itUpTr++, kTr++) {
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
      //
      // Now reconstructed the Downstream (of the target) Space points. 
      //
      fDwnstrTrRec.SetDebugOn(debugIsOn); 
      for(size_t kSt=2; kSt != 6; kSt++)  {
	fStationsRecMult[kSt-2] = fDwnstrTrRec.RecStation(kSt, evt, fSSDClsPtr);
      }
      if ((fDoAntiSt2) && (fStationsRecMult[0] > 0))  {
	  this->dumpSummaryMultiplicities();  
	  return; 
      }
//      if (fDoAntiSt2) std::cerr << " No XY hits in Station2, and aintiSt2 is on for evt " << fEvtNum << std::endl; 
      //
      // Now downstream tracks.. 
      //
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
      //
      // Temporary stop 
      //
//      if (debugIsOn) { std::cerr << " .... O.K., Back to Analyze of art module, enough is enough .. " << std::endl; exit(2); }
      // Vertex analysis donw here... This should be in a SSD vertex reconstruction package. 
      // Limit the multiplicities.. 
      size_t kTrUp = 0; 
      if (fUpStreamBeamTrRec.Size() > 2) { this->dumpSummaryMultiplicities();  return; } // cut on the very noisy event, upstream. 
      if (fDwnstrTrRec.Size() > 15) { this->dumpSummaryMultiplicities();  return; } 
      const double dz20 = fEmVolAlP->ZPos(emph::geo::X_VIEW, 2) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0);
      for(std::vector<rb::BeamTrackAlgo1>::const_iterator itUp =  fUpStreamBeamTrRec.CBegin();  itUp != fUpStreamBeamTrRec.CEnd(); itUp++, kTrUp++) {
        const double x0u = itUp->XOffset(); const double y0u = itUp->YOffset();
	const double slx0u = itUp->XSlope(); const double sly0u = itUp->YSlope();
        const double x0uErr = itUp->XOffsetErr(); const double y0uErr = itUp->YOffsetErr();
	const double slx0uErr = itUp->XSlopeErr(); const double sly0uErr = itUp->YSlopeErr();
	size_t kTrDwn = 0;
        for (std::vector<rb::DwnstrTrackAlgo1>::const_iterator itDwn = fDwnstrTrRec.CBegin();  itDwn != fDwnstrTrRec.CEnd(); itDwn++, kTrDwn++) { 
	  const double slx0d = itDwn->XSlope(); const double sly0d = itDwn->YSlope();
	// Ignoring correlation between X and Y, via the U and W measurements.. 
	  const double slx0dErr = itDwn->XSlopeErr(); const double sly0dErr = itDwn->YSlopeErr();
          const double x0d = itDwn->XOffset() - dz20*slx0d; const double y0d = itUp->YOffset() - dz20*sly0d;
	  const double x0dErr = std::sqrt(itDwn->XOffsetErr() * itDwn->XOffsetErr() + dz20*dz20*slx0dErr*slx0dErr);
	  const double y0dErr = std::sqrt(itDwn->YOffsetErr() * itDwn->YOffsetErr() + dz20*dz20*sly0dErr*sly0dErr);
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
	  
	  const double zz = 0.5*(zx+zy); // arbitrary.. 
 	  if (std::abs(zz - 250.) < 5000.) fNumVertices++;
	  else continue;
//          fFOutVertices << " subRun evt iV itUp itDwn zx zxErr zyErr  zyErr " << std::endl;  
	   fFOutVertices << " " << fSubRun << " " << fEvtNum << " " << fNumVertices << " " << kTrUp << " " << kTrDwn 
	                 << " " << zx << " " << zxErr << " " << zy << " " << zyErr << std::endl; 
        } //  on dwonstream Tracks
      } // on Upstream tracks.. 
//      if (fNEvents > 200) {
//        std::cerr << " Analyze... Stop here.. check after 200 events " << std::endl; exit(2);
//      }
      this->dumpSummaryMultiplicities();  return;
	
    } // end of Analyze, event by events.  
   
DEFINE_ART_MODULE(emph::StudyAllTrial1Algo1)
