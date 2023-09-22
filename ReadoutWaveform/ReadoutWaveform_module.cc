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
      
        std::array<std::array<TH2F*,8>,6> h_wvfms;
        std::array<std::array<int,8>,6> nentries_wvfms= {0};
        int plotsize; //0 is small, 1 is medium, 2 is large, 3 is very large
        int nwvfms;
        int plotmax;
        int plotmin;
    };

    //.......................................................................
    ReadoutWaveform::ReadoutWaveform(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
       plotsize(pset.get<int>("PlotSize",0)),
       nwvfms(pset.get<int>("NumberOfWvfms",0))
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
         art::ServiceHandle<art::TFileService> tfs;

         if (plotsize==0) {plotmax = 1900; plotmin = 1800;}
         if (plotsize==1) {plotmax = 1900; plotmin = 1700;}
         if (plotsize==2) {plotmax = 2000; plotmin = 1600;}
         if (plotsize==3) {plotmax = 2000; plotmin = 1400;}
         int nbins = (plotmax-plotmin)*2;

         for (int i=0; i<6; ++i){
            for (int j=0; j<8; ++j){
                h_wvfms[i][j] = tfs->make<TH2F>(Form("wvfm_%i_%i", i, j), Form("V1720 Board %i Chan %i", i, j), 108, 0, 108, nbins, plotmin , plotmax);
                h_wvfms[i][j]->GetYaxis()->SetTitle("mV");
            }
         }

	 }
    
    //......................................................................

    void ReadoutWaveform::endJob()
    {
      for (int i=0; i<6; ++i){
          for (int j=0; j<8; ++j){
                //h_wvfms[i][j]->GetYaxis()->SetRangeUser(h_wvfms
          }
      }
      //f->Close();
      //delete f; f=0;
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
                 echan.SetBoard(board);
                 echan.SetChannel(chan);
                 auto adcvals = wvfm.AllADC();
                 //Stop filling after specified number of wvfms in fcl file waveforms have been added
                 if (nentries_wvfms[board][chan] > nwvfms) continue;
                 for (size_t i=0; i<adcvals.size(); ++i) {
                     float mV = adcvals[i]*adc_to_mV;
                     h_wvfms[board][chan]->Fill(i,mV);
                 }
             nentries_wvfms[board][chan]+=1;
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
