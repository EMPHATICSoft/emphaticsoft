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
      const int n_seg = 10;
      const int n_ch = 41; // Number of all channels for T0
      const int n_ch_det = 20; // Number of leading channels for T0
      const int RPC_board = 1283;
      const int T0_board = 1282;

      const double epoch_const = 10240026.0; // Constant for epochtime
      const double coarse_const = 5000.0; // Constant for coarsetime
      double trb3_linear_low = 15.0; // Calibration for low end
      double trb3LinearHighEnd = 499.0; // Calibration for FPGA2 -- T0
      double trb3LinearHighEnd_RPC = 491.0; // Calibration for FPGA3 -- RPC
      // std::array<double, 2> trb3_linear_high = {499.0, 491.0}; // Calibration for FPGA2 -- T0 and FPGA3 -- RPC

      bool   FindSeg(int);
      bool   FindTrailBot(int);
      bool   FindTrailTop(int);
      bool   FindLeadBot(int);
      bool   FindLeadTop(int);
      int    GetSegIdTrb3(int);
      std::array<int, 2> GetSegChTrb3(int);
      void   FillT0AnaTree(art::Handle< std::vector<rawdata::WaveForm> > &,
			   art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      std::array<double, n_ch_det> GetTot(const std::vector<rawdata::TRB3RawDigit>&);
      std::array<double, n_seg>    GetTdc(const std::vector<rawdata::TRB3RawDigit>&);
      
      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;
      
      TTree *tree;
      std::array<float, n_ch_det> ADCq;
      std::array<float, n_ch_det> ADCmax;
      std::array<float, n_ch_det> ADCblw;

      std::array<double, n_seg> TDCt;
      std::array<double, n_ch_det> TDCtot;
    };

  //.......................................................................
  T0Ana::T0Ana(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {
    
    this->reconfigure(pset);

    // create TTree for TRB3RawDigits
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("T0AnaTree","");
    tree->Branch("q",&ADCq);
    tree->Branch("qmax",&ADCmax);
    tree->Branch("qblw",&ADCblw);

    tree->Branch("t", &TDCq);
    tree->Branch("ttot", &TDCtot);
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
  int T0Ana::GetSegIdTrb3(int ch_daq)
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
  std::array<int, 2> T0Ana::GetSegChTrb3(int seg_id)
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
  std::array<double, n_ch_det> T0Ana::GetTot(const std::vector<rawdata::TRB3RawDigit>& digvec)
  {
    std::array<double, n_ch> time_ch;
    std::array<bool, n_ch> found_ch;
    std::array<double, n_ch_det> T0_tot;

    for(int i_ch; i_ch < n_ch; i_ch++){
      time_ch[i_ch]  = 0;
      found_ch[i_ch] = false;
    }

    for(int i_ch_det; i_ch_det < n_ch_det; i_ch_det++){
      T0_tot[i_ch_det]  = -999.0;
    }

    //loop over TRB3 signals
    int n_vec = digvec->size();
    for(int i_vec = 0; i_vec < n_vec; i_vec++){
      uint32_t evt_ch = digvec.at(i_vec).GetChannel();

      if(FindSeg(evt_ch) && (!found_ch[evt_ch])){
	time_ch[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                   + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                   + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0 - trb3_linear_low);
	found_ch[eve_ch] = true;
      }

    }//end loop over T0 TRB3 signals

    //Calculation of TOT, loop over T0 leading channels
    for(int i_ch_det = 0; i_ch_det < n_ch_det; i_ch_det++){
      if(found_ch[2*i_ch_det + 1] + found_ch[2*i_ch_det + 2]){
	T0_tot[i_ch_det] = time_ch[2*i_ch_det + 1] - time_ch[2*i_ch_det + 1];
      }
    }//end loop over T0 leading channels

    return T0_tot;
  }

  //......................................................................
  std::array<double, n_seg> T0Ana::GetTdc(const std::vector<rawdata::TRB3RawDigit>& digvec)
  {
    std::array<double, n_ch> time_ch;
    std::array<bool, n_ch> found_ch;
    std::array<double, n_seg> T0_tdc;

    for(int i_ch; i_ch < n_ch; i_ch++){
      time_ch[i_ch]  = 0;
      found_ch[i_ch] = false;
    }

    for(int i_ch_det; i_ch_det < n_ch_det; i_ch_det++){
      T0_tdc[i_ch_det]  = -100000000.0;
    }

    //loop over TRB3 signals
    int n_vec = digvec->size();
    for(int i_vec = 0; i_vec < n_vec; i_vec++){
      uint32_t evt_ch = digvec.at(i_vec).GetChannel();

      if(evt_ch == 0 && (!found_ch[0])){
	time_ch[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                   + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                   + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0 - trb3_linear_low);
	found_ch[eve_ch] = true;
      }else if(FindSeg(evt_ch) && (!found_ch[evt_ch])){
	time_ch[evt_ch] = epoch_const*digvec.at(i_vec).GetEpochCounter()
	                   + coarse_const*digvec.at(i_vec).GetCoarseTime()
	                   + coarse_const*(digvec.at(i_vec).GetFineTime() - trb3_linear_low)/(trb3_linear_high_T0 - trb3_linear_low);
	found_ch[eve_ch] = true;
      }
    }//end loop over T0 TRB3 signals

    //Calculation of tdc, loop over T0 segments
    if(found_ch[0]){
      for(int i_seg = 0; i_seg < n_seg; i_seg++){
	std::array<int, 2> ch_seg = GetSegChTrb3(i_seg);
	if(found_ch[ch_seg[0]] && found_ch[ch_seg[1]]){
	  T0_tdc[i_seg] = (time_ch[ch_seg[0]] + time_ch[ch_seg[1]])/2.0 - time_ch[0];
	}
      }//end loop over T0 leading channels
    }

    return T0_tdc;
  }

  //......................................................................
  
  void T0Ana::FillT0AnaTree(art::Handle< std::vector<emph::rawdata::WaveForm> > & T0wvfm, art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > & T0trb3)
    {
      for (int i=0; i<20; ++i) {
	ADCq[i] = -999999.;
	ADCmax[i] = -9999.;
	ADCblw[i] = -1.;	

	TDCt[i] = -100000000.0;
	TDCtot[i] = -999.;
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
	  echan.SetBoard(board);
	  echan.SetChannel(chan);
	  emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	  int detchan = dchan.Channel();
	  if (detchan >0 && detchan <= 20){
	    ADCq[detchan - 1] = wvfm.Charge(); 
	    ADCmax[detchan - 1] = wvfm.Baseline()-wvfm.PeakADC(); 
	    ADCblw[detchan - 1] = wvfm.BLWidth();
	  }	  
	} // end loop over T0 ADC channels
      }

      // get TDC info for T0
      if (!T0trb3->empty()) {
	TDCt = GetTdc(T0trb3);
	TDCtot = GetTot(T0trb3);

	emph::cmap::FEBoardType boardType = emph::cmap::TRB3;
	emph::cmap::EChannel T0echan;
	T0echan.SetBoardType(boardType);
	// loop over ADC channels
	for (size_t idx=0; idx < T0trb3->size(); ++idx) { 
	  const rawdata::TRB3RawDigit& trb3 = (*T0trb3)[idx];
	  int chan = trb3.GetChannel();
	  int board = trb3.GetBoardId();
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
      }
      tree->Fill();
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
