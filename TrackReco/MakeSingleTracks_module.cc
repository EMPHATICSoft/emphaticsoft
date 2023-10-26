////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct single-particle tracks
///       
/// \author  $Author: robert chirco $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraph2D.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RecoBase/SSDCluster.h"
#include "DetGeoMap/DetGeoMap.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/Track.h"
#include "Simulation/SSDHit.h"
#include "Simulation/Particle.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class MakeSingleTracks : public art::EDProducer {
  public:
    explicit MakeSingleTracks(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeSingleTracks() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

    void    MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls);
    double  dotProduct(double a[3], double b[3]); 
    void    ClosestApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double F[3], double l1[3], double l2[3]);
    void    findLine(std::vector<std::vector<double>> v, double lfirst[3], double llast[3]);
    double  findAngle(double p1[3], double p2[3], double p3[3], double p4[3]);
    void    findDist(std::vector<sim::SSDHit> sim, rb::LineSegment track, int seg);
    void    compareAngle(std::vector<sim::SSDHit> sim, rb::LineSegment track, double ang[2]);
    double  findAngleRes(std::vector<sim::SSDHit> sim_i, std::vector<sim::SSDHit> sim_f, double reco_angle);
    void    MakeHits(rb::SpacePoint sp);
    void    MakeLines(std::vector<rb::TrackSegment> &);

  private:
  
    TTree*      spacepoint;
    int run,subrun,event;
    int         fEvtNum;

    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> stripv;
    std::vector<rb::SpacePoint> spv;
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group;

    std::map<std::pair<int, int>, int> clustMap;

    bool        fMakePlots;
    int 	goodclust = 0;
    int         badclust = 0; 
    size_t      nPlanes;
    size_t      nStations;
    int st;

    //fcl parameters
    bool        fCheckClusters;     //Check clusters for event 

    emph::dgmap::DetGeoMap* fDetGeoMap;

    //histograms and graphs for hits
    TH2F* hSPDist0;
    TH2F* hSPDist1;
    TH2F* hSPDist2;
    TH2F* hSPDist3;
    TH2F* hSPDist4;
    TH2F* hSPDist5;
    TH2F* hSPDist6;
    TH2F* hSPDist7;

    TH2F* hDist_xz;
    TH2F* hDist_yz;

    TGraph* gRecoHits_xz[100];
    TGraph* gRecoHits_yz[100];
    TGraph* gSimHits_xz[100];
    TGraph* gSimHits_yz[100];
    TMultiGraph* gHits_xz[100];
    TMultiGraph* gHits_yz[100];

    TH1F* hRes_x[8];
    TH1F* hRes_y[8];

    //simulated hits (avg position)
    double xsim[8] = {999,999,999,999,999,999,999,999};
    double ysim[8] = {999,999,999,999,999,999,999,999};
    double zsim[8] = {999,999,999,999,999,999,999,999};

    //reconstructed hits
    double xreco[8] = {999,999,999,999,999,999,999,999};
    double yreco[8] = {999,999,999,999,999,999,999,999};
    double zreco[8] = {999,999,999,999,999,999,999,999};

    //all simulated hits
    std::vector<std::pair<double, int>> xsim_pair;
    std::vector<std::pair<double, int>> ysim_pair;
    std::vector<std::pair<double, int>> zsim_pair;

    //reco info for lines
    std::vector<std::vector<double>> v_reco;
    std::vector<std::vector<double>> sp01;
    std::vector<std::vector<double>> sp234;
    std::vector<std::vector<double>> sp567;
    std::vector<rb::SpacePoint> ts01;
    std::vector<rb::SpacePoint> ts234;
    std::vector<rb::SpacePoint> ts567;

    //truth position info
    std::vector<std::vector<double>> v_truth;
    double xtruth[20];
    double ytruth[20];
    double ztruth[20];

    //truth info for lines
    std::vector<sim::SSDHit> truthhit01;
    std::vector<sim::SSDHit> truthhit234;
    std::vector<sim::SSDHit> truthhit567;

    //truth info for station
    std::vector<sim::SSDHit> truthhit1;
    std::vector<sim::SSDHit> truthhit2;
    std::vector<sim::SSDHit> truthhit3;
    std::vector<sim::SSDHit> truthhit6;

    //line segment end points
    double l01x[2]; double l01y[2]; double l01z[2];
    double l234x[2]; double l234y[2]; double l234z[2];
    double l567x[2]; double l567y[2]; double l567z[2];
   
    //histograms and graphs for lines 
    TGraph* gTruthHits_xz[100]; TGraph* gTruthHits_yz[100];
    TGraph* gLine_xz01[100]; TGraph* gLine_yz01[100];
    TGraph* gLine_xz234[100]; TGraph* gLine_yz234[100];
    TGraph* gLine_xz567[100]; TGraph* gLine_yz567[100];
    TMultiGraph* gLines_xz[100];
    TMultiGraph* gLines_yz[100];

    TH1F* hPulls[8];

    TH1F* hScattering;
    TH1F* hBending;
    TH1F* hScatteringRes;
    TH1F* hBendingRes;

    TH1F* hChi2_seg1;
    TH1F* hChi2_seg2;
    TH1F* hChi2_seg3;
    //TH1F* hChi2[8];

    TH1F* hAngDiffx_seg1;
    TH1F* hAngDiffx_seg2;
    TH1F* hAngDiffx_seg3;
    TH1F* hAngDiffy_seg1;
    TH1F* hAngDiffy_seg2;
    TH1F* hAngDiffy_seg3;

    TH1F* hAngDiffx[8];
    TH1F* hAngDiffy[8];

  };

  //.......................................................................
  
  emph::MakeSingleTracks::MakeSingleTracks(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")),
    fDetGeoMap(NULL)
  {
    this->produces< std::vector<rb::SpacePoint> >();
    this->produces< std::vector<rb::TrackSegment> >();
    this->produces< std::vector<rb::Track> >();
  }

  //......................................................................
  
//  MakeSingleTracks::~MakeSingleTracks()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................

  // void MakeSingleTracks::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }

  //......................................................................
  
  void MakeSingleTracks::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();
  }

  //......................................................................
   
  void emph::MakeSingleTracks::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");
   
    hSPDist0 = tfs->make<TH2F>("hSPDist0","hSPDist0",1000,-50.,50.,1000,-50.,50.); 
    hSPDist1 = tfs->make<TH2F>("hSPDist1","hSPDist1",1000,-50.,50.,1000,-50.,50.);
    hSPDist2 = tfs->make<TH2F>("hSPDist2","hSPDist2",1000,-50.,50.,1000,-50.,50.);
    hSPDist3 = tfs->make<TH2F>("hSPDist3","hSPDist3",1000,-50.,50.,1000,-50.,50.);
    hSPDist4 = tfs->make<TH2F>("hSPDist4","hSPDist4",1000,-50.,50.,1000,-50.,50.);
    hSPDist5 = tfs->make<TH2F>("hSPDist5","hSPDist5",1000,-50.,50.,1000,-50.,50.);
    hSPDist6 = tfs->make<TH2F>("hSPDist6","hSPDist6",1000,-50.,50.,1000,-50.,50.);
    hSPDist7 = tfs->make<TH2F>("hSPDist7","hSPDist7",1000,-50.,50.,1000,-50.,50.);

    hDist_xz = tfs->make<TH2F>("hDist_xz","hDist_xz",2050,-50.,2000.,1000,-50.,50.); 
    hDist_yz = tfs->make<TH2F>("hDist_yz","hDist_yz",2050,-50.,2000.,1000,-50.,50.);

    char *hresx = new char[8];
    char *hresy = new char[8];

    //residuals
    for (int i=0; i<8; i++){
	sprintf(hresx,"hRes_x_%d",i);
        hRes_x[i] = tfs->make<TH1F>(hresx,hresx,100,-0.1,0.1);	
        sprintf(hresy,"hRes_y_%d",i);
        hRes_y[i] = tfs->make<TH1F>(hresy,hresy,100,-0.1,0.1);
    }

    //pulls
    char *hpulls = new char[9];
    for (int i=0; i<8; i++){
        sprintf(hpulls,"hPulls_%d",i);
        hPulls[i] = tfs->make<TH1F>(hpulls,hpulls,100,0.,0.2);
    }

    hScattering = tfs->make<TH1F>("hScattering","hScattering",500,0.,0.03);
    hScatteringRes = tfs->make<TH1F>("hScatteringRes","hScatteringRes",500,-0.005,0.005);
    hBending = tfs->make<TH1F>("hBending","hBending",500,0.,0.05); 
    hBendingRes = tfs->make<TH1F>("hBendingRes","hBendingRes",500,-0.005,0.005);

    hChi2_seg1 = tfs->make<TH1F>("hChi2_seg1","hChi2_seg1",100,0.,30.);
    hChi2_seg2 = tfs->make<TH1F>("hChi2_seg2","hChi2_seg2",100,0.,100.);
    hChi2_seg3 = tfs->make<TH1F>("hChi2_seg3","hChi2_seg3",100,0.,1000.);

    hAngDiffx_seg1 = tfs->make<TH1F>("hAngDiffx_seg1","hAngDiffx_seg1",100,-0.1,0.1);
    hAngDiffx_seg2 = tfs->make<TH1F>("hAngDiffx_seg2","hAngDiffx_seg2",100,-0.1,0.1);
    hAngDiffx_seg3 = tfs->make<TH1F>("hAngDiffx_seg3","hAngDiffx_seg3",100,-0.1,0.1);

    hAngDiffy_seg1 = tfs->make<TH1F>("hAngDiffy_seg1","hAngDiffy_seg1",100,-0.1,0.1);
    hAngDiffy_seg2 = tfs->make<TH1F>("hAngDiffy_seg2","hAngDiffy_seg2",100,-0.1,0.1);
    hAngDiffy_seg3 = tfs->make<TH1F>("hAngDiffy_seg3","hAngDiffy_seg3",100,-0.1,0.1);

    char *hangx = new char[11];
    char *hangy = new char[11];
    //char *chi2st = new char[8];
    for (int i=0; i<8; i++){
        sprintf(hangx,"hAngDiffx_%d",i);
        sprintf(hangy,"hAngDiffy_%d",i);
        hAngDiffx[i] = tfs->make<TH1F>(hangx,hangx,100,-0.005,0.005); //0.02
        hAngDiffy[i] = tfs->make<TH1F>(hangy,hangy,100,-0.005,0.005);

        //sprintf(chi2st,"hChi2_%d",i);
        //hChi2[i] = tfs->make<TH1F>(chi2st,chi2st,100,0,10);
    }

  }
 
  //......................................................................
  
  void emph::MakeSingleTracks::endJob()
  {
       std::cout<<"Number of clusters with one cluster per sensor: "<<goodclust<<std::endl;
       std::cout<<"Number of available clusters: "<<badclust+goodclust<<std::endl;
	
       int bmin_s = hScattering->FindFirstBinAbove();
       int bmax_s = hScattering->FindLastBinAbove();
       hScattering->GetXaxis()->SetRange(bmin_s,bmax_s);

       int bmin_sr = hScatteringRes->FindFirstBinAbove();
       int bmax_sr = hScatteringRes->FindLastBinAbove();
       hScatteringRes->GetXaxis()->SetRange(bmin_sr,bmax_sr);

       int bmin_b = hBending->FindFirstBinAbove();
       int bmax_b = hBending->FindLastBinAbove();
       hBending->GetXaxis()->SetRange(bmin_b,bmax_b);

       int bmin_br = hBendingRes->FindFirstBinAbove();
       int bmax_br = hBendingRes->FindLastBinAbove();
       hBendingRes->GetXaxis()->SetRange(bmin_br,bmax_br);
  }

  //......................................................................

  void emph::MakeSingleTracks::MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
  {
    if (!fDetGeoMap) fDetGeoMap = new emph::dgmap::DetGeoMap();

    fDetGeoMap->SSDClusterToLineSegment(cl, ls);
  }

  //......................................................................

  void emph::MakeSingleTracks::ClosestApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double F[3], double l1[3], double l2[3]){

        double r12 = (B(0) - A(0))*(B(0) - A(0)) + (B(1) - A(1))*(B(1) - A(1)) + (B(2) - A(2))*(B(2) - A(2));
	double r22 = (D(0) - C(0))*(D(0) - C(0)) + (D(1) - C(1))*(D(1) - C(1)) + (D(2) - C(2))*(D(2) - C(2));

	double d4321 = (D(0) - C(0))*(B(0) - A(0)) + (D(1) - C(1))*(B(1) - A(1)) + (D(2) - C(2))*(B(2) - A(2));
        double d3121 = (C(0) - A(0))*(B(0) - A(0)) + (C(1) - A(1))*(B(1) - A(1)) + (C(2) - A(2))*(B(2) - A(2));
	double d4331 = (D(0) - C(0))*(C(0) - A(0)) + (D(1) - C(1))*(C(1) - A(1)) + (D(2) - C(2))*(C(2) - A(2));

	double s = (-d4321*d4331 + d3121*r22) / (r12*r22 - d4321*d4321);
	double t = (d4321*d3121 - d4331*r12) / (r12*r22 - d4321*d4321);

	double L1[3]; double L2[3];
	if ( s >= 0 && s <= 1 && t >=0 && t <= 1){
	   //std::cout<<"Closest approach all good :)"<<std::endl;
	   for (int i=0; i<3; i++){
	       L1[i] = A(i) + s*(B(i) - A(i));
	       L2[i] = C(i) + t*(D(i) - C(i));	    
	       F[i] = (L1[i] + L2[i])/2.;
	       l1[i] = L1[i]; 
	       l2[i] = L2[i];
	   }
	//std::cout<<"CA CHECK (L1)...x: "<<L1[0]<<"   y: "<<L1[1]<<"   z: "<<L1[2]<<std::endl;
	//std::cout<<"CA CHECK (L2)...x: "<<L2[0]<<"   y: "<<L2[1]<<"   z: "<<L2[2]<<std::endl;
        }
	else{
	   //this should be very rare
	   std::cout<<"Closest approach calculation exception @ event "<<fEvtNum<<std::endl;
	   std::cout<<"A: ("<<A(0)<<","<<A(1)<<","<<A(2)<<")"<<std::endl;
	   std::cout<<"B: ("<<B(0)<<","<<B(1)<<","<<B(2)<<")"<<std::endl;
	   std::cout<<"C: ("<<C(0)<<","<<C(1)<<","<<C(2)<<")"<<std::endl;
	   std::cout<<"D: ("<<D(0)<<","<<D(1)<<","<<D(2)<<")"<<std::endl;
	   std::cout<<"How do line segments AB and CD look if you draw them in the beam view (i.e. the same plane)?"<<std::endl;
	   std::cout<<"And don't worry! A hit is still created, but the line segments (probably) come close to intersecting...but don't"<<std::endl;
	   //std::cout<<"s: "<<s<<std::endl;
	   //std::cout<<"t: "<<t<<std::endl;

	   std::clamp(s,0.,1.);
	   std::clamp(t,0.,1.);
 	
	   TVector3 l1p3;
	   TVector3 l1p4;
	   TVector3 l2p1;
	   TVector3 l2p2; 

           double d4121 = (D(0) - A(0))*(B(0) - A(0)) + (D(1) - A(1))*(B(1) - A(1)) + (D(2) - A(2))*(B(2) - A(2)); 
	   double d4332 = (D(0) - C(0))*(C(0) - B(0)) + (D(1) - C(1))*(C(1) - B(1)) + (D(2) - C(2))*(C(2) - B(2));

           double s_l1p3 = d3121/r12;
	   double s_l1p4 = d4121/r12;
	   double t_l2p1 = -d4331/r22;
	   double t_l2p2 = -d4332/r22;

	   double d_l1p3; 	
	   double d_l1p4;
	   double d_l2p1;
	   double d_l2p2;

	   for (int i=0; i<3; i++){	
	       l1p3(i) = A(i) + s_l1p3*(B(i) - A(i));
               l1p4(i) = A(i) + s_l1p4*(B(i) - A(i)); 
	       l2p1(i) = C(i) + t_l2p1*(D(i) - C(i));
	       l2p2(i) = C(i) + t_l2p2*(D(i) - C(i));

	   }   
	   d_l1p3 = C.Dot(l1p3);
	   d_l1p4 = D.Dot(l1p4);
	   d_l2p1 = A.Dot(l2p1);
	   d_l2p2 = B.Dot(l2p2);

	   if (d_l1p3 < d_l1p4){
	      for (int i=0; i<3; i++) { L1[i] = l1p3(i); l1[i] = L1[i]; }
	   }
	   else{
	      for (int i=0; i<3; i++) { L1[i] = l1p4(i); l1[i] = L1[i]; }
	   }
	   if (d_l2p1 < d_l2p2){
	      for (int i=0; i<3; i++) { L2[i] = l2p1(i); l2[i] = L2[i]; }
	   }
	   else{
	      for (int i=0; i<3; i++) { L2[i] = l2p2(i); l2[i] = L2[i]; }
	   }
	   for (int i=0; i<3; i++){
		F[i] = (L1[i] + L2[i])/2.;
           }
	}	
  }

  //......................................................................

  double emph::MakeSingleTracks::dotProduct(double a[3], double b[3]){
     double product = 0;
     for (int i=0; i<3; i++){
         product = product + a[i] * b[i];
     }
     return product;
  }

  //......................................................................
  
  void emph::MakeSingleTracks::findLine(std::vector<std::vector<double>> v, double lfirst[3], double llast[3])
  {
          //a line can be parameterized by L = A + tN
          //A is a point that lies on the line
          //N is a normalized direction vector
          //t is a real number

          int N = v.size();

          double mean[3] = {0., 0., 0.};
          double corr[3][3] = {0.};
          for(auto p : v)
          {
             //construct A which is the mean value of all points
             mean[0] += p[0];
             mean[1] += p[1];
             mean[2] += p[2];
             //construct correlation matrix
             for(int i = 0; i < 3; i++){
                for(int j = i; j < 3; j++){
                   corr[i][j] += p[i] * p[j];
                }
             }
          }
          for (int i = 0; i < 3; i++){
              mean[i] /= N;
              for(int j = i; j < 3; j++){
                 corr[i][j] /= N;
              }
          }
          //construct covariance matrix
          double cov_arr[] = { corr[0][0] - mean[0] * mean[0], corr[0][1] - mean[0] * mean[1], corr[0][2] - mean[0] * mean[2],
                               corr[0][1] - mean[0] * mean[1], corr[1][1] - mean[1] * mean[1], corr[1][2] - mean[1] * mean[2],
                               corr[0][2] - mean[0] * mean[2], corr[1][2] - mean[2] * mean[1], corr[2][2] - mean[2] * mean[2] };
          TMatrixDSym cov(3,cov_arr);
          TMatrixDSymEigen cov_e(cov);
          //find N by solving the eigenproblem for the covariance matrix
          TVectorD eig = cov_e.GetEigenValues();
          TMatrixD eigv = cov_e.GetEigenVectors();

          //take the eigenvector corresponding to the largest eigenvalue,
          //corresponding to the solution N
          double eig_max;
          eig_max = std::max(eig[0], std::max(eig[1], eig[2]));

          //std::cout<<"eig"<<std::endl;
          //eig.Print();
          //std::cout<<"eigmax: "<<eig_max<<std::endl;

          int el;
          for (int i = 0; i < 3; i++){
             if (eig[i] == eig_max){ el = i; break; }
          }

          double n[3];
          n[0] = eigv[0][el];
          n[1] = eigv[1][el];
          n[2] = eigv[2][el];

          //we can create any point L on the line by varying t

          //create endpoints at first and last station z-position
          //find t where z = v[0][2] then z = [v.size()-1][2]
          double tfirst; double tlast;

          tfirst = (v[0][2] - mean[2])/n[2];
          tlast  = (v[v.size()-1][2] - mean[2])/n[2];

          lfirst[0] = mean[0] + tfirst*n[0];
          lfirst[1] = mean[1] + tfirst*n[1];
          lfirst[2] = v[0][2];

          llast[0] = mean[0] + tlast*n[0];
          llast[1] = mean[1] + tlast*n[1];
          llast[2] = v[v.size()-1][2];

  }

  //......................................................................

  void emph::MakeSingleTracks::findDist(std::vector<sim::SSDHit> sim, rb::LineSegment track, int seg){
    //calculate the distance between each point (SSD truth hits) in vector v and the track

    //put track into vector form
    double a = track.X1()[0] - track.X0()[0];
    double b = track.X1()[1] - track.X0()[1];
    double c = track.X1()[2] - track.X0()[2];

    TVector3 m(a,b,c);

    double chi2 = 0.;

    //distance between any point on the line A and P
    //we take track.X0() as a point A
    for (auto i : sim) {
        TVector3 p;
        p(0) = i.GetX(); p(1) = i.GetY(); p(2) = i.GetZ();

        TVector3 ap;
        for (int j=0; j<3; j++){
            double mean = (track.X0()[j]+track.X1()[j])/2.;
            ap(j) = mean - p[j];
        }
        //calculate cross product
        TVector3 vtemp = ap.Cross(m);

        //calculate orthogonal distance
        double d = vtemp.Mag() / m.Mag();

        //std::cout<<"Distance from point to LOBF for seg "<<seg<<": "<<d<<std::endl;

        hPulls[i.GetStation()]->Fill(d);

        //calculate chi2
        double err = 0.06/std::sqrt(12.);
        //double chi2perstation = std::pow((d/err),2);
        chi2 += std::pow((d/err),2);
        //hChi2[i.GetStation()]->Fill(chi2perstation);
    }

    if (seg==1) hChi2_seg1->Fill(chi2);
    if (seg==2) hChi2_seg2->Fill(chi2);
    if (seg==3) hChi2_seg3->Fill(chi2);
  }

  //......................................................................

  double emph::MakeSingleTracks::findAngle(double p1[3], double p2[3], double p3[3], double p4[3])
  {
     //create line in cartesian coordinates
     double a1 = p2[0] - p1[0]; double a2 = p4[0] - p3[0];
     double b1 = p2[1] - p1[1]; double b2 = p4[1] - p3[1];
     double c1 = p2[2] - p1[2]; double c2 = p4[2] - p3[2];

     TVector3 m1(a1,b1,c1);
     TVector3 m2(a2,b2,c2);

     double theta_rad = m1.Angle(m2);

     return theta_rad;
  }

  //......................................................................

  void emph::MakeSingleTracks::compareAngle(std::vector<sim::SSDHit> sim, rb::LineSegment track, double ang[2]){
     double thetax = 0.;
     double thetay = 0.;

     double thetax_re = TMath::ATan2( ( track.X1()[0] - track.X0()[0] ) , ( track.X1()[2] - track.X0()[2] ) );
     double thetay_re = TMath::ATan2( ( track.X1()[1] - track.X0()[1] ) , ( track.X1()[2] - track.X0()[2] ) );

     for (auto i : sim){
         double thetax_i = TMath::ATan2(i.GetPx(),i.GetPz());
         double thetay_i = TMath::ATan2(i.GetPy(),i.GetPz());
         thetax += thetax_i;
         thetay += thetay_i;

         hAngDiffx[i.GetStation()]->Fill(thetax_re - thetax_i);
         hAngDiffy[i.GetStation()]->Fill(thetay_re - thetay_i);
     }
     thetax /= sim.size();
     thetay /= sim.size();

     //reconstructed - avg simulated angle
     double angdiffx = thetax_re - thetax;
     double angdiffy = thetay_re - thetay;

     ang[0] = angdiffx;
     ang[1] = angdiffy;
  }

  //......................................................................

  double emph::MakeSingleTracks::findAngleRes(std::vector<sim::SSDHit> sim_i, std::vector<sim::SSDHit> sim_f, double reco_angle){

     double p_ix=0.; double p_iy=0.; double p_iz=0.;
     double p_fx=0.; double p_fy=0.; double p_fz=0.;

     for (auto i : sim_i){
         p_ix += i.GetPx();
         p_iy += i.GetPy();
         p_iz += i.GetPz();
     }
     p_ix /= sim_i.size();
     p_iy /= sim_i.size();
     p_iz /= sim_i.size();

     for (auto f : sim_f){
         p_fx += f.GetPx();
         p_fy += f.GetPy();
         p_fz += f.GetPz();
     }
     p_fx /= sim_f.size();
     p_fy /= sim_f.size();
     p_fz /= sim_f.size();

     TVector3 p_i(p_ix,p_iy,p_iz);
     TVector3 p_f(p_fx,p_fy,p_fz);

     double theta_rad = p_i.Angle(p_f);

     double res = reco_angle - theta_rad;

     return res;
  }

  //......................................................................

  void emph::MakeSingleTracks::MakeHits(rb::SpacePoint sp){

     art::ServiceHandle<emph::geo::GeometryService> geo;
     auto emgeo = geo->Geo();

     for (size_t i=0; i<nStations; i++){
         int nssds = emgeo->GetSSDStation(i)->NPlanes();
         for (size_t j=0; j<nPlanes; j++){
             if (nssds == 2){ //station 0,1,4,7
                for (size_t k=0; k<ls_group[i][j].size(); k++){
                    st = cl_group[i][j][k]->Station();
                    for (size_t l=0; l<ls_group[i][j+1].size(); l++){
			TVector3 fA( ls_group[i][j][k]->X0()[0], ls_group[i][j][k]->X0()[1], ls_group[i][j][k]->X0()[2] );
			TVector3 fB( ls_group[i][j][k]->X1()[0], ls_group[i][j][k]->X1()[1], ls_group[i][j][k]->X1()[2] );
		        TVector3 fC( ls_group[i][j+1][l]->X0()[0], ls_group[i][j+1][l]->X0()[1], ls_group[i][j+1][l]->X0()[2] );
			TVector3 fD( ls_group[i][j+1][l]->X1()[0], ls_group[i][j+1][l]->X1()[1], ls_group[i][j+1][l]->X1()[2] );

                        double x[3];
                        double l1[3]; double l2[3];
                        ClosestApproach(fA,fB,fC,fD,x,l1,l2);

                        //set SpacePoint object
                        sp.SetX(x);

                        //check stations
                        if (cl_group[i][j+1][l]->Station() == st){}
                        else std::cout<<"XY: Stations do not match..."<<std::endl;

                        sp.SetStation(st);
                        spv.push_back(sp);

                        if (st==0) hSPDist0->Fill(x[0],x[1]);
                        if (st==1) hSPDist1->Fill(x[0],x[1]);
                        if (st==4) hSPDist4->Fill(x[0],x[1]);
                        if (st==7) hSPDist7->Fill(x[0],x[1]);
                        hDist_xz->Fill(x[2],x[0]);
                        hDist_yz->Fill(x[2],x[1]);
       
                        //check for plots
                        if (fEvtNum < 10000){
                           xreco[st] = x[0];
                           yreco[st] = x[1];
                           zreco[st] = x[2];
                        }
                    }
                }
             }
             if (nssds == 3){ //station 2,3,5,6
                for (size_t k=0; k<ls_group[i][j].size(); k++){
                    st = cl_group[i][j][k]->Station();
                    for (size_t l=0; l<ls_group[i][j+1].size(); l++){
                        for (size_t m=0; m<ls_group[i][j+2].size(); m++){
			    TVector3 fA01( ls_group[i][j][k]->X0()[0], ls_group[i][j][k]->X0()[1], ls_group[i][j][k]->X0()[2] );
			    TVector3 fB01( ls_group[i][j][k]->X1()[0], ls_group[i][j][k]->X1()[1], ls_group[i][j][k]->X1()[2] );
			    TVector3 fC01( ls_group[i][j+1][l]->X0()[0], ls_group[i][j+1][l]->X0()[1], ls_group[i][j+1][l]->X0()[2] );
			    TVector3 fD01( ls_group[i][j+1][l]->X1()[0], ls_group[i][j+1][l]->X1()[1], ls_group[i][j+1][l]->X1()[2] );

                            double x01[3];
                            double l1_01[3]; double l2_01[3];
                            ClosestApproach(fA01,fB01,fC01,fD01,x01,l1_01,l2_01);

			    TVector3 fA02( ls_group[i][j][k]->X0()[0], ls_group[i][j][k]->X0()[1], ls_group[i][j][k]->X0()[2] );
			    TVector3 fB02( ls_group[i][j][k]->X1()[0], ls_group[i][j][k]->X1()[1], ls_group[i][j][k]->X1()[2] );
			    TVector3 fC02( ls_group[i][j+2][m]->X0()[0], ls_group[i][j+2][m]->X0()[1], ls_group[i][j+2][m]->X0()[2] );
			    TVector3 fD02( ls_group[i][j+2][m]->X1()[0], ls_group[i][j+2][m]->X1()[1], ls_group[i][j+2][m]->X1()[2] );

                            double x02[3];
                            double l1_02[3]; double l2_02[3];
                            ClosestApproach(fA02,fB02,fC02,fD02,x02,l1_02,l2_02);

			    TVector3 fA12( ls_group[i][j+1][l]->X0()[0], ls_group[i][j+1][l]->X0()[1], ls_group[i][j+1][l]->X0()[2] );
			    TVector3 fB12( ls_group[i][j+1][l]->X1()[0], ls_group[i][j+1][l]->X1()[1], ls_group[i][j+1][l]->X1()[2] );
			    TVector3 fC12( ls_group[i][j+2][m]->X0()[0], ls_group[i][j+2][m]->X0()[1], ls_group[i][j+2][m]->X0()[2] );
			    TVector3 fD12( ls_group[i][j+2][m]->X1()[0], ls_group[i][j+2][m]->X1()[1], ls_group[i][j+2][m]->X1()[2] );

                            double x12[3];
                            double l1_12[3]; double l2_12[3];
                            ClosestApproach(fA12,fB12,fC12,fD12,x12,l1_12,l2_12);
        
                            //average of three points (center of mass)
                            double x[3];
                            for (int i=0; i<3; i++){
                                x[i] = (x01[i]+x02[i]+x12[i])/3.;
                            }

                            //set SpacePoint object
                            sp.SetX(x);

			    //check stations
                            if (cl_group[i][j+1][l]->Station() == st && cl_group[i][j+2][m]->Station() == st){}
                            else std::cout<<"XYU: Stations do not match..."<<std::endl;

                            sp.SetStation(st);

                            spv.push_back(sp);

			    if (st==2) hSPDist2->Fill(x[0],x[1]);
                            if (st==3) hSPDist3->Fill(x[0],x[1]);
                            if (st==5) hSPDist5->Fill(x[0],x[1]);
                            if (st==6) hSPDist6->Fill(x[0],x[1]);
                            hDist_xz->Fill(x[2],x[0]);
                            hDist_yz->Fill(x[2],x[1]);

                            //check for plots
                            if (fEvtNum < 10000){
                               xreco[st] = x[0];
                               yreco[st] = x[1];
                               zreco[st] = x[2];
                            }
                        }
                    }
                }
             }
         }
     }
  }
 
  //......................................................................

  void emph::MakeSingleTracks::MakeLines(std::vector<rb::TrackSegment>& tsv) {
         //segment 01 -> don't need to fit anything, just connect two points
         double lfirst01[3]; double llast01[3];
         //findLine(sp01,lfirst01,llast01);
         lfirst01[0] = sp01[0][0];
         lfirst01[1] = sp01[0][1];
         lfirst01[2] = sp01[0][2];

         llast01[0] = sp01[1][0];
         llast01[1] = sp01[1][1];
         llast01[2] = sp01[1][2];
	 
         //segment 234
         double lfirst234[3]; double llast234[3];
         findLine(sp234,lfirst234,llast234);

         //segment 567
         double lfirst567[3]; double llast567[3];
         findLine(sp567,lfirst567,llast567);

	 
         //find angles
         double scattering = findAngle(lfirst01,llast01,lfirst234,llast234);
         double bending = findAngle(lfirst234,llast234,lfirst567,llast567);

         hScattering->Fill(scattering);
         hBending->Fill(bending);

         double bres = findAngleRes(truthhit3,truthhit6,bending);
         double sres = findAngleRes(truthhit1,truthhit2,scattering);
         hBendingRes->Fill(bres);
         hScatteringRes->Fill(sres);

         //assign to track vector
         rb::LineSegment track1 = rb::LineSegment(lfirst01,llast01);
         rb::LineSegment track2 = rb::LineSegment(lfirst234,llast234);
         rb::LineSegment track3 = rb::LineSegment(lfirst567,llast567);
 
	 // create rb::TrackSegments and insert them into the vector
	 rb::TrackSegment ts1 = rb::TrackSegment();
	 for (auto p : spv)
	   if (p.Station() == 0 || p.Station() == 1)
	     ts1.Add(p);
	 ts1.SetVtx(lfirst01);
	 double p[3];
	 double dx = llast01[0]-lfirst01[0];
	 double dy = llast01[1]-lfirst01[1];
	 double dz = llast01[2]-lfirst01[2];	 
	 double pmag = sqrt(dx*dx + dy*dy + dz*dz);
	 p[0] = dx/pmag;
	 p[1] = dy/pmag;
	 p[2] = dz/pmag;
	 ts1.SetP(p);
	 tsv.push_back(ts1);

	 rb::TrackSegment ts2 = rb::TrackSegment();
	 for (auto p : spv)
	   if (p.Station() == 2 || p.Station() == 3 || p.Station() == 4)
	     ts2.Add(p);
	 ts2.SetVtx(lfirst234);
	 dx = llast234[0]-lfirst234[0];
	 dy = llast234[1]-lfirst234[1];
	 dz = llast234[2]-lfirst234[2];	 
	 pmag = sqrt(dx*dx + dy*dy + dz*dz);
	 p[0] = dx/pmag;
	 p[1] = dy/pmag;
	 p[2] = dz/pmag;
	 ts2.SetP(p);
	 tsv.push_back(ts2);

	 rb::TrackSegment ts3 = rb::TrackSegment();
	 for (auto p : spv)
	   if (p.Station() == 5 || p.Station() == 6 || p.Station() == 7)
	     ts3.Add(p);
	 ts3.SetVtx(lfirst567);
	 dx = llast567[0]-lfirst567[0];
	 dy = llast567[1]-lfirst567[1];
	 dz = llast567[2]-lfirst567[2];	 
	 pmag = sqrt(dx*dx + dy*dy + dz*dz);
	 p[0] = dx/pmag;
	 p[1] = dy/pmag;
	 p[2] = dz/pmag;
	 ts3.SetP(p);
	 tsv.push_back(ts3);

         //reorganize for plotting
         l01x[0] = lfirst01[0]; l01x[1] = llast01[0];
         l01y[0] = lfirst01[1]; l01y[1] = llast01[1];
         l01z[0] = lfirst01[2]; l01z[1] = llast01[2];

         l234x[0] = lfirst234[0]; l234x[1] = llast234[0];
         l234y[0] = lfirst234[1]; l234y[1] = llast234[1];
         l234z[0] = lfirst234[2]; l234z[1] = llast234[2];

         l567x[0] = lfirst567[0]; l567x[1] = llast567[0];
         l567y[0] = lfirst567[1]; l567y[1] = llast567[1];
         l567z[0] = lfirst567[2]; l567z[1] = llast567[2];

         findDist(truthhit01,track1,1);
         findDist(truthhit234,track2,2);
         findDist(truthhit567,track3,3);

         //angular diffs
         double ts01[2]; double ts234[2]; double ts567[2];
         compareAngle(truthhit01,track1,ts01);
         compareAngle(truthhit234,track2,ts234);
         compareAngle(truthhit567,track3,ts567);

         hAngDiffx_seg1->Fill(ts01[0]);
         hAngDiffy_seg1->Fill(ts01[1]);
         hAngDiffx_seg2->Fill(ts234[0]);
         hAngDiffy_seg2->Fill(ts234[1]);
         hAngDiffx_seg3->Fill(ts567[0]);
         hAngDiffy_seg3->Fill(ts567[1]);
  }

  //......................................................................

  void emph::MakeSingleTracks::produce(art::Event& evt)
  {

    std::unique_ptr< std::vector<rb::SpacePoint> > spacepointv(new std::vector<rb::SpacePoint>);
    std::unique_ptr< std::vector<rb::TrackSegment> > tracksegmentv(new std::vector<rb::TrackSegment>);
    std::unique_ptr< std::vector<rb::Track> > trackv(new std::vector<rb::Track>);
    std::vector<rb::TrackSegment> tsv;

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    //debug
    //if(fEvtNum==1080) fMakePlots = true;
    //else fMakePlots = false;

    fMakePlots = true;

    if(fMakePlots){ 

    if (fCheckClusters){
       auto hasclusters = evt.getHandle<rb::SSDCluster>("clust");
       if (!hasclusters){
       mf::LogError("HasSSDClusters")<<"No clusters found in event but CheckClusters set to true!";
       abort();
       }
    }

    std::string fClusterLabel = "clust";
    art::Handle< std::vector<rb::SSDCluster> > clustH;

    std::string fG4Label = "geantgen";
    art::Handle< std::vector<sim::SSDHit> > ssdHitH;

    try {
      evt.getByLabel(fG4Label,ssdHitH);
    } 
    catch(...) {
      std::cout << "WARNING: No SSDHits found!" << std::endl;
    }

    bool goodEvent = false;
    bool goodHit = false;

    try {
      evt.getByLabel(fClusterLabel, clustH);
      if (!clustH->empty()){
         for (size_t idx=0; idx < clustH->size(); ++idx) {
              const rb::SSDCluster& clust = (*clustH)[idx];
	      ++clustMap[std::pair<int,int>(clust.Station(),clust.Plane())];
	      clusters.push_back(&clust); 
	  }

          //ONE CLUSTER PER PLANE
          //If there are more clusters than sensors, skip event
	  if (clusters.size()==nPlanes){
	     for (auto i : clustMap){
		 if (i.second != 1){goodEvent = false; break;} 
	         else goodEvent = true;
	     }
	     if (goodEvent==true) {goodclust++;} 
	     else {badclust++;}
          }
	  else badclust++;
	  
          cl_group.resize(nStations);
          ls_group.resize(nStations);

	  for (size_t i=0; i<nStations; i++){
	      cl_group[i].resize(nPlanes);
              ls_group[i].resize(nPlanes);
	  }

          for (size_t i=0; i<clusters.size(); i++){
	      int plane = clusters[i]->Plane();
	      int station = clusters[i]->Station();	 
 
	      //group clusters according to plane
	      //within each station, do every combination
	      cl_group[station][plane].push_back(clusters[i]);
	  }
	 
	  //ANY CLUSTER
          /*bool goodEvent = true;
          for (size_t i=0; i<nPlanes; i++){
              if (cl_group[i].size() == 0){ goodEvent = false; break; }
          }
          */

	  rb::LineSegment strip;
          if (goodEvent == true && clusters.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
		stripv.push_back(strip);
		MakeSegment(*clusters[i],stripv[i]);
		
	     }
          }

	  rb::SpacePoint sp;
	  if (goodEvent == true && stripv.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
                 int plane = clusters[i]->Plane();
		 int station = clusters[i]->Station();
		 ls_group[station][plane].push_back(&stripv[i]);
             }
	     //make reconstructed hits
	     MakeHits(sp);
	     spacepointv->push_back(sp);
	  }

          ls_group.clear();
          cl_group.clear();
	  stripv.clear();
	  clusters.clear();
          clustMap.clear();

          //get truth info
          if (goodEvent && spv.size() > 0 && ssdHitH->size() == nPlanes){
             for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
                 const sim::SSDHit& ssdhit = (*ssdHitH)[idx];

	       	 //for hits comparison	
	         xsim_pair.push_back(std::pair<double, int>(ssdhit.GetX(),ssdhit.GetStation()));
                 ysim_pair.push_back(std::pair<double, int>(ssdhit.GetY(),ssdhit.GetStation()));
                 zsim_pair.push_back(std::pair<double, int>(ssdhit.GetZ(),ssdhit.GetStation()));

	         //for lines comparison
	         std::vector<double> x = {ssdhit.GetX(),ssdhit.GetY(),ssdhit.GetZ()};
                 v_truth.push_back(x);

	         //for tracks
                 if (ssdhit.GetStation() == 0 || ssdhit.GetStation() == 1) truthhit01.push_back(ssdhit);
                 if (ssdhit.GetStation() == 2 || ssdhit.GetStation() == 3 || ssdhit.GetStation() == 4) truthhit234.push_back(ssdhit);
                 if (ssdhit.GetStation() == 5 || ssdhit.GetStation() == 6 || ssdhit.GetStation() == 7) truthhit567.push_back(ssdhit);

		 //for bending and scattering angles
                 if (ssdhit.GetStation() == 1) truthhit1.push_back(ssdhit);
                 if (ssdhit.GetStation() == 2) truthhit2.push_back(ssdhit);
                 if (ssdhit.GetStation() == 3) truthhit3.push_back(ssdhit);
                 if (ssdhit.GetStation() == 6) truthhit6.push_back(ssdhit);
             }
             goodHit = true;

       	     for (size_t i=0; i<spv.size(); i++){
                 std::vector<double> x = {spv[i].Pos()[0],spv[i].Pos()[1],spv[i].Pos()[2]};	
                 v_reco.push_back(x);

	         //reconstructed hits
                 if (spv[i].Station() == 0 || spv[i].Station() == 1) sp01.push_back(x);
                 if (spv[i].Station() == 2 || spv[i].Station() == 3 || spv[i].Station() == 4) sp234.push_back(x);
                 if (spv[i].Station() == 5 || spv[i].Station() == 6 || spv[i].Station() == 7) sp567.push_back(x);
             }

             for (size_t i=0; i<v_truth.size(); i++){
                 xtruth[i] = v_truth[i][0];
                 ytruth[i] = v_truth[i][1];
                 ztruth[i] = v_truth[i][2];
             }

	     //form lines and fill plots
	     MakeLines(tsv);
	     for (auto ts : tsv)
	       tracksegmentv->push_back(ts);
	     
	  }

          spv.clear();
          v_reco.clear();
          v_truth.clear();
          sp01.clear();
          sp234.clear();
          sp567.clear();

          truthhit01.clear();
          truthhit234.clear();
          truthhit567.clear();

          truthhit1.clear();
          truthhit2.clear();
          truthhit3.clear();
          truthhit6.clear();

	  //for hits comparison
	  //where xsim, ysim, and zsim are avg truth hit positions
          if (goodEvent == true){
             for (int j=0; j<8; j++){
                 double xsum = 0.; double ysum = 0.; double zsum = 0.;
                 int xc=0; int yc=0; int zc=0;

	         for (size_t i=0; i<xsim_pair.size(); i++){
	             if (j == xsim_pair[i].second) {
	                xsum += xsim_pair[i].first;
                        ysum += ysim_pair[i].first;
                        zsum += zsim_pair[i].first;
		        xc++; yc++; zc++;
	             }
	         }
                 xsim[j] = xsum/(double)xc;
                 ysim[j] = ysum/(double)yc;
                 zsim[j] = zsum/(double)zc;
              }
          }
      } //clust not empty

      if (goodEvent && goodHit){
         if (fEvtNum < 100){
            art::ServiceHandle<art::TFileService> tfs;
            char *Ghevt = new char[16];
   	    char *Gxzsim = new char[12];
	    char *Gyzsim = new char[12];

            if ( std::find(std::begin(xreco), std::end(xreco), 999) == std::end(xreco)){
               sprintf(Ghevt,"gRecoHits_xz_e%d",fEvtNum);
	       sprintf(Gxzsim,"gHits_xz_e%d",fEvtNum);		
	       size_t graphsize = 8; //xsim.size();

               gHits_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gxzsim,Gxzsim);

	       //get array by the address of the first element of the vector
	       gSimHits_xz[fEvtNum] = tfs->make<TGraph>(graphsize,&zsim[0],&xsim[0]);
               gRecoHits_xz[fEvtNum] = tfs->make<TGraph>(8,zreco,xreco);			

               gHits_xz[fEvtNum]->Add(gRecoHits_xz[fEvtNum]);
               gHits_xz[fEvtNum]->Add(gSimHits_xz[fEvtNum]);

	       gHits_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
               gHits_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
               gSimHits_xz[fEvtNum]->SetMarkerStyle(3);
               gSimHits_xz[fEvtNum]->SetMarkerSize(2);
	       gSimHits_xz[fEvtNum]->SetMarkerColor(kRed);
	       gSimHits_xz[fEvtNum]->SetLineColor(kRed);

	       gRecoHits_xz[fEvtNum]->SetMarkerStyle(3);
               gRecoHits_xz[fEvtNum]->SetMarkerSize(2);
	    }
            if ( std::find(std::begin(yreco), std::end(yreco), 999) == std::end(yreco)){
	       sprintf(Gyzsim,"gHits_yz_e%d",fEvtNum);
	       sprintf(Ghevt,"gRecoHits_yz_e%d",fEvtNum);
	       size_t graphsize = 8; //ysim.size();

	       gHits_yz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gyzsim,Gyzsim);
	       gSimHits_yz[fEvtNum] = tfs->make<TGraph>(graphsize,&zsim[0],&ysim[0]);
               gRecoHits_yz[fEvtNum] = tfs->make<TGraph>(8,zreco,yreco);
               gHits_yz[fEvtNum]->Add(gRecoHits_yz[fEvtNum]);
               gHits_yz[fEvtNum]->Add(gSimHits_yz[fEvtNum]);

	       gHits_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
               gHits_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
               gSimHits_yz[fEvtNum]->SetMarkerStyle(3);
               gSimHits_yz[fEvtNum]->SetMarkerSize(2);
               gSimHits_yz[fEvtNum]->SetMarkerColor(kRed);
	       gSimHits_yz[fEvtNum]->SetLineColor(kRed);

	       gRecoHits_yz[fEvtNum]->SetMarkerStyle(3);
               gRecoHits_yz[fEvtNum]->SetMarkerSize(2);
	    }	
         }
      }

      if (goodEvent && goodHit){
         if (fEvtNum < 100){
            art::ServiceHandle<art::TFileService> tfs;
            char *Gxz = new char[13];
            char *Gyz = new char[13];

            sprintf(Gxz,"gLines_xz_e%d",fEvtNum);
            sprintf(Gyz,"gLines_yz_e%d",fEvtNum);

            size_t graphsize = 2;
            gLines_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gxz,Gxz);
            gLines_yz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gyz,Gyz);

            gLine_xz01[fEvtNum] = tfs->make<TGraph>(graphsize,&l01z[0],&l01x[0]);
            gLine_xz234[fEvtNum] = tfs->make<TGraph>(graphsize,&l234z[0],&l234x[0]);
            gLine_xz567[fEvtNum] = tfs->make<TGraph>(graphsize,&l567z[0],&l567x[0]);
            gLine_yz01[fEvtNum] = tfs->make<TGraph>(graphsize,&l01z[0],&l01y[0]);
            gLine_yz234[fEvtNum] = tfs->make<TGraph>(graphsize,&l234z[0],&l234y[0]);
            gLine_yz567[fEvtNum] = tfs->make<TGraph>(graphsize,&l567z[0],&l567y[0]);

            gRecoHits_xz[fEvtNum] = tfs->make<TGraph>(8,zreco,xreco);
	    gRecoHits_yz[fEvtNum] = tfs->make<TGraph>(8,zreco,yreco);

            gTruthHits_xz[fEvtNum] = tfs->make<TGraph>(20,&ztruth[0],&xtruth[0]);
            gTruthHits_yz[fEvtNum] = tfs->make<TGraph>(20,&ztruth[0],&ytruth[0]);

            gLines_xz[fEvtNum]->Add(gRecoHits_xz[fEvtNum]);
            gRecoHits_xz[fEvtNum]->SetMarkerStyle(3);
            gRecoHits_xz[fEvtNum]->SetMarkerSize(2);
            gRecoHits_xz[fEvtNum]->SetMarkerColor(kBlack);
            gRecoHits_xz[fEvtNum]->SetLineColor(0);

	    gLines_yz[fEvtNum]->Add(gRecoHits_yz[fEvtNum]);
            gRecoHits_yz[fEvtNum]->SetMarkerStyle(3);
            gRecoHits_yz[fEvtNum]->SetMarkerSize(2);
            gRecoHits_yz[fEvtNum]->SetMarkerColor(kBlack);
            gRecoHits_yz[fEvtNum]->SetLineColor(0);

            gLines_xz[fEvtNum]->Add(gTruthHits_xz[fEvtNum]);
            gTruthHits_xz[fEvtNum]->SetMarkerStyle(3);
            gTruthHits_xz[fEvtNum]->SetMarkerSize(2);
            gTruthHits_xz[fEvtNum]->SetMarkerColor(kViolet);
            gTruthHits_xz[fEvtNum]->SetLineColor(0);

            gLines_yz[fEvtNum]->Add(gTruthHits_yz[fEvtNum]);
            gTruthHits_yz[fEvtNum]->SetMarkerStyle(3);
            gTruthHits_yz[fEvtNum]->SetMarkerSize(2);
            gTruthHits_yz[fEvtNum]->SetMarkerColor(kViolet);
            gTruthHits_yz[fEvtNum]->SetLineColor(0);

            gLines_xz[fEvtNum]->Add(gLine_xz01[fEvtNum]);
            gLines_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            gLines_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            gLine_xz01[fEvtNum]->SetMarkerStyle(3);
            gLine_xz01[fEvtNum]->SetMarkerSize(2);
            gLine_xz01[fEvtNum]->SetMarkerColor(kRed);
            gLine_xz01[fEvtNum]->SetLineColor(kRed);

            gLines_xz[fEvtNum]->Add(gLine_xz234[fEvtNum]);
            gLines_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            gLines_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            gLine_xz234[fEvtNum]->SetMarkerStyle(3);
            gLine_xz234[fEvtNum]->SetMarkerSize(2);
            gLine_xz234[fEvtNum]->SetMarkerColor(kBlue);
            gLine_xz234[fEvtNum]->SetLineColor(kBlue);

            gLines_xz[fEvtNum]->Add(gLine_xz567[fEvtNum]);
            gLines_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            gLines_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            gLine_xz567[fEvtNum]->SetMarkerStyle(3);
            gLine_xz567[fEvtNum]->SetMarkerSize(2);
            gLine_xz567[fEvtNum]->SetMarkerColor(kGreen);
            gLine_xz567[fEvtNum]->SetLineColor(kGreen);

            gLines_yz[fEvtNum]->Add(gLine_yz01[fEvtNum]);
            gLines_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            gLines_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            gLine_yz01[fEvtNum]->SetMarkerStyle(3);
            gLine_yz01[fEvtNum]->SetMarkerSize(2);
            gLine_yz01[fEvtNum]->SetMarkerColor(kRed);
            gLine_yz01[fEvtNum]->SetLineColor(kRed);

            gLines_yz[fEvtNum]->Add(gLine_yz234[fEvtNum]);
            gLines_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            gLines_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            gLine_yz234[fEvtNum]->SetMarkerStyle(3);
            gLine_yz234[fEvtNum]->SetMarkerSize(2);
            gLine_yz234[fEvtNum]->SetMarkerColor(kBlue);
            gLine_yz234[fEvtNum]->SetLineColor(kBlue);

            gLines_yz[fEvtNum]->Add(gLine_yz567[fEvtNum]);
            gLines_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            gLines_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            gLine_yz567[fEvtNum]->SetMarkerStyle(3);
            gLine_yz567[fEvtNum]->SetMarkerSize(2);
            gLine_yz567[fEvtNum]->SetMarkerColor(kGreen);
            gLine_yz567[fEvtNum]->SetLineColor(kGreen);

         }
      }

    } //try
    catch(...) {

    }

    if (goodEvent == true){
       for (int i=0; i<8; i++){
           if (xsim[i] == 999) std::cout<<"A hit is missing in station "<<i<<std::endl;
           if (ysim[i] == 999) std::cout<<"A hit is missing in station "<<i<<std::endl;
           if (zsim[i] == 999) std::cout<<"A hit is missing in station "<<i<<std::endl;
       }

       for (int i=0; i<8; i++){
           double xres = xreco[i] - xsim[i];
	   double yres = yreco[i] - ysim[i];
           hRes_x[i]->Fill(xres);
	   hRes_y[i]->Fill(yres);
        }	  
    }
   
    //clear reco and sim vectors for next event
    for (int i=0; i<8; i++){
        xreco[i] = 999; xsim[i] = 999;
	yreco[i] = 999; ysim[i] = 999;
	zreco[i] = 999; zsim[i] = 999;
    }
    xsim_pair.clear();
    ysim_pair.clear();
    zsim_pair.clear();

  } //want plots
  
    // now create tracks from tracksegments.  Note, this is a placeholder 
    // for now.  The beam track needs to get the momentum from the SpillInfo, 
    // and the secondary track needs to get the momentum from the bend angle
 
    trackv->clear();
    if (tracksegmentv->size() == 3) {
      rb::Track beamtrk;
      beamtrk.Add(tsv[0]);
      beamtrk.SetP(tsv[0].P());
      beamtrk.SetVtx(tsv[0].Vtx());
      trackv->push_back(beamtrk);

      rb::Track sectrk;
      sectrk.Add(tsv[1]);
      sectrk.Add(tsv[2]);
      sectrk.SetP(tsv[1].P()); // this should come from an analysis of the bend angle between track segments 1 and 2.
      sectrk.SetVtx(tsv[1].Vtx()); // this should come from a calculation of the intersection or point of closest approach between track segments 0 and 1.
      trackv->push_back(sectrk);
    }

    evt.put(std::move(spacepointv));
    evt.put(std::move(tracksegmentv));
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeSingleTracks)
