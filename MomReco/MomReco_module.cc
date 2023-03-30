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
#include "TGraph2D.h"
#include "TROOT.h"
#include "TH2F.h"
#include "TF2.h"
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
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "RawData/TRB3RawDigit.h"
#include "SSDAlignment/SSDAlign.h"
#include "RecoBase/SSDCluster.h"

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
            void beginRun(art::Run& run);
            //      void endSubRun(art::SubRun const&);
            void endJob();
            void Calculate();
            void Analyze();

            struct SSDInfo{
                std::array<int,2> ss; //station sensor
                char axis; //x,y,u,v
                double shift;
                double z;
            };
            std::vector<SSDInfo> SSDTable;

            void line(double t, const double *p, double &x, double &y, double &z) {
                x = p[0] + p[1]*t;
                y = p[2] + p[3]*t;
                z = t;
            }
            struct SumDistance2{
                TGraph2D *fGraph;

                SumDistance2(TGraph2D *g) : fGraph(g) {}

                // calculate distance line-point
                double distance2(double x,double y,double z, const double *p) {
                    // distance line point is D= | (xp-x0) cross  ux |
                    // where ux is direction of line and x0 is a point in the line (like t = 0)
                    ROOT::Math::XYZVector xp(x,y,z);
                    ROOT::Math::XYZVector x0(p[0], p[2], 0. );
                    ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], 1. );
                    ROOT::Math::XYZVector u = (x1-x0).Unit();
                    double d2 = ((xp-x0).Cross(u)).Mag2();
                    return d2;
                }

                // implementation of the function to be minimized
                double operator() (const double *par) {
                    assert(fGraph != 0);
                    double * x = fGraph->GetX();
                    double * y = fGraph->GetY();
                    double * z = fGraph->GetZ();
                    int npoints = fGraph->GetN();
                    double sum = 0;
                    for (int i  = 0; i < npoints; ++i) {
                        double d = distance2(x[i],y[i],z[i],par);
                        sum += d;
                    }
                    return sum;
                }
            };

            std::vector<SSDInfo> CreateSSDTable(std::vector<double>x,std::vector<double>y,std::vector<double>u,std::vector<double> v);

        private:

            emph::cmap::ChannelMap* fChannelMap;
            emph::al::SSDAlign* fAlignmentFile;
            runhist::RunHistory* fRunHistory;
            emph::geo::Geometry *emgeo;
            int fEvtNum;
            int event;
            std::vector<double> x_shifts, y_shifts, u_shifts, v_shifts;
            std::vector<double> angles, angles_x;
            double theta;
            TH1D* hSSDmom;
            TF2* fit;
            TGraph2D* evt_line;
            TGraph2D* inc_line;
            art::ServiceHandle<art::TFileService> tfs;
            std::vector<const rb::SSDCluster*> clusters;
            const double pitch = 0.06;
            bool first_run=1;
            std::array<double,6> zpos;
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



    //......................................................................



    //......................................................................

    std::vector<MomReco::SSDInfo> MomReco::CreateSSDTable(std::vector<double>x,std::vector<double>y,std::vector<double>u,std::vector<double>v){
        std::vector<MomReco::SSDInfo> table;

        table.push_back({{0,0},'y',y[0], emgeo->GetSSDStation(0).Pos()[2]+ emgeo->GetSSDStation(0).GetSSD(0).Pos()[2]});
        table.push_back({{0,1},'x',x[0], emgeo->GetSSDStation(0).Pos()[2]+ emgeo->GetSSDStation(0).GetSSD(1).Pos()[2]});
        table.push_back({{1,0},'y',y[1], emgeo->GetSSDStation(1).Pos()[2]+ emgeo->GetSSDStation(1).GetSSD(0).Pos()[2]});
        table.push_back({{1,1},'x',x[1], emgeo->GetSSDStation(1).Pos()[2]+ emgeo->GetSSDStation(1).GetSSD(1).Pos()[2]});
        table.push_back({{2,1},'y',y[2], emgeo->GetSSDStation(2).Pos()[2]+ emgeo->GetSSDStation(2).GetSSD(1).Pos()[2]});
        table.push_back({{2,2},'x',x[2], emgeo->GetSSDStation(2).Pos()[2]+ emgeo->GetSSDStation(2).GetSSD(2).Pos()[2]});
        table.push_back({{3,1},'y',y[3], emgeo->GetSSDStation(3).Pos()[2]+ emgeo->GetSSDStation(3).GetSSD(1).Pos()[2]});
        table.push_back({{3,2},'x',x[3], emgeo->GetSSDStation(3).Pos()[2]+ emgeo->GetSSDStation(3).GetSSD(2).Pos()[2]});
        table.push_back({{2,0},'u',u[2], emgeo->GetSSDStation(2).Pos()[2]+ emgeo->GetSSDStation(2).GetSSD(0).Pos()[2]});
        table.push_back({{3,0},'u',u[3], emgeo->GetSSDStation(3).Pos()[2]+ emgeo->GetSSDStation(3).GetSSD(0).Pos()[2]});
        table.push_back({{4,0},'x',x[4], emgeo->GetSSDStation(4).Pos()[2]+ emgeo->GetSSDStation(4).GetSSD(0).Pos()[2]});
        table.push_back({{4,1},'x',x[5], emgeo->GetSSDStation(4).Pos()[2]+ emgeo->GetSSDStation(4).GetSSD(1).Pos()[2]});
        table.push_back({{4,2},'y',y[4], emgeo->GetSSDStation(4).Pos()[2]+ emgeo->GetSSDStation(4).GetSSD(2).Pos()[2]});
        table.push_back({{4,3},'y',y[5], emgeo->GetSSDStation(4).Pos()[2]+ emgeo->GetSSDStation(4).GetSSD(3).Pos()[2]});
        table.push_back({{4,4},'v',v[4], emgeo->GetSSDStation(4).Pos()[2]+ emgeo->GetSSDStation(4).GetSSD(4).Pos()[2]});
        table.push_back({{4,5},'v',v[5], emgeo->GetSSDStation(4).Pos()[2]+ emgeo->GetSSDStation(4).GetSSD(5).Pos()[2]});
        table.push_back({{5,0},'x',x[6], emgeo->GetSSDStation(5).Pos()[2]+ emgeo->GetSSDStation(5).GetSSD(0).Pos()[2]});
        table.push_back({{5,1},'x',x[7], emgeo->GetSSDStation(5).Pos()[2]+ emgeo->GetSSDStation(5).GetSSD(1).Pos()[2]});
        table.push_back({{5,2},'y',y[6], emgeo->GetSSDStation(5).Pos()[2]+ emgeo->GetSSDStation(5).GetSSD(2).Pos()[2]});
        table.push_back({{5,3},'y',y[7], emgeo->GetSSDStation(5).Pos()[2]+ emgeo->GetSSDStation(5).GetSSD(3).Pos()[2]});
        table.push_back({{5,4},'v',v[6], emgeo->GetSSDStation(5).Pos()[2]+ emgeo->GetSSDStation(5).GetSSD(4).Pos()[2]});
        table.push_back({{5,5},'v',v[7], emgeo->GetSSDStation(5).Pos()[2]+ emgeo->GetSSDStation(5).GetSSD(5).Pos()[2]});

        return table;
    }

    //......................................................................

    void MomReco::beginRun(art::Run& run)
    {
        std::cout<<"New Run"<<std::endl;

        if(first_run){
            std::cout<<"Now we do this"<<std::endl;
            //Clear shift vectors (useful when looping over multiple spils
            //x_shifts.clear();y_shifts.clear();u_shifts.clear();v_shifts.clear();
            // initialize channel map
            fChannelMap = new emph::cmap::ChannelMap();
            fRunHistory = new runhist::RunHistory(run.run());
            fChannelMap->LoadMap(fRunHistory->ChanFile());
            emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());

            // initialize alignment constants
            fAlignmentFile = new emph::al::SSDAlign();
            fAlignmentFile->LoadShifts(fRunHistory->AlignFile(),x_shifts,y_shifts,u_shifts,v_shifts);
            bool use_shifts = 1;
            if (!use_shifts){
                for (size_t i=0; i<x_shifts.size(); ++i){
                    x_shifts[i]=0;    
                    y_shifts[i]=0;    
                    u_shifts[i]=0;    
                    v_shifts[i]=0;    
                }
            }
            std::cout<<"*************************************************************"<<std::endl;
            std::cout<<"X shifts: ";
            for (size_t i=0; i<x_shifts.size(); ++i){
                std::cout<<x_shifts[i]<<" ";
            }
            std::cout<<std::endl<<"Y shifts: ";
            for (size_t i=0; i<y_shifts.size(); ++i){
                std::cout<<y_shifts[i]<<" ";
            }
            std::cout<<std::endl<<"U shifts: ";
            for (size_t i=0; i<u_shifts.size(); ++i){
                std::cout<<u_shifts[i]<<" ";
            }
            std::cout<<std::endl<<"V shifts: ";
            for (size_t i=0; i<v_shifts.size(); ++i){
                std::cout<<v_shifts[i]<<" ";
            }
            std::cout<<std::endl;

            SSDTable = CreateSSDTable(x_shifts,y_shifts,u_shifts,v_shifts);


            std::array<double,6> xzpos;
            std::array<double,6> yzpos;


            for (size_t i=0; i<SSDTable.size(); ++i){
                std::cout<<"Station: "<<SSDTable[i].ss[0]<<"   Sensor: "<<SSDTable[i].ss[1]<<"    zpos: "<<SSDTable[i].z<<'\n';
                if(SSDTable[i].axis == 'x') {
                    xzpos[SSDTable[i].ss[0]]=SSDTable[i].z;
                }
                if(SSDTable[i].axis == 'y') {
                    yzpos[SSDTable[i].ss[0]]=SSDTable[i].z;
                }
            }
            std::cout<<"xz pos:  ";
            for (size_t i=0; i<6; ++i){
                std::cout<<xzpos[i]<<"   ";
            }
            std::cout<<std::endl;
            std::cout<<"yz pos:  ";
            for (size_t i=0; i<6; ++i){
                std::cout<<yzpos[i]<<"   ";
            }
            std::cout<<std::endl;

            for (size_t i=0; i<6; ++i){
                zpos[i] = (xzpos[i]+yzpos[i])/2.;
            }
            std::cout<<"z pos:  ";
            for (size_t i=0; i<6; ++i){
                std::cout<<zpos[i]<<"   ";
            }
            std::cout<<std::endl;
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
        std::array<double,6> xpos={0,0,0,0,0,0};
        std::array<double,6> ypos={0,0,0,0,0,0};

        std::vector<int> xstation, ystation;
        //Looping over all SSD Clusters
        for (size_t i=0; i<clusters.size(); ++i){
            int index = -9999;
            double pos  = -9999;
            double strip = clusters[i]->AvgStrip();
            std::array<int,2> ss = {clusters[i]->Station(),clusters[i]->Sensor()};

            for (size_t j=0; j<SSDTable.size(); ++j){
                if(ss == SSDTable[j].ss){
                    index = j;
                    break;
                }
            }
            emph::geo::Detector sd = emgeo->GetSSDStation(SSDTable[index].ss[0]).GetSSD(SSDTable[index].ss[1]);
            if(SSDTable[index].axis=='x') {
                pos  = (strip*pitch-sd.Height()/2)*sin(sd.Rot())+sd.Pos()[0];  
                xpos[SSDTable[index].ss[0]] = pos - SSDTable[index].shift; //Adding in SSD shift
                xstation.push_back(SSDTable[index].ss[0]);
            }
            if(SSDTable[index].axis=='y') {
                pos  = (strip*pitch-sd.Height()/2)*cos(sd.Rot())+sd.Pos()[1]; 
                ypos[SSDTable[index].ss[0]] = pos - SSDTable[index].shift; //Adding in SSD shift
                ystation.push_back(SSDTable[index].ss[0]);
            }
            if(SSDTable[index].axis=='u') continue;
            if(SSDTable[index].axis=='v') continue;
        }

        const bool bx1 = (std::unique(xstation.begin(), xstation.end()) == xstation.end()); //Only 1 hit per station
        const bool bx2 = (xstation.size()==6);
        const bool by1 = (std::unique(ystation.begin(), ystation.end()) == ystation.end()); //Only 1 hit per station
        const bool by2 = (ystation.size()==6);

        //Loop over events with exactly 1 x and y hit in each directory
        if (bx1 && bx2 && by1 && by2){
            std::array<double,3> dir_i; //Initial direction
            std::array<double,3> dir_f; //Final direction

            //Fit line segment to first 4 SSD stations
            //TF2* fit = new TF2("fit","[0] + [1]*x + [2]*y",0,500);

            //Create line segment for last 2 SSD stations (before magnet)
            dir_f = {xpos[5]-xpos[4],ypos[5]-ypos[4],zpos[5]-zpos[4]};
            double norm_f = sqrt(pow(dir_f[0],2)+pow(dir_f[1],2)+pow(dir_f[2],2));
            for (size_t j=0; j<3; ++j){
                dir_f[j] = dir_f[j]/norm_f;
            }

            inc_line = new TGraph2D(4);
            //for (size_t j=0; j<4; ++j){
            //    inc_line->SetPoint(j,zpos[j],xpos[j],ypos[j]);
            //}
            //inc_line->Fit(fit,"Q0");
            //inc_line->Fit(fit);
            for (size_t j=0; j<4; ++j){
                inc_line->SetPoint(j,xpos[j],ypos[j],zpos[j]);
            }

            //Fit function to parametrized line (3D)
            ROOT::Fit::Fitter  fitter;
            SumDistance2 sdist(inc_line);
            ROOT::Math::Functor fcn(sdist,4);
            //set the function and the initial parameter values
            double pStart[4] = {0,0,0,0};
            fitter.SetFCN(fcn,pStart);
            for (int j = 0; j < 4; ++j) fitter.Config().ParSettings(j).SetStepSize(0.01);
            fitter.FitFCN(); //Fit function

            const ROOT::Fit::FitResult & result = fitter.Result();
            //Get fit parameters
            const double * parFit = result.GetParams();
            //std::cout<<"Chi2 = "<<result.Chi2()<<std::endl;
            dir_i = {parFit[1],parFit[3],1};
            double norm_i = sqrt(pow(dir_i[0],2)+pow(dir_i[1],2)+pow(dir_i[2],2));
            for (size_t j=0; j<3; ++j){
                dir_i[j]=dir_i[j]/norm_i;
            }

            if(fEvtNum<10){
                evt_line = tfs->makeAndRegister<TGraph2D>(Form("EvtDisp%i",fEvtNum),Form("Event Display %i",fEvtNum));

                dir_i = {parFit[1],parFit[3],1};
                double norm_i = sqrt(pow(dir_i[0],2)+pow(dir_i[1],2)+pow(dir_i[2],2));
                for (size_t j=0; j<3; ++j){
                    dir_i[j]=dir_i[j]/norm_i;
                }
            }
            theta = acos(dir_i[0]*dir_f[0] + dir_i[1]*dir_f[1] + dir_i[2]*dir_f[2]);

            //Angle between lines and y axis
            double theta_xi = acos(dir_i[0]);
            double theta_xf = acos(dir_f[0]);
            double theta_x = theta_xf-theta_xi; //y-angle between incoming and outgoing line segment

            //std::cout<<"Angle (degrees) = "<<theta*(180/M_PI)<<std::endl;
            angles.push_back(theta);
            angles_x.push_back(theta_x);

            delete inc_line;

        }


        xstation.clear(); ystation.clear();

        //Residual Method of Alignment

        //Linear Fit function
    }
    //......................................................................

    void MomReco::Analyze(){
        std::cout<<"Number of Events to analyze = "<<angles_x.size()<<std::endl;
        const double p_to_GeV = 1/(5.36e-19);
        const double e = 1.6e-19;
        hSSDmom = tfs->make<TH1D>("SSDMom","12 GeV/c pion: Reconstructed Momentum",160,-30,30);
        hSSDmom->GetXaxis()->SetTitle("p (GeV/c)");
        for (size_t i=0; i<10; ++i){
            std::cout<<"Angle (degrees) = "<<angles[i]*(180/M_PI)<<std::endl;
            std::cout<<"x-Angle (degrees) = "<<angles_x[i]*(180/M_PI)<<std::endl;
        }
        for (size_t i=0; i<angles_x.size(); ++i){
            //double p = (L*B)/(angles_x[i]);
            double p = p_to_GeV*e*(0.15*1.4)/(angles_x[i]);
            hSSDmom->Fill(p);
            if (i<100) std::cout<<"p = "<<p<<" GeV/c"<<std::endl;
        }

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
                    const emph::geo::SSDStation &st = emgeo->GetSSDStation(clust.Station());
                    const emph::geo::Detector &sd = st.GetSSD(clust.Sensor());
                    event = evt.event();
                    emph::al::SSDAlign hit(clust, sd, st, event);

                    clusters.push_back(&clust);
                }
                //If there are more clusters than sensors, skip event
                if (clusters.size()<23){
                    Calculate();
                }
                clusters.clear();
                fEvtNum++;
            }
            //Analyze();
        }
        catch(...) {

        }
    }
} // end namespace emph

DEFINE_ART_MODULE(emph::MomReco)
