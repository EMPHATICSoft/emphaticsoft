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
#include "TH1I.h"
#include "TH2I.h"

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
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "Geometry/DetectorDefs.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace onmon {
    class OnMonModule : public art::EDAnalyzer {
    public:
      explicit OnMonModule(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~OnMonModule();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();

    private:
      TH2I*  fNRawObjectsHisto;
      TH1I*  fNTriggerVsDet;
      void   FillGasCkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > &,
			 art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillSSDPlots(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > &);
      void   FillARICHPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > &);
      void   FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > &);
      void   FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > &);

      void   MakeGasCkovPlots();
      void   MakeBACkovPlots();
      void   MakeT0Plots();
      void   MakeSSDPlots();
      void   MakeARICHPlots();
      void   MakeLGCaloPlots();
      void   MakeRPCPlots();
      void   MakeTrigPlots();

    };

    //.......................................................................
    OnMonModule::OnMonModule(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset)
    {

      this->reconfigure(pset);

    }

    //......................................................................
    OnMonModule::~OnMonModule()
    {
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void OnMonModule::reconfigure(const fhicl::ParameterSet& )// pset)
    {

    }

    //......................................................................
    void OnMonModule::beginJob()
    {
      //
      // Book histograms, ntuples, initialize counts etc., etc., ...
      //
      art::ServiceHandle<art::TFileService> tfs;

      fNRawObjectsHisto = tfs->make<TH2I>("NRawObjectsHisto",
					  "Number of Raw Objects Per Detector",
					  emph::geo::NDetectors+1,0,emph::geo::NDetectors+1,
					  100,0,100);

      fNTriggerVsDet = tfs->make<TH1I>("NTriggerVsDet","Number of Triggers Seen by Each Detector",emph::geo::NDetectors+1,0,emph::geo::NDetectors+1);

      // label x-axis
      std::string labelStr;
      int i=0;
      for (; i<emph::geo::NDetectors; ++i) {
	labelStr = emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	if (i == emph::geo::T0) labelStr += "ADC";
	fNTriggerVsDet->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
	fNRawObjectsHisto->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
      }
      // now add T0TDC
      labelStr = emph::geo::DetInfo::Name(emph::geo::T0) + "TDC";
      fNTriggerVsDet->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());
      fNRawObjectsHisto->GetXaxis()->SetBinLabel(i+1,labelStr.c_str());

      MakeGasCkovPlots();
      MakeBACkovPlots();
      MakeT0Plots();
      MakeSSDPlots();
      MakeARICHPlots();
      MakeLGCaloPlots();
      MakeRPCPlots();
      MakeTrigPlots();

    }

    //......................................................................

    void  OnMonModule::MakeGasCkovPlots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeBACkovPlots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeT0Plots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeSSDPlots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeARICHPlots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeLGCaloPlots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeRPCPlots()
    {
    }

    //......................................................................

    void  OnMonModule::MakeTrigPlots()
    {
    }

    //......................................................................

    void OnMonModule::FillGasCkovPlots(art::Handle< std::vector<rawdata::WaveForm> > & )
    {
    }

    //......................................................................

    void OnMonModule::FillBACkovPlots(art::Handle< std::vector<rawdata::WaveForm> > & )
    {
    }

    //......................................................................

    void OnMonModule::FillT0Plots(art::Handle< std::vector<rawdata::WaveForm> > & , art::Handle< std::vector<rawdata::TRB3RawDigit> > & )
    {
    }

    //......................................................................

    void OnMonModule::FillSSDPlots(art::Handle< std::vector<emph::rawdata::SSDRawDigit> > & )
    {
    }

    //......................................................................

    void OnMonModule::FillARICHPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & )
    {
    }

    //......................................................................

    void    OnMonModule::FillLGCaloPlots(art::Handle< std::vector<rawdata::WaveForm> > & )
    {

    }

    //......................................................................

    void    OnMonModule::FillRPCPlots(art::Handle< std::vector<rawdata::TRB3RawDigit> > & )
    {
    }
    //......................................................................

    void   OnMonModule::FillTrigPlots(art::Handle< std::vector<rawdata::WaveForm> > & )
    {
    }

    //......................................................................
    void OnMonModule::analyze(const art::Event& evt)
    {
      std::string labelStr;

      for (int i=0; i<emph::geo::NDetectors; ++i) {

	labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	art::Handle< std::vector<emph::rawdata::WaveForm> > wfHandle;
	try {
	  evt.getByLabel(labelStr, wfHandle);

	  if (!wfHandle->empty()) {
	    fNRawObjectsHisto->Fill(i,wfHandle->size());
	    fNTriggerVsDet->Fill(i);
	    if (i == emph::geo::Trigger) FillTrigPlots(wfHandle);
	    if (i == emph::geo::GasCkov) FillGasCkovPlots(wfHandle);
	    if (i == emph::geo::BACkov)  FillBACkovPlots(wfHandle);
	    if (i == emph::geo::LGCalo)  FillLGCaloPlots(wfHandle);
	    if (i == emph::geo::T0) {
	      int j = emph::geo::NDetectors;
	      labelStr = "raw:T0";
	      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle;
	      try {
		evt.getByLabel(labelStr, trbHandle);
		if (!trbHandle->empty()) {
		  fNRawObjectsHisto->Fill(j,trbHandle->size());
		  fNTriggerVsDet->Fill(j);
		  FillT0Plots(wfHandle, trbHandle);
		}
	      }
	      catch(...) {
		std::cout << "No TRB3 digits found for the T0!" << std::endl;
	      }
	    }

	  }
	}
	catch(...) {
	  //	  std::cout << "Nothing found in " << labelStr << std::endl;
	}
      }
      // get TRB3digits
      int i = emph::geo::RPC;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbHandle;
      try {
	evt.getByLabel(labelStr, trbHandle);

	if (!trbHandle->empty()) {
	  fNRawObjectsHisto->Fill(i,trbHandle->size());
	  fNTriggerVsDet->Fill(i);
	  FillRPCPlots(trbHandle);
	}
      }
      catch(...) {

      }
      // get SSDdigits
      i = emph::geo::SSD;
      labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
      art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
      try {
	evt.getByLabel(labelStr, ssdHandle);

	if (!ssdHandle->empty()) {
	  fNRawObjectsHisto->Fill(i,ssdHandle->size());
	  fNTriggerVsDet->Fill(i);
	  FillSSDPlots(ssdHandle);
	}
      }
      catch(...) {

      }

      return;
    }
  }
} // end namespace demo

DEFINE_ART_MODULE(emph::onmon::OnMonModule)
