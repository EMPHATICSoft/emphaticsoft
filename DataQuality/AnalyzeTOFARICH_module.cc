////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to check the event integrity between the Trigger counter, the 
///          T0 counter and the ARICH.  That is looking for apattern in the ARICH ring 
///          that is consistent with the T0 segment number. 
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
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
//
// looking for hot channels in ARICH  
//
      std::vector<unsigned int> cntByPixels;
      
      runhist::RunHistory* fRunHistory;
      emph::cmap::ChannelMap* fChannelMap;
      
//
// CSV tuple output..
// 
     
      std::ofstream fFOutA1, fFOutB1;
      
      void openOutputCsvFiles();
      
    };    
// .....................................................................................
    AnalyzeTOFARICH::AnalyzeTOFARICH(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fTOFInfoLabel("?"), fARingInfoLabel("?"), fChanMapFileName("?"),
     fRun(0), fSubRun(0),  fEvtNum(0), fNEvents(0), cntByPixels(9*64, 0)
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
      
      std::ostringstream fNameT0BStrStr; fNameT0BStrStr << "./TOFARICH0Tuple_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0BStr(fNameT0BStrStr.str());
      fFOutB1.open(fNameT0BStr.c_str());
      fFOutB1 << " subRun evt tT0SegNum nARh x y " << std::endl;
      fFilesAreOpen = true;
    }
    void AnalyzeTOFARICH::endJob() {
      
      std::cerr << " AnalyzeTOFARICH::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      
      std::ostringstream fNameStrStr; fNameStrStr << "./TOFARICH0Tuple_CntbyPixels_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameStr(fNameStrStr.str());
      std::ofstream fOutCnt(fNameStr.c_str()); 
      fOutCnt << " index pmt pixel row column cnt " << std::endl;
      for (size_t k = 0; k != cntByPixels.size(); k++) {
        int pmt = k/64;
	int dch = k - 64*pmt;
	int row =  dch/8;
	int column = dch - row*8;
        fOutCnt << " " << k << " " << pmt << " " << dch << " " << row << " " << column << " " << cntByPixels[k] << std::endl;
      }
      fOutCnt.close();
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
//      std::cerr << " AnalyzeTOFARICH::analyze , event " << fEvtNum << " and do nothin.. " <<   std::endl; return; 
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
      bool goodEvt = theTOFInfo->TrigHit()->isSingleBeamParticle() && 
                     (theTOFInfo->NumT0SegmentHits() == 1) && (theARingInfo->size() == 1);
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
	if (nRingHits > 0) {
	  for (std::vector<unsigned short int>::const_iterator it2 = it->cbeginI(); it2 != it->cendI(); it2++) {
	    int iI = static_cast<int>(*it2);
	    if ((iI >= 0) && ( iI < static_cast<int>(cntByPixels.size()))) cntByPixels[static_cast<size_t>(iI)]++; 
            int pmt = iI/64;
	    int dch = iI - pmt*64;
	    int pmtrow =  dch/8;
	    int pmtcol = dch - pmtrow*8;
	    int pxlxbin0 = 25-pmt*9+(pmt/3)*27;
	    int pxlybin0 = (pmt/3)*9;
	    int pxlxbin = 1 + pxlxbin0-pmtcol;
	    int pxlybin = 1 + pxlybin0+pmtrow;
	    bool hotPix = (pmt == 4) && ((pmtcol >= 4) && (pmtcol < 7)) && (pmtrow < 3);  
	    if (goodEvt && (!hotPix)) {
               fFOutB1 << " " << fSubRun << " " << fEvtNum << " " << aT0SegNum << " " << theARingInfo->cbegin()->NIndicesI();
	       fFOutB1 << " " << pxlxbin << " " << pxlybin << std::endl;
	    }
	  }
	}
      }
      fFOutA1 << " " << 	xRing << " " << yRing << " " <<  radRing << " " <<  nRingHits;   
      fFOutA1 << std::endl;
    
    }
  } // namespace dq 
} // name space emph       
DEFINE_ART_MODULE(emph::dq::AnalyzeTOFARICH)
