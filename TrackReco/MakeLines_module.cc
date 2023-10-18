////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct a track segment
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
#include "Simulation/SSDHit.h"
#include "Simulation/Particle.h"

using namespace emph;
//using namespace Eigen;

///package to illustrate how to write modules
namespace emph {
  ///
  class MakeLines : public art::EDProducer {
  public:
    explicit MakeLines(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeLines() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

    void    findLine(std::vector<std::vector<double>> v, double lfirst[3], double llast[3]);
    double  findAngle(double p1[3], double p2[3], double p3[3], double p4[3]);
    void    findDist(std::vector<sim::SSDHit> sim, rb::LineSegment track, int seg);
    void    compareAngle(std::vector<sim::SSDHit> sim, rb::LineSegment track, double ang[2]);

  private:
  
    TTree*      tracks;
    int run,subrun,event;
    int         fEvtNum;

    bool        fMakePlots;
    size_t      nPlanes = 20;

    std::vector<const rb::SpacePoint*> hits;
    std::vector<std::vector<double>> sp_arr;
    std::vector<std::vector<double>> sp01;
    std::vector<std::vector<double>> sp234;
    std::vector<std::vector<double>> sp567;

    std::vector<std::vector<double>> truth_arr;
    double truthHitsx[20];
    double truthHitsy[20];
    double truthHitsz[20];
    TGraph* truthHitsxz[100]; TGraph* truthHitsyz[100];

    std::vector<sim::SSDHit> simhit01;
    std::vector<sim::SSDHit> simhit234;
    std::vector<sim::SSDHit> simhit567;

    TGraph* Gxz01[100]; TGraph* Gyz01[100];
    TGraph* Gxz234[100]; TGraph* Gyz234[100];
    TGraph* Gxz567[100]; TGraph* Gyz567[100];
    TMultiGraph* GMULT_xz[100]; TMultiGraph* GMULT_yz[100];
    double l01x[2]; double l01y[2]; double l01z[2];
    double l234x[2]; double l234y[2]; double l234z[2];
    double l567x[2]; double l567y[2]; double l567z[2];

    double recHitsx[8]; 
    double recHitsy[8];
    double recHitsz[8];
    TGraph* recHitsxz[100]; TGraph* recHitsyz[100];

    TH1F* pulls[8]; 

    TH1F* scatteringdist;
    TH1F* bendingdist;
    TH1F* chi2dist_seg1;
    TH1F* chi2dist_seg2;
    TH1F* chi2dist_seg3;
    //int largechi2 = 0;
    TH1F* chi2dist[8];

    TH1F* angdiffx_seg1;
    TH1F* angdiffx_seg2;
    TH1F* angdiffx_seg3;
    TH1F* angdiffy_seg1;
    TH1F* angdiffy_seg2;
    TH1F* angdiffy_seg3;

    TH1F* angdiffx[8];
    TH1F* angdiffy[8];

    //fcl parameters
    bool        fCheckSP;     //Check clusters for event 

    };

  //.......................................................................
  
  emph::MakeLines::MakeLines(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckSP     (pset.get< bool >("CheckSP"))
  {
    //this->produces< std::vector<rb::LineSegment> >();
  }

  //......................................................................
  
//  MakeTrack::~MakeTrack()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................

  // void SSDCalibration::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }

  //......................................................................
  
  void MakeLines::beginRun(art::Run& run)
  {
  }

  //......................................................................
   
  void emph::MakeLines::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    tracks = tfs->make<TTree>("tracks","");
    tracks->Branch("run",&run,"run/I");
    tracks->Branch("subrun",&subrun,"subrun/I");
    tracks->Branch("event",&event,"event/I");

    char *hpulls = new char[8];

    //pulls
    for (int i=0; i<8; i++){
        sprintf(hpulls,"pulls_%d",i);
        pulls[i] = tfs->make<TH1F>(hpulls,hpulls,100,0.,0.2);
    }

    scatteringdist = tfs->make<TH1F>("scattering","scattering",100,-0.015,0.015); 
    bendingdist = tfs->make<TH1F>("bending","bending",100,0.,0.005); //0.0015 for 120
    chi2dist_seg1 = tfs->make<TH1F>("chi2_seg1","chi2_seg1",100,0.,30.);
    chi2dist_seg2 = tfs->make<TH1F>("chi2_seg2","chi2_seg2",100,0.,100.);
    chi2dist_seg3 = tfs->make<TH1F>("chi2_seg3","chi2_seg3",100,0.,1000.);

    angdiffx_seg1 = tfs->make<TH1F>("angdiffx_seg1","angdiffx_seg1",100,-0.1,0.1);
    angdiffx_seg2 = tfs->make<TH1F>("angdiffx_seg2","angdiffx_seg2",100,-0.1,0.1);
    angdiffx_seg3 = tfs->make<TH1F>("angdiffx_seg3","angdiffx_seg3",100,-0.1,0.1);

    angdiffy_seg1 = tfs->make<TH1F>("angdiffy_seg1","angdiffy_seg1",100,-0.1,0.1);
    angdiffy_seg2 = tfs->make<TH1F>("angdiffy_seg2","angdiffy_seg2",100,-0.1,0.1);
    angdiffy_seg3 = tfs->make<TH1F>("angdiffy_seg3","angdiffy_seg3",100,-0.1,0.1);
    
    char *hangx = new char[10];
    char *hangy = new char[10];
    char *chi2st = new char[7];
    for (int i=0; i<8; i++){
        sprintf(hangx,"angdiffx_%d",i);
        sprintf(hangy,"angdiffy_%d",i);
        angdiffx[i] = tfs->make<TH1F>(hangx,hangx,100,-0.005,0.005); //0.02
	angdiffy[i] = tfs->make<TH1F>(hangy,hangy,100,-0.005,0.005);
        
        sprintf(chi2st,"chi2_%d",i);
        chi2dist[i] = tfs->make<TH1F>(chi2st,chi2st,100,0,10);
    }

  }
 
  //......................................................................
  
  void emph::MakeLines::endJob()
  {
  }

  //......................................................................

  void emph::MakeLines::findLine(std::vector<std::vector<double>> v, double lfirst[3], double llast[3])
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

          //does largest be absolute value or nah?

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

  void emph::MakeLines::findDist(std::vector<sim::SSDHit> sim, rb::LineSegment track, int seg){ 
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

        pulls[i.GetStation()]->Fill(d);

        //calculate chi2
        double err = 0.06/std::sqrt(12.);
	double chi2perstation = std::pow((d/err),2);
        chi2 += std::pow((d/err),2);
        chi2dist[i.GetStation()]->Fill(chi2perstation);
    }

    if (seg==1) chi2dist_seg1->Fill(chi2);
    if (seg==2) chi2dist_seg2->Fill(chi2);
    if (seg==3) chi2dist_seg3->Fill(chi2); 
  }
  
  //......................................................................

  double emph::MakeLines::findAngle(double p1[3], double p2[3], double p3[3], double p4[3])
  {
     //create line in cartesian coordinates
     double a1 = p2[0] - p1[0]; double a2 = p4[0] - p3[0]; 
     double b1 = p2[1] - p1[1]; double b2 = p4[1] - p3[1];
     double c1 = p2[2] - p1[2]; double c2 = p4[2] - p3[2];

     TVector3 m1(a1,b1,c1);
     TVector3 m2(a2,b2,c2);

     double theta_rad = m1.Angle(m2);

     if (m2.Phi() < 0) theta_rad *= -1;

     return theta_rad;
  }

  //......................................................................

  void emph::MakeLines::compareAngle(std::vector<sim::SSDHit> sim, rb::LineSegment track, double ang[2]){

     double thetax = 0.;
     double thetay = 0.;
     
     double thetax_re = TMath::ATan2( ( track.X1()[0] - track.X0()[0] ) , ( track.X1()[2] - track.X0()[2] ) ); 
     double thetay_re = TMath::ATan2( ( track.X1()[1] - track.X0()[1] ) , ( track.X1()[2] - track.X0()[2] ) );

     for (auto i : sim){
         double thetax_i = TMath::ATan2(i.GetPx(),i.GetPz());
         double thetay_i = TMath::ATan2(i.GetPy(),i.GetPz());
         thetax += thetax_i;
         thetay += thetay_i;

	 angdiffx[i.GetStation()]->Fill(thetax_re - thetax_i);
         angdiffy[i.GetStation()]->Fill(thetay_re - thetay_i);
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

  void emph::MakeLines::produce(art::Event& evt)
  {
    //std::unique_ptr< std::vector<rb::LineSegment> > trackv(new std::vector<rb::LineSegment>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    //debug
    //if(fEvtNum==877) fMakePlots = true; //two clusters in a (sta,sen)=(2,0)
    //if(fEvtNum==2826) fMakePlots = true; //good event one cluster per plane 

    //if(fEvtNum==479) fMakePlots = true;
    //else fMakePlots = false;
    fMakePlots = true;

    if(fMakePlots){ 

    if (fCheckSP){
       auto hasSP = evt.getHandle<rb::SpacePoint>("makehit");
       if (!hasSP){
       mf::LogError("HasSP")<<"No space points found in event but CheckSP set to true!";
       abort();
       }
    }

    std::string fSPLabel = "makehits";
    art::Handle< std::vector<rb::SpacePoint> > spH;

    std::string fG4Label = "geantgen";
    art::Handle< std::vector<sim::SSDHit> > ssdHitH;

    art::Handle< std::vector<sim::Particle> > particleH;
    try {
      evt.getByLabel(fG4Label,ssdHitH);
    }
    catch(...) {
      std::cout << "WARNING: No SSDHits found!" << std::endl;
    }

    bool hitPresent = false;

    try {
      evt.getByLabel(fSPLabel, spH);
      evt.getByLabel(fG4Label,particleH);
      //if ( !spH->empty() && !ssdHitH->empty() && particleH->size()==1){
      if (!spH->empty() && !ssdHitH->empty() && ssdHitH->size() == 20 && particleH->size()==1){
         for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
             const sim::SSDHit& ssdhit = (*ssdHitH)[idx];

             std::vector<double> x = {ssdhit.GetX(),ssdhit.GetY(),ssdhit.GetZ()};

	     truth_arr.push_back(x);

             if (ssdhit.GetStation() == 0 || ssdhit.GetStation() == 1) simhit01.push_back(ssdhit);
             if (ssdhit.GetStation() == 2 || ssdhit.GetStation() == 3 || ssdhit.GetStation() == 4) simhit234.push_back(ssdhit);
             if (ssdhit.GetStation() == 5 || ssdhit.GetStation() == 6 || ssdhit.GetStation() == 7) simhit567.push_back(ssdhit);
         }

         hitPresent = true;

         for (size_t idx=0; idx < spH->size(); ++idx) {
             const rb::SpacePoint& sp = (*spH)[idx];
             hits.push_back(&sp);
	      
             //create vector of spacepoint arrays
             std::vector<double> x = {sp.Pos()[0],sp.Pos()[1],sp.Pos()[2]};
             sp_arr.push_back(x);

             if (sp.Station() == 0 || sp.Station() == 1) sp01.push_back(x);
             if (sp.Station() == 2 || sp.Station() == 3 || sp.Station() == 4) sp234.push_back(x);
             if (sp.Station() == 5 || sp.Station() == 6 || sp.Station() == 7) sp567.push_back(x);
         }

         for (size_t i=0; i<sp_arr.size(); i++){
             recHitsx[i] = sp_arr[i][0];
             recHitsy[i] = sp_arr[i][1];
             recHitsz[i] = sp_arr[i][2];
         }
	 
	 for (size_t i=0; i<truth_arr.size(); i++){
	     truthHitsx[i] = truth_arr[i][0];
	     truthHitsy[i] = truth_arr[i][1];
	     truthHitsz[i] = truth_arr[i][2];
	 }

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

         scatteringdist->Fill(scattering);
         bendingdist->Fill(bending);

         //assign to track vector
         rb::LineSegment track1 = rb::LineSegment(lfirst01,llast01);
         rb::LineSegment track2 = rb::LineSegment(lfirst234,llast234);
         rb::LineSegment track3 = rb::LineSegment(lfirst567,llast567);

         //trackv->push_back(track1);
         //trackv->push_back(track2);
         //trackv->push_back(track3);

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

         findDist(simhit01,track1,1);
         findDist(simhit234,track2,2);
         findDist(simhit567,track3,3);

         //angular diffs
         double ts01[2]; double ts234[2]; double ts567[2];
         compareAngle(simhit01,track1,ts01); 
         compareAngle(simhit234,track2,ts234);
         compareAngle(simhit567,track3,ts567);

         angdiffx_seg1->Fill(ts01[0]);
         angdiffy_seg1->Fill(ts01[1]);
         angdiffx_seg2->Fill(ts234[0]);
         angdiffy_seg2->Fill(ts234[1]);
         angdiffx_seg3->Fill(ts567[0]);
         angdiffy_seg3->Fill(ts567[1]);

      }

      hits.clear();
      sp_arr.clear();
      truth_arr.clear();
      sp01.clear();
      sp234.clear();
      sp567.clear();

      if (hitPresent){
         if (fEvtNum < 100){
            art::ServiceHandle<art::TFileService> tfs;
            char *Gxz = new char[7];
            char *Gyz = new char[7];
            char *Grec = new char[9];

	    sprintf(Gxz,"Gxz_e%d",fEvtNum);
	    sprintf(Gyz,"Gyz_e%d",fEvtNum);
            sprintf(Grec,"Grec_e%d",fEvtNum);

	    size_t graphsize = 2;
            GMULT_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gxz,Gxz);
	    GMULT_yz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gyz,Gyz); 

            Gxz01[fEvtNum] = tfs->make<TGraph>(graphsize,&l01z[0],&l01x[0]);
	    Gxz234[fEvtNum] = tfs->make<TGraph>(graphsize,&l234z[0],&l234x[0]);
	    Gxz567[fEvtNum] = tfs->make<TGraph>(graphsize,&l567z[0],&l567x[0]);
	    Gyz01[fEvtNum] = tfs->make<TGraph>(graphsize,&l01z[0],&l01y[0]);
            Gyz234[fEvtNum] = tfs->make<TGraph>(graphsize,&l234z[0],&l234y[0]);
            Gyz567[fEvtNum] = tfs->make<TGraph>(graphsize,&l567z[0],&l567y[0]);
         
	    recHitsxz[fEvtNum] = tfs->make<TGraph>(8,&recHitsz[0],&recHitsx[0]);
            recHitsyz[fEvtNum] = tfs->make<TGraph>(8,&recHitsz[0],&recHitsy[0]);

            size_t truthsize = truth_arr.size();
            truthHitsxz[fEvtNum] = tfs->make<TGraph>(20,&truthHitsz[0],&truthHitsx[0]);
            truthHitsyz[fEvtNum] = tfs->make<TGraph>(20,&truthHitsz[0],&truthHitsy[0]);

	    GMULT_xz[fEvtNum]->Add(recHitsxz[fEvtNum]);
	    recHitsxz[fEvtNum]->SetMarkerStyle(3);
            recHitsxz[fEvtNum]->SetMarkerSize(2);
            recHitsxz[fEvtNum]->SetMarkerColor(kBlack);
            recHitsxz[fEvtNum]->SetLineColor(0);

	    GMULT_yz[fEvtNum]->Add(recHitsyz[fEvtNum]);
            recHitsyz[fEvtNum]->SetMarkerStyle(3);
            recHitsyz[fEvtNum]->SetMarkerSize(2);
            recHitsyz[fEvtNum]->SetMarkerColor(kBlack);
            recHitsyz[fEvtNum]->SetLineColor(0);

            GMULT_xz[fEvtNum]->Add(truthHitsxz[fEvtNum]);
            truthHitsxz[fEvtNum]->SetMarkerStyle(3);
            truthHitsxz[fEvtNum]->SetMarkerSize(2);
            truthHitsxz[fEvtNum]->SetMarkerColor(kViolet);
            truthHitsxz[fEvtNum]->SetLineColor(0);

            GMULT_yz[fEvtNum]->Add(truthHitsyz[fEvtNum]);
            truthHitsyz[fEvtNum]->SetMarkerStyle(3);
            truthHitsyz[fEvtNum]->SetMarkerSize(2);
            truthHitsyz[fEvtNum]->SetMarkerColor(kViolet);
            truthHitsyz[fEvtNum]->SetLineColor(0);

	    GMULT_xz[fEvtNum]->Add(Gxz01[fEvtNum]);
	    GMULT_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            GMULT_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            Gxz01[fEvtNum]->SetMarkerStyle(3);
            Gxz01[fEvtNum]->SetMarkerSize(2);
            Gxz01[fEvtNum]->SetMarkerColor(kRed);
            Gxz01[fEvtNum]->SetLineColor(kRed);

	    GMULT_xz[fEvtNum]->Add(Gxz234[fEvtNum]);
            GMULT_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            GMULT_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            Gxz234[fEvtNum]->SetMarkerStyle(3);
            Gxz234[fEvtNum]->SetMarkerSize(2);
            Gxz234[fEvtNum]->SetMarkerColor(kBlue);
            Gxz234[fEvtNum]->SetLineColor(kBlue);

	    GMULT_xz[fEvtNum]->Add(Gxz567[fEvtNum]);
            GMULT_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            GMULT_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            Gxz567[fEvtNum]->SetMarkerStyle(3);
            Gxz567[fEvtNum]->SetMarkerSize(2);
            Gxz567[fEvtNum]->SetMarkerColor(kGreen);
            Gxz567[fEvtNum]->SetLineColor(kGreen);

	    GMULT_yz[fEvtNum]->Add(Gyz01[fEvtNum]);
            GMULT_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            GMULT_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            Gyz01[fEvtNum]->SetMarkerStyle(3);
            Gyz01[fEvtNum]->SetMarkerSize(2);
            Gyz01[fEvtNum]->SetMarkerColor(kRed);
            Gyz01[fEvtNum]->SetLineColor(kRed);

            GMULT_yz[fEvtNum]->Add(Gyz234[fEvtNum]);
            GMULT_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            GMULT_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            Gyz234[fEvtNum]->SetMarkerStyle(3);
            Gyz234[fEvtNum]->SetMarkerSize(2);
            Gyz234[fEvtNum]->SetMarkerColor(kBlue);
            Gyz234[fEvtNum]->SetLineColor(kBlue);

            GMULT_yz[fEvtNum]->Add(Gyz567[fEvtNum]);
            GMULT_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
            GMULT_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
            Gyz567[fEvtNum]->SetMarkerStyle(3);
            Gyz567[fEvtNum]->SetMarkerSize(2);
            Gyz567[fEvtNum]->SetMarkerColor(kGreen);
            Gyz567[fEvtNum]->SetLineColor(kGreen);

         }
      simhit01.clear();
      simhit234.clear();
      simhit567.clear();
      }
   } //try
    catch(...) {
   }

 } //want plots
  //evt.put(std::move(trackv));

  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeLines)
