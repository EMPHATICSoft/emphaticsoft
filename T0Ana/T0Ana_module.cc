////////////////////////////////////////////////////////////////////////
/// \brief   Module to create T0 analysis TTree
/// \author  $Author: kajikawa, jpaley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <array>
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
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/WaveForm.h"
#include "RawData/TRB3RawDigit.h"

// Define parameters of detectors
#define N_SEG_T0 10
#define N_CH_T0 41
#define N_CH_DET_T0 20
#define N_SEG_RPC 8
#define N_CH_RPC 33
#define N_CH_DET_RPC 16

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
    static const int n_seg_t0 = N_SEG_T0;
    static const int n_ch_det_t0 = N_CH_DET_T0; // Number of leading channels for T0
    static const int n_ch_t0 = N_CH_T0; // Number of all channels for T0
    static const int n_seg_rpc = N_SEG_RPC;
    static const int n_ch_det_rpc = N_CH_DET_RPC; // Number of leading channels for RPC
    static const int n_ch_rpc = N_CH_RPC; // Number of all channels for RPC
    static const int RPC_board = 1283;
    static const int T0_board = 1282;

    const double epoch_const    = 10240026.0; // Constant for epochtime
    const double coarse_const   = 5000.0; // Constant for coarsetime
    double trb3_linear_low      = 16.0; // Calibration for low end
    // double trb3_linear_high_T0  = 499.0; // Calibration for FPGA2 -- T0
    std::vector<double> trb3_linear_high_T0{ // Calibration for FPGA2 -- T0
      495.0,
      490.0,
      491.0,
      490.0,
      485.0,
      490.0,
      492.0,
      490.0,
      488.0,
      489.0,
      489.0,
      487.0,
      492.0,
      496.0,
      493.0,
      493.0,
      493.0,
      477.0,
      479.0,
      482.0,
      478.0,
      481.0,
      478.0,
      489.0,
      489.0,
      489.0,
      490.0,
      490.0,
      487.0,
      490.0,
      489.0,
      485.0,
      483.0,
      485.0,
      483.0,
      481.0,
      481.0,
      479.0,
      479.0,
      490.0,
      489.0
	};
    // double trb3_linear_high_RPC = 491.0; // Calibration for FPGA3 -- RPC
    std::vector<double> trb3_linear_high_RPC{ // Calibration for FPGA3 -- RPC
      483.0,
      479.0,
      479.0,
      479.0,
      480.0,
      479.0,
      485.0,
      482.0,
      479.0,
      486.0,
      480.0,
      479.0,
      486.0,
      484.0,
      485.0,
      485.0,
      479.0,
      477.0,
      476.0,
      477.0,
      476.0,
      481.0,
      480.0,
      486.0,
      481.0,
      479.0,
      484.0,
      481.0,
      479.0,
      479.0,
      482.0,
      478.0,
      477.0
	};
    double t0_tot_gate = 100000000.0;
    double rpc_tot_gate = 100000000.0;

    const int id_seg_ctr_rpc = 4;
    const int id_seg_ctr_t0  = 5;

    int  FindTopT0(int);
    int  FindLeadT0(int);
    int  FindLftRPC(int);
    int  FindLeadRPC(int);
    int  GetSegT0(int);
    int  GetSegRPC(int);
    void FillT0AnaTree(art::Handle< std::vector<rawdata::WaveForm> > &,
		       art::Handle< std::vector<rawdata::TRB3RawDigit> > &,
		       art::Handle< std::vector<rawdata::TRB3RawDigit> > &);

    void GetT0Tdc(const std::vector<rawdata::TRB3RawDigit>&);
    void GetRPCTdc(const std::vector<rawdata::TRB3RawDigit>&);
    void GetT0Tot(void);
    void GetRPCTot(void);

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    // emph::cmap::ChannelMap* fChannelMap;
    std::string fChanMapFileName;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;

    double time_trig_t0;
    double time_trig_rpc;
    std::vector<double> tdc_lead;
    std::vector<double> tdc_trail;
    std::vector<double> tot_vec;
    int    n_tdc;
    int    n_tdc_lead;
    int    n_tdc_trail;
    double tot_temp;

    TTree *tree;
    TTree *tree_fine;
    std::array<int,    n_seg_t0>  T0_seg;  // Segment of T0
    std::array<int,    n_seg_rpc> RPC_seg; // Segment of RPC

    std::array<double, n_seg_t0> TDC_top_ln_hi; // T0 caribration parameter of high edge for T0 top signals
    std::array<double, n_seg_t0> TDC_top_ln_lo; // T0 caribration parameter of low edge for T0 top signals
    std::array<double, n_seg_t0> TDC_bot_ln_hi; // T0 caribration parameter of high edge for T0 bottom signals
    std::array<double, n_seg_t0> TDC_bot_ln_lo; // T0 caribration parameter of low edge for T0 bottom signals

    std::array<double, n_seg_t0> ADC_top_max; // Pulse height of top signals
    std::array<double, n_seg_t0> ADC_bot_max; // Pulse height of bottom signals
    std::array<double, n_seg_t0> ADC_top_blw; // Baseline of top signals
    std::array<double, n_seg_t0> ADC_bot_blw; // Baseline of bottom signals

    std::array<double, n_seg_rpc> RPC_lft_ln_hi; // RPC caribration parameter of high edge for left signals
    std::array<double, n_seg_rpc> RPC_lft_ln_lo; // RPC caribration parameter of low edge for left signals
    std::array<double, n_seg_rpc> RPC_rgt_ln_hi; // RPC caribration parameter of high edge for right signals
    std::array<double, n_seg_rpc> RPC_rgt_ln_lo; // RPC caribration parameter of low edge for right signals

    double TDC_trg_t; // Time of triger signals for T0 TDC
    std::vector<std::vector<double>> TDC_top_t; // Time of top signals
    std::vector<std::vector<int>>    TDC_top_flg; // Flag of leading in top signals
    std::vector<std::vector<double>> TDC_top_tot; // TOT of top signals
    std::vector<std::vector<double>> TDC_bot_t; // Time of bottom signals
    std::vector<std::vector<int>>    TDC_bot_flg; // Flag of leading in bottom signals
    std::vector<std::vector<double>> TDC_bot_tot; // TOT of bottom signals

    std::vector<std::vector<double>> TDC_top_lead; // Time of leading signals of top channel
    std::vector<std::vector<double>> TDC_top_lead_fine; // Fineime of leading signals of top channel
    std::vector<std::vector<double>> TDC_top_trail; // Time of trailing signals of top channel
    std::vector<std::vector<double>> TDC_top_trail_fine; // Finetime of trailing signals of top channel
    std::vector<std::vector<double>> TDC_bot_lead; // Time of leading signals of bot channel
    std::vector<std::vector<double>> TDC_bot_lead_fine; // Fineime of leading signals of bot channel
    std::vector<std::vector<double>> TDC_bot_trail; // Time of trailing signals of bot channel
    std::vector<std::vector<double>> TDC_bot_trail_fine; // Finetime of trailing signals of bot channel

    double RPC_trg_t; // Time of triger signals for T0 TDC
    std::vector<std::vector<double>> RPC_lft_t; // Time of left signals
    std::vector<std::vector<int>>    RPC_lft_flg; // Flag of leading in left signals
    std::vector<std::vector<double>> RPC_lft_tot; // TOT of left signals
    std::vector<std::vector<double>> RPC_rgt_t; // Time of right signals
    std::vector<std::vector<int>>    RPC_rgt_flg; // Flag of leading in right signals
    std::vector<std::vector<double>> RPC_rgt_tot; // TOT of right signals

    std::vector<std::vector<double>> RPC_lft_lead; // Time of leading signals of lft channel
    std::vector<std::vector<double>> RPC_lft_lead_fine; // Fineime of leading signals of lft channel
    std::vector<std::vector<double>> RPC_lft_trail; // Time of trailing signals of lft channel
    std::vector<std::vector<double>> RPC_lft_trail_fine; // Finetime of trailing signals of lft channel
    std::vector<std::vector<double>> RPC_rgt_lead; // Time of leading signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_lead_fine; // Fineime of leading signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_trail; // Time of trailing signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_trail_fine; // Finetime of trailing signals of rgt channel

  };

  //.......................................................................
  T0Ana::T0Ana(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {

    this->reconfigure(pset);

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
    // int nchan_t0 = emph::geo::DetInfo::NChannel(emph::geo::T0);
    // int nchan_rpc = emph::geo::DetInfo::NChannel(emph::geo::RPC);

    // fChannelMap = 0;
    // if (!fChanMapFileName.empty()) {
    //   fChannelMap = new emph::cmap::ChannelMap();
    //   if (!fChannelMap->LoadMap(fChanMapFileName)) {
    // 	std::cerr << "Failed to load channel map from file " << fChanMapFileName << std::endl;
    // 	delete fChannelMap;
    // 	fChannelMap = 0;
    //   }
    //   std::cout << "Loaded channel map from file " << fChanMapFileName << std::endl;
    // }

    for(int i = 0; i < n_seg_t0; i++){
      T0_seg.at(i) = i;
    }

    for(int i = 1; i < n_ch_t0; i++){
      if(FindTopT0(i)){
	TDC_top_ln_hi.at(GetSegT0(i)) = trb3_linear_high_T0.at(i);
	TDC_top_ln_lo.at(GetSegT0(i)) = trb3_linear_low;
      }else{
	TDC_bot_ln_hi.at(GetSegT0(i)) = trb3_linear_high_T0.at(i);
	TDC_bot_ln_lo.at(GetSegT0(i)) = trb3_linear_low;
      }
    }

    for(int i = 0; i < n_seg_rpc; i++){
      RPC_seg.at(i) = i;
    }

    for(int i = 1; i < n_ch_rpc; i++){
      if(FindLftRPC(i)){
	RPC_lft_ln_hi.at(GetSegRPC(i)) = trb3_linear_high_RPC.at(i);
	RPC_lft_ln_lo.at(GetSegRPC(i)) = trb3_linear_low;
      }else{
	RPC_rgt_ln_hi.at(GetSegRPC(i)) = trb3_linear_high_RPC.at(i);
	RPC_rgt_ln_lo.at(GetSegRPC(i)) = trb3_linear_low;
      }
    }

    TDC_top_t.resize(n_seg_t0);
    TDC_top_flg.resize(n_seg_t0);
    TDC_top_tot.resize(n_seg_t0);
    TDC_bot_t.resize(n_seg_t0);
    TDC_bot_flg.resize(n_seg_t0);
    TDC_bot_tot.resize(n_seg_t0);

    TDC_top_lead.resize(n_seg_t0);
    TDC_top_lead_fine.resize(n_seg_t0);
    TDC_top_trail.resize(n_seg_t0);
    TDC_top_trail_fine.resize(n_seg_t0);
    TDC_bot_lead.resize(n_seg_t0);
    TDC_bot_lead_fine.resize(n_seg_t0);
    TDC_bot_trail.resize(n_seg_t0);
    TDC_bot_trail_fine.resize(n_seg_t0);

    RPC_lft_t.resize(n_seg_t0);
    RPC_lft_flg.resize(n_seg_t0);
    RPC_lft_tot.resize(n_seg_t0);
    RPC_rgt_t.resize(n_seg_t0);
    RPC_rgt_flg.resize(n_seg_t0);
    RPC_rgt_tot.resize(n_seg_t0);

    RPC_lft_lead.resize(n_seg_t0);
    RPC_lft_lead_fine.resize(n_seg_t0);
    RPC_lft_trail.resize(n_seg_t0);
    RPC_lft_trail_fine.resize(n_seg_t0);
    RPC_rgt_lead.resize(n_seg_t0);
    RPC_rgt_lead_fine.resize(n_seg_t0);
    RPC_rgt_trail.resize(n_seg_t0);
    RPC_rgt_trail_fine.resize(n_seg_t0);

    // create TTree for TRB3RawDigits
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("T0AnaTree","");
    std::cout << "tree = " << tree << std::endl;

    tree->Branch("T0_seg", &T0_seg);
    tree->Branch("RPC_seg", &RPC_seg);

    tree->Branch("ADC_top_max", &ADC_top_max);
    tree->Branch("ADC_bot_max", &ADC_bot_max);
    tree->Branch("ADC_top_blw", &ADC_top_blw);
    tree->Branch("ADC_bot_blw", &ADC_bot_blw);

    tree->Branch("TDC_trg_t",  &TDC_trg_t);
    tree->Branch("TDC_top_lead",  &TDC_top_lead);
    tree->Branch("TDC_top_trail",  &TDC_top_trail);
    // tree->Branch("TDC_top_t",  &TDC_top_t);
    // tree->Branch("TDC_top_flg", &TDC_top_flg);
    tree->Branch("TDC_top_tot",  &TDC_top_tot);
    tree->Branch("TDC_bot_lead",  &TDC_bot_lead);
    tree->Branch("TDC_bot_trail",  &TDC_bot_trail);
    // tree->Branch("TDC_bot_t",  &TDC_bot_t);
    // tree->Branch("TDC_bot_flg", &TDC_bot_flg);
    tree->Branch("TDC_bot_tot",  &TDC_bot_tot);

    tree->Branch("RPC_trg_t",  &RPC_trg_t);
    tree->Branch("RPC_lft_lead",  &RPC_lft_lead);
    tree->Branch("RPC_lft_trail",  &RPC_lft_trail);
    // tree->Branch("RPC_lft_t",  &RPC_lft_t);
    // tree->Branch("RPC_lft_flg", &RPC_lft_flg);
    tree->Branch("RPC_lft_tot",  &RPC_lft_tot);
    tree->Branch("RPC_rgt_lead",  &RPC_rgt_lead);
    tree->Branch("RPC_rgt_trail",  &RPC_rgt_trail);
    // tree->Branch("RPC_rgt_t",  &RPC_rgt_t);
    // tree->Branch("RPC_rgt_flg", &RPC_rgt_flg);
    tree->Branch("RPC_rgt_tot",  &RPC_rgt_tot);

    tree_fine = tfs->make<TTree>("T0AnaTree_fine","");
    std::cout << "tree_fine = " << tree_fine << std::endl;

    tree_fine->Branch("T0_seg", &T0_seg);
    tree_fine->Branch("RPC_seg", &RPC_seg);

    tree_fine->Branch("ADC_top_max", &ADC_top_max);
    tree_fine->Branch("ADC_bot_max", &ADC_bot_max);
    tree_fine->Branch("ADC_top_blw", &ADC_top_blw);
    tree_fine->Branch("ADC_bot_blw", &ADC_bot_blw);

    tree_fine->Branch("TDC_trg_t",  &TDC_trg_t);

    tree_fine->Branch("TDC_top_lead",  &TDC_top_lead);
    tree_fine->Branch("TDC_top_lead_fine",  &TDC_top_lead_fine);
    tree_fine->Branch("TDC_top_trail",  &TDC_top_trail);
    tree_fine->Branch("TDC_top_trail_fine",  &TDC_top_trail_fine);
    tree_fine->Branch("TDC_top_tot",  &TDC_top_tot);
    tree_fine->Branch("TDC_top_ln_hi",  &TDC_top_ln_hi);
    tree_fine->Branch("TDC_top_ln_lo",  &TDC_top_ln_lo);

    tree_fine->Branch("TDC_bot_lead",  &TDC_bot_lead);
    tree_fine->Branch("TDC_bot_lead_fine",  &TDC_bot_lead_fine);
    tree_fine->Branch("TDC_bot_trail",  &TDC_bot_trail);
    tree_fine->Branch("TDC_bot_trail_fine",  &TDC_bot_trail_fine);
    tree_fine->Branch("TDC_bot_tot",  &TDC_bot_tot);
    tree_fine->Branch("TDC_bot_ln_hi",  &TDC_bot_ln_hi);
    tree_fine->Branch("TDC_bot_ln_lo",  &TDC_bot_ln_lo);

    tree_fine->Branch("RPC_trg_t",  &RPC_trg_t);

    tree_fine->Branch("RPC_lft_lead",  &RPC_lft_lead);
    tree_fine->Branch("RPC_lft_lead_fine",  &RPC_lft_lead_fine);
    tree_fine->Branch("RPC_lft_trail",  &RPC_lft_trail);
    tree_fine->Branch("RPC_lft_trail_fine",  &RPC_lft_trail_fine);
    tree_fine->Branch("RPC_lft_tot",  &RPC_lft_tot);
    tree_fine->Branch("RPC_lft_ln_hi",  &RPC_lft_ln_hi);
    tree_fine->Branch("RPC_lft_ln_lo",  &RPC_lft_ln_lo);

    tree_fine->Branch("RPC_rgt_lead",  &RPC_rgt_lead);
    tree_fine->Branch("RPC_rgt_lead_fine",  &RPC_rgt_lead_fine);
    tree_fine->Branch("RPC_rgt_trail",  &RPC_rgt_trail);
    tree_fine->Branch("RPC_rgt_trail_fine",  &RPC_rgt_trail_fine);
    tree_fine->Branch("RPC_rgt_tot",  &RPC_rgt_tot);
    tree_fine->Branch("RPC_rgt_ln_hi",  &RPC_rgt_ln_hi);
    tree_fine->Branch("RPC_rgt_ln_lo",  &RPC_rgt_ln_lo);
    
  }

  //......................................................................
  void T0Ana::endJob()
  {
  }

  //......................................................................
  int T0Ana::FindTopT0(int ch_daq)
  {
    if (ch_daq >= 21 && ch_daq <= 40) return 1;

    return 0;
  }

  //......................................................................
  int T0Ana::FindLeadT0(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 40 && ch_daq%2 == 1) return 1;

    return 0;
  }

  //......................................................................
  int T0Ana::FindLftRPC(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 16) return 1;

    return 0;
  }

  //......................................................................
  int T0Ana::FindLeadRPC(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 32 && ch_daq%2 == 1) return 1;

    return 0;
  }

  //......................................................................
  int T0Ana::GetSegT0(int ch_daq)
  {
    if(ch_daq ==  1 || ch_daq ==  2 || ch_daq == 21 || ch_daq == 22){
      return 0;
    }else if(ch_daq ==  3 || ch_daq ==  4 || ch_daq == 23 || ch_daq == 24){
      return 1;
    }else if(ch_daq ==  5 || ch_daq ==  6 || ch_daq == 25 || ch_daq == 26){
      return 2;
    }else if(ch_daq ==  7 || ch_daq ==  8 || ch_daq == 27 || ch_daq == 28){
      return 3;
    }else if(ch_daq ==  9 || ch_daq == 10 || ch_daq == 29 || ch_daq == 30){
      return 4;
    }else if(ch_daq == 11 || ch_daq == 12 || ch_daq == 31 || ch_daq == 32){
      return 5;
    }else if(ch_daq == 13 || ch_daq == 14 || ch_daq == 33 || ch_daq == 35){
      return 6;
    }else if(ch_daq == 15 || ch_daq == 16 || ch_daq == 35 || ch_daq == 36){
      return 7;
    }else if(ch_daq == 17 || ch_daq == 18 || ch_daq == 37 || ch_daq == 38){
      return 8;
    }else{
      return 9;
    }
  }

  //......................................................................
  int T0Ana::GetSegRPC(int ch_daq)
  {
    if(ch_daq ==  1 || ch_daq ==  2 || ch_daq == 17 || ch_daq == 18){
      return 0;
    }else if(ch_daq ==  3 || ch_daq ==  4 || ch_daq == 19 || ch_daq == 20){
      return 1;
    }else if(ch_daq ==  5 || ch_daq ==  6 || ch_daq == 21 || ch_daq == 22){
      return 2;
    }else if(ch_daq ==  7 || ch_daq ==  8 || ch_daq == 23 || ch_daq == 24){
      return 3;
    }else if(ch_daq ==  9 || ch_daq == 10 || ch_daq == 25 || ch_daq == 26){
      return 4;
    }else if(ch_daq == 11 || ch_daq == 12 || ch_daq == 27 || ch_daq == 28){
      return 5;
    }else if(ch_daq == 13 || ch_daq == 14 || ch_daq == 29 || ch_daq == 30){
      return 6;
    }else{
      return 7;
    }
  }

  //......................................................................
  void T0Ana::GetT0Tdc(const std::vector<rawdata::TRB3RawDigit>& digvec)
  {
    for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
      TDC_top_t.at(i_seg).clear();
      TDC_top_flg.at(i_seg).clear();
      TDC_top_lead.at(i_seg).clear();
      TDC_top_lead_fine.at(i_seg).clear();
      TDC_top_trail.at(i_seg).clear();
      TDC_top_trail_fine.at(i_seg).clear();

      TDC_bot_t.at(i_seg).clear();
      TDC_bot_flg.at(i_seg).clear();
      TDC_bot_lead.at(i_seg).clear();
      TDC_bot_lead_fine.at(i_seg).clear();
      TDC_bot_trail.at(i_seg).clear();
      TDC_bot_trail_fine.at(i_seg).clear();
    }

    time_trig_t0 = 0;

    //loop over TRB3 signals
    int n_evt = digvec.size();
    for(int i_evt = 0; i_evt < n_evt; i_evt++){
      uint32_t evt_ch = digvec.at(i_evt).GetChannel();
      double time_evt_t0 = (epoch_const*digvec.at(i_evt).GetEpochCounter()
			    + coarse_const*digvec.at(i_evt).GetCoarseTime()
			    - coarse_const*(digvec.at(i_evt).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0.at(evt_ch) - trb3_linear_low))/1000.0;

      if(evt_ch == 0){
	time_trig_t0 = time_evt_t0;
	TDC_trg_t = time_trig_t0;
      }else{
	if(FindTopT0(evt_ch)){
	  TDC_top_t.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - time_trig_t0);
	  TDC_top_flg.at(GetSegT0(evt_ch)).push_back(FindLeadT0(evt_ch));

	  if(FindLeadT0(evt_ch)){
	    TDC_top_lead.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - time_trig_t0);
	    TDC_top_lead_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    TDC_top_trail.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - time_trig_t0);
	    TDC_top_trail_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }
	}else{
	  TDC_bot_t.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - time_trig_t0);
	  TDC_bot_flg.at(GetSegT0(evt_ch)).push_back(FindLeadT0(evt_ch));

	  if(FindLeadT0(evt_ch)){
	    TDC_bot_lead.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - time_trig_t0);
	    TDC_bot_lead_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    TDC_bot_trail.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - time_trig_t0);
	    TDC_bot_trail_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }
	}
      }
    }// end loop over TRB3 signals
  }

  //......................................................................
  void T0Ana::GetRPCTdc(const std::vector<rawdata::TRB3RawDigit>& digvec)
  {
    for(int i_seg = 0; i_seg < n_seg_rpc; i_seg++){
      RPC_lft_t.at(i_seg).clear();
      RPC_lft_flg.at(i_seg).clear();
      RPC_lft_lead.at(i_seg).clear();
      RPC_lft_lead_fine.at(i_seg).clear();
      RPC_lft_trail.at(i_seg).clear();
      RPC_lft_trail_fine.at(i_seg).clear();

      RPC_rgt_t.at(i_seg).clear();
      RPC_rgt_flg.at(i_seg).clear();
      RPC_rgt_lead.at(i_seg).clear();
      RPC_rgt_lead_fine.at(i_seg).clear();
      RPC_rgt_trail.at(i_seg).clear();
      RPC_rgt_trail_fine.at(i_seg).clear();
    }

    time_trig_rpc = 0;

    //loop over TRB3 signals
    int n_evt = digvec.size();
    for(int i_evt = 0; i_evt < n_evt; i_evt++){
      uint32_t evt_ch = digvec.at(i_evt).GetChannel();
      double time_evt_rpc = (epoch_const*digvec.at(i_evt).GetEpochCounter()
	                     + coarse_const*digvec.at(i_evt).GetCoarseTime()
	                     - coarse_const*(digvec.at(i_evt).GetFineTime() - trb3_linear_low)/(trb3_linear_high_RPC.at(evt_ch) - trb3_linear_low))/1000.0;

      if(evt_ch == 0){
	time_trig_rpc = time_evt_rpc;
	RPC_trg_t = time_trig_rpc;
      }else{
	if(FindLftRPC(evt_ch)){
	  RPC_lft_t.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - time_trig_rpc);
	  RPC_lft_flg.at(GetSegRPC(evt_ch)).push_back(FindLeadRPC(evt_ch));

	  if(FindLeadRPC(evt_ch)){
	    RPC_lft_lead.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - time_trig_rpc);
	    RPC_lft_lead_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    RPC_lft_trail.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - time_trig_rpc);
	    RPC_lft_trail_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }
	}else{
	  RPC_rgt_t.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - time_trig_rpc);
	  RPC_rgt_flg.at(GetSegRPC(evt_ch)).push_back(FindLeadRPC(evt_ch));

	  if(FindLeadRPC(evt_ch)){
	    RPC_rgt_lead.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - time_trig_rpc);
	    RPC_rgt_lead_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    RPC_rgt_trail.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - time_trig_rpc);
	    RPC_rgt_trail_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }
	}
      }
    }// end loop over TRB3 signals
  }

  //......................................................................
  void T0Ana::GetT0Tot(void)
  {
    for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
      TDC_top_tot.at(i_seg).clear();
      TDC_bot_tot.at(i_seg).clear();

      n_tdc = TDC_top_t.at(i_seg).size();
      tdc_lead.clear();
      tdc_trail.clear();
      for(int i_tdc = 0; i_tdc < n_tdc; i_tdc++){
	if(TDC_top_flg.at(i_seg).at(i_tdc) == 1){
	  tdc_lead.push_back(TDC_top_t.at(i_seg).at(i_tdc));
	}else{
	  tdc_trail.push_back(TDC_top_t.at(i_seg).at(i_tdc));
	}
      }

      n_tdc_lead = tdc_lead.size();
      n_tdc_trail = tdc_trail.size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_trail)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    TDC_top_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_lead)
      }else{
	for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	  tot_vec.clear();
	  for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_lead)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    TDC_top_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_trail)
      }//if(n_tdc)

      n_tdc = TDC_bot_t.at(i_seg).size();
      tdc_lead.clear();
      tdc_trail.clear();
      for(int i_tdc = 0; i_tdc < n_tdc; i_tdc++){
	if(TDC_bot_flg.at(i_seg).at(i_tdc) == 1){
	  tdc_lead.push_back(TDC_bot_t.at(i_seg).at(i_tdc));
	}else{
	  tdc_trail.push_back(TDC_bot_t.at(i_seg).at(i_tdc));
	}
      }

      n_tdc_lead = tdc_lead.size();
      n_tdc_trail = tdc_trail.size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_trail)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    TDC_bot_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_lead)
      }else{
	for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	  tot_vec.clear();
	  for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_lead)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    TDC_bot_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_trail)
      }//if(n_tdc)
    }//for(n_seg)
  }

  //......................................................................
  void T0Ana::GetRPCTot(void)
  {
    for(int i_seg = 0; i_seg < n_seg_rpc; i_seg++){
      RPC_lft_tot.at(i_seg).clear();
      RPC_rgt_tot.at(i_seg).clear();

      n_tdc = RPC_lft_t.at(i_seg).size();
      tdc_lead.clear();
      tdc_trail.clear();
      for(int i_tdc = 0; i_tdc < n_tdc; i_tdc++){
	if(RPC_lft_flg.at(i_seg).at(i_tdc) == 1){
	  tdc_lead.push_back(RPC_lft_t.at(i_seg).at(i_tdc));
	}else{
	  tdc_trail.push_back(RPC_lft_t.at(i_seg).at(i_tdc));
	}
      }

      n_tdc_lead = tdc_lead.size();
      n_tdc_trail = tdc_trail.size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_trail)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    RPC_lft_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_lead)
      }else{
	for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	  tot_vec.clear();
	  for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_lead)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    RPC_lft_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_trail)
      }//if(n_tdc)

      n_tdc = RPC_rgt_t.at(i_seg).size();
      tdc_lead.clear();
      tdc_trail.clear();
      for(int i_tdc = 0; i_tdc < n_tdc; i_tdc++){
	if(RPC_rgt_flg.at(i_seg).at(i_tdc) == 1){
	  tdc_lead.push_back(RPC_rgt_t.at(i_seg).at(i_tdc));
	}else{
	  tdc_trail.push_back(RPC_rgt_t.at(i_seg).at(i_tdc));
	}
      }

      n_tdc_lead = tdc_lead.size();
      n_tdc_trail = tdc_trail.size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_trail)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    RPC_rgt_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_lead)
      }else{
	for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	  tot_vec.clear();
	  for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	    if(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead) > 0){
	      tot_vec.push_back(tdc_trail.at(i_tdc_trail) - tdc_lead.at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_lead)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    RPC_rgt_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot <empty)
	}//for(n_tdc_trail)
      }//if(n_tdc)
    }//for(n_seg)
  }

  //......................................................................

  void T0Ana::FillT0AnaTree(art::Handle< std::vector<emph::rawdata::WaveForm> > & T0wvfm, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & T0trb3, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & RPCtrb3)
  {
    for(int i = 0; i < n_seg_t0; i++){
      ADC_top_max[i] = -9999.0;
      ADC_bot_max[i] = -9999.0;
      ADC_top_blw[i] = -1.0;
      ADC_bot_blw[i] = -1.0;
    }

    // get ADC info for T0
    if (!T0wvfm->empty()) {
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel T0echan;
      T0echan.SetBoardType(boardType);
      // loop over ADC channels
      for (size_t idx=0; idx < T0wvfm->size(); ++idx){
	const rawdata::WaveForm& wvfm = (*T0wvfm)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	T0echan.SetBoard(board);
	T0echan.SetChannel(chan);
	emph::cmap::DChannel dchan = cmap->DetChan(T0echan);
	int detchan = dchan.Channel();
	if (detchan > 0 && detchan <= n_seg_t0){
	  ADC_bot_max[detchan - 1] = wvfm.Baseline()-wvfm.PeakADC();
	  ADC_bot_blw[detchan - 1] = wvfm.BLWidth();
	}else if(detchan > n_seg_t0 && detchan <= n_ch_det_t0){
	  ADC_top_max[detchan%(n_seg_t0 + 1)] = wvfm.Baseline()-wvfm.PeakADC();
	  ADC_top_blw[detchan%(n_seg_t0 + 1)] = wvfm.BLWidth();
	}
      } // end loop over T0 ADC channels
    }

    // get TDC info for T0
    if (!T0trb3->empty()) {
      std::vector<emph::rawdata::TRB3RawDigit> VecT0trb3 = *T0trb3;
      GetT0Tdc(VecT0trb3);
      GetT0Tot();
    }

    // get TDC info for RPC
    if (!RPCtrb3->empty()) {
      std::vector<emph::rawdata::TRB3RawDigit> VecRPCtrb3 = *RPCtrb3;
      GetRPCTdc(VecRPCtrb3);
      GetRPCTot();
    }

    tree->Fill();
    tree_fine->Fill();

  }

  //......................................................................
  void T0Ana::analyze(const art::Event& evt)
  {
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();
    std::string labelStrT0, labelStrRPC;

    // get WaveForm
    int i = emph::geo::T0;
    labelStrT0 = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
    art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > HandleT0trb3;
    art::Handle< std::vector<emph::rawdata::WaveForm> > HandleT0wvfm;

    int j = emph::geo::RPC;
    labelStrRPC = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(j));
    art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > HandleRPCtrb3;

    try {
      evt.getByLabel(labelStrT0, HandleT0trb3);
      evt.getByLabel(labelStrT0, HandleT0wvfm);
      evt.getByLabel(labelStrRPC, HandleRPCtrb3);

      if (!HandleT0trb3->empty()) {
	FillT0AnaTree(HandleT0wvfm, HandleT0trb3, HandleRPCtrb3);
      }
    }
    catch(...) {

    }

    return;
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::T0Ana)
