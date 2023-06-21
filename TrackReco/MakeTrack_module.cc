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
//#include "TH3F.h"
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
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> strips;

    std::map<std::pair<int, int>, int> clustMap;
    //std::map<const std::pair<int, int>, emph::geo::sensorView> clustMap;
    //std::map<std::pair<const emph::geo::SSDStation, const emph::geo::Detector>, int> clustMap;
    std::map<std::pair<int, int>, std::pair<int, geo::sensorView> > planeViewMap;
    bool        fMakePlots;
    int 	goodclust = 0;
    int 	fTotClust;
    int         naur = 0; 
    //fcl parameters
    bool        fCheckClusters;     ///< Check clusters for event 

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
    //spacepoint->Branch(""
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
    //rb::LineSegment ls;
    if (!fDetGeoMap) fDetGeoMap = new emph::dgmap::DetGeoMap();

    //std::cout<<"In MakePoint"<<std::endl;

    if(fDetGeoMap->SSDClusterToLineSegment(cl, ls)){
	//std::cout<<"Made line segment"<<std::endl;

        strips.push_back(ls);
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

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

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
    //std::cout<<"cluster vector size: "<<"uh"<<std::endl;
    try {
      evt.getByLabel(fClusterLabel, clustH);
      if (!clustH->empty()){
         for (size_t idx=0; idx < clustH->size(); ++idx) {
	      //std::cout<<fEvtNum<<std::endl;
              const rb::SSDCluster& clust = (*clustH)[idx];
//              const emph::geo::SSDStation &st = emgeo->GetSSDStation(clust.Station());
//              const emph::geo::Detector &sd = st.GetSSD(clust.Sensor());
	      ++clustMap[std::pair<int,int>(clust.Station(),clust.Sensor())];

	     //rb::LineSegment ls;

	     //if (! fDetGeoMap) fDetGeoMap = new dgmap::DetGeoMap();
	     //if (fDetGeoMap->SSDClusterToLineSegment(clust, ls)) std::cout<<"DID it"<<std::endl;
/*
	      rb::LineSegment ls;

	      if (! fDetGeoMap) fDetGeoMap = new dgmap::DetGeoMap();

              if (fDetGeoMap->SSDClusterToLineSegment(clust, ls)) std::cout<<"DID it"<<std::endl;
*/
	      //fTotClust++;
	      clusters.push_back(&clust); //push back a  pointer

              //clusters.push_back(clust);
	      //clusters->push_back(&clust); //push back a  pointer
	  }
          //fTotClust++;
	  //std::cout<<"cluster vector size: "<<clusters.size()<<std::endl;

	  bool goodEvent = false; //true
          //If there are more clusters than sensors, skip event
	  if (clusters.size()==20){ //what should this be...
//          if (clusters.size()<23){ //is this right?
	     for (auto i : clustMap){
	         //std::cout<<"("<<i.first.first<<","<<i.first.second<<","<<i.second<<")"<<std::endl; 
		 if (i.second != 1){goodEvent = false;} //std::cout<<"More than one cluster per plane :("<<std::endl;}        
	         else goodEvent = true;
	     }
	     if (goodEvent==true) {goodclust++; std::cout<<"First test passed :3"<<std::endl;}
	     else {naur++; std::cout<<                     "Naurrrrrrr        :("<<std::endl;}
          }
	  else naur++;
	  //else std::cout<<"Naurrrrrrr :("<<std::endl;
	  
	  rb::LineSegment strip;
	  //std::vector<rb::LineSegment> stripy;
          if (goodEvent==true && clusters.size() > 0){
	     for (size_t i=0; i<clusters.size(); i++){
		MakePoint(*clusters[i],strip);
		 //const rb::SSDCluster* clusty = clusters[i];
		//MakePoint(*clusty,strips[i]);
//		 MakePoint(*clusters[i],strips[i]);
//                 if(DetGeoMap::SSDClusterToLineSegment(clusters[i], strips[i])) std::cout<<"Made line segment"<<std::endl;
	     }
          }
 	  
	//if line segment populated, make a point		 
	if (strips.size() > 0){
	   //for (size_t i=0; i<1; i++){
	   //plane 0 --> strips[0] is yview, strips[1] is xview
	   //double m0 = (strips[0].X1()[1]-strips[0].X0()[1])/
	   //	       (strips[0].X1()[0]-strips[0].X0()[0])
	pdd fA = std::make_pair(strips[0].X0()[0],strips[0].X0()[1]);
        pdd fB = std::make_pair(strips[0].X1()[0],strips[0].X1()[1]); 
        pdd fC = std::make_pair(strips[1].X0()[0],strips[1].X0()[1]);
        pdd fD = std::make_pair(strips[1].X1()[0],strips[1].X1()[1]);

	pdd point = MakeIntersection(fA,fB,fC,fD);

	if (point.first == FLT_MAX && point.second==FLT_MAX){
           std::cout << "The given lines AB and CD are parallel.\n";
        }
 
        else std::cout << "The line segments intersect at (" << point.first 
		       << "," << point.second << ")" << std::endl;

        //add point to vector?

		 //what happens when you have three points...(XYU planes)
	   //}
	}
 
	  clusters.clear();
          clustMap.clear();
          strips.clear();

          //make good cluster vector (matrix?)
          //take ccolumn of matrix (vector of clusters)
          //make line segment for each cluster, find position through different combinations?

          //DetGeoMap::SSDClusterToLineSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)

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
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrack) //_Robert)
