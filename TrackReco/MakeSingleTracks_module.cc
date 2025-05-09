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

  private:
  
    TTree*      spacepoint;
    int         run,subrun,event;
    int         fEvtNum;

    std::vector<const rb::TrackSegment*> trksegs;
    std::vector<const rb::TrackSegment*> trksegs1;
    std::vector<const rb::TrackSegment*> trksegs2;
    std::vector<const rb::TrackSegment*> trksegs3;
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::TrackSegment> tsv;
    std::vector<const rb::TrackSegment*> tsvcut;

    std::map<std::pair<int, int>, int> clustMap;

    bool        fMakePlots;
    int 	goodclust = 0;
    int         badclust = 0; 
    size_t      nPlanes;
    size_t      nStations;

    //fcl parameters
    bool        fCheckClusters;     //Check clusters for event 
    bool        fCheckTrackSeg;
    std::string fClusterLabel;
    std::string fG4Label;
    std::string fTrkSegLabel;
    int         fPBeamTmp;

  };

  //.......................................................................
  
  emph::MakeSingleTracks::MakeSingleTracks(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")), 
    fCheckTrackSeg     (pset.get< bool >("CheckTrackSeg")),
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fG4Label           (pset.get< std::string >("G4Label")),
    fTrkSegLabel       (pset.get< std::string >("TrkSegLabel")),
    fPBeamTmp          (pset.get< int >("PBeamTmp"))
    {
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
    std::cerr<<"Starting MakeSingleTracks"<<std::endl;

    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");  
  }
 
  //......................................................................
  
  void emph::MakeSingleTracks::endJob()
  {
       std::cout<<"MakeSingleTracks: Number of events with one cluster per sensor: "<<goodclust<<std::endl;
       std::cout<<"MakeSingleTracks: Number of available clusters: "<<badclust+goodclust<<std::endl;
  }

  //......................................................................

  void emph::MakeSingleTracks::produce(art::Event& evt)
  {
    tsvcut.clear();
    tsv.clear();

    std::unique_ptr< std::vector<rb::Track> > trackv(new std::vector<rb::Track>);

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
	auto hasclusters = evt.getHandle<std::vector<rb::SSDCluster> >(fClusterLabel);
	if (!hasclusters){
	  mf::LogError("HasSSDClusters")<<"No clusters found in event but CheckClusters set to true!";
	  abort();
	}
      }

      if (fCheckTrackSeg){
        auto hastrackseg = evt.getHandle<std::vector<rb::TrackSegment> >(fTrkSegLabel);
        if (!hastrackseg){
          mf::LogError("HasTrackSeg")<<"No track segments found in event but CheckTrackSeg set to true!";
          abort();
        }
      }

      art::Handle< std::vector<rb::TrackSegment> > trksegH;
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
	evt.getByLabel(fTrkSegLabel, trksegH);
	trksegs.clear();
        trksegs1.clear();
        trksegs2.clear();
        trksegs3.clear();
        if (!trksegH->empty()){
         for (size_t idx=0; idx < trksegH->size(); ++idx) {
	    const rb::TrackSegment& ts = (*trksegH)[idx];
	    trksegs.push_back(&ts);
            if (ts.Label() == 1) trksegs1.push_back(&ts);
	    else if (ts.Label() == 2) trksegs2.push_back(&ts);
            else if (ts.Label() == 3) trksegs3.push_back(&ts);		
	    else std::cout<<"Track segments not properly labeled."<<std::endl;

          }
	}
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

          // Instance of single track algorithm
          emph::SingleTrackAlgo algo = emph::SingleTrackAlgo(fEvtNum,nStations,nPlanes);
 
	  clusters.clear();
          clustMap.clear();

          // Reconstructed hits
          // Choose track segments 2 and 3 with 3 space points
          if (goodEvent){
            for (auto t : trksegs){
              if (t->Label() == 1) tsvcut.push_back(t);
              else if (t->NSpacePoints() == 3) tsvcut.push_back(t);
            }  

            // Now make tracks
            // Eventually beamtrk should be fixed later with SpillInfo
            trackv->clear();
            rb::Track beamtrk;
	    std::vector<rb::TrackSegment> tsvec;

	    auto t1 = *tsvcut[0];
            tsvec.push_back(t1);
	    algo.SetBeamTrk(t1,fPBeamTmp);
            beamtrk.Add(t1);
            beamtrk.SetP(t1.P());
            beamtrk.SetVtx(t1.Vtx());
            trackv->push_back(beamtrk);

            rb::Track sectrk;
	    auto t2 = *tsvcut[1]; 
            auto t3 = *tsvcut[2];
	    tsvec.push_back(t2);
	    tsvec.push_back(t3);	
            algo.SetRecoTrk(t2,t3);		
            sectrk.Add(t2);
            sectrk.Add(t3);
            sectrk.SetP(t2.P()); // this should come from an analysis of the bend angle between track segments 1 and 2.
	    auto v = algo.SetTrackInfo(tsvec[0],tsvec[1]);
            sectrk.SetVtx(v); // this should come from a calculation of the intersection or point of closest approach between track segments 0 and 1.
            trackv->push_back(sectrk);
	  }
	} //clust not empty
      } //try
      catch(...) {

      }

    } //want plots
 
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeSingleTracks)
