////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to create alignment constants for SSD
///          alignment
/// \author  $Author: jmirabit@bu.edu and wanly@bu.edu$
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TGraph.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"

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
#include "ChannelMap/service/ChannelMapService.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RawData/TRB3RawDigit.h"
#include "SpacePointReco/SSDAlign.h"
#include "RecoBase/SSDCluster.h"

//using namespace emph;

namespace emph {

    ///
    class SpacePointReco : public art::EDAnalyzer {
        public:
            explicit SpacePointReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
            ~SpacePointReco();

            // Optional, read/write access to event
            void produce(art::Event& evt);

            // Optional if you want to be able to configure from event display, for example
            //void reconfigure(const fhicl::ParameterSet& pset);
            void SpacePointReco::GetSpacePoint(art::Handle< std::vector<rb::SSDCluster> > &, std::unique_ptr<std::vector<rb::SpacePoint>> & SpacePoints);

            // Optional use if you have histograms, ntuples, etc you want around for every event
            void beginJob();
            void endJob();
            void Initialize();
            void SSDFit();

        private:

            art::ServiceHandle<emph::geo::GeometryService> geom;
            art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
            art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

            int nxsensors=0;
            int nysensors=0;
            int nusensors=0;
            int nwsensors=0;
            std::vector<rb::SSDCluster> clusters;
            std::vector<emph::al::SSDAlign> ssdvec;
            std::vector<emph::al::SSDAlign> evt_holder;
            std::vector<emph::al::SSDAlign> xevt_holder;
            std::vector<emph::al::SSDAlign> yevt_holder;
            std::vector<emph::al::SSDAlign> uevt_holder;
            std::vector<emph::al::SSDAlign> vevt_holder;
            std::vector<TH2F*> fSSD_Profile;
            std::vector<TH1F*> fXSSD_Profile;
            std::vector<TH1F*> fYSSD_Profile;
            std::vector<TH1F*> fUSSD_Profile;
            std::vector<TH1F*> fWSSD_Profile;
            std::vector<TH2F*> fadj_SSD_Profile;
            std::vector<TH1F*> fX_Residual_Init;
            std::vector<TH1F*> fY_Residual_Init;
            std::vector<TH1F*> fX_Residual_Fin;
            std::vector<TH1F*> fY_Residual_Fin;
            std::vector<TH1F*> fU_Residual_Fin;
            std::vector<TH1F*> fW_Residual_Fin;
            TGraph** fxShifts;
            TGraph** fyShifts;
            TGraph** fuShifts;
            TGraph** fwShifts;
            TGraph* evt_line;
            TGraph** evt_disp;
            TGraph** evt_disp_adj;
            TF1* fit;
            art::ServiceHandle<art::TFileService> tfs;
            int nstations;
            int nssds;
            int max_sensors=0;
            double max_z = 0;
            bool first_run=1;
            std::map<std::vector<int>, int> spsindex;
            double magnetpos;
            double x0z=-1; double x1z=-1; double y0z=-1; double y1z=-1;
            size_t x_fixedstationindex;
            size_t y_fixedstationindex;
            std::vector<double> zpos_x;
            std::vector<double> zpos_y;
            std::vector<double> zpos_u;
            std::vector<double> zpos_v;
            int loops=50;
    };

    //.......................................................................

    SpacePointReco::SpacePointReco(fhicl::ParameterSet const& pset)
        : EDAnalyzer(pset)
    {
        //this->reconfigure(pset);
        fxShifts = new TGraph*[10];
        fyShifts = new TGraph*[10];
        fuShifts = new TGraph*[10];
        fwShifts = new TGraph*[10];
        evt_disp = new TGraph*[10];
        evt_disp_adj = new TGraph*[10];
    }

    //......................................................................

    SpacePointReco::~SpacePointReco()
    {
        //======================================================================
        // Clean up any memory allocated by your module
        //======================================================================
    }

    //......................................................................

    // void SpacePointReco::reconfigure(const fhicl::ParameterSet& pset)
    // {    
    // }

    //......................................................................

    void SpacePointReco::beginJob()
    {
    }

    //......................................................................

    void SpacePointReco::Initialize()
    {
        // initialize channel map
        auto fChannelMap = cmap->CMap();
        auto emgeo = geom->Geo();

        nstations = emgeo->NSSDStations();
        nssds = emgeo->NSSDs();
        magnetpos = emgeo->MagnetDSZPos();

        //resizing to fill with z positions
        std::cout<<"SSD Stations and Axis Indices Below:"<<std::endl;
        for (int fer=0; fer<10; ++fer){
            for (int mod=0; mod<6; ++mod){
                emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
                if (!fChannelMap->IsValidEChan(echan)) continue;
                emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);

                const emph::geo::SSDStation *st = emgeo->GetSSDStation(dchan.Station());
                const emph::geo::Plane      *pln = st->GetPlane(dchan.Plane());
                const emph::geo::Detector   *sd = pln->SSD(dchan.HiLo());

                //rb::SSDCluster null_clust;
                //emph::al::SSDAlign sensor_info(null_clust,*sd,*st,0);
                emph::al::SSDAlign sensor_info(*sd,*st);
                std::vector<int> sps = {dchan.Station(),dchan.Plane(),dchan.HiLo()};

                if(sensor_info.View()==emph::geo::X_VIEW){
                    spsindex.insert(std::pair<std::vector<int>,int>(sps,nxsensors));
                    zpos_x.push_back(sensor_info.Z());
                    if (x0z==-1 && dchan.Station()==0) x0z = sensor_info.Z();
                    if (x1z==-1 && sensor_info.Z()>magnetpos){
                        x1z = sensor_info.Z();
                        x_fixedstationindex = spsindex[sps];
                    }
                    nxsensors+=1;
                }
                if(sensor_info.View()==emph::geo::Y_VIEW){
                    spsindex.insert(std::pair<std::vector<int>,int>(sps,nysensors));
                    zpos_y.push_back(sensor_info.Z());
                    if (y0z==-1 && dchan.Station()==0) y0z = sensor_info.Z();
                    if (y1z==-1 && sensor_info.Z()>magnetpos){
                        y1z = sensor_info.Z();
                        y_fixedstationindex = spsindex[sps];
                    }
                    nysensors+=1;
                }
                if(sensor_info.View()==emph::geo::U_VIEW){
                    spsindex.insert(std::pair<std::vector<int>,int>(sps,nusensors));
                    zpos_u.push_back(sensor_info.Z());
                    nusensors+=1;
                }
                if(sensor_info.View()==emph::geo::W_VIEW){
                    spsindex.insert(std::pair<std::vector<int>,int>(sps,nwsensors));
                    zpos_v.push_back(sensor_info.Z());
                    nwsensors+=1;
                }
                if (sensor_info.Z()>max_z) max_z=sensor_info.Z();
            }
        }
        //Remove duplicate members
        fSSD_Profile.resize(nstations); fadj_SSD_Profile.resize(nstations);
        fXSSD_Profile.resize(nstations); fYSSD_Profile.resize(nstations);
        fUSSD_Profile.resize(nstations);
        fWSSD_Profile.resize(nstations);
        fX_Residual_Init.resize(nxsensors); fY_Residual_Init.resize(nysensors);
        fX_Residual_Fin.resize(nxsensors); fY_Residual_Fin.resize(nysensors);
        fU_Residual_Fin.resize(nxsensors); fW_Residual_Fin.resize(nysensors);
        std::array<const char*,4> pos_strings = {"x","y","u","v"};
        char hname[64];
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"SSDProfile_%d",i);
            fSSD_Profile[i] = tfs->make<TH2F>(hname,Form("Station %i",i),160,-40,40,160,-40,40);
            fSSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
            fSSD_Profile[i]->GetYaxis()->SetTitle("Y Position (mm)");
            //fSSD_Profile[i]->SetBit(TH1::kNoStats);
        }
        
        first_run=0;
    }

    //......................................................................

    void SpacePointReco::endJob()
    {
        //Then output the alignment constants to ConstBase/SpacePointReco.dat
    }

    //......................................................................

    void SpacePointReco::GetSpacePoint(art::Handle< std::vector<rb::SSDCluster> > & clustH, std::unique_ptr<std::vector<rb::SpacePoint>> & SpacePoints)
    {
        //First we need to initialize SSD configuration and plots based on the geometry
        if (first_run) Initialize();
        for (size_t idx=0; idx < clustH->size(); ++idx) {
            const rb::SSDCluster& clust = (*clustH)[idx];

            //Fill x,y,z of ssdalign object using detgeomap
            rb::LineSegment ls;
            if (clust.AvgStrip() > 640){
                std::cout<<"Skipping nonsense SSD Hit"<<std::endl;
                continue;
            }
            dgm->Map()->SSDClusterToLineSegment(clust, ls);

            int event = evt.event();

            emph::al::SSDAlign hit(clust,event);
            ssdvec.push_back(hit);
            ssdvec.back().SetPos(ls);
        }
    }


    //......................................................................
    void SpacePointReco::produce(art::Event& evt)
    { 
        std::string fClusterLabel = "clust";

        art::Handle< std::vector<rb::SSDCluster> > clustH;
        auto geo = geom->Geo();
        std::unique_ptr<std::vector<rb::SpacePoint> > SpacePointv(new std::vector<rb::SpacePoint>);

        try {
            evt.getByLabel(fClusterLabel, clustH);
            if (!clustH->empty()) {	
                GetSpacePoint(ADCHandle,  BACkovHitv);
                
            }
        }
        catch(...) {

        }
    }
} // end namespace emph

DEFINE_ART_MODULE(emph::SpacePointReco)
