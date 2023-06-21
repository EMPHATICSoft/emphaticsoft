////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to study LG Calorimeter
/// \author  $Author: robert chirco $
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
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "TOF/PeakInWaveForm.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace lgcalo {
  
    class LGCaloAna : public art::EDAnalyzer {
    public:
      explicit LGCaloAna(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~LGCaloAna();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void beginRun(art::Run const& run);
      void endSubRun(art::SubRun const&);
      void endJob();

    private:
    
      void  resetAllADCsTDCs();
      
      void  FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      
      void  FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void  MakeLGCaloPlots();
 
      static const unsigned int kSSDOffset = 1;
      static const unsigned int kARICHOffset = kSSDOffset+22;
      static const unsigned int kLGCaloOffset = kARICHOffset+9;
      static const unsigned int nChanCal = 9;

      TH1F* fLGCaloADCDist[nChanCal];
      std::vector<unsigned int> fNEventsLGCalo; 
      std::vector<TH1F*> fLGCaloWaveForm;
      std::vector<TH1F*> fLGCaloIntCharge;
      TH1F* fLGCaloTotalCharge;
      TH1F* fLGCaloCenterRatio;
      TH1F* fLGCaloEdgeRatio;
      TH1F* fLGCaloSignalRatio;
      TH2F*  fLGCaloIntChgVsCenterRatio;
      TH2F*  fLGCaloIntChgVsEdgeRatio;
      void fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > &);
      bool want;

      //bool fFilesAreOpen;
      //std::string fTokenJob;
      emph::cmap::ChannelMap* fChannelMap;
      runhist::RunHistory* fRunHistory;
      //std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubRun;
      //int fPrevSubRun;
      unsigned int fEvtNum;
      //int fPrevEvtNum;
      unsigned int fNEvents;
      
      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanT0  = 20;
      static const unsigned int nChanRPC = 16;
      static const unsigned int nChanTrig = 4;
      
//hi this is robert speaking idk if we need most of these...but i'm not sure
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
     //bool fMakeT0FullNtuple; 
     //bool fMakeRPCFullNtuple;
     //bool fMakeTrigFullNtuple;
     //bool fMakeEventSummaryNTuple;
     
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
      std::ofstream fFOutRPC; 
      std::ofstream fFOutTrigger; 
      std::ofstream fFOutTrigT0RPC; 
      
      void dumpWaveForm(emph::geo::tDetectorType aType, int detchan, std::string &polarityStr, std::vector<uint16_t> &wf); 

    };    
    //.......................................................................
    LGCaloAna::LGCaloAna(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
      //fFilesAreOpen(false),
      //fTokenJob("none"),
      //fChanMapFileName("Unknwon"), fRun(0), fSubRun(0), fPrevSubRun(-1), fEvtNum(0), fPrevEvtNum(-1), 
      fT0ADCs(nChanT0+2, 0.),
      fT0TDCs(nChanT0+2, DBL_MAX),
      fT0TDCsFrAdc(nChanT0+2, DBL_MAX),
      fRPCTDCs(2*nChanRPC+1, DBL_MAX),
      fRPCHiLows(2*nChanRPC+1, false),
      fRPCEChans(2*nChanRPC+1, INT_MAX), // used only for algorithm consistency..
      fTrigPeakADCs(nChanTrig, 0.),
      fTrigADCs(nChanTrig, 0.),
      fMakeT0FullNtuple(true),
      fMakeRPCFullNtuple(true),
      fMakeTrigFullNtuple(true),
      fMakeEventSummaryNTuple(false),
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
      
      fLGCaloADCDist[nChanCal](nullptr); //need to specify all 9?
      fNEventsLGCalo(0);
      fLGCaloWaveForm(nullptr);
      want(false);

    {

      this->reconfigure(pset);

    }
    
    //......................................................................
    LGCaloAna::~LGCaloAna()
    {
    
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutT0Prof.is_open()) fFOutT0Prof.close();
      if (fFOutRPC.is_open()) fFOutRPC.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0RPC.is_open()) fFOutTrigT0RPC.close();
      
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void LGCaloAna::reconfigure(const fhicl::ParameterSet& pset)
    {
      //fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      //fMakeT0FullNtuple  = pset.get<bool>("makeT0FullNtuple",true); // keep them for now.. 
      //fMakeRPCFullNtuple = pset.get<bool>("makeRPCFullNtuple",true);
      //fMakeTrigFullNtuple = pset.get<bool>("makeTrigFullNtuple",true);
      //fMakeEventSummaryNTuple = pset.get<bool>("makeEventSummaryFullNtuple",false);
      
    }

    //......................................................................
    void LGCaloAna::beginJob()
    {
      fNEvents= 0;
      
      HistoSet& h = HistoSet::Instance();
      fLGCaloIntChgVsCenterRatio = h.GetTH2F("LGCaloIntChgVsCenterRatio");
      fLGCaloIntChgVsEdgeRatio = h.GetTH2F("LGCaloIntChgVsEdgeRatio"); 
      fNTriggerLGArray     = h.GetTH2F("NTriggerLGArray");
      MakeLGCaloPlots();
      //art::ServiceHandle<art::TFileService> tfs;
    }
    //......................................................................
    void LGCaloAna::beginRun(art::Run const& run)
    {
      // initialize channel map
      fChannelMap = new emph::cmap::ChannelMap();
      fRunHistory = new runhist::RunHistory(run.run());
      fChannelMap->LoadMap(fRunHistory->ChanFile());
    }

    //......................................................................
    void OnMonPlotter::endSubRun(const art::SubRun&)
    {
      std::cout<<"Writing file for run/subrun: " << fRun << "/" << fSubrun << std::endl;
      char filename[32];
      sprintf(filename,"lgcaloana_r%d_s%d.root", fRun, fSubrun);
      TFile* f = new TFile(filename,"RECREATE");
      HistoSet::Instance().WriteToRootFile(f);
      f->Close();
      delete f; f=0;

    }

    //......................................................................
    //......................................................................

    void LGCaloAna::endJob()
    {
      if (fFOutT0.is_open()) fFOutT0.close();
      if (fFOutRPC.is_open()) fFOutRPC.close();
      if (fFOutTrigger.is_open()) fFOutTrigger.close();
      if (fFOutTrigT0RPC.is_open()) fFOutTrigT0RPC.close();

      if (fNEvents > 0) {
        float scale = 1./float(fNEvents);
      for (size_t i=0; i<fLGCaloWaveForm.size(); ++i) {
          scale = 1./float(fNEventsLGCalo[i]);
          fLGCaloWaveForm[i]->Scale(scale);
        }
      }
      
     fNTriggerLGArray->SetBinContent(1.5,1.5,fNEventsLGCalo[6]);
     fNTriggerLGArray->SetBinContent(2.5,2.5,fNEventsLGCalo[4]);
     fNTriggerLGArray->SetBinContent(3.5,3.5,fNEventsLGCalo[2]);
     fNTriggerLGArray->SetBinContent(1.5,3.5,fNEventsLGCalo[0]);
     fNTriggerLGArray->SetBinContent(2.5,3.5,fNEventsLGCalo[1]);
     fNTriggerLGArray->SetBinContent(1.5,2.5,fNEventsLGCalo[3]);
     fNTriggerLGArray->SetBinContent(3.5,2.5,fNEventsLGCalo[5]);
     fNTriggerLGArray->SetBinContent(2.5,1.5,fNEventsLGCalo[7]);
     fNTriggerLGArray->SetBinContent(3.5,1.5,fNEventsLGCalo[8]);

      char filename[32];
      sprintf(filename,"lgcaloana_r%d_s%d.root", fRun, fSubrun);
      TFile* f = new TFile(filename,"RECREATE");
      HistoSet::Instance().WriteToRootFile(f);
      f->Close();
      delete f; f=0;
      //if(fIPC)    { delete fIPC; fIPC = 0; }

    }

    //......................................................................

    void  LGCaloAna::MakeLGCaloPlots()
    {
      HistoSet& h = HistoSet::Instance();

      int nchannel = emph::geo::DetInfo::NChannel(emph::geo::LGCalo);
      char hname[256];
      if (fMakeWaveFormPlots) {
        std::cout << "Making LGCalo ADC OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"LGCaloADC_%d",i);
          fLGCaloADCDist[i] = h.GetTH1F(hname);
        }
        std::cout << "Making LGCalo WaveForm OnMon plots" << std::endl;
        for (int i=0; i<nchannel; ++i) {
          sprintf(hname,"LGCaloWaveForm_%d",i);
          fLGCaloWaveForm.push_back(h.GetTH1F(hname));
          fLGCaloWaveForm[i]->SetBit(TH1::kIsAverage);

          sprintf(hname,"LGCaloIntCharge_%d",i);
          fLGCaloIntCharge.push_back(h.GetTH1F(hname));
          fNEventsLGCalo.push_back(0);
        }
        fLGCaloTotalCharge = h.GetTH1F("LGCaloTotalCharge");
        fLGCaloCenterRatio = h.GetTH1F("LGCaloCenterRatio");
        fLGCaloEdgeRatio   = h.GetTH1F("LGCaloEdgeRatio");
        fLGCaloSignalRatio   = h.GetTH1F("LGCaloSignalRatio");
      }
    }

    //......................................................................

    void    OnMonPlotter::fWantLGCalo(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
     int nchan = emph::geo::DetInfo::NChannel(emph::geo::Trigger);
     emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      if (fMakeWaveFormPlots) {
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
              if (adc > 5*blw){
                fTriggerADCDist[detchan]->Fill(adc);
                //std::cout<<adc<<std::endl;
		if (adc > 500) want=true; //return true;
                //std::cout<<"fWantLGCalo: "<<want<<std::endl;
              }
            }
          }
        }
      }
    }

    //......................................................................
    
    void    OnMonPlotter::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
      float c1=0; float e1=0; float t1=0;
      float s1=0; float b1=0;
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::LGCalo);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
      if (fMakeWaveFormPlots) {
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
              if (adc > 5*blw) {
                fHitEffPerChannel->Fill((int)dchan.DetId()+kLGCaloOffset,
                                        detchan);
                fLGCaloADCDist[detchan]->Fill(adc);
                //now fill waveform plot
                auto adcvals = wvfm.AllADC();
                fNEventsLGCalo[detchan]++;
                for (size_t i=0; i<adcvals.size(); ++i) {
                  fLGCaloWaveForm[detchan]->Fill(i+1,adcvals[i]);
                }
		// now fill integrated charge plot
		float x1=20; float nsamp=25; //range where the signal is
                float avg = 0; int ic = 0;
                float sum=0;
                for (size_t i=0; i<adcvals.size(); ++i){
                        if (i<size_t(x1) || i>size_t(x1+nsamp)) avg += float(adcvals[i]), ++ic;
                }

                avg /= float(ic); //baseline for each signal

                for (size_t i=x1; i<size_t(x1+nsamp) && i<adcvals.size(); ++i){
                         sum += (avg-adcvals[i]); //total integrated charge over the range
                }
                fLGCaloIntCharge[detchan]->Fill(sum);
                fLGCaloTotalCharge->Fill(sum);

                //now fill the energy ratio plots
		if (detchan==4) c1 += sum;
                if (detchan!=4) e1 += sum;
                t1 += sum;

                if (detchan==4){

                     if (sum >= 4000 && sum <= 9000){ s1 += sum; std::cout<<"sig"<<std::endl;}
                     if (sum >= 1000 && sum <= 2000){ b1 += sum; std::cout<<"back"<<std::endl;}
                }
                if (s1!=0 && b1!=0){
                  fLGCaloSignalRatio->Fill(s1/b1);
		 }
                if (e1!=0 && c1!=0){
                  fLGCaloEdgeRatio->Fill(e1/t1);
                  fLGCaloCenterRatio->Fill(c1/t1);
                  fLGCaloIntChgVsCenterRatio->Fill(c1/t1,sum);
                  fLGCaloIntChgVsEdgeRatio->Fill(e1/t1,sum);
                }
              }
            }
          }
        }
      }
    }

 
    //......................................................................
    
    void LGCaloAna::resetAllADCsTDCs() {
       fPeakTriggers.clear();
       fPeakT0s.clear();
       for(std::vector<double>::iterator it = fT0ADCs.begin(); it != fT0ADCs.end(); it++) *it = -1.0*DBL_MAX/2.;
       for(std::vector<double>::iterator it = fT0TDCs.begin(); it != fT0TDCs.end(); it++) *it = DBL_MAX;
       for(std::vector<double>::iterator it = fT0TDCsFrAdc.begin(); it != fT0TDCsFrAdc.end(); it++) *it = DBL_MAX;
       for(std::vector<double>::iterator it = fRPCTDCs.begin(); it != fRPCTDCs.end(); it++) *it = DBL_MAX;
       for(std::vector<bool>::iterator it = fRPCHiLows.begin(); it != fRPCHiLows.end(); it++) *it = false;
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

    //......................................................................
    //.....................................}.................................

    void   LGCaloAna::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
    {
//       std::cerr << " T0toRPC::FillTrigPlots tmp debug fRun " << fRun << " spill " << fSubRun << " evt " << fEvtNum << std::endl;
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
    if (debugIsOn) std::cerr << " Starting T0toRPC::FillTrigPlots, spill " << fSubRun << " evt " << fEvtNum << std::endl;
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
          emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
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
                std::cerr << " Autodebug  T0toRPC::FillTrigPlots, spill " << fSubRun << " evt " << fEvtNum << std::endl  
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
    //......................................................................
    void LGCaloAna::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      this->resetAllADCsTDCs();
      fRun = evt.run();
      fSubRun = evt.subRun(); 
      fEvtNum = evt.id().event();
//      art::Timestamp aTime = evt.time();
//      std::string aTimeStr = art::to_iso_string_assuming_unix_epoch(aTime);
      
      
//      if  ((fRun == 1365) && (fSubRun == 2) && (fEvtNum == 91)) {
//         IdentifyRawWaveformV1720T0Board1Chan7(evt);
//      } 
      
//      if (fEvtNum < 3) std::cerr << " Event " << fEvtNum << " time " << aTimeStr << " ....? " <<  std::endl;
      if (fEvtNum < 3) std::cerr << " Event " << fEvtNum << " No time available "  <<  std::endl;
      
      bool gotT0 = false; bool gotRPC = false;    
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

	    if (i == emph::geo::Trigger){FillTrigPlots(wfHandle); fWantLGCalo(wfHandle);} //FillTrigPlots(wfHandle);
            if (i == emph::geo::LGCalo){ //  FillLGCaloPlots(wfHandle);
	    if (want) FillLGCaloPlots(wfHandle);
            }


	  }
	}
	catch(...) {
	  //	  std::cout << "Nothing found in " << labelStr << std::endl; 
	}
      }
    }
    
//
    void LGCaloAna::dumpWaveForm(emph::geo::tDetectorType aType, int detchan,  std::string &polarityStr, std::vector<uint16_t> &tmpwf) 
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

DEFINE_ART_MODULE(emph::tof::LGCaloAna)
