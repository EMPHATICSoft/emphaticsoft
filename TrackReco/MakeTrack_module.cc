////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct a track
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

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
//#include "canvas/Persistency/Common/Ptr.h"
//#include "canvas/Persistency/Common/PtrVector.h"
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

using namespace emph;

// This pair is used to store the X and Y
// coordinates of a point respectively
#define pdd std::pair<double, double>

///package to illustrate how to write modules
namespace emph {
  ///
  class MakeTrack : public art::EDProducer {
  public:
    explicit MakeTrack(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeTrack() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();
    void MakePoint(const rb::SSDCluster& cl, rb::LineSegment& ls);
    pdd MakeIntersection(pdd A, pdd B, pdd C, pdd D);
    void MakeIntersection3D(double A[3],double B[3], double C[3], double D[3], double F[3]);
    double dotProduct(double a[3], double b[3]); 
    void crossProduct(double a[3], double b[3], double c[3]);
    void ClosestApproach(double A[3],double B[3], double C[3], double D[3], double F[3], double l1[3], double l2[3]);

  private:
  
    TTree*      spacepoint;
    int run,subrun,event;
    int         fEvtNum;

    //MAYBE THIS art::PtrVector<const rb::SSDCluster> clusters;
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> stripv;
    std::vector<rb::SpacePoint> spv;

    std::map<std::pair<int, int>, int> clustMap;
    std::map<std::pair<int, int>, std::pair<int, geo::sensorView> > planeViewMap;

    bool        fMakePlots;
    int 	goodclust = 0;
    int 	fTotClust;
    int         naur = 0; 
    size_t         nPlanes = 20;

    //fcl parameters
    bool        fCheckClusters;     ///< Check clusters for event 

    emph::dgmap::DetGeoMap* fDetGeoMap;

    int st;
    TH2F* spdist0;
    TH2F* spdist1;
    TH2F* spdist2;
    TH2F* spdist3;
    TH2F* spdist4;
    TH2F* spdist5;
    TH2F* spdist6;
    TH2F* spdist7;

    TH2F* spdist2_xy;
    TH2F* spdist3_xy;
    TH2F* spdist2_ux;
    TH2F* spdist3_ux;
    TH2F* spdist2_uy;
    TH2F* spdist3_uy;

    TH2F* xzdist;
    TH2F* yzdist;

    //TH2F* xzdist_evt[100];
    TGraph* Gxzdist_evt[100];
    TGraph* Gyzdist_evt[100];

    TGraph* GSIM_xz[100];
    TGraph* GSIM_yz[100];
    TMultiGraph* GMULT_xz[100];
    TMultiGraph* GMULT_yz[100];

    TGraph2D* GSIM_xyz[100];
    TGraph2D* Gxyzdist_evt[100];
    TMultiGraph* GMULT_xyz[100];

    TH1F* res_x[8];
    TH1F* res_y[8];
    TH1F* res_x_re[8];
    TH1F* res_y_re[8];

    double xsp[8] = {999,999,999,999,999,999,999,999};
    double ysp[8] = {999,999,999,999,999,999,999,999};
    double zsp[8] = {999,999,999,999,999,999,999,999};

    double xvec[8] = {999,999,999,999,999,999,999,999};
    double yvec[8] = {999,999,999,999,999,999,999,999};
    double zvec[8] = {999,999,999,999,999,999,999,999};

    std::vector<double> xvec_re;
    std::vector<double> yvec_re;
    std::vector<double> zvec_re;
  
    std::vector<double> xvec_sim;
    std::vector<double> yvec_sim;
    std::vector<double> zvec_sim;

    std::vector<std::pair<double, int>> xvec_sim_pair;
    std::vector<std::pair<double, int>> yvec_sim_pair;
    std::vector<std::pair<double, int>> zvec_sim_pair;
    //double xvec_sim[8] = {999,999,999,999,999,999,999,999};
    //double yvec_sim[8] = {999,999,999,999,999,999,999,999};
    //double zvec_sim[8] = {999,999,999,999,999,999,999,999};

//    std::vector<const Double_t>* xvec;
//    std::vector<const Double_t>* zvec;

  };

  //.......................................................................
  
  emph::MakeTrack::MakeTrack(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")),
    fDetGeoMap(NULL)
  {
    this->produces< std::vector<rb::SpacePoint> >();
    //fEvtNum = 0;
    //fCheckClusters     (pset.get< bool >("CheckClusters"));
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
  
  void MakeTrack::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::cmap::ChannelMapService> fChannelMap;
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();

    for (int fer=0; fer<10; ++fer){
      for (int mod=0; mod<6; ++mod){
        emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
        if (!fChannelMap->CMap()->IsValidEChan(echan)) continue;
          emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);

        const emph::geo::SSDStation &st = emgeo->GetSSDStation(dchan.Station());
        const emph::geo::Detector   &sd = st.GetSSD(dchan.Channel());

        planeViewMap[std::make_pair(dchan.Station(),dchan.Channel())] = std::make_pair(dchan.Plane(),sd.View());
      }
    }
  }

  //......................................................................
   
  void emph::MakeTrack::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");
   
    spdist0 = tfs->make<TH2F>("spdist0","spdist0",1000,-50.,50.,1000,-50.,50.); 
    spdist1 = tfs->make<TH2F>("spdist1","spdist1",1000,-50.,50.,1000,-50.,50.);
    spdist2 = tfs->make<TH2F>("spdist2","spdist2",1000,-50.,50.,1000,-50.,50.);
    spdist3 = tfs->make<TH2F>("spdist3","spdist3",1000,-50.,50.,1000,-50.,50.);
    spdist4 = tfs->make<TH2F>("spdist4","spdist4",1000,-50.,50.,1000,-50.,50.);
    spdist5 = tfs->make<TH2F>("spdist5","spdist5",1000,-50.,50.,1000,-50.,50.);
    spdist6 = tfs->make<TH2F>("spdist6","spdist6",1000,-50.,50.,1000,-50.,50.);
    spdist7 = tfs->make<TH2F>("spdist7","spdist7",1000,-50.,50.,1000,-50.,50.);

    spdist2_xy = tfs->make<TH2F>("spdist2_xy","spdist2_xy",1000,-50.,50.,1000,-50.,50.);
    spdist3_xy = tfs->make<TH2F>("spdist3_xy","spdist3_xy",1000,-50.,50.,1000,-50.,50.);
    spdist2_ux = tfs->make<TH2F>("spdist2_ux","spdist2_ux",1000,-50.,50.,1000,-50.,50.);
    spdist3_ux = tfs->make<TH2F>("spdist3_ux","spdist3_ux",1000,-50.,50.,1000,-50.,50.);
    spdist2_uy = tfs->make<TH2F>("spdist2_uy","spdist2_uy",1000,-50.,50.,1000,-50.,50.);
    spdist3_uy = tfs->make<TH2F>("spdist3_uy","spdist3_uy",1000,-50.,50.,1000,-50.,50.);

    xzdist = tfs->make<TH2F>("xzdist","xzdist",2050,-50.,2000.,1000,-50.,50.); 
    yzdist = tfs->make<TH2F>("yzdist","yzdist",2050,-50.,2000.,1000,-50.,50.);

    char *hevt = new char[11];
    //char *Ghevt = new char[11];

    //space point distribution per event
    /*
    for (int i=0; i<100; i++){
	sprintf(hevt,"spdist_e%d",i);
        //sprintf(Ghevt,"Gspdist_e%d",i);
        //Gxzdist_evt[i] = tfs->make<TGraph>(8,xvec,zvec);
	xzdist_evt[i] = tfs->make<TH2F>(hevt,hevt,2050,-50.,2000.,1000,-50.,50.);
    } 
   */

    char *hresx = new char[7];
    char *hresy = new char[7];
    char *hresxre = new char[12];
    char *hresyre = new char[12];
    for (int i=0; i<8; i++){
	sprintf(hresx,"res_x_%d",i);
        res_x[i] = tfs->make<TH1F>(hresx,hresx,100,-0.1,0.1);	
        //res_x_re[i] = tfs->make<TH1F>(hresx,hresx,100,-0.1,0.1);
        sprintf(hresy,"res_y_%d",i);
        res_y[i] = tfs->make<TH1F>(hresy,hresy,100,-0.1,0.1);
	//res_y_re[i] = tfs->make<TH1F>(hresy,hresy,100,-0.1,0.1);
    }
    for (int i=0; i<8; i++){
        sprintf(hresxre,"res_x_re_%d",i);
        res_x_re[i] = tfs->make<TH1F>(hresxre,hresxre,100,-0.1,0.1);
	sprintf(hresyre,"res_y_re_%d",i);
        res_y_re[i] = tfs->make<TH1F>(hresyre,hresyre,100,-0.1,0.1);
    }
  }
 
  //......................................................................
  
  void emph::MakeTrack::endJob()
  {
       std::cout<<"Number of clusters with one cluster per sensor: "<<goodclust<<std::endl;
       std::cout<<"Number of available clusters: "<<naur+goodclust<<std::endl;
  }

  //......................................................................

  void emph::MakeTrack::MakePoint(const rb::SSDCluster& cl, rb::LineSegment& ls)
  {
    if (!fDetGeoMap) fDetGeoMap = new emph::dgmap::DetGeoMap();

    //std::cout<<"Plane in MakePoint is "<<cl.Plane()<<std::endl;
    //probably doesn't need to be an if statement?
    if(fDetGeoMap->SSDClusterToLineSegment(cl, ls)){

    }
  }

  //......................................................................
  
  pdd emph::MakeTrack::MakeIntersection(pdd A, pdd B, pdd C, pdd D){
      // Line AB represented as a1x + b1y = c1
      double a1 = B.second - A.second;
      double b1 = A.first - B.first;
      double c1 = a1*(A.first) + b1*(A.second);

      // Line CD represented as a2x + b2y = c2
      double a2 = D.second - C.second;
      double b2 = C.first - D.first;
      double c2 = a2*(C.first)+ b2*(C.second);

      double determinant = a1*b2 - a2*b1;

      if (determinant == 0){
        // The lines are parallel. This is simplified
        //  by returning a pair of FLT_MAX
        // This like...shouldn't happen.
        return std::make_pair(FLT_MAX, FLT_MAX);
      }

      else{
        double x = (b2*c1 - b1*c2)/determinant;
        double y = (a1*c2 - a2*c1)/determinant;
        return std::make_pair(x, y);
      } 

  }

  //......................................................................

  void emph::MakeTrack::ClosestApproach(double A[3],double B[3], double C[3], double D[3], double F[3], double l1[3], double l2[3]){

        double r12 = (B[0] - A[0])*(B[0] - A[0]) + (B[1] - A[1])*(B[1] - A[1]) + (B[2] - A[2])*(B[2] - A[2]);
	double r22 = (D[0] - C[0])*(D[0] - C[0]) + (D[1] - C[1])*(D[1] - C[1]) + (D[2] - C[2])*(D[2] - C[2]);

	double d4321 = (D[0] - C[0])*(B[0] - A[0]) + (D[1] - C[1])*(B[1] - A[1]) + (D[2] - C[2])*(B[2] - A[2]);
        double d3121 = (C[0] - A[0])*(B[0] - A[0]) + (C[1] - A[1])*(B[1] - A[1]) + (C[2] - A[2])*(B[2] - A[2]);
	double d4331 = (D[0] - C[0])*(C[0] - A[0]) + (D[1] - C[1])*(C[1] - A[1]) + (D[2] - C[2])*(C[2] - A[2]);

        //double s = (d4321*d4331 + d3121*r22) / (r12*r22 + d4321*d4321);
        //double t = (d4321*d3121 - d4331*r12) / (r12*r22 + d4321*d4321);
	double s = (-d4321*d4331 + d3121*r22) / (r12*r22 - d4321*d4321);
	double t = (d4321*d3121 - d4331*r12) / (r12*r22 - d4321*d4321);

	double L1[3]; double L2[3];
	if ( s >= 0 && s <= 1 && t >=0 && t <= 1){
	   std::cout<<"Closest approach all good :)"<<std::endl;
//	else std::cout<<"Uh oh"<<std::endl;

       	   //l1(s) = A + s(B-A)	
	   //l2(t) = C + t(D-C)
	   //double L1[3]; double L2[3];
	   for (int i=0; i<3; i++){
	       L1[i] = A[i] + s*(B[i] - A[i]);
	       L2[i] = C[i] + t*(D[i] - C[i]);	    
	       F[i] = (L1[i] + L2[i])/2.;
	       l1[i] = L1[i]; 
	       l2[i] = L2[i];
	   }
	//}
	//std::cout<<"CA CHECK (L1)...x: "<<L1[0]<<"   y: "<<L1[1]<<"   z: "<<L1[2]<<std::endl;
	//std::cout<<"CA CHECK (L2)...x: "<<L2[0]<<"   y: "<<L2[1]<<"   z: "<<L2[2]<<std::endl;
        }
	else{
	   std::cout<<"Uh oh"<<std::endl;
	   std::cout<<"A: ("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<std::endl;
	   std::cout<<"B: ("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<std::endl;
	   std::cout<<"C: ("<<C[0]<<","<<C[1]<<","<<C[2]<<")"<<std::endl;
	   std::cout<<"D: ("<<D[0]<<","<<D[1]<<","<<D[2]<<")"<<std::endl;
	   std::cout<<"s: "<<s<<std::endl;
	   std::cout<<"t: "<<t<<std::endl;

	   std::clamp(s,0.,1.);
	   std::clamp(t,0.,1.);
 	
	   double l1p3[3]; 
	   double l1p4[3];
	   double l2p1[3];
	   double l2p2[3];
 
           double d4121 = (D[0] - A[0])*(B[0] - A[0]) + (D[1] - A[1])*(B[1] - A[1]) + (D[2] - A[2])*(B[2] - A[2]); 
	   double d4332 = (D[0] - C[0])*(C[0] - B[0]) + (D[1] - C[1])*(C[1] - B[1]) + (D[2] - C[2])*(C[2] - B[2]);

           double s_l1p3 = d3121/r12;
	   double s_l1p4 = d4121/r12;
	   double t_l2p1 = -d4331/r22;
	   double t_l2p2 = -d4332/r22;

	   double d_l1p3; 	
	   double d_l1p4;
	   double d_l2p1;
	   double d_l2p2;

	   for (int i=0; i<3; i++){	
	       l1p3[i] = A[i] + s_l1p3*(B[i] - A[i]);
               l1p4[i] = A[i] + s_l1p4*(B[i] - A[i]); 
	       l2p1[i] = C[i] + t_l2p1*(D[i] - C[i]);
	       l2p2[i] = C[i] + t_l2p2*(D[i] - C[i]);

	       //d_l1p3 = (C[i] - l1p3[i])*(C[i] - l1p3[i]);	
	       //d_l1p4 = (D[i] - l1p4[i])*(D[i] - l1p4[i]);
	       //d_l2p1 = (A[i] - l2p1[i])*(A[i] - l2p1[i]);
	       //d_l2p2 = (B[i] - l2p2[i])*(B[i] - l2p2[i]);
	   }   
	   d_l1p3 = dotProduct(C,l1p3);
	   d_l1p4 = dotProduct(D,l1p4);
	   d_l2p1 = dotProduct(A,l2p1);
	   d_l2p2 = dotProduct(B,l2p2);
	   
	   if (d_l1p3 < d_l1p4){
	      for (int i=0; i<3; i++) L1[i] = l1p3[i];
	   }
	   else{
	      for (int i=0; i<3; i++) L1[i] = l1p4[i];
	   }
	   if (d_l2p1 < d_l2p2){
	      for (int i=0; i<3; i++) L2[i] = l2p1[i];
	   }
	   else{
	      for (int i=0; i<3; i++) L2[i] = l2p2[i];
	   }
	   for (int i=0; i<3; i++){
		F[i] = (L1[i] + L2[i])/2.;
           }
	}	
  }

  //......................................................................

  void emph::MakeTrack::MakeIntersection3D(double A[3],double B[3], double C[3], double D[3], double F[3]){
        double p[3];
        double v[3];
        double q[3]; 
        double u[3];
        for (int i=0; i<3; i++){
	    p[i] = A[i];
	    v[i] = B[i] - A[i];
            q[i] = C[i];
            u[i] = D[i] - C[i];
        }

        // find cross product a = v x u
        double a[3];
        crossProduct(v,u,a);

        // find dot product = (v x u) . (v x u)
        double dot = dotProduct(a,a);

        // if v and u are parallel (v x u = 0), then no intersection, return NaN point
        if(dot == 0){
	  for (int i=0; i<3; i++){
	      F[i] = NAN;
	  }
	}
	
        // find b = (Q1-P1) x u
        double b_temp[3]; 
        for (int i=0; i<3; i++){
            b_temp[i] = q[i] - p[i];
        }

        double b[3];
        crossProduct(b_temp,u,b);
   
        // find t = (b.a)/(a.a) = ((Q1-P1) x u) .(v x u) / (v x u) . (v x u)
        double t = dotProduct(b,a) / dot;

        for (int i=0; i<3; i++){
	    F[i] = p[i] + (t*v[i]);
	}
  } 

  //......................................................................

  double emph::MakeTrack::dotProduct(double a[3], double b[3]){
     double product = 0;
     for (int i=0; i<3; i++){
         product = product + a[i] * b[i];
     }
     return product;
  }

  //......................................................................
  
  void emph::MakeTrack::crossProduct(double a[3], double b[3], double c[3]){
     c[0] = a[1] * b[2] - a[2] * b[1];
     c[1] = -(a[0] * b[2] - a[2] * b[0]);
     c[2] = a[0] * b[1] - a[1] * b[0];
  }
 
  //......................................................................
  void emph::MakeTrack::produce(art::Event& evt)
  {

    std::unique_ptr< std::vector<rb::SpacePoint> > spacepointv(new std::vector<rb::SpacePoint>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    //debug
    //if(fEvtNum==877) fMakePlots = true; //two clusters in a (sta,sen)=(2,0)
    //if(fEvtNum==2826) fMakePlots = true; //good event one cluster per plane 

    //if(fEvtNum==43) fMakePlots = true;
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
    //if (!ssdHitH->empty()) {}
bool goodEvent = false;
    try {
      evt.getByLabel(fClusterLabel, clustH);
      if (!clustH->empty()){
         for (size_t idx=0; idx < clustH->size(); ++idx) {
              const rb::SSDCluster& clust = (*clustH)[idx];
              //MAYBE THIS   art::Ptr<const rb::SSDCluster> clustRR(clustH,idx);
/*
	      std::cout<<"Station from cluster handle is "<<clust.Station()<<std::endl;
	      std::cout<<"Sensor from cluster handle is "<<clust.Sensor()<<std::endl;
	      std::cout<<"Plane from cluster handle is "<<clust.Plane()<<std::endl;
	      std::cout<<"HiLo from cluster handle is "<<clust.HiLo()<<std::endl;
	      std::cout<<"........"<<std::endl;
*/
	      ++clustMap[std::pair<int,int>(clust.Station(),clust.Sensor())];
	      clusters.push_back(&clust); //push back a  pointer

              //MAYBE THIS   clusters.push_back(clustRR);
	  }

//ONE CLUSTER PER PLANE

	  //bool goodEvent = false; //true
          //If there are more clusters than sensors, skip event
	  if (clusters.size()==20){ //what should this be...
	     for (auto i : clustMap){
		 if (i.second != 1){goodEvent = false; break;} //std::cout<<"More than one cluster per plane :("<<std::endl;}        
	         else goodEvent = true;
	     }
	     if (goodEvent==true) {goodclust++; std::cout<<"First test passed :3 @ Event "<<fEvtNum<<std::endl;}
	     else {naur++; std::cout<<                     "Naurrrrrrr        :("<<std::endl;}
          }
	  else naur++;
//ONE CLUSTER PER PLANE
	  //bool goodEvent = true;
		
	  //std::cout<<"clusters.size(): "<<clusters.size()<<std::endl;

         std::vector<const rb::SSDCluster*> cl_group[20];
         std::vector<const rb::LineSegment*> ls_group[20];

         for (size_t i=0; i<clusters.size(); i++){
             geo::sensorView view = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].second;

             //group clusters according to plane
             //within each station,do every combination
             int plane = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].first;
             cl_group[plane].push_back(clusters[i]);
             //ls_group[plane].push_back(&stripv[i]);
          }
//ANY CLUSTER
/*          bool goodEvent = true;
          for (size_t i=0; i<nPlanes; i++){
              if (cl_group[i].size() == 0){ goodEvent = false; break; }
          }
*/
//ANY CLUSTER
          //std::cout<<"Good event is..."<<goodEvent<<std::endl;

	  rb::LineSegment strip;
          if (goodEvent==true && clusters.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
		stripv.push_back(strip);
		MakePoint(*clusters[i],stripv[i]);
		
		int plane = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].first;
	        //ls_group[plane].push_back(&stripv[i]);
	     }
          }//makepoint is misleading it's more like makesegment

	//if line segment populated, make a point		 
	//if (goodEvent == true && stripv.size() > 0){
	   rb::SpacePoint sp;

	  /* std::vector<const rb::SSDCluster*> cl_group[20];
           std::vector<const rb::LineSegment*> ls_group[20];

           for (size_t i=0; i<clusters.size(); i++){
	       geo::sensorView view = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].second;

	       //group clusters according to plane
	       //within each station,do every combination
	       int plane = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].first;
	       cl_group[plane].push_back(clusters[i]);
               ls_group[plane].push_back(&stripv[i]);
           }

	   bool goodEvent = true;
           for (size_t i=0; i<20; i++){
	       if (cl_group[i].size() == 0){ goodEvent = false; break; }
	   }
	   std::cout<<"Good event is..."<<goodEvent<<std::endl;
 */
	   if (goodEvent == true && stripv.size() > 0){

	   for (size_t i=0; i<clusters.size(); i++){
	       geo::sensorView view = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].second;
               int plane = planeViewMap[std::make_pair(clusters[i]->Station(),clusters[i]->Sensor())].first;
               ls_group[plane].push_back(&stripv[i]);
           }
		
	   //for stations 2 and 3 only (xy)
	   for (size_t i=0; i<nPlanes; i++){
	       if (i==5 || i==8 ){
		  for (size_t j=0; j<ls_group[i].size(); j++){
                      for (size_t k=0; k<ls_group[i+1].size(); k++){
	                  st = cl_group[i][j]->Station();
                          //pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          //pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          //pdd fC = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                          //pdd fD = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                          //pdd point = MakeIntersection(fA,fB,fC,fD);
                          //double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};

		          double fA[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
			  double fB[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
		          double fC[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
			  double fD[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };

			  double x[3];
			  MakeIntersection3D(fA,fB,fC,fD,x);
			
                          sp.SetX(x);
                          if (cl_group[i+1][k]->Station() == st){}
                          else std::cout<<"XY: Stations do not match..."<<std::endl;

                          //sp.SetStation(st);
                          //spv.push_back(sp);
                          if (st==2) spdist2_xy->Fill(x[0],x[1]);
                          if (st==3) spdist3_xy->Fill(x[0],x[1]);
		      }
		   }
	       }
	   }
	   //for stations 2 and 3 only (uy)
	   for (size_t i=0; i<nPlanes; i++){
               if (i==4 || i==7 ){
                  for (size_t j=0; j<ls_group[i].size(); j++){
                      for (size_t k=0; k<ls_group[i+1].size(); k++){
                          st = cl_group[i][j]->Station();
                          //pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          //pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          //pdd fC = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                          //pdd fD = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                          //pdd point = MakeIntersection(fA,fB,fC,fD);
                          //double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};

			  double fA[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                          double fB[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                          double fC[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                          double fD[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };

                          double x[3];
                          MakeIntersection3D(fA,fB,fC,fD,x);

                          sp.SetX(x);
                          if (cl_group[i+1][k]->Station() == st){}
                          else std::cout<<"XY: Stations do not match..."<<std::endl;

                          //sp.SetStation(st);
                          //spv.push_back(sp);
                          if (st==2) spdist2_uy->Fill(x[0],x[1]);
                          if (st==3) spdist3_uy->Fill(x[0],x[1]);
                      }
                   }
               }
           }
	   //for stations 2 and 3 only (ux)
	   for (size_t i=0; i<nPlanes; i++){
               if (i==4 || i==7 ){
                  for (size_t j=0; j<ls_group[i].size(); j++){
                      for (size_t k=0; k<ls_group[i+2].size(); k++){
                          st = cl_group[i][j]->Station();
                          //pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          //pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          //pdd fC = std::make_pair(ls_group[i+2][k]->X0()[0],ls_group[i+2][k]->X0()[1]);
                          //pdd fD = std::make_pair(ls_group[i+2][k]->X1()[0],ls_group[i+2][k]->X1()[1]);

                          //pdd point = MakeIntersection(fA,fB,fC,fD);
			  double fA[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                          double fB[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                          double fC[3] = { ls_group[i+2][k]->X0()[0], ls_group[i+2][k]->X0()[1], ls_group[i+2][k]->X0()[2] };
                          double fD[3] = { ls_group[i+2][k]->X1()[0], ls_group[i+2][k]->X1()[1], ls_group[i+2][k]->X1()[2] };

                          double x[3];
                          MakeIntersection3D(fA,fB,fC,fD,x);
	
                          //double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};
                          sp.SetX(x);
                          if (cl_group[i+2][k]->Station() == st){}
                          else std::cout<<"XY: Stations do not match..."<<std::endl;

                          //sp.SetStation(st);
                          //spv.push_back(sp);
                          if (st==2) spdist2_ux->Fill(x[0],x[1]);
                          if (st==3) spdist3_ux->Fill(x[0],x[1]);
                      }
                   }
               }
           }

	   for (size_t i=0; i<nPlanes; i++){
	       //station 0,1,4,7
	       if (i==0 || i==2 || i==10 || i==18){ //|| i==4 || i==7 || i==10 || i==18){
		  //if (i==4 || i==7) i++;
	          for (size_t j=0; j<ls_group[i].size(); j++){
		      st = cl_group[i][j]->Station();
		      //std::cout<<"Station is..."<<st<<std::endl;
	              for (size_t k=0; k<ls_group[i+1].size(); k++){
		          pdd _fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          pdd _fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          pdd _fC = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                          pdd _fD = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                          pdd _point = MakeIntersection(_fA,_fB,_fC,_fD);	 

		          //make point vector (x,y,z)
		          double x[3] = {_point.first,_point.second,ls_group[i][j]->X0()[2]};
		          //z-component for .X0() and .X1() should be the same      

			  double fA[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                          double fB[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                          double fC[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                          double fD[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };

                          double _x[3];
                          MakeIntersection3D(fA,fB,fC,fD,_x);

			  double fAsz[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
			  double fBsz[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X0()[2] };
			  double fCsz[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i][j]->X0()[2] };
		          double fDsz[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i][j]->X0()[2] };

		          double xsz[3];
			  MakeIntersection3D(fAsz,fBsz,fCsz,fDsz,xsz);

			  double xca[3];
	                  double l1[3]; double l2[3];
			  ClosestApproach(fA,fB,fC,fD,xca,l1,l2);

			  //check
			  std::cout<<"...................STATION "<<st<<"......................"<<std::endl;
                          //std::cout<<"PREVIOUS...x: "<<x[0]<<"   y: "<<x[1]<<"   z: "<<x[2]<<std::endl;
			  //std::cout<<"SZ CHECK...x: "<<xsz[0]<<"   y: "<<xsz[1]<<"   z: "<<xsz[2]<<std::endl;
                          //std::cout<<"NEW........x: "<<_x[0]<<"   y: "<<_x[1]<<"   z: "<<_x[2]<<std::endl;
		          std::cout<<"CA CHECK...x: "<<xca[0]<<"   y: "<<xca[1]<<"   z: "<<xca[2]<<std::endl;
                          std::cout<<".................................................."<<std::endl;

			  //set SpacePoint object
			  sp.SetX(x);

			  //check stations
			  if (cl_group[i+1][k]->Station() == st){} 
                          else std::cout<<"XY: Stations do not match..."<<std::endl;
                          
                          sp.SetStation(st);
			  spv.push_back(sp);
                          //add to unique pointer vector
                          spacepointv->push_back(sp);

			  if (st==0) spdist0->Fill(x[0],x[1]);
                          if (st==1) spdist1->Fill(x[0],x[1]);
                          if (st==4) spdist4->Fill(x[0],x[1]);
			  if (st==7) spdist7->Fill(x[0],x[1]); 
			  //if (st==2) spdist2_xy->Fill(x[0],x[1]);
			  //if (st==3) spdist3_xy->Fill(x[0],x[1]);
		          //if (st!=2 || st!=3) 
		          xzdist->Fill(x[2],x[0]);
			  //if (st!=2 || st!=3) 
			  yzdist->Fill(x[2],x[1]);
			  if (fEvtNum < 8000){
			     //xzdist_evt[fEvtNum]->Fill(x[2],x[0]);
			     xvec[st] = xca[0];
			     yvec[st] = xca[1];
			     zvec[st] = xca[2];
			     //xvec_re.push_back(l1[0]);
		             //xvec_re.push_back(l2[0]);
			     //yvec_re.push_back(l1[1]);
                             //yvec_re.push_back(l2[1]);
			     //yvec_re.push_back(l1[2]);
                             //zvec_re.push_back(l2[2]);
			     
		          }
		      }
	          }
		  //std::cout<<"Station is "<<st<<" and sensor is "<<i<<std::endl;
	          //if (st!=2 || st!=3) 
	          i++; //skip following sensor
		  //if (i==5) --i; //idk why this just doesn't work
		  //if (i==8) --i;
		  //std::cout<<"AND NOW Station is "<<st<<" and sensor is "<<i<<std::endl;
	       }
	       //station 2,3,5,6
	       if (i==4 || i==7 || i==12 || i==15){
	          for (size_t j=0; j<ls_group[i].size(); j++){
		      st = cl_group[i][j]->Station();
		      //std::cout<<"Station is..."<<st<<std::endl;
                      for (size_t k=0; k<ls_group[i+1].size(); k++){
			  for (size_t l=0; l<ls_group[i+2].size(); l++){ 
         	              pdd _fA01 = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                              pdd _fB01 = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                              pdd _fC01 = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]); 
                              pdd _fD01 = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                              pdd _point01 = MakeIntersection(_fA01,_fB01,_fC01,_fD01);

			      double fA01sz[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
			      double fB01sz[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X0()[2] };
			      double fC01sz[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i][j]->X0()[2] };
		              double fD01sz[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i][j]->X0()[2] };

			      double x01sz[3];
			      MakeIntersection3D(fA01sz,fB01sz,fC01sz,fD01sz,x01sz);

			      double fA01[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                              double fB01[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                              double fC01[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                              double fD01[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };

                              double x01[3];
                              MakeIntersection3D(fA01,fB01,fC01,fD01,x01);

			      double x01ca[3];
			      double l1_01[3]; double l2_01[3];
                              ClosestApproach(fA01,fB01,fC01,fD01,x01ca,l1_01,l2_01);

                   	      pdd _fA02 = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                   	      pdd _fB02 = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                              pdd _fC02 = std::make_pair(ls_group[i+2][l]->X0()[0],ls_group[i+2][l]->X0()[1]);
	                      pdd _fD02 = std::make_pair(ls_group[i+2][l]->X1()[0],ls_group[i+2][l]->X1()[1]);

                   	      pdd _point02 = MakeIntersection(_fA02,_fB02,_fC02,_fD02);

			      double fA02sz[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
			      double fB02sz[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X0()[2] };
			      double fC02sz[3] = { ls_group[i+2][l]->X0()[0], ls_group[i+2][l]->X0()[1], ls_group[i][j]->X0()[2] };
		              double fD02sz[3] = { ls_group[i+2][l]->X1()[0], ls_group[i+2][l]->X1()[1], ls_group[i][j]->X0()[2] };

			      double x02sz[3];
                              MakeIntersection3D(fA02sz,fB02sz,fC02sz,fD02sz,x02sz);

			      double fA02[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                              double fB02[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                              double fC02[3] = { ls_group[i+2][l]->X0()[0], ls_group[i+2][l]->X0()[1], ls_group[i+2][l]->X0()[2] };
                              double fD02[3] = { ls_group[i+2][l]->X1()[0], ls_group[i+2][l]->X1()[1], ls_group[i+2][l]->X1()[2] };

                              double x02[3];
                              double l1_02[3]; double l2_02[3];
                              MakeIntersection3D(fA02,fB02,fC02,fD02,x02);

			      double x02ca[3];
                              ClosestApproach(fA02,fB02,fC02,fD02,x02ca,l1_02,l2_02);

                   	      pdd _fA12 = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                              pdd _fB12 = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);
                              pdd _fC12 = std::make_pair(ls_group[i+2][l]->X0()[0],ls_group[i+2][l]->X0()[1]);
                              pdd _fD12 = std::make_pair(ls_group[i+2][l]->X1()[0],ls_group[i+2][l]->X1()[1]);

	                      pdd _point12 = MakeIntersection(_fA12,_fB12,_fC12,_fD12);

			      double fA12sz[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i][j]->X0()[2] };
			      double fB12sz[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i][j]->X0()[2] };
			      double fC12sz[3] = { ls_group[i+2][l]->X0()[0], ls_group[i+2][l]->X0()[1], ls_group[i][j]->X0()[2] };
			      double fD12sz[3] = { ls_group[i+2][l]->X1()[0], ls_group[i+2][l]->X1()[1], ls_group[i][j]->X0()[2] };	

			      double x12sz[3];
                              MakeIntersection3D(fA12sz,fB12sz,fC12sz,fD12sz,x12sz);

			      double fA12[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                              double fB12[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };
                              double fC12[3] = { ls_group[i+2][l]->X0()[0], ls_group[i+2][l]->X0()[1], ls_group[i+2][l]->X0()[2] };
                              double fD12[3] = { ls_group[i+2][l]->X1()[0], ls_group[i+2][l]->X1()[1], ls_group[i+2][l]->X1()[2] };

                              double x12[3];
                              MakeIntersection3D(fA12,fB12,fC12,fD12,x12);

			      double x12ca[3];
                              double l1_12[3]; double l2_12[3];
                              ClosestApproach(fA12,fB12,fC12,fD12,x12ca,l1_12,l2_12);

			      //average of three points (center of mass)
			      double ptavg_x2 = (_point01.first + _point02.first + _point12.first)/3. ;
			      double ptavg_y2 = (_point01.second + _point02.second + _point12.second)/3. ;

			      double _x[3];
			      double xsz[3];
			      double xca[3];

			      for (int i=0; i<3; i++){
			          _x[i] = (x01[i]+x02[i]+x12[i])/3.;
				  xsz[i] = (x01sz[i]+x02sz[i]+x12sz[i])/3.;
				  xca[i] = (x01ca[i]+x02ca[i]+x12ca[i])/3.;
			      }

			     //double ptavg_x2 = point12.first; 
		             //double ptavg_y2 = point12.second;
			      
			      pdd _point2 = std::make_pair(ptavg_x2, ptavg_y2);
				
			      //make point vector (x,y,z)
                              double x[3] = {_point2.first,_point2.second,ls_group[i][j]->X0()[2]};
            	              //z-component for .X0() and .X1() should be the same
                   
			      //check		
			      std::cout<<"...................STATION "<<st<<"......................"<<std::endl;
                              //std::cout<<"PREVIOUS...x: "<<x[0]<<"   y: "<<x[1]<<"   z: "<<x[2]<<std::endl;
			      //std::cout<<"SZ CHECK...x: "<<xsz[0]<<"   y: "<<xsz[1]<<"   z: "<<xsz[2]<<std::endl;
                              //std::cout<<"NEW........x: "<<_x[0]<<"   y: "<<_x[1]<<"   z: "<<_x[2]<<std::endl;
                              std::cout<<"CA CHECK...x: "<<xca[0]<<"   y: "<<xca[1]<<"   z: "<<xca[2]<<std::endl;
                              std::cout<<".................................................."<<std::endl;

                              //set SpacePoint object
                   	      sp.SetX(x);	 
	
		              //check stations 
		              if (cl_group[i+1][k]->Station() == st && cl_group[i+2][l]->Station() == st){}
                              else std::cout<<"XYU: Stations do not match..."<<std::endl;    
			      //reverse the logic (if this or this) + no else?

			      sp.SetStation(st);
                              spv.push_back(sp);
                              //add to unique pointer vector
                              spacepointv->push_back(sp);
                          
                              if (st==2) spdist2->Fill(x[0],x[1]);
                              if (st==3) spdist3->Fill(x[0],x[1]);
			      if (st==5) spdist5->Fill(x[0],x[1]);
			      if (st==6) spdist6->Fill(x[0],x[1]);
			      xzdist->Fill(x[2],x[0]);
                              yzdist->Fill(x[2],x[1]);
			      if (fEvtNum < 8000){
				 //xzdist_evt[fEvtNum]->Fill(x[2],x[0]);
				 xvec[st] = xca[0];
				 yvec[st] = xca[1];
                                 zvec[st] = xca[2];
		              }
			  }
                      }
		  }
	          i += 2;	
	       }
	   }

///
/*
	   if (point.first == FLT_MAX && point.second==FLT_MAX){
              std::cout << "The given lines AB and CD are parallel.\n";
           }
 
           else std::cout << "The line segments intersect at (" << point.first 
	         	  << "," << point.second << ")" << std::endl;
*/
////	
	
	}

	spv.clear();
	stripv.clear();
	clusters.clear();
        clustMap.clear();

	if (fEvtNum < 8000){
           if (!ssdHitH->empty() && goodEvent == true) {
		//std::cout<<"fEvtNum good: "<<fEvtNum<<std::endl;
//	      std::cout<<"Handle size: "<<ssdHitH->size()<<std::endl;
              for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
                  const sim::SSDHit& ssdhit = (*ssdHitH)[idx];
		  //std::cout<<"PID: "<<ssdhit.GetPId()<<std::endl;

		  xvec_sim.push_back(ssdhit.GetX());
		  yvec_sim.push_back(ssdhit.GetY());
		  zvec_sim.push_back(ssdhit.GetZ());
//std::cout<<"getting ssdhit"<<std::endl;
	          xvec_sim_pair.push_back(std::pair<double, int>(ssdhit.GetX(),ssdhit.GetStation()));
                  yvec_sim_pair.push_back(std::pair<double, int>(ssdhit.GetY(),ssdhit.GetStation()));
                  zvec_sim_pair.push_back(std::pair<double, int>(ssdhit.GetZ(),ssdhit.GetStation()));
		  //std::cout<<"Handle size: "<<ssdHitH->size()<<std::endl; 
                  //int plane_sim = ssdhit->GetPlane(); 
//std::cout<<"got ssdhit"<<std::endl;
		  //xvec_sim[plane_sim] = ssdhit->GetX();
		  //yvec_sim[plane_sim] = ssdhit->GetY();
		  //zvec_sim[plane_sim] = ssdhit->GetZ();
              }
	   }
      } 
    //double xsp[8];
    //double ysp[8];
    //double zsp[8];
    if (goodEvent == true){
       //int j =0;
       //while (j<8){
	//std::cout<<"xvec_sim_pair.size() = "<<xvec_sim_pair.size()<<std::endl;
       for (int j=0; j<8; j++){
       double xsum = 0.;
       double ysum = 0.;
       double zsum = 0.;
       int xc=0; int yc=0; int zc=0;

       //for (int j=0; j<8; j++){
	  for (size_t i=0; i<xvec_sim_pair.size(); i++){
	      if (j == xvec_sim_pair[i].second) {
	         xsum += xvec_sim_pair[i].first;
                 ysum += yvec_sim_pair[i].first;
                 zsum += zvec_sim_pair[i].first;
		 xc++; yc++; zc++;
	//	std::cout<<"station "<<j<<std::endl;
	      }
	  }
        //  j++;
       xsp[j] = xsum/(double)xc;
       ysp[j] = ysum/(double)yc;
       zsp[j] = zsum/(double)zc;
       //std::cout<<"sim vector: "<<xsp[j]<<","<<ysp[j]<<","<<zsp[j]<<std::endl; 
     }
   }
/*    double xsum[8]; double ysum[8]; double zsum[8];
    int xc[8]; int yc[8]; int zc[8];
    std::fill( xsum, xsum + 8, 0. );
    std::fill( xc, xc + 8, 1 );
    std::fill( ysum, ysum + 8, 0. );
    std::fill( yc, yc + 8, 1 );
    std::fill( zsum, zsum + 8, 0. );
    std::fill( zc, zc + 8, 1 );

for (int rsta = 0; rsta<8; rsta++){

//    std::cout<<"Size of sim vector is "<<xvec_sim.size()<<std::endl;
//    for (size_t j=0; j<xvec_sim.size(); j++){
//	std::cout<<"sim vector: "<<xvec_sim[j]<<","<<yvec_sim[j]<<","<<zvec_sim[j]<<std::endl;
//	}
//    for (size_t j=0; j<xvec_sim.size(); j++){
//	std::cout<<"j beg is "<<j<<std::endl;
        xsum[rsta] += xvec_sim[j];
        ysum[rsta] += yvec_sim[j];
        zsum[rsta] += zvec_sim[j];
        int counter = 1;
	for (size_t i=1; i<zvec_sim.size(); i++){
        //while (abs(zvec_sim[j] - zvec_sim[j+i]) < 20){
	     if(abs(zvec_sim[j] - zvec_sim[j+i]) < 10){
           counter++;
           xc[rsta]++; yc[rsta]++; zc[rsta]++;  
           xsum[rsta] += xvec_sim[j+i];
           ysum[rsta] += yvec_sim[j+i];
           zsum[rsta] += zvec_sim[j+i];
	     }   
          //i+       }
        }
        j+=counter;
 //       xsp[rsta] = xsum/(double)counter;
 //       ysp[rsta] = ysum/(double)counter;
 //       zsp[rsta] = zsum/(double)counter;
//        std::cout<<"(SIM) STATION "<<rsta<<std::endl;
//        std::cout<<"("<<xsp[rsta]<<","<<ysp[rsta]<<","<<zsp[rsta]<<")"<<std::endl;
        rsta++;
//	std::cout<<"j end is "<<j<<std::endl;
    }
    xsp[rsta] = xsum[rsta]/(double)xc[rsta];
    ysp[rsta] = ysum[rsta]/(double)yc[rsta];
    zsp[rsta] = zsum[rsta]/(double)zc[rsta];    
    std::cout<<"(SIM) STATION "<<rsta<<std::endl;
    std::cout<<"("<<xsp[rsta]<<","<<ysp[rsta]<<","<<zsp[rsta]<<")"<<std::endl;
}
*/	
//	}
      if (fEvtNum < 100){
           art::ServiceHandle<art::TFileService> tfs;
           char *Ghevt = new char[12];
	   char *Gxzsim = new char[11];
	   char *Gyzsim = new char[11];

	   //if (zvec[0] != 999 && zvec[1] != 999 && zvec[2] != 999 && zvec[3] != 999 && zvec[4] != 999 && zvec[5] != 999 && zvec[6] != 999 && zvec[7] != 999){

              //char *Gxyz = new char[12];

     	      //sprintf(Gxyz,"GMULT_xyze%d",fEvtNum);
	      //size_t sz = xvec_sim.size();

	      //GMULT_xyz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gxyz,Gxyz);
	      //GSIM_xyz[fEvtNum] = tfs->make<TGraph2D>(sz,&xvec_sim[0],&yvec_sim[0],&zvec_sim[0]);
              //Gxyzdist_evt[fEvtNum] = tfs->make<TGraph2D>(8,xvec,yvec,zvec);

              //GMULT_xyz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
              //GMULT_xyz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
              //GSIM_xyz[fEvtNum]->SetMarkerStyle(3);
              //GSIM_xyz[fEvtNum]->SetMarkerSize(2);
              //GSIM_xyz[fEvtNum]->SetMarkerColor(kRed);
	      //Gxzdist_evt[fEvtNum]->SetMarkerStyle(3);
              //Gxyzdist_evt[fEvtNum]->SetMarkerSize(2);
	   //}
           //sprintf(Ghevt,"Gspdist_e%d",fEvtNum);
           if (xvec[0] != 999 && xvec[1] != 999 && xvec[2] != 999 && xvec[3] != 999 && xvec[4] != 999 && xvec[5] != 999 && xvec[6] != 999 && xvec[7] != 999){
              sprintf(Ghevt,"Gxzdist_e%d",fEvtNum);
	      sprintf(Gxzsim,"GMULT_xze%d",fEvtNum);		
	      //sprintf(Gyzsim,"GMULT_yze%d",fEvtNum);

              //size_t graphsize = xvec_sim.size();
	      size_t graphsize = 8; //xsp.size();
              //double xvec_sim_arr[graphsize];
	      //for (size_t i=0; i<graphsize; i++){ 
		//  xvec_sim_arr[i] =  xvec_sim[i];
              //}
	      //Gxzdist_evt[fEvtNum] = tfs->makeAndRegister<TGraph>(Ghevt,Ghevt,graphsize,&zvec_sim[0],&xvec_sim[0]); 
	      //^^ get array by the address of the first element of the vector

              GMULT_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gxzsim,Gxzsim);

	      //get array by the address of the first element of the vector
	      //GSIM[fEvtNum] = tfs->makeAndRegister<TGraph>(Ghevt,Ghevt,graphsize,&zvec_sim[0],&xvec_sim[0]);
              //Gxzdist_evt[fEvtNum] = tfs->makeAndRegister<TGraph>(Ghevt,Ghevt,8,zvec,xvec);
//	      GSIM_xz[fEvtNum] = tfs->make<TGraph>(graphsize,&zvec_sim[0],&xvec_sim[0]);
	      GSIM_xz[fEvtNum] = tfs->make<TGraph>(graphsize,&zsp[0],&xsp[0]);
              Gxzdist_evt[fEvtNum] = tfs->make<TGraph>(8,zvec,xvec);			

              GMULT_xz[fEvtNum]->Add(Gxzdist_evt[fEvtNum]);
              GMULT_xz[fEvtNum]->Add(GSIM_xz[fEvtNum]);
	      //GMULT_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Ghsim,Ghsim);

	      GMULT_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
              GMULT_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
              GSIM_xz[fEvtNum]->SetMarkerStyle(3);
              GSIM_xz[fEvtNum]->SetMarkerSize(2);
	      GSIM_xz[fEvtNum]->SetMarkerColor(kRed);
	      GSIM_xz[fEvtNum]->SetLineColor(kRed);

	      //Gxzdist_evt[fEvtNum] = tfs->makeAndRegister<TGraph>(Ghevt,Ghevt,8,zvec,xvec);
              //Gxzdist_evt[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
              //Gxzdist_evt[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
	      Gxzdist_evt[fEvtNum]->SetMarkerStyle(3);
              Gxzdist_evt[fEvtNum]->SetMarkerSize(2);

	      //GMULT_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Ghsim,Ghsim);
	      //GMULT_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Ghsim,Ghsim);
              //GMULT_xz[fEvtNum]->Add(Gxzdist_evt[fEvtNum]);
	      //GMULT_xz[fEvtNum]->Add(GSIM[fEvtNum]);
	   }
           if (yvec[0] != 999 && yvec[1] != 999 && yvec[2] != 999 && yvec[3] != 999 && yvec[4] != 999 && yvec[5] != 999 && yvec[6] != 999 && yvec[7] != 999){
	      sprintf(Gyzsim,"GMULT_yze%d",fEvtNum);
	      sprintf(Ghevt,"Gyzdist_e%d",fEvtNum);
	      //size_t graphsize = yvec_sim.size();
	      size_t graphsize = 8; //ysp.size();

	      GMULT_yz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gyzsim,Gyzsim);
//	      GSIM_yz[fEvtNum] = tfs->make<TGraph>(graphsize,&zvec_sim[0],&yvec_sim[0]);
	       GSIM_yz[fEvtNum] = tfs->make<TGraph>(graphsize,&zsp[0],&ysp[0]);
              Gyzdist_evt[fEvtNum] = tfs->make<TGraph>(8,zvec,yvec);
              GMULT_yz[fEvtNum]->Add(Gyzdist_evt[fEvtNum]);
              GMULT_yz[fEvtNum]->Add(GSIM_yz[fEvtNum]);

	      GMULT_yz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
              GMULT_yz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
              GSIM_yz[fEvtNum]->SetMarkerStyle(3);
              GSIM_yz[fEvtNum]->SetMarkerSize(2);
              GSIM_yz[fEvtNum]->SetMarkerColor(kRed);
	      GSIM_yz[fEvtNum]->SetLineColor(kRed);

	      Gyzdist_evt[fEvtNum]->SetMarkerStyle(3);
              Gyzdist_evt[fEvtNum]->SetMarkerSize(2);

	      //Gyzdist_evt[fEvtNum] = tfs->makeAndRegister<TGraph>(Ghevt,Ghevt,8,zvec,yvec);
              //Gyzdist_evt[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
              //Gyzdist_evt[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
              //Gyzdist_evt[fEvtNum]->SetMarkerStyle(3);
              //Gyzdist_evt[fEvtNum]->SetMarkerSize(2);
	   }	
        }
      }
    } //try
    catch(...) {

    }
/*
    double xsp[8];
    double ysp[8];
    double zsp[8];
    int rsta = 0;
    for (size_t j=0; j<xvec_sim.size(); j++){
        double xsum = xvec_sim[j];
	double ysum = yvec_sim[j];
	double zsum = zvec_sim[j];
	int counter = 1;
	while (abs(zvec_sim[j] - zvec_sim[j+1]) < 20){
	   counter++;
	   xsum += xvec_sim[j+1];
	   ysum += yvec_sim[j+1];
	   zsum += zvec_sim[j+1];
	   j++;	
        }
	xsp[rsta] = xsum/(double)counter;
        ysp[rsta] = ysum/(double)counter;
	zsp[rsta] = zsum/(double)counter;
        std::cout<<"("<<xsp[rsta]<<","<<ysp[rsta]<<","<<zsp[rsta]<<")"<<std::endl;
	rsta++;
    }
*/
if (goodEvent == true){
    for (int i=0; i<8; i++){
        if (xsp[i] == 999) std::cout<<"OHHHHH "<<i<<std::endl;
	if (ysp[i] == 999) std::cout<<"OHHHHH "<<i<<std::endl;
        if (zsp[i] == 999) std::cout<<"OHHHHH "<<i<<std::endl;
    }

    for (int i=0; i<8; i++){
	double xres_re = xvec[i] - xsp[i];
	double yres_re = yvec[i] - ysp[i];
        res_x_re[i]->Fill(xres_re);
	res_y_re[i]->Fill(yres_re);
    }
	  
    for (int i=0; i<8; i++){
	for (size_t j=0; j<xvec_sim.size(); j++){
            if ( abs(zvec[i] - zvec_sim[j]) < 20){
//std::cout<<"Event is: "<<fEvtNum<<std::endl;
               //std::cout<<"z pos diff:"<<abs(zvec[i] - zvec_sim[j])<<std::endl;
               double res = xvec[i] - xvec_sim[j];
               res_x[i]->Fill(res);  
	    }     
	}
	for (size_t j=0; j<yvec_sim.size(); j++){
            if ( abs(zvec[i] - zvec_sim[j]) < 20){
               double res = yvec[i] - yvec_sim[j];
               res_y[i]->Fill(res);
            }
        }
    }
}
/*    for (size_t i=0; i<xvec_re.size(); i++){
        for (size_t j=0; j<xvec_sim.size(); j++){
            if ( abs(zvec_re[i] - zvec_sim[j]) < 0.5){
	       double res_re = xvec_re[i] - xvec_sim[j];
	       res_x_re[i]->Fill(res_re);
	    }
	}
	for (size_t j=0; j<yvec_sim.size(); j++){
            if ( abs(zvec_re[i] - zvec_sim[j]) < 0.5){
               double res_re = yvec_re[i] - yvec_sim[j];
               res_y_re[i]->Fill(res_re);
            }
        }
    }
*/
    for (int i=0; i<8; i++){
        xvec[i] = 999; xsp[i] = 999;
	yvec[i] = 999; ysp[i] = 999;
	zvec[i] = 999; zsp[i] = 999;
    }
    xvec_sim.clear();
    yvec_sim.clear();
    zvec_sim.clear();
	xvec_re.clear();
	yvec_re.clear();
	zvec_re.clear();
    xvec_sim_pair.clear();
    yvec_sim_pair.clear();
    zvec_sim_pair.clear();


  } //want plots
  evt.put(std::move(spacepointv));

  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrack)
