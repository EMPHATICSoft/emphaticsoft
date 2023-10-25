////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create ADC objects from raw data and 
///          store them in the art output file
/// \author  $Author: jmirabit $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
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
  
  ///
  class ADCReco : public art::EDProducer {
  public:
    explicit ADCReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~ADCReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    //void endRun(art::Run const&);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    void GetADC(art::Handle< std::vector<emph::rawdata::WaveForm> > &,std::unique_ptr<std::vector<rb::ADC>> & ADCs);

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::st::SignalTime stmap;

    float CalcBaseline(const emph::rawdata::WaveForm&, const int&) const;
    int CalcTimeMax(const emph::rawdata::WaveForm&) const;
    float CalcTime(const emph::rawdata::WaveForm&) const;
    float CalcCharge(const emph::rawdata::WaveForm&, const int&, const float&) const;
    
    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    int event;

  };

  //.......................................................................
  
  ADCReco::ADCReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

    this->produces< std::vector<rb::ADC>>();

  }

  //......................................................................
  
  ADCReco::~ADCReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void ADCReco::beginJob()
  {
    fNEvents=0;
  } 
  //......................................................................
  
  void ADCReco::endJob()
  {
  }
  
  //......................................................................
  float ADCReco::CalcBaseline(const emph::rawdata::WaveForm& wvfm, const int& exp_time) const
  {
      int tmin = exp_time - 15;
      int tmax = exp_time + 30;
      if (tmin<0) tmin=0;
      if (tmax>108) tmax=108;

      float bl;
      if (tmin>20) bl = wvfm.Baseline(0,20); //use first 20 if signal is not here
      else if (tmax<88) bl = wvfm.Baseline(88,20); //use last 20 if signal is not here
      else bl = (wvfm.Baseline(0,10) + wvfm.Baseline (98,10))/2.; //use first 10 and last 10 if necessary

      return bl;
  }

  //......................................................................
  int ADCReco::CalcTimeMax(const emph::rawdata::WaveForm& wvfm) const
  {
      //Calculate time of max ADC
      int maxVal = -99999;
      int s = -1;
      int tm = 0;
      for (size_t i=0; i<wvfm.NADC(); ++i){
          if (wvfm.ADC(i)*s>maxVal) {
              maxVal = wvfm.ADC(i)*s;
              tm = int(i);
          }
      }
      return tm;
  }
  //......................................................................
  float ADCReco::CalcTime(const emph::rawdata::WaveForm& wvfm) const
  {
      //returns time of hit according to steepest slope
      int timemax = this->CalcTimeMax(wvfm);
      int time=0;
      int steepest_slope=0;
      for (size_t i=0; i<wvfm.NADC(); ++i){
          if (i==0) continue;
          else {
              int slope = wvfm.ADC(i)-wvfm.ADC(i-1);
              //Only record time if it is steepest slope and at most 5 ticks before  maximum ADC
              if (slope<steepest_slope && timemax-i<=5 && timemax-i>=0){
                  steepest_slope=slope;
                  time = int(i);
              }
          }
      }
      return 4.*time; //convert to ns
  }
  //......................................................................
  float ADCReco::CalcCharge(const emph::rawdata::WaveForm& wvfm, const int& exp_time, const float& bl) const
    {
        //Fixed Window Integration
        int tmin = exp_time - 15;
        int tmax = exp_time + 30;
        if (tmin<0) tmin=0;
        if (tmax>108) tmax=108;

        float sum=0;
        for ( size_t i=tmin; i<size_t(tmax) && i<wvfm.NADC(); ++i) sum += wvfm.ADC(i)-bl;
        //convert to pC
        float charge = (-1*sum*2*4e-9*1e12)/(50*4096);
        return charge;
    }
  //......................................................................

  void ADCReco::GetADC(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH, std::unique_ptr<std::vector<rb::ADC>> & ADCs)
  {
    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);
    event = fNEvents;
    if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
        int board = wvfm.Board();
        echan.SetBoard(board);
        echan.SetChannel(chan);
        emph::cmap::DChannel dchan = cmap->DetChan(echan);
        int detchan = dchan.Channel();

        int v1720index = board*8 + chan;
        int exptime = stmap.SigTime(v1720index);

        //Set baseline,charge, and time
        float bl = CalcBaseline(wvfm,exptime);
        float t = CalcTime(wvfm);
        float q = CalcCharge(wvfm,exptime,bl);

        //Create object and store BACkov Charge and PID results
        rb::ADC ADC;
        ADC.SetBaseline(bl);
        ADC.SetTime(t);
        ADC.SetCharge(q);

        ADCs->push_back(ADC);

	  }  
    }
  }
 
  //......................................................................
  void ADCReco::produce(art::Event& evt)
  { 
    ++fNEvents;
    fRun = evt.run();
    fSubrun = evt.subRun();
    if(!stmap.IsTimeMapLoaded()) stmap.LoadMap(fRun);

    std::vector<std::string> detNames;
    detNames.push_back(emph::geo::DetInfo::Name(emph::geo::Trigger));
    detNames.push_back(emph::geo::DetInfo::Name(emph::geo::GasCkov));
    detNames.push_back(emph::geo::DetInfo::Name(emph::geo::BACkov));
    detNames.push_back(emph::geo::DetInfo::Name(emph::geo::LGCalo));
    detNames.push_back(emph::geo::DetInfo::Name(emph::geo::T0));

    std::unique_ptr<std::vector<rb::ADC>> ADCv_Trig(new std::vector<rb::ADC>);
    std::unique_ptr<std::vector<rb::ADC>> ADCv_GC(new std::vector<rb::ADC>);
    std::unique_ptr<std::vector<rb::ADC>> ADCv_BAC(new std::vector<rb::ADC>);
    std::unique_ptr<std::vector<rb::ADC>> ADCv_LGC(new std::vector<rb::ADC>);
    std::unique_ptr<std::vector<rb::ADC>> ADCv_T0(new std::vector<rb::ADC>);

    for (int i=0; i<emph::geo::NDetectors; ++i) {
        std::string labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
        art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
        try {
            evt.getByLabel(labelStr, wfHandle);

            if (!wfHandle->empty()) {
                if (i == emph::geo::Trigger) GetADC(wfHandle, ADCv_Trig);
                if (i == emph::geo::GasCkov) GetADC(wfHandle, ADCv_GC);
                if (i == emph::geo::BACkov)  GetADC(wfHandle, ADCv_BAC);
                if (i == emph::geo::LGCalo)  GetADC(wfHandle, ADCv_LGC);
                if (i == emph::geo::T0)      GetADC(wfHandle, ADCv_T0);
            }
        }
        catch(...) {
        }
    }

    //evt.put(std::move(ADCv_Trig));
    evt.put(std::move(ADCv_Trig), detNames[0]);
    //evt.put(std::move(ADCv_GC),   detNames[1]);
    //evt.put(std::move(ADCv_BAC),  detNames[2]);
    //evt.put(std::move(ADCv_LGC),  detNames[3]);
    //evt.put(std::move(ADCv_T0),   detNames[4]);

  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::ADCReco)
