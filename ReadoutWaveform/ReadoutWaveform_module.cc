////////////////////////////////////////////////////////////////////////
/// \brief   Program to plot V1720 waveforms from a given board
/// \author  $Author: jmirabit $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

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


using namespace emph;

///package to illustrate how to write modules
namespace emph {
    class ReadoutWaveform : public art::EDAnalyzer {
    public:
      explicit ReadoutWaveform(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~ReadoutWaveform();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
      void   FillWaveformPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > &);

      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;

      // define histograms
      
		TFile* f; 
        std::array<TH2F*,8> h_wvfms;
        std::array<int,8> nentries_wvfms= {0};
		int iboard;
    };

    //.......................................................................
    ReadoutWaveform::ReadoutWaveform(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
      iboard(pset.get<int>("boardNum",0))
    {
    }

    //......................................................................
    ReadoutWaveform::~ReadoutWaveform()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void ReadoutWaveform::beginJob()
	 {
		 fNEvents=0;

		 char filename[32];
		 sprintf(filename,Form("wvfms_board%i.root",iboard));
		 f = new TFile(filename,"RECREATE");

         for (int i=0; i<8; ++i){
            h_wvfms[i] = new TH2F(Form("wvfm_%i", i), Form("V1720 Board %i Chan %i", iboard, i), 100, 0, 100, 200, 1800 , 1900);
            h_wvfms[i]->GetYaxis()->SetTitle("mV");
         }

	 }
    
    //......................................................................

    void ReadoutWaveform::endJob()
    {
      for (int i=0; i<8; ++i){
        h_wvfms[i]->Write();
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

         if (!wvfmH->empty()) {

             for (size_t idx=0; idx < wvfmH->size(); ++idx) {
                 const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
                 int chan = wvfm.Channel();
                 int board = wvfm.Board();
                 float bl = wvfm.Baseline();
                 float max = wvfm.Baseline()-wvfm.PeakADC();
                 float adc_to_mV = 2000./4096;
                 if(board!=iboard)continue;
                 //if (fNEvents!=10) continue;
                 echan.SetBoard(board);
                 echan.SetChannel(chan);
                 auto adcvals = wvfm.AllADC();
                 //Stop filling after 1000 waveforms have been added
                 if (nentries_wvfms[chan] > 1000) continue;
                 for (size_t i=0; i<adcvals.size(); ++i) {
                     h_wvfms[chan]->Fill(i+1,adcvals[i]*adc_to_mV);
                 }
                 nentries_wvfms[chan]+=1;
             }
         }
     }

    //......................................................................

    void ReadoutWaveform::analyze(const art::Event& evt)
	 { 
		 ++fNEvents;

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
				 else {
				 }
			 }
			 catch(...) {

			 }

		 }

		 return;
	 }
} // end namespace demo

DEFINE_ART_MODULE(emph::ReadoutWaveform)
