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
#include "Simulation/Particle.h"
#include "TrackReco/SingleTrackAlgo.h"
#include "StandardRecord/SRTrackSegment.h"

using namespace emph;

/// Package to illustrate how to write modules
namespace emph {

  class MakeTrackSegmentsForEfficiency : public art::EDProducer {
  public:
    explicit MakeTrackSegmentsForEfficiency(fhicl::ParameterSet const& pset);
    ~MakeTrackSegmentsForEfficiency() {};

    void produce(art::Event& evt);
    void reconfigure(const fhicl::ParameterSet& pset);
    void beginRun(art::Run& run);
    void beginJob();
    void endJob();

  private:
    void groupClusters();
    void groupLinesegs(bool all);
    void maskClustAndSegs(art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm, const rb::SSDCluster& clust, rb::LineSegment lineseg_tmp);
    bool readClustAndSegs(art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm, const rb::SSDCluster& clust, rb::LineSegment lineseg_tmp);
    void resizeGroups();
    void updateSps(emph::geo::Geometry* emgeo);
    void fillPlots(std::vector<rb::TrackSegment> tstmp, rb::Region kRegion);

    TTree* spacepoint;
    int run, subrun, event;
    int fEvtNum;
    std::vector<double> chi2;
    int chi2lessthan5_1 = 0;
    int chi2lessthan5_2 = 0;
    int chi2lessthan5_3 = 0;

    TH1D* strips1;
    TH1D* strips2;
    TH1D* strips3;
    TH1D* strips4;
    TH1D* strips5;
    TH1D* strips6;
    TH1D* strips7;
    TH1D* strips8;
    TH1D* strips9;
    TH1D* strips10;
    TH1D* strips11;
    TH1D* strips12;
    TH1D* strips13;
    TH1D* strips14;
    TH1D* strips15;
    TH1D* strips16;
    TH1D* strips17;
    TH1D* strips18;
    TH1D* strips19;
    TH1D* strips20;
    TH1D* strips21;
    int fStrip;

    TH1D* dist;

    TH2D* estXYHist;

    TH2D* largeEst;
    TH2D* normEst;

    TH2D* sph2;
    TH2D* sph3;
    TH2D* sp4;
    TH2D* sp5;
    TH2D* sp6;
    TH2D* sp7;

    TTree* norm_dist;
    TTree* large_dist;
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
    int masked_zpos = 0;

    std::map<int, std::map<std::pair<int, int>, int>> clustMapAtLeastOne;

    bool fMakePlots;
    int evts = 0;
    int hasclusters = 0;
    int usableclust = 0;
    int sps = 0;
    size_t nPlanes;
    size_t nStations;

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
    : EDProducer{pset},
      fCheckClusters (pset.get< bool >("CheckClusters")),
      fClusterLabel  (pset.get< std::string >("ClusterLabel")),
      fG4Label       (pset.get< std::string >("G4Label")),
      fMaxClust      (pset.get< size_t >("MaxClust")),
      fMaskedStation (pset.get< int >("MaskedStation")),
      fMaskedPlane   (pset.get< int >("MaskedPlane")),
      fMaskedSensor  (pset.get< int >("MaskedSensor"))
  {
    std::cout << "FCL pset dump: " << pset.to_indented_string() << std::endl;
    this->produces< std::vector<rb::LineSegment>>();
    this->produces< std::vector<rb::SpacePoint>>();
    this->produces< std::vector<rb::TrackSegment>>();
  }

  //......................................................................
  void MakeTrackSegmentsForEfficiency::beginRun(art::Run& run)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();
    nPlanes = emgeo->NSSDPlanes();
    nStations = emgeo->NSSDStations();
  }

  //......................................................................
  void emph::MakeTrackSegmentsForEfficiency::beginJob()
  {
    std::cerr << "Starting MakeTrackSegmentsForEfficiency" << std::endl;

    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint", "");
    spacepoint->Branch("run", &run, "run/I");
    spacepoint->Branch("subrun", &subrun, "subrun/I");
    spacepoint->Branch("event",  &event, "event/I");
    spacepoint->Branch("chi2",   &chi2,   "chi2/I");

    std::string sensor_num = std::to_string(fMaskedStation) + "_"
      + std::to_string(fMaskedPlane) + "_"
      + std::to_string(fMaskedSensor);

    std::string norm_x = "norm_x_" + sensor_num;
    std::string norm_y = "norm_y_" + sensor_num;
    norm_dist = tfs->make<TTree>("dist", "");
    norm_dist->Branch(norm_x.c_str(), &best_x_pos, "x_pos/D");
    norm_dist->Branch(norm_y.c_str(), &best_y_pos, "y_pos/D");

    std::string large_x = "large_x_" + sensor_num;
    std::string large_y = "large_y_" + sensor_num;
    large_dist = tfs->make<TTree>("large dist", "");
    large_dist->Branch(large_x.c_str(), &best_x_pos, "x_pos/D");
    large_dist->Branch(large_y.c_str(), &best_y_pos, "y_pos/D");

    std::string distTitleStr = "Point to line distance " 
      + std::to_string(fMaskedStation) + "/"
      + std::to_string(fMaskedPlane) + "/"
      + std::to_string(fMaskedSensor);
    dist = tfs->make<TH1D>("min_dist", "Distance point to line segment", 101, -10, 10);
    dist->GetXaxis()->SetTitle("Distance (mm)");
    dist = tfs->make<TH1D>("min_dist", "Distance point to line segment", 100, -10, 10);
    dist->GetXaxis()->SetTitle("Distance between prediction and actual (mm) ");
    dist->SetTitle(distTitleStr.c_str());

    strips1 = tfs->make<TH1D>("strips_2_0_0", "Number of hits per strip", 640, 0, 639);
    strips1->GetXaxis()->SetTitle("Strip Number");

    strips2 = tfs->make<TH1D>("strips_2_1_0", "Number of hits per strip", 640, 0, 639);
    strips2->GetXaxis()->SetTitle("Strip Number");
 
    strips3 = tfs->make<TH1D>("strips_2_2_0", "Number of hits per strip", 640, 0, 639);
    strips3->GetXaxis()->SetTitle("Strip Number");

    strips4 = tfs->make<TH1D>("strips_3_0_0", "Number of hits per strip", 640, 0, 639);
    strips4->GetXaxis()->SetTitle("Strip Number");

    strips5 = tfs->make<TH1D>("strips_3_1_0", "Number of hits per strip", 640, 0, 639);
    strips5->GetXaxis()->SetTitle("Strip Number");

    strips6 = tfs->make<TH1D>("strips_3_2_0", "Number of hits per strip", 640, 0, 639);
    strips6->GetXaxis()->SetTitle("Strip Number");

    strips7 = tfs->make<TH1D>("strips_4_0_0", "Number of hits per strip", 640, 0, 639);
    strips7->GetXaxis()->SetTitle("Strip Number");

    strips8 = tfs->make<TH1D>("strips_4_1_0", "Number of hits per strip", 640, 0, 639);
    strips8->GetXaxis()->SetTitle("Strip Number");

    strips9 = tfs->make<TH1D>("strips_5_0_0", "Number of hits per strip", 640, 0, 639);
    strips9->GetXaxis()->SetTitle("Strip Number");

    strips10 = tfs->make<TH1D>("strips_5_1_0", "Number of hits per strip", 640, 0, 639);
    strips10->GetXaxis()->SetTitle("Strip Number");

    strips11 = tfs->make<TH1D>("strips_5_1_1", "Number of hits per strip", 640, 0, 639);
    strips11->GetXaxis()->SetTitle("Strip Number");

    strips12 = tfs->make<TH1D>("strips_5_2_0", "Number of hits per strip", 640, 0, 639);
    strips12->GetXaxis()->SetTitle("Strip Number");

    strips13 = tfs->make<TH1D>("strips_5_2_1", "Number of hits per strip", 640, 0, 639);
    strips13->GetXaxis()->SetTitle("Strip Number");

    strips14 = tfs->make<TH1D>("strips_6_0_0", "Number of hits per strip", 640, 0, 639);
    strips14->GetXaxis()->SetTitle("Strip Number");

    strips15 = tfs->make<TH1D>("strips_6_1_0", "Number of hits per strip", 640, 0, 639);
    strips15->GetXaxis()->SetTitle("Strip Number");

    strips16 = tfs->make<TH1D>("strips_6_1_1", "Number of hits per strip", 640, 0, 639);
    strips16->GetXaxis()->SetTitle("Strip Number");

    strips17 = tfs->make<TH1D>("strips_6_2_0", "Number of hits per strip", 640, 0, 639);
    strips17->GetXaxis()->SetTitle("Strip Number");

    strips18 = tfs->make<TH1D>("strips_6_2_1", "Number of hits per strip", 640, 0, 639);
    strips18->GetXaxis()->SetTitle("Strip Number");

    strips19 = tfs->make<TH1D>("strips_7_0_0", "Number of hits per strip", 640, 0, 639);
    strips19->GetXaxis()->SetTitle("Strip Number");

    strips20 = tfs->make<TH1D>("strips_7_1_0", "Number of hits per strip", 640, 0, 639);
    strips20->GetXaxis()->SetTitle("Strip Number");

    strips21 = tfs->make<TH1D>("strips_7_1_1", "Number of hits per strip", 640, 0, 639);
    strips21->GetXaxis()->SetTitle("Strip Number");

    std::string estTitleStr = "Estimated Position " 
      + std::to_string(fMaskedStation) + "/" 
      + std::to_string(fMaskedPlane) + "/" 
      + std::to_string(fMaskedSensor);
    estXYHist = tfs->make<TH2D>("xyHist", "Estimated Position", 50, -100.0, 100.0, 50, -100.0, 100.0);
    estXYHist->GetXaxis()->SetTitle("Estimated X (mm)");
    estXYHist->GetYaxis()->SetTitle("Estimated Y (mm)");
    estXYHist->SetTitle(estTitleStr.c_str());
    estXYHist->SetOption("COLZ");
    estXYHist->SetStats(0);

    std::string largeEstTitleStr = "Estimated Position (Large Distance) " 
      + std::to_string(fMaskedStation) + "/"
      + std::to_string(fMaskedPlane) + "/"
      + std::to_string(fMaskedSensor);
    largeEst = tfs->make<TH2D>("largeXYHist", "Estimated Position (Large Distance)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    largeEst->GetXaxis()->SetTitle("Estimated X (mm)");
    largeEst->GetYaxis()->SetTitle("Estimated Y (mm)");
    largeEst->SetTitle(largeEstTitleStr.c_str());
    largeEst->SetOption("COLZ");
    largeEst->SetStats(0);

    std::string normEstTitleStr = "Estimated Position (Normal Distance) "
      + std::to_string(fMaskedStation) + "/"
      + std::to_string(fMaskedPlane) + "/"
      + std::to_string(fMaskedSensor);
    normEst = tfs->make<TH2D>("normXYHist", "Estimated Position (Normal Distance)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    normEst->GetXaxis()->SetTitle("Estimated X (mm)");
    normEst->GetYaxis()->SetTitle("Estimated Y (mm)");
    normEst->SetTitle(normEstTitleStr.c_str());
    normEst->SetOption("COLZ");
    normEst->SetStats(0);

    sph2 = tfs->make<TH2D>("sp2", "Position (Station 2)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    sph2->GetXaxis()->SetTitle("X (mm)");
    sph2->GetYaxis()->SetTitle("Y (mm)");
    sph2->SetOption("COLZ");
    sph2->SetStats(0);

    sph3 = tfs->make<TH2D>("sp3", "Position (Station 3)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    sph3->GetXaxis()->SetTitle("X (mm)");
    sph3->GetYaxis()->SetTitle("Y (mm)");
    sph3->SetOption("COLZ");
    sph3->SetStats(0);

    sp4 = tfs->make<TH2D>("sp4", "Position (Station 4)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    sp4->GetXaxis()->SetTitle("X (mm)");
    sp4->GetYaxis()->SetTitle("Y (mm)");
    sp4->SetOption("COLZ");
    sp4->SetStats(0);

    sp5 = tfs->make<TH2D>("sp5", "Position (Station 5)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    sp5->GetXaxis()->SetTitle("X (mm)");
    sp5->GetYaxis()->SetTitle("Y (mm)");
    sp5->SetOption("COLZ");
    sp5->SetStats(0);

    sp6 = tfs->make<TH2D>("sp6", "Position (Station 6)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    sp6->GetXaxis()->SetTitle("X (mm)");
    sp6->GetYaxis()->SetTitle("Y (mm)");
    sp6->SetOption("COLZ");
    sp6->SetStats(0);

    sp7 = tfs->make<TH2D>("sp7", "Position (Station 7)",  50, -100.0, 100.0, 50, -100.0, 100.0);
    sp7->GetXaxis()->SetTitle("X (mm)");
    sp7->GetYaxis()->SetTitle("Y (mm)");
    sp7->SetOption("COLZ");
    sp7->SetStats(0);
  }

  //......................................................................
  void emph::MakeTrackSegmentsForEfficiency::endJob()
  {
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events: " << evts << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with clusters: " << hasclusters << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with less than " << fMaxClust << " clusters: " << usableclust << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with space points: " << sps << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with chi2 < 5 for TrackSegment 1: " << chi2lessthan5_1 << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with chi2 < 5 for TrackSegment 2: " << chi2lessthan5_2 << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Number of events with chi2 < 5 for TrackSegment 3: " << chi2lessthan5_3 << std::endl;
    std::cout << "MakeTrackSegmentsForEfficiency: Masked Station/Plane/Sensor: " << fMaskedStation << "/" << fMaskedPlane << "/" << fMaskedSensor << std::endl;
  }

  //......................................................................
  void emph::MakeTrackSegmentsForEfficiency::produce(art::Event& evt)
  {
    tsv.clear();
    spv.clear();

    std::unique_ptr<std::vector<rb::LineSegment>> linesegv(new std::vector<rb::LineSegment>);
    std::unique_ptr<std::vector<rb::SpacePoint>> spacepointv(new std::vector<rb::SpacePoint>);
    std::unique_ptr<std::vector<rb::TrackSegment>> tracksegmentv(new std::vector<rb::TrackSegment>);

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    art::ServiceHandle<emph::geo::GeometryService> geo;
    auto emgeo = geo->Geo();

    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

    art::ServiceHandle<emph::AlignService> align;
    auto emalign = align->GetAlign();

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
          
            //std::cout << clust.Station() << " " << clust.Plane() << " " << clust.Sensor() << std::endl; 
            if (clust.Station() == 2 && clust.Plane() == 0 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips1->Fill(fStrip);
            }

            if (clust.Station() == 2 && clust.Plane() == 1 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips2->Fill(fStrip);
            }

            if (clust.Station() == 2 && clust.Plane() == 2 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips3->Fill(fStrip);
            }

            if (clust.Station() == 3 && clust.Plane() == 0 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips4->Fill(fStrip);
            }

            if (clust.Station() == 3 && clust.Plane() == 1 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips5->Fill(fStrip);
            }

            if (clust.Station() == 3 && clust.Plane() == 2 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips6->Fill(fStrip);
            }

            if (clust.Station() == 4 && clust.Plane() == 0 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips7->Fill(fStrip);
            }

            if (clust.Station() == 4 && clust.Plane() == 1 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips8->Fill(fStrip);
            }

            if (clust.Station() == 5 && clust.Plane() == 0 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips9->Fill(fStrip);
            }

            if (clust.Station() == 5 && clust.Plane() == 1 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips10->Fill(fStrip);
            }

            if (clust.Station() == 5 && clust.Plane() == 1 && clust.Sensor() == 1) {
              fStrip = clust.AvgStrip();
              strips11->Fill(fStrip);
            }

            if (clust.Station() == 5 && clust.Plane() == 2 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips12->Fill(fStrip);
            }

            if (clust.Station() == 5 && clust.Plane() == 2 && clust.Sensor() == 1) {
              fStrip = clust.AvgStrip();
              strips13->Fill(fStrip);
            }

            if (clust.Station() == 6 && clust.Plane() == 0 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips14->Fill(fStrip);
            }

            if (clust.Station() == 6 && clust.Plane() == 1 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips15->Fill(fStrip);
            }

            if (clust.Station() == 6 && clust.Plane() == 1 && clust.Sensor() == 1) {
              fStrip = clust.AvgStrip();
              strips16->Fill(fStrip);
            }

            if (clust.Station() == 6 && clust.Plane() == 2 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips17->Fill(fStrip);
            }

            if (clust.Station() == 6 && clust.Plane() == 2 && clust.Sensor() == 1) {
              fStrip = clust.AvgStrip();
              strips18->Fill(fStrip);
            }

            if (clust.Station() == 7 && clust.Plane() == 0 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips19->Fill(fStrip);
            }

            if (clust.Station() == 7 && clust.Plane() == 1 && clust.Sensor() == 0) {
              fStrip = clust.AvgStrip();
              strips20->Fill(fStrip);
            }

            if (clust.Station() == 7 && clust.Plane() == 1 && clust.Sensor() == 1) {
              fStrip = clust.AvgStrip();
              strips21->Fill(fStrip);
            }

            if (clust.Station() == fMaskedStation &&
                clust.Plane() == fMaskedPlane &&
                clust.Sensor() == fMaskedSensor) {
              maskClustAndSegs(dgm, clust, lineseg_tmp);
            } else {
              int plane_id = (clust.Station() * 10) + clust.Plane();
              if (interacted.find(plane_id) != interacted.end()) {
                skip_evt = true;
                break;
              }

              interacted.insert(plane_id);
              if (readClustAndSegs(dgm, clust, lineseg_tmp)) {
                linesegv->push_back(linesegments.back());
              } else {
                std::cout << "Couldn't make line segment from Cluster?!?" << std::endl;
              }
            }
          }

          if (clusters.size() < fMaxClust && !skip_evt) {
            usableclust++;

            resizeGroups();

            for (size_t i = 0; i < clustMapAtLeastOne.size(); i++) {
              for (auto j : clustMapAtLeastOne[i]) {
                mf::LogDebug("MakeTrackSegmentsForEfficiency") << "Station " << i << ": " << j.second;
              }
            }

            mf::LogDebug("MakeTrackSegmentsForEfficiency") << "........";
            
            groupClusters();

            groupLinesegs(false);
            groupLinesegs(true);

            // Instance of single track algorithm
            emph::SingleTrackAlgo algo = emph::SingleTrackAlgo(fEvtNum, nStations, nPlanes);

            // Make reconstructed hits
            spv = algo.MakeHits(ls_group, cl_group);

            for (auto sp : spv) {
              spacepointv->push_back(sp);
              if (sp.Station() == 2) { sph2->Fill(sp.Pos()[0], sp.Pos()[1]); }
              else if (sp.Station() == 3) { sph3->Fill(sp.Pos()[0], sp.Pos()[1]); }
              else if (sp.Station() == 4) { sp4->Fill(sp.Pos()[0], sp.Pos()[1]); }
              else if (sp.Station() == 5) { sp5->Fill(sp.Pos()[0], sp.Pos()[1]); }
              else if (sp.Station() == 6) { sp6->Fill(sp.Pos()[0], sp.Pos()[1]); }
              else if (sp.Station() == 7) { sp7->Fill(sp.Pos()[0], sp.Pos()[1]); }
            }
 
            // Reconstructed hits
            if (spv.size() > 0) {
              updateSps(emgeo);

              // Form lines and fill plots 
              std::vector<rb::TrackSegment> tstmp1 = algo.MakeTrackSeg(sp1);
              fillPlots(tstmp1, rb::Region::kRegion1);

              std::vector<rb::TrackSegment> tstmp2 = algo.MakeTrackSeg(sp2);
              fillPlots(tstmp2, rb::Region::kRegion2);

              std::vector<rb::TrackSegment> tstmp3 = algo.MakeTrackSeg(sp3);
              fillPlots(tstmp3, rb::Region::kRegion3);

              for (auto ts : tsv)
                tracksegmentv->push_back(ts);

              masked_region_segments = tstmp2;
              if (masked_zpos < emgeo->GetTarget()->Pos()(2)) {
                masked_region_segments = tstmp1;
              } else if (masked_zpos > emgeo->MagnetDSZPos()) {
                masked_region_segments = tstmp3;
              }
            }

            sp1.clear();
            sp2.clear();
            sp3.clear();
          } // clust < fMaxClust
          
          // Create line segment groups for ALL (including masked line segments)
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
              double min_dist = 10000.0;

              if (all_ls_group[fMaskedStation][fMaskedPlane].size() > 0) {
                min_dist = all_ls_group[fMaskedStation][fMaskedPlane][0]->DistanceToPoint(x_pos, y_pos);
                 
                for (unsigned int i = 1; i < all_ls_group[fMaskedStation][fMaskedPlane].size(); i++) {
                  double curr_dist = all_ls_group[fMaskedStation][fMaskedPlane][0]->DistanceToPoint(x_pos, y_pos);
                  if (std::abs(curr_dist) < std::abs(min_dist)) {
                    min_dist = curr_dist;
                  }
                  //std::cout << "Multiple line segments" << std::endl;
                }            
              }
              
              if (std::abs(min_dist) < std::abs(smallest_min_dist)) {
                best_x_pos = x_pos;
                best_y_pos = y_pos;
                smallest_min_dist = min_dist;
              }
            }
           
            if (smallest_min_dist != 10000.0) {            
              estXYHist->Fill(best_x_pos, best_y_pos);
              dist->Fill(smallest_min_dist);

              if (std::abs(smallest_min_dist) > 0.5) {
                large_dist->Fill();
                largeEst->Fill(best_x_pos, best_y_pos);
              } else {
                norm_dist->Fill();
                normEst->Fill(best_x_pos, best_y_pos);
              }
              auto ls = all_ls_group[fMaskedStation][fMaskedPlane][0];
              //if (std::abs(smallest_min_dist) < 0.75) {
                //std::cout << "PointA: (" << ls->x0.X() << "," << ls->x0.Y() << "," << ls->x0.Z() << ")" << std::endl;
                //std::cout << "PointB: (" << ls->x1.X() << "," << ls->x1.Y() << ","  << ls->x1.Z() << ")" << std::endl;
                //std::cout << "Predicted Position: (" << best_x_pos << "," << best_y_pos << "," << masked_zpos << ")" << std::endl;
                //std::cout << "Distance: " <<  smallest_min_dist << std::endl;
              //}
            }
          }

          ls_group.clear();
          all_ls_group.clear();
          cl_group.clear();
          linesegments.clear();
          all_linesegments.clear();
          clusters.clear();
          all_clusters.clear();
          clustMapAtLeastOne.clear();
        }
      } catch (...) {
        ls_group.clear();
        all_ls_group.clear();
        cl_group.clear();
        linesegments.clear();
        all_linesegments.clear();
        clusters.clear();
        all_clusters.clear();
        clustMapAtLeastOne.clear();
      }

      spacepoint->Fill();
      chi2.clear();
    }

    evt.put(std::move(linesegv));
    evt.put(std::move(spacepointv));
    evt.put(std::move(tracksegmentv));
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

  void emph::MakeTrackSegmentsForEfficiency::groupLinesegs(bool all)
  {
    if (all) {
      for (size_t i = 0; i < all_clusters.size(); i++) {
        int plane = all_clusters[i]->Plane();
        int station = all_clusters[i]->Station();

        if (station < 0 || static_cast<size_t>(station) >= all_ls_group.size()) continue;
        if (plane < 0 || static_cast<size_t>(plane) >= all_ls_group[station].size()) continue;
        
        all_ls_group[station][plane].push_back(&all_linesegments[i]);
      }
    } else {
      for (size_t i = 0; i < clusters.size(); i++) {
        int plane   = clusters[i]->Plane();
        int station = clusters[i]->Station();

        if (station < 0 || static_cast<size_t>(station) >= ls_group.size()) continue;
        if (plane < 0 || static_cast<size_t>(plane) >= ls_group[station].size()) continue;
        ls_group[station][plane].push_back(&linesegments[i]);
      }
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

    for (size_t i = 0; i < nStations; i++) {
      cl_group[i].resize(nPlanes);
      ls_group[i].resize(nPlanes);
    }
 
    all_cl_group.resize(nStations);
    all_ls_group.resize(nStations);

    for (size_t i = 0; i < nStations; i++) {
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

  void emph::MakeTrackSegmentsForEfficiency::fillPlots(std::vector<rb::TrackSegment> tstmp, rb::Region kRegion) {
    for (auto i : tstmp) {
      i.region = kRegion;
      tsv.push_back(i);
      chi2.push_back(i.chi2);
      if (i.chi2 < 5) {
        if (kRegion == rb::Region::kRegion1) {
          chi2lessthan5_1++; 
        } else if (kRegion == rb::Region::kRegion2) {
          chi2lessthan5_2++;
        } else {
          chi2lessthan5_3++; 
        }
      }
    }
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::MakeTrackSegmentsForEfficiency)
