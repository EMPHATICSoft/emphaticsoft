////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create alignment constants for SSD
///          alignment
/// \author  $Author: jmirabit@bu.edu and wanly@bu.edu$
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TGraph.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/ChannelMap.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "RawData/TRB3RawDigit.h"
#include "SSDAlignment/SSDAlign.h"
#include "RecoBase/SSDCluster.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class SSDAlignment : public art::EDProducer {
  public:
    explicit SSDAlignment(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~SSDAlignment();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    void Fit();
    
  private:
    
    emph::cmap::ChannelMap* fChannelMap;
    runhist::RunHistory* fRunHistory;
    emph::geo::Geometry *emgeo;
    int         fEvtNum;
    std::vector<emph::al::SSDAlign> ssdvec;
    TH2F* fSSD_Profile[6];
    TH2F* fadj_SSD_Profile[6];
    TH1F* fX_Residual_Init[8];
    TH1F* fY_Residual_Init[8];
    TGraph* evt_line;
    TGraph** evt_disp;
    TGraph** evt_disp_adj;
    TGraph* sing_disp;
    TF1* fit;
    art::ServiceHandle<art::TFileService> tfs;
    
  };

  //.......................................................................
  
  SSDAlignment::SSDAlignment(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {
    evt_disp = new TGraph*[10];
    evt_disp_adj = new TGraph*[10];
    fEvtNum = 0;

  }

  //......................................................................
  
  SSDAlignment::~SSDAlignment()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void SSDAlignment::beginJob()
  {
    char hname[64];
    for (int i=0; i<=5; ++i) {
      sprintf(hname,"SSDProfile_%d",i);
      fSSD_Profile[i] = tfs->make<TH2F>(hname,Form("Station %i",i),160,-40,40,160,-40,40);
      fSSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
      fSSD_Profile[i]->GetYaxis()->SetTitle("Y Position (mm)");
      //fSSD_Profile[i]->SetBit(TH1::kNoStats);
    }

    for (int i=0; i<=5; ++i) {
      sprintf(hname,"adj_SSDProfile_%d",i);
      fadj_SSD_Profile[i] = tfs->make<TH2F>(hname,Form("Station %i",i),160,-40,40,160,-40,40);
      fadj_SSD_Profile[i]->GetXaxis()->SetTitle("X Position (mm)");
      fadj_SSD_Profile[i]->GetYaxis()->SetTitle("Y Position (mm)");
      //fadj_SSD_Profile[i]->SetBit(TH1::kNoStats);
    }

    for (int i=0; i<8; ++i) {
      sprintf(hname,"XInit_Resid_%d",i);
      fX_Residual_Init[i] = tfs->make<TH1F>(hname,Form("X Residuals - SSD %i",i),200,-5,5);
      fX_Residual_Init[i]->GetXaxis()->SetTitle("Residual");
      fX_Residual_Init[i]->GetYaxis()->SetTitle("Counts");
    }
    for (int i=0; i<8; ++i) {
      sprintf(hname,"YInit_Resid_%d",i);
      fY_Residual_Init[i] = tfs->make<TH1F>(hname,Form("X Residuals - SSD %i",i),200,-10,10);
      fY_Residual_Init[i]->GetXaxis()->SetTitle("Residual");
      fY_Residual_Init[i]->GetYaxis()->SetTitle("Counts");
    }

    double x[1]={1}, y[1]={1};
    sing_disp = tfs->makeAndRegister<TGraph>("test","test",1,x,y);
    sing_disp->GetXaxis()->SetTitle("Z Position (mm)");
    sing_disp->GetYaxis()->SetTitle("Y Position (mm)");


  }

  //......................................................................
  
  void SSDAlignment::beginRun(art::Run& run)
  {
    // initialize channel map
    fChannelMap = new emph::cmap::ChannelMap();
    fRunHistory = new runhist::RunHistory(run.run());
    fChannelMap->LoadMap(fRunHistory->ChanFile());
    emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
  }
    
  //......................................................................
  
  void SSDAlignment::endJob()
  {
    Fit();
    //Then output the alignment constants to ConstBase/SSDAlignment.dat
  }
  
  //......................................................................
  
  void SSDAlignment::Fit()
  {
    //Fit the ssdvectors and obtain x, y shifts

    std::cout<<"*******************************************"<<std::endl;
    std::cout<<"Main code"<<std::endl;

    //Initialize vectors for holding x/y positions for each event and their corresponding z positions
    std::vector<std::vector<double>> xpos = {{},{},{},{},{},{},{},{}};
    std::vector<std::vector<double>> ypos = {{},{},{},{},{},{},{},{}};
    std::vector<std::vector<double>> upos = {{},{},{},{},{},{},{},{}};
    std::vector<std::vector<double>> vpos = {{},{},{},{},{},{},{},{}};

    //Initialize vectors to hold positions from events used for alignment
    std::vector<std::vector<double>> x_cal = {};
    std::vector<std::vector<int>> x_ind = {};
    std::vector<std::vector<double>> y_cal = {};
    std::vector<std::vector<int>> y_ind = {};
    std::vector<std::vector<double>> u_cal = {};
    std::vector<std::vector<int>> ux_ind = {};
    std::vector<std::vector<int>> uy_ind = {};
    std::vector<std::vector<double>> v_cal = {};
    std::vector<std::vector<int>> v_ind = {};
    std::vector<double> xz_cal;
    std::vector<double> yz_cal;
    std::vector<double> uz_cal={0.45, 120.95, 360.45, 481.45, 988.75,1214.95};
    std::vector<double> vz_cal={0.45, 120.95, 363.15, 484.15, 986.05,1212.25};

    int nstation = emgeo->NSSDStations();
    for ( int i = 0; i < nstation; i++){
      emph::geo::SSDStation sta = emgeo->GetSSDStation(i);
      int nplanes = sta.NPlanes();
      for ( int k = 0; k < nplanes; ++k){
	emph::geo::Plane pln = sta.GetPlane(k);
	int nsensor = pln.NSSDs();
	for ( int j = 0; j < nsensor; j++){
	  emph::geo::Detector sensor = pln.SSD(j);
	  std::cout << "The " << j <<"-th SSD sensor in the " << k << "-th plane in the " << i <<"-th SSD station is located at " << sensor.Pos()[0] << " " << sensor.Pos()[1] << " " << sensor.Pos()[2]+sta.Pos()[2] << " mm." << std::endl;
	  std::cout << "The rotation angle is " << sensor.Rot() << std::endl;
	  //Creating vectors for z positions of x and y ssds		
	  if(sensor.Rot() == 3*M_PI/2) xz_cal.push_back(sensor.Pos()[2]+sta.Pos()[2]);
	  else if(sensor.Rot() == 0 ) yz_cal.push_back(sensor.Pos()[2]+sta.Pos()[2]);
	}
      }
    }

    //Looping over all SSD hits
    int nevt = ssdvec[0].Event(); //Variable to keep track of event number
    std::cout<<"ssdvec size is  "<<ssdvec.size()<<std::endl;
    for (size_t i=0; i<ssdvec.size(); ++i){
      //if statement to indicate end of event, events are filtered and pushed to finalized arrays for calculating residuals, then cleared for next event
      if (ssdvec[i].Event()!=nevt){
	//Keeping good events for x alignment	
	if(ssdvec[i].IsAlignmentEvent(xpos)) {
	  std::vector<double> station_x = ssdvec[i].PositionAtStations(xpos);
	  x_cal.push_back(station_x);
	  std::vector<int> station_index = ssdvec[i].IndexAtStations(xpos);
	  x_ind.push_back(station_index);
	}	

	//Keeping good events for y alignment	
	if(ssdvec[i].IsAlignmentEvent(ypos)) {
	  std::vector<double> station_y = ssdvec[i].PositionAtStations(ypos);
	  y_cal.push_back(station_y);
	  std::vector<int> station_index = ssdvec[i].IndexAtStations(ypos);
	  y_ind.push_back(station_index);
	}

	//Keeping good events for u alignment	
	if(ssdvec[i].IsAlignmentEvent(xpos,ypos,upos)) {
	  std::vector<double> station_u = ssdvec[i].PositionAtStations(xpos,ypos,upos);
	  u_cal.push_back(station_u);
	  std::vector<int> xstation_index = ssdvec[i].IndexAtStations(xpos);
	  std::vector<int> ystation_index = ssdvec[i].IndexAtStations(ypos);
	  ux_ind.push_back(xstation_index);
	  uy_ind.push_back(ystation_index);
	}

	//Keeping good events for v alignment	
	if(ssdvec[i].IsAlignmentEvent(xpos,ypos,vpos)) {
	  std::vector<double> station_v = ssdvec[i].PositionAtStations(xpos,ypos,vpos);
	  v_cal.push_back(station_v);
	  std::vector<int> station_index = ssdvec[i].IndexAtStations(vpos);
	  v_ind.push_back(station_index);
	}

			
			
			
	nevt = ssdvec[i].Event();

	//Reset vectors for next event
	for (size_t j=0; j<xpos.size(); ++j){
	  xpos[j] = {};
	  ypos[j] = {};
	  upos[j] = {};
	  vpos[j] = {};
	}
					
      }
		
      //std::cout<<"Checking Event: "<< nevt<<std::endl;
      //Calculating and storing x/y positions based on SSD hits
      //ssdvec[i].FillSSDPosition(xpos,ypos,xzpos,yzpos);
      ssdvec[i].SSDHitPosition(xpos,ypos,upos,vpos);
    }
	
    //Aligning SSDs below
    std::cout<<"Z positions of X SSDs:   ";
    for (size_t i=0; i<xz_cal.size(); ++i){
      std::cout<<xz_cal[i]<<", ";
    }
    std::cout<<std::endl;
    std::cout<<"Z positions of Y SSDs:   ";
    for (size_t i=0; i<yz_cal.size(); ++i){
      std::cout<<yz_cal[i]<<", ";
    }
    std::cout<<std::endl;	
    std::cout<<"Z positions of U SSDs:   ";
    for (size_t i=0; i<uz_cal.size(); ++i){
      std::cout<<uz_cal[i]<<", ";
    }
    std::cout<<std::endl;	
    std::cout<<"Z positions of V SSDs:   ";
    for (size_t i=0; i<vz_cal.size(); ++i){
      std::cout<<vz_cal[i]<<", ";
    }
    std::cout<<std::endl;	
	
    //Residual Method of Alignment
    int loops=100;
    size_t dim = x_cal[0].size(); //number of position measurements (should be 6)
    art::ServiceHandle<art::TFileService> tfs;
    std::cout<<"Dimension: "<<dim<<std::endl;

    //Linear Fit function
    TF1* fit = new TF1("fit","[0] +[1]*x",0,1250);
    std::vector<double> x_shifts = {0,0,0,0,0,0,0,0};
    std::vector<double> y_shifts = {0,0,0,0,0,0,0,0};
    std::vector<double> u_shifts = {0,0,0,0,0,0,0,0};
    std::vector<double> v_shifts = {0,0,0,0,0,0,0,0};

    for (int i=0; i<loops; ++i){
      std::cout<<"Beginning Loop "<<i<<std::endl;
      std::vector<std::vector<double>> xres_array = {{},{},{},{},{},{},{},{}};
      std::vector<double> x_adj = {{},{},{},{},{},{}};
		
      //Looping through alignment events and calculating x-residuals
      for (size_t j=0; j<x_cal.size(); ++j){
				
	//Setting adjusted positions to be measured positions on first pass
	if (i==0)x_adj = x_cal[j];	
	//Setting Adjusted Position Measurements
	for(size_t k=0; k<x_adj.size(); ++k){
	  x_adj[k] = x_cal[j][k]-x_shifts[x_ind[j][k]];
	}

	//TGraph* evt_line = new TGraph(dim, &xz_cal[0], &x_cal[j][0]);
	TGraph* evt_line = new TGraph(dim, &xz_cal[0], &x_adj[0]);
	evt_line->Fit(fit,"Q0");

	//Checking Residual for each point
	for(size_t k=0; k<dim; ++k){
	  double res = x_adj[k] - fit->Eval(xz_cal[k],0,0);
	  //double delta_x = (res*sqrt(12)/pitch)

	  //Filling residual plots using correct index of SSD
	  fX_Residual_Init[x_ind[j][k]]->Fill(res);
	  xres_array[x_ind[j][k]].push_back(res);
	}
	//Store first 10 events (unaligned) to look at
	if (j<10 && i==0){
	  int event_holder = j;  //need to convert j to int for naming Event displays
	  evt_disp[j] = tfs->makeAndRegister<TGraph>(Form("event_%i",event_holder),"Unaligned Event",dim, &xz_cal[0], &x_adj[0]);
	  evt_disp[j]->Fit(fit,"Q");
	  evt_disp[j]->SetMarkerStyle(21);
	  evt_disp[j]->SetTitle(Form("Unaligned Alignment Event %i",event_holder));
	  evt_disp[j]->GetXaxis()->SetTitle("z pos (mm)");
	  evt_disp[j]->GetXaxis()->SetLimits(0,1250);
	  evt_disp[j]->GetYaxis()->SetTitle("x pos (mm)");
	  evt_disp[j]->GetYaxis()->SetRangeUser(-40,40);
	}	
	if (j<10 && i==loops-1){
	  int event_holder = j;  //need to convert j to int for naming Event displays
	  evt_disp_adj[j] = tfs->makeAndRegister<TGraph>(Form("adj_event_%i",event_holder),"Aligned Event",dim, &xz_cal[0], &x_adj[0]);
	  evt_disp_adj[j]->Fit(fit,"Q");
	  evt_disp_adj[j]->SetMarkerStyle(21);
	  evt_disp_adj[j]->SetTitle(Form("Aligned Alignment Event %i",event_holder));
	  evt_disp_adj[j]->GetXaxis()->SetTitle("z pos (mm)");
	  evt_disp_adj[j]->GetXaxis()->SetLimits(0,1250);
	  evt_disp_adj[j]->GetYaxis()->SetTitle("x pos (mm)");
	  evt_disp_adj[j]->GetYaxis()->SetRangeUser(-40,40);
	}
      }

      //Repeat Process for Y
      std::vector<std::vector<double>> yres_array = {{},{},{},{},{},{},{},{}};
      std::vector<double> y_adj = {{},{},{},{},{},{}};
      //Looping through alignment events and calculating y-residuals
      for (size_t j=0; j<y_cal.size(); ++j){
	//Setting adjusted positions to be measured positions on first pass
	if (i==0)y_adj = y_cal[j];	
	//Setting Adjusted Position Measurements
	for(size_t k=0; k<y_adj.size(); ++k){
	  y_adj[k] = y_cal[j][k]-y_shifts[y_ind[j][k]];
	}

	TGraph* evt_line = new TGraph(dim, &yz_cal[0], &y_adj[0]);
	evt_line->Fit(fit,"Q0");

	//Checking Residual for each point
	for(size_t k=0; k<dim; ++k){
	  double res = y_adj[k] - fit->Eval(yz_cal[k],0,0);

	  //Filling residual plots using correct index of SSD
	  fY_Residual_Init[y_ind[j][k]]->Fill(res);
	  yres_array[y_ind[j][k]].push_back(res);
	}
      }

      //Repeat Process for U
      std::vector<std::vector<double>> ures_array = {{},{},{},{},{},{},{},{}};
      std::vector<double> u_adj = {{},{},{},{},{},{}};
      //Looping through alignment events and calculating u-residuals
      for (size_t j=0; j<u_cal.size(); ++j){
	//Setting adjusted positions to be measured positions on first pass
	if (i==0)u_adj = u_cal[j];	
	//Setting Adjusted Position Measurements
	for(size_t k=0; k<u_adj.size(); ++k){
	  //Adjust using u residuals if a u SSD exits for this measurement
	  if(k==2 || k==3) u_adj[k] = u_cal[j][k]-u_shifts[k];
	  else u_adj[k] = u_cal[j][k]-(((sqrt(2))/2)*(x_shifts[ux_ind[j][k]]-y_shifts[uy_ind[j][k]]));
	}

	TGraph* evt_line = new TGraph(dim, &uz_cal[0], &u_adj[0]);
	evt_line->Fit(fit,"Q0");

	//Checking Residual for each point
	for(size_t k=0; k<dim; ++k){
	  double res = u_adj[k] - fit->Eval(uz_cal[k],0,0);

	  //Filling residual plots using correct index of SSD
	  ures_array[ux_ind[j][k]].push_back(res);
	}
			
      }

      //Repeat Process for V
      std::vector<std::vector<double>> vres_array = {{},{},{},{},{},{},{},{}};
      std::vector<double> v_adj = {{},{},{},{},{},{}};

      for (size_t j=0; j<v_cal.size(); ++j){
	//Setting adjusted positions to be measured positions on first pass
	if (i==0)v_adj = v_cal[j];	
	//Setting Adjusted Position Measurements
	for(size_t k=0; k<v_adj.size(); ++k){
	  //Adjust using v residuals if a v SSD exits for this measurement
	  if(k==4 || k==5) v_adj[k] = v_cal[j][k]-v_shifts[v_ind[j][k]];
	  else v_adj[k] = v_cal[j][k]-(((sqrt(2))/2)*(x_shifts[v_ind[j][k]]+y_shifts[v_ind[j][k]]));
	}

	TGraph* evt_line = new TGraph(dim, &vz_cal[0], &v_adj[0]);
	evt_line->Fit(fit,"Q0");

	//Checking Residual for each point
	for(size_t k=0; k<dim; ++k){
	  double res = v_adj[k] - fit->Eval(vz_cal[k],0,0);

	  //Filling residual plots using correct index of SSD
	  vres_array[v_ind[j][k]].push_back(res);
	}
      }

      //variables to fix first x/y ssd station
      double x_ref, y_ref;
      std::cout<<"X Shifts are:    ";
      for(size_t j=0; j<x_shifts.size(); ++j){
	double sum=0;
	//Calculating average residual
	for(size_t k=0; k<xres_array[j].size(); ++k){
	  sum +=xres_array[j][k];
	}
	double mean = sum / xres_array[j].size();
	x_shifts[j] += mean;
	//Fixing first X SSD at x=0 (shift all SSDs in X by x_shift[0])
	if(j==0) x_ref = x_shifts[0];	
	x_shifts[j]=x_shifts[j]-x_ref;
	std::cout<<x_shifts[j]<<", ";
      }
      std::cout<<std::endl;

      std::cout<<"Y Shifts are:    ";
      for(size_t j=0; j<y_shifts.size(); ++j){
	double sum=0;
	//Calculating average residual
	for(size_t k=0; k<yres_array[j].size(); ++k){
	  sum +=yres_array[j][k];
	}
	double mean = sum / yres_array[j].size();
	y_shifts[j] += mean;
	//Fixing first Y SSD at y=0 (shift all SSDs in Y by y_shift[0])
	if(j==0) y_ref = y_shifts[0];
	y_shifts[j]=y_shifts[j]-y_ref;
	std::cout<<y_shifts[j]<<", ";
      }
      std::cout<<std::endl;

      std::cout<<"U Shifts are:    ";
      for(size_t j=0; j<u_shifts.size(); ++j){
	double sum=0;
	//Calculating average residual
	for(size_t k=0; k<ures_array[j].size(); ++k){
	  sum +=ures_array[j][k];
	}
	double mean = sum / ures_array[j].size();
	if(j==2 || j==3){
	  u_shifts[j] += mean;
	  //Fixing first X/Y SSD at (0,0) (shift all u SSDs accordingly)
	  u_shifts[j]=u_shifts[j]-(sqrt(2)/2)*(x_ref-y_ref);
	}
	std::cout<<u_shifts[j]<<", ";
      }
      std::cout<<std::endl;

      std::cout<<"V Shifts are:    ";
      for(size_t j=0; j<v_shifts.size(); ++j){
	double sum=0;
	//Calculating average residual
	for(size_t k=0; k<vres_array[j].size(); ++k){
	  sum +=vres_array[j][k];
	}
	double mean = sum / vres_array[j].size();
	if(j==4 || j==5 || j==6 || j==7){
	  v_shifts[j] += mean;
	  //Fixing first X/Y SSD at (0,0) (shift all v SSDs accordingly)
	  v_shifts[j]=v_shifts[j]-(sqrt(2)/2)*(x_ref+y_ref);
	}
	std::cout<<v_shifts[j]<<", ";
      }
      std::cout<<std::endl;
	
    }
    std::cout<<"Number of X Alignment Events: "<<x_cal.size()<<std::endl;
    std::cout<<"Number of Y Alignment Events: "<<y_cal.size()<<std::endl;
    std::cout<<"Number of U Alignment Events: "<<u_cal.size()<<std::endl;
    std::cout<<"Number of V Alignment Events: "<<v_cal.size()<<std::endl;

    for (size_t j=0; j<xpos.size(); ++j){
      xpos[j] = {};
      ypos[j] = {};
      upos[j] = {};
      vpos[j] = {};
    }

    //Checking SSD Shifts by Filling SSD Profiles with all hits that have x and y data
    double true_x=0;
    double true_y=0;
    for (size_t i=0; i<ssdvec.size(); ++i){
      //if statement to indicate end of event, events are filtered and pushed to finalized arrays for calculating residuals, then cleared for next event
      if (ssdvec[i].Event()!=nevt){

	//Adding event to SSD Profile if it contains 1 x and 1 y hit
	//Using these events to adjust U/W SSDs
	if(ssdvec[i].IsSingleHit(xpos[0]) && ssdvec[i].IsSingleHit(ypos[0])){
	  true_x=xpos[0][0]-x_shifts[0];
	  true_y=ypos[0][0]-y_shifts[0];
	  fSSD_Profile[0]->Fill(xpos[0][0],ypos[0][0]);
	  fadj_SSD_Profile[0]->Fill(true_x,true_y);
	}
	if(ssdvec[i].IsSingleHit(xpos[1]) && ssdvec[i].IsSingleHit(ypos[1])){
	  true_x=xpos[1][0]-x_shifts[1];
	  true_y=ypos[1][0]-y_shifts[1];
	  fSSD_Profile[1]->Fill(xpos[1][0],ypos[1][0]);
	  fadj_SSD_Profile[1]->Fill(true_x,true_y);
	}
	if(ssdvec[i].IsSingleHit(xpos[2]) && ssdvec[i].IsSingleHit(ypos[2])){
	  true_x=xpos[2][0]-x_shifts[2];
	  true_y=ypos[2][0]-y_shifts[2];
	  fSSD_Profile[2]->Fill(xpos[2][0],ypos[2][0]);
	  fadj_SSD_Profile[2]->Fill(true_x,true_y);
	}
	if(ssdvec[i].IsSingleHit(xpos[3]) && ssdvec[i].IsSingleHit(ypos[3])){
	  true_x=xpos[3][0]-x_shifts[3];
	  true_y=ypos[3][0]-y_shifts[3];
	  fSSD_Profile[3]->Fill(xpos[3][0],ypos[3][0]);
	  fadj_SSD_Profile[3]->Fill(true_x,true_y);
	}
	if((ssdvec[i].IsSingleHit(xpos[4])^ssdvec[i].IsSingleHit(xpos[5])) && (ssdvec[i].IsSingleHit(ypos[4])^ssdvec[i].IsSingleHit(ypos[5]))){
	  double temp_x, temp_y;
	  if (xpos[4].empty()) temp_x = xpos[5][0];
	  else  temp_x = xpos[4][0];
	  if (ypos[4].empty())  temp_y = ypos[5][0];
	  else  temp_y = ypos[4][0];
	  fSSD_Profile[4]->Fill(temp_x,temp_y);

	  if (xpos[4].empty())true_x = xpos[5][0]-x_shifts[5];
	  else true_x = xpos[4][0]-x_shifts[4];
	  if (ypos[4].empty()) true_y = ypos[5][0]-y_shifts[5];
	  else true_y = ypos[4][0]-y_shifts[4];
	  fadj_SSD_Profile[4]->Fill(true_x,true_y);
	}
	if((ssdvec[i].IsSingleHit(xpos[6])^ssdvec[i].IsSingleHit(xpos[7])) && (ssdvec[i].IsSingleHit(ypos[6])^ssdvec[i].IsSingleHit(ypos[7]))){
	  double temp_x, temp_y;
	  if (xpos[6].empty())  temp_x = xpos[7][0];
	  else  temp_x = xpos[6][0];
	  if (ypos[6].empty())  temp_y = ypos[7][0];
	  else  temp_y = ypos[6][0];
	  fSSD_Profile[5]->Fill(temp_x,temp_y);

	  if (xpos[6].empty()) true_x = xpos[7][0]-x_shifts[7];
	  else true_x = xpos[6][0]-x_shifts[6];
	  if (ypos[6].empty()) true_y = ypos[7][0]-y_shifts[7];
	  else true_y = ypos[6][0]-y_shifts[6];
	  fadj_SSD_Profile[5]->Fill(true_x,true_y);
	}
			
	nevt = ssdvec[i].Event();

	//Reset vectors for next event
	for (size_t j=0; j<xpos.size(); ++j){
	  xpos[j] = {};
	  ypos[j] = {};
	  upos[j] = {};
	  vpos[j] = {};
	}
		

					
      }
      //Calculating and storing x/y positions based on SSD hits
      ssdvec[i].SSDHitPosition(xpos,ypos,upos,vpos);
    }

    //Writing out shift constants to ConstBase/Alignment
    std::ofstream shift_file;
    shift_file.open ("SSDAlignment.dat");
    shift_file << "#Identifier x0 ... x21"<<std::endl;
    shift_file << "#Identifier y0 ... y21"<<std::endl;
    shift_file << "#Identifier r0 ... r21"<<std::endl;
    shift_file << "#Comment: #0 is the default alignment constant set."<<std::endl;

    shift_file << "0 X ";
    for (size_t i=0; i<x_shifts.size(); ++i){
      shift_file<<x_shifts[i]<<" ";
    }
    shift_file << std::endl;
    
    shift_file << "0 Y ";
    for (size_t i=0; i<y_shifts.size(); ++i){
      shift_file<<y_shifts[i]<<" ";
    }
    shift_file << std::endl;

    shift_file << "0 U ";
    for (size_t i=0; i<u_shifts.size(); ++i){
      shift_file<<u_shifts[i]<<" ";
    }
    shift_file << std::endl;

    shift_file << "0 V ";
    for (size_t i=0; i<v_shifts.size(); ++i){
      shift_file<<v_shifts[i]<<" ";
    }
    shift_file << std::endl;
    shift_file << "#Comment: #1 is the alignment constant set using Run xxxx..."<<std::endl;

    shift_file.close();
  }

  
  //......................................................................
  void SSDAlignment::produce(art::Event& evt)
  { 
	
    //std::string labelstr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::SSD));
    std::string fClusterLabel = "clust";
    //emph::cmap::FEBoardType boardType = emph::cmap::SSD;
    //emph::cmap::EChannel echan;
    //echan.SetBoardType(boardType);

    //art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdH;
    art::Handle< std::vector<rb::SSDCluster> > clustH;
	  
	  
    try {
      evt.getByLabel(fClusterLabel, clustH);
      if (!clustH->empty()) {	
	for (size_t idx=0; idx < clustH->size(); ++idx) {
	  const rb::SSDCluster& clust = (*clustH)[idx];
	  //echan.SetBoard(clust.FER());
	  //echan.SetChannel(clust.Module());
	  //emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	  //const emph::geo::SSDStation &st = emgeo->GetSSDStation(dchan.Station());
	  //const emph::geo::Detector &sd = st.GetSSD(dchan.Channel());

	  const emph::geo::SSDStation &st = emgeo->GetSSDStation(clust.Station());
	  const emph::geo::Plane &pl = st.GetPlane(clust.Plane());
	  const emph::geo::Detector &sd = pl.SSD(clust.Sensor());
				  
	  int event = evt.event();
	  emph::al::SSDAlign hit(clust, sd, st, event);
	  ssdvec.push_back(hit);
	}
	fEvtNum++;
      }
    }
    catch(...) {

    }
  }
} // end namespace emph

DEFINE_ART_MODULE(emph::SSDAlignment)
