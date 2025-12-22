////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to reconstruct candidate beam tracks. 
/// \author  $Author: lebrun $
/// 
/// Based on Autre, 3DXY. See SSDAlign3DUVAutre.   First, the we do a linear fit in the X-Z and YZ plane, 
// then confirm with U view in Station 2, 3, 4, 5 (last two station suspiciousl, SSD pitch unknown.. )
/// 
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
#include "art/Framework/Core/EDProducer.h"
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
#include "RecoBase/BeamTrackAutr.h"
#include "SSDReco/experimental/SSDAlign3DUVAutre.h"
#include "SSDReco/SSDHotChannelList.h" 
//

namespace emph {
  class RecoBeamTracksAutre;
}
  //
    
    class emph::RecoBeamTracksAutre : public art::EDProducer {
    
    public:
      
      explicit RecoBeamTracksAutre(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void produce(art::Event& evt) override;
      
  // Plugins should not be copied or assigned.
      RecoBeamTracksAutre(RecoBeamTracksAutre const&) = delete;
      RecoBeamTracksAutre(RecoBeamTracksAutre&&) = delete;
      RecoBeamTracksAutre& operator=(RecoBeamTracksAutre const&) = delete;
      RecoBeamTracksAutre& operator=(RecoBeamTracksAutre&&) = delete;

      void setupAlignAlgs(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob() override;
      //
      void beginRun(art::Run &run) override;
      void endJob() override;

   private:
//
// Some utilities.. 
//      
      bool fFilesAreOpen;
      std::string fTokenJob;
      std::string fSSDClsLabel;
      bool fSelectHotChannelsFromHits; // Probably a better way.    
      bool fDoAlignUV;  
      bool fDoSkipDeadOrHotStrips;  
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      double fPitch;
      int fNumMaxIterAlignAutre;
      double fChiSqCutAlignAutre;
      double fSetMCRMomentum; // The momentum for a given run.. 
//
// access to the geometry.   
//
      runhist::RunHistory *fRunHistory;
      emph::geo::Geometry *fEmgeo;
      std::vector<double> fZlocXPlanes;
      std::vector<double> fZlocYPlanes;
      std::vector<double> fZlocUPlanes;
      std::vector<double> fZlocVPlanes;
      std::vector<double> fRMSClusterCuts;
      std::map<int, char> fXYUVLabels; // keyed on 10*stations + sensor.  Historical, no longer needed.
      //
// access to input data..   
//
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr; // This works, but use the deprecated art interface.. Upper case C
//
// Hot channel Analysis      
//
   std::vector<emph::ssdr::SSDHotChannelList> fHotChans;
//
// The 5 station X and Y aligners (with option of doing only 0 to 4 )
//
      emph::ssdr::SSDAlign3DUVAutre fAlignUV;   // we will (temporarely ?..) use this aligner to reconstruct in X-Z plane.         
//
// CSV tuple output..
// 
      std::ofstream fFOutXY;
      void openOutputCsvFiles();
      
      void dumpXYInfo(int nh, const rbex::BeamTrack &tr);
      
      void alignFiveStations(const art::Event& evt); // event by event alignment. 
      
      void selectByView(char aView, bool alternate45, bool skipDeadOrHotChannels); 
       // move data from fSSDClsPtr to fSSDcls, for a given view. Also apply cuts on the RMS of the clusters. 
      
      
    }; 
    
// .....................................................................................
    emph::RecoBeamTracksAutre::RecoBeamTracksAutre(fhicl::ParameterSet const& pset) : 
      EDProducer(pset), 
      fFilesAreOpen(false),
      fTokenJob    (pset.get<std::string>("tokenJob", "UnDef")),
      fSSDClsLabel (pset.get<std::string>("SSDClsLabel")),
      fSelectHotChannelsFromHits (pset.get<bool>("selectHotChannelsFromHits", true)),
      fDoAlignUV (pset.get<bool>("alignUV", true)),
      fDoSkipDeadOrHotStrips (pset.get<bool>("skipDeadOrHotStrips", false)),
      fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0) , fPitch(0.06),
      fNumMaxIterAlignAutre (pset.get<int>("NumMaxIterAlignAutre", 1000)),
      fChiSqCutAlignAutre (pset.get<double>("ChiSqCutAlignAutre", 1000.)),
      fSetMCRMomentum (pset.get<double>("SetMCRMomentum", 120.)),
      fRunHistory(nullptr), fEmgeo(nullptr), 
      fZlocXPlanes(0), fZlocYPlanes(0), fZlocUPlanes(0), fZlocVPlanes(0)
    {
       std::cerr << " Constructing RecoBeamTracksAutre " << std::endl;
       this->setupAlignAlgs(pset);
       fFilesAreOpen = false;
//       fSSDClsPtr = nullptr;
       fRMSClusterCuts = std::vector<double>{-1.0, 5.};
       this->produces< std::vector<rbex::BeamTrack> >();
    }
    
    void emph::RecoBeamTracksAutre::setupAlignAlgs(const fhicl::ParameterSet& pset)
    {
      std::string fNameHCFH = pset.get<std::string>("NameFileHCFH", "./SSDCalibHotChanSummary_none_1055.txt");
      std::string fNameDCFH = pset.get<std::string>("NameFileHCFH", "./SSDCalibDeadChanSummary_none_1055.txt");
      double aChiSqCut3DUVXY = pset.get<double>("ChiSqCutAlign3DUVXY", 100.);
      double aChiSqCut3DUVUV = pset.get<double>("ChiSqCutAlign3DUVUV", 100.);
      const double aMagnetiKick = pset.get<double>("MagnetKick", -6.12e-4); 
      // default value for transverse (X) kick is for 120 GeV, assuming COMSOL file is correct. based on G4EMPH 
      std::vector<double> aZLocShifts = pset.get<std::vector<double> >("ZLocShifts", std::vector<double>(6, 0.));
      std::vector<double> aPitchAngles =  pset.get<std::vector<double> >("PitchAngles", std::vector<double>(6, 0.));
      std::vector<double> aTransUncert =  pset.get<std::vector<double> >("TransPosUncert", std::vector<double>(6, 0.));
      std::vector<double> aMeanResidY =  pset.get<std::vector<double> >("MeanResidualsY", std::vector<double>(6, 0.));
      std::vector<double> aMeanResidX =  pset.get<std::vector<double> >("MeanResidualsX", std::vector<double>(6, 0.));
      
//      double aRefPointPitchOrYawAngle = pset.get<double>("RefPointPitchOrYawAngle", 3.0); // in mm, in the local frame of the sensor. 
//    confusing and not needed. 
      std::vector<double> aRMSClusterCutsDef{-1.0, 5.};
      fRMSClusterCuts = pset.get<std::vector<double> >("RMSClusterCuts", aRMSClusterCutsDef);
      if (fSelectHotChannelsFromHits) {
        for (int kSt = 0; kSt != 6; kSt++) { 
	  for (int kSe = 0; kSe != 6; kSe++) { 
             emph::ssdr::SSDHotChannelList aHl(fRun, kSt, kSe);
	     aHl.getItFromSSDCalib(fNameHCFH);
	     aHl.getItFromSSDCalib(fNameDCFH);
	     fHotChans.push_back(aHl);
	  }
	}
      }
      // 
      // set the  XY reconstructor and  U aligner. 
      //
      //fAlignUV.SetZLocShifts(aZLocShifts);
      fAlignUV.SetMagnetKick120GeV(aMagnetiKick);
      fAlignUV.SetOtherUncert(aTransUncert);
      fAlignUV.SetFittedResidualsForY(aMeanResidY);
      fAlignUV.SetFittedResidualsForX(aMeanResidX);
      fAlignUV.SetChiSqCutX(aChiSqCut3DUVXY); 
      fAlignUV.SetChiSqCutY(aChiSqCut3DUVXY); 
      fAlignUV.SetChiSqCut(aChiSqCut3DUVUV); 
      fAlignUV.SetTokenJob(fTokenJob);
      
      std::cerr << " .... O.K. keep going ....  " << std::endl; 
    }
    
    void emph::RecoBeamTracksAutre::beginRun(art::Run& run )
    {
     if (fXYUVLabels.size() != 0) return; // Initialization already done, skip 
      // Assume th same geometry for all sub runs (this is called for every subruns, it turns out.. ) 
      std::cerr << " RecoBeamTracksAutre::beginRun, run " << run.id() << std::endl;
      fRunHistory = new runhist::RunHistory(run.run());
      fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
      if ( fXYUVLabels.size() == 0) { 
        for (int kSt = 0; kSt != 6; kSt++) {
          for(int kSs =0; kSs !=6; kSs++) {
            fXYUVLabels.emplace(std::make_pair(10*kSt + kSs, '?'));    
	  }
        }
      }
//
      std::vector<double> XlocXPlanes, YlocYPlanes; // labeled by view 
      std::vector<double> XRotXPlanes, YRotYPlanes; // labeled by view 
      std::vector<double> XRotUPlanes, XRotVPlanes; // labeled by view 
      for (int k=0; k != fEmgeo->NSSDStations(); k++) {
        const emph::geo::SSDStation* aStation = fEmgeo->GetSSDStation(k);
        TVector3 posSt = aStation->Pos();
	std::cerr << " .... For Station by index " <<  k  << " Stations name is " << aStation->Name() 
	          << " Number of planes " << aStation->NPlanes() << std::endl;
	for (int jj=0; jj < aStation->NPlanes(); ++jj) {
	  const emph::geo::Plane* sPlane = aStation->GetPlane(jj);
	  for (int kk=0; kk != sPlane->NSSDs(); kk++) {
	    const emph::geo::Detector* aPlane = sPlane->SSD(kk);
	    char viewChar = '?';
	    if ((std::abs(aPlane->Rot() - 270.*M_PI/180.) < 0.1) || (std::abs(aPlane->Rot() - 90.*M_PI/180.) < 0.1) || 
		(std::abs(aPlane->Rot() + 270.*M_PI/180.) < 0.1) || (std::abs(aPlane->Rot() + 90.*M_PI/180.) < 0.1) ) {
	      TVector3 pos = aPlane->Pos();
	      fZlocXPlanes.push_back(pos[2] + posSt[2]);
	      XlocXPlanes.push_back(pos[0] + posSt[0]);
	      XRotXPlanes.push_back(aPlane->Rot());
	      viewChar = 'X'; // Measuring the X cooredinate of the track. See convention is SSDCalibration module. 
	    }
	    if ((std::abs(aPlane->Rot()) < 0.1) || (std::abs(aPlane->Rot() - 180.*M_PI/180.) < 0.1)) {
	      TVector3 pos = aPlane->Pos();
	      fZlocYPlanes.push_back(pos[2] + posSt[2]);
	      YlocYPlanes.push_back(pos[1] + posSt[1]);
	      YRotYPlanes.push_back(aPlane->Rot());
	      viewChar = 'Y';
	    }
	    if ((std::abs(aPlane->Rot() - 315.*M_PI/180.) < 0.1) || (std::abs(aPlane->Rot() - 45.*M_PI/180.) < 0.1) ) {
	      TVector3 pos = aPlane->Pos();
	      fZlocUPlanes.push_back(pos[2] + posSt[2]);
	      XRotUPlanes.push_back(aPlane->Rot());
	      viewChar = 'U';
	    }
	    if ((std::abs(aPlane->Rot() - 225.*M_PI/180.) < 0.1) || (std::abs(aPlane->Rot() + 45.*M_PI/180.) < 0.1) ) {
	      TVector3 pos = aPlane->Pos();
	      fZlocVPlanes.push_back(pos[2] + posSt[2]);
	      XRotVPlanes.push_back(aPlane->Rot());
	      viewChar = 'V';
	    }
	    fXYUVLabels.at(10*k + kk) = viewChar;
	    std::cerr << " .... .... For Sensor by index " <<  kk  << " Sensor name is " << aPlane->Name() 
		      << " view " << viewChar << "  view, check  " << fXYUVLabels.at(10*k +kk) << std::endl;
	  } // on SS Detector planes of segment of a plane.   
	} // on index k, SSD Stations.
      }
      if (fZlocXPlanes.size() < 5)  { std::cerr << " Not enough X planes to do this study, quit here and now " << std::endl; exit(2); }
      std::cerr << " Number of SSD X planes (vertical strips) " << fZlocXPlanes.size() << std::endl; 
      std::cerr << " Z locations ";
      for (size_t i=0; i != fZlocXPlanes.size(); i++) std::cerr << " " << fZlocXPlanes[i] << ",";
      std::cerr << std::endl << " X locations ";
      for (size_t i=0; i != XlocXPlanes.size(); i++) std::cerr << " " << XlocXPlanes[i] << ",";
      std::cerr << std::endl << " X Rotations ";
      for (size_t i=0; i != XRotXPlanes.size(); i++) std::cerr << " " << XRotXPlanes[i] << ",";
      
      std::cerr << std::endl << " Number of SSD Y planes (Horizontal strips) " << fZlocYPlanes.size() << std::endl;
      std::cerr << " Z locations ";
      for (size_t i=0; i != fZlocYPlanes.size(); i++) std::cerr << " " << fZlocYPlanes[i] << ",";
      std::cerr << std::endl << " Y location " ;
      for (size_t i=0; i != YlocYPlanes.size(); i++) std::cerr << " " << YlocYPlanes[i] << ",";
      std::cerr << std::endl << " U Rotations ";
      for (size_t i=0; i != XRotUPlanes.size(); i++) std::cerr << " " << XRotUPlanes[i] << ",";
      std::cerr << std::endl << " V Rotations ";
      for (size_t i=0; i != XRotVPlanes.size(); i++) std::cerr << " " << XRotVPlanes[i] << ",";
      std::cerr << std::endl << "------------------------------------" << std::endl; 
      // Load the zCoords to the X Y aligner 
      fAlignUV.InitializeCoords(false, fZlocXPlanes, fZlocYPlanes, fZlocUPlanes, fZlocVPlanes); 
      
      std::cerr << " End of ZCoordinates Setting ... " << std::endl << std::endl; 
      
      if (std::abs(fSetMCRMomentum - 120) > 0.1) {
        std::cerr << " Resetting the effective momentum kick for  momentum " << fSetMCRMomentum << std::endl;
	fAlignUV.SetForMomentum(fSetMCRMomentum);
      }
      std::cerr  << std::endl << " ------------- End of RecoBeamTracksAutre::beginRun ------------------" << std::endl << std::endl;
    }
    
    void emph::RecoBeamTracksAutre::beginJob() {
    
    }
    
    void emph::RecoBeamTracksAutre::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " RecoBeamTracksAutre::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
       std::ostringstream fNameXYStrStr, fNameXYUStrStr, fNameXYVStrStr; 
       fNameXYStrStr << "SSDAlign3DXY_Run_" << fRun << "_" << fTokenJob << "_V1.txt";
       std::string fNameXYStr(fNameXYStrStr.str());
       fFOutXY.open(fNameXYStr.c_str());
       fFOutXY<< " spill evt trType nHitsT stXMissed stXExtra stYMissed stYExtra ";
       fFOutXY << "xOff xOffErr xSl xSlErr chiSqX yOff yOffErr ySl ySlErr chiSqY " << std::endl;
      
      fFilesAreOpen = true;
    }
    
    void emph::RecoBeamTracksAutre::endJob() {
      
      std::cerr << " RecoBeamTracksAutre::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
    }
    
    void emph::RecoBeamTracksAutre::dumpXYInfo(int nHitsT, const rbex::BeamTrack &fTrXY) {
     
       fFOutXY << " " << fSubRun << " " << fEvtNum << " " << fTrXY.Type() << " "  << nHitsT;
       
       int stXMissed = -1;  int stXExtra = -1; int stYMissed = -1; int stYExtra = -1;
       std::vector<int> aNhStsX = fAlignUV.GetNHitsXView(); std::vector<int> aNhStsY = fAlignUV.GetNHitsYView();       
       for (size_t kSt=0; kSt != aNhStsX.size(); kSt++) {
         if (aNhStsX[kSt] == 1) continue;
	 if (aNhStsX[kSt] == 0) stXMissed = static_cast<int>(kSt);
	 else stXExtra = kSt; // there might be more than one station.. Take the last one, rather arbitrarily. 
       }
       for (size_t kSt=0; kSt != aNhStsY.size(); kSt++) {
         if (aNhStsY[kSt] == 1) continue;
	 if (aNhStsY[kSt] == 0) stYMissed = static_cast<int>(kSt);
	 else stYExtra = kSt;
       }
       fFOutXY << " " << stXMissed << " " << stXExtra << " " << stYMissed << " " << stYExtra;
       fFOutXY << " " <<  fTrXY.XOffset() << " " << fTrXY.XOffsetErr() << " " << fTrXY.XSlope() 
                      << " " << fTrXY.XSlopeErr() << " " << fTrXY.XChiSq();
       fFOutXY << " " << fTrXY.YOffset() << " " << fTrXY.YOffsetErr() << " " 
                     << fTrXY.YSlope() << " " << fTrXY.YSlopeErr() << " " << fTrXY.YChiSq() << std::endl;
       
     }
    //
   
    void emph::RecoBeamTracksAutre::produce(art::Event& evt) {
    //
    // Intro.. 
    //
      std::unique_ptr< std::vector<rbex::BeamTrack> > btracks(new std::vector<rbex::BeamTrack>);
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      
//      std::cerr << " RecoBeamTracksAutre::analyze , event " << fEvtNum << " and do not much  else.. Stop here  " <<   std::endl; 
//      exit(2);
    //
    // Get the data. 
      //
      //
      evt.getByLabel (fSSDClsLabel, fSSDClsPtr);
      if (fNEvents < 50){
         std::cerr << " Number SSDClusters, deprecated interface " << fSSDClsPtr->size() << std::endl;
      } 
      fAlignUV.alignIt(evt, fSSDClsPtr);
      //
      // We always produce, even if the track is lousy. 
      //
      rbex::BeamTrack theTrack = fAlignUV.GetBeamTrack();
      this->dumpXYInfo(static_cast<int>(fSSDClsPtr->size()), theTrack);
      btracks->push_back( theTrack ); 
      
      evt.put(std::move(btracks));
      
    } // end of Analyze, event by events.  
   
DEFINE_ART_MODULE(emph::RecoBeamTracksAutre)
