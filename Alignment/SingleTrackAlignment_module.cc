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

#include "/exp/emph/app/users/rchirco/emph_12.4.23/mp/target/Mille.h"
#include "/exp/emph/app/users/rchirco/emph_12.4.23/mp/target/Mille.cc"

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

    //TTree*      aligntree;
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
    fTrackSegLabel     (pset.get< std::string >("TrackSegLabel"))
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
/*
    art::ServiceHandle<art::TFileService> tfs;
    aligntree = tfs->make<TTree>("aligntree","");
    aligntree->Branch("run",&run,"run/I");
    aligntree->Branch("subrun",&subrun,"subrun/I");
    aligntree->Branch("event",&event,"event/I");   
*/
    m = new Mille("m002.bin",true,true);

    //for (int i=1; i<113; i++) label.push_back(i);

    //int l=0;
/*
    for (int i=1; i<29; i++){
      for (int d=0; d<4; d++){
	if (d==0) l = i * 10 + 1;
        if (d==1) l = i * 10 + 2;
        if (d==2) l = i * 10 + 3;
        if (d==3) l = i * 10 + 4;
	label.push_back(l);
      }
    }
*/
    //auto emgeo = geo->Geo();
    //nStations = emgeo->NSSDStations();
    //nPlanes = emgeo->NSSDPlanes();
/*
    for (int ii=0; ii<(int)nStations; ii++){
      for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
        for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
          for (int dd=1; dd<=4; dd++){ 
	    l = ii*1000 + jj*100 + kk*10 + dd;
            label.push_back(l);
std::cout<<"l: "<<l<<std::endl;
std::cout<<"view: "<<emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(kk)->View()<<std::endl;
          }
        }
      }
    }
*/
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
          //std::cout<<"Roll angle: "<<roll<<std::endl;
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

          std::cout<<"View: "<<emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->View()<<std::endl;
	  auto sview = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->View();
	  double phim = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(sens)->Rot(); //in rad i think
          // rad or degree?
	  std::cout<<"phi_m: "<<phim<<std::endl;
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
	      //std::cout<<"tsx,tsy = "<<tsx<<","<<tsy<<std::endl;
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

              //std::cout<<"Pull: "<<pull<<std::endl;
	      //pullsorted[ii][jj].push_back(pull);
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
                gld_x = -1.*TMath::Sin(phim);;
                gld_y = 1.*TMath::Cos(phim);
                gld_z = -1.*ts.P()[0]/ts.P()[2]*TMath::Sin(phim) + ts.P()[1]/ts.P()[2]*TMath::Cos(phim);
              }
              else{
		gld_x = 0.;
		gld_y = 0.;
		gld_z = 0.;
              }
       	
	      //float gld_x = 1.*TMath::Cos(phim);
	      //float gld_xz = ts.P()[0]/ts.P()[2]*TMath::Cos(phim);
	      //float gld_y = 1.*TMath::Sin(phim);
	      //float gld_yz = ts.P()[1]/ts.P()[2]*TMath::Sin(phim);

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
	        //std::cout<<"n: "<<n[i]<<std::endl;
                if (wantN) {derLc.push_back(n[i]); derlctest[ii][jj].push_back(n[i]); }
              }
              // dx/dz, dy/dz, 1 ?

/*
	      for (size_t i=0; i<3; i++){
                n[i] = ts.B()[i] - ts.A()[i];
	      }
	      double mag = sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
              for (size_t i=0; i<3; i++){
		float normn = n[i]/mag;		
	        if (wantN) derLc.push_back(normn);
              std::cout<<"normn: "<<normn<<std::endl;
	      }
*/
              //std::cout<<"n[2]: "<<n[2]<<std::endl;
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
      auto haslineseg = evt.getHandle<rb::LineSegment>("makesingletracks");
      if (!haslineseg){
        mf::LogError("HasLineSeg")<<"No line segments found in event but CheckLineSeg set to true!";
        abort();
      }
    }

    art::Handle< std::vector<rb::LineSegment> > lsH;
    art::Handle< std::vector<rb::SSDCluster> > clustH;
    art::Handle< std::vector<rb::TrackSegment> > tsH;

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
          tsvnom.push_back(trksg);
        }
      }

      bool goodEvent = false;
      if (clusters.size()==nPlanes){
        for (auto i : clustMap){
          if (i.second != 1){goodEvent = false; break;}
          else goodEvent = true;
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

      if (goodEvent && lsH->size()==20 && clustH->size()==20 && tsH->size()==3){      

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
              //align->AddSSDRotation(i,j,k,ur);

	      Double_t phi;
              //Double_t phi1,theta1;
              //Double_t phi2,theta2;
              //Double_t phi3,theta3;
              TGeoCombiTrans* ssdm = align->SSDMatrix(i,j,k);
              //const double* translation = ssdm->GetTranslation();
//i think the rotation is broken? can't use this object it only segfaults
              //TGeoRotation* rotation; 
              //rotation = ssdm->GetRotation();
              auto frm = ssdm->GetRotationMatrix();
	      //auto frm2 = rotation->GetRotationMatrix();
     	      //rotation->SetMatrix(frm);
              //rotation->GetAngles(phi,theta,psi);
	      //rotation->GetAngles(theta1,phi1,theta2,phi2,theta3,phi3);
	      ////std::cout<<"rotation matrix: "<<frm[0]<<"  "<<frm[1]<<"  "<<frm[2]<<std::endl;
	      ////std::cout<<"                 "<<frm[3]<<"  "<<frm[4]<<"  "<<frm[5]<<std::endl;
	      ////std::cout<<"                 "<<frm[6]<<"  "<<frm[7]<<"  "<<frm[8]<<std::endl;
              ////std::cout<<"translation: "<<translation[0]<<","<<translation[1]<<","<<translation[2]<<std::endl;
	      phi = TMath::ACos(frm[0])*180./TMath::Pi();
              nomr.push_back(phi);
	      ////std::cout<<"rotation: "<<phi<<std::endl;
            }
          }
        }
        dgm->Map()->SetAlign(align);

        // Get nominal positions at each station
        auto nom = SSDPos(tsvnom,true);

        // Get pulls
        Pulls(tsvnom);
	//m->end();
        // Get nominal rotations at each station
        //auto nomr = SSDRot();
/*
        int nsens = 0;
        for (int i=0; i<(int)nStations; i++){
        //for (int i=0; i<(int)ls_group.size(); i++){
          for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
          //for (int j=0; j<(int)ls_group[i].size(); j++){
            for (int k=0; k<(int)ls_group[i][j].size(); k++){
              int sens = k;
	      if (cl_group[i][j][k]->Sensor()!=k) {sens = cl_group[i][j][k]->Sensor();}
              // has to stay k in [i][j][k] because it's appended
	      // when we want to know actual sensor number we use sens
	     
	      countvec[i][j][sens]++;
	      nsens++;

              for (int dim=1; dim<=4; dim++){ //start at -1 for rotations
                spv.clear();
                tsv.clear();

		//int l = 0;
		//if (dim==-1) l = nsens * 10 + 1;
                //if (dim==0) l = nsens * 10 + 2;
                //if (dim==1) l = nsens * 10 + 3;
                //if (dim==2) l = nsens * 10 + 4;
		//label.push_back(l);
	
                if (dim==1) h->SetTranslation(epsilon,0,0);
                if (dim==2) h->SetTranslation(0,epsilon,0);
                if (dim==3) h->SetTranslation(0,0,epsilon);
                if (dim==4) align->SetSSDRotation(i,j,sens,hr);
	        if (dim!=4) align->SetSSDTranslation(i,j,sens,h); 

	        //TGeoCombiTrans* ssdm = align->SSDMatrix(i,j,sens);
	  	//const double* translation = ssdm->GetTranslation();
		//TGeoRotation* rotation = ssdm->GetRotation();
		//double phi=0.;
		//double theta=0.;
		//double psi=0.;
//		double phi; //,theta,psi;
	////std::cout<<"Now for shifts..."<<std::endl;	
                //rotation->GetAngles(phi,theta,psi);
//phi = rotation->GetPhiRotation();
//std::cout<<"rotation phi: "<<phi<<std::endl;
//auto frm = ssdm->GetRotationMatrix();
              ////std::cout<<"rotation matrix: "<<frm[0]<<"  "<<frm[1]<<"  "<<frm[2]<<std::endl;
              ////std::cout<<"                 "<<frm[3]<<"  "<<frm[4]<<"  "<<frm[5]<<std::endl;
              ////std::cout<<"                 "<<frm[6]<<"  "<<frm[7]<<"  "<<frm[8]<<std::endl;
                  ////std::cout<<"translation: "<<translation[0]<<","<<translation[1]<<","<<translation[2]<<std::endl;
//phi = TMath::ACos(frm[0])*180./TMath::Pi();
              ////std::cout<<"rotation: "<<phi<<std::endl;
		  //std::cout<<"rotation: "<<phi<<","<<theta<<","<<psi<<std::endl;
		//std::cout<<"rotation matrix: "<<frm[0][0]<<","<<frm[1][1]<<","<<frm[2][2]<<std::endl;

                // Make SingleTrackAlgo instance
                int ns = static_cast<int>(nStations); 
                int np = static_cast<int>(nPlanes);
                emph::SingleTrackAlgo algo = emph::SingleTrackAlgo(fEvtNum,nStations,nPlanes);

                const rb::LineSegment* ls_true = ls_group[i][j][k]; ///////

	        rb::LineSegment* ls_tmp = new rb::LineSegment();

	        // Make shifted LineSegments with DetGeoMap
		auto cl = cl_group[i][j][k]; //////////

	        if (dgm->Map()->SSDClusterToLineSegment(*cl,*ls_tmp)) ls_group[i][j][k] = ls_tmp; ///////	
                spv = algo.MakeHits(ls_group);

		sp1.clear();
                sp2.clear();
                sp3.clear();
	        // Group space points
	        for (size_t i=0; i<spv.size(); i++){
                  std::vector<double> x = {spv[i].Pos()[0],spv[i].Pos()[1],spv[i].Pos()[2]};

                  if (emgeo->GetTarget()){
                    if (spv[i].Pos()[2] < emgeo->GetTarget()->Pos()(2)) sp1.push_back(x);
                    if (spv[i].Pos()[2] > emgeo->GetTarget()->Pos()(2) && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) sp2.push_back(x);
                    if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) sp3.push_back(x);
                  }
                }

                tsv = algo.MakeLines(sp1,sp2,sp3);

		// Get reconstructed positions
		auto shift = SSDPos(tsv);

		// Get reconstructed rotations
		//auto shiftr = SSDRot();
                std::vector<double> shiftr;

                for (int ii=0; ii<(int)nStations; ii++){
                  for (int jj=0; jj<emgeo->GetSSDStation(ii)->NPlanes(); jj++){
                    for (int kk=0; kk<emgeo->GetSSDStation(ii)->GetPlane(jj)->NSSDs(); kk++){
                      TGeoCombiTrans* ssdm = align->SSDMatrix(ii,jj,kk);
                      auto frmr = ssdm->GetRotationMatrix();
                      double roll = TMath::ACos(frmr[0])*180./TMath::Pi();
                //      double roll = emgeo->GetSSDStation(ii)->GetPlane(jj)->SSD(kk)->Rot();
                      ////std::cout<<"Roll angle: "<<roll<<std::endl;
//std::cout<<"phi: "<<phi<<std::endl;
//		      double roll = phi*TMath::Pi()/180.;
                      shiftr.push_back(roll);
                    }
                  }
                }

                // Calculate numerical derivative
                // df/dh = ( f(x+h) - f(x) ) / h
	        //derGl.clear();
                for (size_t aa=0; aa<nom.size(); aa++){
                  for (size_t bb=0; bb<shift.size(); bb++){
                    if (aa==bb){
                      double dfdh_x = (shift[bb](0) - nom[aa](0))/(epsilon);
                      double dfdh_y = (shift[bb](1) - nom[aa](1))/(epsilon);
                      double dfdh_z = (shift[bb](2) - nom[aa](2))/(epsilon);

                      // Store in vector
		      std::vector<double> der = {dfdh_x,dfdh_y,dfdh_z};

                      // Add to global derivatives vector
                      //derGl.push_back((float)dfdh_x);
                      //derGl.push_back((float)dfdh_y);
                      //derGl.push_back((float)dfdh_z);
		      derGl[aa*4] += dfdh_x;
                      derGl[aa*4+1] += dfdh_y;
                      derGl[aa*4+2] += dfdh_z;
		      //std::cout<<"dfdh_(xyz): "<<dfdh_x<<","<<dfdh_y<<","<<dfdh_z<<std::endl;

		      // Rotations
		      double dfdh_r = (shiftr[bb] - nomr[aa])/(0.1); ///TMath::Pi()/180.);
                      //derGl.push_back((float)dfdh_r);
                      derGl[aa*4+3] += dfdh_r;
		      //std::cout<<"dfdh_r: "<<dfdh_r<<std::endl;
	              //std::cout<<"nomr: "<<nomr[i]<<", shiftr: "<<shiftr[j]<<std::endl;

                      // Add to global derivatives vector
                      //derGl.push_back((float)dfdh_x);
                      //derGl.push_back((float)dfdh_y);
                      //derGl.push_back((float)dfdh_z);
		    }
		  }
		}

                //for (int d=0; d<3; d++){
		  // Unsure if this is right --> do I loop through dimensions again?
		  // rMeas should be nominal x,y,z, or angle position
		  //float rMeas = nomsorted[i][j][k](d);
//		  float rMeas = pullsorted[i][j][k]; //sens]; //k]; *****
		  //std::cout<<"rMeas: "<<rMeas<<std::endl;
	          //std::cout<<"rMeas: "<<nomsorted[i][j][k](d)<<std::endl;
	          //std::cout<<"derLc size: "<<derLc.size()<<std::endl;
                  //std::cout<<"derGl size: "<<derGl.size()<<std::endl;
//                  m->mille(84,derLc.data(),112,derGl.data(),label.data(),rMeas,0.0173);
		  //m->mille(84,derLc.data(),84,derGl.data(),label.data(),rMeas,17.3);		

		  // Write to binary file
                  //m->end();
	        //} // d
 
		// Undo line segment defintion
	  	ls_group[i][j][k] = ls_true; //sens] = ls_true; //k] = ls_true; *****

		// Undo alignment shift
		align->SetSSDTranslation(i,j,sens,u);
	        align->SetSSDRotation(i,j,sens,ur);
		//if (dim==-1) align->AddSSDRotation(i,j,sens,ur2);
	      } // dim
	      //m->end();
            } // sensor
          } // plane
        } //station
*/
/*
        for (int i=0; i<(int)derGl.size(); i++){
	  derGl[i] /= derGl.size();
        }

	std::vector<float> dlc12;
	std::vector<float> dgl12;	
	std::vector<int> l12;
	for (int s=0; s<54; s++){
	  dlc12.push_back(derLc[s]);
	}
        for (int s=0; s<72; s++){
          dgl12.push_back(derGl[s]);
          l12.push_back(label[s]);
        }
*/
/*
        std::vector<float> derlctmp;
        std::vector<float> dergltmp;
        std::vector<int> ltmp;
        int n=0;
        for (int i=0; i<(int)nStations; i++){	
          for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
            for (int k=0; k<(int)ls_group[i][j].size(); k++){              
              int sens = k;
              if (cl_group[i][j][k]->Sensor()!=k) {sens = cl_group[i][j][k]->Sensor();}
	      derlctmp.push_back(derLc[n*3]);
	      derlctmp.push_back(derLc[n*3+1]);
              derlctmp.push_back(derLc[n*3+2]);
              dergltmp.push_back(derGl[n*4]);
              dergltmp.push_back(derGl[n*4+1]);
              dergltmp.push_back(derGl[n*4+2]);
	      dergltmp.push_back(derGl[n*4+3]);
              ltmp.push_back(label[n*4]);
              ltmp.push_back(label[n*4+1]);
              ltmp.push_back(label[n*4+2]);
              ltmp.push_back(label[n*4+3]);
	      n++;
            }
          }
        }
	std::cout<<"N="<<n<<std::endl;
        for (int i=0; i<(int)nStations; i++){
          for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
            for (int k=0; k<(int)ls_group[i][j].size(); k++){
//        for (int i=0; i<6; i++){
//          for (int j=0; j<emgeo->GetSSDStation(i)->NPlanes(); j++){
//            for (int k=0; k<(int)ls_group[i][j].size(); k++){
	      float rMeas = pullsorted[i][j][k];
//              m->mille(54,dlc12.data(),72,dgl12.data(),l12.data(),rMeas,0.0173);
              //m->mille(84,derLc.data(),112,derGl.data(),label.data(),rMeas,0.0173);
	      m->mille(3,derlctmp.data(),4,dergltmp.data(),ltmp.data(),rMeas,0.0173);
	    }
	  }
	}
        m->end();
	derlctmp.clear();
	dergltmp.clear();
	ltmp.clear();
*/
      } //if
    } // try	
    catch(...) {

    }
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackAlignment)
