////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create ADC objects from raw data and 
///          store them in the art output file
/// \author  $Author: jmirabit $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <cstddef>
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
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/WaveForm.h"
#include "RecoBase/ADC.h"
#include "SignalTime/SignalTime.h"
#include "ADCUtils/ADCUtils.h"


using namespace emph;

namespace emph {
  
  ///
  class ADCReco : public art::EDProducer {
  public:
    struct Config {
      fhicl::Atom<int> Trigger_low{fhicl::Name("Trigger_low"), 0};
      fhicl::Atom<int> Trigger_high{fhicl::Name("Trigger_high"), 0};
      fhicl::Atom<int> GasCkov_low{fhicl::Name("GasCkov_low"), 0};
      fhicl::Atom<int> GasCkov_high{fhicl::Name("GasCkov_high"), 0};
      fhicl::Atom<int> BACkov_low{fhicl::Name("BACkov_low"), 0};
      fhicl::Atom<int> BACkov_high{fhicl::Name("BACkov_high"), 0};
      fhicl::Atom<int> LGCalo_low{fhicl::Name("LGCalo_low"), 0};
      fhicl::Atom<int> LGCalo_high{fhicl::Name("LGCalo_high"), 0};
      fhicl::Atom<int> T0_low{fhicl::Name("T0_low"), 0};
      fhicl::Atom<int> T0_high{fhicl::Name("T0_high"), 0};
    };
    using Parameters = art::EDProducer::Table<Config>;

    explicit ADCReco(Parameters const& pset);
    ~ADCReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    //void endRun(art::Run const&);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    void GetADC(art::Handle< std::vector<emph::rawdata::WaveForm> > &,std::unique_ptr<std::vector<rb::ADC>> & ADCs, const int&, const int&);

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::st::SignalTime stmap;

    unsigned int fRun;
    unsigned int fSubrun;
    unsigned int fNEvents;
    int event;
    int Trigger_low, Trigger_high;
    int GasCkov_low, GasCkov_high;
    int BACkov_low, BACkov_high;
    int LGCalo_low, LGCalo_high;
    int T0_low, T0_high;

  };

  //.......................................................................
  
  ADCReco::ADCReco(Parameters const& pset)
    : EDProducer(pset),
    Trigger_low (pset().Trigger_low()),
    Trigger_high (pset().Trigger_high()),
    GasCkov_low (pset().GasCkov_low()),
    GasCkov_high (pset().GasCkov_high()),
    BACkov_low (pset().BACkov_low()),
    BACkov_high (pset().BACkov_high()),
    LGCalo_low (pset().LGCalo_low()),
    LGCalo_high (pset().LGCalo_high()),
    T0_low (pset().T0_low()),
    T0_high (pset().T0_high())
  {

    this->produces< std::vector<rb::ADC>>(emph::geo::DetInfo::Name(emph::geo::Trigger));
    this->produces< std::vector<rb::ADC>>(emph::geo::DetInfo::Name(emph::geo::GasCkov));
    this->produces< std::vector<rb::ADC>>(emph::geo::DetInfo::Name(emph::geo::BACkov));
    this->produces< std::vector<rb::ADC>>(emph::geo::DetInfo::Name(emph::geo::LGCalo));
    this->produces< std::vector<rb::ADC>>(emph::geo::DetInfo::Name(emph::geo::T0));

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
  void ADCReco::GetADC(art::Handle< std::vector<emph::rawdata::WaveForm> > & wvfmH, std::unique_ptr<std::vector<rb::ADC>> & ADCs, const int& tlow, const int& thigh)
  {
    emph::cmap::FEBoardType boardType = emph::cmap::V1720;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);
    event = fNEvents;
    if (!wvfmH->empty()) {
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm wvfm = (*wvfmH)[idx];
        const emph::adcu::ADCUtils ADCUtil(wvfm,stmap,tlow,thigh);

        //Set baseline,charge, and time
        int board = wvfm.Board();
        int chan = wvfm.Channel();
        float bl = ADCUtil.Baseline();
        float t = ADCUtil.Time();
        float q = ADCUtil.Charge();

        //Create object and store BACkov Charge and PID results
        rb::ADC ADC;
        ADC.SetBoard(board);
        ADC.SetChan(chan);
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
                if (i == emph::geo::Trigger) GetADC(wfHandle, ADCv_Trig, Trigger_low, Trigger_high);
                if (i == emph::geo::GasCkov) GetADC(wfHandle, ADCv_GC, GasCkov_low, GasCkov_high);
                if (i == emph::geo::BACkov)  GetADC(wfHandle, ADCv_BAC, BACkov_low, BACkov_high);
                if (i == emph::geo::LGCalo)  GetADC(wfHandle, ADCv_LGC, LGCalo_low, LGCalo_high);
                if (i == emph::geo::T0)      GetADC(wfHandle, ADCv_T0, T0_low, T0_high);
            }
        }
        catch(...) {
        }
    }

    //evt.put(std::move(ADCv_Trig));
    evt.put(std::move(ADCv_Trig), detNames[0]);
    evt.put(std::move(ADCv_GC),   detNames[1]);
    evt.put(std::move(ADCv_BAC),  detNames[2]);
    evt.put(std::move(ADCv_LGC),  detNames[3]);
    evt.put(std::move(ADCv_T0),   detNames[4]);

  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::ADCReco)
