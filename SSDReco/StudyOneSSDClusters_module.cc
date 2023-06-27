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
#include "RecoBase/SSDCluster.h"
#include "SSDReco/SSDHotChannelList.h"
#include "SSDReco/SSDAlign2DXYAlgo1.h"
#include "SSDReco/SSDAlign3DUVAlgo1.h"
//

namespace emph {
  class StudyOneSSDClusters;
}
  //
    
    class emph::StudyOneSSDClusters : public art::EDAnalyzer {
    
    public:
      
      explicit StudyOneSSDClusters(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void analyze(const art::Event& evt);
      
  // Plugins should not be copied or assigned.
      StudyOneSSDClusters(StudyOneSSDClusters const&) = delete;
      StudyOneSSDClusters(StudyOneSSDClusters&&) = delete;
      StudyOneSSDClusters& operator=(StudyOneSSDClusters const&) = delete;
      StudyOneSSDClusters& operator=(StudyOneSSDClusters&&) = delete;


      // Optional if you want to be able to configure from event display, for example
      void setupAlignAlgs(const fhicl::ParameterSet& pset);

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
      bool fDumpClusters;    
      bool fSelectHotChannels;  // Mostly obsolete. 
      bool fSelectHotChannelsFromHits; // Probably a better way.    
      bool fDoAlignX;    
      bool fDoAlignY;  
      bool fDoAlignUV; 
      bool fDoGenCompact; 
      bool fDoAlignXAlt45;  
      bool fDoAlignYAlt45;  
      bool fDoAlignYAlt5;  
      bool fDoSkipDeadOrHotStrips;  
      bool fDoLastIs4AlignAlgo1; 
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      double fPitch;
      int fNumMaxIterAlignAlgo1;
      double fChiSqCutAlignAlgo1;
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
      std::map<int, char> fXYUVLabels; // keyed on 10*stations + sensor. 
// 
// Container for the hot channels. 
// 
//
// access to input data..   
//
      std::vector<art::Ptr<rb::SSDCluster> > fSSDclPtrs; // This is what I got from art, see analyze method. 
      std::vector<rb::SSDCluster> fSSDcls; // we will do a deep copy, as my first attempt at using the above vector failed.. lower case c
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr; // This works, but use the deprecated art interface.. Upper case C
//
// Hot channel Analysis      
//
   std::vector<emph::ssdr::SSDHotChannelList> fHotChans;
//
// The 5 station X and Y aligners (with option of doing only 0 to 4 )
//
      emph::ssdr::SSDAlign2DXYAlgo1 fAlignX, fAlignY;
      emph::ssdr::SSDAlign3DUVAlgo1 fAlignUV;           
//
// CSV tuple output..
// 
      std::ofstream fFOutA1X, fFOutA1Y, fFOutA1U, fFOutA1V;
      void openOutputCsvFiles();
      
      void dumpXYCls();
      void fillHotChannels();
      void fillDeadChannels();
      
      void testAccess(); 
      
      void alignFiveStations(const art::Event& evt); // event by event alignment. 
      
      void selectByView(char aView, bool alternate45, bool skipDeadOrHotChannels); 
       // move data from fSSDClsPtr to fSSDcls, for a given view. Also apply cuts on the RMS of the clusters. 
      
      char getView(std::vector<rb::SSDCluster>::const_iterator itCl) const; 
      
      
    }; 
    
// .....................................................................................
    emph::StudyOneSSDClusters::StudyOneSSDClusters(fhicl::ParameterSet const& pset) : 
      EDAnalyzer(pset), 
      fFilesAreOpen(false),
      fTokenJob (pset.get<std::string>("tokenJob", "UnDef")),
      fSSDClsLabel (pset.get<std::string>("SSDClsLabel")),
      fDumpClusters (pset.get<bool>("dumpClusters", false)),
      fSelectHotChannels (pset.get<bool>("selectHotChannels", false)),
      fSelectHotChannelsFromHits (pset.get<bool>("selectHotChannelsFromHits", false)),
      fDoAlignX (pset.get<bool>("alignX", false)),
      fDoAlignY (pset.get<bool>("alignY", false)),
      fDoAlignUV (pset.get<bool>("alignUV", false)),
      fDoGenCompact (pset.get<bool>("genCompactEvts", false)),
      fDoAlignXAlt45 (pset.get<bool>("alignXAlt45", false)),
      fDoAlignYAlt45 (pset.get<bool>("alignYAlt45", false)),
      fDoAlignYAlt5 (pset.get<bool>("alignYAlt5", false)),
      fDoSkipDeadOrHotStrips (pset.get<bool>("skipDeadOrHotStrips", false)),
      fDoLastIs4AlignAlgo1 (pset.get<bool>("LastIs4AlignAlgo1", false)),
      fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0) , fPitch(0.06),
      fNumMaxIterAlignAlgo1 (pset.get<int>("NumMaxIterAlignAlgo1", 1000)),
      fChiSqCutAlignAlgo1 (pset.get<double>("ChiSqCutAlignAlgo1", 1000.)),
      fSetMCRMomentum (pset.get<double>("SetMCRMomentum", 120.)),
      fRunHistory(nullptr), fEmgeo(nullptr), 
      fZlocXPlanes(0), fZlocYPlanes(0), fZlocUPlanes(0), fZlocVPlanes(0)
    {
       std::cerr << " Constructing StudyOneSSDClusters " << std::endl;
       this->setupAlignAlgs(pset);
       fFilesAreOpen = false;
//       fSSDClsPtr = nullptr;
       fAlignX.SetTheView('X'); fAlignY.SetTheView('Y');
       fRMSClusterCuts = std::vector<double>{-1.0, 5.};
    }
    
    void emph::StudyOneSSDClusters::setupAlignAlgs(const fhicl::ParameterSet& pset)
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
//      if (fDoGenCompact)  std::cerr << " StudyOneSSDCluster, we should generate the compact data set " << std::endl;
//      else std::cerr << " StudyOneSSDCluster, we should NOT generate the compact data set " << std::endl;
//      std::cerr << " ...and quit for now.. " << std::endl; exit(2);
//      double aRefPointPitchOrYawAngle = pset.get<double>("RefPointPitchOrYawAngle", 3.0); // in mm, in the local frame of the sensor. 
//    confusing and not needed. 
      std::vector<double> aRMSClusterCutsDef{-1.0, 5.};
      fRMSClusterCuts = pset.get<std::vector<double> >("RMSClusterCuts", aRMSClusterCutsDef);
      if ((!fDumpClusters) && (!fSelectHotChannels) && (!fDoAlignX) && (!fDoAlignY) && (!fDoAlignXAlt45)
                           && (!fDoAlignYAlt45) && (!fDoAlignYAlt5) && (!fDoAlignUV)) { 
        std::cerr << " .... Nothing to do !!! Therefore, quit here and now  " << std::endl; exit(2);
      }
      if (fSelectHotChannels || fSelectHotChannelsFromHits) {
        for (int kSt = 0; kSt != 6; kSt++) { 
	  for (int kSe = 0; kSe != 6; kSe++) { 
             emph::ssdr::SSDHotChannelList aHl(fRun, kSt, kSe);
	     if (fSelectHotChannelsFromHits) {
	       aHl.getItFromSSDCalib(fNameHCFH);
	       aHl.getItFromSSDCalib(fNameDCFH);
	     }
	     fHotChans.push_back(aHl);
	  }
	}
      }
      fAlignX.SetTokenJob(fTokenJob);
      fAlignY.SetTokenJob(fTokenJob);
      fAlignX.SetDoAling0to4(fDoLastIs4AlignAlgo1);
      fAlignY.SetDoAling0to4(fDoLastIs4AlignAlgo1);
      fAlignX.SetZLocShifts(aZLocShifts);
      fAlignY.SetZLocShifts(aZLocShifts);
      fAlignX.SetOtherUncert(aTransUncert);
      fAlignX.SetMagnetKick120GeV(aMagnetiKick);
      fAlignY.SetOtherUncert(aTransUncert);
      fAlignY.SetPitchAngles(aPitchAngles);
//      fAlignY.SetRefPtForPitchOrYawAngle(aRefPointPitchOrYawAngle);
      fAlignY.SetFittedResiduals(aMeanResidY);
      fAlignX.SetFittedResiduals(aMeanResidX);
      
// 
// Same for UV aligner. 
//
//      fAlignUV.SetZLocShifts(aZLocShifts);
      fAlignUV.SetMagnetKick120GeV(aMagnetiKick);
      fAlignUV.SetOtherUncert(aTransUncert);
      fAlignUV.SetFittedResidualsForY(aMeanResidY);
      fAlignUV.SetFittedResidualsForX(aMeanResidX);
      fAlignUV.SetChiSqCutXY(aChiSqCut3DUVXY); 
      fAlignUV.SetChiSqCut(aChiSqCut3DUVUV); 
      fAlignUV.SetTokenJob(fTokenJob);
      
      std::cerr << " .... O.K. keep going ....  " << std::endl; 
    }
    void emph::StudyOneSSDClusters::beginRun(art::Run const &run)
    {
      if (fXYUVLabels.size() != 0) return; // Initialization already done, skip 
      // Assume th same geometry for all sub runs (this is called for every subruns, it turns out.. ) 
      std::cerr << " StudyOneSSDClusters::beginRun, run " << run.id() << std::endl;
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
        emph::geo::SSDStation aStation = fEmgeo->GetSSDStation(k);
        TVector3 posSt = aStation.Pos();
	std::cerr << " .... For Station by index " <<  k  << " Stations name is " << aStation.Name() 
	          << " Number of planes " << aStation.NSSDs() << std::endl;
        for (int kk=0; kk != aStation.NSSDs(); kk++) {
          emph::geo::Detector aPlane = aStation.GetSSD(kk);
	  char viewChar = '?';
	  if ((std::abs(aPlane.Rot() - 270.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() - 90.*M_PI/180.) < 0.1) || 
	     (std::abs(aPlane.Rot() + 270.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() + 90.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos();
	    fZlocXPlanes.push_back(pos[2] + posSt[2]);
	    XlocXPlanes.push_back(pos[0] + posSt[0]);
	    XRotXPlanes.push_back(aPlane.Rot());
	    viewChar = 'X'; // Measuring the X cooredinate of the track. See convention is SSDCalibration module. 
	  }
	  if ((std::abs(aPlane.Rot()) < 0.1) || (std::abs(aPlane.Rot() - 180.*M_PI/180.) < 0.1)) {
	    TVector3 pos = aPlane.Pos();
	    fZlocYPlanes.push_back(pos[2] + posSt[2]);
	    YlocYPlanes.push_back(pos[1] + posSt[1]);
	    YRotYPlanes.push_back(aPlane.Rot());
	    viewChar = 'Y';
	  }
	  if ((std::abs(aPlane.Rot() - 225.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() + 45.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos();
	    if (fZlocUPlanes.size() < 2) { // From what I have been told.. 
	      fZlocUPlanes.push_back(pos[2] + posSt[2]);
	      XRotUPlanes.push_back(aPlane.Rot());
	    }
	    viewChar = 'U';
	  }
	  if ((std::abs(aPlane.Rot() - 315.*M_PI/180.) < 0.1) || (std::abs(aPlane.Rot() - 45.*M_PI/180.) < 0.1) ) {
	    TVector3 pos = aPlane.Pos(); 
	    fZlocVPlanes.push_back(pos[2] + posSt[2]);
	    XRotVPlanes.push_back(aPlane.Rot());
	    viewChar = 'V';
	  }
	  fXYUVLabels.at(10*k + kk) = viewChar;
	  std::cerr << " .... .... For Sensor by index " <<  kk  << " Sensor name is " << aPlane.Name() 
	            << " view " << viewChar << "  view, check  " << fXYUVLabels.at(10*k +kk) << std::endl;
        } // on SS Detector planes of segment of a plane.   
      } // on index k, SSD Stations.
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
      fAlignX.InitializeCoords(fDoLastIs4AlignAlgo1, fZlocXPlanes); 
      fAlignX.SetChiSqCut1(fChiSqCutAlignAlgo1); fAlignX.SetNumIterMax(fNumMaxIterAlignAlgo1);
      fAlignY.InitializeCoords(fDoLastIs4AlignAlgo1, fZlocYPlanes); 
      fAlignY.SetChiSqCut1(fChiSqCutAlignAlgo1); fAlignY.SetNumIterMax(fNumMaxIterAlignAlgo1);
      fAlignUV.InitializeCoords(false, fZlocXPlanes, fZlocYPlanes, fZlocUPlanes, fZlocVPlanes); 
      
      std::cerr << " End of ZCoordinates Setting ... " << std::endl << std::endl; 
      
      if (std::abs(fSetMCRMomentum - 120) > 0.1) {
        std::cerr << " Resetting the effective momentum kick for  momentum " << fSetMCRMomentum << std::endl;
        fAlignX.SetForMomentum(fSetMCRMomentum); fAlignY.SetForMomentum(fSetMCRMomentum);
	fAlignUV.SetForMomentum(fSetMCRMomentum);
      }
      std::cerr  << std::endl << " ------------- End of StudyOneSSDClusters::beginRun ------------------" << std::endl << std::endl;
    }
    
    void emph::StudyOneSSDClusters::beginJob()
    {
    }
    void emph::StudyOneSSDClusters::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " StudyOneSSDClusters::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      if (fDumpClusters) { 
        std::string headerS(" subRun evt station sensor nCl iCl wgtAvgStrip wgtRmsStrip avgADC ");
        std::ostringstream fNameDumpClustStrStr; fNameDumpClustStrStr << "./SSDClusterTuple_V1_" << fRun << "_" << fTokenJob;
        std::string fNameDumpClustStr(fNameDumpClustStrStr.str());
        std::string fNameDumpClusXStr(fNameDumpClustStr); fNameDumpClusXStr += "_X.txt";
        fFOutA1X.open(fNameDumpClusXStr.c_str());
        fFOutA1X << headerS << std::endl;
        std::string fNameDumpClusYStr(fNameDumpClustStr); fNameDumpClusYStr += "_Y.txt";
        fFOutA1Y.open(fNameDumpClusYStr.c_str());
        fFOutA1Y << headerS << std::endl;
        std::string fNameDumpClusUStr(fNameDumpClustStr); fNameDumpClusUStr += "_U.txt";
        fFOutA1U.open(fNameDumpClusUStr.c_str());
        fFOutA1U << headerS << std::endl;
        std::string fNameDumpClusVStr(fNameDumpClustStr); fNameDumpClusVStr += "_V.txt";
        fFOutA1V.open(fNameDumpClusVStr.c_str());
        fFOutA1V << headerS << std::endl;
      }
      fFilesAreOpen = true;
      
    }
    
    void emph::StudyOneSSDClusters::endJob() {
      
      std::cerr << " StudyOneSSDClusters::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      if (fDumpClusters) {
        fFOutA1X.close(); fFOutA1Y.close();fFOutA1U.close(); fFOutA1V.close();
      }
      if (fSelectHotChannels) {
        for (std::vector<emph::ssdr::SSDHotChannelList>::iterator itHl = fHotChans.begin(); itHl != fHotChans.end(); itHl++) 
          itHl->tallyIt(5.0);
        std::string dirName("./"); // in my build directory, 
        for (std::vector<emph::ssdr::SSDHotChannelList>::const_iterator itHl = fHotChans.cbegin(); itHl != fHotChans.cend(); itHl++) 
          itHl->dumpItToFile(dirName);
      }
    }
    
    void emph::StudyOneSSDClusters::dumpXYCls() {      
     // first count the number of hits for the X stations and Y station.. Will do the U and V laters.. 
     std::vector<int> numClsX(6, 0);
     std::vector<int> numClsY(6, 0);
     std::vector<int> numClsV(6, 0);
     std::vector<int> numClsU(6, 0);
     if (fNEvents < 20) {
       std::cerr << " emph::StudyOneSSDClusters::dumpXYCls, number of cluster for evt " << fEvtNum <<  "  is " << fSSDClsPtr->size() << std::endl;
     }
     
     for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aStation = itCl->Station(); // iterator to pointer to the cluster. 
	if ((itCl->Sensor() == -1) || ( aStation == -1)) continue;
        char aView = this->getView(itCl);
	geo::sensorView newView = itCl->View();
	if (aView == 'X')  {
	  if (newView != geo::X_VIEW)  {
	    std::cerr << " StudyOneSSDClusters::dumpXYCls Inconsistent view, X view here and new View " 
	               << newView << " fatal quit here and now " << std::endl;
	    exit(2);       
	  }
	  numClsX[aStation]++;
	}
	if (aView == 'Y')  {
	  if (newView != geo::Y_VIEW)  {
	    std::cerr << " StudyOneSSDClusters::dumpXYCls Inconsistent view, Y view here and new View " 
	               << newView << " fatal quit here and now " << std::endl;
	    exit(2);       
	  }
	  numClsY[aStation]++;
	}
	if (itCl->View() == geo::U_VIEW) numClsU[aStation]++;
	if (itCl->View() == geo::W_VIEW) numClsV[aStation]++;
      }
      for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aSensor = itCl->Sensor();
        int aStation = itCl->Station();
	if ((aSensor == -1) || (aStation == -1)) continue;
        char aView =  fXYUVLabels.at(10*aStation+aSensor); // we have done the check above.. 
	int nn = 0;
	if (aView == 'X') nn = numClsX[aStation]; 
	if (aView == 'Y') nn = numClsY[aStation];
	if (itCl->View() == geo::U_VIEW) nn = numClsU[aStation];
	if (itCl->View() == geo::W_VIEW) nn = numClsV[aStation];
        std::ostringstream aLineStrStr; 
//        std::string headerS(" subRun evt station sensor nCl iCl wgtAvgStrip wgtRmsStrip avgADC ");
	aLineStrStr << " " << fSubRun << " " << fEvtNum << " " << itCl->Station() << " " << itCl->Sensor();
	aLineStrStr << " " << nn << " " << itCl->ID() << " " << itCl->WgtAvgStrip() << " " 
	            << itCl->WgtRmsStrip() << " " << itCl->AvgADC();
        std::string aLineStr(aLineStrStr.str()); 
	if (aView == 'X')  fFOutA1X << aLineStr << std::endl;  
	if (aView == 'Y')  fFOutA1Y << aLineStr << std::endl;  
	if (itCl->View() == geo::U_VIEW)  fFOutA1U << aLineStr << std::endl;  
	if (itCl->View() == geo::W_VIEW)  fFOutA1V << aLineStr << std::endl;  
      }
    }
    
    
    
    void emph::StudyOneSSDClusters::fillHotChannels() {      
      for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aSensor = itCl->Sensor();
        int aStation = itCl->Station();
	if((aSensor == -1) || (aStation == -1)) continue;
        for (std::vector<emph::ssdr::SSDHotChannelList>::iterator itHl = fHotChans.begin(); itHl != fHotChans.end(); itHl++) {
	  if ((itHl->Station() == aStation) && (itHl->Sensor() == aSensor)) itHl->fillHit(itCl);
        }
      }
    }
    
   void emph::StudyOneSSDClusters::testAccess () {
   
     if (fSSDClsPtr->size() == 0) return;
     std::vector<rb::SSDCluster>::const_iterator itCl1 = fSSDClsPtr->cbegin();
     std::cerr << " Setting the ID of the hitst cluster on station " << itCl1->Station() << " sensor " << itCl1->Sensor() << std::endl;
     // Indeed, this does not compile.. 
     // std::vector<rb::SSDCluster>::iterator it1 = fSSDClsPtr->begin();
     // it1->SetID(56234);
     // std::cerr << " check this ID " << it1->ID() << std::endl; 
    }
    //
   char emph::StudyOneSSDClusters::getView(std::vector<rb::SSDCluster>::const_iterator itCl) const {
   
       int aSensor = itCl->Sensor();
       if ((aSensor < 0 ) || (aSensor >= 6) ) {
         std::cerr << " StudyOneSSDClusters::getView, invalid sensor number " << aSensor << " fatal, quit!" << std::endl; 
	 std::cerr <<  " Faulty cluster " << std::endl;
	 std::cerr << (*itCl) << std::endl;	 
	 exit(2);
       } 
       int aStation = itCl->Station();
       if ((aStation < 0 ) || (aStation >= 6) ) {
         std::cerr << " StudyOneSSDClusters::getView, invalid station number " << aStation << " fatal, quit!" << std::endl;
	 std::cerr <<  " Faulty cluster " << std::endl;
	 std::cerr << (*itCl) << std::endl;
	 exit(2);
	 
       }
       char aView =  fXYUVLabels.at(10*aStation+aSensor);
       if (aView  == '?') {
          std::cerr << " StudyOneSSDClusters::getView, invalid sensor/station view, station " 
	             << aStation << " sensor " << aSensor <<  " fatal, quit!" << std::endl; 
		     
	 std::cerr <<  " Faulty cluster " << std::endl;
	 std::cerr << (*itCl) << std::endl;
		     
	 exit(2);
       }
       return aView;
   }
   
   void emph::StudyOneSSDClusters::selectByView(char theView, bool alternate45, bool skipDeadOrHotStrips) {
     fSSDcls.clear();
     for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aSensor = itCl->Sensor();
        int aStation = itCl->Station();
	if((aSensor == -1) || (aStation == -1)) continue;
	const double aRMS = itCl->WgtRmsStrip(); 
//	std::cerr << " ... emph::StudyOneSSDClusters::selectByView, aRMS " << aRMS << "  .. Cuts, min Val  " 
//	          << fRMSClusterCuts[0] << " max Val " <<  fRMSClusterCuts[1] << std::endl;
	if (aRMS < fRMSClusterCuts[0]) continue;
	if (aRMS > fRMSClusterCuts[1]) continue;
        char aView = this->getView(itCl);
	if (aView != theView) continue;
	if (aStation > 3) {
	  if (theView == 'X') {
	    if ((!alternate45) && (itCl->Sensor() == 0)) continue; // The Proton peak is mostly on Sensor 0 
	    if ((alternate45) && (itCl->Sensor() == 1)) continue;// ...  For both station 4 and 5 .. Yes, at least for run 1055
	  } else if (theView == 'Y') {
	    if ((!alternate45) && (itCl->Sensor() == 2)) continue; // The Proton peak is mostly on Sensor 3 
	    if ((alternate45) && (itCl->Sensor() == 3)) continue; // True for station 4 and 5. Station 4 data looks dismal.. 
	  }
	}
	bool accept = true;
	if (skipDeadOrHotStrips) {
	  // a bit tedious, 
	  for (std::vector<emph::ssdr::SSDHotChannelList>::iterator itHl = fHotChans.begin(); itHl != fHotChans.end(); itHl++) {
            if ((itHl->Station() != aStation)  || (itHl->Sensor() != aSensor)) continue;
	    for (int kStrip = itCl->MinStrip(); kStrip <= itCl->MaxStrip(); kStrip++) {
	      if ((itHl->IsHot(kStrip)) ||(itHl->IsDead(kStrip))) { accept = false; break; }  
	    }
	  } 
	}
	if (accept) fSSDcls.push_back(*itCl);
      } // on original clusters.. 
    }
    void emph::StudyOneSSDClusters::alignFiveStations(const art::Event& evt) {
      if (fDoAlignX) {
        this->selectByView('X', false, fDoSkipDeadOrHotStrips); 
        fAlignX.alignIt(evt, fSSDcls);
      } 
      if (fDoAlignY) {
        this->selectByView('Y', false, fDoSkipDeadOrHotStrips); 
        fAlignY.alignIt(evt, fSSDcls);
      } 
      if (fDoAlignYAlt45 || fDoAlignYAlt5) {
        this->selectByView('Y', true, fDoSkipDeadOrHotStrips); 
        if (fDoAlignYAlt45) fAlignY.alignItAlt45(false, evt, fSSDcls);
        if (fDoAlignYAlt5) fAlignY.alignItAlt45(true, evt, fSSDcls);
      } 
      if (fDoAlignXAlt45) {
        this->selectByView('X', true, fDoSkipDeadOrHotStrips); 
        fAlignX.alignItAlt45(false, evt, fSSDcls);
      } 
      if (fDoAlignUV) {
        fAlignUV.alignIt(evt, fSSDClsPtr);
	if (fDoGenCompact) {
//	   std::cerr << " emph::StudyOneSSDClusters::alignFiveStations calling dumpCompactEvt, and quit here and now .. " << std::endl; exit(2);
	   fAlignUV.dumpCompactEvt(fSubRun, fEvtNum, fSSDClsPtr);
	}  
      }
    }
    void emph::StudyOneSSDClusters::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      
//      std::cerr << " StudyOneSSDClusters::analyze , event " << fEvtNum << " and do not much  " <<   std::endl; 
      
    //
    // Get the data. 
      auto hdlCls = evt.getHandle<std::vector<rb::SSDCluster>>(fSSDClsLabel);
      art::fill_ptr_vector(fSSDclPtrs, hdlCls);
      if (fSSDclPtrs.size() == 0) return; // nothing to do, no data. 
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
//      this->testAccess(); 
      fSSDcls = (*fSSDClsPtr); // deep copy here. Probably can be avoided.. Unless we want to make changes to the cluster, which we 
      if (fNEvents < 50){
         std::cerr << " Number SSDClusters, deprecated interface (with deep copy) " << fSSDcls.size() << std::endl;
        if (fSSDcls.size() != 0) {
          std::vector<rb::SSDCluster>::const_iterator itClsTest = fSSDcls.cbegin(); 
          std::cerr << " Station for the first cluster, old interface   " << itClsTest->Station() << std::endl;
          std::vector<rb::SSDCluster>::const_iterator itClsTest2 = fSSDClsPtr->cbegin(); 
          std::cerr << " Station for the first cluster, old interface no deep copy   " << itClsTest2->Station() << std::endl;
	  
        }
      } 
      if (fDumpClusters) this->dumpXYCls();
      if (fSelectHotChannels) this->fillHotChannels(); 
      if (fDoAlignX || fDoAlignY || fDoAlignXAlt45 || fDoAlignYAlt45 || fDoAlignYAlt5 || fDoAlignUV ) this->alignFiveStations(evt);
    } // end of Analyze, event by events.  
   
DEFINE_ART_MODULE(emph::StudyOneSSDClusters)
