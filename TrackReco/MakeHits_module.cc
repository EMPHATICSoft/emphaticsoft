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

///package to illustrate how to write modules
namespace emph {
  ///
  class MakeHits : public art::EDProducer {
  public:
    explicit MakeHits(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeHits() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

    void MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls);
    double dotProduct(double a[3], double b[3]); 
    void crossProduct(double a[3], double b[3], double c[3]);
    void ClosestApproach(double A[3],double B[3], double C[3], double D[3], double F[3], double l1[3], double l2[3]);

  private:
  
    TTree*      spacepoint;
    int run,subrun,event;
    int         fEvtNum;

    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> stripv;
    std::vector<rb::SpacePoint> spv;

    std::map<std::pair<int, int>, int> clustMap;

    bool        fMakePlots;
    int 	goodclust = 0;
    int 	fTotClust;
    int         badclust = 0; 
    size_t      nPlanes;

    //fcl parameters
    bool        fCheckClusters;     //Check clusters for event 

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

    TH2F* xzdist;
    TH2F* yzdist;

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

    //simulated hits
    double xsim[8] = {999,999,999,999,999,999,999,999};
    double ysim[8] = {999,999,999,999,999,999,999,999};
    double zsim[8] = {999,999,999,999,999,999,999,999};

    //reconstructed hits
    double xvec[8] = {999,999,999,999,999,999,999,999};
    double yvec[8] = {999,999,999,999,999,999,999,999};
    double zvec[8] = {999,999,999,999,999,999,999,999};

    std::vector<double> xvec_sim;
    std::vector<double> yvec_sim;
    std::vector<double> zvec_sim;

    std::vector<std::pair<double, int>> xvec_sim_pair;
    std::vector<std::pair<double, int>> yvec_sim_pair;
    std::vector<std::pair<double, int>> zvec_sim_pair;

  };

  //.......................................................................
  
  emph::MakeHits::MakeHits(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")),
    fDetGeoMap(NULL)
  {
    this->produces< std::vector<rb::SpacePoint> >();
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
  
  void MakeHits::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    /*
    art::ServiceHandle<emph::cmap::ChannelMapService> fChannelMap;

    for (int fer=0; fer<10; ++fer){
      for (int mod=0; mod<6; ++mod){
        emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
        if (!fChannelMap->CMap()->IsValidEChan(echan)) continue;
          emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);

        const emph::geo::SSDStation* st = emgeo->GetSSDStation(dchan.Station());
	const emph::geo::Plane      *pl = st->GetPlane(dchan.Plane());       
        const emph::geo::Detector   *sd = pl->SSD(dchan.Channel());

      }
    }
    */
  }

  //......................................................................
   
  void emph::MakeHits::beginJob()
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

    xzdist = tfs->make<TH2F>("xzdist","xzdist",2050,-50.,2000.,1000,-50.,50.); 
    yzdist = tfs->make<TH2F>("yzdist","yzdist",2050,-50.,2000.,1000,-50.,50.);

    char *hresx = new char[7];
    char *hresy = new char[7];

    //residuals
    for (int i=0; i<8; i++){
	sprintf(hresx,"res_x_%d",i);
        res_x[i] = tfs->make<TH1F>(hresx,hresx,100,-0.1,0.1);	
        sprintf(hresy,"res_y_%d",i);
        res_y[i] = tfs->make<TH1F>(hresy,hresy,100,-0.1,0.1);
    }
  }
 
  //......................................................................
  
  void emph::MakeHits::endJob()
  {
       std::cout<<"Number of clusters with one cluster per sensor: "<<goodclust<<std::endl;
       std::cout<<"Number of available clusters: "<<badclust+goodclust<<std::endl;
  }

  //......................................................................

  void emph::MakeHits::MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
  {
    if (!fDetGeoMap) fDetGeoMap = new emph::dgmap::DetGeoMap();

    //if(fDetGeoMap->SSDClusterToLineSegment(cl, ls)){

    //}
    fDetGeoMap->SSDClusterToLineSegment(cl, ls);
  }

  //......................................................................

  void emph::MakeHits::ClosestApproach(double A[3],double B[3], double C[3], double D[3], double F[3], double l1[3], double l2[3]){

        double r12 = (B[0] - A[0])*(B[0] - A[0]) + (B[1] - A[1])*(B[1] - A[1]) + (B[2] - A[2])*(B[2] - A[2]);
	double r22 = (D[0] - C[0])*(D[0] - C[0]) + (D[1] - C[1])*(D[1] - C[1]) + (D[2] - C[2])*(D[2] - C[2]);

	double d4321 = (D[0] - C[0])*(B[0] - A[0]) + (D[1] - C[1])*(B[1] - A[1]) + (D[2] - C[2])*(B[2] - A[2]);
        double d3121 = (C[0] - A[0])*(B[0] - A[0]) + (C[1] - A[1])*(B[1] - A[1]) + (C[2] - A[2])*(B[2] - A[2]);
	double d4331 = (D[0] - C[0])*(C[0] - A[0]) + (D[1] - C[1])*(C[1] - A[1]) + (D[2] - C[2])*(C[2] - A[2]);

	double s = (-d4321*d4331 + d3121*r22) / (r12*r22 - d4321*d4321);
	double t = (d4321*d3121 - d4331*r12) / (r12*r22 - d4321*d4321);

	double L1[3]; double L2[3];
	if ( s >= 0 && s <= 1 && t >=0 && t <= 1){
	   //std::cout<<"Closest approach all good :)"<<std::endl;
	   for (int i=0; i<3; i++){
	       L1[i] = A[i] + s*(B[i] - A[i]);
	       L2[i] = C[i] + t*(D[i] - C[i]);	    
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
	   std::cout<<"A: ("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<std::endl;
	   std::cout<<"B: ("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<std::endl;
	   std::cout<<"C: ("<<C[0]<<","<<C[1]<<","<<C[2]<<")"<<std::endl;
	   std::cout<<"D: ("<<D[0]<<","<<D[1]<<","<<D[2]<<")"<<std::endl;
	   std::cout<<"How do line segments AB and CD look if you draw them in the beam view (i.e. the same plane)?"<<std::endl;
	   std::cout<<"And don't worry! A hit is still created, but the line segments (probably) come close to intersecting...but don't"<<std::endl;
	   //std::cout<<"s: "<<s<<std::endl;
	   //std::cout<<"t: "<<t<<std::endl;

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

  double emph::MakeHits::dotProduct(double a[3], double b[3]){
     double product = 0;
     for (int i=0; i<3; i++){
         product = product + a[i] * b[i];
     }
     return product;
  }

  //......................................................................
  
  void emph::MakeHits::crossProduct(double a[3], double b[3], double c[3]){
     c[0] = a[1] * b[2] - a[2] * b[1];
     c[1] = -(a[0] * b[2] - a[2] * b[0]);
     c[2] = a[0] * b[1] - a[1] * b[0];
  }
 
  //......................................................................
  void emph::MakeHits::produce(art::Event& evt)
  {

    //    art::ServiceHandle<emph::geo::GeometryService> geo;
    //    auto emgeo = geo->Geo();

    std::unique_ptr< std::vector<rb::SpacePoint> > spacepointv(new std::vector<rb::SpacePoint>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    //debug
    //if(fEvtNum==877) fMakePlots = true; //two clusters in a (sta,sen)=(2,0)
    //if(fEvtNum==2826) fMakePlots = true; //good event one cluster per plane 

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

    try {
      evt.getByLabel(fClusterLabel, clustH);
      if (!clustH->empty()){
         for (size_t idx=0; idx < clustH->size(); ++idx) {
              const rb::SSDCluster& clust = (*clustH)[idx];
	      ++clustMap[std::pair<int,int>(clust.Station(),clust.Plane())]; //clust.Sensor())];
	      clusters.push_back(&clust); //push back a  pointer
	  }

          //ONE CLUSTER PER PLANE
          //If there are more clusters than sensors, skip event
	  if (clusters.size()==20){ //what should this be...
	     for (auto i : clustMap){
	         //std::cout<<"cluster (sta,plane)..."<<i.first.first<<","<<i.first.second<<std::endl;
		 if (i.second != 1){goodEvent = false; break;} //std::cout<<"More than one cluster per plane :("<<std::endl;}        
	         else goodEvent = true;
	     }
	     if (goodEvent==true) {goodclust++;} //std::cout<<"First test passed :3 @ Event "<<fEvtNum<<std::endl;}
	     else {badclust++;} // std::cout<<                 "Naurrrrrrr        :("<<std::endl;}
          }
	  else badclust++;
	  
	  std::vector<std::vector<const rb::SSDCluster*> > cl_group;
	  std::vector<std::vector<const rb::LineSegment*> > ls_group;
	  cl_group.resize(nPlanes);
	  ls_group.resize(nPlanes);

         for (size_t i=0; i<clusters.size(); i++){
	   int plane = clusters[i]->Plane();
	   
	   //group clusters according to plane
	   //within each station, do every combination
	   cl_group[plane].push_back(clusters[i]);
	 }
	 
	  //ANY CLUSTER
          /*bool goodEvent = true;
          for (size_t i=0; i<nPlanes; i++){
              if (cl_group[i].size() == 0){ goodEvent = false; break; }
          }
          */

	  rb::LineSegment strip;
          if (goodEvent==true && clusters.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
		stripv.push_back(strip);
		MakeSegment(*clusters[i],stripv[i]);
		
	     }
          }

	  rb::SpacePoint sp;

	  if (goodEvent == true && stripv.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
                 int plane = clusters[i]->Plane();
		 ls_group[plane].push_back(&stripv[i]);
             }
	     
	     for (size_t i=0; i<nPlanes; i++){
	         //station 0,1,4,7
	         if (i==0 || i==2 || i==10 || i==18){ //|| i==4 || i==7 || i==10 || i==18){
	            for (size_t j=0; j<ls_group[i].size(); j++){
		        st = cl_group[i][j]->Station();
	                for (size_t k=0; k<ls_group[i+1].size(); k++){
			    double fA[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                            double fB[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                            double fC[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                            double fD[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };

			    double x[3];
	                    double l1[3]; double l2[3];
			    ClosestApproach(fA,fB,fC,fD,x,l1,l2);

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
		            xzdist->Fill(x[2],x[0]);
			    yzdist->Fill(x[2],x[1]);

			    //check for plots
			    if (fEvtNum < 8000){
			       xvec[st] = x[0];
			       yvec[st] = x[1];
			       zvec[st] = x[2];
		            }
		        }
	            }
	          i++; //skip following sensor
	       }
	       //station 2,3,5,6
	       if (i==4 || i==7 || i==12 || i==15){
	          for (size_t j=0; j<ls_group[i].size(); j++){
		      st = cl_group[i][j]->Station();
                      for (size_t k=0; k<ls_group[i+1].size(); k++){
			  for (size_t l=0; l<ls_group[i+2].size(); l++){ 
			      double fA01[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                              double fB01[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                              double fC01[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                              double fD01[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };

			      double x01[3];
			      double l1_01[3]; double l2_01[3];
                              ClosestApproach(fA01,fB01,fC01,fD01,x01,l1_01,l2_01);

			      double fA02[3] = { ls_group[i][j]->X0()[0], ls_group[i][j]->X0()[1], ls_group[i][j]->X0()[2] };
                              double fB02[3] = { ls_group[i][j]->X1()[0], ls_group[i][j]->X1()[1], ls_group[i][j]->X1()[2] };
                              double fC02[3] = { ls_group[i+2][l]->X0()[0], ls_group[i+2][l]->X0()[1], ls_group[i+2][l]->X0()[2] };
                              double fD02[3] = { ls_group[i+2][l]->X1()[0], ls_group[i+2][l]->X1()[1], ls_group[i+2][l]->X1()[2] };

                              double x02[3];
                              double l1_02[3]; double l2_02[3];
                              ClosestApproach(fA02,fB02,fC02,fD02,x02,l1_02,l2_02);

			      double fA12[3] = { ls_group[i+1][k]->X0()[0], ls_group[i+1][k]->X0()[1], ls_group[i+1][k]->X0()[2] };
                              double fB12[3] = { ls_group[i+1][k]->X1()[0], ls_group[i+1][k]->X1()[1], ls_group[i+1][k]->X1()[2] };
                              double fC12[3] = { ls_group[i+2][l]->X0()[0], ls_group[i+2][l]->X0()[1], ls_group[i+2][l]->X0()[2] };
                              double fD12[3] = { ls_group[i+2][l]->X1()[0], ls_group[i+2][l]->X1()[1], ls_group[i+2][l]->X1()[2] };

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

			      //check for plots
			      if (fEvtNum < 8000){
				 xvec[st] = x[0];
				 yvec[st] = x[1];
                                 zvec[st] = x[2];
		              }
			  }
                      }
		  }
	          i += 2;	
	       }
	   }
	}

	spv.clear();
	stripv.clear();
	clusters.clear();
        clustMap.clear();

        //check with simulation
	if (fEvtNum < 8000){
           if (!ssdHitH->empty() && goodEvent == true) {
              for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
                  const sim::SSDHit& ssdhit = (*ssdHitH)[idx];

		  xvec_sim.push_back(ssdhit.GetX());
		  yvec_sim.push_back(ssdhit.GetY());
		  zvec_sim.push_back(ssdhit.GetZ());
	          xvec_sim_pair.push_back(std::pair<double, int>(ssdhit.GetX(),ssdhit.GetStation()));
                  yvec_sim_pair.push_back(std::pair<double, int>(ssdhit.GetY(),ssdhit.GetStation()));
                  zvec_sim_pair.push_back(std::pair<double, int>(ssdhit.GetZ(),ssdhit.GetStation()));
              }
	   }
        } 

        if (goodEvent == true){
           for (int j=0; j<8; j++){
               double xsum = 0.; double ysum = 0.; double zsum = 0.;
               int xc=0; int yc=0; int zc=0;

	       for (size_t i=0; i<xvec_sim_pair.size(); i++){
	           if (j == xvec_sim_pair[i].second) {
	              xsum += xvec_sim_pair[i].first;
                      ysum += yvec_sim_pair[i].first;
                      zsum += zvec_sim_pair[i].first;
		      xc++; yc++; zc++;
	           }
	       }
               xsim[j] = xsum/(double)xc;
               ysim[j] = ysum/(double)yc;
               zsim[j] = zsum/(double)zc;
            }
        }
        if (fEvtNum < 100){
           art::ServiceHandle<art::TFileService> tfs;
           char *Ghevt = new char[12];
	   char *Gxzsim = new char[11];
	   char *Gyzsim = new char[11];

           if (xvec[0] != 999 && xvec[1] != 999 && xvec[2] != 999 && xvec[3] != 999 && xvec[4] != 999 && xvec[5] != 999 && xvec[6] != 999 && xvec[7] != 999){
              sprintf(Ghevt,"Gxzdist_e%d",fEvtNum);
	      sprintf(Gxzsim,"GMULT_xze%d",fEvtNum);		
	      size_t graphsize = 8; //xsim.size();

              GMULT_xz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gxzsim,Gxzsim);

	      //get array by the address of the first element of the vector
	      GSIM_xz[fEvtNum] = tfs->make<TGraph>(graphsize,&zsim[0],&xsim[0]);
              Gxzdist_evt[fEvtNum] = tfs->make<TGraph>(8,zvec,xvec);			

              GMULT_xz[fEvtNum]->Add(Gxzdist_evt[fEvtNum]);
              GMULT_xz[fEvtNum]->Add(GSIM_xz[fEvtNum]);

	      GMULT_xz[fEvtNum]->GetYaxis()->SetRangeUser(-50,50);
              GMULT_xz[fEvtNum]->GetXaxis()->SetLimits(-50,1900);
              GSIM_xz[fEvtNum]->SetMarkerStyle(3);
              GSIM_xz[fEvtNum]->SetMarkerSize(2);
	      GSIM_xz[fEvtNum]->SetMarkerColor(kRed);
	      GSIM_xz[fEvtNum]->SetLineColor(kRed);

	      Gxzdist_evt[fEvtNum]->SetMarkerStyle(3);
              Gxzdist_evt[fEvtNum]->SetMarkerSize(2);
	   }
           if (yvec[0] != 999 && yvec[1] != 999 && yvec[2] != 999 && yvec[3] != 999 && yvec[4] != 999 && yvec[5] != 999 && yvec[6] != 999 && yvec[7] != 999){
	      sprintf(Gyzsim,"GMULT_yze%d",fEvtNum);
	      sprintf(Ghevt,"Gyzdist_e%d",fEvtNum);
	      size_t graphsize = 8; //ysim.size();

	      GMULT_yz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gyzsim,Gyzsim);
	      GSIM_yz[fEvtNum] = tfs->make<TGraph>(graphsize,&zsim[0],&ysim[0]);
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
	   }	
        }
      }
    } //try
    catch(...) {

    }
    if (goodEvent == true){
       for (int i=0; i<8; i++){
           if (xsim[i] == 999) std::cout<<"A hit is missing..."<<i<<std::endl;
           if (ysim[i] == 999) std::cout<<"A hit is missing..."<<i<<std::endl;
           if (zsim[i] == 999) std::cout<<"A hit is missing..."<<i<<std::endl;
       }

       for (int i=0; i<8; i++){
           double xres = xvec[i] - xsim[i];
	   double yres = yvec[i] - ysim[i];
           res_x[i]->Fill(xres);
	   res_y[i]->Fill(yres);
        }	  
    }
    for (int i=0; i<8; i++){
        xvec[i] = 999; xsim[i] = 999;
	yvec[i] = 999; ysim[i] = 999;
	zvec[i] = 999; zsim[i] = 999;
    }
    xvec_sim.clear();
    yvec_sim.clear();
    zvec_sim.clear();
    xvec_sim_pair.clear();
    yvec_sim_pair.clear();
    zvec_sim_pair.clear();


  } //want plots
  evt.put(std::move(spacepointv));

  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeHits)
