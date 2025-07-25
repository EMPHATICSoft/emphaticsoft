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
    std::vector<double> SSDRot();
    std::vector<TVector3> SSDPos(std::vector<rb::TrackSegment> tracksegv, bool wantN = false);
    void Pulls(std::vector<rb::TrackSegment> tracksegv);

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
    std::vector<std::vector<std::vector<TVector3> > > nomsorted;
    std::vector<std::vector<std::vector<float> > > pullsorted;

    std::vector<rb::SpacePoint> spv;
    std::vector<rb::TrackSegment> tsv;
    std::vector<rb::TrackSegment> tsvnom;

    std::vector<std::vector<double>> sp1;
    std::vector<std::vector<double>> sp2;
    std::vector<std::vector<double>> sp3;

    std::vector<std::vector<std::vector<int> > > countvec;

    //fcl parameters
    bool        fCheckLineSeg;    
    std::string fLineSegLabel;
    std::string fClusterLabel;
    std::string fTrackSegLabel;
    std::string fTrackLabel;
  
    //Millepede stuff
    Mille* m;
    std::vector<int> label;
    std::vector<float> derLc;
    std::vector<std::vector<std::vector<float> > > derlctest;

    double targetz;
    double magnetusz;
    double magnetdsz;

    Align* align = new Align();

  };

  //.......................................................................
  
  emph::SingleTrackAlignment::SingleTrackAlignment(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckLineSeg      (pset.get< bool >("CheckLineSeg")),
    fLineSegLabel      (pset.get< std::string >("LineSegLabel")),
    fClusterLabel      (pset.get< std::string >("ClusterLabel")),
    fTrackSegLabel     (pset.get< std::string >("TrackSegLabel")),
    fTrackLabel        (pset.get< std::string >("TrackLabel"))
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

    targetz = emgeo->GetTarget()->Pos()(2);
    magnetusz = emgeo->MagnetUSZPos();
    magnetdsz = emgeo->MagnetDSZPos();

    countvec.resize(nStations);
    for (size_t i=0; i<nStations; i++){
      countvec[i].resize(nPlanes);
      for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
        countvec[i][j].resize(emgeo->GetSSDStation(i)->GetPlane(j)->NSSDs()-1);
      }
    }

    for (int i=0; i<(int)nStations; i++){
      for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
        countvec[i][j].push_back(0);
      }
    }

    int l=0;
    for (int ii=0; ii<(int)nStations; ii++){
      for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
        for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
          for (int dd=1; dd<=4; dd++){
            l = ii*1000 + jj*100 + kk*10 + dd;
            label.push_back(l);
            std::cout<<"l: "<<l<<"...view: "<<emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(kk)->View()<<std::endl;
          }
        }
      }
    }

  }

  //......................................................................
   
  void emph::SingleTrackAlignment::beginJob()
  {
    std::cerr<<"Starting SingleTrackAlignment"<<std::endl;

    m = new Mille("m002.bin",true,true);
  }
 
  //......................................................................
  
  void emph::SingleTrackAlignment::endJob()
  {
    for (int i=0; i<(int)countvec.size(); i++){
      for (int j=0; j<(int)countvec[i].size(); j++){
        for (int k=0; k<(int)countvec[i][j].size(); k++){
          std::cout<<"(i,j,k) = "<<"("<<i<<","<<j<<","<<k<<") = "<<countvec[i][j][k]<<std::endl;
        }
      }
    } 

    delete m;
  }

  //......................................................................

  std::vector<double> emph::SingleTrackAlignment::SSDRot()
  {
    std::vector<double> rolls;
    auto emgeo = geo->Geo(); 

    for (int ii=0; ii<(int)nStations; ii++){
      for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
        for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
          double roll = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(kk)->Rot();
	  rolls.push_back(roll);
        }
      }
    }
    return rolls;
  }

  //......................................................................

  void emph::SingleTrackAlignment::Pulls(std::vector<rb::TrackSegment> tracksegv)
  {
    auto emgeo = geo->Geo();
    ru::RecoUtils r = ru::RecoUtils(fEvtNum);

    for (int ii=0; ii<(int)ls_group.size(); ii++){
      for (int jj=0; jj<(int)ls_group[ii].size(); jj++){
        for (int kk=0; kk<(int)ls_group[ii][jj].size(); kk++){
          TVector3 x0(ls_group[ii][jj][kk]->X0().X(),ls_group[ii][jj][kk]->X0().Y(),ls_group[ii][jj][kk]->X0().Z());
          TVector3 x1(ls_group[ii][jj][kk]->X1().X(),ls_group[ii][jj][kk]->X1().Y(),ls_group[ii][jj][kk]->X1().Z());

	  int sens = cl_group[ii][jj][kk]->Sensor();

          //std::cout<<"View: "<<emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->View()<<std::endl;
	  auto sview = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->View();
	  double phim = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->Rot(); //in rad i think
          // rad or degree?
	  //std::cout<<"phi_m: "<<phim<<std::endl;
          for (auto ts : tracksegv) {
            double tsz = ts.Vtx()[2];
	    TVector3 a(ts.A()[0],ts.A()[1],ts.A()[2]);
            TVector3 b(ts.B()[0],ts.B()[1],ts.B()[2]);	    

	  // pull = doca between s and ts
            double sensorz = x0(2); //s[2];
            if (x0(2) != x1(2)) std::cout<<"Rotated line segment --> using x0 for now"<<std::endl;

            if ((tsz < targetz && sensorz < targetz)
            || ((tsz > targetz && tsz < magnetusz) && (sensorz > targetz && sensorz < magnetusz))
            || (tsz > magnetdsz && sensorz > magnetdsz)){
              double f1[3]; double f2[3]; double f3[3];
              r.ClosestApproach(x0,x1,a,b,f1,f2,f3,"SSD",false); //TrackSegment");   
              float pull = sqrt((f3[0]-f2[0])*(f3[0]-f2[0])+(f3[1]-f2[1])*(f3[1]-f2[1])+(f3[2]-f2[2])*(f3[2]-f2[2]));

	      // @ sensorz what is ts xy 
	      // find signed distance from sensor xy to ts xy 
	      // find t where sensor z is
	      double t = ( sensorz - a(2) )/( b(2) - a(2) );
	      double tsx = a(0) + (b(0)-a(0))*t;
	      double tsy = a(1) + (b(1)-a(1))*t;
	      // signed distance from point to a line 
              // find slope of line segment
              /*
              double dx = b(0) - a(0);
	      double dy = b(1) - a(1);
	      if (dx == 0){
	        la = 1;
		lb = 0;
	        lc = a(0);
	      }
	      else{
                double m = dy/dx;
		lc = a(1) - m*a(0);
		la = m;
		lb = 1;
	      }
	      */
	      double la = x1(1) - x0(1);
	      double lb = x0(0) - x1(0);
	      double lc = x0(1)*(x1(0)-x0(0)) - (x1(1)-x0(1))*x0(0);	  
	      float dsign = (la*tsx + lb*tsy + lc)/(sqrt(la*la + lb*lb));

	      pullsorted[ii][jj].push_back(dsign);

	      float lcd_x0 = -1.*TMath::Sin(phim);
	      float lcd_pxpz = -sensorz*TMath::Sin(phim);
	      float lcd_y0 = 1.*TMath::Cos(phim);
	      float lcd_pypz = sensorz*TMath::Cos(phim);

	      std::cout<<"..........."<<std::endl;
	      std::cout<<"pull = "<<pull<<std::endl;
	      std::cout<<"dsign = "<<dsign<<std::endl;
	      std::cout<<"sensorz = "<<sensorz<<std::endl;

              float gld_x; float gld_y; float gld_z;
	      if (sview == 1){ // X-VIEW
	        gld_x = -1.*TMath::Sin(phim);
		gld_y = 0.;
		gld_z = -1.*ts.P()[0]/ts.P()[2]*TMath::Sin(phim);
	      }
	      else if (sview == 2){ // Y-VIEW
                gld_x = 0;
                gld_y = 1.*TMath::Cos(phim);
                gld_z = ts.P()[1]/ts.P()[2]*TMath::Cos(phim);
              }
	      else if (sview == 4){ // U-VIEW
                gld_x = -1.*TMath::Sin(phim);
                gld_y = 1.*TMath::Cos(phim);
                gld_z = -1.*ts.P()[0]/ts.P()[2]*TMath::Sin(phim) + ts.P()[1]/ts.P()[2]*TMath::Cos(phim);
              }
              else{
		gld_x = 0.;
		gld_y = 0.;
		gld_z = 0.;
              }
       	
	      //std::cout<<"px = "<<ts.P()[0]<<std::endl;
	      //std::cout<<"py = "<<ts.P()[1]<<std::endl;
              //std::cout<<"pz = "<<ts.P()[2]<<std::endl;
	      //std::cout<<"px/pz = "<<gld_xz<<std::endl;
	      //std::cout<<"py/pz = "<<gld_yz<<std::endl;

	      float mderlc[4] = {lcd_x0,lcd_pxpz,lcd_y0,lcd_pypz};
	      float mdergl[3] = {gld_x,gld_y,gld_z};

	      int ltmp[4] = {ii*1000 + jj*100 + kk*10 + 1,ii*1000 + jj*100 + kk*10 + 2, ii*1000 + jj*100 + kk*10 + 3}; //, ii*1000 + jj*100 + kk*10 + 4};
              m->mille(4,mderlc,3,mdergl,ltmp,dsign,0.0173);
	    }
	  }
	}
      }
    }
    m->end();
  }

  //......................................................................

  std::vector<TVector3> emph::SingleTrackAlignment::SSDPos(std::vector<rb::TrackSegment> tracksegv, bool wantN)
  {
    std::vector<TVector3> ssdv;
    auto emgeo = geo->Geo();

    for (int ii=0; ii<(int)nStations; ii++){
      for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
        for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
          for (auto ts : tracksegv) {
            double tsz = ts.Vtx()[2];
            auto sd = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(kk);
            auto st = emgeo->GetSSDStation(ii);
            auto T = align->SSDMatrix(ii,jj,kk);
            double sl[3] = {sd->Pos()[0],sd->Pos()[1],sd->Pos()[2]};
            double sm[3];
            double s[3];
            sd->LocalToMother(sl,sm);
            st->LocalToMother(sm,sl);
            T->LocalToMaster(sl,s);

            double sensorz = s[2];
            if ((tsz < targetz && sensorz < targetz)
            || ((tsz > targetz && tsz < magnetusz) && (sensorz > targetz && sensorz < magnetusz))
            || (tsz > magnetdsz && sensorz > magnetdsz)){
	      double n[3];
	      double pmag = sqrt(ts.P()[0]*ts.P()[0]+ts.P()[1]*ts.P()[1]+ts.P()[2]*ts.P()[2]);

	      for (size_t i=0; i<3; i++){
                n[i] = ts.P()[i]/pmag;
                if (wantN) {derLc.push_back(n[i]); derlctest[ii][jj].push_back(n[i]); }
              }

	      double t = (sensorz- ts.A()[2])/n[2];
              double sensorx = ts.Vtx()[0] + t*n[0];
              double sensory = ts.Vtx()[1] + t*n[1]; 
              TVector3 x(sensorx,sensory,sensorz);
              ssdv.push_back(x);
	      if (wantN) nomsorted[ii][jj].push_back(x);
            }
          }
        }
      }
    }
    return ssdv;
  }

  //......................................................................

  void emph::SingleTrackAlignment::produce(art::Event& evt)
  {
    std::vector<float> derGl(112, 0.0);
    std::vector<std::vector<std::vector<float> > > dergltest;

    derLc.clear();
    nomsorted.clear();
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
          //tsvnom.push_back(trksg);
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
      nomsorted.resize(nStations);
      pullsorted.resize(nStations);
      derlctest.resize(nStations);
      dergltest.resize(nStations);
      //derGl.resize(28);

      for (size_t i=0; i<nStations; i++){
        cl_group[i].resize(nPlanes);
        ls_group[i].resize(nPlanes);
	nomsorted[i].resize(nPlanes);
        pullsorted[i].resize(nPlanes);
	derlctest[i].resize(nPlanes);
        dergltest[i].resize(nPlanes);
      }

      for (size_t i=0; i<clusters.size(); i++){
        int plane = clusters[i]->Plane();
        int station = clusters[i]->Station();
        cl_group[station][plane].push_back(clusters[i]);
        ls_group[station][plane].push_back(linesegments[i]);
      }

      if (tsvnom.size()==3){
        double epsilon = 0.06; //60 micron
        TGeoTranslation* h = new TGeoTranslation();
        TGeoTranslation* u = new TGeoTranslation(0,0,0);
	TGeoRotation* hr = new TGeoRotation("hr",0.1,0,0);
        TGeoRotation* ur = new TGeoRotation("ur",0,0,0);
        //TGeoRotation* ur2 = new TGeoRotation("ur2",0.1,0,0);

        std::vector<double> nomr;
        for (int i=0; i<(int)nStations; i++){
          for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
            for (int k=0; k<emgeo->GetSSDStation(i)->GetPlane(j)->NSSDs(); k++){
              align->SetSSDTranslation(i,j,k,u);
	      align->SetSSDRotation(i,j,k,ur);

              TGeoCombiTrans* ssdm = align->SSDMatrix(i,j,k);
              auto frm = ssdm->GetRotationMatrix();
	      //std::cout<<"rotation matrix: "<<frm[0]<<"  "<<frm[1]<<"  "<<frm[2]<<std::endl;
	      //std::cout<<"                 "<<frm[3]<<"  "<<frm[4]<<"  "<<frm[5]<<std::endl;
	      //std::cout<<"                 "<<frm[6]<<"  "<<frm[7]<<"  "<<frm[8]<<std::endl;
              //std::cout<<"translation: "<<translation[0]<<","<<translation[1]<<","<<translation[2]<<std::endl;
	      Double_t phi = TMath::ACos(frm[0])*180./TMath::Pi();
              nomr.push_back(phi);
            }
          }
        }
        dgm->Map()->SetAlign(align);

        // Get nominal positions at each station
        auto nom = SSDPos(tsvnom,true);

        // Get pulls
        Pulls(tsvnom);
      } //if
    } // try	
    catch(...) {

    }
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackAlignment)
