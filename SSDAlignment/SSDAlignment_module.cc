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
#include "SSDAlignment/SSDAlign.h"
#include "RecoBase/SSDCluster.h"

//using namespace emph;

namespace emph {

    ///
    class SSDAlignment : public art::EDAnalyzer {
        public:
            explicit SSDAlignment(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
            ~SSDAlignment();

            // Optional, read/write access to event
            void analyze(const art::Event& evt);

            // Optional if you want to be able to configure from event display, for example
            //void reconfigure(const fhicl::ParameterSet& pset);

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
            int nvsensors=0;
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
            std::vector<TH1F*> fVSSD_Profile;
            std::vector<TH2F*> fadj_SSD_Profile;
            std::vector<TH1F*> fX_Residual_Init;
            std::vector<TH1F*> fY_Residual_Init;
            std::vector<TH1F*> fX_Residual_Fin;
            std::vector<TH1F*> fY_Residual_Fin;
            std::vector<TH1F*> fU_Residual_Fin;
            std::vector<TH1F*> fV_Residual_Fin;
            std::array<TH1F*,4> fInit_Chi2;
            std::array<TH1F*,4> fFin_Chi2;
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
    };

    //.......................................................................

    SSDAlignment::SSDAlignment(fhicl::ParameterSet const& pset)
        : EDAnalyzer(pset)
    {
        //this->reconfigure(pset);
        evt_disp = new TGraph*[10];
        evt_disp_adj = new TGraph*[10];
    }

    //......................................................................

    SSDAlignment::~SSDAlignment()
    {
        //======================================================================
        // Clean up any memory allocated by your module
        //======================================================================
    }

    //......................................................................

    // void SSDAlignment::reconfigure(const fhicl::ParameterSet& pset)
    // {    
    // }

    //......................................................................

    void SSDAlignment::beginJob()
    {
    }

    //......................................................................

    void SSDAlignment::Initialize()
    {
        // initialize channel map
        auto fChannelMap = cmap->CMap();
        auto emgeo = geom->Geo();

        nstations = emgeo->NSSDStations();
        nssds = emgeo->NSSDs();
        magnetpos = emgeo->MagnetDSZPos();

        //resizing to fill with z positions
        std::cout<<"SSD Stations and Axis Indices Below:"<<std::endl;
        std::array<std::string,5> sensorviews = {"init","x","y","u","w"};
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
                    spsindex.insert(std::pair<std::vector<int>,int>(sps,nvsensors));
                    zpos_v.push_back(sensor_info.Z());
                    nvsensors+=1;
                }
                if (sensor_info.Z()>max_z) max_z=sensor_info.Z();
                std::cout<<"***** Testing map index: "<<sps[0]<<"  "<<sps[1]<<"  "<<sps[2]<<"   index value = "<<spsindex[sps]<<"   view: "<<sensorviews[sensor_info.View()]<<"   rotation = "<<sd->Rot()<<"  "<<sd->Rot()*180/3.1415<<std::endl;
            }
        }
        //Remove duplicate members
        fSSD_Profile.resize(nstations); fadj_SSD_Profile.resize(nstations);
        fXSSD_Profile.resize(nstations); fYSSD_Profile.resize(nstations);
        fUSSD_Profile.resize(nstations);
        fVSSD_Profile.resize(nstations);
        fX_Residual_Init.resize(nxsensors); fY_Residual_Init.resize(nysensors);
        fX_Residual_Fin.resize(nxsensors); fY_Residual_Fin.resize(nysensors);
        fU_Residual_Fin.resize(nxsensors); fV_Residual_Fin.resize(nysensors);
        std::array<const char*,4> pos_strings = {"x","y","u","v"};
        char hname[64];
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"SSDProfile_%d",i);
            fSSD_Profile[i] = tfs->make<TH2F>(hname,Form("Station %i",i),160,-40,40,160,-40,40);
            fSSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
            fSSD_Profile[i]->GetYaxis()->SetTitle("Y Position (mm)");
            //fSSD_Profile[i]->SetBit(TH1::kNoStats);
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"adj_SSDProfile_%d",i);
            fadj_SSD_Profile[i] = tfs->make<TH2F>(hname,Form("Station %i",i),160,-40,40,160,-40,40);
            fadj_SSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
            fadj_SSD_Profile[i]->GetYaxis()->SetTitle("Y Position (mm)");
            //fadj_SSD_Profile[i]->SetBit(TH1::kNoStats);
        }

        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"x_SSDProfile_%d",i);
            fXSSD_Profile[i] = tfs->make<TH1F>(hname,Form("Sensor %i",i),160,-40,40);
            fXSSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"y_SSDProfile_%d",i);
            fYSSD_Profile[i] = tfs->make<TH1F>(hname,Form("Sensor %i",i),160,-40,40);
            fYSSD_Profile[i]->GetXaxis()->SetTitle("Y Position (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"u_SSDProfile_%d",i);
            fUSSD_Profile[i] = tfs->make<TH1F>(hname,Form("Sensor %i",i),160,-40,40);
            fUSSD_Profile[i]->GetXaxis()->SetTitle("U Position (mm)");
        }
        for (int i=0; i<nstations; ++i) {
            sprintf(hname,"v_SSDProfile_%d",i);
            fVSSD_Profile[i] = tfs->make<TH1F>(hname,Form("Sensor %i",i),160,-40,40);
            fVSSD_Profile[i]->GetXaxis()->SetTitle("V Position (mm)");
        }

        for (int i=0; i<nxsensors; ++i) {
            sprintf(hname,"XInit_Resid_%d",i);
            fX_Residual_Init[i] = tfs->make<TH1F>(hname,Form("Initial X Residuals - SSD %i",i),400,-40,40);
            fX_Residual_Init[i]->GetXaxis()->SetTitle("Residual");
            fX_Residual_Init[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<nysensors; ++i) {
            sprintf(hname,"YInit_Resid_%d",i);
            fY_Residual_Init[i] = tfs->make<TH1F>(hname,Form("Initial Y Residuals - SSD %i",i),400,-40,40);
            fY_Residual_Init[i]->GetXaxis()->SetTitle("Residual");
            fY_Residual_Init[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<nxsensors; ++i) {
            sprintf(hname,"XFin_Resid_%d",i);
            fX_Residual_Fin[i] = tfs->make<TH1F>(hname,Form("Final X Residuals - SSD %i",i),400,-40,40);
            fX_Residual_Fin[i]->GetXaxis()->SetTitle("Residual");
            fX_Residual_Fin[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<nysensors; ++i) {
            sprintf(hname,"YFin_Resid_%d",i);
            fY_Residual_Fin[i] = tfs->make<TH1F>(hname,Form("Final Y Residuals - SSD %i",i),400,-40,40);
            fY_Residual_Fin[i]->GetXaxis()->SetTitle("Residual");
            fY_Residual_Fin[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<nusensors; ++i) {
            sprintf(hname,"UFin_Resid_%d",i);
            fU_Residual_Fin[i] = tfs->make<TH1F>(hname,Form("Final U Residuals - SSD %i",i),400,-40,40);
            fU_Residual_Fin[i]->GetXaxis()->SetTitle("Residual");
            fU_Residual_Fin[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<nvsensors; ++i) {
            sprintf(hname,"VFin_Resid_%d",i);
            fV_Residual_Fin[i] = tfs->make<TH1F>(hname,Form("Final V Residuals - SSD %i",i),400,-40,40);
            fV_Residual_Fin[i]->GetXaxis()->SetTitle("Residual");
            fV_Residual_Fin[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<4; ++i) {
            sprintf(hname,"%sInit_Chi2",pos_strings[i]);
            fInit_Chi2[i] = tfs->make<TH1F>(hname,Form("Initial Chi2 - %s",pos_strings[i]),100,0,10);
            fInit_Chi2[i]->GetXaxis()->SetTitle("Chi^2");
            fInit_Chi2[i]->GetYaxis()->SetTitle("Counts");
        }
        for (int i=0; i<4; ++i) {
            sprintf(hname,"%sFin_Chi2",pos_strings[i]);
            fFin_Chi2[i] = tfs->make<TH1F>(hname,Form("Final Chi2 - %s",pos_strings[i]),100,0,10);
            fFin_Chi2[i]->GetXaxis()->SetTitle("Chi^2");
            fFin_Chi2[i]->GetYaxis()->SetTitle("Counts");
        }
        first_run=0;
    }

    //......................................................................

    void SSDAlignment::endJob()
    {
        SSDFit();
        //Then output the alignment constants to ConstBase/SSDAlignment.dat
    }

    //......................................................................

    void SSDAlignment::SSDFit()
    {
        //First we need to initialize SSD configuration and plots based on the geometry
        if (first_run) Initialize();

        //Fit the ssdvectors and obtain x, y shifts
        std::cout<<"*******************************************"<<std::endl;
        std::cout<<"Main code"<<std::endl;

        //Initialize vectors to hold positions from events used for alignment
        std::vector<std::vector<emph::al::SSDAlign>> x_cal;
        std::vector<std::vector<emph::al::SSDAlign>> y_cal;
        std::vector<std::vector<emph::al::SSDAlign>> u_cal;
        std::vector<std::vector<emph::al::SSDAlign>> v_cal;


        //Position uncertainty of SSDs
        std::cout<<"ssdvec size is  "<<ssdvec.size()<<std::endl;
        double sigma = 0.06/(sqrt(12)); // pitch=0.06
        //double sigma = 1;
        int nevt = ssdvec[0].Event(); //Variable to keep track of event number

        //Looping over all SSD hits
        for (size_t i=0; i<ssdvec.size(); ++i){
            //if statement to indicate end of event, events are filtered and pushed to finalized arrays for alignment, then cleared for next event
            if (ssdvec[i].Event()!=nevt){
                //Fill each axis event holder
                evt_holder[i].FillEvtHolder(evt_holder,xevt_holder,yevt_holder,uevt_holder,vevt_holder);

                //Keeping good events for x alignment	
                if(ssdvec[i].IsAlignmentEvent2(xevt_holder,nstations)){
                    x_cal.push_back(xevt_holder);
                }	

                //Keeping good events for y alignment	
                if(ssdvec[i].IsAlignmentEvent2(yevt_holder,nstations)){
                    y_cal.push_back(yevt_holder);
                }

                //Keeping good events for u alignment	
                if(ssdvec[i].IsAlignmentEvent2(xevt_holder,yevt_holder,uevt_holder,nstations)){
                    u_cal.push_back(uevt_holder);
                }

                //Keeping good events for v alignment	
                if(ssdvec[i].IsAlignmentEvent2(xevt_holder,yevt_holder,vevt_holder,nstations)){
                    v_cal.push_back(vevt_holder);
                }

                nevt = ssdvec[i].Event();

                //Reset event holder for next event			
                evt_holder.clear();
                xevt_holder.clear();
                yevt_holder.clear();
                uevt_holder.clear();
                vevt_holder.clear();
            }
            //Add axis index integer to object based on SSD geometry from spsindex map
            std::vector<int> sps = {ssdvec[i].Station(),ssdvec[i].Plane(), ssdvec[i].Sensor()};
            ssdvec[i].SetAxisIndex(spsindex[sps]);

            //Fill event holder with SSDAlign objects
            evt_holder.push_back(ssdvec[i]);
        }

        //Residual Method of Alignment
        int loops=40;
        size_t dim = nstations; //number of position measurements
        art::ServiceHandle<art::TFileService> tfs;
        std::cout<<"Dimension: "<<dim<<std::endl;

        //Linear Fit function
        TF1* fit = new TF1("fit","[0] +[1]*x",0,max_z);
        std::vector<double> x_shifts(nxsensors,0);
        std::vector<double> y_shifts(nysensors,0);
        std::vector<double> u_shifts(nusensors,0);
        std::vector<double> v_shifts(nvsensors,0);

        int bindex = 0;
        for (int i=0; i<loops; ++i){
            std::cout<<"Beginning Loop "<<i<<std::endl;
            std::vector<std::vector<double>> xres_array(nxsensors);
            
            //Looping through alignment events and calculating x-residuals
            for (size_t j=0; j<x_cal.size(); ++j){
                size_t event_hits = x_cal[j].size(); //number of SSD sensors hit in event
                std::vector<emph::al::SSDAlign*> x_evt(event_hits);
                for (size_t k=0; k<x_cal[j].size(); ++k) x_evt[k] = &x_cal[j][k]; 
                std::vector<double> x_adj(event_hits);
                std::vector<double> xzpos(event_hits);
                std::vector<int> x_ind(event_hits);

                for (size_t k=0; k<event_hits; ++k){
                    //Set Shift (will be 0 for first loop)
                    x_evt[k]->SetShift(x_shifts[x_evt[k]->AxisIndex()]);
                    x_adj[k] = x_evt[k]->X() - x_evt[k]->Shift();
                    xzpos[k] = x_evt[k]->Z();
                    //if(i==0 && j<10)std::cout<<"x measured = "<<x_adj[k]<<"   x raw ="<< x_evt[k].X()<<"    index = "<<x_evt[k].AxisIndex()<<"   k = "<<k<<std::endl;
                    x_ind[k] = x_evt[k]->AxisIndex();
                }

                TGraph* evt_line = new TGraph(event_hits, &xzpos[0], &x_adj[0]);
                evt_line->Fit(fit,"Q0");

                //Checking Residual for each point
                for(size_t k=0; k<event_hits; ++k){
                    double res = x_adj[k] - fit->Eval(xzpos[k]);
                    double chi2 = (res*res)/(sigma*sigma);

                    //Filling residual plots using correct index of SSD
                    if(i==1){
                        fX_Residual_Init[x_ind[k]]->Fill(res/sigma);
                        fInit_Chi2[0]->Fill(chi2);
                    }
                    if(i==loops-1){
                        fX_Residual_Fin[x_ind[k]]->Fill(res/sigma);
                        fFin_Chi2[0]->Fill(chi2);
                    }
                    xres_array[x_ind[k]].push_back(res);

                    //Removing events that have very poor alignment
                    //if (i%10==0 && i!=0){
                    //    auto iterator = x_cal.begin()+j;
                    //    if (res/sigma > 4) x_cal.erase(iterator);
                    //}
                }
                delete evt_line;
                //Store first 10 events (unaligned) to look at
                int event_holder = x_evt[0]->Event();
                if (j<10 && i==0){
                    evt_disp[j] = tfs->makeAndRegister<TGraph>(Form("event_%i",event_holder),"Unaligned Event",event_hits, &xzpos[0], &x_adj[0]);
                    evt_disp[j]->Fit(fit,"Q");
                    evt_disp[j]->SetMarkerStyle(21);
                    evt_disp[j]->SetTitle(Form("Unaligned Event %i",event_holder));
                    evt_disp[j]->GetXaxis()->SetTitle("z pos (mm)");
                    evt_disp[j]->GetXaxis()->SetLimits(0,max_z);
                    evt_disp[j]->GetYaxis()->SetTitle("x pos (mm)");
                    evt_disp[j]->GetYaxis()->SetRangeUser(-40,40);
                }	
                //if (bindex<10 && i==loops-1 && x_ind[5]==6)
                if (j<10 && i==loops-1){
                    bindex+=1;
                    //std::cout<<"x_ind[5]=="<<x_ind[5]<<std::endl;
                    evt_disp_adj[bindex] = tfs->makeAndRegister<TGraph>(Form("adj_event_%i",event_holder),"Aligned Event",event_hits, &xzpos[0], &x_adj[0]);
                    evt_disp_adj[bindex]->Fit(fit,"Q");
                    evt_disp_adj[bindex]->SetMarkerStyle(21);
                    evt_disp_adj[bindex]->SetTitle(Form("Aligned Event %i",event_holder));
                    evt_disp_adj[bindex]->GetXaxis()->SetTitle("z pos (mm)");
                    evt_disp_adj[bindex]->GetXaxis()->SetLimits(0,max_z);
                    evt_disp_adj[bindex]->GetYaxis()->SetTitle("x pos (mm)");
                    evt_disp_adj[bindex]->GetYaxis()->SetRangeUser(-40,40);
                }
                //Clear vectors that are redefined at top 
                x_evt.clear(); x_adj.clear(); xzpos.clear(); x_ind.clear();
            }

            //Repeat Process for Y
            std::vector<std::vector<double>> yres_array(nysensors);
            
            //Looping through alignment events and calculating y-residuals
            for (size_t j=0; j<y_cal.size(); ++j){
                size_t event_hits = y_cal[j].size();
                std::vector<emph::al::SSDAlign*> y_evt(event_hits);
                for (size_t k=0; k<y_cal[j].size(); ++k) y_evt[k] = &y_cal[j][k]; 
                std::vector<double> y_adj(event_hits);
                std::vector<double> yzpos(event_hits);
                std::vector<int> y_ind(event_hits);
                for (size_t k=0; k<event_hits; ++k){
                    y_evt[k]->SetShift(y_shifts[y_evt[k]->AxisIndex()]);
                    y_adj[k] = y_evt[k]->Y() - y_evt[k]->Shift();
                    yzpos[k] = y_evt[k]->Z();
                    y_ind[k] = y_evt[k]->AxisIndex();
                }
                TGraph* evt_line = new TGraph(event_hits, &yzpos[0], &y_adj[0]);
                evt_line->Fit(fit,"Q0");

                //Checking Residual for each point
                for(size_t k=0; k<event_hits; ++k){
                    double res = y_adj[k] - fit->Eval(yzpos[k],0,0);
                    double chi2 = (res*res)/(sigma*sigma);

                    //Filling residual plots using correct index of SSD
                    if(i==1){
                        fY_Residual_Init[y_ind[k]]->Fill(res/sigma);
                        fInit_Chi2[1]->Fill(chi2);
                    }
                    if(i==loops-1){
                        fY_Residual_Fin[y_ind[k]]->Fill(res/sigma);
                        fFin_Chi2[1]->Fill(chi2);
                    }
                    yres_array[y_ind[k]].push_back(res);
                }
                delete evt_line;
                //Clear vectors that are redefined at top 
                y_evt.clear(); y_adj.clear(); yzpos.clear(); y_ind.clear();
            }

            //Repeat Process for U
            std::vector<std::vector<double>> ures_array(nusensors);
            std::vector<double> u_adj(nstations);
            std::vector<double> uzpos(nstations);
            std::vector<int> u_ind(nstations);

            //Looping through alignment events and calculating u-residuals
            for (size_t j=0; j<u_cal.size(); ++j){
                //if(i==0 && j<10)std::cout<<"*** new event ***     event size = "<<u_cal[j].size()<<std::endl;
                std::vector<emph::al::SSDAlign> u_evt = u_cal[j];
                int evt_index=0;
                for (size_t k=0; k<dim; ++k){
                    //if(i==1 && j<10)std::cout<<" evt index = "<<evt_index<<std::endl;
                    if(u_evt[evt_index].View()==emph::geo::U_VIEW){
                        u_evt[evt_index].SetShift(u_shifts[u_evt[evt_index].AxisIndex()]);
                        u_adj[k] = u_evt[evt_index].U() - u_evt[evt_index].Shift();
                        uzpos[k] = u_evt[k].U();
                        //if(i==1 && j<10)std::cout<<"u measured = "<<u_adj[k]<<"   u raw ="<< u_evt[evt_index].U()<<"    index = "<<u_evt[evt_index].AxisIndex()<<"   k = "<<k<<std::endl;
                        u_ind[k] = u_evt[evt_index].AxisIndex();
                        evt_index+=1;
                        //std::cout<<"WE MADE IT HERE   "<<j<<std::endl;
                    }
                    else{
                        double temp_xval=-999; double temp_yval=-999;
                        temp_xval = u_evt[evt_index].X() - u_evt[evt_index].Shift();
                        evt_index+=1;
                        temp_yval = u_evt[evt_index].Y() - u_evt[evt_index].Shift();
                        evt_index+=1;
                        u_adj[k] = (sqrt(2)/2)*(temp_xval-temp_yval);
                        uzpos[k] = u_evt[evt_index].Z();
                        //std::cout<<"is this working?   "<<u_adj[k]<<"   index"<<k<<std::endl;
                    }
                }
                TGraph* evt_line = new TGraph(dim, &uzpos[0], &u_adj[0]);
                evt_line->Fit(fit,"Q0");

                //Checking Residual for each point
                for(size_t k=0; k<dim; ++k){
                    double res = u_adj[k] - fit->Eval(uzpos[k],0,0);
                    double chi2 = (res*res)/(sigma*sigma);

                    //Filling residual plots using correct index of SSD
                    if(i==1){
                        fInit_Chi2[2]->Fill(chi2);
                    }
                    if(i==loops-1){
                        fU_Residual_Fin[u_ind[k]]->Fill(res/sigma);
                        fFin_Chi2[2]->Fill(chi2);
                    }
                    ures_array[u_ind[k]].push_back(res);
                }
                delete evt_line;
                //Clear vectors that are redefined at top 
                u_evt.clear(); u_adj.clear(); uzpos.clear(); u_ind.clear();
            }

            //Repeat Process for V
            std::vector<std::vector<double>> vres_array(nvsensors);
            std::vector<double> v_adj(nstations);
            std::vector<double> vzpos(nstations);
            std::vector<int> v_ind(nstations);

            for (size_t j=0; j<v_cal.size(); ++j){
                std::vector<emph::al::SSDAlign> v_evt = v_cal[j];
                int evt_index=0;
                //if(i==1 && j<10)std::cout<<"*** new event ***     event size = "<<v_cal[j].size()<<std::endl;
                for (size_t k=0; k<dim; ++k){
                    if(v_evt[evt_index].View()==emph::geo::W_VIEW){
                        v_evt[evt_index].SetShift(v_shifts[v_evt[evt_index].AxisIndex()]);
                        v_adj[k] = v_evt[evt_index].V() - v_evt[evt_index].Shift();
                        vzpos[k] = v_evt[k].Z();
                        //if(i==loops-1 && j<10)std::cout<<"v measured = "<<v_adj[k]<<"   v raw ="<< v_evt[evt_index].V()<<"    index = "<<v_evt[evt_index].AxisIndex()<<"   k = "<<k<<std::endl;
                        v_ind[k] = v_evt[evt_index].AxisIndex();
                        evt_index+=1;
                    }
                    else{
                        double temp_xval=-999; double temp_yval=-999;
                        temp_xval = v_evt[evt_index].X() - v_evt[evt_index].Shift();
                        evt_index+=1;
                        temp_yval = v_evt[evt_index].Y() - v_evt[evt_index].Shift();
                        evt_index+=1;
                        v_adj[k] = (sqrt(2)/2)*(temp_xval+temp_yval);
                        vzpos[k] = v_evt[evt_index].Z();
                    }
                }

                TGraph* evt_line = new TGraph(dim, &vzpos[0], &v_adj[0]);
                evt_line->Fit(fit,"Q0");

                //Checking Residual for each point
                for(size_t k=0; k<dim; ++k){
                    double res = v_adj[k] - fit->Eval(vzpos[k],0,0);
                    double chi2 = (res*res)/(sigma*sigma);

                    //Filling residual plots using correct index of SSD
                    if(i==1){
                        fInit_Chi2[3]->Fill(chi2);
                    }
                    if(i==loops-1){
                        fV_Residual_Fin[v_ind[k]]->Fill(res/sigma);
                        fFin_Chi2[3]->Fill(chi2);
                    }
                    vres_array[v_ind[k]].push_back(res);
                }
                delete evt_line;
                //Clear vectors that are redefined at top 
                v_evt.clear(); v_adj.clear(); vzpos.clear(); v_ind.clear();
            }
            

            //variables to fix first x/y ssd station and x/y station after magnet
            size_t x_station=0; size_t y_station; 
            double x_ref=0, y_ref=0;
            std::cout<<"X Shifts are:    ";
            for(size_t j=0; j<x_shifts.size(); ++j){
                double sum=0;
                //Calculating average residual
                for(size_t k=0; k<xres_array[j].size(); ++k){
                    sum +=xres_array[j][k];
                }
                double mean = sum / xres_array[j].size();
                x_shifts[j] += mean;
                if (j==0) x_ref = x_shifts[0];
                if (j==x_fixedstationindex) x1z = zpos_x[j];
            }
            //Fixing first X SSD after magnet to be x=0 (shift all SSDs in X by x_shift[0])
            double xslope = (x_shifts[x_fixedstationindex]-x_shifts[0])/(x1z-x0z);
            for(size_t j=0; j<x_shifts.size(); ++j){
                x_shifts[j] = x_shifts[j]-x_ref - xslope*(zpos_x[j]-zpos_x[0]);
                std::cout<<x_shifts[j]<<", ";
            }
            std::cout<<std::endl;

            std::cout<<"Y Shifts are:    ";
            for(size_t j=0; j<y_shifts.size(); ++j){
                double sum=0;
                //Calculating average residual
                for(size_t k=0; k<yres_array[j].size(); ++k){
                    sum +=yres_array[j][k];
                }
                double mean = sum / yres_array[j].size();
                y_shifts[j] += mean;
                if (j==0) y_ref = y_shifts[0];
                if (j==y_fixedstationindex) y1z = zpos_y[j];
            }
            //Fixing first Y SSD after magnet to be y=0 (shift all SSDs in Y by y_shift[0])
            double yslope = (y_shifts[y_fixedstationindex]-y_shifts[0])/(y1z-y0z);
            for(size_t j=0; j<y_shifts.size(); ++j){
                y_shifts[j] = y_shifts[j]-y_ref - yslope*(zpos_y[j]-zpos_y[0]);
                std::cout<<x_shifts[j]<<", ";
            }
            std::cout<<std::endl;

            std::cout<<"U Shifts are:    ";
            for(size_t j=0; j<u_shifts.size(); ++j){
                double sum=0;
                //Calculating average residual
                for(size_t k=0; k<ures_array[j].size(); ++k){
                    sum +=ures_array[j][k];
                }
                double mean = sum / ures_array[j].size();
                u_shifts[j] += mean;
                //Fixing first X/Y SSD at (0,0) (shift all u SSDs accordingly)
                //u_shifts[j]=u_shifts[j]-(sqrt(2)/2)*(x_ref-y_ref);
                std::cout<<u_shifts[j]<<", ";
            }
            std::cout<<std::endl;

            if (v_shifts.size()!=0){
            std::cout<<"V Shifts are:    ";
            for(size_t j=0; j<v_shifts.size(); ++j){
                double sum=0;
                //Calculating average residual
                for(size_t k=0; k<vres_array[j].size(); ++k){
                    sum +=vres_array[j][k];
                }
                double mean = sum / vres_array[j].size();
                v_shifts[j] += mean;
                //Fixing first X/Y SSD at (0,0) (shift all v SSDs accordingly)
                //v_shifts[j]=v_shifts[j]-(sqrt(2)/2)*(x_ref+y_ref);
                std::cout<<v_shifts[j]<<", ";
            }
            std::cout<<std::endl;
            }

            //Clear residual arrays
            xres_array.clear();  yres_array.clear(); ures_array.clear(); vres_array.clear();
        }
        //Delete fit function
        delete fit;
        std::cout<<"Number of X Alignment Events: "<<x_cal.size()<<std::endl;
        std::cout<<"Number of Y Alignment Events: "<<y_cal.size()<<std::endl;
        std::cout<<"Number of U Alignment Events: "<<u_cal.size()<<std::endl;
        std::cout<<"Number of V Alignment Events: "<<v_cal.size()<<std::endl;

        //Checking SSD Shifts by Filling SSD Profiles with all hits that have x and y data
        double x=0;
        double y=0;
        double true_x=0;
        double true_y=0;
        std::vector<int> xstations_hit;
        std::vector<int> ystations_hit;
        for (size_t i=0; i<ssdvec.size(); ++i){
            //if statement to indicate end of event, events are filtered and pushed to finalized arrays for calculating residuals, then cleared for next event
            if (ssdvec[i].Event()!=nevt){
                evt_holder[i].FillEvtHolder(evt_holder,xevt_holder,yevt_holder,uevt_holder,vevt_holder);
                for (size_t j=0; j<xevt_holder.size(); ++j){
                    xevt_holder[j].SetShift(x_shifts[xevt_holder[j].AxisIndex()]);
                    xstations_hit.push_back(xevt_holder[j].Station());
                }
                for (size_t j=0; j<yevt_holder.size(); ++j){
                    yevt_holder[j].SetShift(y_shifts[yevt_holder[j].AxisIndex()]);
                    ystations_hit.push_back(yevt_holder[j].Station());
                }
                for (size_t j=0; j<uevt_holder.size(); ++j) uevt_holder[j].SetShift(u_shifts[uevt_holder[j].AxisIndex()]);
                for (size_t j=0; j<vevt_holder.size(); ++j) vevt_holder[j].SetShift(v_shifts[vevt_holder[j].AxisIndex()]);
                for (size_t k=0; k<xevt_holder.size(); ++k){
                    if(xevt_holder[k].View()==emph::geo::X_VIEW){
                        fXSSD_Profile[xevt_holder[k].Station()]->Fill(xevt_holder[k].X());
                    }
                }
                for (size_t k=0; k<yevt_holder.size(); ++k){
                    if(yevt_holder[k].View()==emph::geo::Y_VIEW){
                        fYSSD_Profile[yevt_holder[k].Station()]->Fill(yevt_holder[k].Y());
                    }
                }
                for (size_t k=0; k<uevt_holder.size(); ++k){
                    if(uevt_holder[k].View()==emph::geo::U_VIEW){
                        if (uevt_holder[k].AxisIndex()==2 || uevt_holder[k].AxisIndex()==4) continue;
                        fUSSD_Profile[uevt_holder[k].Station()]->Fill(uevt_holder[k].U());
                        //std::cout<< "sensor index " <<uevt_holder[k].AxisIndex()<<"  value "<<uevt_holder[k].U()<<std::endl;
                    }
                }
                for (size_t k=0; k<vevt_holder.size(); ++k){
                    if(vevt_holder[k].View()==emph::geo::W_VIEW){
                        fVSSD_Profile[vevt_holder[k].Station()]->Fill(vevt_holder[k].V());
                    }
                }
                //Loop through stations and add a hit to a station if it has exactly one x/y hit
                for (size_t j=0; j<dim; ++j){
                    //Number of x and y hits in each station (required to be 1)
                    int xhits = std::count(xstations_hit.begin(),xstations_hit.end(),j);
                    int yhits = std::count(ystations_hit.begin(),ystations_hit.end(),j);
                    if (xhits == 1 && yhits==1){
                        auto xhit_finder = std::find(xstations_hit.begin(),xstations_hit.end(),j);
                        auto yhit_finder = std::find(ystations_hit.begin(),ystations_hit.end(),j);
                        int xhit_loc = std::distance(xstations_hit.begin(),xhit_finder);
                        int yhit_loc = std::distance(ystations_hit.begin(),yhit_finder);
                        //if (j==0) std::cout<<"xhit loc " <<xhit_loc<< "   yhitloc "<<yhit_loc<<"     x Station "<<xevt_holder[xhit_loc].Station()<<"     y Station "<<yevt_holder[yhit_loc].Station()<< "   event# = "<<xevt_holder[xhit_loc].Event()<< " yevt = "<<yevt_holder[yhit_loc].Event() <<std::endl;
                        x = xevt_holder[xhit_loc].X();
                        y = yevt_holder[yhit_loc].Y();
                        true_x = x - xevt_holder[xhit_loc].Shift();
                        true_y = y - yevt_holder[yhit_loc].Shift();

                        fSSD_Profile[j]->Fill(x,y);
                        fadj_SSD_Profile[j]->Fill(true_x,true_y);
                        fYSSD_Profile[j]->Fill(y);
                    }
                    
                }
                nevt = ssdvec[i].Event();

                //Reset event holder for next event			
                evt_holder.clear();
                xevt_holder.clear();
                yevt_holder.clear();
                uevt_holder.clear();
                vevt_holder.clear();
                xstations_hit.clear();
                ystations_hit.clear();
            }
            //Add axis index integer to object based on SSD geometry from spsindex map
            std::vector<int> sps = {ssdvec[i].Station(),ssdvec[i].Plane(),ssdvec[i].Sensor()};
            ssdvec[i].SetAxisIndex(spsindex[sps]);

            //Fill event holder with SSDAlign objects
            evt_holder.push_back(ssdvec[i]);
        }

        //Writing out shift constants to ConstBase/Alignment
        std::ofstream shift_file;
        shift_file.open ("SSDAlignment.dat");
        shift_file << "#Identifier x0 ... x21"<<std::endl;
        shift_file << "#Identifier y0 ... y21"<<std::endl;
        shift_file << "#Identifier r0 ... r21"<<std::endl;
        shift_file << "#Comment: #0 is the default alignment constant set."<<std::endl;

        shift_file << "0 X ";
        for (size_t i=0; i<x_shifts.size(); ++i){
            shift_file<<x_shifts[i]<<" ";
        }
        shift_file << std::endl;

        shift_file << "0 Y ";
        for (size_t i=0; i<y_shifts.size(); ++i){
            shift_file<<y_shifts[i]<<" ";
        }
        shift_file << std::endl;

        shift_file << "0 U ";
        for (size_t i=0; i<u_shifts.size(); ++i){
            shift_file<<u_shifts[i]<<" ";
        }
        shift_file << std::endl;

        shift_file << "0 V ";
        for (size_t i=0; i<v_shifts.size(); ++i){
            shift_file<<v_shifts[i]<<" ";
        }
        shift_file << std::endl;
        shift_file << "#Comment: #1 is the alignment constant set using Run xxxx..."<<std::endl;

        shift_file.close();
    }


    //......................................................................
    void SSDAlignment::analyze(const art::Event& evt)
    { 
        std::string fClusterLabel = "clust";

        art::Handle< std::vector<rb::SSDCluster> > clustH;
        auto geo = geom->Geo();

        try {
            evt.getByLabel(fClusterLabel, clustH);
            if (!clustH->empty()) {	
                for (size_t idx=0; idx < clustH->size(); ++idx) {
                    const rb::SSDCluster& clust = (*clustH)[idx];
                    //rb::SSDCluster clusterobj = rb::SSDCluster((*clustH)[idx]);
                    //clusters.push_back(clusterobj);
                    //std::cout<<clusterobj.AvgStrip()<<std::endl;
                    //
                    //Fill x,y,z of ssdalign object using detgeomap
                    rb::LineSegment ls;
                    //std::cout<<"strip = "<<clusters[i].AvgStrip()<<std::endl;
                    if (clust.AvgStrip() > 640){
                        std::cout<<"Skipping nonsense SSD Hit"<<std::endl;
                        continue;
                    }
                    dgm->Map()->SSDClusterToLineSegment(clust, ls);

                    int event = evt.event();

                    emph::al::SSDAlign hit(clust,event);
                    ssdvec.push_back(hit);
                    ssdvec.back().SetPos(ls);
                    //std::cout<<"xyz = "<<ssdvec.back().X()<<"  "<<ssdvec.back().Y()<<"  "<<ssdvec.back().Z()<<std::endl;
                }
            }
        }
        catch(...) {

        }
    }
} // end namespace emph

DEFINE_ART_MODULE(emph::SSDAlignment)
