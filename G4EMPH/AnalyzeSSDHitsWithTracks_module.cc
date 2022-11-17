////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to check the track Id assignment to simulated SSDHit
////            Also compute a simple estimate of the momentum resolution based on perfect SSD detector and reconstruction software. 
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
// C++ includes
#include <cstddef>
#include <memory>
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
#include "Simulation/SSDHit.h"
#include "Simulation/Track.h"
//
using namespace emph;

namespace emph {
  //
    
    class AnalyzeSSDHitsWithTracks : public art::EDAnalyzer {
    public:
      explicit AnalyzeSSDHitsWithTracks(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void analyze(const art::Event& evt);

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
      std::string fSSDHitLabel, fTrackLabel;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      double fPitch;
//
// access to data..   
//
      runhist::RunHistory *fRunHistory;
      emph::geo::Geometry *fEmgeo;
      sim::SSDHit *fEmSSDHits;
      sim::Track *fEmTracks;
      std::vector<double> fZlocXPlanes;
      std::vector<double> fZlocXStations;
      std::vector<sim::SSDHit> fSSDVec;
      std::vector<sim::Track> fTrackVec;
//
// CSV tuple output..
// 
      std::ofstream fFOutA1;
      void openOutputCsvFiles();
      
      void StudyPResol1 (const std::vector<sim::SSDHit> &theSSDHits, const std::vector<sim::Track> &theTracks) ; 
      
    }; 
    
// .....................................................................................
    AnalyzeSSDHitsWithTracks::AnalyzeSSDHitsWithTracks(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fSSDHitLabel("?"), fTrackLabel("?"),
     fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0) , fPitch(0.06),
     fRunHistory(nullptr), fEmgeo(nullptr), fEmSSDHits(nullptr), fEmTracks(nullptr), 
     fZlocXPlanes(0), fZlocXStations(0)
    {
       std::cerr << " Constructing AnalyzeSSDHitsWithTracks " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;
    }
    
    void AnalyzeSSDHitsWithTracks::reconfigure(const fhicl::ParameterSet& pset)
    {
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fSSDHitLabel = pset.get<std::string>("SSDHitLabel");
      fTrackLabel = pset.get<std::string>("TrackLabel");
    }
    void AnalyzeSSDHitsWithTracks::beginRun(art::Run const &run)
    {
     std::cerr << " AnalyzeSSDHitsWithTracks::beginRun, run " << run.id() << std::endl;
     fRunHistory = new runhist::RunHistory(run.run());
     fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());     
//
     for (int k=0; k != fEmgeo->NSSDStations(); k++) {
       emph::geo::SSDStation aStation = fEmgeo->GetSSDStation(k);
       TVector3 posSt = aStation.Pos();
       fZlocXStations.push_back(posSt[2]);
       for (int kk=0; kk != aStation.NSSDs(); kk++) {
         emph::geo::Detector aPlane = aStation.GetSSD(kk);
	 if ((std::abs(aPlane.Rot() - 270.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() - 90.*M_PI/180.) < 0.1) || 
	     (std::abs(aPlane.Rot() + 270.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() + 90.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos();
	    fZlocXPlanes.push_back(pos[2] + posSt[2]);
	 }
       } // on SS Detector planes of segment of a plane.   
     } // on index k, SSD Stations.
     if (fZlocXPlanes.size() < 5)  { std::cerr << " Not enough X planes to do this study, quit here and now " << std::endl; exit(2); }
     std::cerr << " Number of SSD X planes " << fZlocXPlanes.size() << std::endl;
     std::cerr << " Z locations ";
     for (size_t i=0; i != fZlocXPlanes.size(); i++) std::cerr << " " << fZlocXPlanes[i] << ",";
     std::cerr << " Number of SSDStations  " << fZlocXStations.size() << std::endl;
     std::cerr << " Z locations ";
     for (size_t i=0; i != fZlocXStations.size(); i++) std::cerr << " " << fZlocXStations[i] << ",";
     std::cerr << std::endl << std::endl;
    }
    void AnalyzeSSDHitsWithTracks::beginJob()
    {
    }
    void AnalyzeSSDHitsWithTracks::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " AnalyzeSSDHitsWithTracks::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      std::ostringstream fNamePResol1StrStr; fNamePResol1StrStr << "./PResol1Tuple_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNamePResol1Str(fNamePResol1StrStr.str());
      fFOutA1.open(fNamePResol1Str.c_str());
      fFOutA1 << " subRun evt nTr nTr1G tr x y z pNorm pTrans slx01 slx23 slx45 slx01D slx23D slx45D pMeas pMeasD ";
      fFOutA1 << " " << std::endl;
      
      fFilesAreOpen = true;
    }
    void AnalyzeSSDHitsWithTracks::endJob() {
      
      std::cerr << " AnalyzeSSDHitsWithTracks::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      
      fFOutA1.close(); 
    }
    void AnalyzeSSDHitsWithTracks::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      
//      std::cerr << " AnalyzeSSDHitsWithTracks::analyze , event " << fEvtNum << " and do not much  " <<   std::endl; 
      
//      auto tokenForTrack = evt.getProductTokens<std::vector<sim::Track>(); 
      
    //
    // Get the data. 
      art::Handle<std::vector<sim::SSDHit> > theSSDHits;
      evt.getByLabel (fSSDHitLabel, theSSDHits);
      std::vector<sim::SSDHit> mySSDHits(*theSSDHits); // a deep copy that should not be here.. Conveninece for mulyiple analyssis.
//
      art::Handle<std::vector<sim::Track> > theTracks;
      evt.getByLabel(fTrackLabel, theTracks );
      std::vector<sim::Track> myTracks(*theTracks); // a deep copy that should not be here.. Conveninece for mulyiple analyssis.
//      std::cerr << " Number of tracks : " << myTracks.size() << std::endl;
      
     this->StudyPResol1(mySSDHits, myTracks);
    } // end of Analyze 
    //
    // simple Xslope measurement 
    //
    void AnalyzeSSDHitsWithTracks::StudyPResol1 (const std::vector<sim::SSDHit> &theSSDHits, const std::vector<sim::Track> &theTracks) {
     const double arbitraryChannelOffset = 50000.;
      const int discretShift = static_cast<int>(arbitraryChannelOffset/fPitch);
      // we could put systematic biases here.. 
      std::ostringstream prologStrStr;
      prologStrStr << " " << fSubRun << " " << fEvtNum << " " << theTracks.size();
      int nTr1G = 0;
      const std::string prologStr(prologStrStr.str());
      double slx01=DBL_MAX; double slx23=DBL_MAX; double slx45=DBL_MAX; 
      double slx01D=DBL_MAX; double slx23D=DBL_MAX; double slx45D=DBL_MAX;
      for (std::vector<sim::Track>::const_iterator iTrack = theTracks.cbegin(); iTrack != theTracks.cend(); iTrack++ ) {
	const double pMom = std::sqrt(iTrack->GetPx()*iTrack->GetPx() +  iTrack->GetPy()*iTrack->GetPy() + iTrack->GetPz()*iTrack->GetPz());
        if (pMom > 100.) nTr1G++;
      }
      for (std::vector<sim::Track>::const_iterator iTrack = theTracks.cbegin(); iTrack != theTracks.cend(); iTrack++ ) {
	const double pMom = std::sqrt(iTrack->GetPx()*iTrack->GetPx() +  iTrack->GetPy()*iTrack->GetPy() + iTrack->GetPz()*iTrack->GetPz());
	const double pTrans = std::sqrt(iTrack->GetPx()*iTrack->GetPx() +  iTrack->GetPy()*iTrack->GetPy());
        if (pMom < 100.) continue; // skip the low energy tracks.. We have them tally above. 
	std::vector<double> xi = {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX };
	std::vector<double> xiD(xi);
	for (size_t kH = 0; kH != theSSDHits.size(); kH++) {
	  sim::SSDHit aHit = theSSDHits[kH];
	  if (aHit.GetTrackID() != iTrack->GetTrackID()) continue;  // first big cheat..
	  for (size_t kPl = 0; kPl != xi.size(); kPl++) {
	    if ( std::abs(fZlocXStations[kPl] - aHit.GetZ()) < 2.0 )  {
	       xi[kPl] = aHit.GetX();
	       double xx = aHit.GetX() + arbitraryChannelOffset;
	       int xxI = static_cast<int>(xx/fPitch) - discretShift;// Assume 60 microns pitch
	       xiD[kPl] = fPitch * xxI;
	     } // got a mtach in Z  
	  } 
	}// Over the hits..
	if ((xi[0] != DBL_MAX) && (xi[1] != DBL_MAX)) {
	  slx01 = (xi[1] - xi[0])/(fZlocXStations[1] - fZlocXStations[0]);
	  slx01D = (xiD[1] - xiD[0])/(fZlocXStations[1] - fZlocXStations[0]);
	} 
	if ((xi[2] != DBL_MAX) && (xi[3] != DBL_MAX)) {
	  slx23 = (xi[3] - xi[2])/(fZlocXStations[3] - fZlocXStations[2]);
	  slx23D = (xiD[3] - xiD[2])/(fZlocXStations[3] - fZlocXStations[2]);
	} 
	if ((xi[4] != DBL_MAX) && (xi[5] != DBL_MAX)) {
	  slx45 = (xi[5] - xi[4])/(fZlocXStations[5] - fZlocXStations[4]);
	  slx45D = (xiD[5] - xiD[4])/(fZlocXStations[5] - fZlocXStations[4]);
	}
	fFOutA1 << prologStr << " " << nTr1G;
	fFOutA1 << " " << iTrack->GetTrackID() << " " << iTrack->GetX() << " " << iTrack->GetY() << " " << iTrack->GetZ();
	fFOutA1 << " " << pMom << " " << pTrans << " " << slx01 << " " << slx23 << " " << slx45;
        const double pMeas = ((slx23 != DBL_MAX) && (slx45 != DBL_MAX)) ? 1.0/(slx45 - slx23) : DBL_MAX;
        const double pMeasD = ((slx23D != DBL_MAX) && (slx45D != DBL_MAX)) ? 1.0/(slx45D - slx23D) : DBL_MAX;
	fFOutA1 << " " << slx01D << " "<< slx23D << " "<< slx45D << " "<< pMeas << " " << pMeasD << std::endl; 
      } // onTracks.
   } 
} // name space emph       
DEFINE_ART_MODULE(emph::AnalyzeSSDHitsWithTracks)
