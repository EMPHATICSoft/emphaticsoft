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
#include "RecoBase/RecoBaseDefs.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/Track.h"
#include "RecoUtils/RecoUtils.h"
#include "TrackReco/SingleTrackAlgo.h"
#include "StandardRecord/SRTrackSegment.h"

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
    void AddSSDLineSegmentsToTrack(rb::Track& trk);

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
    std::string fTrkSegLabel;
    bool        fShortOn;
    int         fPBeamTmp;
    bool        fLessStrict;
    double      fTrgtZ;
  };

  //.......................................................................
  
  emph::MakeSingleTracks::MakeSingleTracks(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")), 
    fCheckTrackSeg     (pset.get< bool >("CheckTrackSeg")),
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fTrkSegLabel       (pset.get< std::string >("TrkSegLabel")),
    fShortOn           (pset.get< bool >("ShortOn")),
    fPBeamTmp          (pset.get< int >("PBeamTmp")),
    fLessStrict        (pset.get< bool >("LessStrict"))
    {
      fTrgtZ = -99999.;
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
    fTrgtZ = (emgeo->TargetDSZPos()+emgeo->TargetUSZPos())/2.;

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
       if (fLessStrict) std::cout<<"MakeSingleTracks: Number of events with one track segment per region: "<<goodclust<<std::endl;
       else std::cout<<"MakeSingleTracks: Number of events with one cluster per plane: "<<goodclust<<std::endl;
       std::cout<<"MakeSingleTracks: Number of events available: "<<badclust+goodclust<<std::endl;
  }

  //......................................................................

  void emph::MakeSingleTracks::AddSSDLineSegmentsToTrack(rb::Track& trk)
  {
    int nTrackSegs = trk.NTrackSegments();
    for (int its=0; its<nTrackSegs; ++its) { // loop over track segments      
      auto ts = trk.GetTrackSegment(its);
      int nSP = ts->NSpacePoints();
      for (int isp=0; isp<nSP; ++isp) { // loop over spacepoints of the track segment
        auto sp = ts->GetSpacePoint(isp);
        int nLS = sp->NLineSegments();
        for (int ils=0; ils<nLS; ++ils) { // loop over line segments that form the spacepoint
          auto ls = sp->GetLineSegment(ils);
          trk.Add(rb::LineSegment(*ls));
        }
      }
    }

  }
  //......................................................................

  void emph::MakeSingleTracks::produce(art::Event &evt)
  {

    tsvcut.clear();
    tsv.clear();

    std::unique_ptr<std::vector<rb::Track>> trackv(new std::vector<rb::Track>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();
    // debug
    // if(fEvtNum==1080) fMakePlots = true;
    // else fMakePlots = false;

    fMakePlots = true;

    if (fMakePlots)
    {

      if (fCheckClusters)
      {
        auto hasclusters = evt.getHandle<std::vector<rb::SSDCluster>>(fClusterLabel);
        if (!hasclusters)
        {
          mf::LogError("HasSSDClusters") << "No clusters found in event but CheckClusters set to true!";
          abort();
        }
      }

      if (fCheckTrackSeg)
      {
        auto hastrackseg = evt.getHandle<std::vector<rb::TrackSegment>>(fTrkSegLabel);
        if (!hastrackseg)
        {
          mf::LogError("HasTrackSeg") << "No track segments found in event but CheckTrackSeg set to true!";
          abort();
        }
      }

      art::Handle<std::vector<rb::TrackSegment>> trksegH;
      art::Handle<std::vector<rb::SSDCluster>> clustH;

      bool goodEvent = false;

      try
      {
        evt.getByLabel(fTrkSegLabel, trksegH);
        trksegs.clear();
        trksegs1.clear();
        trksegs2.clear();
        trksegs3.clear();
        if (!trksegH->empty())
        {
          for (size_t idx = 0; idx < trksegH->size(); ++idx)
          {
            const rb::TrackSegment &ts = (*trksegH)[idx];
            trksegs.push_back(&ts);
            if (ts.region == rb::Region::kRegion1)
              trksegs1.push_back(&ts);
            else if (ts.region == rb::Region::kRegion2)
              trksegs2.push_back(&ts);
            else if (ts.region == rb::Region::kRegion3)
              trksegs3.push_back(&ts);
            else
              std::cout << "Track segments not properly labeled." << std::endl;
          }
        }

        evt.getByLabel(fClusterLabel, clustH);
        if (!clustH->empty())
        {
          for (size_t idx = 0; idx < clustH->size(); ++idx)
          {
            const rb::SSDCluster &clust = (*clustH)[idx];
            ++clustMap[std::pair<int, int>(clust.Station(), clust.Plane())];
            clusters.push_back(&clust);
          }

          /*
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
          */

          // Check for three good track segments
          int nts1 = 0;
          int nts2 = 0;
          int nts3 = 0;
          int nts2sp2 = 0;
          int nts2sp3 = 0;
          int nts3sp2 = 0;
          int nts3sp3 = 0;

          for (auto t : trksegs)
          {
            if (t->region == rb::Region::kRegion1)
              nts1++;
            if (t->region == rb::Region::kRegion2)
              nts2++;
            if (t->region == rb::Region::kRegion3)
              nts3++;
          }

          if (fLessStrict)
          {

            for (auto t : trksegs)
            {
              if (t->region == rb::Region::kRegion2)
              {
                if (t->NSpacePoints() == 2)
                  nts2sp2++;
                if (t->NSpacePoints() == 3)
                  nts2sp3++;
              }
              if (t->region == rb::Region::kRegion3)
              {
                if (t->NSpacePoints() == 2)
                  nts3sp2++;
                if (t->NSpacePoints() == 3)
                  nts3sp3++;
              }
            }
            // We want events with either nts2 (nts3)
            //   (1) 1 = one track segment option in each region (1 -- 1 -- 0) or (1 -- 0 -- 1) or (0 -- 1 -- 1)
            //   (2) 4 = 1 (three space points) + 3 (2 space points) (1 -- 1 -- 0) and (1 -- 0 -- 1) and (0 -- 1 -- 1) and (1 -- 1 -- 1)
            // but not
            //   (1) 4 = multiple space points per station but not on every station ( 2 -- 0 -- 2) and so on
            //   (2) >4 track segment options, indicating a multitrack event
            if (nts1 == 1 &&
                ((nts2 == 1 || nts2 == 4) && nts2sp2 != 4) &&
                ((nts3 == 1 || nts3 == 4) && nts3sp2 != 4))
              goodEvent = true;
            if (goodEvent == true)
              goodclust++;
            else
              badclust++;
          }
          else
          { // for SingelTrackAlignment
            // ONE CLUSTER PER PLANE
            // If there are more clusters than sensors, skip event
            if (clusters.size() == nPlanes)
            {
              for (auto i : clustMap)
              {
                if (i.second != 1)
                {
                  goodEvent = false;
                  break;
                }
                else
                  goodEvent = true;
              }
              if (goodEvent == true)
                goodclust++;
              else
              {
                badclust++;
              }
            }
            else
              badclust++;
          }

          // Instance of single track algorithm
          emph::SingleTrackAlgo algo = emph::SingleTrackAlgo(fEvtNum, nStations, nPlanes);

          clusters.clear();
          clustMap.clear();
          // Reconstructed hits
          if (goodEvent)
          {
            for (auto t : trksegs)
            {
              bool shortTrackSeg = true;

              if (t->region == rb::Region::kRegion1)
                tsvcut.push_back(t);
              if (t->region == rb::Region::kRegion2)
              {
                // If there is only one track segment, push back
                // If there are more, choose the one with space points in only stations 2 and 3
                // The space point in station 4 may be biased from the magnetic field
                if (nts2 == 1)
                  tsvcut.push_back(t); // Only one combination
                else
                {
                  if (fShortOn)
                  {
                    for (size_t i = 0; i < t->NSpacePoints(); i++)
                    {
                      if (t->GetSpacePoint(i)->Station() == 4)
                        shortTrackSeg = false;
                    }
                    if (shortTrackSeg)
                      tsvcut.push_back(t);
                  }
                  else
                  {
                    tsvcut.push_back(t);
                    if (t->NSpacePoints() == 3)
                      tsvcut.push_back(t); // Space point in every station
                  }
                }
              }
              if (t->region == rb::Region::kRegion3)
              {
                // If there is only one track segment, push back
                // If there are more, choose the one with more (3) space points
                if (nts3 == 1)
                  tsvcut.push_back(t); // Only one combination
                else
                {
                  if (t->NSpacePoints() == 3)
                    tsvcut.push_back(t);
                }
              }
            }

            // Now make tracks
            // Eventually beamtrk should be fixed later with SpillInfo
            trackv->clear();
            rb::Track beamtrk;
            std::vector<rb::TrackSegment> tsvec;

            auto t1 = *tsvcut[0];
            tsvec.push_back(t1);
            algo.SetBeamTrk(t1, fPBeamTmp);
            beamtrk.Add(t1);
            beamtrk.mom = t1.mom;   // SetP(t1.P());
            beamtrk.vtx = t1.vtx;   // SetVtx(t1.Vtx());
            beamtrk.chi2 = t1.chi2; // SetChi2(t1.Chi2());
            // fill position and momentum projected to the center of the target
            beamtrk.momTrgt = t1.mom;
            double posAtTrgt[3];
            // dz should be positive since we are projecting forward to the target
            double dz = fTrgtZ - t1.pointB.Z();
            posAtTrgt[2] = fTrgtZ;
            posAtTrgt[0] = (t1.mom.X() / t1.mom.Z()) * dz + t1.pointB.X();
            posAtTrgt[1] = (t1.mom.Y() / t1.mom.Z()) * dz + t1.pointB.Y();

            beamtrk.posTrgt.SetCoordinates(posAtTrgt);
            AddSSDLineSegmentsToTrack(beamtrk);
            trackv->push_back(beamtrk);

            rb::Track sectrk;
            auto t2 = *tsvcut[1];
            auto t3 = *tsvcut[2];
            tsvec.push_back(t2);
            tsvec.push_back(t3);
            int pm;
            if (fPBeamTmp > 0)
              pm = 1;
            else
              pm = -1;

            algo.SetRecoTrk(t2, t3, pm);
            // add TrackSegments to the track
            sectrk.Add(t2);
            sectrk.Add(t3);
            AddSSDLineSegmentsToTrack(beamtrk);
            
            sectrk.mom = t2.mom; // this should come from an analysis of the bend angle between track segments 1 and 2.
            auto v = algo.SetTrackInfo(tsvec[0], tsvec[1]);
            sectrk.vtx.SetCoordinates(v); // this should come from a calculation of the intersection or point of closest approach between track segments 0 and 1.

            // fill position and momentum projected to the center of the target
            sectrk.mom = t2.mom;
            sectrk.momTrgt = t2.mom;
            // dz should be negative since we're projecting back to the target
            dz = fTrgtZ - t2.pointA.Z();
            posAtTrgt[0] = (t2.mom.X() / t2.mom.Z()) * dz + t2.pointA.X();
            posAtTrgt[1] = (t2.mom.Y() / t2.mom.Z()) * dz + t2.pointA.Y();

            sectrk.posTrgt.SetCoordinates(posAtTrgt);
            sectrk.chi2 = t2.chi2 + t3.chi2;
            AddSSDLineSegmentsToTrack(sectrk);
            trackv->push_back(sectrk);
          }
        } // clust not empty
      } // try
      catch (...)
      {
      }

    } // want plots
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeSingleTracks)
