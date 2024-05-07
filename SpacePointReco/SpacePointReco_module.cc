////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create alignment constants for SSD
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
#include "ChannelMap/ChannelMap.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"

//using namespace emph;

namespace emph {

    ///
    class SpacePointReco : public art::EDProducer {
        public:
            explicit SpacePointReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
            ~SpacePointReco();

            // Optional, read/write access to event
            void produce(art::Event& evt);

            // Optional if you want to be able to configure from event display, for example
            //void reconfigure(const fhicl::ParameterSet& pset);
            void GetSpacePoint(art::Handle< std::vector<rb::SSDCluster> > &, std::unique_ptr<std::vector<rb::SpacePoint>> & SpacePoints);

            // Optional use if you have histograms, ntuples, etc you want around for every event
            void beginJob();
            void endJob();
            void Initialize();
            void SSDFit();

        private:

            art::ServiceHandle<emph::geo::GeometryService> geom;
            art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
            art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

            int event;
            std::vector<int> u_stations;
            std::vector<int> w_stations;
            int nxsensors=0;
            int nysensors=0;
            int nusensors=0;
            int nwsensors=0;
            std::vector<TH2F*> fSSD_Profile;
            std::vector<TH2F*> fSSD_Profile_notilt;
            std::vector<TH2F*> fSSD_Profile_onlytilt;
            std::vector<TH1F*> h_diff;
            std::vector<TH1F*> h_sp;
            art::ServiceHandle<art::TFileService> tfs;
            int nstations;
            int nssds;
            bool first_run=1;
    };

    //.......................................................................

    SpacePointReco::SpacePointReco(fhicl::ParameterSet const& pset)
        : EDProducer(pset)
    {
        //this->reconfigure(pset);
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

        //Remove duplicate members
        fSSD_Profile.resize(nstations);
        fSSD_Profile_notilt.resize(nstations);
        fSSD_Profile_onlytilt.resize(nstations);
        h_diff.resize(nstations);
        h_sp.resize(nstations);
        std::array<const char*,4> pos_strings = {"x","y","u","v"};
        char hname[64];
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"SSDProfile_%d",i);
            fSSD_Profile[i] = tfs->make<TH2F>(hname,Form("Station %i",i),160,-40,40,160,-40,40);
            fSSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
            fSSD_Profile[i]->GetYaxis()->SetTitle("Y Position (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"SSDProfile_notilt_%d",i);
            fSSD_Profile_notilt[i] = tfs->make<TH2F>(hname,Form("Station %i (Only X/Y spacepoints)",i),160,-40,40,160,-40,40);
            fSSD_Profile_notilt[i]->GetXaxis()->SetTitle("X Position (mm)");
            fSSD_Profile_notilt[i]->GetYaxis()->SetTitle("Y Position (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"SSDProfile_onlytilt_%d",i);
            fSSD_Profile_onlytilt[i] = tfs->make<TH2F>(hname,Form("Station %i (Only X/Y/W spacepoints)",i),160,-40,40,160,-40,40);
            fSSD_Profile_onlytilt[i]->GetXaxis()->SetTitle("X Position (mm)");
            fSSD_Profile_onlytilt[i]->GetYaxis()->SetTitle("Y Position (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"h_diff_%d",i);
            h_diff[i] = tfs->make<TH1F>(hname,Form("Station %i (W)",i),200,-2,2);
            h_diff[i]->GetXaxis()->SetTitle("Difference in W (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"h_sp_%d",i);
            h_sp[i] = tfs->make<TH1F>(hname,Form("Station %i",i),20,0,20);
            h_sp[i]->GetXaxis()->SetTitle("Number of Spacepoints");
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
        //Initialize Plots etc.
        if (first_run) Initialize();

        //Create vectors for containing hits from SSD sensors
        std::vector<std::vector<std::array<double,2>>> x_hits(nstations);
        std::vector<std::vector<std::array<double,2>>> y_hits(nstations);
        std::vector<std::vector<std::array<double,2>>> u_hits(nstations);
        std::vector<std::vector<std::array<double,2>>> w_hits(nstations);

        for (size_t idx=0; idx < clustH->size(); ++idx) {
            const rb::SSDCluster& clust = (*clustH)[idx];

            //Fill x,y,z of ssdalign object using detgeomap
            rb::LineSegment ls;
            if (clust.AvgStrip() > 640){
                std::cout<<"Skipping nonsense SSD Hit"<<std::endl;
                continue;
            }
            dgm->Map()->SSDClusterToLineSegment(clust, ls);
            //get x,y,z (and convert to u/w if necessary) of line segment
            double x0[3];
            double x1[3];
            double xavg[3];

            for (int i=0; i<3; ++i){
                x0[i] = ls.X0()[i];
                x1[i] = ls.X1()[i];
                xavg[i] = (x0[i]+x1[i])/2.;
            }
            if (clust.View()==emph::geo::X_VIEW){
                x_hits[clust.Station()].push_back({xavg[0],xavg[2]});
            }
            if (clust.View()==emph::geo::Y_VIEW){
                y_hits[clust.Station()].push_back({xavg[1],xavg[2]});
            }
            if (clust.View()==emph::geo::U_VIEW){
            std::cout<<"***** UHIT: SHOULD NOT SEE FOR RUN 1c"<<std::endl;
                double u = (sqrt(2)/2)*(xavg[0]+xavg[1]);
                u_hits[clust.Station()].push_back({u,xavg[2]});
            }
            if (clust.View()==emph::geo::W_VIEW){
                double w = (sqrt(2)/2)*((-1.)*xavg[0]+xavg[1]);
                w_hits[clust.Station()].push_back({w,xavg[2]});
            }
        }
        //for (size_t i=0; i<w_hits.size(); ++i){
        //    std::cout<<"Event: "<<event<<" "<<w_hits[i].size()<<std::endl;
        //}

        //Loop through all stations
        for (int i=0; i<nstations; ++i){
            int n_sp=0;
            //Create Spacepoint if there are no u/w constraints
            if (u_hits[i].size()==0 && w_hits[i].size()==0){
                for (size_t j=0; j<x_hits[i].size(); ++j){
                    for (size_t k=0; k<y_hits[i].size(); ++k){
                        double x = x_hits[i][j][0];
                        double y = y_hits[i][k][0];
                        double z = (1/2.)*(x_hits[i][j][1]+y_hits[i][k][1]);
                        double pos[3];
                        pos[0]=x; pos[1]=y; pos[2]=z;
                        //std::cout<<"Spacepoint (Event "<<event<<") at station "<<i<<": "<<x<<" "<<y<<" "<<z<<std::endl;

                        rb::SpacePoint sp;
                        sp.SetX(pos);
                        sp.SetStation(i);
                        SpacePoints->push_back(sp);
                        fSSD_Profile[i]->Fill(pos[0],pos[1]);
                        fSSD_Profile_notilt[i]->Fill(pos[0],pos[1]);
                        n_sp+=1;
                    }
                }
            }
            else{
                bool u_or_w; //0 for U sensors, 1 for W sensors
                std::vector<std::array<double,2>> tilt_hits;
                size_t nhits=0;
                if (u_hits[i].size()!=0){
                    u_or_w=0;
                    for (size_t iu=0; iu<u_hits[i].size(); ++iu){
                        tilt_hits.push_back({u_hits[i][iu][0],u_hits[i][iu][1]});
                    }
                    nhits=u_hits[i].size();
                }
                else if (w_hits[i].size()!=0){
                    u_or_w=1;
                    for (size_t iw=0; iw<w_hits[i].size(); ++iw){
                        tilt_hits.push_back({w_hits[i][iw][0],w_hits[i][iw][1]});
                    }
                    nhits=w_hits[i].size();
                }
                else{
                    std::cout<<"Attemping to use U/W sensors when there are no U/W hits"<<std::endl;
                    exit(1);
                }
                //std::cout<<"nhits = "<<nhits<<std::endl;
                //std::cout<<"xhits = "<<x_hits[i].size()<<std::endl;
                //std::cout<<"yhits = "<<y_hits[i].size()<<std::endl;

                for (size_t it=0; it<nhits; ++it){
                    size_t x_index; size_t y_index;
                    double final_x=-999; double final_y=-999; double final_z=-999;
                    double tilt = tilt_hits[it][0];
                    double tilt_diff = 999;
                    bool valid_space_point=0;
                    double final_diff=-999;
                    //if(x_hits[i].size()<nhits || y_hits[i].size()<nhits) continue;
                    for (size_t j=0; j<x_hits[i].size(); ++j){
                        for (size_t k=0; k<y_hits[i].size(); ++k){
                            double x = x_hits[i][j][0];
                            double y = y_hits[i][k][0];
                            double tilt_xy;
                            if (u_or_w) tilt_xy = (sqrt(2)/2)*((-1.)*x+y);
                            else tilt_xy = (sqrt(2)/2)*(x+y);
                            tilt_xy = (sqrt(2)/2)*((-1.)*x+y);
                            double diff = tilt_xy-tilt;
                            //**** Add extra constraint here to ensure that the wrong x,y points are not being associated with a u hit
                            if (abs(diff)<abs(tilt_diff) && abs(diff)<2.){
                                //std::cout<<"Event: "<<event<<"  Station:"<<i<<"  Diff = "<<diff<<std::endl;
                                tilt_diff=diff;
                                //final_x = x-diff/sqrt(2.);
                                final_x = x;
                                //final_y = y-diff/sqrt(2.);
                                final_y = y;
                                double check_w = (sqrt(2)/2)*((-1.)*final_x+final_y);
                                //std::cout<<"w_xy = "<<tilt_xy<<"  w = "<<tilt<<"  check_w = "<<check_w<<"  diff= "<<diff<<std::endl;
                                final_z = (1/3.)*(x_hits[i][j][1]+y_hits[i][k][1]+tilt_hits[it][1]);
                                x_index = j;
                                y_index = k;
                                final_diff=diff;
                                valid_space_point=1;
                            }
                        }
                    }
                    if (valid_space_point){
                        double pos[3];
                        pos[0]=final_x; pos[1]=final_y; pos[2]=final_z;
                        rb::SpacePoint sp;
                        sp.SetX(pos);
                        sp.SetStation(i);
                        SpacePoints->push_back(sp);
                        fSSD_Profile[i]->Fill(pos[0],pos[1]);
                        fSSD_Profile_onlytilt[i]->Fill(pos[0],pos[1]);
                        h_diff[i]->Fill(final_diff);
                        x_hits[i].erase(x_hits[i].begin()+x_index);
                        y_hits[i].erase(y_hits[i].begin()+y_index);
                        n_sp+=1;
                        //std::cout<<"Spacepoint (Event "<<event<<") at station "<<i<<": "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<std::endl;
                    }
                }
            }
            h_sp[i]->Fill(n_sp);
        }
    }


    //......................................................................
    void SpacePointReco::produce(art::Event& evt)
    { 
        std::string fClusterLabel = "clust";

        art::Handle< std::vector<rb::SSDCluster> > clustH;
        auto geo = geom->Geo();
        std::unique_ptr<std::vector<rb::SpacePoint> > SpacePointv(new std::vector<rb::SpacePoint>);
        event = evt.event();

        try {
            evt.getByLabel(fClusterLabel, clustH);
            if (!clustH->empty()) {	
                GetSpacePoint(clustH,  SpacePointv);
            }
        }
        catch(...) {

        }
    }
} // end namespace emph

DEFINE_ART_MODULE(emph::SpacePointReco)
