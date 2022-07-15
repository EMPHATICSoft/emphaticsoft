////////////////////////////////////////////////////////////////////////
/// \brief   Module to create T0 analysis TTree
/// \author  $Author: kajikawa, jpaley $
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
#include "RawData/TRB3RawDigit.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
    class T0Ana : public art::EDAnalyzer {
    public:
      explicit T0Ana(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~T0Ana();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:

      bool   FindSeg(int ch_daq);
      bool   FindTrailBot(int ch_daq);
      bool   FindTrailTop(int ch_daq);
      bool   FindLeadBot(int ch_daq);
      bool   FindLeadTop(int ch_daq);
      int    GetSeg(int ch_daq);
      double GetTot(const TRB3RawDigit*);

      void   FillT0AnaTree(art::Handle< std::vector<rawdata::WaveForm> > &,
			   art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      
      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;
      
      TTree *tree;
      int event;
      float Tq[20];
      float Tmax[20];
      float Tblw[20];
      double Ttot[20];
    };

  //.......................................................................
  T0Ana::T0Ana(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {
    
    this->reconfigure(pset);

    // create TTree for TRB3RawDigits
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("T0AnaTree","");
    tree->Branch("event",&event);
    tree->Branch("q",Tq);
    tree->Branch("qmax",Tmax);
    tree->Branch("qblw",Tblw);
    tree->Branch("tot",Ttot);
  }

  //......................................................................
  T0Ana::~T0Ana()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }
  
  //......................................................................
  void T0Ana::reconfigure(const fhicl::ParameterSet& pset)
  {
    fChanMapFileName = pset.get<std::string>("channelMapFileName","");    
  }

    //......................................................................
  void T0Ana::beginJob()
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
  }
  
  //......................................................................
  void T0Ana::endJob()
  {
    
  }
  
  //......................................................................

  bool T0Ana::FindSeg(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 40) return true;

    return false;
  }


  //......................................................................
  int T0Ana::GetSeg(int ch_daq)
  {
    if(ch_daq ==  1 || ch_daq ==  2 || ch_daq == 21 || ch_daq == 22) return 0;
    if(ch_daq ==  3 || ch_daq ==  4 || ch_daq == 23 || ch_daq == 24) return 1;
    if(ch_daq ==  5 || ch_daq ==  6 || ch_daq == 25 || ch_daq == 26) return 2;
    if(ch_daq ==  7 || ch_daq ==  8 || ch_daq == 27 || ch_daq == 28) return 3;
    if(ch_daq ==  9 || ch_daq == 10 || ch_daq == 29 || ch_daq == 30) return 4;
    if(ch_daq == 11 || ch_daq == 12 || ch_daq == 31 || ch_daq == 32) return 5;
    if(ch_daq == 13 || ch_daq == 14 || ch_daq == 35 || ch_daq == 36) return 6;
    if(ch_daq == 15 || ch_daq == 16 || ch_daq == 33 || ch_daq == 34) return 7;
    if(ch_daq == 17 || ch_daq == 18 || ch_daq == 37 || ch_daq == 38) return 8;
    if(ch_daq == 19 || ch_daq == 20 || ch_daq == 39 || ch_daq == 40) return 9;
    return -1;
  }


  //......................................................................
  bool T0Ana::FindTrailBot(int ch_daq)
  {    
    if ((ch_daq < 21) && ((ch_daq%2)==0)) return true;
    return false;
  }

  //......................................................................
  bool T0Ana::FindTrailTop(int ch_daq) 
  {
    if ((ch_daq > 20) && ((ch_daq%2)==0)) return true;
    return false;
  }
  //......................................................................
  bool T0Ana::FindLeadBot(int ch_daq)
  {
    if ((ch_daq < 21) && ((ch_daq%2)==1)) return true;
    return false;
  }
  //......................................................................
  int T0Ana::FindLeadTop(int ch_daq)
  {
    if ((ch_daq > 20) && ((ch_daq%2)==1)) return true;
    return false;
  }

  //......................................................................
  double T0Ana::GetTot(const TRB3RawDigit* dig)
  {
    int n_seg = 10; // number of segment
  
    int RPC_board = 1283;
    int T0_board = 1282;
    double trb3LinearLowEnd = 15.0; // Calibration for low end
    double trb3LinearHighEnd = 499.0; // Calibration for FPGA2 -- T0
    double trb3LinearHighEnd_RPC = 491.0; // Calibration for FPGA3? -- RPC
    double max_diff = 0;
    int n_count = 0;
    int total_entry = 0;
    double gate_min = -10000000.0;

    double time_top_l;
    double time_bottom_l;
    double time_top_t;
    double time_bottom_t;
    bool found_top_l;
    bool found_bottom_l;
    bool found_top_t;
    bool found_bottom_t;

    for(int i_seg; i_seg < n_seg; i_seg++){
      time_top_l[i_seg]     = 0;
      time_bottom_l[i_seg]  = 0;
      time_top_t[i_seg]     = 0;
      time_bottom_t[i_seg]  = 0;
      found_top_l[i_seg]    = false;
      found_bottom_l[i_seg] = false;
      found_top_t[i_seg]    = false;
      found_bottom_t[i_seg] = false;
    }//for(i_seg:n_seg)

    int n_vector = headerWord->size();
    for(int i_vector = 0; i_vector < n_vector; i_vector++){
      if(headerWord->at(i_vector) == T0_board){
	int event_channel = channel->at(i_vector);
	if(event_channel==0){ // there is a trigger at the FPGA
	  found_trig = true;
	  time_trig = (double)epochtime->at(i_vector)*10240026.0 + (double)coarsetime->at(i_vector) * 5000.0 - (((double)finetime->at(i_vector) - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;  // this is the calibrated trigger time in ps
	}
	if(find_bottom_l(event_channel)){
	  seg_id = find_seg(event_channel);
	  if(!found_bottom_l[seg_id]){
	    found_bottom_l[seg_id] = true;
	    time_bottom_l[seg_id]  = (double)epochtime->at(i_vector)*10240026.0 + (double)coarsetime->at(i_vector) * 5000.0 - (((double)finetime->at(i_vector) - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;  // this is the calibrated T0 time in ps
	  }
	}
	if(find_top_l(event_channel)){
	  seg_id = find_seg(event_channel);
	  if(!found_top_l[seg_id]){
	    found_top_l[seg_id] = true;
	    time_top_l[seg_id]  = (double)epochtime->at(i_vector)*10240026.0 + (double)coarsetime->at(i_vector) * 5000.0 - (((double)finetime->at(i_vector) - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;  // this is the calibrated T0 time in ps
	  }
	}
	if(find_bottom_t(event_channel)){
	  seg_id = find_seg(event_channel);
	  if(!found_bottom_t[seg_id]){
	    found_bottom_t[seg_id] = true;
	    time_bottom_t[seg_id]  = (double)epochtime->at(i_vector)*10240026.0 + (double)coarsetime->at(i_vector) * 5000.0 - (((double)finetime->at(i_vector) - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;  // this is the calibrated T0 time in ps
	  }
	}
	if(find_top_t(event_channel)){
	  seg_id = find_seg(event_channel);
	  if(!found_top_t[seg_id]){
	    found_top_t[seg_id] = true;
	    time_top_t[seg_id]  = (double)epochtime->at(i_vector)*10240026.0 + (double)coarsetime->at(i_vector) * 5000.0 - (((double)finetime->at(i_vector) - trb3LinearLowEnd)/(trb3LinearHighEnd-trb3LinearLowEnd))*5000.0;  // this is the calibrated T0 time in ps
	  }
	}
      }//if(headerWord==T0)
    }//for(i_vector:n_vector)
    if(found_trig){
      for(int i_seg = 0; i_seg < n_seg; i_seg++){
	if(found_bottom_l[i_seg] && found_bottom_t[i_seg]){
	  h_tot_bottom[i_seg]->Fill(time_bottom_t[i_seg] - time_bottom_l[i_seg]);
	  h_tdc_tot_bottom[i_seg]->Fill(time_bottom_l[i_seg] - time_trig, time_bottom_l[i_seg] - time_bottom_t[i_seg]);
	}//if(found_bottom)
	if(found_top_l[i_seg] && found_top_t[i_seg]){
	  h_tot_top[i_seg]->Fill(time_top_t[i_seg] - time_top_l[i_seg]);
	  h_tdc_tot_top[i_seg]->Fill(time_top_l[i_seg] - time_trig, time_top_l[i_seg] - time_top_t[i_seg]);
	}//if(found_top)
      }//for(i_seg:n_seg)
    }//if(found_trig)
    found_trig   = false;
    time_trig  = 0;
    for(int i_seg = 0; i_seg < n_seg; i_seg++){
      found_top_l[i_seg]    = false;
      found_bottom_l[i_seg] = false;
      found_top_t[i_seg]    = false;
      found_bottom_t[i_seg] = false;
    
      time_top_l[i_seg]     = 0;
      time_bottom_l[i_seg]  = 0;
      time_top_t[i_seg]     = 0;
      time_bottom_t[i_seg]  = 0;
    }//for(i_seg:n_seg)
  }//for(i_entry:n_entry)
}//for()

    return -1.;
  }

  //......................................................................
  
  void T0Ana::FillT0AnaTree(art::Handle< std::vector<emph::rawdata::WaveForm> > & T0wvfm, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & T0trb3)
    {
      for (int i=0; i<20; ++i) {
	Tq[i] = -999999.;
	Tmax[i] = -9999.;
	Tblw[i] = -1.;	
	Ttot[i] = -999.;
      }
      
      // get ADC info for T0
      if (!T0wvfm->empty()) {
	emph::cmap::FEBoardType boardType = emph::cmap::V1720;
	emph::cmap::EChannel T0echan;
	T0echan.SetBoardType(boardType);
	// loop over ADC channels
	for (size_t idx=0; idx < T0wvfm->size(); ++idx) { 
	  const rawdata::WaveForm& wvfm = (*T0wvfm)[idx];
	  int chan = wvfm.Channel();
	  int board = wvfm.Board();
	  //	  event = fNEvents;
	  echan.SetBoard(board);
	  echan.SetChannel(chan);
	  emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	  int detchan = dchan.Channel();
	  if (detchan >0 && detchan <= 20){
	    Tq[detchan] = wvfm.Charge(); 
	    Tmax[detchan] = wvfm.Baseline()-wvfm.PeakADC(); 
	    Tblw[detchan] = wvfm.BLWidth();
	  }	  
	} // end loop over T0 ADC channels

      // get TDC info for T0
      if (!T0trb3->empty()) {
	emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
	emph::cmap::EChannel T0echan;
	T0echan.SetBoardType(boardType);
	// loop over ADC channels
	for (size_t idx=0; idx < T0trb3->size(); ++idx) { 
	  const rawdata::TRB3RawDigit& trb3 = (*T0trb3)[idx];
	  int chan = trb3.GetChannel();
	  int board = trb3.GetBoardId();
	  //	  event = fNEvents;
	  echan.SetBoard(board);
	  echan.SetChannel(chan);
	  emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	  int detchan = dchan.Channel();
	  if (detchan >0 && detchan <= 20){
	    Tq[detchan] = wvfm.Charge(); 
	    Tmax[detchan] = wvfm.Baseline()-wvfm.PeakADC(); 
	    Tblw[detchan] = wvfm.BLWidth();
	  }	  
	} // end loop over T0 ADC channels
	
	tree->Fill();
      }
    }
	 }
    
    //......................................................................

    void T0Ana::analyze(const art::Event& evt)
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

DEFINE_ART_MODULE(emph::T0Ana)
