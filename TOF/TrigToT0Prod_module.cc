////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to study trigger, and T0
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
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/WaveForm.h"
#include "RecoBase/TrigToT0.h"
#include "TOF/PeakInWaveForm.h"

using namespace emph;

///Validate the signal from the trigger counter to filter out the multiple particle per 400 ns window, 
///look at T0, find the segment with the unipolar signal, and produce a short block with the trigger and T0 segment number and signal characteristic. 
//
namespace emph {
  namespace tof {
  
    
    class TrigToT0Prod : public art::EDProducer {
    public:
      explicit TrigToT0Prod(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~TrigToT0Prod();

       // Plugins should not be copied or assigned.
      TrigToT0Prod(TrigToT0Prod const&) = delete;
      TrigToT0Prod(TrigToT0Prod&&) = delete;
      TrigToT0Prod& operator=(TrigToT0Prod const&) = delete;
      TrigToT0Prod& operator=(TrigToT0Prod&&) = delete;

     // Optional, read/write access to event
      void produce(art::Event& evt) override;

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
    
      void  openOutputCsvFiles(); // At this point, we know the run number..
      void  resetAllADCsTDCs();
      
      void   FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > &,
			 art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillTrigT0Final(art::Event& evt, bool gotT0); 
      
       // an analysis routine, first version, joint analysis of the content of the Trigger, T0 and RPC buffers. 

      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

      bool fFilesAreOpen;
      std::string fTokenJob;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      unsigned int fNoT0Info;
      unsigned int fNoTrigInfo;
      unsigned int fNoTDCInfo;
      unsigned int fNoT0RPCInfo;
      unsigned int fNRPCHi_Leading, fNRPCLo_Leading;
      unsigned int fNRPCHi_Trailing, fNRPCLo_Trailing;
      
      

      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanT0  = 20;
      static const unsigned int nChanTrig = 4;
//
// Some data structure being filled in specific fill routines;
//  There content is event specific. To be used in the summary method, where we establish  some coincidences.  
//
     std::vector<double> fT0ADCs; // summary 
     std::vector<emph::tof::PeakInWaveForm> fPeakT0s; // details
     std::vector<double> fT0TDCs; // parallel vectors
     std::vector<double> fT0TDCsFrAdc; // The bin at which the peak was found 
//     std::vector<double> fRPCADCs(nChanRPC, 0.);
     std::vector<double> fRPCTDCs;
     std::vector<bool> fRPCHiLows;
     std::vector<int> fRPCEChans;
     std::vector<double> fTrigPeakADCs;
     std::vector<double> fTrigADCs;
     std::vector<emph::tof::PeakInWaveForm> fPeakTriggers;
     //
     // flags for studying, dumping the info. 
     //
     bool fMakeT0FullNtuple; 
     bool fMakeRPCFullNtuple;
     bool fMakeTrigFullNtuple;
     bool fMakeEventSummaryNTuple;
     
     // In case we have a unique T0 segment, with good adc signal for both the up and down light guide/SiPm, we store 
     int fNumT0Hits;	// Total number of Unipolar peaks found in the 20 T0 V1720 waveform for the T0 counter.  	     
     int fNumT0HitsFirst; // The number of hits found in the first pass.		     
     int fNumT0HitsFirstUpDown;	 // ... with a matched coincidence 	     
     int fNumT0Hits2nd; // The number of hits found in the 2nd pass.	There must be a first pass peak in the given waveform. 	     
     int fNumT0Hits2ndUpDown; // ... with a matched coincidence  	     
     int fT0SegmentHitFirst; // The segment number corresponding to the first pass Unipolar peak. 
     int fT0SegmentHit2nd; //ibid, 2nd pass peak  
     double fT0SumSigUpFirst; // The peak intergal for the up segment, found in the 1rst pass 
     double fT0SumSigDownFirst; // ibid, down segment 
     double fT0SumSigUp2nd; // ibid , for the 2nd pass. 
     double fT0SumSigDown2nd;
     double fTdcUniqueSegmentBottom, fTdcUniqueSegmentTop; // TDC (from TRB3 handle), for the corresponding segments. 


     
      // define streamers for csv files. 
      std::ofstream fFOutT0, fFOutT0Prof;
      std::ofstream fFOutTrigger; 
      std::ofstream fFOutTrigT0Final; 
      
      void dumpWaveForm(emph::geo::tDetectorType aType, int detchan, std::string &polarityStr, std::vector<uint16_t> &wf); 

    };    
    //.......................................................................
    TrigToT0Prod::TrigToT0Prod(fhicl::ParameterSet const& pset)
      : EDProducer(pset),
	fFilesAreOpen(false),
	fTokenJob (pset.get<std::string>("tokenJob", "UnDef")),
	fRun(0), fSubRun(0), fEvtNum(0), 
	fT0ADCs(nChanT0+2, 0.),
	fT0TDCs(nChanT0+2, DBL_MAX),
	fT0TDCsFrAdc(nChanT0+2, DBL_MAX),
	fTrigPeakADCs(nChanTrig, 0.),
	fTrigADCs(nChanTrig, 0.),
	fMakeT0FullNtuple  (pset.get<bool>("makeT0FullNtuple",true)), // keep them for now.. 
	fMakeTrigFullNtuple (pset.get<bool>("makeTrigFullNtuple",true)),
	fMakeEventSummaryNTuple (pset.get<bool>("makeEventSummaryFullNtuple",false)),
	fNumT0Hits(0),		     
	fNumT0HitsFirst(0),		     
      fNumT0HitsFirstUpDown(0),		     
      fNumT0Hits2nd(0),		     
      fNumT0Hits2ndUpDown(0),  	     
      fT0SegmentHitFirst(-1),
      fT0SegmentHit2nd(-1),   
      fT0SumSigUpFirst(0.),
      fT0SumSigDownFirst(0.),
      fT0SumSigUp2nd(0.),
      fT0SumSigDown2nd(0.),
      fTdcUniqueSegmentBottom(DBL_MAX),
      fTdcUniqueSegmentTop(DBL_MAX)
    {

      produces< rb::TrigToT0 >();

    }
    
    //......................................................................
    TrigToT0Prod::~TrigToT0Prod()
    {
    
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutT0Prof.is_open()) fFOutT0Prof.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0Final.is_open()) fFOutTrigT0Final.close();
      
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void TrigToT0Prod::beginJob()
    {
      fNEvents= 0;
      fNoT0Info = 0; fNoTrigInfo = 0; fNoTrigInfo = 0;  
      fNRPCHi_Leading = 0; fNRPCHi_Trailing = 0; fNRPCLo_Leading = 0; fNRPCLo_Trailing = 0; 

      //
      // open a few csv file for output. Delayed until we know the run number.  
      //
            
    }

    //......................................................................
    void TrigToT0Prod::openOutputCsvFiles() {
//
      if (fRun == 0) {
        std::cerr << " TrigToT0Prod::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	exit(2);
      }
      if (fMakeT0FullNtuple) { 
        std::ostringstream fNameT0StrStr; fNameT0StrStr << "./T0Tuple_V2_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameT0Str(fNameT0StrStr.str());
        fFOutT0.open(fNameT0Str.c_str());
	fFOutT0 << " subRun evt nP1Uni nP1Bi nP2Uni nP2Bi ";
        for (unsigned int k=0; k != nChanT0; k++) fFOutT0 << " adc" << k << " tdc" << k << " tdcA" << k << " type" << k; 
        fFOutT0 << " " << std::endl;
	
        std::ostringstream fNameT0ProfStrStr; fNameT0ProfStrStr << "./T0TupleProf_V2_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameT0ProfStr(fNameT0ProfStrStr.str());
        fFOutT0Prof.open(fNameT0ProfStr.c_str());	
        fFOutT0Prof << " Spill  evt nH nH1 nH1UpDwn nH2 nH2UpDwn seg1 seq2 sumSigUp1 sumSigDwn1 sumSigUp2 sumSigDwn2 tdcH1Bot tdcH1Top " << std::endl;
      }
      if (fMakeTrigFullNtuple) { 
        std::ostringstream fNameTrigStrStr; fNameTrigStrStr << "./TrigTuple_V2_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameTrigStr(fNameTrigStrStr.str());
        fFOutTrigger.open(fNameTrigStr.c_str());
	fFOutTrigger << " subRun evt ";
        for (unsigned int k=0; k != nChanTrig; k++) 
	  fFOutTrigger << " Padc" << k << " Sadc" << k << " Width" << k << " EndBin" << k 
	               << " numPeak" << k << " deltaT12_" << k << " S2adc" << k  << " deltaT13_" << k << " S3adc" << k;
        fFOutTrigger << " " << std::endl;
      }
      if (fMakeEventSummaryNTuple) {
        std::ostringstream fNameSumStrStr; fNameSumStrStr << "./SummaryTrigT0RPC_V2_" << fRun << "_" << fTokenJob << ".txt";
        std::string fNameSumStr(fNameSumStrStr.str());
        fFOutTrigT0Final.open(fNameSumStr.c_str());
	std::cerr << " Opening fFOutTrigT0Final... " << std::endl;
	fFOutTrigT0Final << " spill evt ";
	for (int k=0; k != 4; k++) fFOutTrigT0Final << " nPeakTrig" << k << " dtPeakTrig" << k;
	fFOutTrigT0Final << " Sum4PMT";
        fFOutTrigT0Final << " T0nH T0nH1 T0nH1UpDwn T0nH2 T0nH2UpDwn T0seg1 T0seq2 T0sumSigUp1" <<
	               " T0sumSigDwn1 T0sumSigUp2 T0sumSigDwn2 T0tdcH1Bot T0tdcH1Top ";
	fFOutTrigT0Final << " " << std::endl;
      }
      fFilesAreOpen = true;
   }
    
    //......................................................................

    void TrigToT0Prod::endJob()
    {
      std::cerr << " TrigToT0Prod::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Total number of events " << fNEvents << " No Info T0 " << fNoT0RPCInfo 
                << "  No trigger Info " << fNoTrigInfo << " No T0 info " << fNoT0Info << std::endl; 
	
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0Final.is_open()) fFOutTrigT0Final.close();
    }

    //......................................................................
    
    void TrigToT0Prod::resetAllADCsTDCs() {
       fPeakTriggers.clear();
       fPeakT0s.clear();
       for(std::vector<double>::iterator it = fT0ADCs.begin(); it != fT0ADCs.end(); it++) *it = -1.0*DBL_MAX/2.;
       for(std::vector<double>::iterator it = fT0TDCs.begin(); it != fT0TDCs.end(); it++) *it = DBL_MAX;
       for(std::vector<double>::iterator it = fT0TDCsFrAdc.begin(); it != fT0TDCsFrAdc.end(); it++) *it = DBL_MAX;
       for(std::vector<int>::iterator it = fRPCEChans.begin(); it != fRPCEChans.end(); it++) *it = INT_MAX;
       for(std::vector<double>::iterator it = fTrigADCs.begin();it != fTrigADCs.end(); it++) *it = -1.0*DBL_MAX/2.;
       for(std::vector<double>::iterator it = fTrigPeakADCs.begin();it != fTrigPeakADCs.end(); it++) *it = 0.;
       fNumT0Hits = 0;		      
       fNumT0HitsFirst = 0;		      
       fNumT0HitsFirstUpDown = 0;		      
       fNumT0Hits2nd = 0;		      
       fNumT0Hits2ndUpDown = 0;		      
       fT0SegmentHitFirst = -1;
       fT0SegmentHit2nd = -1;	
       fT0SumSigUpFirst = 0.;
       fT0SumSigDownFirst = 0.;
       fT0SumSigUp2nd = 0.;
       fT0SumSigDown2nd = 0.;
       fTdcUniqueSegmentBottom = DBL_MAX; fTdcUniqueSegmentTop = DBL_MAX;
		      

    }

    void TrigToT0Prod::FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH, art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H)
    {
//       std::cerr << " TrigToT0Prod::FillT0Plots tmp debug fRun " << fRun << " spill " << fSubRun << " evt " << fEvtNum << std::endl;
//       bool debugIsOn = (((fEvtNum == 47) && (fSubRun == 11)) || ((fEvtNum == 411) && (fSubRun == 13)));
//       bool debugIsOn = ((fEvtNum > 89) && (fEvtNum < 93) && (fSubRun == 2));
       bool debugIsOn = ((fEvtNum == 91) && (fSubRun == 2) && (fRun == 1365));
       debugIsOn = ((debugIsOn) || ((fEvtNum == 6) && (fSubRun == 1) && (fRun == 1365)));
       debugIsOn = ((debugIsOn) || ((fEvtNum == 26) && (fSubRun == 1) && (fRun == 1365)));
       debugIsOn = ((debugIsOn) || ((fEvtNum == 43) && (fSubRun == 1) && (fRun == 1365)));
       debugIsOn = ((debugIsOn) || ((fEvtNum == 308) && (fSubRun == 1) && (fRun == 1365)));
       debugIsOn = ((debugIsOn) || ((fEvtNum == 691) && (fSubRun == 10) && (fRun == 1295)));
       if (debugIsOn) std::cerr << "----------------------------------------------------------------------" << std::endl 
                                << " TrigToT0Prod::FillT0Plots, spill " << fSubRun << " evt " << fEvtNum << std::endl;
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::T0);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      double trb3LinearLowEnd = 15.0;
      double trb3LinearHighEnd = 494.0; // For FPGA2 -- T0 // Phase 1a version.. 
      echan.SetBoardType(boardType);
      std::vector<int> vT0ADChits(nchan,0);	// keep... old code from plotMon..     
      std::vector<int> vT0TDChits(nchan,0);
      if (debugIsOn) std::cerr << " Number of waveforms " << wvfmH->size() << std::endl;
      int numFirstPeakBipolar = 0;	  
      int numFirstPeakUnipolar = 0;	  
      int num2ndPeakBipolar = 0;	  
      int num2ndPeakUnipolar = 0;	  
      if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetBoardType(boardType);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    size_t detchan = static_cast<size_t> (dchan.Channel());
            if (debugIsOn) std::cerr << std::endl << " For wave form index  " << idx << " board " 
	                             << board << " dchan " << dchan << " detchannel " << detchan << std::endl;	  
	    fT0ADCs[detchan] = -1.0e10;
	    vT0ADChits[detchan]=0; 
	    if (detchan <= static_cast<size_t>(nchan)) {
	      std::vector<uint16_t> tmpwf = wvfm.AllADC();
              PeakInWaveForm firstPeak(emph::geo::T0);
	      if (debugIsOn) firstPeak.setDebugOn(true);
	      std::string polarityStr("none");
	      if (!firstPeak.findIt(tmpwf, 5.0, 10)) {
	        if (debugIsOn) dumpWaveForm(emph::geo::T0, detchan, polarityStr, tmpwf);
		continue; 
	      }
	      firstPeak.setUserID(detchan);
	      fPeakT0s.push_back(firstPeak);
	      if (firstPeak.getPeakType() == UNIPOLAR) numFirstPeakUnipolar++; 
	      if (firstPeak.getPeakType() == BIPOLAR) numFirstPeakBipolar++; 
	      if (debugIsOn) {
	        if (firstPeak.getPeakType() == emph::tof::BIPOLAR) polarityStr = std::string("Bipolar"); 
                else if (firstPeak.getPeakType() == emph::tof::UNIPOLAR) polarityStr = std::string("Unipolar"); 
		std::cerr << firstPeak;
	        dumpWaveForm(emph::geo::T0, detchan, polarityStr, tmpwf); 
	      }
	      PeakInWaveForm secondPeak(emph::geo::T0);
	      secondPeak.setDebugOn(debugIsOn);
	      if (secondPeak.findItAfter(firstPeak, tmpwf, 5.0)) { 
	        if (secondPeak.getPeakType() == UNIPOLAR) num2ndPeakUnipolar++; 
	        if (secondPeak.getPeakType() == BIPOLAR) num2ndPeakBipolar++; 
	        if (debugIsOn) {
	          std::cerr << " ... Found a 2nd peak...  " << std::endl;
		  std::cerr << secondPeak;
	        }
	        secondPeak.setUserID( 1000 + detchan);
	        fPeakT0s.push_back(secondPeak);
              }
	      if (detchan < fT0ADCs.size()) {
	        fT0ADCs[detchan] = firstPeak.getSumSig();
	        fT0TDCsFrAdc[detchan] = firstPeak.getPeakBin();
	        vT0ADChits[detchan]= (firstPeak.getPeakType() == emph::tof::BIPOLAR) ? 2 : 1;
              } 
	    } else {
	      std::cerr << " TrigToT0Prod::FillT0Plots , Unexpected Channel in ADC array detchan " 
	                << detchan << " nchan " << nchan << " expected nchan " << fT0ADCs.size()-1 <<  std::endl;
              size_t badDetChan =  fT0ADCs.size()-1;
	      fT0ADCs[badDetChan] = detchan;
	    }
	  }
      }
      if (debugIsOn) { std::cerr << " Total number of peaks  " << fPeakT0s.size() << std::endl;  }
      
      
      // Note : if the handle is empty or faulty (no data), this method returns.. Not intended.. Fix it only for run 1365 
      if ((fRun != 1365) && (!trb3H->empty())) {
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
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    int detchan = dchan.Channel();
	    long double time_T0 = trb3.GetEpochCounter()*10240026.0 + trb3.GetCoarseTime() *
	                               5000.0 - ((trb3.GetFineTime() - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;
	    //std::cout<<"detchan value: "<<detchan<<std::endl;
	    if (detchan < nchan) { // watch out for channel 500!
//	      hitCount[detchan] += 1;
	      fT0TDCs[detchan] = ((triggerTime-time_T0)/100000);
	    }  else {
	    // This occurs at every events... 
//                 std::cerr << " TrigToT0Prod::FillT0Plots , Unexpected Channel in TDC array detchan " 
//	                << detchan << " nchan " << nchan << " expected nchan " << fT0ADCs.size()-1 <<  std::endl;
                 size_t badDetChan =  fT0TDCs.size()-1;
	         fT0TDCs[badDetChan] = detchan;
			
	    }
         }
      }
	// 
	// get a profile along X, asking for large pulse, and correct timing..Count the number of matching up/down pairs.  
	// 
	// first elements are guess, based on the study of run 1365, special run with no target, no TRB3,  120 GeV proton data. 
	//
	std::vector<double> cutSumVals{ 320., 325., 230., 320., 370., 380., 240., 365., 185., 250., 270., 280., 
	                                275., 320., 290., 315., 290., 315., 250., 235. };
	std::vector<double> meanTPeakVals{29., 2.970284e+01,  2.996849e+01,  2.993687e+01,  2.964449e+01,  2.929533e+01,  1.872250e+01,
	                     30.,  1.746695e+01,  1.588931e+01,  1.837553e+01,
                             1.744499e+01,  1.710197e+01,  1.691933e+01,  1.679155e+01,  1.590024e+01,  1.528530e+01, 
			      1.619635e+01,  1.583242e+01,  1.661496e+01 };
			      
	for (std::vector<PeakInWaveForm>::const_iterator it1 = fPeakT0s.cbegin(); it1 != fPeakT0s.cend(); it1++) {
	  if (it1->getPeakType() != emph::tof::UNIPOLAR) continue;
	  const int uid1 = it1->getUserID();
	  const int uid1R = (uid1 < 1000) ? uid1 : uid1-1000;	// first of 2nd peak. 
	  if ((uid1R < 0) || (static_cast<size_t>(uid1R) >=  cutSumVals.size())) continue; // should not happen   
	  if (it1->getSumSig() <  cutSumVals[static_cast<size_t>(uid1R)]) continue;
	  fNumT0Hits++;
	  if (uid1 < 1000) { // first encountred hits. 
	      if ((fRun == 1365) && (std::abs(it1->getPeakBin() - meanTPeakVals[(size_t)uid1R]) > 15.)) continue; 
	      // Timing did changed for run 1295.. 
	      fNumT0HitsFirst++; fT0SegmentHitFirst = uid1;
	     if (uid1 >= 10) fT0SumSigUpFirst = it1->getSumSig();
	     else  fT0SumSigDownFirst = it1->getSumSig();
	     // look for an up Down match. 
	     for (std::vector<PeakInWaveForm>::const_iterator it2 = fPeakT0s.cbegin(); it2 != fPeakT0s.cend(); it2++) {
	       if (it1 == it2) continue;
	       if (it2->getPeakType() != emph::tof::UNIPOLAR) continue;
	       const int uid2 = it2->getUserID();
	       if (uid2 >= 1000) continue;
	       if ((uid2 < 0) || (static_cast<size_t>(uid2) >=  cutSumVals.size())) continue; // should not happen   
	       if (it2->getSumSig() <  cutSumVals[(size_t) uid2]) continue;
	       if ((uid1 == uid2+10) || (uid2 == uid1 + 10)) fNumT0HitsFirstUpDown++; // Does not take into accout the offset between top/bottom row Might be incomplete.
	     }
	  
	  } else { // 2nd 
	      fNumT0Hits2nd++; fT0SegmentHit2nd = uid1R;
	     if (uid1R >= 10) fT0SumSigUp2nd = it1->getSumSig();
	     else  fT0SumSigDown2nd = it1->getSumSig();
	     // look for an up Down match. 
	     for (std::vector<PeakInWaveForm>::const_iterator it2 = fPeakT0s.cbegin(); it2 != fPeakT0s.cend(); it2++) {
	       if (it1 == it2) continue;
	       if (it2->getPeakType() != emph::tof::UNIPOLAR) continue;
	       const int uid2 = it2->getUserID();
	       if (uid2 < 1000) continue;
	       const int uid2R = uid2 - 1000;
	       if ((uid2R < 0) || (static_cast<size_t>(uid2R) >=  cutSumVals.size())) continue; // should not happen   
	       if (it2->getSumSig() <  cutSumVals[(size_t) uid2R]) continue;
	       if ((uid1R == uid2R + 10) || (uid2R == uid1R + 10)) fNumT0Hits2ndUpDown++;
	     }

	  }
	 
	} // on the Peaks, the first ones, chronologically. 
	if ((fNumT0HitsFirst == 2) && (fNumT0HitsFirstUpDown == 2) && (fNumT0Hits2nd == 0)) {
	  int aSegment = fT0SegmentHitFirst;
	  if (aSegment > 10) aSegment -= 10;
          fTdcUniqueSegmentBottom = fT0TDCs[aSegment]; 
	  fTdcUniqueSegmentTop = fT0TDCs[aSegment+10];
	}
	
      if (fMakeT0FullNtuple) {
        fFOutT0 << " " << fSubRun << " " << fEvtNum << " " << numFirstPeakUnipolar << " " << numFirstPeakBipolar 
	        << " " << num2ndPeakUnipolar << " " << num2ndPeakBipolar;
        for (int k=0; k != nchan; k++) fFOutT0 << " " << fT0ADCs[k] << " " << fT0TDCs[k] << " " << fT0TDCsFrAdc[k] << " " << vT0ADChits[k];
	fFOutT0 << " " <<std::endl;
        // 
 	fFOutT0Prof << " " << fSubRun << " " << fEvtNum << " " << fNumT0Hits << " " << fNumT0HitsFirst << " " << fNumT0HitsFirstUpDown
	            << " " << fNumT0Hits2nd << " " << fNumT0Hits2ndUpDown << " " << fT0SegmentHitFirst << " " << fT0SegmentHit2nd << " " 
	            << fT0SumSigUpFirst << " " << fT0SumSigDownFirst << " " << fT0SumSigUp2nd << " " << fT0SumSigDown2nd 
		    << " " << fTdcUniqueSegmentBottom << " " << fTdcUniqueSegmentTop << std::endl;

      }
      
      
    }  
        
    //.....................................}.................................

    void   TrigToT0Prod::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
//       std::cerr << " TrigToT0Prod::FillTrigPlots tmp debug fRun " << fRun << " spill " << fSubRun << " evt " << fEvtNum << std::endl;
       std::string polarityStr("DCNEG"); 
//      const bool debugIsOn = (((fEvtNum < 5) && (fSubRun < 5)) || (((fRun == 511) && ((fSubRun == 10) ) && ( 
//     	(fEvtNum == 92) || (fEvtNum == 158) || (fEvtNum == 585) || 
//        (fEvtNum == 601) || (fEvtNum == 762) || (fEvtNum == 875))) || ((fSubRun == 7) && (fEvtNum > 1874)));
//      const bool debugIsOn = ((fRun == 511) && (fSubRun == 7) && (fEvtNum > 1874 ) && (fEvtNum < 1880 )); 
//      const bool debugIsOn = ((fRun == 511) && (fEvtNum > 250000 )); 
//      const bool debugIsOn = ((((fRun == 511) && (fSubRun == 10) && ( 
//     	(fEvtNum == 69) || (fEvtNum == 387) || (fEvtNum == 569) || 
//        (fEvtNum == 954) || (fEvtNum == 1414) || (fEvtNum == 1455)))));
     bool debugIsOn = ((fRun == 1365) && (fSubRun == 1) && (fEvtNum == 6));  // Single proton, no problem. 
     debugIsOn = ((debugIsOn) || ((fRun == 1365) && (fSubRun == 1) && (fEvtNum == 26))); // Large amplitude, two particle in T0 
     debugIsOn = ((debugIsOn) || ((fRun == 1365) && (fSubRun == 1) && (fEvtNum == 333))); // 2nd particle 7rf. buchet away.. 
     debugIsOn = ((debugIsOn) || ((fRun == 1365) && (fSubRun == 1) && (fEvtNum == 56))); // 2nd particle Within first pulse. in 4 PMTs..
      debugIsOn = ((debugIsOn) || ((fRun == 1365) && (fSubRun == 1) && (fEvtNum == 308))); // 2nd particle in T0 channel 5 
       debugIsOn = ((debugIsOn) || ((fEvtNum == 691) && (fSubRun == 10) && (fRun == 1295)));
    if (debugIsOn) std::cerr << " Starting TrigToT0Prod::FillTrigPlots, spill " << fSubRun << " evt " << fEvtNum << std::endl;
      if (wvfmH->empty()) {
         std::cerr << " No Trigger Waveform data... " << std::endl;
         return;
      }
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      std::vector<int> numPeaks(fTrigADCs.size(), 0);
      for (size_t idx=0; idx < wvfmH->size(); ++idx) {
          const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
          int chan = wvfm.Channel();
          int board = wvfm.Board();
          echan.SetBoard(board);
          echan.SetChannel(chan);
          emph::cmap::DChannel dchan = cmap->DetChan(echan);
          int detchan = dchan.Channel();
          if (debugIsOn) std::cerr << std::endl <<  " Searching for Peak in channel  " << detchan << std::endl;
          if (detchan >= 0 && (detchan < nchan)) {
            std::vector<uint16_t> tmpwf = wvfm.AllADC();
	    if (tmpwf.size() < 100) {
	      std::cerr << " Anomalous waveform, size " << tmpwf.size() << "  spill " << fSubRun << " evt " << fEvtNum << " fatal.. " << std::endl; exit(2);
	    }
            // Dump of some wave forms
	    if (debugIsOn) this->dumpWaveForm(emph::geo::Trigger, detchan, polarityStr, tmpwf);
//
// search for peaks 
	    PeakInWaveForm peak1(emph::geo::Trigger);
	    if (debugIsOn) peak1.setDebugOn(true);
	    if (!peak1.findIt(tmpwf, 8.0, 10)) continue; // Should be rare case..
	    peak1.setUserID(detchan); // to be stored after we look for a 2nd peak within the first one. 
	    if (debugIsOn) {
	      std::cerr << " First Peak found for channel " << detchan << std::endl; std::cerr << peak1;
	    }
	    numPeaks[detchan]++;
	    PeakInWaveForm peak2(emph::geo::Trigger);
	    if (debugIsOn) peak2.setDebugOn(true);
	    if (peak2.findItWithin(peak1, tmpwf, 7.0)) {
	      peak2.setUserID(10 + detchan);
	      fPeakTriggers.push_back(peak1);
	      fPeakTriggers.push_back(peak2);
	      numPeaks[detchan]++;
	      if (debugIsOn) {
	        emph::tof::PeakType pType = peak2.getPeakType();
	        std::cerr << " Testing stupid compiler " << pType << std::endl;
	        std::cerr << " 2nd Peak, within, found for channel " << detchan << std::endl; std::cerr << peak2;
	      }
	    } else {
	     fPeakTriggers.push_back(peak1);
	    }
	    PeakInWaveForm peak3(emph::geo::Trigger);
	    peak3.setDebugOn(debugIsOn);
	    bool lastPeakis2nd = (peak2.getPeakBin() != 1024);
	    if ((!lastPeakis2nd) && (!peak3.findItAfter(peak1, tmpwf, 5.0))) continue;  // offset a bit arbitrary, to be tuned. 
	    if ((lastPeakis2nd) && (!peak3.findItAfter(peak2, tmpwf, 5.0))) continue;  // offset a bit arbitrary, to be tuned. 
	    if (debugIsOn) {
	        if (!lastPeakis2nd) std::cerr << " 2nd Peak, after 1rst,  "; 
		else std::cerr << " 2nd Peak, after 2nd,  ";
		std::cerr << " for channel " << detchan << std::endl; 
		std::cerr << peak3;
	    }
	    /*
	     else { // We will comment out this code after checking the algorithm.. 
                std::cerr << " Autodebug  TrigToT0Prod::FillTrigPlots, spill " << fSubRun << " evt " << fEvtNum << std::endl  
		          << " First Peak " << std::endl;
		std::cerr <<  peak1 << std::endl;
		if (lastPeakis2nd)  std::cerr << " Peak2 " << peak2 << std::endl; 
	        PeakInWaveForm peak3Dbg(emph::geo::Trigger);
	        peak3Dbg.setDebugOn(true);
	        if (lastPeakis2nd) peak3Dbg.findItAfter(peak2, tmpwf, 5.0);
	        else  peak3Dbg.findItAfter(peak1, tmpwf, 5.0);
	        std::cerr << " Auto debug of 2nd Peak, After ,   for channel " << detchan << std::endl; 
		std::cerr << peak3Dbg;
	    }
	    */
	    peak3.setUserID(100 + detchan);
	    fPeakTriggers.push_back(peak3);
	    numPeaks[detchan]++;
        } // this detchan is OK  
      } // loop on channels 
      if (fEvtNum > 1000000) { std::cerr << " And quit after 1000000  evts, still debugging ... " << std::endl; exit(2); }
      if (fMakeTrigFullNtuple) {

            fFOutTrigger << " " << fSubRun << " " << fEvtNum;
            for (int k=0; k != static_cast<int>(numPeaks.size()); k++) {
	       std::vector<PeakInWaveForm>::const_iterator iPk1 = fPeakTriggers.cend();
	       std::vector<PeakInWaveForm>::const_iterator iPk2 = fPeakTriggers.cend();
	       std::vector<PeakInWaveForm>::const_iterator iPk3 = fPeakTriggers.cend();
	       for (std::vector<PeakInWaveForm>::const_iterator iPk = fPeakTriggers.cbegin(); iPk != fPeakTriggers.cend(); iPk++) {
	         if (iPk->getUserID() == k) iPk1 = iPk;
	         if (iPk->getUserID() == (10 + k)) iPk2 = iPk;
	         if (iPk->getUserID() == (100 + k)) iPk3 = iPk;
	       }
	       if (iPk1 == fPeakTriggers.cend()) {
	          fFOutTrigger << " 0 0 0 0 0 0 0 0 0 "; continue;
		  //                1 2 3 4 5 6 7 8 9
	       } 
               fFOutTrigger << " " << iPk1->getPeakVal() << " " << iPk1->getSumSig()
                            << " " << iPk1->getPeakBin() << " " << iPk1->getEndBin() << " " << numPeaks[k];
	       if ((iPk2 == fPeakTriggers.cend()) && (iPk3 == fPeakTriggers.cend())) {
	         fFOutTrigger << " 0 0 0 0 ";
		 continue;
	       } 
	       if (iPk2 != fPeakTriggers.cend()) {
	         const double dt12 = iPk2->getPeakBin() - iPk1->getPeakBin();
	         fFOutTrigger << " " << dt12 << " " << iPk2->getSumSig();
	       } else {
	         fFOutTrigger << " 0 0 ";
	       }
	       if (iPk3 != fPeakTriggers.cend()) {
	         const double dt13 = iPk3->getPeakBin() - iPk1->getPeakBin();
	         fFOutTrigger << " " << dt13 << " " << iPk3->getSumSig();
	       } else {
	         fFOutTrigger << " 0 0 ";
	       }
            }
            fFOutTrigger << std::endl;
      }
//      if (debugIsOn) { std::cerr << " And quit after first Trigger dump complete analysis " << std::endl; exit(2); }
    }
    void TrigToT0Prod::FillTrigT0Final(art::Event& evt, bool gotT0) {  
        
      const int nBlankTrigger = 4 + 4 + 1; // the number of peaks for each PMT, the delta ts between first and 2nd (found within), 
                                           // or first and third (found after)   
      const int nBlankT0 = 13;
      
      int anPeakTrig = 0;  //For final Trig to T0 event quality. 
      double asumPMTTrig = 0.;
      int anumSegT0 = 0;
      int asegT0 = INT_MAX;
      double asumSigUpT0 = 0.;
      double asumSigDownT0 = 0.;

       if (fMakeEventSummaryNTuple) fFOutTrigT0Final << " " << fSubRun  << " " << fEvtNum << " ";
       // Trigger..
       if (fPeakTriggers.size() == 0) {
         for (int k=0; k != nBlankTrigger; k++) fFOutTrigT0Final << " 0"; 
       } else {
         double sumSigAllFirst = 0.;
         for (int iPMT=0; iPMT !=4; iPMT++) {
	   int np = 0;
	   int peakBinFirst = 0; 
           for (std::vector<PeakInWaveForm>::const_iterator it = fPeakTriggers.cbegin(); it != fPeakTriggers.cend(); it++) {
              if ((it->getUserID() == iPMT) || (it->getUserID() == (iPMT + 10)) || (it->getUserID() == (iPMT + 100))) np++;
	      if (it->getUserID() == iPMT) { anPeakTrig++; peakBinFirst = it->getPeakBin(); sumSigAllFirst += it->getSumSig(); }
           }
	   if (fMakeEventSummaryNTuple) fFOutTrigT0Final << " " << np; 
	   int deltaT = 0; // in 4ns units (digitization frequency of the V1720 waveforms).. 2nd 
           for (std::vector<PeakInWaveForm>::const_iterator it = fPeakTriggers.cbegin(); it != fPeakTriggers.cend(); it++) {
	      if  (it->getUserID() == iPMT) continue;
              if (it->getUserID() == (iPMT + 10)) deltaT = it->getPeakBin() - peakBinFirst;
	      if (it->getUserID() == (iPMT + 100)) deltaT = it->getPeakBin() - peakBinFirst;
           }
	   if (fMakeEventSummaryNTuple) fFOutTrigT0Final << " " << deltaT;
	 }
	 asumPMTTrig = sumSigAllFirst;
 	 if (fMakeEventSummaryNTuple) fFOutTrigT0Final << " " <<  sumSigAllFirst; 
       }
	// T0. Simply copy the profile info. 
       if (!gotT0) {
           for (int k=0; k != nBlankT0; k++) fFOutTrigT0Final << " 0"; 
       } else { 
          anumSegT0 = fNumT0Hits;
	  asegT0 = fT0SegmentHitFirst;
	  asumSigUpT0 = fT0SumSigUpFirst;
	  asumSigDownT0 = fT0SumSigDownFirst;
	  if (fMakeEventSummaryNTuple) fFOutTrigT0Final << " " << fNumT0Hits << " " << fNumT0HitsFirst << " " << fNumT0HitsFirstUpDown
	            << " " << fNumT0Hits2nd << " " << fNumT0Hits2ndUpDown << " " << fT0SegmentHitFirst << " " << fT0SegmentHit2nd << " " 
	            << fT0SumSigUpFirst << " " << fT0SumSigDownFirst << " " << fT0SumSigUp2nd << " " << fT0SumSigDown2nd 
		    << " " << fTdcUniqueSegmentBottom << " " << fTdcUniqueSegmentTop;
       }
	
       if (fMakeEventSummaryNTuple) fFOutTrigT0Final << std::endl;
       std::unique_ptr<rb::TrigToT0>
         aTrigToT0Ptr(new rb::TrigToT0(anPeakTrig, asumPMTTrig, anumSegT0, asegT0, asumSigUpT0, asumSigDownT0));
       evt.put(std::move(aTrigToT0Ptr));
    }
    //......................................................................
    void TrigToT0Prod::produce(art::Event& evt)
    {
      ++fNEvents;
      this->resetAllADCsTDCs();
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();  
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
      if (fSubRun == 1) {
       std::unique_ptr<rb::TrigToT0>
         aTrigToT0Ptr(new rb::TrigToT0(0, 0., 0, INT_MAX, 0., 0.));
        evt.put(std::move(aTrigToT0Ptr));
        return; // Spill first is ignore, but in artdaq. 
      }
      if (fEvtNum < 3) std::cerr << " Event " << fEvtNum << " No time available "  <<  std::endl;
      
      bool gotT0 = false;   
      std::string labelStr;
      

      for (int i=0; i<emph::geo::NDetectors; ++i) {

	labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
//	std::cerr << " At label " << labelStr << std::endl; 
	art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	try {
	  evt.getByLabel(labelStr, wfHandle);

	  if (!wfHandle->empty()) {
//	    if ((fRun == 1365) && (i == emph::geo::T0)) std::cerr << " Got Wave form for event  " 
//		              << fEvtNum << " spill " << fSubRun << " label " << labelStr << std::endl;
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
		  if (fRun == 1365) { 
  		    if (fEvtNum < 10) std::cerr << "**No TRB3 digits found for the T0!, still, studying ADC's for event  " 
		              << fEvtNum << " spill " << fSubRun << std::endl;
		    FillT0Plots(wfHandle, trbHandle); // We still have the ADC to study.. 
		 }
//                  fNoT0Info[0]++;  // There seem to be nocase where the acces to trbHandle succeed, but it is empty.. 
		}
	      }
	      catch(...) {
//		std::cerr << "No TRB3 digits found for the T0!, catch " << std::endl;
		  if (fRun == 1365) { 
  		    if (fEvtNum < 10) std::cerr << "**No TRB3 digits found for the T0!, still, studying ADC's for event  " 
		              << fEvtNum << " spill " << fSubRun << std::endl;
		    FillT0Plots(wfHandle, trbHandle); // We still have the ADC to study.. 
		 }
                  fNoT0Info++;
	      }
	    }

	  }
	}
	catch(...) {
	  //	  std::cout << "Nothing found in " << labelStr << std::endl; 
	}
      }
      if (gotT0 || (fRun == 1365)) { 
        int iTrigger = emph::geo::Trigger;

	labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(iTrigger));
	art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	try {
	    evt.getByLabel(labelStr, wfHandle);
	    if (!wfHandle->empty()) { 
	      FillTrigPlots(wfHandle); 
	    }
         } catch(...) { 
	    std::cerr << " Error in uploading Trigger wave forms .. " << std::endl; 
            std::unique_ptr<rb::TrigToT0>
                aTrigToT0Ptr(new rb::TrigToT0(0, 0., 0, INT_MAX, 0., 0.));
            evt.put(std::move(aTrigToT0Ptr));
	    return; 
	 } 
         this->FillTrigT0Final(evt, gotT0); // this will do the move.. 
	 return;
      }
      std::unique_ptr<rb::TrigToT0>
          aTrigToT0Ptr(new rb::TrigToT0(0, 0., 0, INT_MAX, 0., 0.));
      evt.put(std::move(aTrigToT0Ptr));
      
      
    }
    
//
// 
    void TrigToT0Prod::dumpWaveForm(emph::geo::tDetectorType aType, int detchan,  std::string &polarityStr, std::vector<uint16_t> &tmpwf) 
    {
      std::string dirDump("?");
      switch (aType) {
        case emph::geo::T0 :
	  dirDump=std::string("./T0WaveForms/");
	  break;
	case emph::geo::Trigger :
	  dirDump=std::string("./TriggerWaveForms/");
	  break;
	default:
	  return;
      }
      std::ostringstream fWvOutStrStr;
      fWvOutStrStr << dirDump << "WaveForm_" << polarityStr << "_" << detchan << "_Run_" << fRun << "_Spill" << fSubRun << "_evt_" << fEvtNum << ".txt";
      std::string fWvOutStr( fWvOutStrStr.str());
      std::ofstream fWvOut(fWvOutStr.c_str()); fWvOut << " k adc " << std::endl;
      for (size_t k=0; k != tmpwf.size(); k++) fWvOut << " " << k << " " << tmpwf[k] << std::endl;
      fWvOut.close();
    }
    
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::tof::TrigToT0Prod)
