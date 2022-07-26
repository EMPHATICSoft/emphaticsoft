////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create online monitoring plots
/// \author  $Author: jpaley $
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
#include "OnlineMonitoring/plotter/HistoSet.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/Settings.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace onmon {
    class OnMonT0toRPC : public art::EDAnalyzer {
    public:
      explicit OnMonT0toRPC(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~OnMonT0toRPC();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
    
      void  openOutputCsvFiles(); // At this point, we know the run number..
      
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

    //.......................................................................
    OnMonT0toRPC::OnMonT0toRPC(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
      fFilesAreOpen(false),
      fTokenJob("none"),
      fChanMapFileName("Unknwon"), fRun(0), fSubRun(0), fPrevSubRun(-1), fEvtNum(0), fPrevEvtNum(-1), 
      fT0ADCs(nChanT0+1, 0.),
      fT0TDCs(nChanT0+1, DBL_MAX),
      fRPCTDCs(nChanRPC, DBL_MAX),
      fTrigADCs(nChanTrig, 0.),
      fMakeT0FullNtuple(true),
      fMakeRPCFullNtuple(true),
      fMakeTrigFullNtuple(true),
      fMakeEventSummaryNTuple(false)
    {

      this->reconfigure(pset);

    }
    
    //......................................................................
    OnMonT0toRPC::~OnMonT0toRPC()
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
    void OnMonT0toRPC::reconfigure(const fhicl::ParameterSet& pset)
    {
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fChanMapFileName = pset.get<std::string>("channelMapFileName","");
      fMakeT0FullNtuple  = pset.get<bool>("makeT0FullNtuple",true); // keep them for now.. 
      fMakeRPCFullNtuple = pset.get<bool>("makeRPCFullNtuple",true);
      fMakeTrigFullNtuple = pset.get<bool>("makeTrigFullNtuple",true);
      fMakeEventSummaryNTuple = pset.get<bool>("makeEventSummaryFullNtuple",false);
      
      Settings::Instance().fDet = kEMPH;
    }

    //......................................................................
    void OnMonT0toRPC::beginJob()
    {
      fNEvents= 0;
      fNoT0Info = 0; fNoTrigInfo = 0; fNoT0RPCInfo = 0; fNoTrigInfo = 0; 
      // initialize channel map
      fChannelMap = 0;
      if (!fChanMapFileName.empty()) {
	fChannelMap = new emph::cmap::ChannelMap();
	if (!fChannelMap->LoadMap(fChanMapFileName)) {
	  std::cerr << "Failed to load channel map from file " << fChanMapFileName << std::endl;
	  delete fChannelMap;
	  fChannelMap = 0;
	}
	std::cout << "Loaded channel map from file " << fChanMapFileName << std::endl;
      }
    
      //
      // open a few csv file for output. Delayed until we know the run number.  
      //
            
    }
    void OnMonT0toRPC::openOutputCsvFiles() {
    
      if (fRun == 0) {
        std::cerr << " OnMonT0toRPC::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
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
        for (unsigned int k=0; k != nChanTrig; k++) fFOutTrigger << " adc" << k;
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
	fFOutTrigT0RPC << " spill dSpill evt dEvt T0OK RPCOK trigAdc0 trigAdc1  trigAdc2 trigAdc3 numCT0HighTOk numCT0High numCT0Low ";
	for (size_t k=1; k != 19; k++) fFOutTrigT0RPC << " T0tdc" << k << " T0adc" << k;
	for (size_t k=0; k != nchanRPC; k++) fFOutTrigT0RPC << " RPCtdc" << k;
	fFOutTrigT0RPC << " " << std::endl;
      }
      fFilesAreOpen = true;
   }
    
    //......................................................................

    void OnMonT0toRPC::endJob()
    {
      std::cerr << " OnMonT0toRPC::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Total number of events " << fNEvents << " No Info T0 and No RPC " << fNoT0RPCInfo 
                << "  No trigger Info " << fNoTrigInfo << " No T0 info " << fNoT0Info << " No RPC info " << fNoRPCInfo << std::endl; 
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutRPC.is_open()) fFOutRPC.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0RPC.is_open()) fFOutTrigT0RPC.close();
    }

    //......................................................................


    void OnMonT0toRPC::FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH, art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
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
	    if (detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw) {
		fT0ADCs[detchan] = adc;
		vT0ADChits[detchan]=1; 
	      }
	    } else {
	      std::cerr << " OnMonT0toRPC::FillT0Plots , Unexpected Channel in ADC array detchan " 
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
	    int chan = trb3.GetChannel() + 65*(trb3.fpga_header_word-1280);
	    int board = 100;
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
//                 std::cerr << " OnMonT0toRPC::FillT0Plots , Unexpected Channel in TDC array detchan " 
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
    void    OnMonT0toRPC::FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::RPC);
//      std::cerr << " OnMonT0toRPC::FillRPCPlots, number of channels " << nchan << std::endl;
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
            int chan = trb3.GetChannel() + 65*(trb3.fpga_header_word-1280);
            int board = 100;
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

    void   OnMonT0toRPC::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	    int detchan = dchan.Channel();
	    if (detchan >= 0 && detchan < nchan) {
	      float adc = wvfm.Baseline()-wvfm.PeakADC();
	      float blw = wvfm.BLWidth();
	      if (adc > 5*blw)
		fTrigADCs[detchan] = adc;
	    }
	  }
      }
      if (fMakeTrigFullNtuple) {
        fFOutTrigger << " " << fSubRun << " " << fEvtNum;
        for (int k=0; k != nchan; k++) fFOutTrigger << " " << fTrigADCs[k];
        fFOutTrigger << std::endl;
      }
      
    }
    void OnMonT0toRPC::FillTrigT0RPCV1(bool gotT0, bool gotRPC) {  
        
      int nchanTrig = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      size_t nchanRPC = static_cast<size_t> (emph::geo::DetInfo::NChannel(emph::geo::RPC));
      int nBlank = 3 + 2*18 + nchanRPC; 
// 	fFOutTrigT0RPC << " spill dSpill evt dEvt T0OK RPCOK trigAdc0 trigAdc1  trigAdc2 trigAdc3 numcT0HighTok numcT0High numcT0Low "; 
// For now.. 
       fFOutTrigT0RPC << " " << fSubRun << " " << fSubRun - fPrevSubRun << " " << fEvtNum << " " << fEvtNum - fPrevEvtNum;
       if (gotT0) { fFOutTrigT0RPC << " 1" ; } else { fFOutTrigT0RPC << " 0" ; } 
       if (gotRPC) { fFOutTrigT0RPC << " 1" ; } else { fFOutTrigT0RPC << " 0" ; }
       for (int k=0; k != nchanTrig; k++) fFOutTrigT0RPC << " " << fTrigADCs[k];
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
    void OnMonT0toRPC::analyze(const art::Event& evt)
    { 
      ++fNEvents;
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
	    if (i == emph::geo::Trigger) FillTrigPlots(wfHandle);
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
      this->FillTrigT0RPCV1(gotT0, gotRPC);
      fPrevEvtNum = static_cast<int>(fEvtNum);
      fPrevSubRun = static_cast<int>(fSubRun);
      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::OnMonT0toRPC)
