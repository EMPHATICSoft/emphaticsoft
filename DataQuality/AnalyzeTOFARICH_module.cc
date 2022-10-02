////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create online monitoring plots
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
// C++ includes
#include <memory>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
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
// Reco stuff
// 
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "RecoBase/TOFHit.h"
#include "RecoBase/ARing.h"
#include "RawData/TRB3RawDigit.h"
using namespace emph;

namespace emph {
  namespace dq {
    class AnalyzeTOFARICH : public art::EDAnalyzer {
    public:
      explicit AnalyzeTOFARICH(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
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
      std::string fTOFInfoLabel, fARingInfoLabel;
      runhist::RunHistory* fRunHistory;
      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubRun;
      int fPrevSubRun;
      unsigned int fEvtNum;
      int fPrevEvtNum;
      unsigned int fNEvents;
//
// CSV tuple output..
//      
      std::ofstream fFOutA1;
      void openOutputCsvFiles();
      
    };    
// .....................................................................................
    AnalyzeTOFARICH::AnalyzeTOFARICH(fhicl::ParameterSet const& pset) : EDAnalyzer(pset) 
    {
       std::cerr << " Constructing AnalyzeTOFARICH " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;
    }
    
    void AnalyzeTOFARICH::reconfigure(const fhicl::ParameterSet& pset)
    {
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fTOFInfoLabel = pset.get<std::string>("TOFInfoLabel");
      fARingInfoLabel = pset.get<std::string>("ARingInfoLabel");
    }
    void AnalyzeTOFARICH::beginRun(art::Run const &run)
    {
      // initialize channel map
      fChannelMap = new emph::cmap::ChannelMap();
      fRunHistory = new runhist::RunHistory(run.run());
      fChannelMap->LoadMap(fRunHistory->ChanFile());
    }
    void AnalyzeTOFARICH::beginJob()
    {
    }
    void AnalyzeTOFARICH::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " AnalyzeTOFARICH::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      std::ostringstream fNameT0AStrStr; fNameT0AStrStr << "./TOFARICH0Tuple_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0AStr(fNameT0AStrStr.str());
      fFOutA1.open(fNameT0AStr.c_str());
      fFOutA1 << " subRun evt trigNumPart trigSum nT0Seg T0SegNum nRPC RPCStripNum nARing xRing yRing radRing nHitRing ";
      fFOutA1 << " " << std::endl;
      fFilesAreOpen = true;
    }
    void AnalyzeTOFARICH::endJob() {
      
      std::cerr << " AnalyzeTOFARICH::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
    
    }
    void AnalyzeTOFARICH::analyze(const art::Event& evt) {
    //
    // Intro.. 
    //
      ++fNEvents;
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();  
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
    //
    // Get the TOF data. 
      art::Handle<rb::TOFHit> theTOFInfo;
      evt.getByLabel(fTOFInfoLabel, theTOFInfo);
    //
    // Get the reconstructed ARing  data. 
    //
      art::Handle<std::vector<rb::ARing> > theARingInfo;
      evt.getByLabel(fARingInfoLabel, theARingInfo);
    //
    // Dump this information. 
    // 
      int aT0SegNum = (theTOFInfo->NumT0SegmentHits()  == 0) ? INT_MAX : theTOFInfo->cbeginT0()->SegNumber();
      int aRPCStripNum = (theTOFInfo->NumRPCStripHits()  == 0) ? INT_MAX : theTOFInfo->cbeginRPC()->StripNumber();
      fFOutA1 << " " << fSubRun << " " << fEvtNum;
      fFOutA1  << " " << theTOFInfo->TrigHit()->isSingleBeamParticle() << " " << theTOFInfo->TrigHit()-> Sum4PMTs() 
              << " " << theTOFInfo->NumT0SegmentHits() << " " << aT0SegNum 
	      << " " << theTOFInfo->NumRPCStripHits() << " " << aRPCStripNum;
	
      int nRingHits = 0;
      double xRing = DBL_MAX;
      double yRing = DBL_MAX;
      double radRing = DBL_MAX;
      fFOutA1  << " " << theARingInfo->size();
      if (theARingInfo->size() == 1) {
        std::vector<rb::ARing>::const_iterator it= theARingInfo->cbegin();
	xRing = it->XCenter(); 
	yRing = it->YCenter();
	radRing = it->Radius();
	nRingHits = it->NHits();
      }
      fFOutA1 << " " << 	xRing << " " << yRing << " " <<  radRing << " " <<  nRingHits;   
      fFOutA1 << std::endl;
    
    }
  } // namespace dq 
} // name space emph       
DEFINE_ART_MODULE(emph::dq::AnalyzeTOFARICH)
