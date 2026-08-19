////////////////////////////////////////////////////////////////////////
///// \brief   Producer module to construct single-particle tracks
/////
///// \author  $Author: robert chirco $
//////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>
#include <set>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraph2D.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
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
#include "Simulation/Particle.h"
#include "TrackReco/SingleTrackAlgo.h"
#include "StandardRecord/SRTrackSegment.h"

using namespace emph;

/// Package to illustrate how to write modules
namespace emph {

  class MakeTrackSegmentsForEfficiency : public art::EDAnalyzer {
  public:
    explicit MakeTrackSegmentsForEfficiency(fhicl::ParameterSet const& pset);
    ~MakeTrackSegmentsForEfficiency();

    void analyze(art::Event const& evt);
    void reconfigure(const fhicl::ParameterSet& pset);
    void beginRun(art::Run const& run);
    void beginJob() override;
    void endJob();

  private:
    void groupClusters();
    void groupLinesegs();
    void maskClustAndSegs(art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm, const rb::SSDCluster& clust, rb::LineSegment lineseg_tmp);
    bool readClustAndSegs(art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm, const rb::SSDCluster& clust, rb::LineSegment lineseg_tmp);
    void resizeGroups();
    void updateSps(emph::geo::Geometry* emgeo);

    TTree* spacepoint;
    int run, subrun, event;
    int fEvtNum;

    std::vector<std::vector<std::vector<TH1D*>>> min_dist;
    std::vector<std::vector<std::vector<TH2D*>>> est_pos;
    std::vector<std::vector<std::vector<TH2D*>>> est_pos_full;

    double best_x_pos = 10000.0;
    double best_y_pos = 10000.0;

    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> linesegments;
    std::vector<rb::SpacePoint> spv;
    std::vector<rb::TrackSegment> tsv;
    std::vector<std::vector<std::vector<const rb::SSDCluster*>>> cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*>>> ls_group;
    double sectrkp[3];
    double sectrkvtx[3];

    std::vector<const rb::SSDCluster*> all_clusters;
    std::vector<rb::LineSegment> all_linesegments;
    std::vector<std::vector<std::vector<const rb::SSDCluster*>>> all_cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*>>> all_ls_group;
    double masked_zpos = 0;

    std::map<int, std::map<std::pair<int, int>, int>> clustMapAtLeastOne;

    bool fMakePlots;
    int evts = 0;
    int hasclusters = 0;
    int usableclust = 0;
    int sps = 0;
    int nPlanes;
    int nStations;

    // fcl parameters
    bool fCheckClusters;
    std::string fClusterLabel;
    std::string fG4Label;
    size_t fMaxClust;
    int fMaskedStation;
    int fMaskedPlane;
    int fMaskedSensor;

    // reco info for lines
    std::vector<rb::SpacePoint> sp1;
    std::vector<rb::SpacePoint> sp2;
    std::vector<rb::SpacePoint> sp3;
    double pbeam[3];
  };

  //.......................................................................
  emph::MakeTrackSegmentsForEfficiency::MakeTrackSegmentsForEfficiency(fhicl::ParameterSet const& pset)
    : EDAnalyzer{pset},
      fCheckClusters (pset.get< bool >("CheckClusters")),
      fClusterLabel  (pset.get< std::string >("ClusterLabel")),
      fG4Label       (pset.get< std::string >("G4Label")),
      fMaxClust      (pset.get< size_t >("MaxClust")),
      fMaskedStation (pset.get< int >("MaskedStation")),
      fMaskedPlane   (pset.get< int >("MaskedPlane")),
      fMaskedSensor  (pset.get< int >("MaskedSensor"))
  {
    std::cout << "FCL pset dump: " << pset.to_indented_string() << std::endl;
  }

  //......................................................................

  MakeTrackSegmentsForEfficiency::~MakeTrackSegmentsForEfficiency()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................
  void MakeTrackSegmentsForEfficiency::beginRun(art::Run const& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    geo::Geometry* emgeo = geo->Geo();

    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();

    min_dist.resize(nStations, std::vector<std::vector<TH1D*>>(nPlanes, std::vector<TH1D*>(2, nullptr)));
    est_pos.resize(nStations, std::vector<std::vector<TH2D*>>(nPlanes, std::vector<TH2D*>(2, nullptr)));
    est_pos_full.resize(nStations, std::vector<std::vector<TH2D*>>(nPlanes, std::vector<TH2D*>(2, nullptr)));
  }

  //......................................................................
  void emph::MakeTrackSegmentsForEfficiency::beginJob()
  {
    std::cerr << "Starting MakeTrackSegmentsForEfficiency" << std::endl;
  }

  //......................................................................
  void emph::MakeTrackSegmentsForEfficiency::endJob()
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    geo::Geometry* emgeo = geo->Geo();

    for (int fMaskedStation = 2; fMaskedStation < nStations; fMaskedStation++) {
      auto station = emgeo->GetSSDStation(fMaskedStation);
      int num_planes = station->NPlanes();

      for (int fMaskedPlane = 0; fMaskedPlane < num_planes; fMaskedPlane++) {
        auto plane = station->GetPlane(fMaskedPlane);
        int num_sensors = plane->NSSDs();

        for (int fMaskedSensor = 0; fMaskedSensor < num_sensors; fMaskedSensor++) {
          if (est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor] != nullptr) {
            est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->Divide(est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor]);
          }
        }
      }
    }



    std::cout << "MakeTrackSegmentsForEfficiency: Number of events: " << evts << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with clusters: " << hasclusters << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with less than " << fMaxClust << " clusters: " << usableclust << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with space points: " << sps << std::endl;
  }

  //......................................................................
  void emph::MakeTrackSegmentsForEfficiency::analyze(art::Event const& evt)
  {
    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

    art::ServiceHandle<emph::AlignService> align;
    auto emalign = align->GetAlign();

    art::ServiceHandle<emph::geo::GeometryService> geo;
    geo::Geometry* emgeo = geo->Geo();

    art::ServiceHandle<art::TFileService> tfs;

    std::unique_ptr<std::vector<rb::LineSegment>> linesegv(new std::vector<rb::LineSegment>);
    std::unique_ptr<std::vector<rb::SpacePoint>> spacepointv(new std::vector<rb::SpacePoint>);
    std::unique_ptr<std::vector<rb::TrackSegment>> tracksegmentv(new std::vector<rb::TrackSegment>);

    for (int fMaskedStation = 2; fMaskedStation < nStations; fMaskedStation++) {
      auto station = emgeo->GetSSDStation(fMaskedStation);
      int num_planes = station->NPlanes();

        for (int fMaskedPlane = 0; fMaskedPlane < num_planes; fMaskedPlane++) {
          auto plane = station->GetPlane(fMaskedPlane);
          int num_sensors = plane->NSSDs();

          for (int fMaskedSensor = 0; fMaskedSensor < num_sensors; fMaskedSensor++) {
            if (min_dist[fMaskedStation][fMaskedPlane][fMaskedSensor] == nullptr) {
              std::string desc_str = "h" + std::to_string(fMaskedStation) + "_"
                + std::to_string(fMaskedPlane) + "_"
                + std::to_string(fMaskedSensor);

              min_dist[fMaskedStation][fMaskedPlane][fMaskedSensor] = tfs->make<TH1D>(desc_str.c_str(), "Distance point to line segment", 101, -1, 1);
              min_dist[fMaskedStation][fMaskedPlane][fMaskedSensor]->GetXaxis()->SetTitle("Distance Point to Line (mm)");

              std::string est_desc_str = "d" + desc_str;
              std::string estTitleStr = "Estimated Position "
                + std::to_string(fMaskedStation) + "/"
                + std::to_string(fMaskedPlane) + "/"
                + std::to_string(fMaskedSensor);
              est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor] = tfs->make<TH2D>(est_desc_str.c_str(), "Estimated Position", 50, -100.0, 100.0, 50, -100.0, 100.0);
              est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->GetXaxis()->SetTitle("Estimated X (mm)");
              est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->GetYaxis()->SetTitle("Estimated Y (mm)");
              est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->SetTitle(estTitleStr.c_str());
              est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->SetOption("COLZ");
              est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->SetStats(0);

              std::string fest_desc_str = "f" + desc_str;
              std::string divideTitleStr = "Divided Position "
                + std::to_string(fMaskedStation) + "/"
                + std::to_string(fMaskedPlane) + "/"
                + std::to_string(fMaskedSensor);
              est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor] = tfs->make<TH2D>(fest_desc_str.c_str(), "Divided Positions", 50, -100.0, 100.0, 50, -100.0, 100.0);
              est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor]->GetXaxis()->SetTitle("X (mm)");
              est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor]->GetYaxis()->SetTitle("Y (mm)");
              est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor]->SetTitle(divideTitleStr.c_str());
              est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor]->SetOption("COLZ");
            }

            tsv.clear();
            spv.clear();
            ls_group.clear();
            all_ls_group.clear();
            cl_group.clear();
            linesegments.clear();
            all_linesegments.clear();
            clusters.clear();
            all_clusters.clear();
            clustMapAtLeastOne.clear();
            sp1.clear();
            sp2.clear();
            sp3.clear();

            run = evt.run();
            subrun = evt.subRun();
            event = evt.event();
            fEvtNum = evt.id().event();

            // Determine predicted (x,y) positions of masked sensor
            masked_zpos = emgeo
              ->GetSSDStation(fMaskedStation)
              ->GetPlane(fMaskedPlane)
              ->SSD(fMaskedSensor)
              ->Pos().Z();

            fMakePlots = true;
            if (fMakePlots) {
              if (fCheckClusters) {
                auto hasclusters = evt.getHandle<std::vector<rb::SSDCluster>>(fClusterLabel);
                if (!hasclusters) {
                  mf::LogError("HasSSDClusters") << "No clusters found in event but CheckClusters set to true!";
                  abort();
                }
              }

              art::Handle<std::vector<rb::SSDCluster>> clustH;

              try {
                evt.getByLabel(fClusterLabel, clustH);
                evts++;

                bool skip_evt = false;
                if (!clustH->empty()) {
                  hasclusters++;
                  std::vector<rb::TrackSegment> masked_region_segments;
                  rb::LineSegment lineseg_tmp = rb::LineSegment();

                  std::set<int> interacted;

                  linesegments.reserve(clustH->size());
                  all_linesegments.reserve(clustH->size());

                  for (size_t idx = 0; idx < clustH->size(); ++idx) {
                    const rb::SSDCluster& clust = (*clustH)[idx];

                    int plane_id = (clust.Station() * 10) + clust.Plane();
                    if (interacted.find(plane_id) != interacted.end()) {
                      skip_evt = true;
                      break;
                    }

                    interacted.insert(plane_id);

                    if (clust.Station() == fMaskedStation &&
                        clust.Plane() == fMaskedPlane &&
                        clust.Sensor() == fMaskedSensor) {
                      maskClustAndSegs(dgm, clust, lineseg_tmp);
                    } else {
                      if (readClustAndSegs(dgm, clust, lineseg_tmp)) {
                        linesegv->push_back(linesegments.back());
                      } else {
                        std::cout << "Couldn't make line segment from Cluster?!?" << std::endl;
                      }
                    }
                  }

                  resizeGroups();
                  if (clusters.size() < fMaxClust && !skip_evt) {
                    usableclust++;

                    for (size_t i = 0; i < clustMapAtLeastOne.size(); i++) {
                      for (auto j : clustMapAtLeastOne[i]) {
                        mf::LogDebug("MakeTrackSegmentsForEfficiency") << "Station " << i << ": " << j.second;
                      }
                    }

                    mf::LogDebug("MakeTrackSegmentsForEfficiency") << "........";

                    groupClusters();

                    groupLinesegs();

                    // Instance of single track algorithm
                    emph::SingleTrackAlgo algo = emph::SingleTrackAlgo(fEvtNum, nStations, nPlanes);

                    // Make reconstructed hits
                    spv = algo.MakeHits(ls_group, cl_group);

                    for (auto sp : spv) {
                      spacepointv->push_back(sp);
                    }

                    // Reconstructed hits
                    if (spv.size() > 0) {
                      updateSps(emgeo);

                      // Form lines
                      std::vector<rb::TrackSegment> tstmp1 = algo.MakeTrackSeg(sp1);

                      std::vector<rb::TrackSegment> tstmp2 = algo.MakeTrackSeg(sp2);

                      std::vector<rb::TrackSegment> tstmp3 = algo.MakeTrackSeg(sp3);

                      for (auto ts : tsv)
                        tracksegmentv->push_back(ts);

                      masked_region_segments = tstmp2;
                      if (masked_zpos < emgeo->GetTarget()->Pos()(2)) {
                        masked_region_segments = tstmp1;
                      } else if (masked_zpos > emgeo->MagnetDSZPos()) {
                        masked_region_segments = tstmp3;
                      }
                    }
                  } // clust < fMaxClust

                  //Create line segment groups for ALL (including masked line segments)
                  if (all_clusters.size() < fMaxClust && !skip_evt) {
                    best_x_pos = 10000.0;
                    best_y_pos = 10000.0;
                    double smallest_min_dist = 10000.0;

                    for (auto ts : masked_region_segments) {
                      double gradxz = (ts.pointA.X() - ts.pointB.X()) / (ts.pointA.Z() - ts.pointB.Z());
                      double gradyz = (ts.pointA.Y() - ts.pointB.Y()) / (ts.pointA.Z() - ts.pointB.Z());
                      double deltaz = masked_zpos - ts.pointA.Z();
                      double x_pos = (gradxz * deltaz) + ts.pointA.X();
                      double y_pos = (gradyz * deltaz) + ts.pointA.Y();

                      if (all_ls_group[fMaskedStation][fMaskedPlane].size() > 0) {
                        double dist = all_ls_group[fMaskedStation][fMaskedPlane][0]->DistanceToPoint(x_pos, y_pos);

                        if (std::abs(dist) < std::abs(smallest_min_dist)) {
                          best_x_pos = x_pos;
                          best_y_pos = y_pos;
                          smallest_min_dist = dist;
                        }
                      }
                    }

                    if (smallest_min_dist != 10000.0) {
                      double x0[3] = {best_x_pos, best_y_pos, masked_zpos};
                      bool on_detect = true;
                      if (dgm->Map()->IsPointOnDetector(fMaskedStation, fMaskedPlane, fMaskedSensor, x0)) {
                        min_dist[fMaskedStation][fMaskedPlane][fMaskedSensor]->Fill(smallest_min_dist);
                        est_pos_full[fMaskedStation][fMaskedPlane][fMaskedSensor]->Fill(best_x_pos, best_y_pos);
                        if (std::abs(smallest_min_dist) <= 0.2) {
                          est_pos[fMaskedStation][fMaskedPlane][fMaskedSensor]->Fill(best_x_pos, best_y_pos);
                        }
                      }
                    }

                  }
                }
              } catch (...) {   }

          }
        }
      }
    }
  }

  void emph::MakeTrackSegmentsForEfficiency::groupClusters()
  {
    for (size_t i = 0; i < clusters.size(); i++) {
      int plane = clusters[i]->Plane();
      int station = clusters[i]->Station();

      if (station < 0 || static_cast<size_t>(station) >= cl_group.size()) {
        mf::LogWarning("MakeTrackSegmentsForEfficiency")
          << "Skipping cluster with out-of-range station index " << station
          << " (cl_group size = " << cl_group.size() << ") at event " << fEvtNum;
        continue;
      }
      if (plane < 0 || static_cast<size_t>(plane) >= cl_group[station].size()) {
        mf::LogWarning("MakeTrackSegmentsForEfficiency")
          << "Skipping cluster with out-of-range plane index station=" << station
          << " plane=" << plane
          << " (nPlanes alloc per station = " << cl_group[station].size()
          << ") at event " << fEvtNum;
        continue;
      }

      cl_group[station][plane].push_back(clusters[i]);
    }

    for (size_t i = 0; i < all_clusters.size(); i++) {
      int plane = all_clusters[i]->Plane();
      int station = all_clusters[i]->Station();

      if (station < 0 || static_cast<size_t>(station) >= all_cl_group.size()) {
        mf::LogWarning("MakeTrackSegmentsForEfficiency")
          << "Skipping cluster with out-of-range station index " << station
          << " (cl_group size = " << all_cl_group.size() << ") at event " << fEvtNum;
        continue;
      }
      if (plane < 0 || static_cast<size_t>(plane) >= all_cl_group[station].size()) {
        mf::LogWarning("MakeTrackSegmentsForEfficiency")
          << "Skipping cluster with out-of-range plane index station=" << station
          << " plane=" << plane
          << " (nPlanes alloc per station = " << all_cl_group[station].size()
          << ") at event " << fEvtNum;
        continue;
      }

      all_cl_group[station][plane].push_back(all_clusters[i]);
    }
  }

  void emph::MakeTrackSegmentsForEfficiency::groupLinesegs()
  {
    for (size_t i = 0; i < all_clusters.size(); i++) {
      int plane = all_clusters[i]->Plane();
      int station = all_clusters[i]->Station();

      if (station < 0 || static_cast<size_t>(station) >= all_ls_group.size()) continue;
      if (plane < 0 || static_cast<size_t>(plane) >= all_ls_group[station].size()) continue;

      all_ls_group[station][plane].push_back(&all_linesegments[i]);
    }

    for (size_t i = 0; i < clusters.size(); i++) {
      int plane   = clusters[i]->Plane();
      int station = clusters[i]->Station();

      if (station < 0 || static_cast<size_t>(station) >= ls_group.size()) continue;
      if (plane < 0 || static_cast<size_t>(plane) >= ls_group[station].size()) continue;

      ls_group[station][plane].push_back(&linesegments[i]);
    }
  }

  void emph::MakeTrackSegmentsForEfficiency::maskClustAndSegs(art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm,
    const rb::SSDCluster& clust,
    rb::LineSegment lineseg_tmp)
  {
    mf::LogDebug("MakeTrackSegmentsForEfficiency")
      << "Skipping cluster due to mask Station: "
      << clust.Station() << " Plane: " << clust.Plane()
      << " Sensor: " << clust.Sensor() << std::endl;

    all_clusters.push_back(&clust);
    lineseg_tmp.SetSSDInfo(clust.Station(), clust.Plane(), clust.Sensor(), clust.MaxStrip());

    all_linesegments.push_back(lineseg_tmp);

    if (clust.AvgStrip() > 640) {
      throw art::Exception(art::errors::InvalidNumber)
        << "Skipping nonsense: cluster strip number > 640\n";
      abort();
    }

    if (!dgm->Map()->SSDClusterToLineSegment(clust, all_linesegments.back())) {
      std::cout << "Couldn't make line segment from Cluster?!?" << std::endl;
    }
  }

  bool emph::MakeTrackSegmentsForEfficiency::readClustAndSegs(art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm,
    const rb::SSDCluster& clust,
    rb::LineSegment lineseg_tmp)
  {
    ++clustMapAtLeastOne[clust.Station()][std::pair<int, int>(clust.Station(), clust.Plane())];

    clusters.push_back(&clust);
    all_clusters.push_back(&clust);

    lineseg_tmp.SetSSDInfo(clust.Station(), clust.Plane(), clust.Sensor(), clust.MaxStrip());
    linesegments.push_back(lineseg_tmp);
    all_linesegments.push_back(lineseg_tmp);

    if (clust.AvgStrip() > 640) {
      throw art::Exception(art::errors::InvalidNumber)
        << "Skipping nonsense: cluster strip number > 640\n";
      abort();
    }

    return ((dgm->Map()->SSDClusterToLineSegment(clust, linesegments.back()))
      && (dgm->Map()->SSDClusterToLineSegment(clust, all_linesegments.back())));
  }

  void emph::MakeTrackSegmentsForEfficiency::resizeGroups() {
    cl_group.resize(nStations);
    ls_group.resize(nStations);

    for (int i = 0; i < nStations; i++) {
      cl_group[i].resize(nPlanes);
      ls_group[i].resize(nPlanes);
    }

    all_cl_group.resize(nStations);
    all_ls_group.resize(nStations);

    for (int i = 0; i < nStations; i++) {
      all_cl_group[i].resize(nPlanes);
      all_ls_group[i].resize(nPlanes);
    }
  }

  void emph::MakeTrackSegmentsForEfficiency::updateSps(emph::geo::Geometry* emgeo) {
    sps++;
    for (size_t i = 0; i < spv.size(); i++) {
      if (emgeo->GetTarget()) {
        if (spv[i].Pos()[2] < emgeo->GetTarget()->Pos()(2)) { sp1.push_back(spv[i]); }
        if (spv[i].Pos()[2] > emgeo->GetTarget()->Pos()(2) && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) { sp2.push_back(spv[i]); }
        if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) { sp3.push_back(spv[i]); }
      } else {
        if (spv[i].Pos()[2] < 380.5) { sp1.push_back(spv[i]); }
        if (spv[i].Pos()[2] > 380.5 && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) { sp2.push_back(spv[i]); }
        if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) { sp3.push_back(spv[i]); }
      }
    }

    mf::LogDebug("MakeTrackSegmentsForEfficiency") << "sp1 size: " << sp1.size();
    mf::LogDebug("MakeTrackSegmentsForEfficiency") << "sp2 size: " << sp2.size();
    mf::LogDebug("MakeTrackSegmentsForEfficiency") << "sp3 size: " << sp3.size();
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrackSegmentsForEfficiency)
