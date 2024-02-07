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
#include "ChannelMap/service/ChannelMapService.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
#include "DetGeoMap/service/DetGeoMapService.h"
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

            std::vector<SSDInfo> CreateSSDTable(const std::vector<double>& x,const std::vector<double>& y,const std::vector<double>& u,const std::vector<double>& v);

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
            TF2* fit;
            TGraph2D* evt_line;
            TGraph2D* inc_line;
            TGraph2D* out_line;
            art::ServiceHandle<art::TFileService> tfs;
            std::vector<emph::al::SSDAlign> ssdhits;
            const double pitch = 0.06;
            bool first_run=1;
            double targetpos;
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
    void MomReco::beginRun(art::Run& run)
    {
        std::cout<<"New Run"<<std::endl;

        if(first_run){
            std::cout<<"Now we do this"<<std::endl;
            auto fChannelMap = cmap->CMap();
            auto emgeo = geom->Geo();
            nstations = emgeo->NSSDStations();
            targetpos = emgeo->TargetUSZPos();

            // initialize alignment constants
            emph::al::SSDAlign* fAlignmentFile = new emph::al::SSDAlign();
            fAlignmentFile->LoadShifts("/exp/emph/app/users/jmirabit/build/SSDAlignment.dat",x_shifts,y_shifts,u_shifts,v_shifts);
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

        //For each station check if there is one hit in x and y, if so add the avg x,y,z to xps,ypos,zpos
        for (size_t i=0; i<size_t(nstations); ++i){
            int xhits = 0;
            int yhits = 0;
            double x0=-999; double x1=-999; double y0=-999; double y1=-999; double z0=-999; double z1=-999;
            for (size_t j=0; j<ssdhits.size(); ++j){
                if(ssdhits[j].View()==emph::geo::X_VIEW && ssdhits[j].Station()==int(i)){
                    xhits+=1;
                    x0 = ssdhits[j].X();
                    y0 = ssdhits[j].Y();
                    z0 = ssdhits[j].Z();
                }
                if(ssdhits[j].View()==emph::geo::Y_VIEW && ssdhits[j].Station()==int(i)){
                    yhits+=1;
                    x1 = ssdhits[j].X();
                    y1 = ssdhits[j].Y();
                    z1 = ssdhits[j].Z();
                }
            }
            if (xhits==1 && yhits==1){
                xpos.push_back((x0+x1)/2);
                ypos.push_back((y0+y1)/2);
                zpos.push_back((z0+z1)/2);
            }
        }

        //Only fit event with at least 2 hits before and after target
        int nhitsbefore=0;
        int nhitsafter=0;
        for (size_t i=0; i<zpos.size(); ++i){
            if (zpos[i]<targetpos) nhitsbefore+=1;
            if (zpos[i]>targetpos) nhitsafter+=1;
        }


        //Loop over events with exactly 1 x and y hit in each directory
        if (nhitsbefore >1 &&  nhitsafter>1){
            std::array<double,3> dir_i; //Initial direction
            std::array<double,3> dir_f; //Final direction

            //Fit line segment to track before magnet
            inc_line = new TGraph2D();
            out_line = new TGraph2D();
            for (size_t j=0; j<zpos.size(); ++j){
                if(zpos[j]<targetpos) inc_line->SetPoint(j,xpos[j],ypos[j],zpos[j]);
                if(zpos[j]>targetpos) out_line->SetPoint(j,xpos[j],ypos[j],zpos[j]);
            }

            ROOT::Fit::Fitter  fitter;

            //Fit function to parametrized line (3D)
            SumDistance2 sdist_in(inc_line);
            ROOT::Math::Functor fcn_in(sdist_in,4);
            //set the function and the initial parameter values
            double pStart_in[4] = {0,0,0,0};
            fitter.SetFCN(fcn_in,pStart_in);
            for (int j = 0; j < 4; ++j) fitter.Config().ParSettings(j).SetStepSize(0.01);
            fitter.FitFCN(); //Fit function

            const ROOT::Fit::FitResult & result_in = fitter.Result();
            //Get fit parameters
            const double * parFit_in = result_in.GetParams();
            dir_i = {parFit_in[1],parFit_in[3],1};
            double norm_i = sqrt(pow(dir_i[0],2)+pow(dir_i[1],2)+pow(dir_i[2],2));
            for (size_t j=0; j<3; ++j){
                dir_i[j]=dir_i[j]/norm_i;
            }

            //And again for outgoing line
            SumDistance2 sdist_out(inc_line);
            ROOT::Math::Functor fcn_out(sdist_out,4);
            //set the function and the initial parameter values
            double pStart_out[4] = {0,0,0,0};
            fitter.SetFCN(fcn_out,pStart_out);
            for (int j = 0; j < 4; ++j) fitter.Config().ParSettings(j).SetStepSize(0.01);
            fitter.FitFCN(); //Fit function

            const ROOT::Fit::FitResult & result_out = fitter.Result();
            //Get fit parameters
            const double * parFit_out = result_out.GetParams();
            dir_f = {parFit_out[1],parFit_out[3],1};
            double norm_f = sqrt(pow(dir_f[0],2)+pow(dir_f[1],2)+pow(dir_f[2],2));
            for (size_t j=0; j<3; ++j){
                dir_f[j]=dir_f[j]/norm_f;
            }

            if(fEvtNum<50){
                inc_line = tfs->makeAndRegister<TGraph2D>(Form("EvtDisp%i",fEvtNum),Form("Event Display %i",fEvtNum));

                //dir_i = {parFit[1],parFit[3],1};
                //double norm_i = sqrt(pow(dir_i[0],2)+pow(dir_i[1],2)+pow(dir_i[2],2));
                //for (size_t j=0; j<3; ++j){
                //    dir_i[j]=dir_i[j]/norm_i;
                //}
            }
            theta = acos(dir_i[0]*dir_f[0] + dir_i[1]*dir_f[1] + dir_i[2]*dir_f[2]);

            //Angle between lines and y axis
            double theta_xi = acos(dir_i[0]);
            double theta_xf = acos(dir_f[0]);
            double theta_x = theta_xf-theta_xi; //y-angle between incoming and outgoing line segment

            //std::cout<<"Angle (degrees) = "<<theta*(180/M_PI)<<std::endl;
            if(xpos[5]>0){
                angles.push_back(theta);
                angles_x.push_back(theta_x);
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
            if (i<10) std::cout<<"p = "<<p<<" GeV/c"<<std::endl;
        }
        for (size_t i=0; i<avg_hit_pos.size();++i){
            double length = avg_hit_pos[i].size();
            double sum = 0;
            for (size_t j=0; j<length; ++j){
                sum += avg_hit_pos[i][j];
            }
            if(length !=0){
               sum = sum/length;
               std::cout<<"Avg hit pos of index " << i << " is "<< sum<< "    axis: "<<SSDTable[i].axis<<"  station: "<<SSDTable[i].ss[0]<<"  Shift: "<<SSDTable[i].shift<<"  # of hits: "<<length<<std::endl;
            }
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
                    rb::LineSegment ls;

                    if (clust.AvgStrip() > 640){
                        std::cout<<"Skipping nonsense SSD Hit"<<std::endl;
                        continue;
                    }

                    int event = evt.event();
                    emph::al::SSDAlign hit(clust, event);

                    dgm->Map()->SSDClusterToLineSegment(clust, ls);
                    hit.SetPos(ls);
                    if(hit.View()==emph::geo::X_VIEW) {
                        hit.SetShift(x_shifts[hit.AxisIndex()]);
                        hit.SetX(hit.X()-hit.Shift());
                    }
                    if(hit.View()==emph::geo::Y_VIEW) {
                        hit.SetShift(y_shifts[hit.AxisIndex()]);
                        hit.SetX(hit.Y()-hit.Shift());
                    }
                    //Skip u,w for now
                    if(hit.View()==emph::geo::U_VIEW) {
                        continue;
                    }
                    if(hit.View()==emph::geo::W_VIEW) {
                        continue;
                    }

                    ssdhits.push_back(hit);
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
