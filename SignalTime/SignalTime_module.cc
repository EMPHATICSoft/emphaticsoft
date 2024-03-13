////////////////////////////////////////////////////////////////////////
/// \brief   Program to extract expected signal times for each V1720 channel
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
#include "RecoBase/ADC.h"
#include "SignalTime/SignalTime.h"


using namespace emph;

///package to illustrate how to write modules
namespace emph {
    class SignalTime : public art::EDAnalyzer {
    public:
      explicit SignalTime(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~SignalTime();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
      void   FillWaveformPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > &);

      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
      emph::st::SignalTime stmap;

      unsigned int fRun;
      unsigned int fSubrun;
      unsigned int fNEvents;
      

      // each channel has a vector of max values and corresponding times, avgs are taken at end to determine general signal position
     std::array<std::vector<int>,48> times; //40 total V1720 channels 
        std::array<std::array<TH2F*,8>,6> h_wvfms;
        std::array<std::array<int,8>,6> nentries_wvfms= {0};
    };

    //.......................................................................
    SignalTime::SignalTime(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset)
    {
    }

    //......................................................................
    SignalTime::~SignalTime()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void SignalTime::beginJob()
	 {
		 fNEvents=0;
	 }
    
    //......................................................................

    void SignalTime::endJob()
    {
         std::ofstream outfile;
         outfile.open(Form("run%itimes.dat",fRun));
         outfile<<"##V1720 index = (board*8)+chan"<<std::endl;
         outfile<<"Run  "<<fRun<<std::endl;
         outfile<<"V1720Index   ";
         for (size_t i=0; i<times.size(); ++i){
             outfile<<i<<"   ";
             if (i<10) outfile<<" ";
         }
         outfile<<std::endl<<"SignalTime   ";
         for (size_t i=0; i<times.size(); ++i){
            int tot_time =0;
            int avg_time =0;
            for(size_t j=0; j<times[i].size(); ++j){
                tot_time+=times[i][j];
            }
            if(times[i].size()!=0) avg_time = tot_time/times[i].size();
            outfile<<avg_time<<"   ";
             if (avg_time<10) outfile<<" ";
         }
         outfile<<std::endl;
    }
    
    //......................................................................

    void SignalTime::FillWaveformPlots(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH)
    {

         emph::cmap::FEBoardType boardType = emph::cmap::V1720;
         emph::cmap::EChannel echan;
         echan.SetBoardType(boardType);

         if (!wvfmH->empty()) {

             for (size_t idx=0; idx < wvfmH->size(); ++idx) {
                 const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
                 rb::ADC wvr;
                 int chan = wvfm.Channel();
                 int board = wvfm.Board();
                 echan.SetBoard(board);
                 echan.SetChannel(chan);
                 auto adcvals = wvfm.AllADC();
                 int max =-1*(wvfm.PeakADC() - wvfm.Baseline());

                 //Calculate index for board/channel of V1720
                 int index = (8*board)+chan;
                 int time = wvr.Time()/4;
                 if(max>30) times[index].push_back(time); //only include time if signal is large enough
                 //if(max>30 && index = 32) std::cout<<"time = "<<time<<std::endl;

             }
         }
    }

    //......................................................................

    void SignalTime::analyze(const art::Event& evt)
	 { 
		 ++fNEvents;

		 fRun = evt.run();
		 fSubrun = evt.subRun();     
         if(!stmap.IsTimeMapLoaded()) stmap.LoadMap(fRun);
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

DEFINE_ART_MODULE(emph::SignalTime)
