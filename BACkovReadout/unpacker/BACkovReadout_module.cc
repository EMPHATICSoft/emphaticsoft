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
      void   FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      
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
		int event, chan;
		float max, blw, bl;
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
		tree->Branch("chan",&chan);
		tree->Branch("event",&event);
		tree->Branch("max",&max);
		tree->Branch("blw",&blw);
		tree->Branch("bl",&bl);

    }
    
    //......................................................................

    void BACkovReadout::endJob()
    {

		tree->Write();
      f->Close();
      delete f; f=0;
    }
    
    //......................................................................

    void BACkovReadout::FillBACkovPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH)
    {
      int nchan = emph::geo::DetInfo::NChannel(emph::geo::BACkov);
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      echan.SetBoardType(boardType);
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
              max = wvfm.Baseline()-wvfm.PeakADC();
	      bl = wvfm.Baseline();
              blw = wvfm.BLWidth();
              if (max > 5*blw) {
                auto adcvals = wvfm.AllADC();
                fNEventsBACkov[detchan]++;
              }
            }
          tree->Fill();

	  }
      }
    }
	 }
    
    //......................................................................

    void BACkovReadout::analyze(const art::Event& evt)
    { 
      ++fNEvents;
      fRun = evt.run();
      fSubrun = evt.subRun();     
      std::string labelStr;

      // get WaveForm
      int i = emph::geo::BACkov;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
      try {
	evt.getByLabel(labelStr, wfHandle);

	if (!wfHandle->empty()) {
	  FillBACkovPlots(wfHandle);
	}
      }
      catch(...) {

      }

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::BACkovReadout)
