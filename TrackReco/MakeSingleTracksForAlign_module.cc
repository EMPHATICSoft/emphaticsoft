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
  class MakeSingleTracksForAlign : public art::EDProducer {
  public:
    explicit MakeSingleTracksForAlign(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeSingleTracksForAlign() {};
    
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
    std::vector<std::vector<double>> sp1;
    std::vector<std::vector<double>> sp2;
    std::vector<std::vector<double>> sp3;
    //std::vector<rb::SpacePoint> sp1;
    //std::vector<rb::SpacePoint> sp2;
    //std::vector<rb::SpacePoint> sp3;
    double pbeam[3];

  };

  //.......................................................................
  
  emph::MakeSingleTracksForAlign::MakeSingleTracksForAlign(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")), 
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fG4Label           (pset.get< std::string >("G4Label")),
    fClusterCut        (pset.get< std::string >("ClusterCut"))
    {
      this->produces< std::vector<rb::LineSegment> >();
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
  
  void MakeSingleTracksForAlign::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();
  }

  //......................................................................
   
  void emph::MakeSingleTracksForAlign::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");  
  }
 
  //......................................................................
  
  void emph::MakeSingleTracksForAlign::endJob()
  {
       if (fClusterCut == "strict") std::cout<<"Number of events with one cluster per sensor: "<<goodclust<<std::endl;
       if (fClusterCut == "lessstrict") std::cout<<"Number of events with at least two clusters per station: "<<goodclust<<std::endl;
       std::cout<<"Number of available clusters: "<<badclust+goodclust<<std::endl;
  }

  //......................................................................

  void emph::MakeSingleTracksForAlign::produce(art::Event& evt)
  {
    tsv.clear();
    spv.clear();

    std::unique_ptr< std::vector<rb::LineSegment> > linesegv(new std::vector<rb::LineSegment>);
    std::unique_ptr< std::vector<rb::SpacePoint> > spacepointv(new std::vector<rb::SpacePoint>);
    std::unique_ptr< std::vector<rb::TrackSegment> > tracksegmentv(new std::vector<rb::TrackSegment>);
    std::unique_ptr< std::vector<rb::Track> > trackv(new std::vector<rb::Track>);

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
	auto hasclusters = evt.getHandle<rb::SSDCluster>("ssdclusts");
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
        
          //std::cout<<"Number of particles: "<<particles.size()<<std::endl;
          pbeam[0] = particles[0]->Px();
          pbeam[1] = particles[0]->Py();
 	  pbeam[2] = particles[0]->Pz();
          //std::cout<<"Incident pbeam[0]: "<<pbeam[0]<<std::endl;
          //std::cout<<"Incident pbeam[1]: "<<pbeam[1]<<std::endl;
          //std::cout<<"Incident pbeam[2]: "<<pbeam[2]<<std::endl;
        }
	evt.getByLabel(fClusterLabel, clustH);
	if (!clustH->empty()){
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

	  //AT LEAST TWO CLUSTERS PER STATION
	  if (fClusterCut == "lessstrict"){
	    if (clusters.size()<=nPlanes){
	      int sum[8] = {0};
	      for (auto i : clustMap){
                if (i.second != 1){goodEvent = false; break;}
	        sum[i.first.first] += i.second;
	      }
	      for (size_t i=0; i<nStations; i++){
	        if (sum[i] == 2 || sum[i] ==3) goodEvent = true;
	        else {goodEvent = false; break;}
	      }
	      if (goodEvent==true) {goodclust++;}
              else {badclust++;}
	    }
            else badclust++;

	    // debug
	    //if (goodEvent && clusters.size()<nPlanes){
	      //std::cout<<"# clusters: "<<clusters.size()<<std::endl;
	      //for (auto i : clustMap){
	      //  std::cout<<"Cluster: "<<i.first.first<<","<<i.first.second<<std::endl;
	      //}
	    //}
	  }

          //ALL CLUSTERS
/*
	  fClusterCut = "notstrict"; //hardcode
          if (fClusterCut == "notstrict"){
            goodEvent = true;
            for (size_t i=0; i<nPlanes; i++){
              if (cl_group[i].size() == 0){ goodEvent = false; break; }
            }
          }
*/ 
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
            spv = algo.MakeHitsOrig(ls_group);
	    for (auto sp : spv)
	      spacepointv->push_back(sp);
	  }

          ls_group.clear();
          cl_group.clear();
          linesegments.clear();
	  //particles.clear();
	  clusters.clear();
          clustMap.clear();
	  clustMapAtLeastOne.clear();

          //reconstructed hits
          if (goodEvent && spv.size() > 0 && ssdHitH->size() == nPlanes){

	    for (size_t i=0; i<spv.size(); i++){
	      std::vector<double> x = {spv[i].Pos()[0],spv[i].Pos()[1],spv[i].Pos()[2]};	

	      if (emgeo->GetTarget()){
                if (spv[i].Pos()[2] < emgeo->GetTarget()->Pos()(2)) sp1.push_back(x);
                if (spv[i].Pos()[2] > emgeo->GetTarget()->Pos()(2) && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) sp2.push_back(x);
                if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) sp3.push_back(x);
              }
	    }
/*
            for (size_t i=0; i<spv.size(); i++){
              if (emgeo->GetTarget()){
                if (spv[i].Pos()[2] < emgeo->GetTarget()->Pos()(2)) sp1.push_back(spv[i]);
                if (spv[i].Pos()[2] > emgeo->GetTarget()->Pos()(2) && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) sp2.push_back(spv[i]);
                if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) sp3.push_back(spv[i]);
              }
            }
*/
            //form lines and fill plots
            tsv = algo.MakeLines(sp1,sp2,sp3);
/*
            std::vector<rb::TrackSegment> tstmp1 = algo.MakeTrackSeg(sp1);
	    for (auto i : tstmp1){
	      algo.SetBeamTrk(i,pbeam);
	      tsv.push_back(i);
            }
            std::vector<rb::TrackSegment> tstmp2 = algo.MakeTrackSeg(sp2);   
            std::vector<rb::TrackSegment> tstmp3 = algo.MakeTrackSeg(sp3);    
	    for (auto i : tstmp2){
	      for (auto j : tstmp3){
		algo.SetRecoTrk(i,j);		
	      }
	    }
	    for (auto i : tstmp2) tsv.push_back(i);
            for (auto i : tstmp3) tsv.push_back(i);
*/
	    //for (auto i : {sp1,sp2,sp3}){//three vectors
	      //std::vector<rb::TrackSegment> tstmp = algo.MakeTrackSeg(i);
	      //if i is sp1, then do SetBeamTrk(something,pbeam)
	      //for now,

	      //how to do SetRecoTrk with multiple track options...?
	      //for (auto t : tstmp)
		//tsv.push_back(t);
	    //}
	    //algo.MakeTrackSeg(sp1);
	    //algo.MakeTrackSeg(sp2);
	    //algo.MakeTrackSeg(sp3);
	    for (auto ts : tsv) {
	      tracksegmentv->push_back(ts);	     
	    }
	    for (int i=0; i<3; ++i) {
	      sectrkp[i] = algo.GetSecTrkP()[i];
	      sectrkvtx[i] = algo.GetSecTrkVtx()[i];
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
      sectrk.SetP(sectrkp); // this should come from an analysis of the bend angle between track segments 1 and 2.
      sectrk.SetVtx(sectrkvtx); // this should come from a calculation of the intersection or point of closest approach between track segments 0 and 1.
      trackv->push_back(sectrk);
    }

    evt.put(std::move(linesegv));
    evt.put(std::move(spacepointv));
    evt.put(std::move(tracksegmentv));
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeSingleTracksForAlign)
