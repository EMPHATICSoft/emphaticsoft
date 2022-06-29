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
#include "TH1I.h"

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
    class ReadoutWaveform : public art::EDAnalyzer {
    public:
      explicit ReadoutWaveform(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~ReadoutWaveform();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
      void   FillWaveformPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > &);

      emph::cmap::ChannelMap* fChannelMap;
      std::string fChanMapFileName;
      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;

      // define histograms
      
      bool fMakeWaveformPlots;
		TFile* f; 
		TH1I *hist;
		int iboard, ichan;
		unsigned int ievt;
    };

    //.......................................................................
    ReadoutWaveform::ReadoutWaveform(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset)
    {

      this->reconfigure(pset);

    }

    //......................................................................
    ReadoutWaveform::~ReadoutWaveform()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void ReadoutWaveform::reconfigure(const fhicl::ParameterSet& pset)
    {
      fChanMapFileName = pset.get<std::string>("channelMapFileName","");
      fMakeWaveformPlots = pset.get<bool>("makeWaveformPlots",false);
      iboard = pset.get<int>("boardNum",0);
      ichan = pset.get<int>("chanNum",0);
      ievt = pset.get<int>("evtNum",0);
      
    }

    //......................................................................
    void ReadoutWaveform::beginJob()
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
		 sprintf(filename,"Waveform.root");
		 f = new TFile(filename,"RECREATE");

		 hist=new TH1I("Waveform",Form("Board%i_Chan%i_Evt%i", iboard, ichan, ievt), 200, 0, 200);

	 }
    
    //......................................................................

    void ReadoutWaveform::endJob()
    {

		if(hist->GetEntries()>0){
			hist->Write();
		}
		else{
			std::cout<<"No such waveform found."<<std::endl;
		}
      f->Close();
      delete f; f=0;
    }
    
    //......................................................................

    void ReadoutWaveform::FillWaveformPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH)
	 {

		 emph::cmap::FEBoardType boardType = emph::cmap::V1720;
		 emph::cmap::EChannel echan;
		 echan.SetBoardType(boardType);

		 if (fMakeWaveformPlots) {
			 if (!wvfmH->empty()) {
				 for (size_t idx=0; idx < wvfmH->size(); ++idx) {
					 const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
					 int chan = wvfm.Channel();
					 int board = wvfm.Board();
					 if(chan!=ichan||board!=iboard)continue;
					 echan.SetBoard(board);
					 echan.SetChannel(chan);
					 auto adcvals = wvfm.AllADC();
					 for (size_t i=0; i<adcvals.size(); ++i) {
						 hist->SetBinContent(i+1,adcvals[i]);
					 }
					 break;
				 }
			 }
		 }
	 }
    
    //......................................................................

    void ReadoutWaveform::analyze(const art::Event& evt)
	 { 
		 ++fNEvents;
		 if(fNEvents!=ievt)return;

		 fRun = evt.run();
		 fSubrun = evt.subRun();     
		 std::string labelStr;

		 // get Waveformdigits
		 for (int i=0; i<emph::geo::NDetectors; ++i) {

			 labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
			 art::Handle< std::vector<emph::rawdata::WaveForm> > wvfmHandle;
			 try {
				 evt.getByLabel(labelStr, wvfmHandle);

				 if (!wvfmHandle->empty()) {
					 FillWaveformPlots(wvfmHandle);
				 }
			 }
			 catch(...) {

			 }
		 }

		 return;
	 }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::ReadoutWaveform)
