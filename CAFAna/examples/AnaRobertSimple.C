#include "StandardRecord/StandardRecord.h"
#include "StandardRecord/SRVector3D.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TMultiGraph.h"

bool IsSig(caf::SRTruth& truth);
bool IsSigSimple(std::vector<caf::SRSimpleTruth> beamtruth, std::vector<caf::SRSimpleTruth> sectruth);
bool IsSel(std::vector<caf::SRVertex> vtx);
Double_t DoubleGaus(Double_t *x, Double_t *par);
void AnaRobertSimple(std::string fname)
{

  TChain *chain_evts = new TChain("recTree");

  std::ifstream inputFile(fname);
  std::string filename;

  if (inputFile.is_open()) {
    while (std::getline(inputFile, filename)) {
      chain_evts->Add(filename.c_str());
    }
    inputFile.close();
  }
  else {
    std::cerr << "Error opening file_list.txt" << std::endl;
  }

  double trueScatteringSig;
  ROOT::Math::XYZVector truep00sig(0,0,0);
  ROOT::Math::XYZVector truep0sig(0,0,0);
  ROOT::Math::XYZVector truep1sig(0,0,0);
  ROOT::Math::XYZVector truep2sig(0,0,0);
  ROOT::Math::XYZVector truep3sig(0,0,0);
  ROOT::Math::XYZVector truex00sig(0,0,0);
  ROOT::Math::XYZVector truex01sig(0,0,0);
  ROOT::Math::XYZVector truex10sig(0,0,0);
  ROOT::Math::XYZVector truex11sig(0,0,0);
  ROOT::Math::XYZVector truex20sig(0,0,0);
  ROOT::Math::XYZVector truex21sig(0,0,0);
  ROOT::Math::XYZVector truex22sig(0,0,0);
  ROOT::Math::XYZVector truex30sig(0,0,0);
  ROOT::Math::XYZVector truex31sig(0,0,0);
  ROOT::Math::XYZVector truex32sig(0,0,0);

  TH1F* hClusters = new TH1F("hClusters","hClusters",50,0,50);
  TH2F* hClustersVsPlane = new TH2F("hClustersVsPlane","hClustersVsPlane",21,0,21,50,0,50);
  TH1F* hClustersNotSel = new TH1F("hClustersNotSel","hClustersNotSel",50,0,50);
  TH2F* hClustersVsPlaneNotSel = new TH2F("hClustersVsPlaneNotSel","hClustersVsPlaneNotSel",21,0,21,50,0,50);
  TH1F* hNSigTruth = new TH1F("hNSigTruth","hNSigTruth",180,0.,0.018);
  TH1F* hNSigSelTruth = new TH1F("hNSigSelTruth","hNSigSelTruth",180,0.,0.018);
  TH1F* hNSelReco = new TH1F("hNSelReco","hNSelReco",180,0.,0.018);
  TH1F* hNSelSigReco = new TH1F("hNSelSigReco","hNSelSigReco",180,0.,0.018);

  TH1F* hRecoScattering = new TH1F("hRecoScattering","hRecoScattering",36,0.,0.018);
  TH1F* hRecoScatteringXZ = new TH1F("hRecoScatteringXZ","hRecoScatteringXZ",100,-0.005,0.005);
  TH1F* hRecoScatteringYZ = new TH1F("hRecoScatteringYZ","hRecoScatteringYZ",100,-0.005,0.005);
  TH1F* hScatResXZ = new TH1F("hScatResXZ","hScatResXZ",100,-0.005,0.005);
  TH1F* hScatResYZ = new TH1F("hScatResYZ","hScatResYZ",100,-0.005,0.005);
  TH2F* hScatResYZvsXZ = new TH2F("hScatResYZvsXZ","hScatResYZvsXZ",100,-0.005,0.005,100,-0.005,0.005);
  TH1F* hRecoBending = new TH1F("hRecoBending","hRecoBending",1500,0.,0.15);
  TH1F* hBendAngleRes = new TH1F("hBendAngleRes","hBendAngleRes",100,-0.05,0.05);
  TH2F* hBeamProfileXY = new TH2F("hBeamProfileXY","hBeamProfileXY",100,-50.,50.,100,-50.,50.);
  TH1F* hVertexX = new TH1F("hVertexX","hVertexX",100,-50.,50.);
  TH1F* hVertexY = new TH1F("hVertexY","hVertexY",100,-50.,50.);
  TH1F* hVertexZ = new TH1F("hVertexZ","hVertexZ",200,0.,2000.);
  TH1F* hP2Theta2 = new TH1F("hP2Theta2","hP2Theta2",60,0.,0.15);
  TH1F* hP2Theta2Truth = new TH1F("hP2Theta2Truth","hP2Theta2Truth",60,0.,0.15);
  TH1F* hCrossSection = new TH1F("hCrossSection","hCrossSection",60,0.,0.15);
  TH1F* hCrossSectionTruth = new TH1F("hCrossSectionTruth","hCrossSectionTruth",60,0.,0.15);
  TH1F* hCrossSectionScat = new TH1F("hCrossSectionScat","hCrossSectionScat",36,0.,0.018);
  TH1F* hTrk0dXdZ = new TH1F("hTrk0dXdZ","hTrk0dXdZ",100,-0.005,0.005);
  TH1F* hTrk0dYdZ = new TH1F("hTrk0dYdZ","hTrk0dYdZ",100,0.005,0.015);
  //TH1F* hTrk0dYdZ = new TH1F("hTrk0dYdZ","hTrk0dYdZ",100,-0.005,0.005);
  TH1F* hTrk1dXdZ = new TH1F("hTrk1dXdZ","hTrk1dXdZ",100,-0.005,0.005);
  TH1F* hTrk1dYdZ = new TH1F("hTrk1dYdZ","hTrk1dYdZ",100,0.0,0.01);
  TH2F* hTrk1dXdZvsX = new TH2F("hTrk1dXdZvsX","hTrk1dXdZvsX",100,-50.,50.,100,-0.005,0.005);
  TH2F* hTrk1dYdZvsY = new TH2F("hTrk1dYdZvsY","hTrk1dYdZvsY",100,-50.,50.,100,0.,0.01);
  TH2F* hScatteringAngleResVsTrueAngle = new TH2F("hScatteringAngleResVsTrueAngle","hScatteringAngleResVsTrueAngle",180,0.,0.018,100,-0.005,0.005);
  TH2F* hScatteringAngleResVsTrueAngleZoom = new TH2F("hScatteringAngleResVsTrueAngleZoom","hScatteringAngleResVsTrueAngleZoom",100,0.,0.0001,100,-0.001,0.009);
  TH2F* hScatteringAngleRes2VsTrueAngleZoom = new TH2F("hScatteringAngleRes2VsTrueAngleZoom","hScatteringAngleRes2VsTrueAngleZoom",100,0.,0.0001,100,-0.001,0.009);
  TProfile* tScatteringAngleResVsTrueAngle = new TProfile("tScatteringAngleResVsTrueAngle","tScatteringAngleResVsTrueAngle",180,0.,0.018,-0.005,0.005);
  TH1F* hScatteringAngleResolution = new TH1F("hScatteringAngleResolution","hScatteringAngleResolution",100,-0.005,0.005);
  TH1F* hScatteringAngleResolution2 = new TH1F("hScatteringAngleResolution2","hScatteringAngleResolution2",1100,-0.003,0.003);
  TH1F* hScatteringAngleResolution3 = new TH1F("hScatteringAngleResolution3","hScatteringAngleResolution3",100,-0.005,0.005);
  TH1F* hScatteringAngleResolution4 = new TH1F("hScatteringAngleResolution4","hScatteringAngleResolution4",100,-0.005,0.005);
  TH1F* hScatteringAngleResolutionATan2 = new TH1F("hScatteringAngleResolutionATan2","hScatteringAngleResolutionATan2",100,-0.005,0.005);

  TH2F* hScatteringAngleMigrationMatrix = new TH2F("hScatteringAngleMigrationMatrix","hScatteringAngleMigrationMatrix",36,0.,0.018,36,0.,0.018);
  TH2F* hMigrationMatrixRelative = new TH2F("hMigrationMatrixRelative","hMigrationMatrixRelative",36,0.,0.018,36,0.,0.018);
  TH2F* hRecoTS1XY = new TH2F("hRecoTS1XY","hRecoTS1XY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoTS2XY = new TH2F("hRecoTS2XY","hRecoTS2XY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoTS3XY = new TH2F("hRecoTS3XY","hRecoTS3XY",100,-50.,50.,100,-50.,50.);
  TH1F* hMomentumRes = new TH1F("hMomentumRes","hMomentumRes",1000,-2.,2.);
  TH1F* hRecoBeamAnglePxPz = new TH1F("hRecoBeamAnglePxPz","hRecoBeamAnglePxPz",100,-0.001,0.001);
  TH1F* hRecoBeamAnglePyPz = new TH1F("hRecoBeamAnglePyPz","hRecoBeamAnglePyPz",100,-0.001,0.001);
  TH1F* hResX00 = new TH1F("hResX00","hResX00",100,-1,1);
  TH1F* hResY00 = new TH1F("hResY00","hResY00",100,-1,1);
  TH1F* hResX11 = new TH1F("hResX11","hResX11",100,-1,1);
  TH1F* hResY11 = new TH1F("hResY11","hResY11",100,-1,1);
  TH1F* hResX20 = new TH1F("hResX20","hResX20",100,-1,1);
  TH1F* hResY20 = new TH1F("hResY20","hResY20",100,-1,1);
  TH1F* hResX32 = new TH1F("hResX32","hResX32",100,-1,1);
  TH1F* hResY32 = new TH1F("hResY32","hResY32",100,-1,1);

  int nsig = 0;
  int nsigsel = 0;
  int dontcare = 0;
  int process = 0;
  std::map<std::pair<int, int>, int> clustMap;

  caf::StandardRecord* rec = 0;
  chain_evts->SetBranchAddress("rec", &rec);
  int nentries  = chain_evts->GetEntries();

  std::cout<<"Entries = "<< chain_evts->GetEntries()<<std::endl;

  int pot = 0;

  for(int i=0;i<nentries;i++){
    chain_evts->GetEntry(i);

// FOR DATA ONLY
/*
    bool useEvent = false;
    if (rec->evtqual.hasssdhits && rec->evtqual.hast0trb3 && rec->evtqual.hast0caen) useEvent = true;
    if (!useEvent) continue;
*/
//    std::string digitStr = std::to_string(rec->hdr.evt);
//    if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
//      continue;
//    }
    pot++;
  }
  
  std::cout<<"POT = "<<pot<<std::endl;

  for(int i=0;i<nentries;i++){
    //std::cout<<"Entry "<<i<<std::endl;
    chain_evts->GetEntry(i);

    clustMap.clear();

    // Select 70% of data entries not already used for alignment
//    std::string digitStr = std::to_string(rec->hdr.evt);

    // Spit out Run number for the first entry only
//    if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;

//    if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
//      continue;
//    }

// FOR DATA ONLY
/*
    bool useEvent = false;
    if (rec->evtqual.hasssdhits && rec->evtqual.hast0trb3 && rec->evtqual.hast0caen) useEvent = true;
    if (!useEvent) continue;
*/
    int nclusters = int(rec->cluster.clust.size());
    hClusters->Fill(nclusters);

    for(int i=0; i<nclusters; i++){
      caf::SRSSDClust& c = rec->cluster.clust[i];
      ++clustMap[std::pair<int,int>(c.station,c.plane)];
    }

    if (!clustMap.empty()){
      for (auto i : clustMap){
        int p = 0;
        if (i.first.first == 0 && i.first.second == 0) p=1; 
        if (i.first.first == 0 && i.first.second == 1) p=2; 
        if (i.first.first == 1 && i.first.second == 0) p=3;  
        if (i.first.first == 1 && i.first.second == 1) p=4; 
        if (i.first.first == 2 && i.first.second == 0) p=5;
        if (i.first.first == 2 && i.first.second == 1) p=6;
        if (i.first.first == 2 && i.first.second == 2) p=7;
        if (i.first.first == 3 && i.first.second == 0) p=8;
        if (i.first.first == 3 && i.first.second == 1) p=9;
        if (i.first.first == 3 && i.first.second == 2) p=10;
        if (i.first.first == 4 && i.first.second == 0) p=11;
        if (i.first.first == 4 && i.first.second == 1) p=12;
        if (i.first.first == 5 && i.first.second == 0) p=13;
        if (i.first.first == 5 && i.first.second == 1) p=14;
        if (i.first.first == 5 && i.first.second == 2) p=15;
        if (i.first.first == 6 && i.first.second == 0) p=16;
        if (i.first.first == 6 && i.first.second == 1) p=17;
        if (i.first.first == 6 && i.first.second == 2) p=18;
        if (i.first.first == 7 && i.first.second == 0) p=19;
        if (i.first.first == 7 && i.first.second == 1) p=20;

        hClustersVsPlane->Fill(p,i.second);

      }
    }
    else {
      for (int i=1; i<20; i++) hClustersVsPlane->Fill(i,0);
    }

    ROOT::Math::XYZVector recopv = rec->vtxs.vtx[0].sectrk[0].mom;
    double recofit = recopv.R();
    double recotruth = rec->truth.beam.caf::SRParticle::P()/1000.; 
    //double momres = (recotruth-recofit)/recotruth;
    double momres = recotruth-recofit;

    double truebeampx  = rec->truth.beam.caf::SRParticle::Px()/1000.;
    double truebeampy  = rec->truth.beam.caf::SRParticle::Py()/1000.;
    double truebeampz  = rec->truth.beam.caf::SRParticle::Pz()/1000.;
    double truebeamangle_pxpz = truebeampx/truebeampz;
    double truebeamangle_pypz = truebeampy/truebeampz;
    ROOT::Math::XYZVector p0 = rec->vtxs.vtx[0].beamtrk.mom;
    hRecoBeamAnglePxPz->Fill(p0.X()/p0.Z());
//std::cout<<"reco py/pz = "<<p0.Y()/p0.Z()<<std::endl;
    hRecoBeamAnglePyPz->Fill(p0.Y()/p0.Z());
//    hTrueBeamAnglePxPz->Fill(truebeamangle_pxpz);
//    hTrueBeamAnglePyPz->Fill(truebeamangle_pypz);    

    int nvtx = int(rec->vtxs.vtx.size());
    int nsectrk = 0;
    int nbeamtrk = 0;
    if (IsSig(rec->truth)){
      nsig++;

      for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
        caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	//std::cout<<"Process: "<<h.process<<std::endl;
        if (h.station == 0 && h.plane == 0 && h.pid == 2212) {
          //std::cout<<"Process at (0,0): "<<h.process<<std::endl;
          truep00sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z());		
          truex00sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
	}
	if (h.station == 0 && h.plane == 1 && h.pid == 2212) {
          //std::cout<<"Process at (0,1): "<<h.process<<std::endl;
          truex01sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
        }
	if (h.station == 1 && h.plane == 0 && h.pid == 2212) {
          //std::cout<<"Process at (1,0): "<<h.process<<std::endl;
          truex10sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
        }
	if (h.station == 1 && h.plane == 1 && h.pid == 2212) {
          //std::cout<<"Process at (1,1): "<<h.process<<std::endl;
          truep0sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z()); 
	  truex11sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
	}
        if (h.station == 2 && h.plane == 0 && h.pid == 2212) {
          //std::cout<<"Process at (2,0): "<<h.process<<std::endl;
	  if (h.process != "") process++; 
          truep1sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z());
	  truex20sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z()); 
        }
	if (h.station == 2 && h.plane == 1 && h.pid == 2212) {
          truex21sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
        }
	if (h.station == 2 && h.plane == 2 && h.pid == 2212) {
          truex22sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
        }
	if (h.station == 3 && h.plane == 0 && h.pid == 2212) {
          truex30sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
        }
	if (h.station == 3 && h.plane == 1 && h.pid == 2212) {
          truex31sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
        }
	if (h.station == 3 && h.plane == 2 && h.pid == 2212) {
          truep2sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z());
          truex32sig.SetXYZ(h.pos.X(),h.pos.Y(),h.pos.Z());
	}
        if (h.station == 6 && h.plane == 0 && h.pid == 2212) {
          truep3sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z());
        }

      }
//std::cout<<"..."<<std::endl;

      double trueScatteringSig= TMath::ACos(truep0sig.Unit().Dot(truep1sig.Unit()));          
      ROOT::Math::XYZVector truets1v((truex11sig.X() - truex00sig.X()), (truex11sig.Y() - truex00sig.Y()), (truex11sig.Z() - truex00sig.Z()));
      ROOT::Math::XYZVector truets2v((truex32sig.X() - truex20sig.X()), (truex32sig.Y() - truex20sig.Y()), (truex32sig.Z() - truex20sig.Z()));
      double trueScatteringSig2 = TMath::ACos(truets1v.Unit().Dot(truets2v.Unit()));

      double recoScatATan2t = std::atan2(truets1v.Cross(truets2v).R(),truets1v.Dot(truets2v));

      ROOT::Math::XYZVector truesp0((truex00sig.X()+truex01sig.X())/2.,(truex00sig.Y()+truex01sig.Y())/2.,(truex00sig.Z()+truex01sig.Z())/2.);
      ROOT::Math::XYZVector truesp1((truex10sig.X()+truex11sig.X())/2.,(truex10sig.Y()+truex11sig.Y())/2.,(truex10sig.Z()+truex11sig.Z())/2.);
      ROOT::Math::XYZVector truesp2((truex20sig.X()+truex21sig.X()+truex22sig.X())/3.,(truex20sig.Y()+truex21sig.Y()+truex22sig.Y())/3.,(truex20sig.Z()+truex21sig.Z()+truex22sig.Z())/3.);
      ROOT::Math::XYZVector truesp3((truex30sig.X()+truex31sig.X()+truex32sig.X())/3.,(truex30sig.Y()+truex31sig.Y()+truex32sig.Y())/3.,(truex30sig.Z()+truex31sig.Z()+truex32sig.Z())/3.);
      ROOT::Math::XYZVector truets1valt((truesp1.X() - truesp0.X()), (truesp1.Y() - truesp0.Y()), (truesp1.Z() - truesp0.Z()));
      ROOT::Math::XYZVector truets2valt((truesp3.X() - truesp2.X()), (truesp3.Y() - truesp2.Y()), (truesp3.Z() - truesp2.Z()));
      double trueScatteringSig3 = TMath::ACos(truets1valt.Unit().Dot(truets2valt.Unit()));

      ROOT::Math::XYZVector truep1avg((truep00sig.X()+truep0sig.X())/2.,(truep00sig.Y()+truep0sig.Y())/2.,(truep00sig.Z()+truep0sig.Z())/2.);
      ROOT::Math::XYZVector truep2avg((truep1sig.X()+truep2sig.X())/2.,(truep1sig.Y()+truep2sig.Y())/2.,(truep1sig.Z()+truep2sig.Z())/2.);

      double trueScatteringSig4 = TMath::ACos(truep1avg.Unit().Dot(truep2avg.Unit()));

        ROOT::Math::XYZVector u1t = truets1v.Unit();
        ROOT::Math::XYZVector u2t = truets2v.Unit();
        double thetaXZt = std::atan2(u2t.X(), u2t.Z()) - std::atan2(u1t.X(), u1t.Z());
        // wrap angle into (−π,π]
        double thetaXZ2t = thetaXZt = std::atan2(std::sin(thetaXZt), std::cos(thetaXZt));
        
        double thetaYZt = std::atan2(u2t.Y(), u2t.Z()) - std::atan2(u1t.Y(), u1t.Z());
        double thetaYZ2t = std::atan2(std::sin(thetaYZt), std::cos(thetaYZt));

     // Get position info at endpoints
     double x00, y00, x11, y11, x20, y20, x32, y32;
     for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
        caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
        if (h.station == 0 && h.plane == 0 && h.pid == 2212) {
	  x00 = h.pos.X(); 
	  y00 = h.pos.Y(); }
        if (h.station == 1 && h.plane == 1 && h.pid == 2212) {
          x11 = h.pos.X();
          y11 = h.pos.Y(); }
        if (h.station == 2 && h.plane == 0 && h.pid == 2212) {
          x20 = h.pos.X();
          y20 = h.pos.Y(); }
        if (h.station == 3 && h.plane == 2 && h.pid == 2212) {
          x32 = h.pos.X();
          y32 = h.pos.Y(); }
      }

      hNSigTruth->Fill(trueScatteringSig);
      if (IsSel(rec->vtxs.vtx)){
        hMomentumRes->Fill(momres);
        hNSigSelTruth->Fill(trueScatteringSig);

        hP2Theta2Truth->Fill(trueScatteringSig*trueScatteringSig*120.*120.);
      }

      else{
    hClustersNotSel->Fill(nclusters);
    if (!clustMap.empty()){
      for (auto i : clustMap){
        int p = 0;
        if (i.first.first == 0 && i.first.second == 0) p=1;
        if (i.first.first == 0 && i.first.second == 1) p=2;
        if (i.first.first == 1 && i.first.second == 0) p=3;
        if (i.first.first == 1 && i.first.second == 1) p=4;
        if (i.first.first == 2 && i.first.second == 0) p=5;
        if (i.first.first == 2 && i.first.second == 1) p=6;
        if (i.first.first == 2 && i.first.second == 2) p=7;
        if (i.first.first == 3 && i.first.second == 0) p=8;
        if (i.first.first == 3 && i.first.second == 1) p=9;
        if (i.first.first == 3 && i.first.second == 2) p=10;
        if (i.first.first == 4 && i.first.second == 0) p=11;
        if (i.first.first == 4 && i.first.second == 1) p=12;
        if (i.first.first == 5 && i.first.second == 0) p=13;
        if (i.first.first == 5 && i.first.second == 1) p=14;
        if (i.first.first == 5 && i.first.second == 2) p=15;
        if (i.first.first == 6 && i.first.second == 0) p=16;
        if (i.first.first == 6 && i.first.second == 1) p=17;
        if (i.first.first == 6 && i.first.second == 2) p=18;
        if (i.first.first == 7 && i.first.second == 0) p=19;
        if (i.first.first == 7 && i.first.second == 1) p=20;

        hClustersVsPlaneNotSel->Fill(p,i.second);

      }
    }
    else {
      for (int i=1; i<20; i++) hClustersVsPlaneNotSel->Fill(i,0);
    }
    }
/*
      else{
        std::cout<<"Looking at cluster distribution..."<<std::endl;
        if (!clustMap.empty()){
          for (auto i : clustMap){
            std::cout<<"...cluster at ("<<i.first.first<<","<<i.first.second<<"): "<<i.second<<std::endl;
            int st = i.first.first;
            int pl = i.first.second;
            if (i.second > 1){
              for(int i=0; i<nclusters; i++){
                caf::SRSSDClust& c = rec->cluster.clust[i];

                if (c.station == st && c.plane == pl){
                  std::cout<<"...minstrip = "<<c.minstrip<<" and maxstrip = "<<c.maxstrip<<std::endl;
                }
              }
            }
          }
          for (auto i : clustMap){
            if (i.second > 1){
              if (i.first.first > 4){
                dontcare++;
                break;
              }
            }
          }
	}
      }
*/
      if (IsSel(rec->vtxs.vtx)){
        nsigsel++;

        // Tracks
        caf::SRTrack& trk0 = rec->vtxs.vtx[0].beamtrk;
        caf::SRTrack& trk1 = rec->vtxs.vtx[0].sectrk[0];
        caf::SRBeamTrack &beamtrk = rec->vtxs.vtx[0].beamtrk;
        caf::SRSecondaryTrack &sectrk = rec->vtxs.vtx[0].sectrk[0];

        // Track segments
        caf::SRTrackSegment& ts1 = trk0.sgmnt[0];
        caf::SRTrackSegment& ts2 = trk1.sgmnt[0];
        caf::SRTrackSegment& ts3 = trk1.sgmnt[1];

        // Space points
        int nspacepoints = int(rec->spcpts.sp.size());
        std::vector<double> spx, spy, spz;
        for (size_t i=0; i<nspacepoints; i++){
          caf::SRSpacePoint& s = rec->spcpts.sp[i];
          spx.push_back(s.x[0]);
          spy.push_back(s.x[1]);
          spz.push_back(s.x[2]);
        }

        // Scattering angle
        ROOT::Math::XYZVector p0 = rec->vtxs.vtx[0].beamtrk.mom;
        ROOT::Math::XYZVector p1 = rec->vtxs.vtx[0].sectrk[0].mom;
        ROOT::Math::XYZVector p2 = rec->vtxs.vtx[0].sectrk[0].sgmnt[1].mom;
        double recoBend = TMath::ACos(p2.Unit().Dot(p1.Unit())); //p1.Dot(p2)/(p1.Mag()*p2.Mag()));
        hRecoBending->Fill(recoBend);

        //double recoScattering = TMath::ACos(sectrk.momTrgt.Unit().Dot(beamtrk.momTrgt.Unit()));
	ROOT::Math::XYZVector ts1v((ts1.pointB.X() - ts1.pointA.X()), (ts1.pointB.Y() - ts1.pointA.Y()), (ts1.pointB.Z() - ts1.pointA.Z()));
        ROOT::Math::XYZVector ts2v((ts2.pointB.X() - ts2.pointA.X()), (ts2.pointB.Y() - ts2.pointA.Y()), (ts2.pointB.Z() - ts2.pointA.Z()));	

        double acos = ts2v.Unit().Dot(ts1v.Unit());
        acos = TMath::Min(TMath::Max(acos, -1.), 1.);
        double recoScatteringSigSel = TMath::ACos(acos);

        double recoScatATan2 = std::atan2(ts1v.Cross(ts2v).R(), ts1v.Dot(ts2v));

        hRecoScattering->Fill(recoScatteringSigSel);
        hP2Theta2->Fill(recoScatteringSigSel*recoScatteringSigSel*120.*120.);
	//std::cout<<"p2theta2 = "<<recoScatteringSigSel*recoScatteringSigSel*120.*120.<<std::endl;

	// Signed scattering angle in XZ and YZ planes
        ROOT::Math::XYZVector u1 = ts1v.Unit();
        ROOT::Math::XYZVector u2 = ts2v.Unit();
        double thetaXZ = std::atan2(u2.X(), u2.Z()) - std::atan2(u1.X(), u1.Z());
        // wrap angle into (−π,π]
        double thetaXZ2 = thetaXZ = std::atan2(std::sin(thetaXZ), std::cos(thetaXZ));

        double thetaYZ = std::atan2(u2.Y(), u2.Z()) - std::atan2(u1.Y(), u1.Z());
        double thetaYZ2 = std::atan2(std::sin(thetaYZ), std::cos(thetaYZ));

	//std::cout<<"Thetax = "<<thetaXZ2<<" and Thetay = "<<thetaYZ2<<std::endl;

	hRecoScatteringXZ->Fill(thetaXZ2);
        hRecoScatteringYZ->Fill(thetaYZ2);

	hScatResXZ->Fill(thetaXZ2t - thetaXZ2);
        hScatResYZ->Fill(thetaYZ2t - thetaYZ2);
	hScatResYZvsXZ->Fill(thetaXZ2t - thetaXZ2,thetaYZ2t - thetaYZ2);

        // Extrapolate beam track upstream i.e. track segment 1
        double t = (-100. - trk0.sgmnt[0].pointA.Z())/(trk0.sgmnt[0].pointB.Z() - trk0.sgmnt[0].pointA.Z()) ;

        double x = trk0.sgmnt[0].pointA.X() + t*(trk0.sgmnt[0].pointB.X() - trk0.sgmnt[0].pointA.X());
        double y = trk0.sgmnt[0].pointA.Y() + t*(trk0.sgmnt[0].pointB.Y() - trk0.sgmnt[0].pointA.Y());

        hBeamProfileXY->Fill(x,y);
        hVertexX->Fill(trk1.posTrgt.X());
        hVertexY->Fill(trk1.posTrgt.Y());
        hVertexZ->Fill(trk1.posTrgt.Z());

        double trk0_dxdz = (trk0.sgmnt[0].pointB.X() - trk0.sgmnt[0].pointA.X()) / (trk0.sgmnt[0].pointB.Z() - trk0.sgmnt[0].pointA.Z());
        double trk0_dydz = (trk0.sgmnt[0].pointB.Y() - trk0.sgmnt[0].pointA.Y()) / (trk0.sgmnt[0].pointB.Z() - trk0.sgmnt[0].pointA.Z());
        hTrk0dXdZ->Fill(trk0_dxdz);
        hTrk0dYdZ->Fill(trk0_dydz);

        double trk1_dxdz = (trk1.sgmnt[0].pointB.X() - trk1.sgmnt[0].pointA.X()) / (trk1.sgmnt[0].pointB.Z() - trk1.sgmnt[0].pointA.Z());
        double trk1_dydz = (trk1.sgmnt[0].pointB.Y() - trk1.sgmnt[0].pointA.Y()) / (trk1.sgmnt[0].pointB.Z() - trk1.sgmnt[0].pointA.Z());
        hTrk1dXdZ->Fill(trk1_dxdz);
        hTrk1dYdZ->Fill(trk1_dydz);

        hTrk1dXdZvsX->Fill(trk1.posTrgt.X(),trk1_dxdz);
        hTrk1dYdZvsY->Fill(trk1.posTrgt.Y(),trk1_dydz);

        double angleDiff = recoScatteringSigSel - trueScatteringSig;
        hScatteringAngleResVsTrueAngle->Fill(trueScatteringSig,angleDiff);
        tScatteringAngleResVsTrueAngle->Fill(trueScatteringSig,angleDiff);

        hScatteringAngleResVsTrueAngleZoom->Fill(trueScatteringSig,angleDiff);

	double scattres = trueScatteringSig - recoScatteringSigSel;
	hScatteringAngleResolution->Fill(scattres);

	double scattres2 = trueScatteringSig2 - recoScatteringSigSel;
        hScatteringAngleResolution2->Fill(scattres2);

        hScatteringAngleResolutionATan2->Fill(recoScatATan2t - recoScatATan2);

        double angleDiff2 = recoScatteringSigSel - trueScatteringSig2;
        hScatteringAngleRes2VsTrueAngleZoom->Fill(trueScatteringSig,angleDiff2);

        double scattres3 = trueScatteringSig3 - recoScatteringSigSel;
        hScatteringAngleResolution3->Fill(scattres3);

        double scattres4 = trueScatteringSig4 - recoScatteringSigSel;
        hScatteringAngleResolution4->Fill(scattres4);

        hScatteringAngleMigrationMatrix->Fill(trueScatteringSig,recoScatteringSigSel);

	// With space points
	//ROOT::Math::XYZVector sp00 = rec->trk0.sgmnt[0]...

	// Compare position
	hResX00->Fill(x00 - trk0.sgmnt[0].pointA.X());
	hResY00->Fill(y00 - trk0.sgmnt[0].pointA.Y());
	hResX11->Fill(x11 - trk0.sgmnt[0].pointB.X());
        hResY11->Fill(y11 - trk0.sgmnt[0].pointB.Y());
        hResX20->Fill(x20 - trk1.sgmnt[0].pointA.X());
        hResY20->Fill(y20 - trk1.sgmnt[0].pointA.Y());
        hResX32->Fill(x32 - trk1.sgmnt[0].pointB.X());
        hResY32->Fill(y32 - trk1.sgmnt[0].pointB.Y());


	// Bending Angle Resolution
        double trueBendingSig= TMath::ACos(truep2sig.Unit().Dot(truep3sig.Unit()));
	double angleDiffB = recoBend - trueBendingSig;	
	hBendAngleRes->Fill(angleDiffB);
	
      } // IsSel
    } // IsSig
    if (IsSel(rec->vtxs.vtx)){
      // Track
      caf::SRTrack& trk0 = rec->vtxs.vtx[0].beamtrk;
      caf::SRTrack& trk1 = rec->vtxs.vtx[0].sectrk[0];

      // Track segments
      caf::SRTrackSegment& ts1 = trk0.sgmnt[0];
      caf::SRTrackSegment& ts2 = trk1.sgmnt[0];
      caf::SRTrackSegment& ts3 = trk1.sgmnt[1];

      ROOT::Math::XYZVector ts1v((ts1.pointB.X() - ts1.pointA.X()), (ts1.pointB.Y() - ts1.pointA.Y()), (ts1.pointB.Z() - ts1.pointA.Z()));
      ROOT::Math::XYZVector ts2v((ts2.pointB.X() - ts2.pointA.X()), (ts2.pointB.Y() - ts2.pointA.Y()), (ts2.pointB.Z() - ts2.pointA.Z()));

      double acos = ts2v.Unit().Dot(ts1v.Unit());
      acos = TMath::Min(TMath::Max(acos, -1.), 1.);
      double recoScatteringSel = TMath::ACos(acos);

      hNSelReco->Fill(recoScatteringSel);

      if (IsSig(rec->truth)){
        hNSelSigReco->Fill(recoScatteringSel);
      } // IsSig
    } // IsSel
    int nts1=0; int nts2=0; int nts3=0;
    int nts2sp2=0; int nts2sp3=0;
    int nts3sp2=0; int nts3sp3=0;
    bool makets2 = false;
    bool makets3 = false;

    for (auto t : rec->sgmnts.seg){
      if (t.region == rb::Region::kRegion1) nts1++;
      if (t.region == rb::Region::kRegion2) nts2++;
      if (t.region == rb::Region::kRegion3) nts3++;
    }

    for (auto t : rec->sgmnts.seg){
      if (t.region == rb::Region::kRegion2){
        if (t.nspacepoints == 2) nts2sp2++;
        if (t.nspacepoints == 3) nts2sp3++;
      }
      if (t.region == rb::Region::kRegion3){
        if (t.nspacepoints == 2) nts3sp2++;
        if (t.nspacepoints == 3) nts3sp3++;
      }
    }
    if (nts1 == 1 &&
       ((nts2 == 1 || nts2 == 4) && nts2sp2 != 4)) makets2 = true;
    if (makets2 && (nts3 == 1 || nts3 == 4) && nts3sp2 != 4) makets3 = true;

    std::vector<caf::SRTrackSegment> tsvcut;
    if (nts1 == 1){
      for (auto t : rec->sgmnts.seg){
        if (t.region == rb::Region::kRegion1){
          tsvcut.push_back(t);
        }
      }
    }
    if (makets2){
      for (auto t : rec->sgmnts.seg){
        if (t.region == rb::Region::kRegion2){
          bool shortTrackSeg = true;
          if (nts2 == 1) tsvcut.push_back(t); // Only one combination
          else{
            if (t.pointB.Z() > 700) shortTrackSeg = false;
            if (shortTrackSeg) tsvcut.push_back(t);
          }
        }
      }
    }
    if (makets3){
      for (auto t : rec->sgmnts.seg){
        if (t.region == rb::Region::kRegion3){
          if (nts3 == 1) tsvcut.push_back(t); // Only one combination
          else{
            if (t.nspacepoints == 3) tsvcut.push_back(t);
          }
        }
      }
    }
    for (auto t : tsvcut){
      if (t.region == rb::Region::kRegion1){
        double tz = (-10. - t.pointA.Z()) / (t.pointB.Z() - t.pointA.Z());
        double xe = t.pointA.X() + tz*(t.pointB.X() - t.pointA.X());
        double ye = t.pointA.Y() + tz*(t.pointB.Y() - t.pointA.Y());
        hRecoTS1XY->Fill(xe,ye);
      }
      // For 2, extrapolate to (4,1) roughly at 849
      // For 3, extrapolate to (5,0) roughly at 1142
      if (t.region == rb::Region::kRegion2){
        double tz = (849. - t.pointA.Z()) / (t.pointB.Z() - t.pointA.Z());
        double xe = t.pointA.X() + tz*(t.pointB.X() - t.pointA.X());
        double ye = t.pointA.Y() + tz*(t.pointB.Y() - t.pointA.Y());
        hRecoTS2XY->Fill(xe,ye);
      }
      if (t.region == rb::Region::kRegion3){
        double tz = (1142 - t.pointA.Z()) / (t.pointB.Z() - t.pointA.Z());
        double xe = t.pointA.X() + tz*(t.pointB.X() - t.pointA.X());
        double ye = t.pointA.Y() + tz*(t.pointB.Y() - t.pointA.Y());
        hRecoTS3XY->Fill(xe,ye);
      }
    }
  } // Entries

  TFile* caf_out = new TFile("ana.root","RECREATE");

  // Efficiency
  TH1F *hEff = (TH1F*)hNSigSelTruth->Clone("hEff");
  hEff->Divide(hNSigTruth);
  hEff->SetTitle("Efficiency; Scattering Angle (rad); hNSigSelTruth / hNSigTruth");
  hEff->Write();

  // Purity
  TH1F *hPur = (TH1F*)hNSelSigReco->Clone("hPur");
  hPur->Divide(hNSelReco);
  hPur->SetTitle("Purity; Scattering Angle (rad); hNSelSigReco /hNSelReco ");
  hPur->Write();

  hNSigTruth->Write();
  hNSigSelTruth->Write();
  hNSelReco->Write();
  hNSelSigReco->Write();

  // Cross section calculation
  double N_A = 6.02214076 * TMath::Power(10,23); // mol^-1
  double rho = 1.83; // g/cm^3
  double M = 12.0107; // g/mol
  double target_L = 2.; // cm
  double numberdensity = N_A*rho/M*target_L; // cm^-2
  double cm2tobarns = TMath::Power(10,-27); // cm^2/mb
  double numberdensitymb = numberdensity * cm2tobarns; //mb^-1
  //double binwidth = 0.0025;
  hP2Theta2->Sumw2();

  for (int i=0; i<hP2Theta2->GetNbinsX(); i++){
    double N_i = hP2Theta2->GetBinContent(i);
    double binwidth = hP2Theta2->GetBinWidth(i);
    hCrossSection->SetBinContent(i,N_i/pot/numberdensitymb/binwidth);
  }
  hCrossSection->SetTitle("Differential Cross Section; p_{beam}^{2}theta^{2} [(GeV/c)^{2}]; d#sigma/d(p_{beam}^{2}#theta^{2}) [mb/(GeV)^{2}]");
  hCrossSection->Write();

  hP2Theta2Truth->Sumw2();

  for (int i=0; i<hP2Theta2Truth->GetNbinsX(); i++){
    double N_i = hP2Theta2Truth->GetBinContent(i);
    double binwidth = hP2Theta2Truth->GetBinWidth(i);
    hCrossSectionTruth->SetBinContent(i,N_i/pot/numberdensitymb/binwidth);
  }
  hCrossSectionTruth->SetTitle("Differential Cross Section; p_{beam}^{2}theta^{2} [(GeV/c)^{2}]; d#sigma/d(p_{beam}^{2}#theta^{2}) [mb/(GeV)^{2}]");
  hCrossSectionTruth->Write();

  hRecoScattering->Sumw2();
  for (int i=0; i<hRecoScattering->GetNbinsX(); i++){
    double N_i = hRecoScattering->GetBinContent(i);
    double binwidth = hRecoScattering->GetBinWidth(i);
    hCrossSectionScat->SetBinContent(i,N_i/pot/numberdensitymb/binwidth);
  }
  hCrossSectionScat->SetTitle("Differential Cross Section; Scattering Angle [rad]; d#sigma/d#theta [mb]");
  hCrossSectionScat->Write();

  hClustersVsPlane->SetTitle("Clusters Vs Plane; Plane; # Clusters");
  hClustersVsPlane->Write();

  hClusters->SetTitle("Clusters Distribution; # Clusters; # Events");
  hClusters->Write();

  hClustersNotSel->SetTitle("If not IsSel: Clusters Distribution; # Clusters; # Events");
  hClustersNotSel->Write();

  hClustersVsPlaneNotSel->SetTitle("If not IsSel: Clusters Vs Plane; Plane; # Clusters");
  hClustersVsPlaneNotSel->Write();

  hRecoScattering->SetTitle("Scattering Angle Distribution; Scattering Angle [rad]; # Events");
  hRecoScattering->Write();

  hRecoScatteringXZ->SetTitle("Scattering Angle XZ Distribution; Scattering Angle [rad]; # Events");
  hRecoScatteringXZ->Write();

  hRecoScatteringYZ->SetTitle("Scattering Angle YZ Distribution; Scattering Angle [rad]; # Events");
  hRecoScatteringYZ->Write();

  hScatResXZ->SetTitle("Scattering Angle XZ Resolution; Scattering Angle [rad]; # Events");
  hScatResXZ->Write();

  hScatResYZ->SetTitle("Scattering Angle YZ Resolution; Scattering Angle [rad]; # Events");
  hScatResYZ->Write();

  hScatResYZvsXZ->SetTitle("Scattering Angle YZ Resolution vs Scattering Angle XZ Resolution; Scattering Angle [rad]; Scattering Angle [rad]");
  hScatResYZvsXZ->Write();

  hRecoBending->SetTitle("Bending Angle Distribution; Bending Angle [rad]; # Events");
  hRecoBending->Write();

  hBendAngleRes->SetTitle("Bending Angle Resolution; Angle [rad]; # Events");
  hBendAngleRes->Write();

  hBeamProfileXY->SetTitle("Beam Profile @ z = - 100; Position (X) [mm]; Position (Y) [mm]");
  hBeamProfileXY->Write();

  hVertexX->SetTitle("Reco Track Vertex X-Position; Position (X) [mm]; # Events");
  hVertexX->Write();

  hVertexY->SetTitle("Reco Track Vertex Y-Position; Position (Y) [mm]; # Events");
  hVertexY->Write();

  hVertexZ->SetTitle("Reco Track Vertex Z-Position; Position (Z) [mm]; # Events");
  hVertexZ->Write();

  hP2Theta2->Write();
  hP2Theta2Truth->Write();

  hTrk0dXdZ->SetTitle("Beam Track Slope (dx/dz); dx/dz; # Events");
  hTrk0dXdZ->Write();

  hTrk0dYdZ->SetTitle("Beam Track Slope (dy/dz); dy/dz; # Events");
  hTrk0dYdZ->Write();

  hTrk1dXdZ->SetTitle("Reco Track Slope (dx/dz); dx/dz; # Events");
  hTrk1dXdZ->Write();

  hTrk1dYdZ->SetTitle("Reco Track Slope (dy/dz); dy/dz; # Events");
  hTrk1dYdZ->Write();

  hTrk1dXdZvsX->SetTitle("Reco Track Slope (dx/dz) vs Vertex X-Position; Position (X) [mm]; dx/dz");
  hTrk1dXdZvsX->Write();

  hTrk1dYdZvsY->SetTitle("Reco Track Slope (dy/dz) vs Vertex Y-Position; Position (Y) [mm]; dy/dz");
  hTrk1dYdZvsY->Write();

  hScatteringAngleResVsTrueAngle->Write();
  tScatteringAngleResVsTrueAngle->Write();
  hScatteringAngleResVsTrueAngleZoom->Write();
  hScatteringAngleRes2VsTrueAngleZoom->Write();

  hScatteringAngleResolution->Write();

  hScatteringAngleResolution2->SetTitle("Scattering Angle Resolution; #theta_{true} - #theta_{reco}; Number of Events");
  hScatteringAngleResolution2->Write();
  //hScatteringAngleResolution3->Write();
  //hScatteringAngleResolution4->Write();
  hScatteringAngleResolutionATan2->Write();

  hScatteringAngleMigrationMatrix->SetTitle("Scattering Angle Migration Matrix; True (rad); Reco (rad)");
  hScatteringAngleMigrationMatrix->Write();

  for (int i=1; i<hScatteringAngleMigrationMatrix->GetNbinsX(); i++){
    double diag = hScatteringAngleMigrationMatrix->GetBinContent(i, i);
    std::cout<<"diag = "<<diag<<std::endl;
    for (int j=1; j<hScatteringAngleMigrationMatrix->GetNbinsY(); j++){
      double content = hScatteringAngleMigrationMatrix->GetBinContent(i,j);
      //std::cout<<"bin content = "<<hScatteringAngleMigrationMatrix->GetBinContent(i,j)<<std::endl;
      if (content != 0) hMigrationMatrixRelative->SetBinContent(i,j,content/diag);
    }
  }
  hMigrationMatrixRelative->Write();

  hRecoTS1XY->Write();
  hRecoTS2XY->Write();
  hRecoTS3XY->Write();

  hResX00->Write();
  hResY00->Write();
  hResX11->Write();
  hResY11->Write();
  hResX20->Write();
  hResY20->Write();
  hResX32->Write();
  hResY32->Write();

  hMomentumRes->Write();

  hRecoBeamAnglePxPz->Write();
  hRecoBeamAnglePyPz->Write();

  std::cout<<"Nsig = "<<nsig<<std::endl;
  std::cout<<"Nsigsel = "<<nsigsel<<std::endl;

  std::cout<<"Process = "<<process<<std::endl;

  //std::cout<<"Two or more clusters but after the target = "<<dontcare<<std::endl;

  TCanvas *mc1 = new TCanvas("mc1","My canvas", 600, 400);
  TF1 *fDG = new TF1("fDG", DoubleGaus, -0.0015, 0.000, 6);

double max = hScatteringAngleResolution2->GetMaximum();
double mean = hScatteringAngleResolution2->GetMean();
double rms = hScatteringAngleResolution2->GetRMS();

  fDG->SetParameters(
    max,
    //hScatteringAngleResolution2->GetMaximum(), // A1
    mean,                // mean
    rms/2.0,                // sigma1
    0.5*max,
    mean,
    //0.2*hScatteringAngleResolution2->GetMaximum(), // A2
    rms                 // sigma2
  );

//fDG->SetRange(mean - 3*rms, mean + 3*rms);
  hScatteringAngleResolution2->Fit("fDG","R");

double chi2 = fDG->GetChisquare();
int ndf = fDG->GetNDF();
double chi2_ndf = chi2 / ndf;
  std::cout<<"chi2, ndf = "<<chi2<<","<<ndf<<std::endl;
  std::cout<<"chi2/ndf = "<<chi2_ndf<<std::endl;
  fDG->SetLineColor(kRed);
  hScatteringAngleResolution2->Draw();
  fDG->Draw("same");
  mc1->SaveAs("fit.png");

  caf_out->Close();
}
bool IsSigSimple(std::vector<caf::SRSimpleTruth> beamtruth, std::vector<caf::SRSimpleTruth> sectruth)
{
  std::vector<caf::SRSimpleTruth> one_particle11;
  std::vector<caf::SRSimpleTruth> one_particle20;
  std::vector<caf::SRSimpleTruth> allbeamparticles;
  std::vector<caf::SRSimpleTruth> vbeamparticles;

  int ssd11 = 0; int ssd20 = 0;
  int beamparticles = 0;
  bool good = false;

  for (size_t j=0; j<beamtruth.size(); ++j) {
    caf::SRSimpleTruth& h = beamtruth[j];
    allbeamparticles.push_back(h);
  }
  for (size_t j=0; j<sectruth.size(); ++j) {
    caf::SRSimpleTruth& h = sectruth[j];
    allbeamparticles.push_back(h);
  }

  for (size_t j=0; j<allbeamparticles.size(); ++j) {
    caf::SRSimpleTruth& h = allbeamparticles[j];

    if (h.station <= 1){
      beamparticles++;
      vbeamparticles.push_back(h);
    }
    if (h.station == 1 && h.plane == 1){
      if (h.pdgCode == 2212){
        one_particle11.push_back(h);
        ssd11++; }
    }
    if (h.station == 2 && h.plane == 0){
      if (h.pdgCode == 2212){
        one_particle20.push_back(h);
        ssd20++; }
    }
  }

  if (one_particle11.size() > 1){
    for (size_t j=0; j+1<one_particle11.size(); ++j) {
      if (std::abs(one_particle11[j].strip - one_particle11[j+1].strip) <= 1) ssd11--;
    }
  }
  if (one_particle20.size() > 1){
    for (size_t j=0; j+1<one_particle20.size(); ++j) {
      if (std::abs(one_particle20[j].strip - one_particle20[j+1].strip) <= 1) ssd20--;
    }
  }
  if (vbeamparticles.size() > 1){
    for (size_t j=0; j+1<vbeamparticles.size(); ++j) {
      if (std::abs(vbeamparticles[j].strip - vbeamparticles[j+1].strip) <= 1) beamparticles--;
    }
  }

  if (ssd11 == 1 && ssd20 == 1 && beamparticles == 4) good = true;
  //if (good) std::cout<<"This is Signal"<<std::endl;
  //else std::cout<<"This is NotSignal"<<std::endl;
  return good;

}
bool IsSig(caf::SRTruth& truth)
{
  std::vector<caf::SRTrueSSDHits> one_particle11;
  std::vector<caf::SRTrueSSDHits> one_particle20;
  std::vector<caf::SRTrueSSDHits> vbeamparticles;

  int ssd11 = 0; int ssd20 = 0;
  int beamparticles = 0;
  bool good = false;

  for (size_t j=0; j<truth.truehits.truehits.size(); ++j) {
    caf::SRTrueSSDHits& h = truth.truehits.truehits[j];

    if (h.station <= 1 && h.pid == 2212){
      beamparticles++;
      vbeamparticles.push_back(h);
    }
   if (h.station == 1 && h.plane == 1){
      if (h.pid == 2212){
        one_particle11.push_back(h);
        ssd11++; }
      //else if (h.pid == 11) std::cout<<"Electron with DE = "<<h.dE<<std::endl;
      //else std::cout<<"Other pdg = "<<h.pid<<std::endl;
    }
    if (h.station == 2 && h.plane == 0){
      if (h.pid == 2212){
        one_particle20.push_back(h);
        ssd20++; }
      //else if (h.pid == 11) std::cout<<"Electron with DE = "<<h.dE<<std::endl;
      //else std::cout<<"Other pdg = "<<h.pid<<std::endl;
    }
  }

  if (one_particle11.size() > 1){
    for (size_t j=0; j+1<one_particle11.size(); ++j) {
      if (std::abs(one_particle11[j].strip - one_particle11[j+1].strip) <= 1) ssd11--;
    }
  }
  if (one_particle20.size() > 1){
    for (size_t j=0; j+1<one_particle20.size(); ++j) {
      if (std::abs(one_particle20[j].strip - one_particle20[j+1].strip) <= 1) ssd20--;
    }
  }
  if (vbeamparticles.size() > 1){
    for (size_t j=0; j+1<vbeamparticles.size(); ++j) {
      if (std::abs(vbeamparticles[j].strip - vbeamparticles[j+1].strip) <= 1) beamparticles--;
    }
  }

  //std::cout<<"ssd11 = "<<ssd11<<" and ssd20 = "<<ssd20<<std::endl;

  if (ssd11 == 1 && ssd20 == 1 && beamparticles == 4) good = true;
  //if (good) std::cout<<"This is Signal"<<std::endl;
  //else std::cout<<"This is NotSignal"<<std::endl;
  return good;

}
bool IsSel(std::vector<caf::SRVertex> vtx)
{
  bool sigsel = false;
  if (vtx.size() == 1 && vtx[0].nsectrk == 1) sigsel = true;
  return sigsel;
}
Double_t DoubleGaus(Double_t *x, Double_t *par)
{
//par[0] = A1
//par[1] = mean
//par[2] = sigma1
//par[3] = A2
//par[4] = sigma2
    return par[0]*TMath::Gaus(x[0], par[1], par[2], false)
         + par[3]*TMath::Gaus(x[0], par[4], par[5], false);
}
