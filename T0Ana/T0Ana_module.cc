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
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "DataQuality/EventQuality.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RawData/WaveForm.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/SSDHit.h"

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
    static const int n_ch_lg = 9; // Number of channels of lg

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


    //  Parameters for SSD
    const int N_bSSD = 2;// Number of SSD station before target (Station 0, 1)
    const int N_sSSD1 = 3;// Number of SSD station between target and magnet (Station 2 - 4)

    int NPlanes = 0;
    static const int NStations = 8;
    static const int MaxPlnsPerSta = 3;
    static const int MaxSensPerPln = 2;
    int fRowGap = 1;//< Maximum allowed gap between strips for forming clusters

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
		     art::Handle< std::vector<rawdata::WaveForm> > &, // BACwvfm
		     art::Handle< std::vector<rawdata::WaveForm> > &); // LGwvfm
    void FillTreeSSD(art::Handle< std::vector<rawdata::SSDRawDigit> > &); //SSD raw data

    static bool CompareByRow(const art::Ptr<emph::rawdata::SSDRawDigit>& a,
			     const art::Ptr<emph::rawdata::SSDRawDigit>& b);
    void SortByRow(art::PtrVector<emph::rawdata::SSDRawDigit>& dl);
    void FormClusters(art::PtrVector<emph::rawdata::SSDRawDigit> sensDigits,
		      std::vector<rb::SSDCluster>* sensClusters,
		      int station, int plane, int sensor);


    void GetT0Tdc(const std::vector<rawdata::TRB3RawDigit>&);
    void GetRPCTdc(const std::vector<rawdata::TRB3RawDigit>&);
    void GetT0Tot(void);
    void GetRPCTot(void);

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    // art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
    art::ServiceHandle<emph::geo::GeometryService> geo;
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
    double TRIG_base; // Baseline of Trigger signal
    double TRIG_blw; // Baseline width of Trigger signal

    std::array<double, n_ch_gc> GC_t; // Pulse time of GasCkov signals
    std::array<double, n_ch_gc> GC_hgt; // Pulse height of GasCkov signals
    std::array<double, n_ch_gc> GC_base; // Baseline of GasCkov signals
    std::array<double, n_ch_gc> GC_blw; // Baseline width of GasCkov signals

    std::array<double, n_ch_bac> BAC_t; // Pulse time of BACkov signals
    std::array<double, n_ch_bac> BAC_hgt; // Pulse height of BACkov signals
    std::array<double, n_ch_bac> BAC_base; // Baseline of BACkov signals
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
    std::array<double, n_seg_t0> ADC_top_base; // Baseline of top signals
    std::array<double, n_seg_t0> ADC_bot_base; // Baseline of bottom signals
    std::array<double, n_seg_t0> ADC_top_blw; // Baseline width of top signals
    std::array<double, n_seg_t0> ADC_bot_blw; // Baseline width of bottom signals

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

    // SSD parameters before target(SSD station 0, 1)
    std::vector<double> bSSD_fer;
    std::vector<double> bSSD_mod;
    std::vector<double> bSSD_row;
    std::vector<double> bSSD_t;
    std::vector<double> bSSD_adc;
    std::vector<double> bSSD_chip;
    std::vector<double> bSSD_set;
    std::vector<double> bSSD_strip;
    std::vector<double> bSSD_st;
    std::vector<double> bSSD_pln;
    std::vector<double> bSSD_hl;

    std::vector<double> bSSD_cl_station;
    std::vector<double> bSSD_cl_plane;
    std::vector<double> bSSD_cl_sens;
    std::vector<double> bSSD_cl_view;
    std::vector<double> bSSD_cl_ndigits;
    std::vector<double> bSSD_cl_width;
    std::vector<double> bSSD_cl_timerange;
    std::vector<double> bSSD_cl_avgadc;
    std::vector<double> bSSD_cl_avgstrip;
    std::vector<double> bSSD_cl_wgtavgstrip;
    std::vector<double> bSSD_cl_wgtrmsstrip;
    std::vector<double> bSSD_cl_ncluster;

    std::vector<double> bSSD_ls_x0_x;
    std::vector<double> bSSD_ls_x0_y;
    std::vector<double> bSSD_ls_x0_z;
    std::vector<double> bSSD_ls_x1_x;
    std::vector<double> bSSD_ls_x1_y;
    std::vector<double> bSSD_ls_x1_z;


    // SSD parameters between target and magnet(SSD station 2 - 4)
    std::vector<double> sSSD1_fer;
    std::vector<double> sSSD1_mod;
    std::vector<double> sSSD1_row;
    std::vector<double> sSSD1_t;
    std::vector<double> sSSD1_adc;
    std::vector<double> sSSD1_chip;
    std::vector<double> sSSD1_set;
    std::vector<double> sSSD1_strip;
    std::vector<double> sSSD1_st;
    std::vector<double> sSSD1_pln;
    std::vector<double> sSSD1_hl;

    std::vector<double> sSSD1_cl_station;
    std::vector<double> sSSD1_cl_plane;
    std::vector<double> sSSD1_cl_sens;
    std::vector<double> sSSD1_cl_view;
    std::vector<double> sSSD1_cl_ndigits;
    std::vector<double> sSSD1_cl_width;
    std::vector<double> sSSD1_cl_timerange;
    std::vector<double> sSSD1_cl_avgadc;
    std::vector<double> sSSD1_cl_avgstrip;
    std::vector<double> sSSD1_cl_wgtavgstrip;
    std::vector<double> sSSD1_cl_wgtrmsstrip;
    std::vector<double> sSSD1_cl_ncluster;

    std::vector<double> sSSD1_ls_x0_x;
    std::vector<double> sSSD1_ls_x0_y;
    std::vector<double> sSSD1_ls_x0_z;
    std::vector<double> sSSD1_ls_x1_x;
    std::vector<double> sSSD1_ls_x1_y;
    std::vector<double> sSSD1_ls_x1_z;


    // SSD parameters after magnet(SSD station 5 - 7)
    std::vector<double> sSSD2_fer;
    std::vector<double> sSSD2_mod;
    std::vector<double> sSSD2_row;
    std::vector<double> sSSD2_t;
    std::vector<double> sSSD2_adc;
    std::vector<double> sSSD2_chip;
    std::vector<double> sSSD2_set;
    std::vector<double> sSSD2_strip;
    std::vector<double> sSSD2_st;
    std::vector<double> sSSD2_pln;
    std::vector<double> sSSD2_hl;

    std::vector<double> sSSD2_cl_station;
    std::vector<double> sSSD2_cl_plane;
    std::vector<double> sSSD2_cl_sens;
    std::vector<double> sSSD2_cl_view;
    std::vector<double> sSSD2_cl_ndigits;
    std::vector<double> sSSD2_cl_width;
    std::vector<double> sSSD2_cl_timerange;
    std::vector<double> sSSD2_cl_avgadc;
    std::vector<double> sSSD2_cl_avgstrip;
    std::vector<double> sSSD2_cl_wgtavgstrip;
    std::vector<double> sSSD2_cl_wgtrmsstrip;
    std::vector<double> sSSD2_cl_ncluster;

    std::vector<double> sSSD2_ls_x0_x;
    std::vector<double> sSSD2_ls_x0_y;
    std::vector<double> sSSD2_ls_x0_z;
    std::vector<double> sSSD2_ls_x1_x;
    std::vector<double> sSSD2_ls_x1_y;
    std::vector<double> sSSD2_ls_x1_z;


    std::array<double, n_ch_lg> LG_t; // Pulse time of LGCalo signals
    std::array<double, n_ch_lg> LG_hgt; // Pulse height of LGCalo signals
    std::array<double, n_ch_lg> LG_base; // Baseline of LGCalo signals
    std::array<double, n_ch_lg> LG_blw; // Baseline width of LGCalo signals

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

    auto emgeo = geo->Geo();
    // NPlanes = emgeo->NSSDPlanes();
    NPlanes = 6;

    bSSD_cl_ncluster.resize(NPlanes);
    sSSD1_cl_ncluster.resize(NPlanes);
    sSSD2_cl_ncluster.resize(NPlanes);

    // create TTree for TRB3RawDigits
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("T0AnaTree","");
    std::cout << "tree = " << tree << std::endl;

    // Branchse for Trigger
    tree->Branch("TRIG_t", &TRIG_t);
    tree->Branch("TRIG_hgt", &TRIG_hgt);
    tree->Branch("TRIG_base", &TRIG_base);
    tree->Branch("TRIG_blw", &TRIG_blw);

    // Branchse for GasCkov
    tree->Branch("GC_t", &GC_t);
    tree->Branch("GC_hgt", &GC_hgt);
    tree->Branch("GC_base", &GC_base);
    tree->Branch("GC_blw", &GC_blw);

    // Branchse for BACkov
    tree->Branch("BAC_t", &BAC_t);
    tree->Branch("BAC_hgt", &BAC_hgt);
    tree->Branch("BAC_base", &BAC_base);
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
    tree->Branch("ADC_top_base", &ADC_top_base);
    tree->Branch("ADC_top_blw", &ADC_top_blw);
    tree->Branch("ADC_bot_ts", &ADC_bot_ts);
    tree->Branch("ADC_bot_t", &ADC_bot_t);
    tree->Branch("ADC_bot_hgt", &ADC_bot_hgt);
    tree->Branch("ADC_bot_base", &ADC_bot_base);
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

    // Branches for bSSD
    tree->Branch("bSSD_fer", &bSSD_fer);
    tree->Branch("bSSD_mod", &bSSD_mod);
    tree->Branch("bSSD_row", &bSSD_row);
    tree->Branch("bSSD_t", &bSSD_t);
    tree->Branch("bSSD_adc", &bSSD_adc);
    tree->Branch("bSSD_chip", &bSSD_chip);
    tree->Branch("bSSD_set", &bSSD_set);
    tree->Branch("bSSD_strip", &bSSD_strip);
    tree->Branch("bSSD_st", &bSSD_st);
    tree->Branch("bSSD_pln", &bSSD_pln);
    tree->Branch("bSSD_hl", &bSSD_hl);

    tree->Branch("bSSD_cl_station", &bSSD_cl_station);
    tree->Branch("bSSD_cl_plane", &bSSD_cl_plane);
    tree->Branch("bSSD_cl_sens", &bSSD_cl_sens);
    tree->Branch("bSSD_cl_view", &bSSD_cl_view);
    tree->Branch("bSSD_cl_ndigits", &bSSD_cl_ndigits);
    tree->Branch("bSSD_cl_width", &bSSD_cl_width);
    tree->Branch("bSSD_cl_timerange", &bSSD_cl_timerange);
    tree->Branch("bSSD_cl_avgadc", &bSSD_cl_avgadc);
    tree->Branch("bSSD_cl_avgstrip", &bSSD_cl_avgstrip);
    tree->Branch("bSSD_cl_wgtavgstrip", &bSSD_cl_wgtavgstrip);
    tree->Branch("bSSD_cl_wgtrmsstrip", &bSSD_cl_wgtrmsstrip);
    tree->Branch("bSSD_cl_ncluster", &bSSD_cl_ncluster);

    tree->Branch("bSSD_ls_x0_x", &bSSD_ls_x0_x);
    tree->Branch("bSSD_ls_x0_y", &bSSD_ls_x0_y);
    tree->Branch("bSSD_ls_x0_z", &bSSD_ls_x0_z);
    tree->Branch("bSSD_ls_x1_x", &bSSD_ls_x1_x);
    tree->Branch("bSSD_ls_x1_y", &bSSD_ls_x1_y);
    tree->Branch("bSSD_ls_x1_z", &bSSD_ls_x1_z);

    // Branches for sSSD1
    tree->Branch("sSSD1_fer", &sSSD1_fer);
    tree->Branch("sSSD1_mod", &sSSD1_mod);
    tree->Branch("sSSD1_row", &sSSD1_row);
    tree->Branch("sSSD1_t", &sSSD1_t);
    tree->Branch("sSSD1_adc", &sSSD1_adc);
    tree->Branch("sSSD1_chip", &sSSD1_chip);
    tree->Branch("sSSD1_set", &sSSD1_set);
    tree->Branch("sSSD1_strip", &sSSD1_strip);
    tree->Branch("sSSD1_st", &sSSD1_st);
    tree->Branch("sSSD1_pln", &sSSD1_pln);
    tree->Branch("sSSD1_hl", &sSSD1_hl);

    tree->Branch("sSSD1_cl_station", &sSSD1_cl_station);
    tree->Branch("sSSD1_cl_plane", &sSSD1_cl_plane);
    tree->Branch("sSSD1_cl_sens", &sSSD1_cl_sens);
    tree->Branch("sSSD1_cl_view", &sSSD1_cl_view);
    tree->Branch("sSSD1_cl_ndigits", &sSSD1_cl_ndigits);
    tree->Branch("sSSD1_cl_width", &sSSD1_cl_width);
    tree->Branch("sSSD1_cl_timerange", &sSSD1_cl_timerange);
    tree->Branch("sSSD1_cl_avgadc", &sSSD1_cl_avgadc);
    tree->Branch("sSSD1_cl_avgstrip", &sSSD1_cl_avgstrip);
    tree->Branch("sSSD1_cl_wgtavgstrip", &sSSD1_cl_wgtavgstrip);
    tree->Branch("sSSD1_cl_wgtrmsstrip", &sSSD1_cl_wgtrmsstrip);
    tree->Branch("sSSD1_cl_ncluster", &sSSD1_cl_ncluster);

    tree->Branch("sSSD1_ls_x0_x", &sSSD1_ls_x0_x);
    tree->Branch("sSSD1_ls_x0_y", &sSSD1_ls_x0_y);
    tree->Branch("sSSD1_ls_x0_z", &sSSD1_ls_x0_z);
    tree->Branch("sSSD1_ls_x1_x", &sSSD1_ls_x1_x);
    tree->Branch("sSSD1_ls_x1_y", &sSSD1_ls_x1_y);
    tree->Branch("sSSD1_ls_x1_z", &sSSD1_ls_x1_z);

    // Branches for sSSD2
    tree->Branch("sSSD2_fer", &sSSD2_fer);
    tree->Branch("sSSD2_mod", &sSSD2_mod);
    tree->Branch("sSSD2_row", &sSSD2_row);
    tree->Branch("sSSD2_t", &sSSD2_t);
    tree->Branch("sSSD2_adc", &sSSD2_adc);
    tree->Branch("sSSD2_chip", &sSSD2_chip);
    tree->Branch("sSSD2_set", &sSSD2_set);
    tree->Branch("sSSD2_strip", &sSSD2_strip);
    tree->Branch("sSSD2_st", &sSSD2_st);
    tree->Branch("sSSD2_pln", &sSSD2_pln);
    tree->Branch("sSSD2_hl", &sSSD2_hl);

    tree->Branch("sSSD2_cl_station", &sSSD2_cl_station);
    tree->Branch("sSSD2_cl_plane", &sSSD2_cl_plane);
    tree->Branch("sSSD2_cl_sens", &sSSD2_cl_sens);
    tree->Branch("sSSD2_cl_view", &sSSD2_cl_view);
    tree->Branch("sSSD2_cl_ndigits", &sSSD2_cl_ndigits);
    tree->Branch("sSSD2_cl_width", &sSSD2_cl_width);
    tree->Branch("sSSD2_cl_timerange", &sSSD2_cl_timerange);
    tree->Branch("sSSD2_cl_avgadc", &sSSD2_cl_avgadc);
    tree->Branch("sSSD2_cl_avgstrip", &sSSD2_cl_avgstrip);
    tree->Branch("sSSD2_cl_wgtavgstrip", &sSSD2_cl_wgtavgstrip);
    tree->Branch("sSSD2_cl_wgtrmsstrip", &sSSD2_cl_wgtrmsstrip);
    tree->Branch("sSSD2_cl_ncluster", &sSSD2_cl_ncluster);

    tree->Branch("sSSD2_ls_x0_x", &sSSD2_ls_x0_x);
    tree->Branch("sSSD2_ls_x0_y", &sSSD2_ls_x0_y);
    tree->Branch("sSSD2_ls_x0_z", &sSSD2_ls_x0_z);
    tree->Branch("sSSD2_ls_x1_x", &sSSD2_ls_x1_x);
    tree->Branch("sSSD2_ls_x1_y", &sSSD2_ls_x1_y);
    tree->Branch("sSSD2_ls_x1_z", &sSSD2_ls_x1_z);

    // Branchse for LGCalo
    tree->Branch("LG_t", &LG_t);
    tree->Branch("LG_hgt", &LG_hgt);
    tree->Branch("LG_base", &LG_base);
    tree->Branch("LG_blw", &LG_blw);

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
      ADC_top_base[i] = -9999.0;
      ADC_bot_base[i] = -9999.0;
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
	  ADC_bot_base[detchan - 1] = wvfm.Baseline();
	  ADC_bot_blw[detchan - 1] = wvfm.BLWidth();
	}else if(detchan > n_seg_t0 && detchan <= n_ch_det_t0){
	  ADC_top_ts[detchan%(n_seg_t0 + 1)] = static_cast<double>(wvfm.FragmentTime());
	  ADC_top_t[detchan%(n_seg_t0 + 1)] = wvfm.PeakTDC();
	  ADC_top_hgt[detchan%(n_seg_t0 + 1)] = wvfm.Baseline()-wvfm.PeakADC();
	  ADC_top_base[detchan%(n_seg_t0 + 1)] = wvfm.Baseline();
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

  void T0Ana::FillTreeADC(art::Handle< std::vector<emph::rawdata::WaveForm> > & TRIGwvfm, art::Handle< std::vector<emph::rawdata::WaveForm> > & GCwvfm, art::Handle< std::vector<emph::rawdata::WaveForm> > & BACwvfm, art::Handle< std::vector<emph::rawdata::WaveForm> > & LGwvfm)
  {

    TRIG_t = -9999.0;
    TRIG_hgt = -9999.0;
    TRIG_base = -9999.0;
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
	TRIG_base = wvfm.Baseline();
	TRIG_blw = wvfm.BLWidth();
      } // end loop over TRIG ADC channels
    }

    for(int i = 0; i < n_ch_gc; i++){
      GC_t[i] = -9999.0;
      GC_hgt[i] = -9999.0;
      GC_base[i] = -9999.0;
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
	GC_base[detchan] = wvfm.Baseline();
	GC_blw[detchan] = wvfm.BLWidth();
      } // end loop over GCkov ADC channels
    }

    for(int i = 0; i < n_ch_bac; i++){
      BAC_t[i] = -9999.0;
      BAC_hgt[i] = -9999.0;
      BAC_base[i] = -9999.0;
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
	BAC_base[detchan] = wvfm.Baseline();
	BAC_blw[detchan] = wvfm.BLWidth();
      } // end loop over BACkov ADC channels
    }

    for(int i = 0; i < n_ch_lg; i++){
      LG_t[i] = -9999.0;
      LG_hgt[i] = -9999.0;
      LG_base[i] = -9999.0;
      LG_blw[i] = -1.0;
    }

    // get ADC info for LG
    if(!LGwvfm->empty()) {
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel LGechan;
      LGechan.SetBoardType(boardType);
      // loop over ADC channels
      for (size_t idx=0; idx < LGwvfm->size(); ++idx){
	const rawdata::WaveForm& wvfm = (*LGwvfm)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	LGechan.SetBoard(board);
	LGechan.SetChannel(chan);
	emph::cmap::DChannel dchan = cmap->DetChan(LGechan);
	int detchan = dchan.Channel();
	LG_t[detchan] = wvfm.PeakTDC();
	LG_hgt[detchan] = wvfm.Baseline()-wvfm.PeakADC();
	LG_base[detchan] = wvfm.Baseline();
	LG_blw[detchan] = wvfm.BLWidth();
      } // end loop over LGCalo ADC channels
    }
  }

  //......................................................................
  bool T0Ana::CompareByRow(const art::Ptr<emph::rawdata::SSDRawDigit>& a,
			   const art::Ptr<emph::rawdata::SSDRawDigit>& b)
  {
    return a->Row() < b->Row();
  }

  //......................................................................
  void T0Ana::SortByRow(art::PtrVector<emph::rawdata::SSDRawDigit>& dl)
  {
    std::stable_sort(dl.begin(), dl.end(), CompareByRow);
  }

  //......................................................................
  void T0Ana::FormClusters(art::PtrVector<emph::rawdata::SSDRawDigit> sensDigits,
			   std::vector<rb::SSDCluster>* sensClusters,
			   int station, int plane, int sensor)
  { 
    auto emgeo = geo->Geo();
  
    auto gSt = emgeo->GetSSDStation(station);
    auto gPl = gSt->GetPlane(plane);
    auto gD = gPl->SSD(sensor);
  
    geo::sensorView view = gD->View();

    int prevRow=sensDigits[0]->Row();
    int curRow;
    rb::SSDCluster ssdClust;
    int i=0;

    // loop over digits on sensor
    for (auto & dig : sensDigits) {
      curRow = dig->Row();
      // if gap too big, push cluster and clear it
      if ( curRow-prevRow > (fRowGap) ) {
	ssdClust.SetStation(station);
	ssdClust.SetSensor(sensor);
	ssdClust.SetPlane(plane);
	ssdClust.SetView(view);
	ssdClust.SetPlane(plane);
	sensClusters->push_back(ssdClust);
	ssdClust = rb::SSDCluster();
      }
      // add current digit to cluster
      ssdClust.Add(dig);
      prevRow=curRow;
    }

    // push last cluster
    ssdClust.SetStation(station);
    ssdClust.SetSensor(sensor);
    ssdClust.SetPlane(plane);
    ssdClust.SetView(view);
    ssdClust.SetPlane(plane);
    sensClusters->push_back(ssdClust);
  }

  void T0Ana::FillTreeSSD(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > & SSDdigit)
  {
    // Clear bSSD
    bSSD_fer.clear();
    bSSD_mod.clear();
    bSSD_row.clear();
    bSSD_t.clear();
    bSSD_adc.clear();
    bSSD_chip.clear();
    bSSD_set.clear();
    bSSD_strip.clear();

    bSSD_st.clear();
    bSSD_pln.clear();
    bSSD_hl.clear();

    bSSD_cl_station.clear();
    bSSD_cl_plane.clear();
    bSSD_cl_sens.clear();
    bSSD_cl_view.clear();
    bSSD_cl_ndigits.clear();
    bSSD_cl_width.clear();
    bSSD_cl_timerange.clear();
    bSSD_cl_avgadc.clear();
    bSSD_cl_avgstrip.clear();
    bSSD_cl_wgtavgstrip.clear();
    bSSD_cl_wgtrmsstrip.clear();
    std::fill_n(bSSD_cl_ncluster.begin(),bSSD_cl_ncluster.size(),0);

    bSSD_ls_x0_x.clear();
    bSSD_ls_x0_y.clear();
    bSSD_ls_x0_z.clear();
    bSSD_ls_x1_x.clear();
    bSSD_ls_x1_y.clear();
    bSSD_ls_x1_z.clear();

    // Clear sSSD1
    sSSD1_fer.clear();
    sSSD1_mod.clear();
    sSSD1_row.clear();
    sSSD1_t.clear();
    sSSD1_adc.clear();
    sSSD1_chip.clear();
    sSSD1_set.clear();
    sSSD1_strip.clear();

    sSSD1_st.clear();
    sSSD1_pln.clear();
    sSSD1_hl.clear();

    sSSD1_cl_station.clear();
    sSSD1_cl_plane.clear();
    sSSD1_cl_sens.clear();
    sSSD1_cl_view.clear();
    sSSD1_cl_ndigits.clear();
    sSSD1_cl_width.clear();
    sSSD1_cl_timerange.clear();
    sSSD1_cl_avgadc.clear();
    sSSD1_cl_avgstrip.clear();
    sSSD1_cl_wgtavgstrip.clear();
    sSSD1_cl_wgtrmsstrip.clear();
    std::fill_n(sSSD1_cl_ncluster.begin(),sSSD1_cl_ncluster.size(),0);

    sSSD1_ls_x0_x.clear();
    sSSD1_ls_x0_y.clear();
    sSSD1_ls_x0_z.clear();
    sSSD1_ls_x1_x.clear();
    sSSD1_ls_x1_y.clear();
    sSSD1_ls_x1_z.clear();

    // Clear sSSD2
    sSSD2_fer.clear();
    sSSD2_mod.clear();
    sSSD2_row.clear();
    sSSD2_t.clear();
    sSSD2_adc.clear();
    sSSD2_chip.clear();
    sSSD2_set.clear();
    sSSD2_strip.clear();

    sSSD2_st.clear();
    sSSD2_pln.clear();
    sSSD2_hl.clear();

    sSSD2_cl_station.clear();
    sSSD2_cl_plane.clear();
    sSSD2_cl_sens.clear();
    sSSD2_cl_view.clear();
    sSSD2_cl_ndigits.clear();
    sSSD2_cl_width.clear();
    sSSD2_cl_timerange.clear();
    sSSD2_cl_avgadc.clear();
    sSSD2_cl_avgstrip.clear();
    sSSD2_cl_wgtavgstrip.clear();
    sSSD2_cl_wgtrmsstrip.clear();
    std::fill_n(sSSD2_cl_ncluster.begin(),sSSD2_cl_ncluster.size(),0);

    sSSD2_ls_x0_x.clear();
    sSSD2_ls_x0_y.clear();
    sSSD2_ls_x0_z.clear();
    sSSD2_ls_x1_x.clear();
    sSSD2_ls_x1_y.clear();
    sSSD2_ls_x1_z.clear();


    art::PtrVector<emph::rawdata::SSDRawDigit> digitList[NStations][MaxPlnsPerSta][MaxSensPerPln];

    // get SSD info
    if(!SSDdigit->empty()){
      emph::cmap::FEBoardType boardType = emph::cmap::SSD;
      emph::cmap::EChannel SSDechan;
      SSDechan.SetBoardType(boardType);
      for(size_t idx=0; idx < SSDdigit->size(); ++idx){
	art::Ptr<emph::rawdata::SSDRawDigit> ssd(SSDdigit, idx);
	SSDechan.SetBoard(ssd->FER());
	SSDechan.SetChannel(ssd->Module());
	emph::cmap::DChannel dchan = cmap->DetChan(SSDechan);
	int detst = dchan.Station();
	int detpln = dchan.Plane();
	int dethl = dchan.HiLo();

	if(detst < N_bSSD){
	  bSSD_fer.push_back(ssd->FER());
	  bSSD_mod.push_back(ssd->Module());
	  bSSD_row.push_back(ssd->Row());
	  bSSD_t.push_back(ssd->Time());
	  bSSD_adc.push_back(ssd->ADC());
	  bSSD_chip.push_back(ssd->Chip());
	  bSSD_set.push_back(ssd->Set());
	  bSSD_strip.push_back(ssd->Strip());

	  bSSD_st.push_back(detst);
	  bSSD_pln.push_back(detpln);
	  bSSD_hl.push_back(dethl);
	}else if(detst < N_bSSD + N_sSSD1){
	  sSSD1_fer.push_back(ssd->FER());
	  sSSD1_mod.push_back(ssd->Module());
	  sSSD1_row.push_back(ssd->Row());
	  sSSD1_t.push_back(ssd->Time());
	  sSSD1_adc.push_back(ssd->ADC());
	  sSSD1_chip.push_back(ssd->Chip());
	  sSSD1_set.push_back(ssd->Set());
	  sSSD1_strip.push_back(ssd->Strip());

	  sSSD1_st.push_back(detst);
	  sSSD1_pln.push_back(detpln);
	  sSSD1_hl.push_back(dethl);
	}else{
	  sSSD2_fer.push_back(ssd->FER());
	  sSSD2_mod.push_back(ssd->Module());
	  sSSD2_row.push_back(ssd->Row());
	  sSSD2_t.push_back(ssd->Time());
	  sSSD2_adc.push_back(ssd->ADC());
	  sSSD2_chip.push_back(ssd->Chip());
	  sSSD2_set.push_back(ssd->Set());
	  sSSD2_strip.push_back(ssd->Strip());

	  sSSD2_st.push_back(detst);
	  sSSD2_pln.push_back(detpln);
	  sSSD2_hl.push_back(dethl);
	}//if(detst region)

	digitList[dchan.Station()][dchan.Plane()][dchan.HiLo()].push_back(ssd);
      } // end loop over SSDRawDigit


      std::vector<rb::SSDCluster> clusters;
      // Should really pull counts of these from geometry somehow
      for (int sta=0; sta<NStations; ++sta){
	for (int pln=0; pln<MaxPlnsPerSta; ++pln){
	  for (int sensor=0; sensor<MaxSensPerPln; ++sensor){
	    clusters.clear();
	    // Don't bother to cluster if we didn't have any raw digits
	    if (digitList[sta][pln][sensor].size()==0)
	      continue;
	    // FormClusters() assumes digits are ordered by row
	    this->SortByRow(digitList[sta][pln][sensor]);
	    this->FormClusters(digitList[sta][pln][sensor],
			       &clusters, sta, pln, sensor);


	    for (int i=0; i<(int)clusters.size(); i++){
	      // fill vectors for optimizing algorithm. This part of module should be removed once it's more finalized.
	      if(clusters[i].Station() < N_bSSD){
		bSSD_cl_station.push_back(clusters[i].Station());
		bSSD_cl_plane.push_back(clusters[i].Plane());
		bSSD_cl_sens.push_back(clusters[i].Sensor());
		bSSD_cl_view.push_back(clusters[i].View());
		bSSD_cl_ndigits.push_back(clusters[i].NDigits());
		bSSD_cl_width.push_back(clusters[i].Width());
		bSSD_cl_timerange.push_back(clusters[i].TimeRange());
		bSSD_cl_avgadc.push_back(clusters[i].AvgADC());
		bSSD_cl_avgstrip.push_back(clusters[i].AvgStrip());
		bSSD_cl_wgtavgstrip.push_back(clusters[i].WgtAvgStrip());
		bSSD_cl_wgtrmsstrip.push_back(clusters[i].WgtRmsStrip());
		int plane = clusters[i].Plane();
		bSSD_cl_ncluster[plane]++;
	      }else if(clusters[i].Station() < N_bSSD + N_sSSD1){
		sSSD1_cl_station.push_back(clusters[i].Station());
		sSSD1_cl_plane.push_back(clusters[i].Plane());
		sSSD1_cl_sens.push_back(clusters[i].Sensor());
		sSSD1_cl_view.push_back(clusters[i].View());
		sSSD1_cl_ndigits.push_back(clusters[i].NDigits());
		sSSD1_cl_width.push_back(clusters[i].Width());
		sSSD1_cl_timerange.push_back(clusters[i].TimeRange());
		sSSD1_cl_avgadc.push_back(clusters[i].AvgADC());
		sSSD1_cl_avgstrip.push_back(clusters[i].AvgStrip());
		sSSD1_cl_wgtavgstrip.push_back(clusters[i].WgtAvgStrip());
		sSSD1_cl_wgtrmsstrip.push_back(clusters[i].WgtRmsStrip());
		int plane = clusters[i].Plane();
		sSSD1_cl_ncluster[plane]++;
	      }else{
		sSSD2_cl_station.push_back(clusters[i].Station());
		sSSD2_cl_plane.push_back(clusters[i].Plane());
		sSSD2_cl_sens.push_back(clusters[i].Sensor());
		sSSD2_cl_view.push_back(clusters[i].View());
		sSSD2_cl_ndigits.push_back(clusters[i].NDigits());
		sSSD2_cl_width.push_back(clusters[i].Width());
		sSSD2_cl_timerange.push_back(clusters[i].TimeRange());
		sSSD2_cl_avgadc.push_back(clusters[i].AvgADC());
		sSSD2_cl_avgstrip.push_back(clusters[i].AvgStrip());
		sSSD2_cl_wgtavgstrip.push_back(clusters[i].WgtAvgStrip());
		sSSD2_cl_wgtrmsstrip.push_back(clusters[i].WgtRmsStrip());
		int plane = clusters[i].Plane();
		sSSD2_cl_ncluster[plane]++;
	      }//if(clusters[i].Station() region)
	      // clusters[i].SetID(i);
	      // clusterv->push_back(clusters[i]);

	      // find line segment for each cluster
	      // check first for reasonable cluster (hack for now, need better checks earlier on)
	      rb::LineSegment lineseg_tmp = rb::LineSegment();

	      // if (clusters[i].AvgStrip() > 640){
	      // 	std::cout<<"Skipping nonsense"<<std::endl;

		// linesegv->push_back(lineseg_tmp);
	      if(clusters[i].Station() < N_bSSD){
		bSSD_ls_x0_x.push_back(lineseg_tmp.X0()[0]);
		bSSD_ls_x0_y.push_back(lineseg_tmp.X0()[1]);
		bSSD_ls_x0_z.push_back(lineseg_tmp.X0()[2]);
		bSSD_ls_x1_x.push_back(lineseg_tmp.X1()[0]);
		bSSD_ls_x1_y.push_back(lineseg_tmp.X1()[1]);
		bSSD_ls_x1_z.push_back(lineseg_tmp.X1()[2]);
	      }else if(clusters[i].Station() < N_bSSD + N_sSSD1){
		sSSD1_ls_x0_x.push_back(lineseg_tmp.X0()[0]);
		sSSD1_ls_x0_y.push_back(lineseg_tmp.X0()[1]);
		sSSD1_ls_x0_z.push_back(lineseg_tmp.X0()[2]);
		sSSD1_ls_x1_x.push_back(lineseg_tmp.X1()[0]);
		sSSD1_ls_x1_y.push_back(lineseg_tmp.X1()[1]);
		sSSD1_ls_x1_z.push_back(lineseg_tmp.X1()[2]);
	      }else{
		sSSD2_ls_x0_x.push_back(lineseg_tmp.X0()[0]);
		sSSD2_ls_x0_y.push_back(lineseg_tmp.X0()[1]);
		sSSD2_ls_x0_z.push_back(lineseg_tmp.X0()[2]);
		sSSD2_ls_x1_x.push_back(lineseg_tmp.X1()[0]);
		sSSD2_ls_x1_y.push_back(lineseg_tmp.X1()[1]);
		sSSD2_ls_x1_z.push_back(lineseg_tmp.X1()[2]);
	      }//if(clusters[i].Station() region)

	      // 	continue;
	      // }//if(AveStrip > 640)

	      // if(dgm->Map()->SSDClusterToLineSegment(clusters[i], lineseg_tmp)){
	      // 	// linesegv->push_back(lineseg_tmp);
	      // 	SSD_ls_x0_x.push_back(lineseg_tmp.X0()[0]);
	      // 	SSD_ls_x0_y.push_back(lineseg_tmp.X0()[1]);
	      // 	SSD_ls_x0_z.push_back(lineseg_tmp.X0()[2]);
	      // 	SSD_ls_x1_x.push_back(lineseg_tmp.X0()[0]);
	      // 	SSD_ls_x1_y.push_back(lineseg_tmp.X0()[1]);
	      // 	SSD_ls_x1_z.push_back(lineseg_tmp.X0()[2]);
	      // }else{
	      // 	std::cout<<"Couldn't make line segment from Cluster?!?"<<std::endl;

	      // 	lineseg_tmp = rb::LineSegment();//Regenerate blank LineSegment

	      // 	SSD_ls_x0_x.push_back(lineseg_tmp.X0()[0]);
	      // 	SSD_ls_x0_y.push_back(lineseg_tmp.X0()[1]);
	      // 	SSD_ls_x0_z.push_back(lineseg_tmp.X0()[2]);
	      // 	SSD_ls_x1_x.push_back(lineseg_tmp.X1()[0]);
	      // 	SSD_ls_x1_y.push_back(lineseg_tmp.X1()[1]);
	      // 	SSD_ls_x1_z.push_back(lineseg_tmp.X1()[2]);
	      // }//if(SSDClusterToLineSegment)
	    }//for(i:cluster.size())
	  }//for(sensor:MaxSensPerPln)
	}//for(pln:MaxPlnsPerSta)
      }//for(sta<NStations)

    }
  }

  //......................................................................
  void T0Ana::analyze(const art::Event& evt)
  {
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();
    std::string labelStrTRIG, labelStrGC, labelStrBAC, labelStrT0, labelStrRPC, labelStrSSD, labelStrLG;

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

    int i_lg = emph::geo::LGCalo;
    labelStrLG = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i_lg));
    art::Handle< std::vector<emph::rawdata::WaveForm> > HandleLGwvfm;

    try {
      evt.getByLabel(labelStrTRIG, HandleTRIGwvfm);
      evt.getByLabel(labelStrGC, HandleGCwvfm);
      evt.getByLabel(labelStrBAC, HandleBACwvfm);
      evt.getByLabel(labelStrT0, HandleT0wvfm);
      evt.getByLabel(labelStrT0, HandleT0trb3);
      evt.getByLabel(labelStrRPC, HandleRPCtrb3);
      evt.getByLabel(labelStrSSD, HandleSSDdigit);
      evt.getByLabel(labelStrLG, HandleLGwvfm);

      if (!HandleT0trb3->empty()) {
	FillTreeTOF(HandleT0wvfm, HandleT0trb3, HandleRPCtrb3);
	FillTreeADC(HandleTRIGwvfm, HandleGCwvfm, HandleBACwvfm, HandleLGwvfm);
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
