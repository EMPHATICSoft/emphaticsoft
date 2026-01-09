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
      bool fDoPResol1Stu; 
      bool fDoMult2Stu; 
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
      std::vector<double> fZlocYPlanes;
      std::vector<double> fZlocYStations;
      std::vector<sim::SSDHit> fSSDVec;
      std::vector<sim::Track> fTrackVec;
//
// CSV tuple output..
// 
      std::ofstream fFOutA1, fFOutMult2;
      void openOutputCsvFiles();
      
      void StudyPResol1 (const std::vector<sim::SSDHit> &theSSDHits, const std::vector<sim::Track> &theTracks) ; 
      void StudyMult2 (const std::vector<sim::SSDHit> &theSSDHits, const std::vector<sim::Track> &theTracks) ; 
      
    }; 
    
// .....................................................................................
    AnalyzeSSDHitsWithTracks::AnalyzeSSDHitsWithTracks(fhicl::ParameterSet const& pset) : 
      EDAnalyzer(pset), 
      fFilesAreOpen(false),
      fDoPResol1Stu (pset.get<bool>("doPresol1Stu", false)),
      fDoMult2Stu   (pset.get<bool>("doMult2Stu", false)),
      fTokenJob     (pset.get<std::string>("tokenJob", "UnDef")),
      fSSDHitLabel  (pset.get<std::string>("SSDHitLabel")),
      fTrackLabel   (pset.get<std::string>("TrackLabel")),
      fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0) , fPitch(0.06),
      fRunHistory(nullptr), fEmgeo(nullptr), fEmSSDHits(nullptr), fEmTracks(nullptr), 
      fZlocXPlanes(0), fZlocXStations(0), fZlocYPlanes(0), fZlocYStations(0)
    {
       std::cerr << " Constructing AnalyzeSSDHitsWithTracks " << std::endl;
       fFilesAreOpen = false;
    }
    
    void AnalyzeSSDHitsWithTracks::beginRun(art::Run const &run)
    {
     std::cerr << " AnalyzeSSDHitsWithTracks::beginRun, run " << run.id() << std::endl;
     fRunHistory = new runhist::RunHistory(run.run());
     fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());     
//
     for (int k=0; k != fEmgeo->NSSDStations(); k++) {
       const emph::geo::SSDStation* aStation = fEmgeo->GetSSDStation(k);
       TVector3 posSt = aStation->Pos();
       fZlocXStations.push_back(posSt[2]);
       fZlocYStations.push_back(posSt[2]);
       for (int jj=0; jj < aStation->NPlanes(); ++jj) {
	 const emph::geo::Plane* sPlane = aStation->GetPlane(jj);
	 for (int kk=0; kk != sPlane->NSSDs(); kk++) {
	   const emph::geo::Detector* aPlane = sPlane->SSD(kk);
	   if ((std::abs(aPlane->Rot() - 270.*M_PI/180.) < 0.1) || (std::abs(aPlane->Rot() - 90.*M_PI/180.) < 0.1) || 
	       (std::abs(aPlane->Rot() + 270.*M_PI/180.) < 0.1) || (std::abs(aPlane->Rot() + 90.*M_PI/180.) < 0.1) ) {
	     TVector3 pos = aPlane->Pos();
	     fZlocXPlanes.push_back(pos[2] + posSt[2]);
	   }
	   if ((std::abs(aPlane->Rot())  < 0.1) || (std::abs(aPlane->Rot() - M_PI/180.) < 0.1) || 
	       (std::abs(aPlane->Rot() + M_PI/180.) < 0.1) ) {
	     TVector3 pos = aPlane->Pos();
	     fZlocYPlanes.push_back(pos[2] + posSt[2]);
	   }
	 }
       } // on SS Detector planes of segment of a plane.   
     } // on index k, SSD Stations.
     if (fZlocXPlanes.size() < 5)  { std::cerr << " Not enough X planes to do this study, quit here and now " << std::endl; exit(2); }
     std::cerr << " Number of SSD X planes " << fZlocXPlanes.size() << std::endl;
     std::cerr << " Z locations ";
     for (size_t i=0; i != fZlocXPlanes.size(); i++) std::cerr << " " << fZlocXPlanes[i] << ",";
     std::cerr << " Number of SSD Y planes " << fZlocYPlanes.size() << std::endl;
     std::cerr << " Z locations ";
     for (size_t i=0; i != fZlocYPlanes.size(); i++) std::cerr << " " << fZlocYPlanes[i] << ",";
     
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
      if (fDoPResol1Stu) { 
        std::ostringstream fNamePResol1StrStr; fNamePResol1StrStr << "./PResol1Tuple_V1_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNamePResol1Str(fNamePResol1StrStr.str());
        fFOutA1.open(fNamePResol1Str.c_str());
        fFOutA1 << " subRun evt nTr nTr1G tr x y z pNorm pTrans slx01 slx23 slx45 slx01D slx23D slx45D pMeas pMeasD ";
        fFOutA1 << " " << std::endl;
      }
      if (fDoMult2Stu) {
        std::ostringstream fNameMult2StrStr; fNameMult2StrStr << "./MultStuTuple_V1_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameMult2Str(fNameMult2StrStr.str());
        fFOutMult2.open(fNameMult2Str.c_str());
        fFOutMult2 << " subRun evt nTr nTrHM kTr y0 y1 y2 y3 y4 y5 "; // simply.. 
        fFOutMult2 << " " << std::endl;
      }
      fFilesAreOpen = true;
    }
    void AnalyzeSSDHitsWithTracks::endJob() {
      
      std::cerr << " AnalyzeSSDHitsWithTracks::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      
      if (fDoPResol1Stu) fFOutA1.close(); 
      if (fDoMult2Stu) fFOutMult2.close();
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

    //
    // Get the data. 
      art::Handle<std::vector<sim::SSDHit> > theSSDHits;
      evt.getByLabel (fSSDHitLabel, theSSDHits);
      std::vector<sim::SSDHit> mySSDHits(*theSSDHits); // a deep copy that should not be here.. Convenience for multiple analysis.
//
      art::Handle<std::vector<sim::Track> > theTracks;
      evt.getByLabel(fTrackLabel, theTracks );
      std::vector<sim::Track> myTracks(*theTracks); // a deep copy that should not be here.. Convenience for multiple analysis.
//      std::cerr << " Number of tracks : " << myTracks.size() << std::endl;
      
     if (fDoPResol1Stu) this->StudyPResol1(mySSDHits, myTracks);
     if (fDoMult2Stu) this->StudyMult2(mySSDHits, myTracks);
     
    } // end of Analyze 
    //
    // simple Xslope measurement 
    //
    void AnalyzeSSDHitsWithTracks::StudyPResol1 (const std::vector<sim::SSDHit> &theSSDHits, 
                                                 const std::vector<sim::Track> &theTracks) {
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
	  if (aHit.TrackID() != iTrack->GetTrackID()) continue;  // first big cheat..
	  for (size_t kPl = 0; kPl != xi.size(); kPl++) {
	    if ( std::abs(fZlocXStations[kPl] - aHit.Z()) < 2.0 )  {
	       xi[kPl] = aHit.X();
	       double xx = aHit.X() + arbitraryChannelOffset;
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
   //
   // Multiple scattering study, for assign proper uncertainties for alignment.. 
   // 
    void AnalyzeSSDHitsWithTracks::StudyMult2 (const std::vector<sim::SSDHit> &theSSDHits, const std::vector<sim::Track> &theTracks) {
      std::ostringstream prologStrStr;
      prologStrStr << " " << fSubRun << " " << fEvtNum << " " << theTracks.size();
      int nTr1G = 0;
      const std::string prologStr(prologStrStr.str());
      std::vector<double> yPosHits(6, DBL_MAX);
      for (std::vector<sim::Track>::const_iterator iTrack = theTracks.cbegin(); iTrack != theTracks.cend(); iTrack++ ) {
	const double pMom = std::sqrt(iTrack->GetPx()*iTrack->GetPx() +  iTrack->GetPy()*iTrack->GetPy() + iTrack->GetPz()*iTrack->GetPz());
        if (pMom > 100.) nTr1G++;
      }
      if (nTr1G != 1) return; // not interested in hadronic interactions. 
      for (std::vector<sim::Track>::const_iterator iTrack = theTracks.cbegin(); iTrack != theTracks.cend(); iTrack++ ) {
	const double pMom = std::sqrt(iTrack->GetPx()*iTrack->GetPx() +  iTrack->GetPy()*iTrack->GetPy() + iTrack->GetPz()*iTrack->GetPz());
        if (pMom < 100.) continue; // skip the low energy tracks.. We have them tally above. 
	for (size_t kH = 0; kH != theSSDHits.size(); kH++) {
	  sim::SSDHit aHit = theSSDHits[kH];
	  if (aHit.TrackID() != iTrack->GetTrackID()) continue;  // first big cheat..
	  for (size_t kPl = 0; kPl != yPosHits.size(); kPl++) {
	    if ( std::abs(fZlocYPlanes[kPl] - aHit.Z()) < 1.0 )  {
	       yPosHits[kPl] =  aHit.Y();
	     } // got a match in Z  
	  } 
	}// Over the hits..
	fFOutMult2 << prologStr << " " << nTr1G;
	fFOutMult2 << " " << iTrack->GetTrackID();
	for (size_t k=0; k != yPosHits.size(); k++) fFOutMult2  << " " << yPosHits[k];
	fFOutMult2 << std::endl;
      } // onTracks.
   } 
} // name space emph       
DEFINE_ART_MODULE(emph::AnalyzeSSDHitsWithTracks)
