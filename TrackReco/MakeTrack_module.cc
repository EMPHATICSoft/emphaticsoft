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
    emph::geo::Geometry* emgeo;
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

    double xvec[8] = {999,999,999,999,999,999,999,999};
    double yvec[8] = {999,999,999,999,999,999,999,999};
    double zvec[8] = {999,999,999,999,999,999,999,999};
  
    std::vector<double> xvec_sim;
    std::vector<double> yvec_sim;
    std::vector<double> zvec_sim;
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
    fChannelMap = new emph::cmap::ChannelMap();
    fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
    emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());

    for (int fer=0; fer<10; ++fer){
      for (int mod=0; mod<6; ++mod){
        emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
        if (!fChannelMap->IsValidEChan(echan)) continue;
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
    for (int i=0; i<8; i++){
	sprintf(hresx,"res_x_%d",i);
        res_x[i] = tfs->make<TH1F>(hresx,hresx,100,-0.1,0.1);
        sprintf(hresy,"res_y_%d",i);
        res_y[i] = tfs->make<TH1F>(hresy,hresy,100,-0.1,0.1);
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

    //if(fEvtNum==877) fMakePlots = true;
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
                          pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          pdd fC = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                          pdd fD = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                          pdd point = MakeIntersection(fA,fB,fC,fD);
                          double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};
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
                          pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          pdd fC = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                          pdd fD = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                          pdd point = MakeIntersection(fA,fB,fC,fD);
                          double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};
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
                          pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          pdd fC = std::make_pair(ls_group[i+2][k]->X0()[0],ls_group[i+2][k]->X0()[1]);
                          pdd fD = std::make_pair(ls_group[i+2][k]->X1()[0],ls_group[i+2][k]->X1()[1]);

                          pdd point = MakeIntersection(fA,fB,fC,fD);
                          double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};
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
		          pdd fA = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                          pdd fB = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                          pdd fC = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                          pdd fD = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                          pdd point = MakeIntersection(fA,fB,fC,fD);	 

		          //make point vector (x,y,z)
		          double x[3] = {point.first,point.second,ls_group[i][j]->X0()[2]};
		          //z-component for .X0() and .X1() should be the same      

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
			  if (fEvtNum < 2000){
			     //xzdist_evt[fEvtNum]->Fill(x[2],x[0]);
			     xvec[st] = x[0];
			     yvec[st] = x[1];
			     zvec[st] = x[2];
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
         	              pdd fA01 = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                              pdd fB01 = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                              pdd fC01 = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]); 
                              pdd fD01 = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);

                              pdd point01 = MakeIntersection(fA01,fB01,fC01,fD01);

                   	      pdd fA02 = std::make_pair(ls_group[i][j]->X0()[0],ls_group[i][j]->X0()[1]);
                   	      pdd fB02 = std::make_pair(ls_group[i][j]->X1()[0],ls_group[i][j]->X1()[1]);
                              pdd fC02 = std::make_pair(ls_group[i+2][l]->X0()[0],ls_group[i+2][l]->X0()[1]);
	                      pdd fD02 = std::make_pair(ls_group[i+2][l]->X1()[0],ls_group[i+2][l]->X1()[1]);

                   	      pdd point02 = MakeIntersection(fA02,fB02,fC02,fD02);

                   	      pdd fA12 = std::make_pair(ls_group[i+1][k]->X0()[0],ls_group[i+1][k]->X0()[1]);
                              pdd fB12 = std::make_pair(ls_group[i+1][k]->X1()[0],ls_group[i+1][k]->X1()[1]);
                              pdd fC12 = std::make_pair(ls_group[i+2][l]->X0()[0],ls_group[i+2][l]->X0()[1]);
                              pdd fD12 = std::make_pair(ls_group[i+2][l]->X1()[0],ls_group[i+2][l]->X1()[1]);

	                      pdd point12 = MakeIntersection(fA12,fB12,fC12,fD12);

			      //average of three points (center of mass)
			      double ptavg_x2 = (point01.first + point02.first + point12.first)/3. ;
			      double ptavg_y2 = (point01.second + point02.second + point12.second)/3. ;

			     //double ptavg_x2 = point12.first; 
		             //double ptavg_y2 = point12.second;
			      
			      pdd point2 = std::make_pair(ptavg_x2, ptavg_y2);
				
			      //make point vector (x,y,z)
                              double x[3] = {point2.first,point2.second,ls_group[i][j]->X0()[2]};
            	              //z-component for .X0() and .X1() should be the same
                   
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
			      if (fEvtNum < 2000){
				 //xzdist_evt[fEvtNum]->Fill(x[2],x[0]);
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

	if (fEvtNum < 2000){
           if (!ssdHitH->empty() && goodEvent == true) {
		//std::cout<<"fEvtNum good: "<<fEvtNum<<std::endl;
//	      std::cout<<"Handle size: "<<ssdHitH->size()<<std::endl;
              for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
                  const sim::SSDHit& ssdhit = (*ssdHitH)[idx];
		  //std::cout<<"PID: "<<ssdhit.GetPId()<<std::endl;

		  xvec_sim.push_back(ssdhit.GetX());
		  yvec_sim.push_back(ssdhit.GetY());
		  zvec_sim.push_back(ssdhit.GetZ());
		  //std::cout<<"Handle size: "<<ssdHitH->size()<<std::endl; 
                  //int plane_sim = ssdhit->GetPlane(); 

		  //xvec_sim[plane_sim] = ssdhit->GetX();
		  //yvec_sim[plane_sim] = ssdhit->GetY();
		  //zvec_sim[plane_sim] = ssdhit->GetZ();
              }
	   }
      } 

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

              size_t graphsize = xvec_sim.size();
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
	      GSIM_xz[fEvtNum] = tfs->make<TGraph>(graphsize,&zvec_sim[0],&xvec_sim[0]);
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
	      size_t graphsize = yvec_sim.size();

	      GMULT_yz[fEvtNum] = tfs->makeAndRegister<TMultiGraph>(Gyzsim,Gyzsim);
	      GSIM_yz[fEvtNum] = tfs->make<TGraph>(graphsize,&zvec_sim[0],&yvec_sim[0]);
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
    //std::cout<<"Event is: "<<fEvtNum<<std::endl;
    for (int i=0; i<8; i++){
	for (size_t j=0; j<xvec_sim.size(); j++){
            if ( abs(zvec[i] - zvec_sim[j]) < 20){
//std::cout<<"Event is: "<<fEvtNum<<std::endl;
               std::cout<<"z pos diff:"<<abs(zvec[i] - zvec_sim[j])<<std::endl;
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

    for (int i=0; i<8; i++){
        xvec[i] = 999;
	yvec[i] = 999;
	zvec[i] = 999;
    }
    xvec_sim.clear();
    yvec_sim.clear();
    zvec_sim.clear();
  } //want plots
  evt.put(std::move(spacepointv));

  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrack)