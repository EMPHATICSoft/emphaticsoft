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

    void    MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls);
    void    MakeHits();
    void    MakeLines();

  private:
  
    TTree*      spacepoint;
    int         run,subrun,event;
    int         fEvtNum;

    std::vector<const rb::SSDCluster*> clusters;
    std::vector<rb::LineSegment> stripv;
    std::vector<rb::SpacePoint> spv;
    std::vector<rb::TrackSegment> tsv;
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group;

    std::map<std::pair<int, int>, int> clustMap;

    bool        fMakePlots;
    int 	goodclust = 0;
    int         badclust = 0; 
    size_t      nPlanes;
    size_t      nStations;
    int         st;

    //fcl parameters
    bool        fCheckClusters;     //Check clusters for event 

    //histograms and graphs for hits
    TH2F* hSPDist0;
    TH2F* hSPDist1;
    TH2F* hSPDist2;
    TH2F* hSPDist3;
    TH2F* hSPDist4;
    TH2F* hSPDist5;
    TH2F* hSPDist6;
    TH2F* hSPDist7;

    TH2F* hDist_xz;
    TH2F* hDist_yz;

    //reco info for lines
    std::vector<std::vector<double>> sp1;
    std::vector<std::vector<double>> sp2;
    std::vector<std::vector<double>> sp3;
    std::vector<rb::SpacePoint> ts1;
    std::vector<rb::SpacePoint> ts2;
    std::vector<rb::SpacePoint> ts3;

  };

  //.......................................................................
  
  emph::MakeSingleTracks::MakeSingleTracks(fhicl::ParameterSet const& pset)
    : EDProducer{pset},
    fCheckClusters     (pset.get< bool >("CheckClusters")) 
    {
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
    art::ServiceHandle<art::TFileService> tfs;
    spacepoint = tfs->make<TTree>("spacepoint","");
    spacepoint->Branch("run",&run,"run/I");
    spacepoint->Branch("subrun",&subrun,"subrun/I");
    spacepoint->Branch("event",&event,"event/I");
   
    hSPDist0 = tfs->make<TH2F>("hSPDist0","hSPDist0",1000,-50.,50.,1000,-50.,50.); 
    hSPDist1 = tfs->make<TH2F>("hSPDist1","hSPDist1",1000,-50.,50.,1000,-50.,50.);
    hSPDist2 = tfs->make<TH2F>("hSPDist2","hSPDist2",1000,-50.,50.,1000,-50.,50.);
    hSPDist3 = tfs->make<TH2F>("hSPDist3","hSPDist3",1000,-50.,50.,1000,-50.,50.);
    hSPDist4 = tfs->make<TH2F>("hSPDist4","hSPDist4",1000,-50.,50.,1000,-50.,50.);
    hSPDist5 = tfs->make<TH2F>("hSPDist5","hSPDist5",1000,-50.,50.,1000,-50.,50.);
    hSPDist6 = tfs->make<TH2F>("hSPDist6","hSPDist6",1000,-50.,50.,1000,-50.,50.);
    hSPDist7 = tfs->make<TH2F>("hSPDist7","hSPDist7",1000,-50.,50.,1000,-50.,50.);

    hDist_xz = tfs->make<TH2F>("hDist_xz","hDist_xz",2050,-50.,2000.,1000,-50.,50.); 
    hDist_yz = tfs->make<TH2F>("hDist_yz","hDist_yz",2050,-50.,2000.,1000,-50.,50.);

  }
 
  //......................................................................
  
  void emph::MakeSingleTracks::endJob()
  {
       std::cout<<"Number of clusters with one cluster per sensor: "<<goodclust<<std::endl;
       std::cout<<"Number of available clusters: "<<badclust+goodclust<<std::endl;
/*
       // Don't seg fault if there are no tracks in the subrun(s).
       if (hScattering) {
	
	 int bmin_s = hScattering->FindFirstBinAbove();
	 int bmax_s = hScattering->FindLastBinAbove();
	 hScattering->GetXaxis()->SetRange(bmin_s,bmax_s);

	 int bmin_sr = hScatteringRes->FindFirstBinAbove();
	 int bmax_sr = hScatteringRes->FindLastBinAbove();
	 hScatteringRes->GetXaxis()->SetRange(bmin_sr,bmax_sr);

	 int bmin_b = hBending->FindFirstBinAbove();
	 int bmax_b = hBending->FindLastBinAbove();
	 hBending->GetXaxis()->SetRange(bmin_b,bmax_b);

	 int bmin_br = hBendingRes->FindFirstBinAbove();
	 int bmax_br = hBendingRes->FindLastBinAbove();
	 hBendingRes->GetXaxis()->SetRange(bmin_br,bmax_br);

       }
*/
  }

  //......................................................................

  void emph::MakeSingleTracks::MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls)
  {
    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;

    dgm->Map()->SSDClusterToLineSegment(cl, ls);
  }
  
  //......................................................................

  void emph::MakeSingleTracks::MakeHits()
  {
    rb::SpacePoint sp;

     art::ServiceHandle<emph::geo::GeometryService> geo;
     auto emgeo = geo->Geo();

     ru::RecoUtils recoFcn = ru::RecoUtils(fEvtNum);

     for (size_t i=0; i<nStations; i++){
         int nssds = emgeo->GetSSDStation(i)->NPlanes();
         for (size_t j=0; j<nPlanes; j++){
             if (nssds == 2){ //station 0,1,4,7
                for (size_t k=0; k<ls_group[i][j].size(); k++){
                    st = cl_group[i][j][k]->Station();
                    for (size_t l=0; l<ls_group[i][j+1].size(); l++){
			TVector3 fA( ls_group[i][j][k]->X0()[0], ls_group[i][j][k]->X0()[1], ls_group[i][j][k]->X0()[2] );
			TVector3 fB( ls_group[i][j][k]->X1()[0], ls_group[i][j][k]->X1()[1], ls_group[i][j][k]->X1()[2] );
		        TVector3 fC( ls_group[i][j+1][l]->X0()[0], ls_group[i][j+1][l]->X0()[1], ls_group[i][j+1][l]->X0()[2] );
			TVector3 fD( ls_group[i][j+1][l]->X1()[0], ls_group[i][j+1][l]->X1()[1], ls_group[i][j+1][l]->X1()[2] );

                        double x[3];
                        double l1[3]; double l2[3];
                        recoFcn.ClosestApproach(fA,fB,fC,fD,x,l1,l2);

                        //set SpacePoint object
                        sp.SetX(x);

                        //check stations
                        if (cl_group[i][j+1][l]->Station() == st){}
                        else std::cout<<"XY: Stations do not match..."<<std::endl;

                        sp.SetStation(st);
                        spv.push_back(sp);

                        if (st==0) hSPDist0->Fill(x[0],x[1]);
                        if (st==1) hSPDist1->Fill(x[0],x[1]);
                        if (st==4) hSPDist4->Fill(x[0],x[1]);
                        if (st==7) hSPDist7->Fill(x[0],x[1]);
                        hDist_xz->Fill(x[2],x[0]);
                        hDist_yz->Fill(x[2],x[1]);
                    }
                }
             }
             if (nssds == 3){ //station 2,3,5,6
                for (size_t k=0; k<ls_group[i][j].size(); k++){
                    st = cl_group[i][j][k]->Station();
                    for (size_t l=0; l<ls_group[i][j+1].size(); l++){
                        for (size_t m=0; m<ls_group[i][j+2].size(); m++){
			    TVector3 fA01( ls_group[i][j][k]->X0()[0], ls_group[i][j][k]->X0()[1], ls_group[i][j][k]->X0()[2] );
			    TVector3 fB01( ls_group[i][j][k]->X1()[0], ls_group[i][j][k]->X1()[1], ls_group[i][j][k]->X1()[2] );
			    TVector3 fC01( ls_group[i][j+1][l]->X0()[0], ls_group[i][j+1][l]->X0()[1], ls_group[i][j+1][l]->X0()[2] );
			    TVector3 fD01( ls_group[i][j+1][l]->X1()[0], ls_group[i][j+1][l]->X1()[1], ls_group[i][j+1][l]->X1()[2] );

                            double x01[3];
                            double l1_01[3]; double l2_01[3];
                            recoFcn.ClosestApproach(fA01,fB01,fC01,fD01,x01,l1_01,l2_01);

			    TVector3 fA02( ls_group[i][j][k]->X0()[0], ls_group[i][j][k]->X0()[1], ls_group[i][j][k]->X0()[2] );
			    TVector3 fB02( ls_group[i][j][k]->X1()[0], ls_group[i][j][k]->X1()[1], ls_group[i][j][k]->X1()[2] );
			    TVector3 fC02( ls_group[i][j+2][m]->X0()[0], ls_group[i][j+2][m]->X0()[1], ls_group[i][j+2][m]->X0()[2] );
			    TVector3 fD02( ls_group[i][j+2][m]->X1()[0], ls_group[i][j+2][m]->X1()[1], ls_group[i][j+2][m]->X1()[2] );

                            double x02[3];
                            double l1_02[3]; double l2_02[3];
                            recoFcn.ClosestApproach(fA02,fB02,fC02,fD02,x02,l1_02,l2_02);

			    TVector3 fA12( ls_group[i][j+1][l]->X0()[0], ls_group[i][j+1][l]->X0()[1], ls_group[i][j+1][l]->X0()[2] );
			    TVector3 fB12( ls_group[i][j+1][l]->X1()[0], ls_group[i][j+1][l]->X1()[1], ls_group[i][j+1][l]->X1()[2] );
			    TVector3 fC12( ls_group[i][j+2][m]->X0()[0], ls_group[i][j+2][m]->X0()[1], ls_group[i][j+2][m]->X0()[2] );
			    TVector3 fD12( ls_group[i][j+2][m]->X1()[0], ls_group[i][j+2][m]->X1()[1], ls_group[i][j+2][m]->X1()[2] );

                            double x12[3];
                            double l1_12[3]; double l2_12[3];
                            recoFcn.ClosestApproach(fA12,fB12,fC12,fD12,x12,l1_12,l2_12);
        
                            //average of three points (center of mass)
                            double x[3];
                            for (int i=0; i<3; i++){
                                x[i] = (x01[i]+x02[i]+x12[i])/3.;
                            }

                            //set SpacePoint object
                            sp.SetX(x);

			    //check stations
                            if (cl_group[i][j+1][l]->Station() == st && cl_group[i][j+2][m]->Station() == st){}
                            else std::cout<<"XYU: Stations do not match..."<<std::endl;

                            sp.SetStation(st);
                            spv.push_back(sp);

			    if (st==2) hSPDist2->Fill(x[0],x[1]);
                            if (st==3) hSPDist3->Fill(x[0],x[1]);
                            if (st==5) hSPDist5->Fill(x[0],x[1]);
                            if (st==6) hSPDist6->Fill(x[0],x[1]);
                            hDist_xz->Fill(x[2],x[0]);
                            hDist_yz->Fill(x[2],x[1]);
                        }
                    }
                }
             }
         }
     }
  }
 
  //......................................................................

  void emph::MakeSingleTracks::MakeLines() {
         ru::RecoUtils recoFcn2 = ru::RecoUtils(fEvtNum);

         //segment 1 -> don't need to fit anything, just connect two points
         double lfirst1[3]; double llast1[3];
         lfirst1[0] = sp1[0][0];
         lfirst1[1] = sp1[0][1];
         lfirst1[2] = sp1[0][2];

         llast1[0] = sp1[1][0];
         llast1[1] = sp1[1][1];
         llast1[2] = sp1[1][2];
	 
         //segment 2
         double lfirst2[3]; double llast2[3];
         recoFcn2.findLine(sp2,lfirst2,llast2);

         //segment 3
         double lfirst3[3]; double llast3[3];
         recoFcn2.findLine(sp3,lfirst3,llast3);
	 
         //assign to track vector
         rb::LineSegment track1 = rb::LineSegment(lfirst1,llast1);
         rb::LineSegment track2 = rb::LineSegment(lfirst2,llast2);
         rb::LineSegment track3 = rb::LineSegment(lfirst3,llast3);
 
	 // create rb::TrackSegments and insert them into the vector
	 rb::TrackSegment ts1 = rb::TrackSegment();
	 for (auto p : spv)
	   if (p.Station() == 0 || p.Station() == 1)
	     ts1.Add(p);
	 ts1.SetVtx(lfirst1);
	 double p[3];
	 double dx = llast1[0]-lfirst1[0];
	 double dy = llast1[1]-lfirst1[1];
	 double dz = llast1[2]-lfirst1[2];	 

	 p[0] = dx/dz;
	 p[1] = dy/dz;
	 p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
	 ts1.SetP(p);
	 tsv.push_back(ts1);

	 rb::TrackSegment ts2 = rb::TrackSegment();
	 for (auto p : spv)
	   if (p.Station() == 2 || p.Station() == 3 || p.Station() == 4)
	     ts2.Add(p);
	 ts2.SetVtx(lfirst2);
	 dx = llast2[0]-lfirst2[0];
	 dy = llast2[1]-lfirst2[1];
	 dz = llast2[2]-lfirst2[2];	 

	 p[0] = dx/dz;
	 p[1] = dy/dz;
	 p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
	 ts2.SetP(p);
	 tsv.push_back(ts2);

	 rb::TrackSegment ts3 = rb::TrackSegment();
	 for (auto p : spv)
	   if (p.Station() == 5 || p.Station() == 6 || p.Station() == 7)
	     ts3.Add(p);
	 ts3.SetVtx(lfirst3);
	 dx = llast3[0]-lfirst3[0];
	 dy = llast3[1]-lfirst3[1];
	 dz = llast3[2]-lfirst3[2];	 

	 p[0] = dx/dz;
	 p[1] = dy/dz;
	 p[2] = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
	 ts3.SetP(p);
	 tsv.push_back(ts3);
  }

  //......................................................................

  void emph::MakeSingleTracks::produce(art::Event& evt)
  {
    tsv.clear();
    spv.clear();

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

      std::string fG4Label = "geantgen";
      art::Handle< std::vector<sim::SSDHit> > ssdHitH;
 
      //std::string fLineSegLabel = "";
      // art::Handle< std::vector<rb::LineSegment> > lsH;

      try {
	evt.getByLabel(fG4Label,ssdHitH);
      } 
      catch(...) {
	std::cout << "WARNING: No SSDHits found!" << std::endl;
      }

      bool goodEvent = false;
      bool goodHit = false;
      
      try {
	evt.getByLabel(fClusterLabel, clustH);
	if (!clustH->empty()){
	  for (size_t idx=0; idx < clustH->size(); ++idx) {
	    const rb::SSDCluster& clust = (*clustH)[idx];
	    ++clustMap[std::pair<int,int>(clust.Station(),clust.Plane())];
	    clusters.push_back(&clust); 
	  }
	  //line segments
	  //evt.getByLabel(fLineSegLabel, lsH);
	  //for (size_t idx=0; idx < lsH->size(); ++idx) {
	  //  const rb::LineSegment& lineseg = (*lsH)[idx];
	  //  linesegments.push_back(&lineseg);
	  //}

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

	  rb::LineSegment strip;
          if (goodEvent == true && clusters.size() > 0){
	    for (size_t i=0; i<clusters.size(); i++){
	      stripv.push_back(strip);
	      MakeSegment(*clusters[i],stripv[i]);
		
	    }
          }

	  if (goodEvent == true && stripv.size() > 0){
	    for (size_t i=0; i<clusters.size(); i++){
	      int plane = clusters[i]->Plane();
	      int station = clusters[i]->Station();
	      ls_group[station][plane].push_back(&stripv[i]);
	      //ls_group[station][plane].push_back(&linesegments[i]);
	      //this assumes they are ordered the same? could loop over clusters or line segments?
	      //does it matter?
	    }
	    //make reconstructed hits
	    MakeHits();
	    for (auto sp : spv)
	      spacepointv->push_back(sp);
	  }

          ls_group.clear();
          cl_group.clear();
	  stripv.clear();
	  clusters.clear();
          clustMap.clear();

          //get truth info
          if (goodEvent && spv.size() > 0 && ssdHitH->size() == nPlanes){
	    for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
	      const sim::SSDHit& ssdhit = (*ssdHitH)[idx];

	      // if you use the magnetic field survey
              //if (ssdhit.GetStation() == 4){
              //   if (abs(ssdhit.GetX()) < 15 && abs(ssdhit.GetY()) < 15) goodHit = true;
              //   else std::cout<<"SSDHit outside magnetic field map bounds."<<std::endl;
              //}

              // if not
	      goodHit = true;
	    }	

	    if (goodHit){
	      for (size_t i=0; i<spv.size(); i++){
	        std::vector<double> x = {spv[i].Pos()[0],spv[i].Pos()[1],spv[i].Pos()[2]};	

	        //reconstructed hits
	        if (emgeo->GetTarget()){
                  if (spv[i].Pos()[2] < emgeo->GetTarget()->Pos()(2)) sp1.push_back(x);
                  if (spv[i].Pos()[2] > emgeo->GetTarget()->Pos()(2) && spv[i].Pos()[2] < emgeo->MagnetUSZPos()) sp2.push_back(x);
                  if (spv[i].Pos()[2] > emgeo->MagnetDSZPos()) sp3.push_back(x);
                }
	      }

	      //form lines and fill plots
	      MakeLines();
	      for (auto ts : tsv) {
	        tracksegmentv->push_back(ts);	     
	      }
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
      sectrk.SetP(tsv[1].P()); // this should come from an analysis of the bend angle between track segments 1 and 2.
      sectrk.SetVtx(tsv[1].Vtx()); // this should come from a calculation of the intersection or point of closest approach between track segments 0 and 1.
      trackv->push_back(sectrk);
    }

    evt.put(std::move(spacepointv));
    evt.put(std::move(tracksegmentv));
    evt.put(std::move(trackv));
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MakeSingleTracks)
