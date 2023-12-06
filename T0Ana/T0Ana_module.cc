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
// #include "Geometry/service/GeometryService.h"
#include "RawData/WaveForm.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/SSDHit.h"

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
    static const int n_ch_trig = 1; // Number of channels of Trigger
    static const int n_ch_gc = 3; // Number of channels of GCkov
    static const int n_ch_bac = 6; // Number of channels of BACkov

    static const int n_seg_t0 = 10; // Number of segments of T0
    static const int n_ch_det_t0 = 20; // Number of leading channels for T0
    static const int n_ch_t0 = 41; // Number of all channels for T0

    static const int n_seg_rpc = 8; // Number of segments of RPC
    static const int n_ch_det_rpc = 33; // Number of leading channels for RPC
    static const int n_ch_rpc = 16; // Number of all channels for RPC

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

    int  FindTopT0(int);
    int  FindLeadT0(int);
    int  FindLftRPC(int);
    int  FindLeadRPC(int);
    int  GetSegT0(int);
    int  GetSegRPC(int);
    void FillTreeTOF(art::Handle< std::vector<rawdata::WaveForm> > &, // T0wvfm
		     art::Handle< std::vector<rawdata::TRB3RawDigit> > &, // T0trb3
		     art::Handle< std::vector<rawdata::TRB3RawDigit> > &); // RPCtrb3
    void FillTreeADC(art::Handle< std::vector<rawdata::WaveForm> > &, // TRIGwvfm
		     art::Handle< std::vector<rawdata::WaveForm> > &, // GCwvfm
		     art::Handle< std::vector<rawdata::WaveForm> > &); // BACwvfm
    void FillTreeSSD(art::Handle< std::vector<rawdata::SSDRawDigit> > &); //SSD raw data

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

    std::array<double, n_seg_t0> TDC_top_ln_hi; // T0 caribration parameter of high edge for T0 top signals
    std::array<double, n_seg_t0> TDC_top_ln_lo; // T0 caribration parameter of low edge for T0 top signals
    std::array<double, n_seg_t0> TDC_bot_ln_hi; // T0 caribration parameter of high edge for T0 bottom signals
    std::array<double, n_seg_t0> TDC_bot_ln_lo; // T0 caribration parameter of low edge for T0 bottom signals

    std::array<double, n_seg_rpc> RPC_lft_ln_hi; // RPC caribration parameter of high edge for left signals
    std::array<double, n_seg_rpc> RPC_lft_ln_lo; // RPC caribration parameter of low edge for left signals
    std::array<double, n_seg_rpc> RPC_rgt_ln_hi; // RPC caribration parameter of high edge for right signals
    std::array<double, n_seg_rpc> RPC_rgt_ln_lo; // RPC caribration parameter of low edge for right signals


    TTree *tree;
    double TRIG_t; // Pulse time of Trigger signal
    double TRIG_hgt; // Pulse height of Trigger signal
    double TRIG_blw; // Baseline width of Trigger signal

    std::array<double, n_ch_gc> GC_t; // Pulse time of GasCkov signals
    std::array<double, n_ch_gc> GC_hgt; // Pulse height of GasCkov signals
    std::array<double, n_ch_gc> GC_blw; // Baseline width of GasCkov signals

    std::array<double, n_ch_bac> BAC_t; // Pulse time of BACkov signals
    std::array<double, n_ch_bac> BAC_hgt; // Pulse height of BACkov signals
    std::array<double, n_ch_bac> BAC_blw; // Baseline width of BACkov signals

    std::array<int,    n_seg_t0>  T0_seg;  // Segment of T0
    double TDC_trg_t; // Time of triger signals for T0 TDC
    uint64_t TDC_trg_ts; // Timestamp of triger signals for T0 TDC

    std::vector<std::vector<double>> TDC_top_ts; // Timestamp of leading signals of top channel
    std::vector<std::vector<double>> TDC_top_lead; // Time of leading signals of top channel
    std::vector<std::vector<double>> TDC_top_lead_fine; // Fineime of leading signals of top channel
    std::vector<std::vector<double>> TDC_top_trail; // Time of trailing signals of top channel
    std::vector<std::vector<double>> TDC_top_trail_fine; // Finetime of trailing signals of top channel
    std::vector<std::vector<double>> TDC_top_tot; // TOT of top signals
    std::vector<std::vector<double>> TDC_bot_ts; // Timestamp of leading signals of bot channel
    std::vector<std::vector<double>> TDC_bot_lead; // Time of leading signals of bot channel
    std::vector<std::vector<double>> TDC_bot_lead_fine; // Fineime of leading signals of bot channel
    std::vector<std::vector<double>> TDC_bot_trail; // Time of trailing signals of bot channel
    std::vector<std::vector<double>> TDC_bot_trail_fine; // Finetime of trailing signals of bot channel
    std::vector<std::vector<double>> TDC_bot_tot; // TOT of bottom signals

    std::array<double, n_seg_t0> ADC_top_ts; // Timestamp of top signals
    std::array<double, n_seg_t0> ADC_bot_ts; // Timestamp of bottom signals
    std::array<double, n_seg_t0> ADC_top_t; // Pulse time of top signals
    std::array<double, n_seg_t0> ADC_bot_t; // Pulse time of bottom signals
    std::array<double, n_seg_t0> ADC_top_hgt; // Pulse height of top signals
    std::array<double, n_seg_t0> ADC_bot_hgt; // Pulse height of bottom signals
    std::array<double, n_seg_t0> ADC_top_blw; // Baseline of top signals
    std::array<double, n_seg_t0> ADC_bot_blw; // Baseline of bottom signals

    std::array<int,    n_seg_rpc> RPC_seg; // Segment of RPC
    double RPC_trg_t; // Time of triger signals for T0 TDC
    uint64_t RPC_trg_ts; // Timestamp of triger signals for T0 TDC

    std::vector<std::vector<double>> RPC_lft_ts; // Timestamp of leading signals of lft channel
    std::vector<std::vector<double>> RPC_lft_lead; // Time of leading signals of lft channel
    std::vector<std::vector<double>> RPC_lft_lead_fine; // Fineime of leading signals of lft channel
    std::vector<std::vector<double>> RPC_lft_trail; // Time of trailing signals of lft channel
    std::vector<std::vector<double>> RPC_lft_trail_fine; // Finetime of trailing signals of lft channel
    std::vector<std::vector<double>> RPC_lft_tot; // TOT of left signals
    std::vector<std::vector<double>> RPC_rgt_ts; // Timestamp of leading signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_lead; // Time of leading signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_lead_fine; // Fineime of leading signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_trail; // Time of trailing signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_trail_fine; // Finetime of trailing signals of rgt channel
    std::vector<std::vector<double>> RPC_rgt_tot; // TOT of right signals

    std::vector<double> SSD_st;
    std::vector<double> SSD_ch;
    std::vector<double> SSD_t;
    std::vector<double> SSD_adc;

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

    TDC_top_ts.resize(n_seg_t0);
    TDC_top_lead.resize(n_seg_t0);
    TDC_top_lead_fine.resize(n_seg_t0);
    TDC_top_trail.resize(n_seg_t0);
    TDC_top_trail_fine.resize(n_seg_t0);
    TDC_top_tot.resize(n_seg_t0);
    TDC_bot_ts.resize(n_seg_t0);
    TDC_bot_lead.resize(n_seg_t0);
    TDC_bot_lead_fine.resize(n_seg_t0);
    TDC_bot_trail.resize(n_seg_t0);
    TDC_bot_trail_fine.resize(n_seg_t0);
    TDC_bot_tot.resize(n_seg_t0);

    RPC_lft_ts.resize(n_seg_t0);
    RPC_lft_lead.resize(n_seg_t0);
    RPC_lft_lead_fine.resize(n_seg_t0);
    RPC_lft_trail.resize(n_seg_t0);
    RPC_lft_trail_fine.resize(n_seg_t0);
    RPC_lft_tot.resize(n_seg_t0);
    RPC_rgt_ts.resize(n_seg_t0);
    RPC_rgt_lead.resize(n_seg_t0);
    RPC_rgt_lead_fine.resize(n_seg_t0);
    RPC_rgt_trail.resize(n_seg_t0);
    RPC_rgt_trail_fine.resize(n_seg_t0);
    RPC_rgt_tot.resize(n_seg_t0);

    // create TTree for TRB3RawDigits
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("T0AnaTree","");
    std::cout << "tree = " << tree << std::endl;

    // Branchse for Trigger
    tree->Branch("TRIG_t", &TRIG_t);
    tree->Branch("TRIG_hgt", &TRIG_hgt);
    tree->Branch("TRIG_blw", &TRIG_blw);

    // Branchse for GasCkov
    tree->Branch("GC_t", &GC_t);
    tree->Branch("GC_hgt", &GC_hgt);
    tree->Branch("GC_blw", &GC_blw);

    // Branchse for BACkov
    tree->Branch("BAC_t", &BAC_t);
    tree->Branch("BAC_hgt", &BAC_hgt);
    tree->Branch("BAC_blw", &BAC_blw);

    // Branchse for T0
    tree->Branch("T0_seg", &T0_seg);

    tree->Branch("TDC_trg_t",  &TDC_trg_t);
    tree->Branch("TDC_trg_ts",  &TDC_trg_ts);
    tree->Branch("TDC_top_ts",  &TDC_top_ts);
    tree->Branch("TDC_top_lead",  &TDC_top_lead);
    tree->Branch("TDC_top_trail",  &TDC_top_trail);
    tree->Branch("TDC_top_tot",  &TDC_top_tot);
    tree->Branch("TDC_bot_ts",  &TDC_bot_ts);
    tree->Branch("TDC_bot_lead",  &TDC_bot_lead);
    tree->Branch("TDC_bot_trail",  &TDC_bot_trail);
    tree->Branch("TDC_bot_tot",  &TDC_bot_tot);

    tree->Branch("ADC_top_ts", &ADC_top_ts);
    tree->Branch("ADC_top_t", &ADC_top_t);
    tree->Branch("ADC_top_hgt", &ADC_top_hgt);
    tree->Branch("ADC_top_blw", &ADC_top_blw);
    tree->Branch("ADC_bot_ts", &ADC_bot_ts);
    tree->Branch("ADC_bot_t", &ADC_bot_t);
    tree->Branch("ADC_bot_hgt", &ADC_bot_hgt);
    tree->Branch("ADC_bot_blw", &ADC_bot_blw);

    // Branches for RPC
    tree->Branch("RPC_seg", &RPC_seg);

    tree->Branch("RPC_trg_t",  &RPC_trg_t);
    tree->Branch("RPC_trg_ts",  &RPC_trg_ts);
    tree->Branch("RPC_lft_ts",  &RPC_lft_ts);
    tree->Branch("RPC_lft_lead",  &RPC_lft_lead);
    tree->Branch("RPC_lft_trail",  &RPC_lft_trail);
    tree->Branch("RPC_lft_tot",  &RPC_lft_tot);
    tree->Branch("RPC_rgt_ts",  &RPC_rgt_ts);
    tree->Branch("RPC_rgt_lead",  &RPC_rgt_lead);
    tree->Branch("RPC_rgt_trail",  &RPC_rgt_trail);
    tree->Branch("RPC_rgt_tot",  &RPC_rgt_tot);

    // Branches for SSD
    tree->Branch("SSD_st", &SSD_st);
    tree->Branch("SSD_ch", &SSD_ch);
    tree->Branch("SSD_t", &SSD_t);
    tree->Branch("SSD_adc", &SSD_adc);

    // Branches for TDC fine parameters
    tree->Branch("TDC_top_lead_fine",  &TDC_top_lead_fine);
    tree->Branch("TDC_top_trail_fine",  &TDC_top_trail_fine);
    tree->Branch("TDC_bot_lead_fine",  &TDC_bot_lead_fine);
    tree->Branch("TDC_bot_trail_fine",  &TDC_bot_trail_fine);

    tree->Branch("RPC_lft_lead_fine",  &RPC_lft_lead_fine);
    tree->Branch("RPC_lft_trail_fine",  &RPC_lft_trail_fine);
    tree->Branch("RPC_rgt_lead_fine",  &RPC_rgt_lead_fine);
    tree->Branch("RPC_rgt_trail_fine",  &RPC_rgt_trail_fine);
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
    }else if(ch_daq == 13 || ch_daq == 14 || ch_daq == 33 || ch_daq == 34){
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
      TDC_top_ts.at(i_seg).clear();
      TDC_top_lead.at(i_seg).clear();
      TDC_top_lead_fine.at(i_seg).clear();
      TDC_top_trail.at(i_seg).clear();
      TDC_top_trail_fine.at(i_seg).clear();

      TDC_bot_ts.at(i_seg).clear();
      TDC_bot_lead.at(i_seg).clear();
      TDC_bot_lead_fine.at(i_seg).clear();
      TDC_bot_trail.at(i_seg).clear();
      TDC_bot_trail_fine.at(i_seg).clear();
    }

    TDC_trg_t = 0;

    //loop over TRB3 signals
    int n_evt = digvec.size();
    for(int i_evt = 0; i_evt < n_evt; i_evt++){
      uint32_t evt_ch = digvec.at(i_evt).GetChannel();
      double time_evt_t0 = (epoch_const*digvec.at(i_evt).GetEpochCounter()
			    + coarse_const*digvec.at(i_evt).GetCoarseTime()
			    - coarse_const*(digvec.at(i_evt).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0.at(evt_ch) - trb3_linear_low))/1000.0;

      if(evt_ch == 0){
	TDC_trg_t = time_evt_t0;
	TDC_trg_ts = digvec.at(i_evt).GetFragmentTimestamp();
      }else{
	if(FindTopT0(evt_ch)){
	  if(FindLeadT0(evt_ch)){
	    TDC_top_ts.at(GetSegT0(evt_ch)).push_back(static_cast<double>(digvec.at(i_evt).GetFragmentTimestamp()));
	    TDC_top_lead.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - TDC_trg_t);
	    TDC_top_lead_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    TDC_top_trail.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - TDC_trg_t);
	    TDC_top_trail_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }
	}else{
	  if(FindLeadT0(evt_ch)){
	    TDC_bot_ts.at(GetSegT0(evt_ch)).push_back(static_cast<double>(digvec.at(i_evt).GetFragmentTimestamp()));
	    TDC_bot_lead.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - TDC_trg_t);
	    TDC_bot_lead_fine.at(GetSegT0(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    TDC_bot_trail.at(GetSegT0(evt_ch)).push_back(time_evt_t0 - TDC_trg_t);
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
      RPC_lft_ts.at(i_seg).clear();
      RPC_lft_lead.at(i_seg).clear();
      RPC_lft_lead_fine.at(i_seg).clear();
      RPC_lft_trail.at(i_seg).clear();
      RPC_lft_trail_fine.at(i_seg).clear();

      RPC_rgt_ts.at(i_seg).clear();
      RPC_rgt_lead.at(i_seg).clear();
      RPC_rgt_lead_fine.at(i_seg).clear();
      RPC_rgt_trail.at(i_seg).clear();
      RPC_rgt_trail_fine.at(i_seg).clear();
    }

    RPC_trg_t = 0;

    //loop over TRB3 signals
    int n_evt = digvec.size();
    for(int i_evt = 0; i_evt < n_evt; i_evt++){
      uint32_t evt_ch = digvec.at(i_evt).GetChannel();
      double time_evt_rpc = (epoch_const*digvec.at(i_evt).GetEpochCounter()
	                     + coarse_const*digvec.at(i_evt).GetCoarseTime()
	                     - coarse_const*(digvec.at(i_evt).GetFineTime() - trb3_linear_low)/(trb3_linear_high_RPC.at(evt_ch) - trb3_linear_low))/1000.0;

      if(evt_ch == 0){
	RPC_trg_t = time_evt_rpc;
	RPC_trg_ts = digvec.at(i_evt).GetFragmentTimestamp();
      }else{
	if(FindLftRPC(evt_ch)){
	  if(FindLeadRPC(evt_ch)){
	    RPC_lft_ts.at(GetSegRPC(evt_ch)).push_back(static_cast<double>(digvec.at(i_evt).GetFragmentTimestamp()));
	    RPC_lft_lead.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - RPC_trg_t);
	    RPC_lft_lead_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    RPC_lft_trail.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - RPC_trg_t);
	    RPC_lft_trail_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }
	}else{
	  if(FindLeadRPC(evt_ch)){
	    RPC_rgt_ts.at(GetSegRPC(evt_ch)).push_back(static_cast<double>(digvec.at(i_evt).GetFragmentTimestamp()));
	    RPC_rgt_lead.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - RPC_trg_t);
	    RPC_rgt_lead_fine.at(GetSegRPC(evt_ch)).push_back(digvec.at(i_evt).GetFineTime());
	  }else{
	    RPC_rgt_trail.at(GetSegRPC(evt_ch)).push_back(time_evt_rpc - RPC_trg_t);
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

      n_tdc_lead = TDC_top_lead.at(i_seg).size();
      n_tdc_trail = TDC_top_trail.at(i_seg).size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(TDC_top_trail.at(i_seg).at(i_tdc_trail) - TDC_top_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(TDC_top_trail.at(i_seg).at(i_tdc_trail) - TDC_top_lead.at(i_seg).at(i_tdc_lead));
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
	    if(TDC_top_trail.at(i_seg).at(i_tdc_trail) - TDC_top_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(TDC_top_trail.at(i_seg).at(i_tdc_trail) - TDC_top_lead.at(i_seg).at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_lead)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    TDC_top_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_trail)
      }//if(n_tdc)

      n_tdc_lead = TDC_bot_lead.at(i_seg).size();
      n_tdc_trail = TDC_bot_trail.at(i_seg).size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(TDC_bot_trail.at(i_seg).at(i_tdc_trail) - TDC_bot_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(TDC_bot_trail.at(i_seg).at(i_tdc_trail) - TDC_bot_lead.at(i_seg).at(i_tdc_lead));
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
	    if(TDC_bot_trail.at(i_seg).at(i_tdc_trail) - TDC_bot_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(TDC_bot_trail.at(i_seg).at(i_tdc_trail) - TDC_bot_lead.at(i_seg).at(i_tdc_lead));
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

      n_tdc_lead = RPC_lft_lead.at(i_seg).size();
      n_tdc_trail = RPC_lft_trail.at(i_seg).size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(RPC_lft_trail.at(i_seg).at(i_tdc_trail) - RPC_lft_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(RPC_lft_trail.at(i_seg).at(i_tdc_trail) - RPC_lft_lead.at(i_seg).at(i_tdc_lead));
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
	    if(RPC_lft_trail.at(i_seg).at(i_tdc_trail) - RPC_lft_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(RPC_lft_trail.at(i_seg).at(i_tdc_trail) - RPC_lft_lead.at(i_seg).at(i_tdc_lead));
	    }//if(tot gate)
	  }//for(n_tdc_lead)
	  if(!tot_vec.empty()){
	    tot_temp = *std::min_element(tot_vec.begin(), tot_vec.end());
	    RPC_lft_tot.at(i_seg).push_back(tot_temp);
	  }//if(tot empty)
	}//for(n_tdc_trail)
      }//if(n_tdc)

      n_tdc_lead = RPC_rgt_lead.at(i_seg).size();
      n_tdc_trail = RPC_rgt_trail.at(i_seg).size();
      if(n_tdc_lead <= n_tdc_trail){
	for(int i_tdc_lead = 0; i_tdc_lead < n_tdc_lead; i_tdc_lead++){
	  tot_vec.clear();
	  for(int i_tdc_trail = 0; i_tdc_trail < n_tdc_trail; i_tdc_trail++){
	    if(RPC_rgt_trail.at(i_seg).at(i_tdc_trail) - RPC_rgt_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(RPC_rgt_trail.at(i_seg).at(i_tdc_trail) - RPC_rgt_lead.at(i_seg).at(i_tdc_lead));
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
	    if(RPC_rgt_trail.at(i_seg).at(i_tdc_trail) - RPC_rgt_lead.at(i_seg).at(i_tdc_lead) > 0){
	      tot_vec.push_back(RPC_rgt_trail.at(i_seg).at(i_tdc_trail) - RPC_rgt_lead.at(i_seg).at(i_tdc_lead));
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

  void T0Ana::FillTreeTOF(art::Handle< std::vector<emph::rawdata::WaveForm> > & T0wvfm, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & T0trb3, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & RPCtrb3)
  {
    for(int i = 0; i < n_seg_t0; i++){
      ADC_top_ts[i] = -1.0;
      ADC_bot_ts[i] = -1.0;
      ADC_top_t[i] = -9999.0;
      ADC_bot_t[i] = -9999.0;
      ADC_top_hgt[i] = -9999.0;
      ADC_bot_hgt[i] = -9999.0;
      ADC_top_blw[i] = -1.0;
      ADC_bot_blw[i] = -1.0;
    }//for(i:n_seg_t0)

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
	  ADC_bot_ts[detchan - 1] = static_cast<double>(wvfm.FragmentTime());
	  ADC_bot_t[detchan - 1] = wvfm.PeakTDC();
	  ADC_bot_hgt[detchan - 1] = wvfm.Baseline()-wvfm.PeakADC();
	  ADC_bot_blw[detchan - 1] = wvfm.BLWidth();
	}else if(detchan > n_seg_t0 && detchan <= n_ch_det_t0){
	  ADC_top_ts[detchan%(n_seg_t0 + 1)] = static_cast<double>(wvfm.FragmentTime());
	  ADC_top_t[detchan%(n_seg_t0 + 1)] = wvfm.PeakTDC();
	  ADC_top_hgt[detchan%(n_seg_t0 + 1)] = wvfm.Baseline()-wvfm.PeakADC();
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
  }

  //......................................................................

  void T0Ana::FillTreeADC(art::Handle< std::vector<emph::rawdata::WaveForm> > & TRIGwvfm, art::Handle< std::vector<emph::rawdata::WaveForm> > & GCwvfm, art::Handle< std::vector<emph::rawdata::WaveForm> > & BACwvfm)
  {

    TRIG_t = -9999.0;
    TRIG_hgt = -9999.0;
    TRIG_blw = -1.0;

    // get ADC info for TRIG
    if(!TRIGwvfm->empty()) {
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel TRIGechan;
      TRIGechan.SetBoardType(boardType);
      // loop over ADC channels
      for (size_t idx=0; idx < TRIGwvfm->size(); ++idx){
	const rawdata::WaveForm& wvfm = (*TRIGwvfm)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	TRIGechan.SetBoard(board);
	TRIGechan.SetChannel(chan);
	emph::cmap::DChannel dchan = cmap->DetChan(TRIGechan);
	// int detchan = dchan.Channel();
	TRIG_t = wvfm.PeakTDC();
	TRIG_hgt = wvfm.Baseline()-wvfm.PeakADC();
	TRIG_blw = wvfm.BLWidth();
      } // end loop over TRIG ADC channels
    }

    for(int i = 0; i < n_ch_gc; i++){
      GC_t[i] = -9999.0;
      GC_hgt[i] = -9999.0;
      GC_blw[i] = -1.0;
    }

    // get ADC info for GC
    if(!GCwvfm->empty()) {
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel GCechan;
      GCechan.SetBoardType(boardType);
      // loop over ADC channels
      for (size_t idx=0; idx < GCwvfm->size(); ++idx){
	const rawdata::WaveForm& wvfm = (*GCwvfm)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	GCechan.SetBoard(board);
	GCechan.SetChannel(chan);
	emph::cmap::DChannel dchan = cmap->DetChan(GCechan);
	int detchan = dchan.Channel();
	GC_t[detchan] = wvfm.PeakTDC();
	GC_hgt[detchan] = wvfm.Baseline()-wvfm.PeakADC();
	GC_blw[detchan] = wvfm.BLWidth();
      } // end loop over GCkov ADC channels
    }

    for(int i = 0; i < n_ch_bac; i++){
      BAC_t[i] = -9999.0;
      BAC_hgt[i] = -9999.0;
      BAC_blw[i] = -1.0;
    }

    // get ADC info for BAC
    if(!BACwvfm->empty()) {
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel BACechan;
      BACechan.SetBoardType(boardType);
      // loop over ADC channels
      for (size_t idx=0; idx < BACwvfm->size(); ++idx){
	const rawdata::WaveForm& wvfm = (*BACwvfm)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	BACechan.SetBoard(board);
	BACechan.SetChannel(chan);
	emph::cmap::DChannel dchan = cmap->DetChan(BACechan);
	int detchan = dchan.Channel();
	BAC_t[detchan] = wvfm.PeakTDC();
	BAC_hgt[detchan] = wvfm.Baseline()-wvfm.PeakADC();
	BAC_blw[detchan] = wvfm.BLWidth();
      } // end loop over BACkov ADC channels
    }
  }

  void T0Ana::FillTreeSSD(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > & SSDdigit)
  {
    SSD_st.clear();
    SSD_ch.clear();
    SSD_t.clear();
    SSD_adc.clear();

    // get SSD info
    if(!SSDdigit->empty()){
      // emph::cmap::FEBoardType boardType = emph::cmap::SSD;
      emph::cmap::EChannel SSDechan;
      for(size_t idx=0; idx < SSDdigit->size(); ++idx){
	const rawdata::SSDRawDigit& ssd = (*SSDdigit)[idx];
	SSDechan.SetBoard(ssd.FER());
	SSDechan.SetChannel(ssd.Module());
	emph::cmap::DChannel dchan = cmap->DetChan(SSDechan);
	int detst = dchan.Station();
	int detch =dchan.Channel();
	SSD_st.push_back(detst);
	SSD_ch.push_back(detch);
	SSD_t.push_back(ssd.Time());
	SSD_adc.push_back(ssd.ADC());
      } // end loop over SSDRawDigit
    }
  }
  //......................................................................
  void T0Ana::analyze(const art::Event& evt)
  {
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();
    std::string labelStrTRIG, labelStrGC, labelStrBAC, labelStrT0, labelStrRPC, labelStrSSD;

    // get Raw Data
    int i_trig = emph::geo::Trigger;
    labelStrTRIG = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_trig));
    art::Handle< std::vector<emph::rawdata::WaveForm> > HandleTRIGwvfm;

    int i_gc = emph::geo::GasCkov;
    labelStrGC = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_gc));
    art::Handle< std::vector<emph::rawdata::WaveForm> > HandleGCwvfm;

    int i_bac = emph::geo::BACkov;
    labelStrBAC = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_bac));
    art::Handle< std::vector<emph::rawdata::WaveForm> > HandleBACwvfm;

    int i_t0 = emph::geo::T0;
    labelStrT0 = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_t0));
    art::Handle< std::vector<emph::rawdata::WaveForm> > HandleT0wvfm;
    art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > HandleT0trb3;

    int i_rpc = emph::geo::RPC;
    labelStrRPC = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_rpc));
    art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > HandleRPCtrb3;

    int i_ssd = emph::geo::SSD;
    labelStrSSD = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_ssd));
    art::Handle< std::vector<emph::rawdata::SSDRawDigit> > HandleSSDdigit;

    try {
      evt.getByLabel(labelStrTRIG, HandleTRIGwvfm);
      evt.getByLabel(labelStrGC, HandleGCwvfm);
      evt.getByLabel(labelStrBAC, HandleBACwvfm);
      evt.getByLabel(labelStrT0, HandleT0wvfm);
      evt.getByLabel(labelStrT0, HandleT0trb3);
      evt.getByLabel(labelStrRPC, HandleRPCtrb3);
      evt.getByLabel(labelStrSSD, HandleSSDdigit);

      if (!HandleT0trb3->empty()) {
	FillTreeTOF(HandleT0wvfm, HandleT0trb3, HandleRPCtrb3);
	FillTreeADC(HandleTRIGwvfm, HandleGCwvfm, HandleBACwvfm);
	FillTreeSSD(HandleSSDdigit);
      }

      tree->Fill();

    }
    catch(...) {

    }

    return;
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::T0Ana)
