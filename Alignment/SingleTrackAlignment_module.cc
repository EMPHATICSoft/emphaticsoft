////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to construct single-particle tracks
///       
/// \author  $Author: robert chirco $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>
#include <fstream>

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
#include "TMath.h"
//#include "TGeoMatrix.h"

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

#include "millepede_ii/Mille.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  ///
  class SingleTrackAlignment : public art::EDProducer {
  public:
    explicit SingleTrackAlignment(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~SingleTrackAlignment() {};
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void beginJob();
    void endJob();
    void Pulls(std::vector<rb::TrackSegment> trksegv);

  private:
  
    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
    art::ServiceHandle<emph::geo::GeometryService> geo;

    int         run,subrun,event;
    int         fEvtNum;

    size_t         nStations;
    size_t         nPlanes;

    std::map<std::pair<int, int>, int> clustMap;
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group;
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<const rb::LineSegment*> linesegments;
    std::vector<const rb::TrackSegment*> tracksegments;
    std::vector<const rb::Track*> tracks;
    std::vector<std::vector<std::vector<float> > > pullsorted;

    std::vector<rb::SpacePoint> spv;
    std::vector<rb::TrackSegment> tsv;
    std::vector<rb::TrackSegment> tsvnom;

    std::vector<std::vector<double>> sp1;
    std::vector<std::vector<double>> sp2;
    std::vector<std::vector<double>> sp3;

    //fcl parameters
    bool        fCheckLineSeg;    
    std::string fLineSegLabel;
    std::string fClusterLabel;
    std::string fTrackSegLabel;
    std::string fTrackLabel;
    bool        fSevenOn;
    bool        fUpstream; 
 
    //Millepede stuff
    Mille* m;
    std::vector<int> label;

    double targetz;
    double magnetusz;
    double magnetdsz;

    art::ServiceHandle<emph::AlignService> emalign;
    Align* align0 = emalign->GetAlign();

    int re = 1;

    int usingEvent = 0;
 };

  //.......................................................................
  
  emph::SingleTrackAlignment::SingleTrackAlignment(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckLineSeg      (pset.get< bool >("CheckLineSeg")),
    fLineSegLabel      (pset.get< std::string >("LineSegLabel")),
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fTrackSegLabel     (pset.get< std::string >("TrackSegLabel")),
    fTrackLabel        (pset.get< std::string >("TrackLabel")),
    fSevenOn           (pset.get< bool >("SevenOn")),
    fUpstream          (pset.get< bool >("Upstream"))
    {
      //this->produces< std::vector<rb::Track> >();
    }
  
  //......................................................................
  
//  SingleTrackAlignment::~SingleTrackAlignment()
//  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
//  }

  //......................................................................

  // void SingleTrackAlignment::reconfigure(const fhicl::ParameterSet& pset)
  // {    
  // }

  //......................................................................
  
  void SingleTrackAlignment::beginRun(art::Run& run)
  {
    auto emgeo = geo->Geo();
    nStations = emgeo->NSSDStations();
    nPlanes = emgeo->NSSDPlanes();

    if (!fSevenOn){
      nStations = nStations - 1;
      nPlanes = nPlanes - 2;
    }

    if (emgeo->GetTarget()) targetz = emgeo->GetTarget()->Pos()(2);
    else targetz = 380.5;    

    magnetusz = emgeo->MagnetUSZPos();
    magnetdsz = emgeo->MagnetDSZPos();

    int l=0;
    for (int ii=0; ii<(int)nStations; ii++){
      for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
        for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
          for (int dd=1; dd<=4; dd++){
            l = ii*1000 + jj*100 + kk*10 + dd;
            label.push_back(l);
          }
        }
      }
    }

  }

  //......................................................................
   
  void emph::SingleTrackAlignment::beginJob()
  {
    std::cerr<<"Starting SingleTrackAlignment"<<std::endl;

    m = new Mille("m004.bin",true,true);
  }
 
  //......................................................................
  
  void emph::SingleTrackAlignment::endJob()
  {
    delete m;

    std::cout<<"SingleTrackAlignment: Number of events used = "<<usingEvent<<std::endl;
  }

  //......................................................................

  void emph::SingleTrackAlignment::Pulls(std::vector<rb::TrackSegment> trksegv)
  {
    auto emgeo = geo->Geo();
    ru::RecoUtils r = ru::RecoUtils(fEvtNum);

    TVector3 a2(0.,0.,0.);
    TVector3 b2(0.,0.,0.);
    TVector3 ts2(0.,0.,0.);

    for (int ii=0; ii<(int)ls_group.size(); ii++){
      for (int jj=0; jj<(int)ls_group[ii].size(); jj++){
        for (int kk=0; kk<(int)ls_group[ii][jj].size(); kk++){
          TVector3 x0(ls_group[ii][jj][kk]->X0().X(),ls_group[ii][jj][kk]->X0().Y(),ls_group[ii][jj][kk]->X0().Z());
          TVector3 x1(ls_group[ii][jj][kk]->X1().X(),ls_group[ii][jj][kk]->X1().Y(),ls_group[ii][jj][kk]->X1().Z());

	  int sens = cl_group[ii][jj][kk]->Sensor();

	  if (fUpstream){
	    if (cl_group[ii][jj][kk]->Station() > 4) continue;
          }       

	  float uncer = cl_group[ii][jj][kk]->WgtRmsStrip()*0.06;
          mf::LogDebug("SingleTrackAlignment") <<"View: "<<emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->View() ;
	  auto sview = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->View();
	  //double phim = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->Rot(); //in rad

          x0(0) = -1*x0(0);
          x1(0) = -1*x1(0);

          TVector3 vec = x0 - x1;
          TVector3 posx(1.,0.,0.);
          Double_t ta = TMath::ATan2(posx.Y(),posx.X());
          Double_t tb = TMath::ATan2(vec.Y(),vec.X());
          Double_t tt = tb - ta;
          Double_t phim = 0.;
          if (tt < 0) phim = tt + 2.*TMath::Pi();
          else phim = tt;

          for (auto ts : trksegv) {
            double tsz = ts.Vtx()[2];

	    TVector3 a(ts.A()[0],ts.A()[1],ts.A()[2]);
            TVector3 b(ts.B()[0],ts.B()[1],ts.B()[2]);	    
            if (ts.RegLabel() == rb::Region::kRegion2){ a2 = a; b2 = b; ts2 = ts.P(); }

	  // pull = doca between s and ts
            double sensorz = x0(2); //s[2];
            if (x0(2) != x1(2)) std::cout<<"Rotated line segment --> using x0 for now"<<std::endl;

            if ((tsz < targetz && sensorz < targetz)
            || ((tsz > targetz && tsz < magnetusz) && (sensorz > targetz && sensorz < magnetusz))
            || (tsz > magnetdsz && sensorz > magnetdsz)){
	      if (ts.RegLabel() == rb::Region::kRegion3){ 
	        a = a2; 
                b = b2;
	      } 

              double f1[3]; double f2[3]; double f3[3];
              r.ClosestApproach(x0,x1,a,b,f1,f2,f3,"SSD",false); //TrackSegment");   
              float pull = sqrt((f3[0]-f2[0])*(f3[0]-f2[0])+(f3[1]-f2[1])*(f3[1]-f2[1])+(f3[2]-f2[2])*(f3[2]-f2[2]));

	      // @ sensorz what is ts xy 
	      // find signed distance from sensor xy to ts xy 
	      // find t where sensor z is

	      double t = ( sensorz - a(2) )/( b(2) - a(2) );
	      double tsx = a(0) + (b(0)-a(0))*t;
	      double tsy = a(1) + (b(1)-a(1))*t;

              a(0) = -1*a(0);
              b(0) = -1*b(0);
              tsx = -1*tsx;

	      // signed distance from point to a line 
	      double la = x1(1) - x0(1);
	      double lb = x0(0) - x1(0);
	      double lc = x0(1)*(x1(0)-x0(0)) - (x1(1)-x0(1))*x0(0);	  
	      float dsign = (la*tsx + lb*tsy + lc)/(sqrt(la*la + lb*lb));

	      pullsorted[ii][jj].push_back(dsign);

	      float lcd_x0 = -1.*TMath::Sin(phim);
	      float lcd_pxpz = -sensorz*TMath::Sin(phim);
	      float lcd_y0 = 1.*TMath::Cos(phim);
	      float lcd_pypz = sensorz*TMath::Cos(phim);

	      mf::LogDebug("SingleTrackAlignment") << "..........." ;
              mf::LogDebug("SingleTrackAlignment") << "pull = " << pull ;
              mf::LogDebug("SingleTrackAlignment") << "dsign = " << dsign ;
              mf::LogDebug("SingleTrackAlignment") << "sensorz = " << sensorz ;

              float gld_x; float gld_y; float gld_z;
	      float gld_phim;

	      if (sview == 1 || sview == 2 || sview == 4){ // U-VIEW
                gld_x = -1.*TMath::Sin(phim);
                gld_y = 1.*TMath::Cos(phim);
                gld_z = -1.*ts.P()[0]/ts.P()[2]*TMath::Sin(phim) + ts.P()[1]/ts.P()[2]*TMath::Cos(phim);
	        if (ts.RegLabel() == rb::Region::kRegion3){
		  gld_z = -1.*ts2(0)/ts2(2)*TMath::Sin(phim) + ts2(1)/ts2(2)*TMath::Cos(phim); 
		}
                gld_phim = -1.*TMath::Cos(phim) * tsx - 1.*TMath::Sin(phim) * tsy;
              }
              else{
		gld_x = 0.;
		gld_y = 0.;
		gld_z = 0.;
		gld_phim =  0.;
              }
       	
	      float mderlc[4] = {lcd_x0,lcd_pxpz,lcd_y0,lcd_pypz};
	      float mdergl[4] = {gld_x,gld_y,gld_z,gld_phim};

	      int ltmp[4] = {ii*1000 + jj*100 + sens*10 + 1,ii*1000 + jj*100 + sens*10 + 2, ii*1000 + jj*100 + sens*10 + 3, ii*1000 + jj*100 + sens*10 + 4};
              m->mille(4,mderlc,4,mdergl,ltmp,dsign,uncer);
              mf::LogDebug("SingleTrackAlignment") << "uncer = " << uncer ;
	    }
	  }
	}
      }
    }
    re++;
    mf::LogDebug("SingleTrackAlignment") << "^ Record " << re ;
    m->end();
  }

  //......................................................................

  void emph::SingleTrackAlignment::produce(art::Event& evt)
  {
    pullsorted.clear();
    cl_group.clear();
    ls_group.clear();
    linesegments.clear();
    clusters.clear();
    clustMap.clear();
    tracksegments.clear();
    tsvnom.clear();

    auto emgeo = geo->Geo();

    run = evt.run();
    subrun = evt.subRun();
    event = evt.event();
    fEvtNum = evt.id().event();

    // if data fcl
    std::string digitStr = std::to_string(event);
    bool useEvent = false;
    if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
      useEvent = true;
    }
                                                  
    if (fCheckLineSeg){
      auto haslineseg = evt.getHandle<std::vector<rb::LineSegment>>(fTrackSegLabel);
      if (!haslineseg){
        mf::LogError("HasLineSeg")<<"No line segments found in event but CheckLineSeg set to true!";
        abort();
      }
    }

    art::Handle< std::vector<rb::LineSegment> > lsH;
    art::Handle< std::vector<rb::SSDCluster> > clustH;
    art::Handle< std::vector<rb::TrackSegment> > tsH;
    art::Handle< std::vector<rb::Track> > trackH;

    if (useEvent){
      // Get line segments and make map
      try {
        evt.getByLabel(fLineSegLabel, lsH);
        if (!lsH->empty()){
          for (size_t idx=0; idx < lsH->size(); ++idx) {
            const rb::LineSegment& lineseg = (*lsH)[idx];
            linesegments.push_back(&lineseg);
          }
        }
        evt.getByLabel(fClusterLabel, clustH);
        if (!clustH->empty()){
          for (size_t idx=0; idx < clustH->size(); ++idx) {
            const rb::SSDCluster& clust = (*clustH)[idx];
            ++clustMap[std::pair<int,int>(clust.Station(),clust.Plane())];
            clusters.push_back(&clust);
          }
        }
        evt.getByLabel(fTrackSegLabel, tsH);
        if (!tsH->empty()){
          for (size_t idx=0; idx < tsH->size(); ++idx) {
            const rb::TrackSegment& trackseg = (*tsH)[idx];
	    rb::TrackSegment trksg(*&trackseg);
            tracksegments.push_back(&trackseg);
          }
        }
        evt.getByLabel(fTrackLabel, trackH);
        if (!trackH->empty()){
          for (size_t idx=0; idx < trackH->size(); ++idx) {
            const rb::Track& track = (*trackH)[idx];
            rb::Track trk(*&track);
            tracks.push_back(&track);
            for (size_t i=0; i<trk.NTrackSegments(); i++){
              rb::TrackSegment tseg = *trk.GetTrackSegment(i);
	      tsvnom.push_back(tseg);
            }
          }
        }

        cl_group.resize(nStations);	
        ls_group.resize(nStations);
        pullsorted.resize(nStations);

        for (size_t i=0; i<nStations; i++){
          cl_group[i].resize(nPlanes);
          ls_group[i].resize(nPlanes);
          pullsorted[i].resize(nPlanes);
        }

        for (size_t i=0; i<clusters.size(); i++){
          int plane = clusters[i]->Plane();
          int station = clusters[i]->Station();
          cl_group[station][plane].push_back(clusters[i]);
          ls_group[station][plane].push_back(linesegments[i]);
        }

        if (tsvnom.size()==3){
          usingEvent++;

          dgm->Map()->SetAlign(align0);

          // Get pulls
          Pulls(tsvnom);
        } //if
      } // try	
      catch(...) {

      }
    } //useEvent
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackAlignment)
