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
//#include "TFile.h"
//#include "TH1F.h"
//#include "TH2F.h"
#include "TH3F.h"
//#include "TVector3.h"
#include "TTree.h"

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
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/Geometry.h"
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
    //~MakeTrack();
    
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
    
  private:
   
    emph::cmap::ChannelMap* fChannelMap;
    runhist::RunHistory* fRunHistory;
    emph::geo::Geometry *emgeo;
    TTree*      spacepoint;
    int run,subrun,event;
    int         fEvtNum;
	//std::vector<const rb::SSDCluster> clusters;

//MAYBE THIS art::PtrVector<const rb::SSDCluster> clusters;
    std::vector<const rb::SSDCluster*> clusters;
    //std::vector<rb::LineSegment> strips;
    std::vector<rb::LineSegment> stripv;
    std::vector<rb::SpacePoint> spv;

    std::map<std::pair<int, int>, int> clustMap; 
    std::map<std::pair<int, rb::LineSegment&>, int> clustStripMap;
    //std::vector<std::pair<int,rb::LineSegment&>> clustsegpair;
    std::vector<std::pair<const rb::SSDCluster&, rb::LineSegment&>> clustsegpair;
//std::unique_ptr< std::vector<std::pair<const rb::SSDCluster&,rb::LineSegment&>> > clustsegpair;
    //std::map<const std::pair<int, int>, emph::geo::sensorView> clustMap;
    //std::map<std::pair<const emph::geo::SSDStation, const emph::geo::Detector>, int> clustMap;
    std::map<std::pair<int, int>, std::pair<int, geo::sensorView> > planeViewMap;
    bool        fMakePlots;
    int 	goodclust = 0;
    int 	fTotClust;
    int         naur = 0; 
    //fcl parameters
    bool        fCheckClusters;     ///< Check clusters for event 

    TH3F*       fSpacePoint3D;

    emph::dgmap::DetGeoMap* fDetGeoMap;
  };

  //.......................................................................
  
  emph::MakeTrack::MakeTrack(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")),
    fDetGeoMap(NULL)
  {

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
    fChannelMap = new emph::cmap::ChannelMap();
    fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
  //auto emgeo = geo->Geo();
  //auto emcmap = cmap->CMap();
/*  for (int fer=0; fer<10; ++fer){
    for (int mod=0; mod<6; ++mod){
      emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
      if (!fChannelMap->IsValidEChan(echan)) continue;
        emph::cmap::DChannel dchan = cmap->DetChan(echan);

      const emph::geo::SSDStation &st = emgeo->GetSSDStation(dchan.Station());
      const emph::geo::Detector   &sd = st.GetSSD(dchan.Channel());

      planeViewMap[std::make_pair(dchan.Station(),dchan.Channel())] = std::make_pair(dchan.Plane(),sd.View());
    }
  }*/
  }
   
  void emph::MakeTrack::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");
    //fSpacePoint3D = tfs->make<TH3F>("fSpacePoint3D","3D Visual of Space Points",1000,0.,1000.,1000.,0.,1000.,2000,0.,2000.);
    //spacepoint->Branch("SpacePoint3D",&SpacePoint3D,"SpacePoint3D");
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

    //std::cout<<"In MakePoint"<<std::endl;

     //std::unique_ptr< std::vector<std::pair<int,rb::LineSegment&>> > clustsegpair(new std::vector<std::pair<int,rb::LineSegment&>>);

    //probably doesn't need to be an if statement?
    if(fDetGeoMap->SSDClusterToLineSegment(cl, ls)){
	//std::cout<<"Made line segment"<<std::endl;

	//std::cout<<"Cluster (sta,sen): "<<cl.Station()<<","<<cl.Sensor()<<std::endl;
	//clustsegpair.push_back(std::pair<const rb::SSDCluster&,rb::LineSegment&>(cl,ls));
        //std::cout<<"One segment (in makepoint)"<<std::endl;
        //std::cout<<"("<<ls.X0()[0]<<", "<<ls.X0()[1]<<", "<<ls.X0()[2]<<")"<<std::endl;
        //std::cout<<"("<<ls.X1()[0]<<", "<<ls.X1()[1]<<", "<<ls.X1()[2]<<")"<<std::endl;
/*
        if (cl.Station()==0){ //station is 0
        //shouldn't be the same as above!!!  
             std::cout<<"One segment (in clustsegpair)"<<std::endl;
             std::cout<<"("<<clustsegpair[0].second.X0()[0]<<", "<<clustsegpair[0].second.X0()[1]<<", "<<clustsegpair[0].second.X0()[2]<<")"<<std::endl;
             std::cout<<"("<<clustsegpair[0].second.X1()[0]<<", "<<clustsegpair[0].second.X1()[1]<<", "<<clustsegpair[0].second.X1()[2]<<")"<<std::endl;

        }
*/
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
  void emph::MakeTrack::produce(art::Event& evt)
  {
      //std::unique_ptr< std::vector<rb::SSDCluster> > clusters(new std::vector<rb::SSDCluster>);
   //clustsegpair.clear();

    std::unique_ptr< std::vector<rb::SpacePoint> > spacepointv(new std::vector<rb::SpacePoint>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    //debug
    //if(fEvtNum==877) fMakePlots = true; //two clusters in a (sta,sen)=(2,0)
    if(fEvtNum==2826) fMakePlots = true; //good event one cluster per plane 
    else fMakePlots = false;
    //fMakePlots = true;

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
    try {
      evt.getByLabel(fClusterLabel, clustH);
      if (!clustH->empty()){
         for (size_t idx=0; idx < clustH->size(); ++idx) {
	      //std::cout<<fEvtNum<<std::endl;
              const rb::SSDCluster& clust = (*clustH)[idx];
              //MAYBE THIS   art::Ptr<const rb::SSDCluster> clustRR(clustH,idx);

              //const emph::geo::SSDStation &st = emgeo->GetSSDStation(clust.Station());
              //const emph::geo::Detector &sd = st.GetSSD(clust.Sensor());
	      ++clustMap[std::pair<int,int>(clust.Station(),clust.Sensor())];

	      //std::cout<<"Station: "<<clust.Station()<<"...Sensor: "<<clust.Sensor()<<std::endl;

	      clusters.push_back(&clust); //push back a  pointer

              //MAYBE THIS   clusters.push_back(clustRR);
	      //clusters->push_back(&clust); //push back a  pointer
	  }
	  //std::cout<<"cluster vector size: "<<clusters.size()<<std::endl;

	  bool goodEvent = false; //true
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
	 
	  //std::cout<<"clusters.size(): "<<clusters.size()<<std::endl;
 
	  rb::LineSegment strip;
          if (goodEvent==true && clusters.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
	        std::cout<<"Cluster station is: "<<clusters[i]->Station()	<<std::endl;
		stripv.push_back(strip);
	        //clustsegpair.push_back(std::pair<const rb::SSDCluster&,rb::LineSegment&>(*clusters[i],stripv[i]));	
	        //MakePoint(clustsegpair[i].first,clustsegpair[i].second);
		MakePoint(*clusters[i],stripv[i]);
	     }
          }
 	  
	//if line segment populated, make a point		 
	if (goodEvent == true){ // && strips.size() > 0){
	   //stripvcheck
	   /*
	   for (size_t i=0; i<stripv.size(); i++){
	       std::cout<<"........"<<std::endl;
	       //std::cout<<"Cluster station is: "<<clustsegpair[i].first.Station()
               //           <<" and sensor is : "<<clustsegpair[i].first.Sensor()<<std::endl;
	       std::cout<<"("<<stripv[i].X0()[0]<<", "<<stripv[i].X0()[1]<<", "<<stripv[i].X0()[2]<<")"<<std::endl;
	       std::cout<<"("<<stripv[i].X1()[0]<<", "<<stripv[i].X1()[1]<<", "<<stripv[i].X1()[2]<<")"<<std::endl;
	   }
	   */

//	   for (size_t i=0; i<clustsegpair.size(); i++){
//	       std::cout<<"...checking pairs..."<<std::endl;
//	       std::cout<<"Cluster station: "<<clustsegpair[i].first.Station()<<std::endl;
//	       std::cout<<"("<<clustsegpair[i].second.X0()[0]<<", "<<clustsegpair[i].second.X0()[1]<<", "<<clustsegpair[i].second.X0()[2]<<")"<<std::endl;
//               std::cout<<"("<<clustsegpair[i].second.X1()[0]<<", "<<clustsegpair[i].second.X1()[1]<<", "<<clustsegpair[i].second.X1()[2]<<")"<<std::endl;
//         } 

	   //if (station 0)
	   std::vector< std::vector< const rb::SSDCluster*> > xy;
	   std::vector< std::vector< rb::LineSegment> > xy_stripv;
	   std::vector< std::vector< const rb::SSDCluster*> > xyu;
	   std::vector< std::vector< rb::LineSegment> > xyu_stripv;
	   //std::vector< std::vector<  rb::SSDCluster> > xy;
	   //std::vector< std::vector<  rb::SSDCluster> > xyu;

	   for (size_t i=1; i<clusters.size()-1; i++){
	   //group clusters into groups according to station
	   //probably not the smartest way
	   std::vector<const rb::SSDCluster*> tmp;
	   std::vector<rb::LineSegment> tmp_stripv;
		 //last station
	         if (clusters[i]->Station()==7){
                    tmp.push_back(clusters[i]);    tmp_stripv.push_back(stripv[i]);
                    tmp.push_back(clusters[i+1]);  tmp_stripv.push_back(stripv[i+1]); 
                    xy.push_back(tmp);             xy_stripv.push_back(tmp_stripv);
                 }
		 if (clusters[i]->Station()==clusters[i-1]->Station() && clusters[i]->Station()!=clusters[i+1]->Station()){ 
		    // || clusters[i]->Station()==clusters[i+1]->Station() && clusters[i]->Station()!=clusters[i-1]->Station()){
		 //pushback i and i-1 to xy
		    tmp.push_back(clusters[i-1]);  tmp_stripv.push_back(stripv[i-1]);
		    tmp.push_back(clusters[i]);    tmp_stripv.push_back(stripv[i]);
		    xy.push_back(tmp);             xy_stripv.push_back(tmp_stripv);
		 }
		 if (clusters[i]->Station()==clusters[i-1]->Station() && clusters[i]->Station()==clusters[i+1]->Station()){
		 //pusback i, i-1,i+1 to xyu i+2
		    tmp.push_back(clusters[i-1]);  tmp_stripv.push_back(stripv[i-1]);
                    tmp.push_back(clusters[i]);    tmp_stripv.push_back(stripv[i]);
		    tmp.push_back(clusters[i+1]);  tmp_stripv.push_back(stripv[i+1]);
                    xyu.push_back(tmp);            xyu_stripv.push_back(tmp_stripv);
		    i++;
                 }
	   tmp.clear();
	   }
	   
	   //xy and xyu check
/*	   std::cout<<"xy check"<<std::endl;
	   for (size_t i=0; i<xy.size(); i++){
           std::cout<<"i: "<<i<<std::endl;
		for (size_t j=0; j<xy[i].size(); j++){
	            std::cout<<"xy[i].size(): "<<xy[i].size()<<std::endl;
		    std::cout<<"j: "<<j<<std::endl;
		    std::cout<<"Cluster Station: "<<xy[i][j]->Station()<<"...Sensor: "<<xy[i][j]->Sensor()<<std::endl;
		}
	   }	
           std::cout<<"xyu check"<<std::endl;
	   for (size_t i=0; i<xyu.size(); i++){
           std::cout<<"i: "<<i<<std::endl;
                for (size_t j=0; j<xyu[i].size(); j++){
                    std::cout<<"xyu[i].size(): "<<xyu[i].size()<<std::endl;
                    std::cout<<"j: "<<j<<std::endl;
                    std::cout<<"Cluster Station: "<<xyu[i][j]->Station()<<"...Sensor: "<<xyu[i][j]->Sensor()<<std::endl;
                }
           }    
*/

	   rb::SpacePoint sp;
	   //make points for xy
           for (size_t i=0; i<xy_stripv.size(); i++){
	       //for (size_t j=0; j<xy_stripv[i].size(); j++){	   
	           pdd fA = std::make_pair(xy_stripv[i][0].X0()[0],xy_stripv[i][0].X0()[1]);
                   pdd fB = std::make_pair(xy_stripv[i][0].X1()[0],xy_stripv[i][0].X1()[1]);
                   pdd fC = std::make_pair(xy_stripv[i][1].X0()[0],xy_stripv[i][1].X0()[1]);
                   pdd fD = std::make_pair(xy_stripv[i][1].X1()[0],xy_stripv[i][1].X1()[1]);

                   pdd point = MakeIntersection(fA,fB,fC,fD);

		   //make point vector (x,y,z)
	           double x[3] = {point.first,point.second,xy_stripv[i][0].X0()[2]}; 
		   //z-component for .X0() and .X1() should be the same

		   //set SpacePoint object 
		   sp.SetX(x);
		   //fSpacePoint3D->Fill(x[0],x[1],x[2]);
		   spv.push_back(sp); 
		   //add to unique pointer vector 
		   spacepointv->push_back(sp);

		   std::cout<<"Space Point position: "<<"("<<x[0]<<","<<x[1]<<","<<x[2]<<")"<<std::endl;
	       //}
	   }
	   //make points for xyu
	   //unsure if we want to do this in this way
	   for (size_t i=0; i<xyu.size(); i++){
		   pdd fA01 = std::make_pair(xyu_stripv[i][0].X0()[0],xyu_stripv[i][0].X0()[1]);
                   pdd fB01 = std::make_pair(xyu_stripv[i][0].X1()[0],xyu_stripv[i][0].X1()[1]);
                   pdd fC01 = std::make_pair(xyu_stripv[i][1].X0()[0],xyu_stripv[i][1].X0()[1]);
                   pdd fD01 = std::make_pair(xyu_stripv[i][1].X1()[0],xyu_stripv[i][1].X1()[1]);

	           pdd point01 = MakeIntersection(fA01,fB01,fC01,fD01);	  

	           pdd fA02 = std::make_pair(xyu_stripv[i][0].X0()[0],xyu_stripv[i][0].X0()[1]);
                   pdd fB02 = std::make_pair(xyu_stripv[i][0].X1()[0],xyu_stripv[i][0].X1()[1]);
                   pdd fC02 = std::make_pair(xyu_stripv[i][2].X0()[0],xyu_stripv[i][2].X0()[1]);
                   pdd fD02 = std::make_pair(xyu_stripv[i][2].X1()[0],xyu_stripv[i][2].X1()[1]);

                   pdd point02 = MakeIntersection(fA02,fB02,fC02,fD02);

		   pdd fA12 = std::make_pair(xyu_stripv[i][1].X0()[0],xyu_stripv[i][1].X0()[1]);
                   pdd fB12 = std::make_pair(xyu_stripv[i][1].X1()[0],xyu_stripv[i][1].X1()[1]);
                   pdd fC12 = std::make_pair(xyu_stripv[i][2].X0()[0],xyu_stripv[i][2].X0()[1]);
                   pdd fD12 = std::make_pair(xyu_stripv[i][2].X1()[0],xyu_stripv[i][2].X1()[1]);

                   pdd point12 = MakeIntersection(fA12,fB12,fC12,fD12);

		   //average of three points (center of mass)
                   double ptavg_x2 = (point01.first + point02.first + point12.first)/3. ;
                   double ptavg_y2 = (point01.second + point02.second + point12.second)/3. ;

                   pdd point2 = std::make_pair(ptavg_x2, ptavg_y2);
	        
		   //make point vector (x,y,z)
		   double x[3] = {point2.first,point2.second,xyu_stripv[i][0].X0()[2]};
		   //z-component for .X0() and .X1() should be the same

		   //set SpacePoint object
		   sp.SetX(x);
                   spv.push_back(sp);
                   //add to unique pointer vector
                   spacepointv->push_back(sp);

	           std::cout<<"Space Point position: "<<"("<<x[0]<<","<<x[1]<<","<<x[2]<<")"<<std::endl;
	   }

	   std::cout<<"The total number of points is "<<spv.size()<<std::endl; 
	   for (size_t i=0; i<spv.size(); i++){
//		std::cout<<"Space Point position: "<<spv[i].Pos()<<std::endl;
	   }	  
 
	   //if groupsize =2 do point, if =3 average between points
	   //for each station of two sensors...and what about three? avg of two points?
	   //    for (size_t i=0; i<clusters.size(); i++){
/*
	   //Station 0		
	   pdd fA0 = std::make_pair(stripv[0].X0()[0],stripv[0].X0()[1]);
           pdd fB0 = std::make_pair(stripv[0].X1()[0],stripv[0].X1()[1]); 
           pdd fC0 = std::make_pair(stripv[1].X0()[0],stripv[1].X0()[1]);
           pdd fD0 = std::make_pair(stripv[1].X1()[0],stripv[1].X1()[1]);

	   pdd point0 = MakeIntersection(fA0,fB0,fC0,fD0);

	   //Station 1
	   pdd fA1 = std::make_pair(stripv[2].X0()[0],stripv[2].X0()[1]);
           pdd fB1 = std::make_pair(stripv[2].X1()[0],stripv[2].X1()[1]);
           pdd fC1 = std::make_pair(stripv[3].X0()[0],stripv[3].X0()[1]);
           pdd fD1 = std::make_pair(stripv[3].X1()[0],stripv[3].X1()[1]);

           pdd point1 = MakeIntersection(fA1,fB1,fC1,fD1);

	   //Station 2
	   //three combinations 4 5 6
	   pdd fA2_45 = std::make_pair(stripv[4].X0()[0],stripv[4].X0()[1]);
           pdd fB2_45 = std::make_pair(stripv[4].X1()[0],stripv[4].X1()[1]);
           pdd fC2_45 = std::make_pair(stripv[5].X0()[0],stripv[5].X0()[1]);
           pdd fD2_45 = std::make_pair(stripv[5].X1()[0],stripv[5].X1()[1]);

           pdd point2_45 = MakeIntersection(fA2_45,fB2_45,fC2_45,fD2_45);

	   pdd fA2_56 = std::make_pair(stripv[5].X0()[0],stripv[5].X0()[1]);
           pdd fB2_56 = std::make_pair(stripv[5].X1()[0],stripv[5].X1()[1]);
           pdd fC2_56 = std::make_pair(stripv[6].X0()[0],stripv[6].X0()[1]);
           pdd fD2_56 = std::make_pair(stripv[6].X1()[0],stripv[6].X1()[1]);

           pdd point2_56 = MakeIntersection(fA2_56,fB2_56,fC2_56,fD2_56);

	   pdd fA2_46 = std::make_pair(stripv[4].X0()[0],stripv[4].X0()[1]);
           pdd fB2_46 = std::make_pair(stripv[4].X1()[0],stripv[4].X1()[1]);
           pdd fC2_46 = std::make_pair(stripv[6].X0()[0],stripv[6].X0()[1]);
           pdd fD2_46 = std::make_pair(stripv[6].X1()[0],stripv[6].X1()[1]);

           pdd point2_46 = MakeIntersection(fA2_46,fB2_46,fC2_46,fD2_46);

	   //average of three points (center of mass)
	   double ptavg_x2 = (point2_45.first + point2_56.first + point2_46.first)/3. ;
	   double ptavg_y2 = (point2_45.second + point2_56.second + point2_46.second)/3. ;

	   pdd point2 = std::make_pair(ptavg_x2, ptavg_y2); 

	   //Station 3
	   //three combinations 7 8 9 
	   pdd fA3_78 = std::make_pair(stripv[7].X0()[0],stripv[7].X0()[1]);
           pdd fB3_78 = std::make_pair(stripv[7].X1()[0],stripv[7].X1()[1]);
           pdd fC3_78 = std::make_pair(stripv[8].X0()[0],stripv[8].X0()[1]);
           pdd fD3_78 = std::make_pair(stripv[8].X1()[0],stripv[8].X1()[1]);

           pdd point3_78 = MakeIntersection(fA3_78,fB3_78,fC3_78,fD3_78);

           pdd fA3_89 = std::make_pair(stripv[8].X0()[0],stripv[8].X0()[1]);
           pdd fB3_89 = std::make_pair(stripv[8].X1()[0],stripv[8].X1()[1]);
           pdd fC3_89 = std::make_pair(stripv[9].X0()[0],stripv[9].X0()[1]);
           pdd fD3_89 = std::make_pair(stripv[9].X1()[0],stripv[9].X1()[1]);

           pdd point3_89 = MakeIntersection(fA3_89,fB3_89,fC3_89,fD3_89);

           pdd fA3_79 = std::make_pair(stripv[7].X0()[0],stripv[7].X0()[1]);
           pdd fB3_79 = std::make_pair(stripv[7].X1()[0],stripv[7].X1()[1]);
           pdd fC3_79 = std::make_pair(stripv[9].X0()[0],stripv[9].X0()[1]);
           pdd fD3_79 = std::make_pair(stripv[9].X1()[0],stripv[9].X1()[1]);

           pdd point3_79 = MakeIntersection(fA3_79,fB3_79,fC3_79,fD3_79);

	   //average of three points (center of mass)
	   double ptavg_x3 = (point3_78.first + point3_89.first + point3_79.first)/3. ;
           double ptavg_y3 = (point3_78.second + point3_89.second + point3_79.second)/3. ;

           pdd point3 = std::make_pair(ptavg_x3, ptavg_y3);
*/
	   //maybe put things into a rb::SpacePoint object (or a vector of them) and add to art file

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
	clustsegpair.clear();
	clustStripMap.clear(); 
	clusters.clear();
        clustMap.clear();

	  //fEvtNum++;
	  // Place SSDSpacePoint object into event
//	  evt.put(std::move(SpacePoint_Robertv)); //or fSPVector?
	//}
//	fEvtNum++; //can get rid of?
      }
    }
    catch(...) {

    }
//    spacepoint->Fill();
  } //want plots
  //evt.put(std::move(spacepointv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrack) //_Robert)
