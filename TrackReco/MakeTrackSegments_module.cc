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
#include <numeric>

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
#include "Align/service/AlignService.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RecoBase/SSDCluster.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/Track.h"
#include "RecoUtils/RecoUtils.h"
#include "Simulation/SSDHit.h"
#include "Simulation/Particle.h"
#include "TrackReco/SingleTrackAlgo.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class MakeTrackSegments : public art::EDProducer {
  public:
    explicit MakeTrackSegments(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeTrackSegments() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();

  private:
  
    TTree*      spacepoint;
    int         run,subrun,event;
    int         fEvtNum;

    std::vector<const sim::Particle*> particles;
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> linesegments;
    std::vector<rb::SpacePoint> spv;
    std::vector<rb::TrackSegment> tsv;
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group;
    double sectrkp[3];
    double sectrkvtx[3];

    std::map<std::pair<int, int>, int> clustMap;
    std::map<int, std::map<std::pair<int, int>, int>> clustMapAtLeastOne;

    bool        fMakePlots;
    int 	goodclust = 0;
    int         badclust = 0; 
    size_t      nPlanes;
    size_t      nStations;

    //fcl parameters
    bool        fCheckClusters;     //Check clusters for event 
    std::string fClusterLabel;
    std::string fG4Label;
    std::string fClusterCut;

    //reco info for lines
    std::vector<rb::SpacePoint> sp1;
    std::vector<rb::SpacePoint> sp2;
    std::vector<rb::SpacePoint> sp3;
    double pbeam[3];

  };

  //.......................................................................
  
  emph::MakeTrackSegments::MakeTrackSegments(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")), 
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fG4Label           (pset.get< std::string >("G4Label")),
    fClusterCut        (pset.get< std::string >("ClusterCut"))
    {
      this->produces< std::vector<rb::LineSegment> >();
      this->produces< std::vector<rb::SpacePoint> >();
      this->produces< std::vector<rb::TrackSegment> >();
    }
  
  //......................................................................
  
//  MakeTrackSegments::~MakeTrackSegments()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................

  // void MakeTrackSegments::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }

  //......................................................................
  
  void MakeTrackSegments::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();
  }

  //......................................................................
   
  void emph::MakeTrackSegments::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");  
  }
 
  //......................................................................
  
  void emph::MakeTrackSegments::endJob()
  {
       if (fClusterCut == "strict") std::cout<<"Number of events with one cluster per sensor: "<<goodclust<<std::endl;
       if (fClusterCut == "lessstrict") std::cout<<"Number of events with at least two clusters per station: "<<goodclust<<std::endl;
       std::cout<<"Number of available clusters: "<<badclust+goodclust<<std::endl;
  }

  //......................................................................

  void emph::MakeTrackSegments::produce(art::Event& evt)
  {
    std::cout<<"Starting MakeTrackSegments"<<std::endl;

    tsv.clear();
    spv.clear();

    std::unique_ptr< std::vector<rb::LineSegment> > linesegv(new std::vector<rb::LineSegment>);
    std::unique_ptr< std::vector<rb::SpacePoint> > spacepointv(new std::vector<rb::SpacePoint>);
    std::unique_ptr< std::vector<rb::TrackSegment> > tracksegmentv(new std::vector<rb::TrackSegment>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    //debug
    //if(fEvtNum==1080) fMakePlots = true;
    //else fMakePlots = false;

    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

    fMakePlots = true;

    if(fMakePlots){ 

      if (fCheckClusters){
	//auto hasclusters = evt.getHandle<std::vector<rb::SSDCluster>>("clust"); //fClusterLabel); //"ssdclusts");
        //auto hasclusters = evt.getHandle<rb::SSDCluster>(fClusterLabel);
	auto hasclusters = evt.getHandle<std::vector<rb::SSDCluster>>(fClusterLabel);
	if (!hasclusters){
	  mf::LogError("HasSSDClusters")<<"No clusters found in event but CheckClusters set to true!";
	  abort();
	}
      }

      art::Handle< std::vector<sim::Particle> > particleH;
      art::Handle< std::vector<rb::SSDCluster> > clustH;
      art::Handle< std::vector<sim::SSDHit> > ssdHitH;
 
      try {
	evt.getByLabel(fG4Label,ssdHitH);
      } 
      catch(...) {
	std::cout << "WARNING: No SSDHits found!" << std::endl;
      }

      bool goodEvent = false;
      
      try {
        evt.getByLabel(fG4Label, particleH);
        particles.clear();
        if (!particleH->empty()){
          for (size_t idx=0; idx < particleH->size(); ++idx) {
            const sim::Particle& part = (*particleH)[idx];
            particles.push_back(&part);
          }
        
          std::cout<<"Number of particles: "<<particles.size()<<std::endl;
          pbeam[0] = particles[0]->Px();
          pbeam[1] = particles[0]->Py();
 	  pbeam[2] = particles[0]->Pz();
          //std::cout<<"Incident pbeam[0]: "<<pbeam[0]<<std::endl;
          //std::cout<<"Incident pbeam[1]: "<<pbeam[1]<<std::endl;
          //std::cout<<"Incident pbeam[2]: "<<pbeam[2]<<std::endl;
        }
	std::cout<<"After particle"<<std::endl;
	//fClusterLabel = "clust";
	evt.getByLabel(fClusterLabel, clustH);
        if (clustH->empty()) std::cout<<"EMPTY"<<std::endl;
	if (!clustH->empty()){

	std::cout<<"Clust not empty"<<std::endl;
          rb::LineSegment lineseg_tmp  = rb::LineSegment();
	  for (size_t idx=0; idx < clustH->size(); ++idx) {
	    const rb::SSDCluster& clust = (*clustH)[idx];
	    ++clustMap[std::pair<int,int>(clust.Station(),clust.Plane())];
	    ++clustMapAtLeastOne[clust.Station()][std::pair<int,int>(clust.Station(),clust.Plane())];
	    clusters.push_back(&clust);

            linesegments.push_back(lineseg_tmp);
            if (clust.AvgStrip() > 640){
              std::cout<<"Skipping nonsense"<<std::endl;
              linesegv->push_back(linesegments[idx]); //lineseg_tmp);
              continue; }
            if (dgm->Map()->SSDClusterToLineSegment(clust,linesegments[idx])){
              linesegv->push_back(linesegments[idx]); //lineseg_tmp);
	    }
            else
              std::cout<<"Couldn't make line segment from Cluster?!?"<<std::endl; 
	  }

          //ONE CLUSTER PER PLANE
          //If there are more clusters than sensors, skip event
	  if (fClusterCut == "strict"){
	    if (clusters.size()==nPlanes){
	      for (auto i : clustMap){
	        if (i.second != 1){goodEvent = false; break;} 
	        else goodEvent = true;
	      }
	      if (goodEvent==true) {goodclust++;} 
	      else {badclust++;}
            }
	    else badclust++;
	  }

          //remove this loop?
	  int count=0;
	  for (auto i : clustMapAtLeastOne){
            std::cout<<"i.first: "<<i.first<<std::endl; //"..."<<"i.second.first: "<<i.second.first<<std::endl;
	    for (auto j : i.second){
	      count++;
	      std::cout<<"... "<<j.second<<std::endl;
	    }
            std::cout<<"count = "<<count<<std::endl;
            count = 0;
	  }
	  std::cout<<"nclusters: "<<clusters.size()<<std::endl;

	  fClusterCut == "lessstrict";
	  //AT LEAST TWO CLUSTERS PER STATION
	  if (fClusterCut == "lessstrict"){
            int count=0; int countSt = 0;
            for (auto i : clustMapAtLeastOne){
              for (auto j : i.second){
                count++;
              }
	      countSt++;
	      // make sure every station has at least two unique clusters
	      if (count < 2) { goodEvent = false; break; }
	      else { goodEvent = true; count = 0; }
	    }
	    // make sure every station is populated
	    if (countSt != (int)nStations) goodEvent = false;
	    if (goodEvent==true) {goodclust++;}
            else {badclust++;}

            std::cout<<"Number of clusters: "<<clusters.size()<<" and goodEvent = "<<goodEvent<<std::endl;
	  }

          cl_group.resize(nStations);
          ls_group.resize(nStations);

	  for (size_t i=0; i<nStations; i++){
	    cl_group[i].resize(nPlanes); //emgeo->GetSSDStation(i)->NPlanes()); //nPlanes);
	    ls_group[i].resize(nPlanes); //emgeo->GetSSDStation(i)->NPlanes()); //nPlanes);
	  }

          for (size_t i=0; i<clusters.size(); i++){
	    int plane = clusters[i]->Plane();
	    int station = clusters[i]->Station();	 
  
	    //group clusters according to plane
	    //within each station, do every combination
	    cl_group[station][plane].push_back(clusters[i]);
	  }
	 
          //instance of single track algorithm
          emph::SingleTrackAlgo algo = emph::SingleTrackAlgo(fEvtNum,nStations,nPlanes);
 
	  //group linesegments
	  if (goodEvent == true){
	    for (size_t i=0; i<clusters.size(); i++){
	      int plane = clusters[i]->Plane();
              int station = clusters[i]->Station();
	      ls_group[station][plane].push_back(&linesegments[i]);
	    }

            //make reconstructed hits
            spv = algo.MakeHits(ls_group);
	    for (auto sp : spv)
	      spacepointv->push_back(sp);
	  }
	  
          ls_group.clear();
          cl_group.clear();
          linesegments.clear();
	  clusters.clear();
          clustMap.clear();
	  clustMapAtLeastOne.clear();

          //reconstructed hits
          if (goodEvent && spv.size() > 0){ // && ssdHitH->size() == nPlanes){
            for (size_t i=0; i<spv.size(); i++){
              if (emgeo->GetTarget()){
                if (spv[i].Pos()[2] < emgeo->GetTarget()->Pos()(2)) sp1.push_back(spv[i]);
                if (spv[i].Pos()[2] > emgeo->GetTarget()->Pos()(2) && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) sp2.push_back(spv[i]);
                if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) sp3.push_back(spv[i]);
              }
            }
            //form lines and fill plots

std::cout<<"Checking sp1"<<std::endl;
for (auto i : sp1){
std::cout<<"Station = "<<i.Station()<<std::endl;
}
std::cout<<"Checking sp2"<<std::endl;
for (auto i : sp2){
std::cout<<"Station = "<<i.Station()<<std::endl;
}
std::cout<<"Checking sp3"<<std::endl;
for (auto i : sp3){
std::cout<<"Station = "<<i.Station()<<std::endl;
}
std::cout<<"Now sp1"<<std::endl;

            std::vector<rb::TrackSegment> tstmp1 = algo.MakeTrackSeg(sp1);
	    for (auto i : tstmp1){ i.SetLabel(1); tsv.push_back(i); }
	std::cout<<"MODULE tsv size: "<<tsv.size()<<std::endl;
	
            std::vector<rb::TrackSegment> tstmp2 = algo.MakeTrackSeg(sp2); 
            for (auto i : tstmp2) { i.SetLabel(2); tsv.push_back(i); }  
        std::cout<<"MODULE tsv size: "<<tsv.size()<<std::endl;

            std::vector<rb::TrackSegment> tstmp3 = algo.MakeTrackSeg(sp3);    
            for (auto i : tstmp3) {i.SetLabel(3); tsv.push_back(i);}
        std::cout<<"MODULE tsv size: "<<tsv.size()<<std::endl;
	  
            for (auto ts : tsv) {
              tracksegmentv->push_back(ts);
            }
	  }
          sp1.clear();
          sp2.clear();
          sp3.clear();

	} //clust not empty

      } //try
      catch(...) {

      }

    } //want plots
  
    evt.put(std::move(linesegv));
    evt.put(std::move(spacepointv));
    evt.put(std::move(tracksegmentv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrackSegments)
