////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create online monitoring plots
/// \author  $Author: jpaley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

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
      void   FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &,art::Handle< std::vector<rawdata::WaveForm> > &,art::Handle< std::vector<rawdata::WaveForm> > &);
      
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
		int event, chan, Tchan, Gchan;
		float Bq0, Bq1, Bq2, Bq3, Bq4, Bq5;
		float Bmax0, Bmax1, Bmax2, Bmax3, Bmax4, Bmax5;
		float Bblw0, Bblw1, Bblw2, Bblw3, Bblw4, Bblw5;
		float Tq9, Tq10, Tq11, Tq12;
		float Tmax9, Tmax10, Tmax11, Tmax12;
		float Tblw9, Tblw10, Tblw11, Tblw12;
		float Gq0, Gq1, Gq2;
		float Gmax0, Gmax1, Gmax2;
		float Gblw0, Gblw1, Gblw2;
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

		tree->Branch("T0_q9",&Tq9);
		tree->Branch("T0_max9",&Tmax9);
		tree->Branch("T0_blw9",&Tblw9);
		tree->Branch("T0_q10",&Tq10);
		tree->Branch("T0_max10",&Tmax10);
		tree->Branch("T0_blw10",&Tblw10);
		tree->Branch("T0_q11",&Tq11);
		tree->Branch("T0_max11",&Tmax11);
		tree->Branch("T0_blw11",&Tblw11);
		tree->Branch("T0_q12",&Tq12);
		tree->Branch("T0_max12",&Tmax12);
		tree->Branch("T0_blw12",&Tblw12);
		
		tree->Branch("GC_q0",&Gq0);
		tree->Branch("GC_max0",&Gmax0);
		tree->Branch("GC_blw0",&Gblw0);
		tree->Branch("GC_q1",&Gq1);
		tree->Branch("GC_max1",&Gmax1);
		tree->Branch("GC_blw1",&Gblw1);
		tree->Branch("GC_q2",&Gq2);
		tree->Branch("GC_max2",&Gmax2);
		tree->Branch("GC_blw2",&Gblw2);
    }
    
    //......................................................................

    void BACkovReadout::endJob()
    {

		tree->Write();
      f->Close();
      delete f; f=0;
    }
    
    //......................................................................

    void BACkovReadout::FillBACkovPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH, art::Handle< std::vector<emph::rawdata::WaveForm> > & TwvfmH, art::Handle< std::vector<emph::rawdata::WaveForm> > & GwvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      emph::cmap::EChannel Techan;
      emph::cmap::EChannel Gechan;
      echan.SetBoardType(boardType);
      Techan.SetBoardType(boardType);
      Gechan.SetBoardType(boardType);
      if (fMakeBACkovPlots) {
        //std::cout << "Making BACkov ADC OnMon plots" << std::endl;
        for (int i=0; i<nchan; ++i) {
          //sprintf(hname,"BACkovADC_%d",i);
          //fBACkovADCDist[i] = h.GetTH1F(hname);
        }
        //std::cout << "Making BACkov WaveForm OnMon plots" << std::endl;
        for (int i=0; i<nchan; ++i) {
          //sprintf(hname,"BACkovWaveForm_%d",i);
          //fBACkovWaveForm.push_back(h.GetTH1F(hname));
          //fBACkovWaveForm[i]->SetBit(TH1::kIsAverage);
          fNEventsBACkov.push_back(0);
          }

	if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    chan = wvfm.Channel();
	    int board = wvfm.Board();
	    event = fNEvents;
            echan.SetBoard(board);
            echan.SetChannel(chan);
            emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
            int detchan = dchan.Channel();
            if (detchan >= 0 && detchan < nchan) {
	      if (detchan==0){
		Bq0 = wvfm.Charge();
	        Bmax0 = wvfm.Baseline()-wvfm.PeakADC();
	        Bblw0 = wvfm.BLWidth();
	      }
	      if (detchan==1){
		Bq1 = wvfm.Charge();
	        Bmax1 = wvfm.Baseline()-wvfm.PeakADC();
	        Bblw1 = wvfm.BLWidth();
	      }
	      if (detchan==2){
		Bq2 = wvfm.Charge();
	        Bmax2 = wvfm.Baseline()-wvfm.PeakADC();
	        Bblw2 = wvfm.BLWidth();
	      }
	      if (detchan==3){
		Bq3 = wvfm.Charge();
	        Bmax3 = wvfm.Baseline()-wvfm.PeakADC();
	        Bblw3 = wvfm.BLWidth();
	      }
	      if (detchan==4){
		Bq4 = wvfm.Charge();
	        Bmax4 = wvfm.Baseline()-wvfm.PeakADC();
	        Bblw4 = wvfm.BLWidth();
	      }
	      if (detchan==5){
		Bq5 = wvfm.Charge();
	        Bmax5 = wvfm.Baseline()-wvfm.PeakADC();
	        Bblw5 = wvfm.BLWidth();
	      } 
            }

	  }
	  for (size_t idx=0; idx<TwvfmH->size(); ++idx){
	    const rawdata::WaveForm& Twvfm = (*TwvfmH)[idx];
	    Tchan = Twvfm.Channel();
	    int Tboard = Twvfm.Board();
            Techan.SetBoard(Tboard);
            Techan.SetChannel(Tchan);
            emph::cmap::DChannel Tdchan = fChannelMap->DetChan(Techan);
            int Tdetchan = Tdchan.Channel();
            if (Tdetchan==9){
	      Tq9 = Twvfm.Charge();
	      Tmax9 = Twvfm.Baseline()-Twvfm.PeakADC();
	      Tblw9 = Twvfm.BLWidth();
	      std::cout<<"Tboard is: "<<Tboard<<"     and Tchan is: "<<Tchan<<std::endl;	
	    }
	    if (Tdetchan==10){
	      Tq10 = Twvfm.Charge();
	      Tmax10 = Twvfm.Baseline()-Twvfm.PeakADC();
	      Tblw10 = Twvfm.BLWidth();	
	    }
	    if (Tdetchan==11){
	      Tq11 = Twvfm.Charge();
	      Tmax11 = Twvfm.Baseline()-Twvfm.PeakADC();
	      Tblw11 = Twvfm.BLWidth();	
	    }
	    if (Tdetchan==12){
	      Tq12 = Twvfm.Charge();
	      Tmax12 = Twvfm.Baseline()-Twvfm.PeakADC();
	      Tblw12 = Twvfm.BLWidth();	
	    }
	  } 
	  for (size_t idx=0; idx<GwvfmH->size(); ++idx){
	    const rawdata::WaveForm& Gwvfm = (*GwvfmH)[idx];
	    Gchan = Gwvfm.Channel();
	    int Gboard = Gwvfm.Board();
            Gechan.SetBoard(Gboard);
            Gechan.SetChannel(Gchan);
            emph::cmap::DChannel Gdchan = fChannelMap->DetChan(Gechan);
            int Gdetchan = Gdchan.Channel();
            if (Gdetchan==0){
	      Gq0 = Gwvfm.Charge();
;	      Gmax0 = Gwvfm.Baseline()-Gwvfm.PeakADC();
	      Gblw0 = Gwvfm.BLWidth();
	    }
	    if (Gdetchan==1){
	      Gq1 =  Gwvfm.Charge();
	      Gmax1 = Gwvfm.Baseline()-Gwvfm.PeakADC();
	      Gblw1 = Gwvfm.BLWidth();
	    }
	    if (Gdetchan==2){
	      Gq2 = Gwvfm.Charge();
	      Gmax2 = Gwvfm.Baseline()-Gwvfm.PeakADC();
	      Gblw2 = Gwvfm.BLWidth();
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
      std::string labelStrBAC, labelStrT0, labelStrGC;

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

      try {
	evt.getByLabel(labelStrBAC, wfHandleBAC);
	evt.getByLabel(labelStrT0, wfHandleT0);
	evt.getByLabel(labelStrGC, wfHandleGC);

	if (!wfHandleBAC->empty()) {
	  FillBACkovPlots(wfHandleBAC, wfHandleT0, wfHandleGC);
	}
      }
      catch(...) {

      }

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::BACkovReadout)
