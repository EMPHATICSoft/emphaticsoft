////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to study trigger, RPC, and T0
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

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

// EMPHATICSoft includes
#include "ChannelMap/ChannelMap.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace tof {
  
    class PeakInWaveform {
      public:
        PeakInWaveform(); 
        int peakNumber;
        size_t startBin;
        size_t peakBin;
        size_t endBin;
        double peakVal;
        double sumSig;
        double prevSumSig;
    };
  
    class T0toRPC : public art::EDAnalyzer {
    public:
      explicit T0toRPC(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~T0toRPC();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const& run);
      void endJob();

    private:
    
      void  openOutputCsvFiles(); // At this point, we know the run number..
      void  resetAllADCsTDCs();
      
      void   FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > &,
			 art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillTrigT0RPCV1(bool gotT0, bool gotRPC); 
       // an analysis routine, first version, joint analysis of the content of the Trigger, T0 and RPC buffers. 

      bool fFilesAreOpen;
      std::string fTokenJob;
      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubRun;
      int fPrevSubRun;
      unsigned int fEvtNum;
      int fPrevEvtNum;
      unsigned int fNEvents;
      unsigned int fNoT0Info;
      unsigned int fNoTrigInfo;
      unsigned int fNoRPCInfo;
      unsigned int fNoTDCInfo;
      unsigned int fNoT0RPCInfo;
      

      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanT0  = 20;
      static const unsigned int nChanRPC = 16;
      static const unsigned int nChanTrig = 4;
      
//
// Some data structure being filled in specific fill routines;
//  There content is event specific. To be used in the summary method, where we establish  some coincidences.  
//
     std::vector<double> fT0ADCs;
     std::vector<double> fT0TDCs; // parallel vectors
//     std::vector<double> fRPCADCs(nChanRPC, 0.);
     std::vector<double> fRPCTDCs;
     std::vector<double> fTrigPeakADCs;
     std::vector<double> fTrigADCs;

      bool fMakeT0FullNtuple; 
      bool fMakeRPCFullNtuple;
      bool fMakeTrigFullNtuple;
      bool fMakeEventSummaryNTuple;
     

     
      // define streamers for csv files. 
      std::ofstream fFOutT0;
      std::ofstream fFOutRPC; 
      std::ofstream fFOutTrigger; 
      std::ofstream fFOutTrigT0RPC; 

    };
    //
    // Implementation.. 
    //
    
    PeakInWaveform::PeakInWaveform() :
      peakNumber(-1),
      startBin(4096),
      peakBin(4096),
      endBin(4096),
      peakVal(0.),
      sumSig(0.),
      prevSumSig(0.)
      { ; }
    
    //.......................................................................
    T0toRPC::T0toRPC(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
      fFilesAreOpen(false),
      fTokenJob("none"),
      fChanMapFileName("Unknwon"), fRun(0), fSubRun(0), fPrevSubRun(-1), fEvtNum(0), fPrevEvtNum(-1), 
      fT0ADCs(nChanT0+1, 0.),
      fT0TDCs(nChanT0+1, DBL_MAX),
      fRPCTDCs(nChanRPC, DBL_MAX),
      fTrigPeakADCs(nChanTrig, 0.),
      fTrigADCs(nChanTrig, 0.),
      fMakeT0FullNtuple(true),
      fMakeRPCFullNtuple(true),
      fMakeTrigFullNtuple(true),
      fMakeEventSummaryNTuple(false)
    {

      this->reconfigure(pset);

    }
    
    //......................................................................
    T0toRPC::~T0toRPC()
    {
    
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutRPC.is_open()) fFOutRPC.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0RPC.is_open()) fFOutTrigT0RPC.close();
      
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void T0toRPC::reconfigure(const fhicl::ParameterSet& pset)
    {
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fChanMapFileName = pset.get<std::string>("channelMapFileName","");
      fMakeT0FullNtuple  = pset.get<bool>("makeT0FullNtuple",true); // keep them for now.. 
      fMakeRPCFullNtuple = pset.get<bool>("makeRPCFullNtuple",true);
      fMakeTrigFullNtuple = pset.get<bool>("makeTrigFullNtuple",true);
      fMakeEventSummaryNTuple = pset.get<bool>("makeEventSummaryFullNtuple",false);
      
    }

    //......................................................................
    void T0toRPC::beginJob()
    {
      fNEvents= 0;
      fNoT0Info = 0; fNoTrigInfo = 0; fNoT0RPCInfo = 0; fNoTrigInfo = 0; 
    
      //
      // open a few csv file for output. Delayed until we know the run number.  
      //
            
    }
    //......................................................................
    void T0toRPC::beginRun(art::Run const& run)
    {
      // initialize channel map
      fChannelMap = new emph::cmap::ChannelMap();
      fChannelMap->LoadMap(run.run());
    }

    //......................................................................
    void T0toRPC::openOutputCsvFiles() {
    
      if (fRun == 0) {
        std::cerr << " T0toRPC::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	exit(2);
      }
      if (fMakeT0FullNtuple) { 
        std::ostringstream fNameT0StrStr; fNameT0StrStr << "./T0Tuple_V1_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameT0Str(fNameT0StrStr.str());
        fFOutT0.open(fNameT0Str.c_str());
	fFOutT0 << " subRun evt ";
        for (unsigned int k=0; k != nChanT0; k++) fFOutT0 << " adc" << k << " tdc" << k; 
        fFOutT0 << " adc500 tdc500 " << std::endl;
      }
      if (fMakeTrigFullNtuple) { 
        std::ostringstream fNameTrigStrStr; fNameTrigStrStr << "./TrigTuple_V1_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameTrigStr(fNameTrigStrStr.str());
        fFOutTrigger.open(fNameTrigStr.c_str());
	fFOutTrigger << " subRun evt ";
        for (unsigned int k=0; k != nChanTrig; k++) 
	  fFOutTrigger << " Padc" << k << " Sadc" << k << " Mean" << k << " Width" << k << " Skew" << k 
	               << " numPeak" << k << " deltaT" << k << " peakSig1" << k  << " peakSig2" << k;
        fFOutTrigger << " " << std::endl;
      }
      if (fMakeRPCFullNtuple) { 
        size_t nchanRPC = static_cast<size_t> (emph::geo::DetInfo::NChannel(emph::geo::RPC));
        std::ostringstream fNameRPCStrStr; fNameRPCStrStr << "./RPCTuple_V1_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameRPCStr(fNameRPCStrStr.str());
        fFOutRPC.open(fNameRPCStr.c_str());
	fFOutRPC << " subRun evt ";
        for (unsigned int k=0; k != nchanRPC; k++) fFOutRPC << " tdc" << k;
        fFOutRPC << " " << std::endl;
      }
      if (fMakeEventSummaryNTuple) {
        size_t nchanRPC = static_cast<size_t> (emph::geo::DetInfo::NChannel(emph::geo::RPC));
        std::ostringstream fNameSumStrStr; fNameSumStrStr << "./SummaryTrigT0RPC_V1_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameSumStr(fNameSumStrStr.str());
        fFOutTrigT0RPC.open(fNameSumStr.c_str());
	std::cerr << " Opening fFOutTrigT0RPC... " << std::endl;
	fFOutTrigT0RPC << " spill dSpill evt dEvt T0OK RPCOK ";
	fFOutTrigT0RPC << " trigPAdc0 trigSAdc0  trigPAdc1 trigSAdc1 trigPAdc2 trigSAdc2  trigPAdc3 trigSAdc3";
	fFOutTrigT0RPC << " numCT0HighTOk numCT0High numCT0Low ";
	for (size_t k=1; k != 19; k++) fFOutTrigT0RPC << " T0tdc" << k << " T0adc" << k;
	for (size_t k=0; k != nchanRPC; k++) fFOutTrigT0RPC << " RPCtdc" << k;
	fFOutTrigT0RPC << " " << std::endl;
      }
      fFilesAreOpen = true;
   }
    
    //......................................................................

    void T0toRPC::endJob()
    {
      std::cerr << " T0toRPC::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Total number of events " << fNEvents << " No Info T0 and No RPC " << fNoT0RPCInfo 
                << "  No trigger Info " << fNoTrigInfo << " No T0 info " << fNoT0Info << " No RPC info " << fNoRPCInfo << std::endl; 
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutRPC.is_open()) fFOutRPC.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0RPC.is_open()) fFOutTrigT0RPC.close();
    }

    //......................................................................
    
    void T0toRPC::resetAllADCsTDCs() {
       for(std::vector<double>::iterator it = fT0ADCs.begin(); it != fT0ADCs.end(); it++) *it = -1.0*DBL_MAX/2.;
       for(std::vector<double>::iterator it = fT0TDCs.begin(); it != fT0TDCs.end(); it++) *it = DBL_MAX;
       for(std::vector<double>::iterator it = fRPCTDCs.begin(); it != fRPCTDCs.end(); it++) *it = DBL_MAX;
       for(std::vector<double>::iterator it = fTrigADCs.begin();it != fTrigADCs.end(); it++) *it = -1.0*DBL_MAX/2.;
       for(std::vector<double>::iterator it = fTrigPeakADCs.begin();it != fTrigPeakADCs.end(); it++) *it = 0.;
    }

    void T0toRPC::FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH, art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
       bool debugIsOn = (((fEvtNum == 47) && (fSubRun == 11)) || ((fEvtNum == 411) && (fSubRun == 13)));
       if (debugIsOn) std::cerr << " T0toRPC::FillT0Plots, spill " << fSubRun << " " << fEvtNum << std::endl;
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::T0);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      double trb3LinearLowEnd = 15.0;
      double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0
      echan.SetBoardType(boardType);
      std::vector<int> vT0ADChits(nchan,0);	// keep... old code from plotMon..     
      std::vector<int> vT0TDChits(nchan,0);	  
      if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    fT0ADCs[detchan] = -1.0e10;
	    vT0ADChits[detchan]=0; 
	    if (detchan < nchan) {
	      float bl = wvfm.Baseline(0, 12);
	      float adcPeakNeg = bl -  static_cast<float>(wvfm.PeakADC(true)); 
	      float adcPeakPos = static_cast<float>(wvfm.PeakADC(false)) - bl;
	      int adcPeakNegBin = wvfm.PeakTDC(true); int adcPeakPosBin = wvfm.PeakTDC(false);
	      int deltaPeakBin = adcPeakNegBin - adcPeakPosBin;	  // look for differential signals.     
	      float blw = wvfm.BLWidth();
	      float effadc = adcPeakNeg + adcPeakPos;
	      if (debugIsOn) {
	        std::cerr << ".. detchan  " << detchan << " bl " << bl << " bwl " << blw 
		          << " PeakNeg, Ampl/Bin " << adcPeakNeg << "/" << adcPeakNegBin << " PeakPos " 
			  << adcPeakPos << "/" << adcPeakPosBin << " deltaPeakBin " << deltaPeakBin << std::endl;
	      }
	      if ((effadc > 3.0*blw) && 
	          (adcPeakPosBin >= 14) && (deltaPeakBin > 0) && (deltaPeakBin < 5)) effadc += 10000;
	      if ((fNEvents%1000 == 5) && ((idx == 5) || (idx == 4))) {
	        std::string polarStr("none");
		if (effadc > 10000.) polarStr = std::string("Bipolar");
		if ((adcPeakNeg > 5.0*blw) && (adcPeakPos < 2.0*blw)) polarStr = std::string("UnipolarNeg"); 
		if ((adcPeakPos > 5.0*blw) && (adcPeakNeg < 2.0*blw)) polarStr = std::string("UnipolarPos"); 
	        std::ostringstream fWvOutStrStr; 
		fWvOutStrStr << "./T0WaveForms/T0ADC" << polarStr << "_" << idx << "_Run_" << fRun << "_Spill" << fSubRun << "_evt_" << fEvtNum << ".txt";
		std::string fWvOutStr( fWvOutStrStr.str());
		std::ofstream fWvOut(fWvOutStr.c_str()); fWvOut << " k adc " << std::endl;
		std::vector<uint16_t> tmpwf = wvfm.AllADC();
		for (size_t k=0; k != tmpwf.size(); k++) fWvOut << " " << k << " " << tmpwf[k] << std::endl;
		fWvOut.close();
	      } 
	      fT0ADCs[detchan] = effadc;
	      vT0ADChits[detchan]=1; 
	    } else {
	      std::cerr << " T0toRPC::FillT0Plots , Unexpected Channel in ADC array detchan " 
	                << detchan << " nchan " << nchan << " expected nchan " << fT0ADCs.size()-1 <<  std::endl;
              size_t badDetChan =  fT0ADCs.size()-1;
	      fT0ADCs[badDetChan] = detchan;
	    }
	  }
      }
      
      if (! trb3H->empty()) {
	  //std::cout<<"New Event!"<<std::endl;
	  int i = 0;
	  i++;
//	  std::vector<int> hitCount;
//	  hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::T0));
	  boardType = emph::cmap::TRB3;
	  echan.SetBoardType(boardType);
	  const rawdata::TRB3RawDigit& trb3Trigger = (*trb3H)[0];
	  long double triggerTime = trb3Trigger.GetEpochCounter()*10240026.0 + trb3Trigger.GetCoarseTime() * 
	                              5000.0 - ((trb3Trigger.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	  for (size_t idx=0; idx < trb3H->size(); ++idx) {
	    const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];	  
	    int chan = trb3.GetChannel();
	    int board = trb3.GetBoardId();
	    echan.SetBoard(board);	
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    long double time_T0 = trb3.GetEpochCounter()*10240026.0 + trb3.GetCoarseTime() *
	                               5000.0 - ((trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	    //std::cout<<"detchan value: "<<detchan<<std::endl;
	    if (detchan < nchan) { // watch out for channel 500!
//	      hitCount[detchan] += 1;
	      fT0TDCs[detchan] = ((triggerTime-time_T0)/100000);
	    }  else {
	    // This occurs at every events... 
//                 std::cerr << " T0toRPC::FillT0Plots , Unexpected Channel in TDC array detchan " 
//	                << detchan << " nchan " << nchan << " expected nchan " << fT0ADCs.size()-1 <<  std::endl;
                 size_t badDetChan =  fT0TDCs.size()-1;
	         fT0TDCs[badDetChan] = detchan;
			
	    }
         }
      }
      if (fMakeT0FullNtuple) {
        fFOutT0 << " " << fSubRun << " " << fEvtNum;
        for (int k=0; k != nchan+1; k++) fFOutT0 << " " << fT0ADCs[k] << " " << fT0TDCs[k];
        fFOutT0 << std::endl;
      }
    }  
        
    //......................................................................
    void    T0toRPC::FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::RPC);
//      std::cerr << " T0toRPC::FillRPCPlots, number of channels " << nchan << std::endl;
      emph::cmap::EChannel echan;
      emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
      double trb3LinearLowEnd = 15.0;
      double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0
      //double trb3LinearHighEnd_RPC = 476.0; // For FPGA3? -- RPC
      if (! trb3H->empty()) {
//	  std::vector<int> hitCount;
//          hitCount.resize(emph::geo::DetInfo::NChannel(emph::geo::RPC));
          echan.SetBoardType(boardType);
	  //The First hit for every event was in channel 500 (trigger)
	  const rawdata::TRB3RawDigit& trb3Trigger = (*trb3H)[0];
	  long double triggerTime = trb3Trigger.GetEpochCounter()*10240026.0 + trb3Trigger.GetCoarseTime() * 5000.0 - 
	             ((trb3Trigger.GetFineTime() -   trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
          for (size_t idx=0; idx < trb3H->size(); ++idx) {
            const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];
            int chan = trb3.GetChannel();
            int board = trb3.GetBoardId();
            echan.SetBoard(board);
            echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
//	    std::cerr  <<"Found TRB3 hit: IsLeading: "<<trb3.IsLeading()<<"; IsTrailing: "<<
//	     trb3.IsTrailing()<<"; Fine Time: " <<trb3.GetFineTime()<<"; Course Time: "<<trb3.GetCoarseTime()<<"; Epoch Counter: //"<<trb3.GetEpochCounter()<<std::endl;
	    long double time_RPC = trb3.GetEpochCounter()*10240026.0 + trb3.GetCoarseTime() * 5000.0 - 
	                         ((trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0; 
	    if (detchan < nchan) { // watch out for channel 500!                                                                  
//              hitCount[detchan] += 1;
	      fRPCTDCs[detchan] = ((time_RPC - triggerTime)/100000);
            }
         }
      }
      if (fMakeRPCFullNtuple) {
        fFOutRPC << " " << fSubRun << " " << fEvtNum;
        for (int k=0; k != nchan; k++) fFOutRPC << " " << fRPCTDCs[k];
        fFOutRPC << std::endl;
      }
    }
    //.....................................}.................................

    void   T0toRPC::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      if (wvfmH->empty()) return;
//      const bool debugIsOn = (((fEvtNum < 5) && (fSubRun < 5)) || (((fRun == 511) && ((fSubRun == 10) ) && ( 
//     	(fEvtNum == 92) || (fEvtNum == 158) || (fEvtNum == 585) || 
//        (fEvtNum == 601) || (fEvtNum == 762) || (fEvtNum == 875))) || ((fSubRun == 7) && (fEvtNum > 1874)));
//      const bool debugIsOn = ((fRun == 511) && (fSubRun == 7) && (fEvtNum > 1874 ) && (fEvtNum < 1880 )); 
//      const bool debugIsOn = ((fRun == 511) && (fEvtNum > 250000 )); 
      const bool debugIsOn = ((((fRun == 511) && (fSubRun == 10) && ( 
     	(fEvtNum == 69) || (fEvtNum == 387) || (fEvtNum == 569) || 
        (fEvtNum == 954) || (fEvtNum == 1414) || (fEvtNum == 1455)))));
     if (debugIsOn) std::cerr << " Starting T0toRPC::FillTrigPlots, spill " << fSubRun << " evt " << fEvtNum << std::endl;
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      std::vector<double> meanT(fTrigADCs);
      std::vector<double> widthT(fTrigADCs);
      std::vector<double> skewT(fTrigADCs);
      std::vector<int> numPeaks(fTrigADCs.size(), 0);
      std::vector<double> deltaT(fTrigADCs);
      std::vector<double> peakSignalP1(fTrigADCs);
      std::vector<double> peakSignalP2(fTrigADCs);
      std::vector<PeakInWaveform> peaks;
      for (size_t idx=0; idx < wvfmH->size(); ++idx) {
          peaks.clear();
          const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
          int chan = wvfm.Channel();
          int board = wvfm.Board();
          echan.SetBoard(board);
          echan.SetChannel(chan);
          emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
          int detchan = dchan.Channel();
          if (detchan >= 0 && (detchan < nchan)) {
            std::vector<uint16_t> tmpwf = wvfm.AllADC();
	    if (tmpwf.size() < 100) {
	      std::cerr << " Anomalous waveform, size " << tmpwf.size() << "  spill " << fSubRun << " evt " << fEvtNum << " fatal.. " << std::endl; exit(2);
	    }
            // Dump of some wave forms
//            if (debugIsOn && (fEvtNum > 500000)) {
            if (debugIsOn) {
              std::cerr << " ...  At channel " << detchan << std::endl;
              std::ostringstream WvOutStrStr; 
              if (fSubRun == 10) WvOutStrStr << "./TriggerWaveForms/TrigADCMp";
              else WvOutStrStr << "./TriggerWaveForms/TrigADC";
              WvOutStrStr << detchan << "_Spill" << fSubRun << "_evt" << fEvtNum << ".txt";
              std::string WvOutStr( WvOutStrStr.str());
              std::ofstream WvOut(WvOutStr.c_str()); WvOut << " k adc " << std::endl;
              for (size_t k=0; k != tmpwf.size(); k++) WvOut << " " << k << " " << tmpwf[k] << std::endl;
              WvOut.close();
            } 
            float bl = wvfm.Baseline(0, 10);
            float adc = bl - wvfm.PeakADC();
            float sumadc = 40.0*bl - wvfm.IntegratedADC(10, 40);
            float blw = wvfm.BLWidth();
            if (adc > 5*blw) {
              fTrigADCs[detchan] = sumadc;
              fTrigPeakADCs[detchan] = adc;
            }
            if (fMakeTrigFullNtuple) {
              std::vector<float> tmpwfSig; for (size_t k=11; k != tmpwf.size(); k++) tmpwfSig.push_back(bl - static_cast<float>(tmpwf[k]));
              double sumSig = 0.; meanT[detchan] = 0; widthT[detchan] = 0; skewT[detchan] = 0;
              for (size_t k=0; k != tmpwfSig.size(); k++) { meanT[detchan] += tmpwfSig[k] * k; sumSig += tmpwfSig[k]; } 
              meanT[detchan] /= sumSig;
              for (size_t k=0; k != tmpwfSig.size(); k++) {
        	const double dk = static_cast<double>(k) - meanT[detchan]; 
        	widthT[detchan] += dk * dk * tmpwfSig[k];
        	skewT[detchan] += dk * dk * dk * tmpwfSig[k];
              }
              widthT[detchan] /= sumSig;
              skewT[detchan] /= sumSig;
//
// Search for multiple peaks. 
//
               PeakInWaveform peak1; peak1.peakNumber=1;
               PeakInWaveform peak2; peak2.peakNumber=2;
               PeakInWaveform peak3; peak3.peakNumber=3;
               bool peak1IsOn = false;
               bool peak2IsOn = false;
               for (size_t kP1=0; kP1 != tmpwfSig.size()-3; kP1++) {
        	 size_t kP1N = kP1 + 1;
        	 if ((!peak1IsOn) && ((tmpwfSig[kP1N] - tmpwfSig[kP1]) > 5.0*blw)) {
        	   peak1IsOn = true; 
        	   peak1.startBin = kP1;
        	    if (debugIsOn) std::cerr << " ... ... Starting first peak at bin " << peak1.startBin << " Next value " << tmpwfSig[kP1N] << std::endl;
        	 }
        	 if (peak1IsOn) {
        	   peak1.sumSig += tmpwfSig[kP1];
        	   const double deltaV1 = (tmpwfSig[kP1N] - tmpwfSig[kP1]);
        	   if (deltaV1 > 0.) { peak1.peakBin= kP1N; peak1.peakVal = tmpwfSig[kP1N]; }
        	   if ((kP1 > peak1.startBin+4) && (deltaV1 > 0.) && 
        	       (peak2.startBin == 4096) && (kP1 < (tmpwfSig.size()-4))) {  // take ony the first peak. 
        	     // search for a second peak within the first peak, after the first peak value.  
        	     peak2IsOn = false;
        	     for (size_t kP2=kP1N; kP2 != tmpwfSig.size()-3; kP2++) {
        		size_t kP2N = kP2 + 1;
        		if((!peak2IsOn) && ((tmpwfSig[kP2N] - tmpwfSig[kP2]) > 5.0*blw) && (tmpwfSig[kP2N] > 0.25*peak1.peakVal)) { 
        		  peak2IsOn = true;
        		  peak2.startBin = kP2;
        		  if (debugIsOn) std::cerr << " ... ... ... Starting 2nd peak at bin " << peak2.startBin <<  " within first  " << std::endl;
        		}
        		if (peak2IsOn) {
        		  peak2.sumSig += tmpwfSig[kP2];
        		  if ((tmpwfSig[kP2N] - tmpwfSig[kP2]) > 0.) { peak2.peakBin= kP2N; peak2.peakVal = tmpwfSig[kP2N]; }
        		  if ((kP2 > peak2.startBin+2) && (std::abs(peak2.sumSig - peak2.prevSumSig) < 0.01*peak2.sumSig)) {
        		    peak2IsOn = false;
        		    peak2.endBin = kP2N;
        		    if (debugIsOn) std::cerr << " ... ... ... End 2nd peak at bin " << peak2.endBin <<  " within first  " << std::endl;
        		    break;
        		  }
        		  peak2.prevSumSig = peak2.sumSig;
        		}
        	     }// integrating the 2nd peak within the first peak. 
        	   } // condition for starting 2nd peak within first peak
        	   if ((kP1 > peak1.startBin+2) && (std::abs(peak1.sumSig - peak1.prevSumSig) < 0.01*peak1.sumSig)) {
        		    peak1IsOn = false;
        		    peak1.endBin = kP1N;
        		    if (debugIsOn) std::cerr << " ... ... End first peak at bin " << peak1.endBin << std::endl;
        	     }
        	     peak1.prevSumSig = peak1.sumSig;
        	     if (!peak1IsOn) break;
        	    } // integrating the first peak.	 
        	 } // Loop for the first peak the first peak. 
        	 // search for the 2nd peak, after the first peak. if we did find the first peak.. 
        	 bool peak3IsOn = false;
        	 if ((peak1.startBin < 4096) && (peak1.startBin < tmpwfSig.size()-4) && (peak1.endBin < tmpwfSig.size()-3)) { // must have a complete first peak. 
        	     for (size_t kP3=peak1.endBin+1; kP3 != tmpwfSig.size()-2; kP3++) {
        		size_t kP3N = kP3 + 1;
        		if((!peak3IsOn) && ((tmpwfSig[kP3N] - tmpwfSig[kP3]) > 5.0*blw) && (tmpwfSig[kP3N] > 0.25*peak1.peakVal)) { 
        		  peak3IsOn = true;
        		  peak3.startBin = kP3;
        		  if (debugIsOn) std::cerr << " ... ... Starting 2nd peak at bin " << peak3.startBin <<  " after the  first  " << std::endl;
        		}
        		if (peak3IsOn) {
        		  peak3.sumSig += tmpwfSig[kP3];
        		  if ((tmpwfSig[kP3N] - tmpwfSig[kP3]) > 0.) { peak3.peakBin= kP3N; peak3.peakVal = tmpwfSig[kP3N]; }
        		  if ((kP3 > peak3.startBin+2) && (std::abs(peak3.sumSig - peak3.prevSumSig) < 0.01*peak3.sumSig)) {
        		    peak3IsOn = false;
        		    peak3.endBin = kP3N;
        		    if (debugIsOn) std::cerr << " ... ... End 2nd peak at bin " << peak3.endBin <<  " after first peak " << std::endl;
        		    break;
        		  }
        		}
        		peak3.prevSumSig = peak3.sumSig;
        	     }// integrating the 2nd peak after the first peak. 
        	 } // conditional search for a 2nd peak after the first one. 
        	 if (peak1.startBin < 4096) peaks.push_back(peak1);
        	 if (peak2.startBin < 4096) peaks.push_back(peak2);  
        	 if (peak3.startBin < 4096) peaks.push_back(peak3);
        	 numPeaks[detchan] = static_cast<int>(peaks.size());
        	 if (peaks.size() > 0)  {
        	   peakSignalP1[detchan] = peaks[0].peakVal;
        	   if (peaks.size() > 1)  {
        	      deltaT[detchan] = 4.0 * static_cast<int>(peaks[1].startBin - peaks[0].startBin);
        	      peakSignalP2[detchan] = peaks[1].peakVal;
        	   }
        	 }
               if (debugIsOn) {
        	 std::cerr << " Dump of the peaks found, numPeaks  " << peaks.size() << std::endl;
        	 for (std::vector<PeakInWaveform>::const_iterator it=peaks.cbegin(); it != peaks.cend(); it++) { 
        	   std::cerr <<  " number " << it->peakNumber << " start " << 10 + it->startBin << " end " 
        		     << 10 + it->endBin << " peaking at " << 10 + it->peakBin << " sum " << it->sumSig << std::endl; 
        	 } 
               }	     
           } // do this multi-peak search. 
        } // this detchan is OK  
      } // loop on channels 
      if (fMakeTrigFullNtuple) {

            fFOutTrigger << " " << fSubRun << " " << fEvtNum;
            for (size_t k=0; k != fTrigPeakADCs.size(); k++) {
               fFOutTrigger << " " << fTrigPeakADCs[k] << " " << fTrigADCs[k];
               fFOutTrigger << " " << meanT[k] << " " << widthT[k] << " " << skewT[k] << " " 
        		  << numPeaks[k] << " " << deltaT[k] << " " << peakSignalP1[k] << " " <<  peakSignalP2[k];
            }
            fFOutTrigger << std::endl;
      }
//      if (debugIsOn) { std::cerr << " And quit after first Trigger dump complete analysis " << std::endl; exit(2); }
    }
    void T0toRPC::FillTrigT0RPCV1(bool gotT0, bool gotRPC) {  
        
      int nchanTrig = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      size_t nchanRPC = static_cast<size_t> (emph::geo::DetInfo::NChannel(emph::geo::RPC));
      int nBlank = 3 + 2*18 + nchanRPC; 
// 	fFOutTrigT0RPC << " spill dSpill evt dEvt T0OK RPCOK trigAdc0 trigAdc1  trigAdc2 trigAdc3 numcT0HighTok numcT0High numcT0Low "; 
// For now.. 
       fFOutTrigT0RPC << " " << fSubRun << " " << fSubRun - fPrevSubRun << " " << fEvtNum << " " << fEvtNum - fPrevEvtNum;
       if (gotT0) { fFOutTrigT0RPC << " 1" ; } else { fFOutTrigT0RPC << " 0" ; } 
       if (gotRPC) { fFOutTrigT0RPC << " 1" ; } else { fFOutTrigT0RPC << " 0" ; }
       for (int k=0; k != nchanTrig; k++) fFOutTrigT0RPC << " " << fTrigPeakADCs[k] << " " << fTrigADCs[k] ;
       if ((!gotT0) && (!gotRPC)) { for (int k=0; k != nBlank; k++) fFOutTrigT0RPC << " 0" ; } 
       else if ((!gotT0) && (gotRPC)) { for (int k=0; k != nBlank; k++) fFOutTrigT0RPC << " 0" ; } // we got some RPC, to be analyzed later.. 
       else if (gotT0) {
       // 
       // T0 analysis.
       // Channel 0 is weirs, has NaN... 
       //                                     0       1      2    3      4    5     6     7      8     9     10    11
       std::vector<double> threshT0ADC{DBL_MAX, 200., 200., 200., 200., 200., 250., 250., 200., 100., 200.,  250., 
	                                    250., 300., 300., 300., 325., 300., 325., DBL_MAX};
        int numC0High = 0; int numC0HighTOk = 0;
	int numC0Low = 0;
	for (size_t k=1; k!=19; k++) {
	  if ((fT0ADCs[k] > threshT0ADC[k]) && (std::abs(fT0TDCs[k] - 4.) < 8.)) numC0HighTOk++;
	  if (fT0ADCs[k] < threshT0ADC[k]) numC0Low++;
	  if (fT0ADCs[k] > threshT0ADC[k]) numC0High++;
	}  				   
        fFOutTrigT0RPC << " " << numC0HighTOk << " " << numC0High << " " << numC0Low;
	for (size_t k=1; k != 19; k++) fFOutTrigT0RPC << " " << fT0TDCs[k] << " " << fT0ADCs[k]; 
	for (size_t k=0; k != nchanRPC; k++) fFOutTrigT0RPC << " " << fRPCTDCs[k]; 
	fFOutTrigT0RPC << std::endl;
//        std::cerr << " FillTrigT0RPCV1 ... evtNum " << fEvtNum << " spill " << fSubRun << std::endl;
       }
    }
    //......................................................................
    void T0toRPC::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      this->resetAllADCsTDCs();
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();  
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      art::Timestamp aTime = evt.time();
      std::string aTimeStr = art::to_iso_string_assuming_unix_epoch(aTime);
      
      if (fEvtNum < 3) std::cerr << " Event " << fEvtNum << " time " << aTimeStr << " ....? " <<  std::endl;
      
      bool gotT0 = false; bool gotRPC = false;    
      std::string labelStr;
      

      for (int i=0; i<emph::geo::NDetectors; ++i) {

	labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
//	std::cerr << " At label " << labelStr << std::endl; 
	art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	try {
	  evt.getByLabel(labelStr, wfHandle);

	  if (!wfHandle->empty()) {
//	    if (i == emph::geo::Trigger) FillTrigPlots(wfHandle); // We will do this only for the T0 & RPC data.. 
	    if (i == emph::geo::T0) {
//	      int j = emph::geo::NDetectors;
	      labelStr = "raw:T0";
	      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle;
	      try {
		evt.getByLabel(labelStr, trbHandle);
		if (!trbHandle->empty()) {
		  gotT0 = true;
		  FillT0Plots(wfHandle, trbHandle);
		}
		else {
//		  std::cerr << "**No TRB3 digits found for the T0!, try " << std::endl;
//                  fNoT0Info[0]++;  // There seem to be nocase where the acces to trbHandle succeed, but it is empty.. 
		}
	      }
	      catch(...) {
//		std::cerr << "No TRB3 digits found for the T0!, catch " << std::endl;
                  fNoT0Info++;
	      }
	    }

	  }
	}
	catch(...) {
	  //	  std::cout << "Nothing found in " << labelStr << std::endl; 
	}
      }
      // get RPC TRB3digits
      int i = emph::geo::RPC;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle;
      try {
	evt.getByLabel(labelStr, trbHandle);
	if (!trbHandle->empty()) {
	  gotRPC = true;
	  FillRPCPlots(trbHandle);
	}
      }
      catch(...) {
        fNoRPCInfo++;
      }
      if ((!gotRPC) && (!gotT0)) fNoT0RPCInfo++;
      fPrevEvtNum = static_cast<int>(fEvtNum);
      fPrevSubRun = static_cast<int>(fSubRun);
      if (gotRPC && gotT0) { 
        for (int i=0; i<emph::geo::NDetectors; ++i) {

	  labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
//	std::cerr << " At label " << labelStr << std::endl; 
	  art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	  try {
	    evt.getByLabel(labelStr, wfHandle);

	    if (!wfHandle->empty()) {
	      if (i == emph::geo::Trigger) FillTrigPlots(wfHandle);
	      break;
            } 
          } catch(...) { return; } 
        } 
        this->FillTrigT0RPCV1(gotT0, gotRPC);
      }
    }   
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::tof::T0toRPC)
