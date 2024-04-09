////////////////////////////////////////////////////////////////////////
/// \brief   Test module for various reconstruction tasks
/// \author  $Author: jmirabit@bu.edu
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
#include "TH2F.h"
#include "TF1.h"
#include "TLine.h"
#include <Math/Vector3D.h>
#include <Math/Functor.h>
#include <Fit/Fitter.h>

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
#include "SSDAlignment/SSDAlign.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/Spill.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {

    ///
    class MomReco : public art::EDProducer {
        public:
            explicit MomReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
            ~MomReco();

            // Optional, read/write access to event
            void produce(art::Event& evt);

            // Optional if you want to be able to configure from event display, for example
            void reconfigure(const fhicl::ParameterSet& pset);

            // Optional use if you have histograms, ntuples, etc you want around for every event
            void beginJob();
            void beginSubRun(art::SubRun& sr);
            //      void endSubRun(art::SubRun const&);
            void endJob();
            void Calculate();
            void Analyze();

        private:

            art::ServiceHandle<emph::geo::GeometryService> geom;
            art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
            art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
            int fEvtNum;
            int nstations;
            std::vector<double> x_shifts, y_shifts, u_shifts, v_shifts;
            std::vector<double> angles, angles_x;
            std::array<std::vector<double>,22> avg_hit_pos;
            double theta;
            TH1D* hSSDmom;
            TF1* fit;
            int evt_disp_counter=0;
            TGraph** evt_line;
            TGraph* inc_line;
            TGraph* out_line;
            TLine* inc_fit;
            TLine* out_fit;
            art::ServiceHandle<art::TFileService> tfs;
            std::vector<emph::al::SSDAlign> ssdhits;
            const double pitch = 0.06;
            bool first_run=1;
            std::map<std::vector<int>, int> spsindex;
            double targetpos;
            double magnetpos;
            double max_z = 0;
            int nxsensors=0;
            int nysensors=0;
            int nusensors=0;
            int nvsensors=0;
            int f_points,i_points;
            int mom;
    };

    //.......................................................................

    MomReco::MomReco(fhicl::ParameterSet const& pset)
        : EDProducer(pset)
    {
        //this->reconfigure(pset);
        fEvtNum = 0;

    }

    //......................................................................

    MomReco::~MomReco()
    {
        //======================================================================
        // Clean up any memory allocated by your module
        //======================================================================
    }

    //......................................................................

    // void MomReco::reconfigure(const fhicl::ParameterSet& pset)
    // {    
    // }

    //......................................................................

    void MomReco::beginJob()
    {

    }

    //......................................................................
    void MomReco::beginSubRun(art::SubRun& sr)
    {
        if(first_run){
            art::Handle<rb::Spill> spillHandle;
            try {
                sr.getByLabel("spillinfo",spillHandle);
                mom = spillHandle->Momentum();
            }
            catch(...) {
                std::cout << "No spill info object found!  Aborting..." << std::endl;
                std::abort();
            }

            auto fChannelMap = cmap->CMap();
            auto emgeo = geom->Geo();
            nstations = emgeo->NSSDStations();
            targetpos = emgeo->TargetUSZPos();
            magnetpos = emgeo->MagnetUSZPos();
            evt_line = new TGraph*[10];

            for (int fer=0; fer<10; ++fer){
                for (int mod=0; mod<6; ++mod){
                    emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
                    if (!fChannelMap->IsValidEChan(echan)) continue;
                    emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);

                    const emph::geo::SSDStation *st = emgeo->GetSSDStation(dchan.Station());
                    const emph::geo::Plane      *pln = st->GetPlane(dchan.Plane());
                    const emph::geo::Detector   *sd = pln->SSD(dchan.HiLo());

                    emph::al::SSDAlign sensor_info(*sd,*st);
                    std::vector<int> sps = {dchan.Station(),dchan.Plane(),dchan.HiLo()};
                    if (sensor_info.Z()>max_z) max_z=sensor_info.Z();
                }
            }
        }
        first_run=0;
    }

    //......................................................................

    void MomReco::endJob()
    {
        Analyze();
    }

    //......................................................................

    void MomReco::Calculate()
    {
        std::vector<double> xpos;
        std::vector<double> ypos;
        std::vector<double> zpos;

        //For each station check if there is a single hit in x
        bool s1 = ssdhits[0].IsAlignmentEvent2(ssdhits,nstations);
        if (s1){
            for (size_t i=0; i<ssdhits.size(); ++i){
                xpos.push_back(ssdhits[i].X());
                zpos.push_back(ssdhits[i].Z());
            }
        }

        //Only fit event with at least 2 hits before and after magnet(can switch to target)
        int nhitsbefore=0;
        int nhitsafter=0;
        for (size_t i=0; i<zpos.size(); ++i){
            if (zpos[i]<magnetpos) nhitsbefore+=1;
            if (zpos[i]>magnetpos) nhitsafter+=1;
        }

        //Linear Fit function
        TF1* fit = new TF1("fit","[0] +[1]*x",0,1800);
        //Loop over events with at most 1 x hit in each station and at least 2 before and after magnet (or target)
        if (s1 && nhitsbefore >1 &&  nhitsafter>1){
            std::array<double,3> dir_i; //Initial direction
            std::array<double,3> dir_f; //Final direction

            //Fit line segment to track before magnet
            TGraph* inc_line = new TGraph();
            TGraph* out_line = new TGraph();
            i_points=0;
            f_points=0;
            for (size_t j=0; j<zpos.size(); ++j){
                if(zpos[j]<magnetpos){
                    inc_line->SetPoint(i_points,zpos[j],xpos[j]);
                    i_points+=1;
                }
                if(zpos[j]>magnetpos){
                    out_line->SetPoint(f_points,zpos[j],xpos[j]);
                    f_points+=1;
                }
            }
            //Get direction for inc line
            inc_line->Fit(fit,"Q0");
            double inc_slope = fit->GetParameter(1);
            double inc_angle = atan(inc_slope);

            out_line->Fit(fit,"Q0");
            double out_slope = fit->GetParameter(1);
            double out_angle = atan(out_slope);


            double theta_x = out_angle-inc_angle;

            //std::cout<<"Angle (degrees) = "<<theta*(180/M_PI)<<std::endl;
            angles_x.push_back(theta_x);
            //if(evt_disp_counter<10 && theta_x<0){
            if(evt_disp_counter<10){
                std::cout<<"here "<<zpos[0]<<" "<<xpos[0]<<std::endl;
                evt_line[evt_disp_counter] = tfs->makeAndRegister<TGraph>(Form("EvtDisp%i",evt_disp_counter),Form("Event %i",fEvtNum));
                for (size_t j=0; j<zpos.size(); ++j){
                    evt_line[evt_disp_counter]->SetPoint(j,zpos[j],xpos[j]);
                }
                evt_line[evt_disp_counter]->SetMarkerStyle(21);
                evt_line[evt_disp_counter]->GetXaxis()->SetTitle("z pos (mm)");
                evt_line[evt_disp_counter]->GetYaxis()->SetTitle("x pos (mm)");
                evt_line[evt_disp_counter]->GetXaxis()->SetLimits(0,max_z);
                evt_line[evt_disp_counter]->GetYaxis()->SetRangeUser(-40,40);

                evt_disp_counter+=1;
            }


            delete inc_line;
            delete out_line;
        }
    }
    //......................................................................

    void MomReco::Analyze(){
        std::cout<<"Number of Events to analyze = "<<angles_x.size()<<std::endl;
        const double p_to_GeV = 1/(5.36e-19);
        const double e = 1.6e-19;
        hSSDmom = tfs->make<TH1D>("SSDMom",Form("%i GeV/c pion: Reconstructed Momentum",mom),200,mom-5,mom+5);
        //hSSDmom = tfs->make<TH1D>("SSDMom","-20 GeV/c pion: Reconstructed Momentum",600,-30,30);
        hSSDmom->GetXaxis()->SetTitle("p (GeV/c)");
        for (size_t i=0; i<10; ++i){
            //std::cout<<"x-Angle (degrees) = "<<angles_x[i]*(180/M_PI)<<std::endl;
        }
        for (size_t i=0; i<angles_x.size(); ++i){
            //double p = (L*B)/(angles_x[i]);
            double p = p_to_GeV*e*(0.15*1.4)/(angles_x[i]);
            hSSDmom->Fill(p);
            //if (i<10) std::cout<<"p = "<<p<<" GeV/c"<<std::endl;
        }
        for (size_t i=0; i<avg_hit_pos.size();++i){
            double length = avg_hit_pos[i].size();
            double sum = 0;
            for (size_t j=0; j<length; ++j){
                sum += avg_hit_pos[i][j];
            }
            if(length !=0){
               sum = sum/length;
            }
        }
        std::cout<<"Magnet Pos = "<<magnetpos<<" mm"<<std::endl;
        std::cout<<"Momentum of Run = "<<mom<<" GeV/c"<<std::endl;

    }
    //......................................................................

    //......................................................................
    void MomReco::produce(art::Event& evt)
    { 
        std::string fClusterLabel = "clust";

        art::Handle< std::vector<rb::SSDCluster> > clustH;

        try {
            evt.getByLabel(fClusterLabel, clustH);
            if (!clustH->empty()) {	
                for (size_t idx=0; idx < clustH->size(); ++idx) {
                    const rb::SSDCluster& clust = (*clustH)[idx];
                    rb::LineSegment ls;

                    if (clust.AvgStrip() > 640){
                        std::cout<<"Skipping nonsense SSD Hit"<<std::endl;
                        continue;
                    }

                    int event = evt.event();
                    emph::al::SSDAlign hit(clust, event);

                    dgm->Map()->SSDClusterToLineSegment(clust, ls);
                    hit.SetPos(ls);

                    //Only push back x events for now, ignore the rest
                    if(hit.View()==emph::geo::X_VIEW) {
                        ssdhits.push_back(hit);
                    }
                    if(hit.View()==emph::geo::Y_VIEW) {
                        continue;
                    }
                    //Skip u,w for now
                    if(hit.View()==emph::geo::U_VIEW) {
                        continue;
                    }
                    if(hit.View()==emph::geo::W_VIEW) {
                        continue;
                    }
                }
                //If there are more clusters than sensors, skip event
                if (ssdhits.size()<99){
                    Calculate();
                }
                ssdhits.clear();
                fEvtNum++;
            }
            //Analyze();
        }
        catch(...) {

        }
    }
} // end namespace emph

DEFINE_ART_MODULE(emph::MomReco)
