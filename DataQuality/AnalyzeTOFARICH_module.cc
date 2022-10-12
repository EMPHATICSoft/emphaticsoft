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
#include "RecoBase/SSDHit.h"
#include "RawData/TRB3RawDigit.h"
using namespace emph;

namespace emph {
  namespace dq {
  //
  // small struct to do the spill by spill analysis. Strictly internal to this module, not to be exported  
  //
    class SSDXSpillInfo {
      public:
      
        explicit SSDXSpillInfo(int StationNumber, int T0Seg);
	
	int getNumBounded(double  minX, double maxX) const ;
	double getMeanX(double  minX, double maxX) const ;
	double getRMSX(double minX, double maxX, double mean) const;
	
      private:
	int fStationNum;
	int fT0Seg;
	int fT0SegPrev;
	std::vector<short int> fRows;

      public:
	//
	// clearing, filling it 
	//
	inline void clear() {fRows.clear();}
	inline void addRow(short int r) { fRows.push_back(r); }
	//
	// Accessors 
	//
	inline int Station() const { return fStationNum; }
	inline int T0Segment() const { return fT0Seg; } 
	inline std::vector<short int>::const_iterator cbegin() const {return fRows.cbegin(); }  
	inline std::vector<short int>::const_iterator cend() const {return fRows.cend(); }  
	inline size_t size() const { return fRows.size(); } 
    };
    
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
      void analyzePreviousSpill(int spillNumber); 

   private:
//
// Some utilities.. 
//      
      bool fFilesAreOpen;
      std::string fTokenJob;    
      std::string fTOFInfoLabel, fARingInfoLabel, fSSDInfoLabel;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum, fEvtNumPrev,  fEvtNumPrevPrev;
      unsigned int fNEvents;
      int fT0SegnumPrev; // Or the previous event. Used to try to match 
      int fT0SegnumPrevPrev;
      
//
// looking for hot channels in ARICH  
//
      std::vector<unsigned int> cntByPixels;
      std::vector<int> fLowXRingLeft, fHighXRingLeft, fLowXRingRight, fHighXRingRight;
      std::vector<rb::SSDHit> fSSDHitsPrev;
      std::vector<rb::SSDHit> fSSDHitsPrevPrev;
//
//  To do the spill be spill analysis.   
//
      std::vector<SSDXSpillInfo> fStation1SpillInfo; // 
      std::vector<SSDXSpillInfo> fStation2SpillInfo; // 
      
      
      runhist::RunHistory* fRunHistory;
      emph::cmap::ChannelMap* fChannelMap; // not needed.. Indeed..Yet... 
//
//
// CSV tuple output..
// 
     
      std::ofstream fFOutA1, fFOutB1, fFOutC1, fFOutC1Prev, fFOutC1PrevPrev, fFOutC1Next, fFOutC1NextNext;
      void openOutputCsvFiles();
      
    }; 
    
    SSDXSpillInfo::SSDXSpillInfo(int Snum, int T0Seg):
       fStationNum(Snum), fT0Seg(T0Seg) { ; }
       
    int SSDXSpillInfo::getNumBounded(double xMin, double xMax) const {
      int n = 0;
      for(std::vector<short int>::const_iterator it=fRows.cbegin(); it != fRows.cend();  it++) {
        const double x= 20. - 0.06*static_cast<double>(*it); // pitch & offset hardcoded, for now... 
	if ((x <= xMin) || (x > xMax)) continue;
	n++; 
      }
      return n;
    }
    double SSDXSpillInfo::getMeanX(double xMin, double xMax) const {
      double a = 0.;
      int n = 0;
      for(std::vector<short int>::const_iterator it=fRows.cbegin();  it != fRows.cend();  it++) {
        const double x= 20. - 0.06*static_cast<double>(*it); // pitch & offset hardcoded, for now... 
	if ((x <= xMin) || (x > xMax)) continue;
	a += x;
	n++; 
      }
      if (n > 0) { a /= n; return a; }
      return DBL_MAX;
    }
    double SSDXSpillInfo::getRMSX(double xMin, double xMax, double mean) const {
      double a = 0.;
      int n = 0;
      for(std::vector<short int>::const_iterator it=fRows.cbegin(); it != fRows.cend();  it++) {
        const double x= 20. - 0.06*static_cast<double>(*it); // pitch & offset hardcoded, for now... 
	if ((x <= xMin) || (x > xMax)) continue;
	a += (x - mean) * (x- mean);
	n++; 
      }
      if (n > 1) { a /= (n - 1); return std::sqrt(a); }
      return DBL_MAX;
    }
   
// .....................................................................................
    AnalyzeTOFARICH::AnalyzeTOFARICH(fhicl::ParameterSet const& pset) : 
    EDAnalyzer(pset), 
    fFilesAreOpen(false), fTokenJob("undef"), fTOFInfoLabel("?"), fARingInfoLabel("?"), fSSDInfoLabel("?"), fChanMapFileName("?"),
     fRun(0), fSubRun(0),  fEvtNum(INT_MAX), fEvtNumPrev(INT_MAX), fEvtNumPrevPrev(INT_MAX),
     fNEvents(0), fT0SegnumPrev(INT_MAX), fT0SegnumPrevPrev(INT_MAX),
     cntByPixels(9*64, 0), fLowXRingLeft(12,0), fHighXRingLeft(12, 0), 
     fLowXRingRight(12, 0), fHighXRingRight(12, 0)
    {
       std::cerr << " Constructing AnalyzeTOFARICH " << std::endl;
       this->reconfigure(pset);
       fFilesAreOpen = false;
       //
       // by looking at the band of ARICH hits, we do see spatial correlation between the Ring hits (and the central hit), and the 
       // T0 segment which is hit. We deduce the rough Ring location, based on T0 detector, and set corresponding windows. 
       //
       for (size_t kT0=1; kT0 != fLowXRingLeft.size(); kT0++) {  
         fLowXRingLeft[kT0] = 7 - static_cast<int>(0.5*kT0); 
         fHighXRingLeft[kT0] = fLowXRingLeft[kT0] + 5; 
         fLowXRingRight[kT0] = 20 - static_cast<int>(0.5*kT0); 
         fHighXRingRight[kT0] = fLowXRingRight[kT0] + 5;
        }
        for (int k=1; k != 10; k++) { 
	   SSDXSpillInfo aSSDX1(1, k);
	   fStation1SpillInfo.push_back(aSSDX1);
	   SSDXSpillInfo aSSDX2(2, k);
	   fStation2SpillInfo.push_back(aSSDX2);
	}   
    }
    
    void AnalyzeTOFARICH::reconfigure(const fhicl::ParameterSet& pset)
    {
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fTOFInfoLabel = pset.get<std::string>("TOFInfoLabel");
      fARingInfoLabel = pset.get<std::string>("ARingInfoLabel");
      fARingInfoLabel = pset.get<std::string>("ARingInfoLabel");
      fSSDInfoLabel = pset.get<std::string>("SSDInfoLabel");
    }
    void AnalyzeTOFARICH::beginRun(art::Run const &run)
    {
     std::cerr << " AnalyzeTOFARICH::beginRun, run " << run.id() << std::endl;
      // initialize channel map
//      fChannelMap = new emph::cmap::ChannelMap();
//      fRunHistory = new runhist::RunHistory(run.run());
//      fChannelMap->LoadMap(fRunHistory->ChanFile());
// Analyze the previous spill 
//
       size_t nXSSDTot = 0;
       for (std::vector<SSDXSpillInfo>::const_iterator itS = fStation1SpillInfo.cbegin(); itS != fStation1SpillInfo.cend(); itS++) {
         nXSSDTot += itS->size();
       }
       if (nXSSDTot > 3*fStation1SpillInfo.size()) {  
          this->analyzePreviousSpill(fSubRun);
          for (std::vector<SSDXSpillInfo>::iterator itS = fStation1SpillInfo.begin(); itS != fStation1SpillInfo.end(); itS++) 
	      itS->clear();
          for (std::vector<SSDXSpillInfo>::iterator itS = fStation2SpillInfo.begin(); itS != fStation2SpillInfo.end(); itS++) 
	      itS->clear();
       }
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
      fFOutA1 << " subRun evt trigNumPart trigSum nT0Seg T0SegNum nRPC RPCStripNum nARing xRing yRing radRing nHitRing nHitRingX nHitSSD ";
      fFOutA1 << " " << std::endl;
      
      std::ostringstream fNameT0BStrStr; fNameT0BStrStr << "./TOFARICH0Tuple_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0BStr(fNameT0BStrStr.str());
      fFOutB1.open(fNameT0BStr.c_str());
      fFOutB1 << " subRun evt tT0SegNum nARh x y " << std::endl;
      std::ostringstream fNameT0CStrStr; fNameT0CStrStr << "./TOFSSD0Tuple_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0CStr(fNameT0CStrStr.str());
      fFOutC1.open(fNameT0CStr.c_str());
      fFOutC1 << " subRun evt tT0SegNum row strip angle x y z " << std::endl;
      
      std::ostringstream fNameT0CPStrStr; fNameT0CPStrStr << "./TOFSSD0TuplePrev_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0CPStr(fNameT0CPStrStr.str());
      fFOutC1Prev.open(fNameT0CPStr.c_str());
      fFOutC1Prev << " subRun evt evtPrev tT0SegNum row strip angle x y z " << std::endl; // SSD data is from the previous event,
       //  with event number fEvtNumPrev
       
      std::ostringstream fNameT0CPPStrStr; fNameT0CPPStrStr << "./TOFSSD0TuplePrevPrev_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0CPPStr(fNameT0CPPStrStr.str());
      fFOutC1PrevPrev.open(fNameT0CPPStr.c_str());
      fFOutC1PrevPrev << " subRun evt evtPrevPrev tT0SegNum row strip angle x y z " << std::endl;// SSD data is from the previous, previous event,
       //  with event number fEvtNumPrevPrev 
      
      std::ostringstream fNameT0CNStrStr; fNameT0CNStrStr << "./TOFSSD0TupleNext_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0NCStr(fNameT0CNStrStr.str());
      fFOutC1Next.open(fNameT0NCStr.c_str());      
      fFOutC1Next << " subRun evt evtT0SegNumPrev tT0SegNum tT0SegNumPrev row strip angle x y z " << std::endl; 
      // SSD data from the current event, but the T0Segment number from the previous event (usually INT_MAX, T0 acceptance ~< 50 %)
      
      std::ostringstream fNameT0CNNStrStr; fNameT0CNNStrStr << "./TOFSSD0TupleNextNext_XY_V1_" << fRun << "_" << fTokenJob << ".txt";
      std::string fNameT0CNNStr(fNameT0CNNStrStr.str());
      fFOutC1NextNext.open(fNameT0CNNStr.c_str());
      fFOutC1NextNext << " subRun evt evtT0SegNumPrevPrev tT0SegNum tT0SegNumPrevPrev row strip angle x y z " << std::endl;
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
      fFOutA1.close(); fFOutB1.close(); fFOutC1.close();
      fFOutC1Prev.close(); fFOutC1PrevPrev.close();  fFOutC1Next.close(); fFOutC1NextNext.close();
      // the last end spill analysis.. 
      size_t nXSSDTot = 0;
      for (std::vector<SSDXSpillInfo>::const_iterator itS = fStation1SpillInfo.cbegin(); itS != fStation1SpillInfo.cend(); itS++) {
         nXSSDTot += itS->size();
      }
      if (nXSSDTot > 3*fStation1SpillInfo.size())  this->analyzePreviousSpill(fSubRun);
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
    // Get the SSD hits.. 
    //
      art::Handle<std::vector<rb::SSDHit> > theSSDInfo;
      evt.getByLabel(fSSDInfoLabel, theSSDInfo);
      
    
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
      const size_t aT0SegNumIndex = static_cast<size_t>(aT0SegNum); 
      int nRingHits = 0;
      double xRing = DBL_MAX;
      double yRing = DBL_MAX;
      double radRing = DBL_MAX;
      fFOutA1  << " " << theARingInfo->size();
      int nRinHitsXBand = 0;
      
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
	       if ((aT0SegNumIndex < fLowXRingLeft.size()) && (std::abs(pxlybin - 13) < 4 )) {
	         if ((pxlxbin >= fLowXRingLeft[aT0SegNumIndex]) && (pxlxbin <= fHighXRingLeft[aT0SegNumIndex])) nRinHitsXBand++;
	         if ((pxlxbin >= fLowXRingRight[aT0SegNumIndex]) && (pxlxbin <= fHighXRingRight[aT0SegNumIndex])) nRinHitsXBand++;
	       }
               fFOutB1 << " " << fSubRun << " " << fEvtNum << " " << aT0SegNum << " " << theARingInfo->cbegin()->NIndicesI();
	       fFOutB1 << " " << pxlxbin << " " << pxlybin << std::endl;
	    }
	  }
	}
      }
      fFOutA1 << " " << xRing << " " << yRing << " " <<  radRing << " " <<  nRingHits << " " 
                     << nRinHitsXBand << " " << theSSDInfo->size() << std::endl; 
		     
      //
      if (goodEvt && (nRinHitsXBand > 2)) {
        for(std::vector<rb::SSDHit>::const_iterator it = theSSDInfo->cbegin(); it != theSSDInfo->cend(); it++) {
               fFOutC1 << " " << fSubRun << " " << fEvtNum << " " << aT0SegNum << " " 
	               << it->Row() << " " <<  it->Strip() << " " << it->Angle() << " " 
		       << it->X() << " " << it->Y() << " " << it->Z() << std::endl;
		if (std::abs(it->Angle() - 3.0*M_PI/2) < 0.01) {
		  short int aRow = static_cast<short int>(it->Row());
		  if (it->Z() < 15) {
                    for (std::vector<SSDXSpillInfo>::iterator itS = fStation1SpillInfo.begin(); itS != fStation1SpillInfo.end(); itS++) 
	   	      if (itS->T0Segment() == aT0SegNum) { itS->addRow(aRow); break; }
		  } else if (std::abs(it->Z() - 360.) < 15.) {
                    for (std::vector<SSDXSpillInfo>::iterator itS = fStation2SpillInfo.begin(); itS != fStation2SpillInfo.end(); itS++) 
	   	      if (itS->T0Segment() == aT0SegNum) { itS->addRow(aRow); break; }
		  } 
		}
	}
	if ((fEvtNumPrevPrev != INT_MAX) && (fSSDHitsPrevPrev.size() > 0)) {
          for (std::vector<rb::SSDHit>::const_iterator it=fSSDHitsPrevPrev.cbegin(); it != fSSDHitsPrevPrev.cend(); it++ ) 
               fFOutC1PrevPrev << " " << fSubRun << " " << fEvtNum << " " << fEvtNumPrevPrev << " " << aT0SegNum << " " 
	               << it->Row() << " " <<  it->Strip() << " " << it->Angle() << " " 
		       << it->X() << " " << it->Y() << " " << it->Z() << std::endl ;
          }
	if ((fEvtNumPrev != INT_MAX) && (fSSDHitsPrev.size() > 0)) {
          for (std::vector<rb::SSDHit>::const_iterator it=fSSDHitsPrev.cbegin(); it != fSSDHitsPrev.cend(); it++ ) 
               fFOutC1Prev << " " << fSubRun << " " << fEvtNum << " " << fEvtNumPrev << " " << aT0SegNum << " " 
	               << it->Row() << " " <<  it->Strip() << " " << it->Angle() << " " 
		       << it->X() << " " << it->Y() << " " << it->Z() << std::endl ;
          }
      } else { aT0SegNum = INT_MAX; } // flag it as a bad event, for T0SegNum next list. 
      
      // now check the event matching by dumping the previous SSD, and the previous T0SegNum 
      
      if (fT0SegnumPrevPrev != INT_MAX) {
         for(std::vector<rb::SSDHit>::const_iterator it = theSSDInfo->cbegin(); it != theSSDInfo->cend(); it++) {
               fFOutC1NextNext << " " << fSubRun << " " << fEvtNum << " " << fEvtNumPrevPrev 
	               << " "  << aT0SegNum << " " << fT0SegnumPrevPrev << " " 
	               << it->Row() << " " <<  it->Strip() << " " << it->Angle() << " " 
		       << it->X() << " " << it->Y() << " " << it->Z()  << std::endl;
	}
      }
      if (fT0SegnumPrev != INT_MAX) {
         for(std::vector<rb::SSDHit>::const_iterator it = theSSDInfo->cbegin(); it != theSSDInfo->cend(); it++) {
               fFOutC1Next << " " << fSubRun << " " << fEvtNum << " " << fEvtNumPrev << " " << aT0SegNum << " " << fT0SegnumPrev << " " 
	               << it->Row() << " " <<  it->Strip() << " " << it->Angle() << " " 
		       << it->X() << " " << it->Y() << " " << it->Z() << std::endl ;
	}
      }
      fEvtNumPrevPrev = fEvtNumPrev;
      fEvtNumPrev = fEvtNum;
      fT0SegnumPrevPrev = fT0SegnumPrev;
      fT0SegnumPrev = aT0SegNum;
      // Stash the SSD hits 
      fSSDHitsPrevPrev.clear();
      for (std::vector<rb::SSDHit>::const_iterator it=fSSDHitsPrev.cbegin(); it != fSSDHitsPrev.cend(); it++ ) 
         fSSDHitsPrevPrev.push_back(*it);
      fSSDHitsPrev.clear();
      for (std::vector<rb::SSDHit>::const_iterator it=theSSDInfo->cbegin(); it != theSSDInfo->cend(); it++ ) 
         fSSDHitsPrev.push_back(*it);
      
    } // end of Analyze 
    //
    // Spill by Spill analysis
    //
    void AnalyzeTOFARICH::analyzePreviousSpill (int spillNumber) {
    
      std::cerr << " Analysis of Previous spill, " << spillNumber << ", T0 Segment number vs SSD X coordinates " << std::endl;
      std::vector<double> XWindow = {30., 10., 7.5, 5., 3., 2., 1.8};
      //
      // Two the first two station.. 
      //
      std::ofstream fOutSSD1CutW;
      for (int kStation = 1; kStation !=3; kStation++) { 
        std::vector<double> xT0, meanSSDX, sigmaSSDX; 
        std::cerr << " .... for Station.. " << kStation << std::endl;
        std::ostringstream fNameAStrStr; 
        if (kStation == 1) fNameAStrStr << "./TOFARICH0SSD1vsT0Seg_";
        else fNameAStrStr << "./TOFARICH0SSD2vsT0Seg_"; 
        fNameAStrStr  << fRun << "_Spill_" << spillNumber << "_" << fTokenJob << ".txt";
        std::string fNameAStr(fNameAStrStr.str());
        fOutSSD1CutW.open(fNameAStr.c_str());
        double rNSeg5 = DBL_MAX;
        fOutSSD1CutW << " T0Seg nA meanSSD1A sigmaSSD1A nR meanSSD1R sigmaSSD1R " << std::endl;
	std::vector<SSDXSpillInfo>::const_iterator itSBegin = (kStation == 1) ? fStation1SpillInfo.cbegin() : fStation2SpillInfo.cbegin();
	std::vector<SSDXSpillInfo>::const_iterator itSEnd = (kStation == 1) ? fStation1SpillInfo.cend() : fStation2SpillInfo.cend();
        for (std::vector<SSDXSpillInfo>::const_iterator itS = itSBegin; itS != itSEnd; itS++) {
          double Xc = 0.;
	  double meanSSD1A = DBL_MAX; double sigmaSSD1A = DBL_MAX; double meanSSD1R = DBL_MAX; double sigmaSSD1R = DBL_MAX;
	  int nAll = 0; int nFinal = 0;
          for(size_t kW = 0; kW != XWindow.size(); kW++) {
	    int nn = itS->getNumBounded(Xc - XWindow[kW], Xc + XWindow[kW]);
	    if (nn < 5) continue;
	    double mX = itS->getMeanX(Xc - XWindow[kW], Xc + XWindow[kW]);
	    double sX =  itS->getRMSX(mX - XWindow[kW], mX + XWindow[kW], mX); 
	    if (kW == 0) { nAll = nn;  meanSSD1A = mX; sigmaSSD1A = sX; }
	    Xc = mX;
	    if (kW == XWindow.size()-1) {nFinal = nn;  meanSSD1R = mX; sigmaSSD1R = sX; }
          }
	  if (nFinal > 5) {
	    xT0.push_back(3.0 * (itS->T0Segment() -5));
	    meanSSDX.push_back(meanSSD1R); sigmaSSDX.push_back(sigmaSSD1R);
	    if ((nAll > 1) && (itS->T0Segment() == 5)) rNSeg5 = static_cast<double>(nFinal)/nAll; 
	  }
	  fOutSSD1CutW << " " << itS->T0Segment() << " " << nAll << " " << meanSSD1A << " " << sigmaSSD1A 
	               <<  " " << nFinal << " " << meanSSD1R << " " << sigmaSSD1R << std::endl;
        }
        fOutSSD1CutW.close();
        std::cerr << " ... Number of good segments with enough data " << xT0.size() 
	          << " Signal to noise for segment 5 " << rNSeg5 << std::endl; 
        //
        // Linear fit Could have (should have ?) use TlinearFit, but no need for such a complicated tool.  User Numerical Recipes.. 
        //
        if (xT0.size() > 3) { 
          double ss, st2, sx, sxoss, sy, a, b, sigb, chi2;
          sx = 0.; sy = 0.; st2 = 0.; ss = 0.; b = 0.;
          for (size_t i=0; i != xT0.size(); i++) {
            const double wt = 1.0/(sigmaSSDX[i] * sigmaSSDX[i]); ss += wt; sx = xT0[i]*wt; sy = meanSSDX[i]*wt;  
          }
          sxoss  = sx/ss; 
          for (size_t i=0; i != xT0.size(); i++) {
            const double t = (xT0[i] - sxoss)/sigmaSSDX[i];
	    st2 += t*t; b += t*meanSSDX[i]/sigmaSSDX[i]; 
         }
          b /= st2;
	  a = (sy - sx*b)/ss;
//	  siga = std::sqrt((1.0 + sx*sx/(ss*st2))/ss);  //That is the alignment business.. skip.. 
	  sigb = std::sqrt(1.0/st2);
	  chi2 = 0.;
          for (size_t i=0; i != xT0.size(); i++)  {
	    const double dd = (meanSSDX[i] - a - b * xT0[i])/sigmaSSDX[i];
	    chi2 += dd * dd;
	  }
	  std::cerr << " ... Linear fit slope = " << b << " +- " << sigb << " chi2 " << chi2 << std::endl;
        } // got enough data to do a linear fit 
         else {
           std::cerr << " Not enough events with narrow (+- 1.8 mm) to establish SDD station1 vs T0 signal " << std::endl;
         }
      }
   } 
  } // namespace dq 
} // name space emph       
DEFINE_ART_MODULE(emph::dq::AnalyzeTOFARICH)
