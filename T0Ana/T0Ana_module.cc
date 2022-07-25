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
#include "ChannelMap/ChannelMap.h"
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
    double trb3_linear_low      = 15.0; // Calibration for low end
    double trb3_linear_high_T0  = 499.0; // Calibration for FPGA2 -- T0
    double trb3_linear_high_RPC = 491.0; // Calibration for FPGA3 -- RPC

    const int id_seg_ctr_rpc = 4;
    const int id_seg_ctr_t0  = 5;

    bool   FindSegT0(int);
    bool   FindSegLeadT0(int);
    bool   FindSegRPC(int);
    bool   FindSegLeadRPC(int);
    std::array<int, 2> GetSegChT0(int);
    std::array<int, 2> GetSegChRPC(int);
    void   FillT0AnaTree(art::Handle< std::vector<rawdata::WaveForm> > &,
			 art::Handle< std::vector<rawdata::TRB3RawDigit> > &,
			 art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
    std::array<double, N_SEG_T0>    GetT0Tot(const std::vector<rawdata::TRB3RawDigit>&, bool);
    std::array<double, N_SEG_T0>    GetT0Tdc(const std::vector<rawdata::TRB3RawDigit>&);
    std::array<double, N_SEG_RPC>   GetRPCTot(const std::vector<rawdata::TRB3RawDigit>&, bool);
    std::array<double, N_SEG_RPC>   GetRPCTdc(const std::vector<rawdata::TRB3RawDigit>&);

    emph::cmap::ChannelMap* fChannelMap;
    std::string fChanMapFileName;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;

    std::array<double, n_ch_t0> time_ch_t0;
    std::array<bool,   n_ch_t0> found_ch_t0;
    std::array<double, n_ch_rpc> time_ch_rpc;
    std::array<bool,   n_ch_rpc> found_ch_rpc;
    std::array<double, n_seg_t0> T0_tot_seg;
    std::array<double, n_seg_t0> T0_tdc;
    std::array<double, n_seg_rpc> RPC_tot_seg;
    std::array<double, n_seg_rpc> RPC_tdc;

    TTree *tree;
    std::array<double, n_seg_t0> ADCqt; // Charge of top signals
    std::array<double, n_seg_t0> ADCqb; // Charge of bottom signals
    std::array<double, n_seg_t0> ADCmaxt; // Pulse height of top signals
    std::array<double, n_seg_t0> ADCmaxb; // Pulse height of bottom signals
    std::array<double, n_seg_t0> ADCblwt; // Baseline of top signals
    std::array<double, n_seg_t0> ADCblwb; // Baseline of bottom signals

    std::array<double, n_seg_t0> TDCt; // Average times of top and bottom signals
    std::array<double, n_seg_t0> TDCtott; // TOT of top signals
    std::array<double, n_seg_t0> TDCtotb; // TOT of bottom signals

    std::array<double, n_seg_rpc> RPCt; // Average times of top and bottom signals of RPC
    std::array<double, n_seg_rpc> RPCtotl; // TOT of left signals of RPC
    std::array<double, n_seg_rpc> RPCtotr; // TOT of eignt signals of RPC

    std::array<double, n_seg_t0> T0tof; // TOF between T0 and RPC with fixed RPC segment
    std::array<double, n_seg_rpc> RPCtof; // TOF between T0 and RPC with fixed T0 segment

    int test = 1;

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

    // create TTree for TRB3RawDigits
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("T0AnaTree","");
    std::cout << "tree = " << tree << std::endl;

    tree->Branch("qt", &ADCqt);
    tree->Branch("qb", &ADCqb);
    tree->Branch("qmaxt", &ADCmaxt);
    tree->Branch("qmaxb", &ADCmaxb);
    tree->Branch("qblwt", &ADCblwt);
    tree->Branch("qblwb", &ADCblwb);

    tree->Branch("t", &TDCt);
    tree->Branch("ttott", &TDCtott);
    tree->Branch("ttotb", &TDCtotb);

    tree->Branch("rpct", &RPCt);
    tree->Branch("rpctotl", &RPCtotl);
    tree->Branch("rpctotr", &RPCtotr);

    tree->Branch("t0tof", &T0tof);
    tree->Branch("rpctof", &RPCtof);

    std::cout << "tree = " << tree << std::endl;
    
  }

  //......................................................................
  void T0Ana::endJob()
  {
  }

  //......................................................................
  bool T0Ana::FindSegT0(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 40) return true;

    return false;
  }

  //......................................................................
  bool T0Ana::FindSegLeadT0(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 40 && ch_daq%2 == 1) return true;

    return false;
  }

  //......................................................................
  bool T0Ana::FindSegRPC(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 32) return true;

    return false;
  }

  //......................................................................
  bool T0Ana::FindSegLeadRPC(int ch_daq)
  {
    if (ch_daq >= 1 && ch_daq <= 32 && ch_daq%2 == 1) return true;

    return false;
  }

  //......................................................................
  std::array<int, 2> T0Ana::GetSegChT0(int seg_id)
  {
    if(seg_id == 0){
      return { 1, 21};
    }else if(seg_id == 1){
      return { 3, 23};
    }else if(seg_id == 2){
      return { 5, 25};
    }else if(seg_id == 3){
      return { 7, 27};
    }else if(seg_id == 4){
      return { 9, 29};
    }else if(seg_id == 5){
      return {11, 31};
    }else if(seg_id == 6){
      return {13, 35};
    }else if(seg_id == 7){
      return {15, 33};
    }else if(seg_id == 8){
      return {17, 37};
    }else{
      return {19, 39};
    }
  }

  //......................................................................
  std::array<int, 2> T0Ana::GetSegChRPC(int seg_id)
  {
    if(seg_id == 0){
      return { 1, 17};
    }else if(seg_id == 1){
      return { 3, 19};
    }else if(seg_id == 2){
      return { 5, 21};
    }else if(seg_id == 3){
      return { 7, 23};
    }else if(seg_id == 4){
      return { 9, 25};
    }else if(seg_id == 5){
      return {11, 27};
    }else if(seg_id == 6){
      return {13, 29};
    }else{
      return {15, 31};
    }
  }

  //......................................................................
  std::array<double, N_SEG_T0> T0Ana::GetT0Tot(const std::vector<rawdata::TRB3RawDigit>& digvec, bool sel_top = true)
  {
    for(int i_ch = 0; i_ch < n_ch_t0; i_ch++){
      time_ch_t0[i_ch]  = 0;
      found_ch_t0[i_ch] = false;
    }

    for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
      T0_tot_seg[i_seg]  = -999.0;
    }

    //loop over TRB3 signals
    int n_vec = digvec.size();
    for(int i_vec = 0; i_vec < n_vec; i_vec++){
      uint32_t evt_ch = digvec.at(i_vec).GetChannel();

      if(FindSegT0(evt_ch) && (!found_ch_t0[evt_ch])){
	time_ch_t0[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                    + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                    + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0 - trb3_linear_low);
	found_ch_t0[evt_ch] = true;
      }
    }//end loop over T0 TRB3 signals

    //Calculation of TOT, loop over T0 segments
    if(sel_top){
      for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
	if(found_ch_t0[2*i_seg + 2*n_seg_t0 + 1] && found_ch_t0[2*i_seg + 2*n_seg_t0 + 2]){
	  T0_tot_seg[i_seg] = time_ch_t0[2*i_seg + 2*n_seg_t0 + 1] - time_ch_t0[2*i_seg + 2*n_seg_t0 + 2];
	}
      }//end loop over T0 segments for top
    }else{
      for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
	if(found_ch_t0[2*i_seg + 1] && found_ch_t0[2*i_seg + 2]){
	  T0_tot_seg[i_seg] = time_ch_t0[2*i_seg + 1] - time_ch_t0[2*i_seg + 2];
	}
      }//end loop over T0 segments for bottom
    }

    return T0_tot_seg;
  }

  //......................................................................
  std::array<double, N_SEG_T0> T0Ana::GetT0Tdc(const std::vector<rawdata::TRB3RawDigit>& digvec)
  {
    for(int i_ch = 0; i_ch < n_ch_t0; i_ch++){
      time_ch_t0[i_ch]  = 0;
      found_ch_t0[i_ch] = false;
    }

    for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
      T0_tdc[i_seg]  = -100000000.0;
    }

    //loop over TRB3 signals
    int n_vec = digvec.size();
    for(int i_vec = 0; i_vec < n_vec; i_vec++){
      uint32_t evt_ch = digvec.at(i_vec).GetChannel();

      if(evt_ch == 0 && (!found_ch_t0[0])){
	time_ch_t0[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                    + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                    + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0 - trb3_linear_low);
	found_ch_t0[evt_ch] = true;
      }else if(FindSegLeadT0(evt_ch) && (!found_ch_t0[evt_ch])){
	time_ch_t0[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                    + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                    + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0 - trb3_linear_low);
	found_ch_t0[evt_ch] = true;
      }
    }//end loop over T0 TRB3 signals

    //Calculation of tdc, loop over T0 segments
    if(found_ch_t0[0]){
      for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
	std::array<int, 2> ch_seg = GetSegChT0(i_seg);
	if(found_ch_t0[ch_seg[0]] && found_ch_t0[ch_seg[1]]){
	  T0_tdc[i_seg] = (time_ch_t0[ch_seg[0]] + time_ch_t0[ch_seg[1]])/2.0 - time_ch_t0[0];
	}
      }//end loop over T0 leading channels
    }

    return T0_tdc;
  }

  //......................................................................
  std::array<double, N_SEG_RPC> T0Ana::GetRPCTot(const std::vector<rawdata::TRB3RawDigit>& digvec, bool sel_left = true)
  {
    for(int i_ch = 0; i_ch < n_ch_rpc; i_ch++){
      time_ch_rpc[i_ch]  = 0;
      found_ch_rpc[i_ch] = false;
    }

    for(int i_seg = 0; i_seg < n_seg_t0; i_seg++){
      RPC_tot_seg[i_seg]  = -999.0;
    }

    //loop over TRB3 signals
    int n_vec = digvec.size();
    for(int i_vec = 0; i_vec < n_vec; i_vec++){
      uint32_t evt_ch = digvec.at(i_vec).GetChannel();

      if(FindSegRPC(evt_ch) && (!found_ch_rpc[evt_ch])){
	time_ch_rpc[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                     + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                     + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_RPC - trb3_linear_low);
	found_ch_rpc[evt_ch] = true;
      }
    }//end loop over RPC TRB3 signals

    //Calculation of TOT, loop over RPC segments
    if(sel_left){
      for(int i_seg = 0; i_seg < n_seg_rpc; i_seg++){
	if(found_ch_rpc[2*i_seg + 1] && found_ch_rpc[2*i_seg + 2]){
	  RPC_tot_seg[i_seg] = time_ch_rpc[2*i_seg + 1] - time_ch_rpc[2*i_seg + 2];
	}
      }//end loop over RPC segments for top
    }else{
      for(int i_seg = 0; i_seg < n_seg_rpc; i_seg++){
	if(found_ch_rpc[2*i_seg + 2*n_seg_rpc + 1] && found_ch_rpc[2*i_seg + 2*n_seg_rpc + 1]){
	  RPC_tot_seg[i_seg] = time_ch_rpc[2*i_seg + 2*n_seg_rpc + 1] - time_ch_rpc[2*i_seg + 2*n_seg_rpc + 2];
	}
      }//end loop over RPC segments for bottom
    }

    return RPC_tot_seg;
  }

  //......................................................................
  std::array<double, N_SEG_RPC> T0Ana::GetRPCTdc(const std::vector<rawdata::TRB3RawDigit>& digvec)
  {
    for(int i_ch = 0; i_ch < n_ch_rpc; i_ch++){
      time_ch_rpc[i_ch]  = 0;
      found_ch_rpc[i_ch] = false;
    }

    for(int i_seg = 0; i_seg < n_seg_rpc; i_seg++){
      RPC_tdc[i_seg]  = -100000000.0;
    }

    //loop over TRB3 signals
    int n_vec = digvec.size();
    for(int i_vec = 0; i_vec < n_vec; i_vec++){
      uint32_t evt_ch = digvec.at(i_vec).GetChannel();

      if(evt_ch == 0 && (!found_ch_rpc[0])){
	time_ch_rpc[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                     + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                     + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_RPC - trb3_linear_low);
	found_ch_rpc[evt_ch] = true;
      }else if(FindSegLeadRPC(evt_ch) && (!found_ch_rpc[evt_ch])){
	time_ch_rpc[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                     + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                     + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_RPC - trb3_linear_low);
	found_ch_rpc[evt_ch] = true;
      }
    }//end loop over RPC TRB3 signals

    //Calculation of tdc, loop over RPC segments
    if(found_ch_rpc[0]){
      for(int i_seg = 0; i_seg < n_seg_rpc; i_seg++){
	std::array<int, 2> ch_seg = GetSegChRPC(i_seg);
	if(found_ch_rpc[ch_seg[0]] && found_ch_rpc[ch_seg[1]]){
	  RPC_tdc[i_seg] = (time_ch_rpc[ch_seg[0]] + time_ch_rpc[ch_seg[1]])/2.0 - time_ch_rpc[0];
	}
      }//end loop over RPC leading channels
    }

    return RPC_tdc;
  }

  //......................................................................

  void T0Ana::FillT0AnaTree(art::Handle< std::vector<emph::rawdata::WaveForm> > & T0wvfm, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & T0trb3, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & RPCtrb3)
  {
    for(int i = 0; i < n_seg_t0; i++){
      ADCqt[i] = -999999.0;
      ADCqb[i] = -999999.0;
      ADCmaxt[i] = -9999.0;
      ADCmaxb[i] = -9999.0;
      ADCblwt[i] = -1.0;
      ADCblwb[i] = -1.0;

      TDCt[i]    = -100000000.0;
      TDCtott[i] = -999.0;
      TDCtotb[i] = -999.0;

      T0tof[i]   = -100000000.0;
    }

    for(int i = 0; i < n_seg_rpc; i++){
      RPCt[i]    = -100000000.0;
      RPCtotl[i] = -999.0;
      RPCtotr[i] = -999.0;

      RPCtof[i]  = -100000000.0;
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
	emph::cmap::DChannel dchan = fChannelMap->DetChan(T0echan);
	int detchan = dchan.Channel();
	if (detchan > 0 && detchan <= n_seg_t0){
	  ADCqb[detchan - 1] = wvfm.Charge();
	  ADCmaxb[detchan - 1] = wvfm.Baseline()-wvfm.PeakADC();
	  ADCblwb[detchan - 1] = wvfm.BLWidth();
	}else if(detchan > n_seg_t0 && detchan <= n_ch_det_t0){
	  ADCqt[detchan%n_seg_t0] = wvfm.Charge();
	  ADCmaxt[detchan%n_seg_t0] = wvfm.Baseline()-wvfm.PeakADC();
	  ADCblwt[detchan%n_seg_t0] = wvfm.BLWidth();
	}
      } // end loop over T0 ADC channels
    }

    // get TDC info for T0
    if (!T0trb3->empty()) {
      std::vector<emph::rawdata::TRB3RawDigit> VecT0trb3 = *T0trb3;
      TDCt = GetT0Tdc(VecT0trb3);
      TDCtott = GetT0Tot(VecT0trb3, true);
      TDCtotb = GetT0Tot(VecT0trb3, false);
    }

    // get TDC info for RPC
    if (!RPCtrb3->empty()) {
      std::vector<emph::rawdata::TRB3RawDigit> VecRPCtrb3 = *RPCtrb3;
      RPCt = GetRPCTdc(VecRPCtrb3);
      RPCtotl = GetRPCTot(VecRPCtrb3, true);
      RPCtotr = GetRPCTot(VecRPCtrb3, false);
    }

    //TOF calculation between T0 and RPC, loop over T0 segments
    if(RPCt[id_seg_ctr_rpc] > -100000000.0){ // Center segment of RPC
      for(int i_seg_t0 = 0; i_seg_t0 < n_seg_t0; i_seg_t0++){
	if(TDCt[i_seg_t0] > -100000000.0){
	  T0tof[i_seg_t0] = RPCt[id_seg_ctr_rpc] - TDCt[i_seg_t0];
	}
      }
    }// end loop over T0 segments

    //TOF calculation between T0 and RPC, loop over RPC segments
    if(TDCt[id_seg_ctr_t0] > -100000000.0){ // Center segment of T0
      for(int i_seg_rpc = 0; i_seg_rpc < n_seg_rpc; i_seg_rpc++){
	if(RPCt[i_seg_rpc] > -100000000.0){
	  RPCtof[i_seg_rpc] = RPCt[i_seg_rpc] - TDCt[id_seg_ctr_t0];
	}
      }
    }// end loop over rpc segments

    if(fNEvents%10000 == 1){
      std::cout << "#D: Tree fill: Event" << fNEvents << std::endl;
    }

    tree->Fill();

    if(fNEvents%10000 == 1){
      std::cout << "#D: Tree filled: Event" << fNEvents << std::endl;
    }

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
