////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to understand our SSD clusters,, in particular, the relative alignment 
///          between views.  
/// \author  $Author: lebrun $
/*
 We should probably look at 120 GeV first.. 
 Search for the ovelap for sensor with the same view and  same station. 
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
#include "MagneticField/service/MagneticFieldService.h"
#include "MagneticField/test/TestEmphMagneticField.h"
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
  class StudyOverlapSSDClusters;
}
  //
    
    class emph::StudyOverlapSSDClusters : public art::EDAnalyzer {
    
    public:
      
      explicit StudyOverlapSSDClusters(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
      void analyze(const art::Event& evt);
      
  // Plugins should not be copied or assigned.
      StudyOverlapSSDClusters(StudyOverlapSSDClusters const&) = delete;
      StudyOverlapSSDClusters(StudyOverlapSSDClusters&&) = delete;
      StudyOverlapSSDClusters& operator=(StudyOverlapSSDClusters const&) = delete;
      StudyOverlapSSDClusters& operator=(StudyOverlapSSDClusters&&) = delete;

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
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
//
// access to the geometry.   
//
      art::ServiceHandle<emph::geo::GeometryService> fGeoService;
      emph::geo::Geometry *fEmgeo;
      emph::ssdr::VolatileAlignmentParams *fEmVolAlP;
      std::vector<art::Ptr<rb::SSDCluster> > fSSDclPtrs; // This is what I got from art, see analyze method. 
      std::vector<rb::SSDCluster> fSSDcls; // we will do a deep copy, as my first attempt at using the above vector failed.. lower case c
      art::Handle<std::vector<rb::SSDCluster> > fSSDClsPtr; // This works, but use the deprecated art interface.. Upper case C
//
// CSV tuple output..
// 
      std::ofstream fFOutA1, fFOutB1;
      void openOutputCsvFiles();
      void dumpCls();
            
    }; 
    
// .....................................................................................
    emph::StudyOverlapSSDClusters::StudyOverlapSSDClusters(fhicl::ParameterSet const& pset) : 
      EDAnalyzer(pset), 
      fFilesAreOpen(false),
      fTokenJob (pset.get<std::string>("tokenJob", "UnDef")),
      fSSDClsLabel (pset.get<std::string>("SSDClsLabel")),
      fDumpClusters (pset.get<bool>("dumpClusters", false)),
      fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fNEvents(0) ,
      fGeoService(), fEmgeo(nullptr), fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance())
    {
       std::cerr << " Constructing StudyOverlapSSDClusters " << std::endl;
    }
    
    void emph::StudyOverlapSSDClusters::beginRun(art::Run const &run)
    {
      std::cerr << " StudyOverlapSSDClusters::beginRun, run " << run.id() << std::endl;
      std::cerr  << std::endl << " ------------- End of StudyOverlapSSDClusters::beginRun ------------------" << std::endl << std::endl;
    }
    
    void emph::StudyOverlapSSDClusters::beginJob()
    {
    }
    void emph::StudyOverlapSSDClusters::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " StudyOverlapSSDClusters::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      if (fDumpClusters) { 
        std::string headerS(" spill evt station view sensor wgtAvgStrip wgtRmsStrip ");
        std::ostringstream fNameDumpClustStrStr; fNameDumpClustStrStr << "./SSDClusterTuple_V1_" << fRun << "_" << fTokenJob;
        std::string fNameDumpClustStr(fNameDumpClustStrStr.str());
        std::string fNameDumpClusXStr(fNameDumpClustStr); fNameDumpClusXStr += "_A.txt";
        fFOutA1.open(fNameDumpClusXStr.c_str());
        fFOutA1 << headerS << std::endl;
	std::ostringstream fNameDumpClustBStrStr; fNameDumpClustBStrStr << "./SSDClusterTupleOverlapp_V1_" << fRun << "_" << fTokenJob << "_V1.txt";
	std::string fNameDumpClustBStr(fNameDumpClustBStrStr.str());
	fFOutB1.open(fNameDumpClustBStr.c_str());
	fFOutB1 << " spill evt nClsTot cleanOne x0 y0 ";
	size_t kStStart = (fRun < 2000) ? 4 : 5;  
	size_t kStEnd = (fRun < 2000) ? 6 : 8; 
	for (size_t kSt = kStStart; kSt != kStEnd; kSt++) {
	  for (size_t iView = 1; iView != 4; iView++) { 
	     fFOutB1 << " nCls_" << kSt << "_" << iView 
	             << " strip0_" << kSt << "_" << iView << " strip1_" << kSt << "_" << iView;
	  }
	}
	fFOutB1 << std::endl;
      }
      fFilesAreOpen = true;
      
    }
    
    void emph::StudyOverlapSSDClusters::endJob() {
      
      std::cerr << " StudyOverlapSSDClusters::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      if (fDumpClusters) fFOutA1.close(); 
      fFOutB1.close();
    }
    
    void emph::StudyOverlapSSDClusters::dumpCls() {      
     if (fNEvents < 20) {
       std::cerr << " emph::StudyOverlapSSDClusters::dumpXYCls, number of cluster for evt " << fEvtNum <<  "  is " << fSSDClsPtr->size() << std::endl;
     }
     
     for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aStation = itCl->Station(); // iterator to pointer to the cluster. 
	if ((itCl->Sensor() == -1) || ( aStation == -1)) continue;
	geo::sensorView newView = itCl->View();
        int aSensor = itCl->Sensor();
	if ((aSensor == -1) || (aStation == -1)) continue;
        std::ostringstream aLineStrStr; 
//        std::string headerS(" subRun evt station sensor nCl iCl wgtAvgStrip wgtRmsStrip avgADC ");
	fFOutA1 << " " << fSubRun << " " << fEvtNum << " " << itCl->Station() << " " << itCl->View() << " " << itCl->Sensor();
	fFOutA1 << " " << itCl->WgtAvgStrip() << " " << itCl->WgtRmsStrip() << std::endl;  
      }
    }
    
    void emph::StudyOverlapSSDClusters::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      
//      std::cerr << " StudyOverlapSSDClusters::analyze , event " << fEvtNum << " and do not much  " <<   std::endl; 
      
    //
    // Get the data. 
      //
      evt.getByLabel (fSSDClsLabel, fSSDClsPtr);
      if (fSSDClsPtr->size() == 0) return;
//      this->testAccess(); 
      if (fDumpClusters) this->dumpCls();
      //
      // First, look at the event multiplicity. 
      //
      int nClX0 = 0; int nClY0 = 0; int nClX1 = 0; int nClY1 = 0;
      double x0 = DBL_MAX; double y0 = DBL_MAX;
      for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
        int aStation = itCl->Station(); // iterator to pointer to the cluster. 
	if ((aStation == 0) && (itCl->View() == emph::geo::X_VIEW)) {
	   nClX0++; x0 = itCl->WgtAvgStrip(); 
	}
	if ((aStation == 0) && (itCl->View() == emph::geo::Y_VIEW)) {
	   nClY0++; y0 = itCl->WgtAvgStrip(); 
	}
	if ((aStation == 1) && (itCl->View() == emph::geo::X_VIEW)) nClX1++;
	if ((aStation == 1) && (itCl->View() == emph::geo::Y_VIEW)) nClY1++;
      }
      const int nClean01 = ((nClX0 == 1) && (nClX1 == 1) && (nClY0 == 1) && (nClY1 == 1)) ? 1 : 0; 
      fFOutB1 << " " << fSubRun << " " << fEvtNum << " " << fSSDClsPtr->size() << " " << nClean01 << " " << x0 << " " << y0; 
      size_t kStStart = (fRun < 2000) ? 4 : 5;  
      size_t kStEnd = (fRun < 2000) ? 6 : 8; 
      size_t numDoubleSens = (kStEnd -kStStart)*3; // 3 views per stations;  
      std::vector<int>  nClDS0(numDoubleSens, 0);
      std::vector<int>  nClDS1(numDoubleSens, 0);
      std::vector<double>  stripDS0(numDoubleSens, 9999.);
      std::vector<double>  stripDS1(numDoubleSens, 9999.);
      size_t iSensD=0;
      for (size_t kSt = kStStart; kSt != kStEnd; kSt++) {     
        for (size_t kView = 0; kView != 3; kView++) {
	  emph::geo::sensorView aViewXY = emph::geo::INIT; 
	  emph::geo::sensorView aViewU = emph::geo::INIT; emph::geo::sensorView aViewW = emph::geo::INIT;
	  switch (kView) { // Casting business.. we have no station with U & W views (I think..) 
	    case 0 :
	      { aViewXY = emph::geo::X_VIEW; break; }
	    case 1 :
	      { aViewXY = emph::geo::Y_VIEW; break; }
	    case 2 : 
	      { aViewU = emph::geo::U_VIEW;  
	        aViewW = emph::geo::W_VIEW; break;}
	  }
	  for (size_t kSens=0; kSens !=2; kSens++) { 
            for(std::vector<rb::SSDCluster>::const_iterator itCl = fSSDClsPtr->cbegin(); itCl != fSSDClsPtr->cend(); itCl++) {
	      if (itCl->Station() != static_cast<int>(kSt)) continue;
	      if ((kView < 2) && (itCl->View() != aViewXY)) continue;
	      if ((kView == 2) && (itCl->View() != aViewU) && (itCl->View() != aViewW)) continue;
	      if (itCl->Sensor() != static_cast<int>(kSens)) continue;
	      if (kSens == 0) {  nClDS0[iSensD]++;  stripDS0[iSensD] = itCl->WgtAvgStrip(); }  
	      if (kSens == 1) {  nClDS1[iSensD]++;  stripDS1[iSensD] = itCl->WgtAvgStrip(); }
            }
	  } // on clusters.. 
	  iSensD++;
        }
      }
      // Dump to Ascii 
      iSensD = 0;
      for (size_t kSt = kStStart; kSt != kStEnd; kSt++) {     
        for (size_t kView = 0; kView != 3; kView++) {
          const int ok01 = ((nClDS0[iSensD] == 1) && (nClDS1[iSensD] == 1)) ? 1 : 0;
	  fFOutB1 << " " << ok01 << " " << stripDS0[iSensD] << " " << stripDS1[iSensD];
	  iSensD++; 
	}
      }
      fFOutB1 << std::endl; 
    } // end of Analyze, event by events.  
   
DEFINE_ART_MODULE(emph::StudyOverlapSSDClusters)
