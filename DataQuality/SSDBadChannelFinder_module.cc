////////////////////////////////////////////////////////////////////////
// Class:       SSDBadChannelFinder
// Plugin Type: analyzer
// File:        SSDBadChannelFinder_module.cc
// Author:      abhattar@nd.edu (Aayush Bhattarai)
//
// June 9, 2026
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"

#include <iostream>

#include "art/Framework/Principal/SubRun.h"
#include "RawData/SSDRawDigit.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "TTree.h"

#include "ChannelMap/service/ChannelMapService.h"

namespace emph {
  namespace dq {

    class SSDBadChannelFinder : public art::EDAnalyzer {
    public:
      explicit SSDBadChannelFinder(fhicl::ParameterSet const& pset);

      void analyze(art::Event const& evt) override;
      void beginJob();
      void endSubRun(art::SubRun const& sr);

    private:
      unsigned int fNHit[5][6][640] = {{{0}}};
      unsigned int fNTriggers = 0;
      
      TTree* fTree = nullptr;
      unsigned int fRun, fSubrun, fFer, fMod, fRow, fNhits, fTrigCount;

      int fStation, fPlane, fSensor;
      art::ServiceHandle<emph::cmap::ChannelMapService> fCmap;

    }; // end of class SSDBadChannelFinder

    //----------------------------------------------------------------
    SSDBadChannelFinder::SSDBadChannelFinder(fhicl::ParameterSet const& pset) 
      : EDAnalyzer{pset}
    {
    
    }

    //----------------------------------------------------------------
    void SSDBadChannelFinder::analyze(art::Event const& evt)
    {
/*      std::cout << "[SSDBadChannelFinder] analyze: run " << evt.run()
                << " subrun " << evt.subRun()
                << " event " << evt.event() << std::endl;
 */  
      auto digits = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit>>("raw:SSD");
      if (!digits) return;

      fNTriggers++;
      for (auto const& dig : *digits) {
        fNHit[dig.FER()][dig.Module()][dig.Row()]++;

      }
    }

    void SSDBadChannelFinder::beginJob()
    {
      std::cout << "[SSDBadChannelFinder] beginJob" << std::endl;
  
      art::ServiceHandle<art::TFileService> tfs;
      fTree = tfs->make<TTree>("chanstats", "per-strip per-subrun hit counts");
      fTree->Branch("run",    &fRun,       "run/i");
      fTree->Branch("subrun", &fSubrun,    "subrun/i");
      fTree->Branch("fer",    &fFer,       "fer/i");
      fTree->Branch("mod",    &fMod,       "mod/i");
      fTree->Branch("row",    &fRow,       "row/i");
      fTree->Branch("nhits",  &fNhits,     "nhits/i");
      fTree->Branch("ntrig",  &fTrigCount, "ntrig/i");

      fTree->Branch("station", &fStation, "station/I");
      fTree->Branch("plane",   &fPlane,   "plane/I");
      fTree->Branch("sensor",  &fSensor,  "sensor/I");

    }

    void SSDBadChannelFinder::endSubRun(art::SubRun const& sr)
    {
      std::cout << "[SSDBadChannelFinder] endSubRun: run " << sr.run()
                << " subrun " << sr.subRun() << std::endl;
      std::cout << "[SSDBadChannelFinder]   ntriggers this subrun: " << fNTriggers << std::endl;

      fRun = sr.run();
      fSubrun = sr.subRun();
      fTrigCount = fNTriggers;
      for (fFer = 0; fFer < 5; ++fFer) {
        for (fMod = 0; fMod < 6; ++fMod) {
          for (fRow = 0; fRow < 640; ++fRow) {
            fNhits = fNHit[fFer][fMod][fRow];
            
            emph::cmap::EChannel echan(emph::cmap::SSD, fFer, fMod);
            if (fCmap->CMap()->IsValidEChan(echan)) {
              emph::cmap::DChannel dchan = fCmap->DetChan(echan);
              fStation = dchan.Station();
              fPlane   = dchan.Plane();
              fSensor  = dchan.HiLo();
            } else {
              fStation = fPlane = fSensor = -1;   // invalid (fer,mod)
            }

            fTree->Fill();
            fNHit[fFer][fMod][fRow] = 0;
          }
        }
      }
        fNTriggers = 0;
    }


  } // end of namespace dq

} // end of namespace emph

DEFINE_ART_MODULE(emph::dq::SSDBadChannelFinder)
