////////////////////////////////////////////////////////////////////////
/// \brief   Module to readout BACkov, Gas Cherenkov Waveforms
/// \author  $Author: jmirabit $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>

// ROOT includes
#include "TFile.h"
#include "TTree.h"

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
#include "RawData/WaveForm.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace onmon {
    class BACkovReadout : public art::EDAnalyzer {
    public:
      explicit BACkovReadout(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~BACkovReadout();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
      void   FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &,art::Handle< std::vector<rawdata::WaveForm> > &,art::Handle< std::vector<rawdata::WaveForm> > &,art::Handle< std::vector<rawdata::WaveForm> > &);
      
      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;

      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanCal = 9;
      static const unsigned int nChanBACkov = 6; 
      
      // define histograms
      std::vector<unsigned int> fNEventsBACkov;


 
      bool fMakeBACkovPlots;
		TFile* f; 
		TTree *tree;
		int event;
		float Bq0, Bq1, Bq2, Bq3, Bq4, Bq5;
		float Bmax0, Bmax1, Bmax2, Bmax3, Bmax4, Bmax5;
		float Bblw0, Bblw1, Bblw2, Bblw3, Bblw4, Bblw5;
		float Tq5, Tq6, Tq15, Tq16;
		float Tmax5, Tmax6, Tmax15, Tmax16;
		float Tblw5, Tblw6, Tblw15, Tblw16;
		float Gq0, Gq1, Gq2;
		float Gmax0, Gmax1, Gmax2;
		float Gblw0, Gblw1, Gblw2;
		float LGq0, LGq1, LGq2, LGq3, LGq4, LGq5, LGq6, LGq7, LGq8;
		float LGblw0, LGblw1, LGblw2, LGblw3, LGblw4, LGblw5, LGblw6, LGblw7, LGblw8;
		
    };

    //.......................................................................
    BACkovReadout::BACkovReadout(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset)
    {

      this->reconfigure(pset);

    }

    //......................................................................
    BACkovReadout::~BACkovReadout()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void BACkovReadout::reconfigure(const fhicl::ParameterSet& pset)
    {
      fChanMapFileName = pset.get<std::string>("channelMapFileName","");
      fMakeBACkovPlots = pset.get<bool>("makeBACkovPlots",false);
      
    }

    //......................................................................
    void BACkovReadout::beginJob()
    {
      fNEvents=0;
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
      char filename[32];
      sprintf(filename,"BACkov.root");
      f = new TFile(filename,"RECREATE");

      		tree=new TTree("BACkovtree","");

		tree->Branch("event",&event);

		tree->Branch("BAC_q0",&Bq0);
		tree->Branch("BAC_q1",&Bq1);
		tree->Branch("BAC_q2",&Bq2);
		tree->Branch("BAC_q3",&Bq3);
		tree->Branch("BAC_q4",&Bq4);
		tree->Branch("BAC_q5",&Bq5);
		tree->Branch("BAC_max0",&Bmax0);
		tree->Branch("BAC_max1",&Bmax1);
		tree->Branch("BAC_max2",&Bmax2);
		tree->Branch("BAC_max3",&Bmax3);
		tree->Branch("BAC_max4",&Bmax4);
		tree->Branch("BAC_max5",&Bmax5);
		tree->Branch("BAC_blw0",&Bblw0);
		tree->Branch("BAC_blw1",&Bblw1);
		tree->Branch("BAC_blw2",&Bblw2);
		tree->Branch("BAC_blw3",&Bblw3);
		tree->Branch("BAC_blw4",&Bblw4);
		tree->Branch("BAC_blw5",&Bblw5);

		tree->Branch("T0_q5",&Tq5);
		tree->Branch("T0_max5",&Tmax5);
		tree->Branch("T0_blw5",&Tblw5);
		tree->Branch("T0_q6",&Tq6);
		tree->Branch("T0_max6",&Tmax6);
		tree->Branch("T0_blw6",&Tblw6);
		tree->Branch("T0_q15",&Tq15);
		tree->Branch("T0_max15",&Tmax15);
		tree->Branch("T0_blw15",&Tblw15);
		tree->Branch("T0_q16",&Tq16);
		tree->Branch("T0_max16",&Tmax16);
		tree->Branch("T0_blw16",&Tblw16);
		
		tree->Branch("GC_q0",&Gq0);
		tree->Branch("GC_max0",&Gmax0);
		tree->Branch("GC_blw0",&Gblw0);
		tree->Branch("GC_q1",&Gq1);
		tree->Branch("GC_max1",&Gmax1);
		tree->Branch("GC_blw1",&Gblw1);
		tree->Branch("GC_q2",&Gq2);
		tree->Branch("GC_max2",&Gmax2);
		tree->Branch("GC_blw2",&Gblw2);

		tree->Branch("LG_q0",&LGq0);
		tree->Branch("LG_q1",&LGq1);
		tree->Branch("LG_q2",&LGq2);
		tree->Branch("LG_q3",&LGq3);
		tree->Branch("LG_q4",&LGq4);
		tree->Branch("LG_q5",&LGq5);
		tree->Branch("LG_q6",&LGq6);
		tree->Branch("LG_q7",&LGq7);
		tree->Branch("LG_q8",&LGq8);
		tree->Branch("LG_blw0",&LGblw0);
		tree->Branch("LG_blw1",&LGblw1);
		tree->Branch("LG_blw2",&LGblw2);
		tree->Branch("LG_blw3",&LGblw3);
		tree->Branch("LG_blw4",&LGblw4);
		tree->Branch("LG_blw5",&LGblw5);
		tree->Branch("LG_blw6",&LGblw5);
		tree->Branch("LG_blw7",&LGblw5);
		tree->Branch("LG_blw8",&LGblw5);
	    }

    //......................................................................
    void BACkovReadout::endJob()
    {
      char filename[32];
      sprintf(filename,"BACkov_r%d_s%d.root",fRun,fSubrun);

      std::cout<<"Writing file for run/subrun: " << fRun << "/" <<fSubrun << std::endl;
      std::rename("BACkov.root",filename);      
      tree->Write();
      f->Close();
      delete f; f=0;
    }
    
    //......................................................................

    void BACkovReadout::FillBACkovPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH, art::Handle< std::vector<emph::rawdata::WaveForm> > & TwvfmH, art::Handle< std::vector<emph::rawdata::WaveForm> > & GwvfmH,art::Handle< std::vector<emph::rawdata::WaveForm> > & LGwvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      emph::cmap::EChannel Techan;
      emph::cmap::EChannel Gechan;
      emph::cmap::EChannel LGechan;
      echan.SetBoardType(boardType);
      Techan.SetBoardType(boardType);
      Gechan.SetBoardType(boardType);
      LGechan.SetBoardType(boardType);
      if (fMakeBACkovPlots) {
	if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    event = fNEvents;
            echan.SetBoard(board);
            echan.SetChannel(chan);
            emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
	    //std::cout<<"BACkov Board, Chan, detchan : "<<board<<", "<<chan<<", "<<detchan<<std::endl;
	    if (detchan >=0 && detchan < nchan){
	      if (detchan==0){
	        Bq0 = wvfm.BACkovCharge(); Bmax0 = wvfm.Baseline()-wvfm.PeakADC(); Bblw0 = wvfm.BLWidth();
	      }
	      if (detchan==1){
	        Bq1 = wvfm.BACkovCharge(); Bmax1 = wvfm.Baseline()-wvfm.PeakADC(); Bblw1 = wvfm.BLWidth();
	      }
	      if (detchan==2){
	        Bq2 = wvfm.BACkovCharge(); Bmax2 = wvfm.Baseline()-wvfm.PeakADC(); Bblw2 = wvfm.BLWidth();
	      }
	      if (detchan==3){
	        Bq3 = wvfm.BACkovCharge(); Bmax3 = wvfm.Baseline()-wvfm.PeakADC(); Bblw3 = wvfm.BLWidth();
	      }
	      if (detchan==4){
	        Bq4 = wvfm.BACkovCharge(); Bmax4 = wvfm.Baseline()-wvfm.PeakADC(); Bblw4 = wvfm.BLWidth();
	      }
	      if (detchan==5){
	        Bq5 = wvfm.BACkovCharge(); Bmax5 = wvfm.Baseline()-wvfm.PeakADC(); Bblw5 = wvfm.BLWidth();
	      }
	    }

	  }
	  for (size_t idx=0; idx<TwvfmH->size(); ++idx){
	    const rawdata::WaveForm& Twvfm = (*TwvfmH)[idx];
	    int Tchan = Twvfm.Channel();
	    int Tboard = Twvfm.Board();
            Techan.SetBoard(Tboard);
            Techan.SetChannel(Tchan);
            emph::cmap::DChannel Tdchan = fChannelMap->DetChan(Techan);
            int Tdetchan = Tdchan.Channel();
	    //std::cout<<"T0 Board, Chan, detchan : "<<Tboard<<", "<<Tchan<<", "<<Tdetchan<<std::endl;
            if (Tdetchan==5){
	      Tq5 = Twvfm.Charge(); Tmax5 = Twvfm.Baseline()-Twvfm.PeakADC(); Tblw5 = Twvfm.BLWidth();
	    }
	    if (Tdetchan==6){
	      Tq6 = Twvfm.Charge(); Tmax6 = Twvfm.Baseline()-Twvfm.PeakADC(); Tblw6 = Twvfm.BLWidth();	
	    }
	    if (Tdetchan==15){
	      Tq15 = Twvfm.Charge(); Tmax15 = Twvfm.Baseline()-Twvfm.PeakADC(); Tblw15 = Twvfm.BLWidth();	
	    }
	    if (Tdetchan==16){
	      Tq16 = Twvfm.Charge(); Tmax16 = Twvfm.Baseline()-Twvfm.PeakADC(); Tblw16 = Twvfm.BLWidth();	
	    }
	  } 

	  for (size_t idx=0; idx<GwvfmH->size(); ++idx){
	    const rawdata::WaveForm& Gwvfm = (*GwvfmH)[idx];
	    int Gchan = Gwvfm.Channel();
	    int Gboard = Gwvfm.Board();
            Gechan.SetBoard(Gboard);
            Gechan.SetChannel(Gchan);
            emph::cmap::DChannel Gdchan = fChannelMap->DetChan(Gechan);
            int Gdetchan = Gdchan.Channel();
	    //std::cout<<"GC Board, Chan, detchan : "<<Gboard<<", "<<Gchan<<", "<<Gdetchan<<std::endl;
            if (Gdetchan==0){
	      Gq0 = Gwvfm.Charge(); Gmax0 = Gwvfm.Baseline()-Gwvfm.PeakADC(); Gblw0 = Gwvfm.BLWidth();
	    }
	    if (Gdetchan==1){
	      Gq1 = Gwvfm.Charge(0,10,60,47); Gmax1 = Gwvfm.Baseline()-Gwvfm.PeakADC(); Gblw1 = Gwvfm.BLWidth();
	    }
	    if (Gdetchan==2){
	      Gq2 = Gwvfm.Charge(0,10,60,47); Gmax2 = Gwvfm.Baseline()-Gwvfm.PeakADC(); Gblw2 = Gwvfm.BLWidth();
	    }
	  } 

          for (size_t idx=0; idx<LGwvfmH->size(); ++idx){
	    const rawdata::WaveForm& LGwvfm = (*LGwvfmH)[idx];
	    int LGchan = LGwvfm.Channel();
	    int LGboard = LGwvfm.Board();
            LGechan.SetBoard(LGboard);
            LGechan.SetChannel(LGchan);
            emph::cmap::DChannel LGdchan = fChannelMap->DetChan(LGechan);
            int LGdetchan = LGdchan.Channel();
            if (LGdetchan==0){
	      LGq0 = LGwvfm.Charge();  LGblw0 = LGwvfm.BLWidth();
	    }
	    if (LGdetchan==1){
	      LGq1 = LGwvfm.Charge(); LGblw1 = LGwvfm.BLWidth();
	    }
	    if (LGdetchan==2){
	      LGq2 = LGwvfm.Charge();  LGblw2 = LGwvfm.BLWidth();
	    }
	    if (LGdetchan==3){
	      LGq3 = LGwvfm.Charge();  LGblw3 = LGwvfm.BLWidth();
	    }
	    if (LGdetchan==4){
	      LGq4 = LGwvfm.Charge();  LGblw4 = LGwvfm.BLWidth();
	    }  
	    if (LGdetchan==5){
	      LGq5 = LGwvfm.Charge();  LGblw5 = LGwvfm.BLWidth();
	    } 
	    if (LGdetchan==6){
	      LGq6 = LGwvfm.Charge();  LGblw6 = LGwvfm.BLWidth();
	    }
	    if (LGdetchan==7){
	      LGq7 = LGwvfm.Charge();  LGblw7 = LGwvfm.BLWidth();
	    }  
	    if (LGdetchan==8){
	      LGq8 = LGwvfm.Charge();  LGblw8 = LGwvfm.BLWidth();
	    } 


	  }	



	  tree->Fill();
      }
    }
	 }
    
    //......................................................................

    void BACkovReadout::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      fRun = evt.run();
      fSubrun = evt.subRun();     
      std::string labelStrBAC, labelStrT0, labelStrGC, labelStrLG;

      // get WaveForm
      int i = emph::geo::BACkov;
      labelStrBAC = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandleBAC;
      
      int j = emph::geo::T0;
      labelStrT0 = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(j));
      art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandleT0;

      int k = emph::geo::GasCkov;
      labelStrGC = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(k));
      art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandleGC;

      int l = emph::geo::LGCalo;
      labelStrLG = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(l));
      art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandleLG;


      try {
	evt.getByLabel(labelStrBAC, wfHandleBAC);
	evt.getByLabel(labelStrT0, wfHandleT0);
	evt.getByLabel(labelStrGC, wfHandleGC);
	evt.getByLabel(labelStrLG, wfHandleLG);

	if (!wfHandleBAC->empty()) {
	  FillBACkovPlots(wfHandleBAC, wfHandleT0, wfHandleGC, wfHandleLG);
	}
      }
      catch(...) {

      }

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::BACkovReadout)
