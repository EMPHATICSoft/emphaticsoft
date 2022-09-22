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
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TTimeStamp.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/WaveForm.h"
#include "RecoBase/Spill.h"

namespace emph { 
  class GasCkovAna : public art::EDAnalyzer {
  public:
    explicit GasCkovAna(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~GasCkovAna();
    
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginSubRun(const art::SubRun& sr);
    void endJob();
    
  private:
    void   FillGasCkovPlots(art::Handle< 
			    std::vector<rawdata::WaveForm> > &);
    
    void   MakeGasCkovPlots();
        
    //    emph::cmap::ChannelMap* fChannelMap;
    std::string fChanMapFileName;
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    
    // hard codes consts for now,
    // need to figure out better solution with Geo NChannel function

    static const unsigned int nChanGasCkov = 3;


    TTimeStamp spilltime;
    float pmtadc1;
    float pmtadc2;
    float pmtadc3;
    float pressure5a;
    float pressure5b;
    float pressure6a;
    float pressure6b;
    float efficiency;
    float adcThresh[3];

    // define histograms and tree
    TH1F* fGasCkovADCDist[nChanGasCkov];
    TTree* tree;
 
  };  
  //.......................................................................
  GasCkovAna::GasCkovAna(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {

    this->reconfigure(pset);

  }

  //......................................................................
  GasCkovAna::~GasCkovAna()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................
  void GasCkovAna::reconfigure(const fhicl::ParameterSet& pset)
  {
    adcThresh[0] = pset.get<double>("ADCThresh0",0.);
    adcThresh[1] = pset.get<double>("ADCThresh1",0.);
    adcThresh[2] = pset.get<double>("ADCThresh2",0.);

  }

  //......................................................................
  void GasCkovAna::beginJob()
  {
    fNEvents=0;

    // initialize channel map
    /*
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
    */
    MakeGasCkovPlots();

  }

  //......................................................................

  void GasCkovAna::endJob()
  {

  }

  //......................................................................
  void GasCkovAna::beginSubRun(const art::SubRun& sr)
  {
    art::Handle<rb::Spill> spillHandle;

    try {
      sr.getByLabel("spillinfo",spillHandle);
      
      pressure5a = spillHandle->MT5CPR();
      pressure5b = spillHandle->MT5CP2();
      pressure6a = spillHandle->MT6CPR();
      pressure6b = spillHandle->MT6CP2();
      spilltime = TTimeStamp(spillHandle->Timestamp());
      
    }
    catch(...) {
      std::cout << "No spill info object found!  Aborting..." << std::endl;
      std::abort();
    }

    std::cout << "Got info for spill at " << spilltime.AsString() << std::endl;
  }

  //......................................................................

  void  GasCkovAna::MakeGasCkovPlots()
  {
    art::ServiceHandle<art::TFileService> tfs;

    tree = tfs->make<TTree>("GasCkovTree","GasCkov Analysis Tree");

    tree->Branch("spilltime", &spilltime);
    tree->Branch("pmtadc1", &pmtadc1);
    tree->Branch("pmtadc2", &pmtadc2);
    tree->Branch("pmtadc3", &pmtadc3);
    tree->Branch("pressure5a", &pressure5a);
    tree->Branch("pressure5b", &pressure5b);
    tree->Branch("pressure6a", &pressure6a);
    tree->Branch("pressure6b", &pressure6b);
    tree->Branch("efficiency", &efficiency);


    int nchannel = emph::geo::DetInfo::NChannel(emph::geo::GasCkov);
    char hname[256];
    std::cout << "Making Gas Ckov ADC OnMon plots" << std::endl;
    for (int i=0; i<nchannel; ++i) {
      sprintf(hname,"GasCkovADC_%d",i);
      fGasCkovADCDist[i] = tfs -> make<TH1F>(hname,hname,100,0,500);
    }
  }


  //......................................................................

  void GasCkovAna::FillGasCkovPlots(art::Handle< std::vector<rawdata::WaveForm> > & wvfmH)
  {
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    int nchan = emph::geo::DetInfo::NChannel(emph::geo::GasCkov);
    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);

    pmtadc1 = -100.;
    pmtadc2 = -100.;
    pmtadc3 = -100.;

    // if (fMakeWaveFormPlots) {
    if (!wvfmH->empty()) {
      //here is where we set tree variables(calculate values)!!!!
      for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	echan.SetBoard(board);
	echan.SetChannel(chan);
	emph::cmap::DChannel dchan = cmap->DetChan(echan);
	int detchan = dchan.Channel();
	if (detchan >= 0 && detchan < nchan) {
	  float adc = wvfm.Baseline()-wvfm.PeakADC();
	  float blw = wvfm.BLWidth();
	  if ((adc > 5*blw) && (adc > adcThresh[detchan])){
	    if (detchan == 0) pmtadc1 = adc;
	    if (detchan == 1) pmtadc2 = adc;
	    if (detchan == 2) pmtadc3 = adc;
	    
	    fGasCkovADCDist[detchan]->Fill(adc);
	  }
	}
      }
      tree->Fill();      
    }
  }

  //......................................................................


  //......................................................................
  void GasCkovAna::analyze(const art::Event& evt)
  {
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();
    std::string labelStr;

    int i = emph::geo::GasCkov;
    
    labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
    art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
    // get raw waveforms for the GasCkov PMTs
    try {
      evt.getByLabel(labelStr, wfHandle);
    }
    catch(...) {
      std::cout << "Failed to get GasCkov waveforms" << std::endl;
      return;
    }

    if (!wfHandle->empty()) {	
      FillGasCkovPlots(wfHandle);      
    }
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::GasCkovAna)
