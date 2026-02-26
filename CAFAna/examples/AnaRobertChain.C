#include "StandardRecord/StandardRecord.h"
#include "StandardRecord/SRVector3D.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TMultiGraph.h"

void AnaRobertChain(std::string fname)
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

  caf::StandardRecord* rec = 0;
  chain_evts->SetBranchAddress("rec", &rec);
  int nentries  = chain_evts->GetEntries();

  double recoAngle;
  double recoAngleNew;
  double recoBend;
  TH1F* hClusters = new TH1F("hClusters","hClusters",50,0,50);
  TH2F* hClustersVsPlane = new TH2F("hClustersVsPlane","hClustersVsPlane",20,0,20,50,0,50);
  TH1F* hTrueAngle = new TH1F("hTrueAngle","hTrueAngle",180,0.,0.018);
  TH1F* hScattering = new TH1F("hScattering","hScattering",180,0.,0.018);
  TH1F* hScatteringNew = new TH1F("hScatteringNew","hScatteringNew",180,0.,0.018);
  TH1F* hScatteringInTarget = new TH1F("hScatteringInTarget","hScatteringInTarget",180,0.,0.018);
  TH1F* hP2Theta2 = new TH1F("hP2Theta2","hP2Theta2",60,0.,0.15);
  TH1F* hCrossSection = new TH1F("hCrossSection","hCrossSection",60,0.,0.15);
  TH1F* hBending = new TH1F("hBending","hBending",500,0.,0.05);
  TH2F* hBeamProfileXY = new TH2F("hBeamProfileXY","hBeamProfileXY",100,-50.,50.,100,-50.,50.);
  //TH1F* hTargetZ = new TH1F("hTargetZ","hTargetZ",200,0.,1000.);
  TH1F* hVertexX = new TH1F("hVertexX","hVertexX",100,-50.,50.);
  TH1F* hVertexY = new TH1F("hVertexY","hVertexY",100,-50.,50.);
  TH1F* hVertexZ = new TH1F("hVertexZ","hVertexZ",200,0.,2000.);
  TH1F* hTrk0dXdZ = new TH1F("hTrk0dXdZ","hTrk0dXdZ",100,-0.005,0.005);
  TH1F* hTrk0dYdZ = new TH1F("hTrk0dYdZ","hTrk0dYdZ",100,0.005,0.015);
  TH1F* hTrk1dXdZ = new TH1F("hTrk1dXdZ","hTrk1dXdZ",100,-0.005,0.005);
  TH1F* hTrk1dYdZ = new TH1F("hTrk1dYdZ","hTrk1dYdZ",100,0.0,0.01);
  TH2F* hTrk1dXdZvsX = new TH2F("hTrk1dXdZvsX","hTrk1dXdZvsX",100,-50.,50.,100,-0.005,0.005);
  TH2F* hTrk1dYdZvsY = new TH2F("hTrk1dYdZvsY","hTrk1dYdZvsY",100,-50.,50.,100,0.,0.01);
  TH1F* hPxPz = new TH1F("hPxPz","hPxPz",100,-0.01,0.01);
  TH1F* hPyPz = new TH1F("hPyPz","hPyPz",100,-0.01,0.01);
  TH1F* hPzPmag = new TH1F("hPzPmag","hPzPmag",500,0.75,1.25);
  TH1F* hDxDzReco = new TH1F("hDxDzReco","hDxDzReco",100,-0.01,0.01);
  TH1F* hDyDzReco = new TH1F("hDyDzReco","hDyDzReco",100,-0.01,0.01);
  TH1F* hPzPmagReco = new TH1F("hPzPmagReco","hPzPmagReco",500,0.75,1.25);
  TH1F* hPxBeam = new TH1F("hPxBeam","hPxBeam",100,-0.25,0.25);
  TH1F* hPyBeam = new TH1F("hPyBeam","hPyBeam",100,0.2,0.7);
  TH1F* hPzBeam = new TH1F("hPzBeam","hPzBeam",100,119,120);

  TH1F* hPxReco = new TH1F("hPxReco","hPxReco",100,-1.,9.);
  TH1F* hPyReco = new TH1F("hPyReco","hPyReco",100,-1.,9.);
  TH1F* hPzReco = new TH1F("hPzReco","hPzReco",100,0.,1000.);

  TH1F* hChi2TS1 = new TH1F("hChi2TS1","hChi2TS1",100,0,100);
  TH1F* hChi2TS2 = new TH1F("hChi2TS2","hChi2TS2",100,0,100);
  TH1F* hChi2TS3 = new TH1F("hChi2TS3","hChi2TS3",100,0,100);

  TH2F* hScatteringAngleResVsTrueAngle = new TH2F("hScatteringAngleResVsTrueAngle","hScatteringAngleResVsTrueAngle",180,0.,0.018,100,-0.005,0.005);
  TProfile* tScatteringAngleResVsTrueAngle = new TProfile("tScatteringAngleResVsTrueAngle","tScatteringAngleResVsTrueAngle",180,0.,0.018,-0.005,0.005);
  TH1F* hScatteringAngleRecoMinusTrue = new TH1F("hScatteringAngleRecoMinusTrue","hScatteringAngleRecoMinusTrue",100,-0.005,0.005);
  TH2F* hScatteringAngleResVsTrueAngleTS = new TH2F("hScatteringAngleResVsTrueAngleTS","hScatteringAngleResVsTrueAngleTS",180,0.,0.018,100,-0.005,0.005);

  TH2F* hacosXY = new TH2F("hacosXY","hacosXY",100,-50.,50.,100,-50.,50.);
  TH2F* hacosdXdZvsdYdZ = new TH2F("hacosdXdZvsdYdZ","hacosdXdZvsdYdZ",100,-0.005,0.005,100,0.0025,0.0125);

  TH1F* hatan2scat = new TH1F("hatan2scat","hatan2scat",180,0.,0.018);
  TH1F* hCombo = new TH1F("hCombo","hCombo",180,0.,0.018);
  TH1F* hatan2scatprecise = new TH1F("hatan2scatprecise","hatan2scatprecise",180,0.,0.018);

  TH2F* hP0DiagonalXY = new TH2F("hP0DiagonalXY","hP0DiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hP0DiagonalSlope = new TH2F("hP0DiagonalSlope","hP0DiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);
  TH2F* hP0NonDiagonalXY = new TH2F("hP0NonDiagonalXY","hP0NonDiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hP0NonDiagonalSlope = new TH2F("hP0NonDiagonalSlope","hP0NonDiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);

  TH2F* hP1DiagonalXY = new TH2F("hP1DiagonalXY","hP1DiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hP1DiagonalSlope = new TH2F("hP1DiagonalSlope","hP1DiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);
  TH2F* hP1NonDiagonalXY = new TH2F("hP1NonDiagonalXY","hP1NonDiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hP1NonDiagonalSlope = new TH2F("hP1NonDiagonalSlope","hP1NonDiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);

  TH2F* hRecoP0DiagonalXY = new TH2F("hRecoP0DiagonalXY","hRecoP0DiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoP0DiagonalSlope = new TH2F("hRecoP0DiagonalSlope","hRecoP0DiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);
  TH2F* hRecoP0NonDiagonalXY = new TH2F("hRecoP0NonDiagonalXY","hRecoP0NonDiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoP0NonDiagonalSlope = new TH2F("hRecoP0NonDiagonalSlope","hRecoP0NonDiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);

  TH2F* hRecoP1DiagonalXY = new TH2F("hRecoP1DiagonalXY","hRecoP1DiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoP1DiagonalSlope = new TH2F("hRecoP1DiagonalSlope","hRecoP1DiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);
  TH2F* hRecoP1NonDiagonalXY = new TH2F("hRecoP1NonDiagonalXY","hRecoP1NonDiagonalXY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoP1NonDiagonalSlope = new TH2F("hRecoP1NonDiagonalSlope","hRecoP1NonDiagonalSlope",100,-0.05,0.05,100,-0.05,0.05);  

  TH2F* hATan2AngleResVsTrueAngle = new TH2F("hATan2AngleResVsTrueAngle","hATan2AngleResVsTrueAngle",180,0.,0.018,100,-0.005,0.005);

  std::vector<TGraph*> gLineSegXZ;

  std::map<std::pair<int, int>, int> clustMap;
  std::vector<std::vector<double>> lsvpos;
  std::vector<double> lsx;
  std::vector<double> lsy;
  std::vector<double> lsz;

  int nplots = 0;
  int acc=0;
  int ac1=0;
  int acall=0;
  int diag=0;
  bool drawFlag = false;
  int drawCount = 0;

  std::vector<TMultiGraph*> mAllXZ;
  std::vector<TMultiGraph*> mAllYZ;

  std::vector<TMultiGraph*> mTrackSegXZ;
  std::vector<TMultiGraph*> mTrackSegYZ;

  double trueAngle, angleDiff;
  double trueAngleTS, angleDiffTS;
  caf::SRVector3D truep0(0,0,0);
  caf::SRVector3D truep1(0,0,0);
  caf::SRVector3D truepos0(0,0,0);
  caf::SRVector3D truepos1(0,0,0);
  caf::SRVector3D truepos10(0,0,0);
  caf::SRVector3D truepos21(0,0,0);

  std::vector<double> truehitx, truehity, truehitz;

  std::cout<<"Entries = "<< chain_evts->GetEntries()<<std::endl;

  int pot = 0;

  for(int i=0;i<nentries;i++){
    chain_evts->GetEntry(i);
    std::string digitStr = std::to_string(rec->hdr.evt);
    if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
      continue;
    }
    pot++;
  }
  std::cout<<"POT = "<<pot<<std::endl;

  for(int i=0;i<nentries;i++){
    clustMap.clear();
    lsvpos.clear();
    lsx.clear();
    lsy.clear();
    lsz.clear();

    chain_evts->GetEntry(i);

    // Select 70% of data entries not already used for alignment
    std::string digitStr = std::to_string(rec->hdr.evt);

    // Spit out Run number for the first entry only
    if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;

    if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
      continue;
    }
    int nclusters = int(rec->cluster.clust.size());
    hClusters->Fill(nclusters);

    for(int i=0; i<nclusters; i++){
      caf::SRSSDClust& c = rec->cluster.clust[i];
      ++clustMap[std::pair<int,int>(c.station,c.plane)];
    }

    char *mNameXZ = new char[12];
    sprintf(mNameXZ,"mEvent_%d_XZ",i);
    TMultiGraph* mLSXZ = new TMultiGraph(mNameXZ,mNameXZ);

    char *mNameYZ = new char[12];
    sprintf(mNameYZ,"mEvent_%d_YZ",i);
    TMultiGraph* mLSYZ = new TMultiGraph(mNameYZ,mNameYZ);

    char *mNameTSXZ = new char[15];
    sprintf(mNameTSXZ,"mTrackSeg_%d_XZ",i);
    TMultiGraph* mTSXZ = new TMultiGraph(mNameTSXZ,mNameTSXZ);

    char *mNameTSYZ = new char[15];
    sprintf(mNameTSYZ,"mTrackSeg_%d_YZ",i);
    TMultiGraph* mTSYZ = new TMultiGraph(mNameTSYZ,mNameTSYZ);    
    
    bool ocpp = false; // One Cluster Per Plane
    int totbeam=0; 
    bool obp = false; // One Beam Particle

    if (!clustMap.empty()){
    for (auto i : clustMap){
      int p = 0;

      //std::cout<<"clust map (s,p) = "<<i.first.first<<", "<<i.first.second<<std::endl; 
      if (i.first.first == 0 && i.first.second == 0){ p=1; totbeam++; }
      if (i.first.first == 0 && i.first.second == 1){ p=2; totbeam++; }
      if (i.first.first == 1 && i.first.second == 0){ p=3; totbeam++; }
      if (i.first.first == 1 && i.first.second == 1){ p=4; totbeam++; }
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

      if (i.second != 1) ocpp = false; 
      else ocpp = true; 
    }
    }
    else {
      for (int i=1; i<20; i++) hClustersVsPlane->Fill(i,0);
    }
    if (totbeam == 4) obp = true;

    //std::cout<<"Track size = "<<rec->trks.trk.size()<<" and ocpp = "<<ocpp<<" and obp = "<<obp<<std::endl;
    if (ocpp && obp){
      if (rec->trks.trk.size() == 2){

        // Draw line segments
        int nlineseg = int(rec->lineseg.lineseg.size());
        for(int i=0; i<nlineseg; i++){
          caf::SRLineSegment& ls = rec->lineseg.lineseg[i];

          double x0x = ls.x0.x;
          double x0y = ls.x0.y;
          double x0z = ls.x0.z;

          double x1x = ls.x1.x;
          double x1y = ls.x1.y;
          double x1z = ls.x1.z;

          double avgx = (x0x + x1x)/2.;
          double avgy = (x0y + x1y)/2.;
          double avgz = (x0z + x1z)/2.;

          std::vector<double> glsx = {x0x,x1x};
          std::vector<double> glsy = {x0y,x1y};
          std::vector<double> glsz = {x0z,x1z};

          if (nplots<10){
            TGraph* glsxz = new TGraph(2,glsz.data(),glsx.data());
            glsxz->SetName(Form("mEvent_%i",i));
            glsxz->SetMarkerStyle(8);
            glsxz->SetMarkerSize(0.75);
            glsxz->SetLineColor(1);
            glsxz->SetLineWidth(2);
            mLSXZ->Add(glsxz);

            TGraph* glsyz = new TGraph(2,glsz.data(),glsy.data());
            glsyz->SetName(Form("mEvent_%i",i));
            glsyz->SetMarkerStyle(8);
            glsyz->SetMarkerSize(0.75);
            glsyz->SetLineColor(1);
            glsyz->SetLineWidth(2);
            mLSYZ->Add(glsyz);
         }

         std::vector avg = {avgx,avgy,avgz};
         lsvpos.push_back(avg);
         lsx.push_back(avgx);
         lsy.push_back(avgy);
         lsz.push_back(avgz);

       }

       // True hits
       if (rec->truth.truehits.truehits.size() > 0){
	 truehitx.clear();
	 truehity.clear();
	 truehitz.clear();
         for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
           caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
           truehitx.push_back(h.GetX);
           truehity.push_back(h.GetY);
           truehitz.push_back(h.GetZ);
         }
       }
 
       // Tracks 
       caf::SRTrack& trk0 = rec->trks.trk[0];
       caf::SRTrack& trk1 = rec->trks.trk[1];

       // Track segments
       caf::SRTrackSegment& ts1 = trk0._sgmnt[0];
       caf::SRTrackSegment& ts2 = trk1._sgmnt[0];
       caf::SRTrackSegment& ts3 = trk1._sgmnt[1];

       // Space points
       int nspacepoints = int(rec->spcpts.sp.size());
       std::vector<double> spx, spy, spz;
       for (size_t i=0; i<nspacepoints; i++){
         caf::SRSpacePoint& s = rec->spcpts.sp[i];
	 spx.push_back(s.x[0]);
         spy.push_back(s.x[1]);
         spz.push_back(s.x[2]);
       }

       double ts1x0x = ts1.pointA[0];
       double ts1x0y = ts1.pointA[1];
       double ts1x0z = ts1.pointA[2];

       double ts1x1x = ts1.pointB[0];
       double ts1x1y = ts1.pointB[1];
       double ts1x1z = ts1.pointB[2];

       std::vector<double> gts1x = {ts1x0x,ts1x1x};
       std::vector<double> gts1y = {ts1x0y,ts1x1y};
       std::vector<double> gts1z = {ts1x0z,ts1x1z};

       double ts2x0x = ts2.pointA[0];
       double ts2x0y = ts2.pointA[1];
       double ts2x0z = ts2.pointA[2];

       double ts2x1x = ts2.pointB[0];
       double ts2x1y = ts2.pointB[1];
       double ts2x1z = ts2.pointB[2];

       std::vector<double> gts2x = {ts2x0x,ts2x1x};
       std::vector<double> gts2y = {ts2x0y,ts2x1y};
       std::vector<double> gts2z = {ts2x0z,ts2x1z};

       double ts3x0x = ts3.pointA[0];
       double ts3x0y = ts3.pointA[1];
       double ts3x0z = ts3.pointA[2];

       double ts3x1x = ts3.pointB[0];
       double ts3x1y = ts3.pointB[1];
       double ts3x1z = ts3.pointB[2];

       std::vector<double> gts3x = {ts3x0x,ts3x1x};
       std::vector<double> gts3y = {ts3x0y,ts3x1y};
       std::vector<double> gts3z = {ts3x0z,ts3x1z};       

/*
       if (nplots<10){
	    // For each track segment...
            TGraph* gts1xz = new TGraph(2,gts1z.data(),gts1x.data());
            gts1xz->SetName(Form("mTrackSegsXZ_%i",i));
            gts1xz->SetMarkerStyle(8);
            gts1xz->SetMarkerSize(0.75);
            gts1xz->SetMarkerColor(kRed);
            gts1xz->SetLineColor(kRed);
            gts1xz->SetLineWidth(2);
            mTSXZ->Add(gts1xz);
	    mLSXZ->Add(gts1xz);

            TGraph* gts2xz = new TGraph(2,gts2z.data(),gts2x.data());
            gts2xz->SetName(Form("mTrackSegsXZ_%i",i));
            gts2xz->SetMarkerStyle(8);
            gts2xz->SetMarkerSize(0.75);
            gts2xz->SetMarkerColor(kBlue);
            gts2xz->SetLineColor(kBlue);
            gts2xz->SetLineWidth(2);
            mTSXZ->Add(gts2xz);
	    mLSXZ->Add(gts2xz);

            TGraph* gts3xz = new TGraph(2,gts3z.data(),gts3x.data());
            gts3xz->SetName(Form("mTrackSegsXZ_%i",i));
            gts3xz->SetMarkerStyle(8);
            gts3xz->SetMarkerSize(0.75);
            gts3xz->SetMarkerColor(kGreen);
            gts3xz->SetLineColor(kGreen);
            gts3xz->SetLineWidth(2);
            mTSXZ->Add(gts3xz);
            mLSXZ->Add(gts3xz);

	    TGraph* gspxz = new TGraph(nspacepoints,spz.data(),spx.data());
	    gspxz->SetMarkerColor(kViolet);
            gspxz->SetMarkerStyle(8);
            gspxz->SetMarkerSize(0.75);
            gspxz->SetLineWidth(0);
	    mLSXZ->Add(gspxz);

            int ntruth = rec->truth.truehits.truehits.size();
            std::cout<<"ntruth = "<<ntruth<<" and truehitx = "<<truehitx.size()<<std::endl;

            if (rec->truth.truehits.truehits.size() > 0){
              TGraph* gthxz = new TGraph(ntruth,truehitz.data(),truehitx.data());
              gthxz->SetMarkerColor(kCyan);
              gthxz->SetMarkerStyle(8);
              gthxz->SetMarkerSize(0.75);
              gthxz->SetLineWidth(0);
              mLSXZ->Add(gthxz);
	    }

            TGraph* gts1yz = new TGraph(2,gts1z.data(),gts1y.data());
            gts1yz->SetName(Form("mTrackSegsYZ_%i",i));
            gts1yz->SetMarkerStyle(8);
            gts1yz->SetMarkerSize(0.75);
            gts1yz->SetMarkerColor(kRed);
            gts1yz->SetLineColor(kRed);
            gts1yz->SetLineWidth(2);
            mTSYZ->Add(gts1yz);
            mLSYZ->Add(gts1yz);

            TGraph* gts2yz = new TGraph(2,gts2z.data(),gts2y.data());
            gts2yz->SetName(Form("mTrackSegsYZ_%i",i));
            gts2yz->SetMarkerStyle(8);
            gts2yz->SetMarkerSize(0.75);
            gts2yz->SetMarkerColor(kBlue);
            gts2yz->SetLineColor(kBlue);
            gts2yz->SetLineWidth(2);
            mTSYZ->Add(gts2yz);
            mLSYZ->Add(gts2yz);

            TGraph* gts3yz = new TGraph(2,gts3z.data(),gts3y.data());
            gts3yz->SetName(Form("mTrackSegsYZ_%i",i));
            gts3yz->SetMarkerStyle(8);
            gts3yz->SetMarkerSize(0.75);
            gts3yz->SetMarkerColor(kGreen);
            gts3yz->SetLineColor(kGreen);
            gts3yz->SetLineWidth(2);
            mTSYZ->Add(gts3yz);
            mLSYZ->Add(gts3yz);

         mAllXZ.push_back(mLSXZ);
         mAllYZ.push_back(mLSYZ);
      
	    mTrackSegXZ.push_back(mTSXZ);
            mTrackSegYZ.push_back(mTSYZ);
       }

       nplots++;
*/
        // Chi2
        hChi2TS1->Fill(ts1.chi2);
        hChi2TS2->Fill(ts2.chi2);
        hChi2TS3->Fill(ts3.chi2);

        // Scattering angle
        caf::SRVector3D& p0 = rec->trks.trk[0].mom;
        caf::SRVector3D& p1 = rec->trks.trk[1].mom;
	recoAngle = TMath::ACos(p0.Dot(p1)/(p0.Mag()*p1.Mag()));
	hScattering->Fill(recoAngle);     

        caf::SRVector3D& p0n = rec->trks.trk[0]._sgmnt[0].mom;
        caf::SRVector3D& p1n = rec->trks.trk[1]._sgmnt[0].mom;         
        caf::SRVector3D& p2 = rec->trks.trk[1]._sgmnt[1].mom;
        recoBend = TMath::ACos(p1.Dot(p2)/(p1.Mag()*p2.Mag()));
        hBending->Fill(recoBend);

        // Track segment 1 and 2 intersection

        double dx = ts1.pointB[0]-ts1.pointA[0];
        double dy = ts1.pointB[1]-ts1.pointA[1];
        double dz = ts1.pointB[2]-ts1.pointA[2];

        double pxpz = dx/dz;
        double pypz = dy/dz;
        double pzpmag = 1./sqrt(1. + (dx*dx)/(dz*dz) + (dy*dy)/(dz*dz));
        hPxPz->Fill(pxpz);
        hPyPz->Fill(pypz);
        hPzPmag->Fill(pzpmag);

        caf::SRVector3D ts1v((ts1.pointB[0] - ts1.pointA[0]), (ts1.pointB[1] - ts1.pointA[1]), (ts1.pointB[2] - ts1.pointA[2]));
        caf::SRVector3D ts2v((ts2.pointB[0] - ts2.pointA[0]), (ts2.pointB[1] - ts2.pointA[1]), (ts2.pointB[2] - ts2.pointA[2]));

        double dxr = ts2.pointB[0]-ts2.pointA[0];
        double dyr = ts2.pointB[1]-ts2.pointA[1];
        double dzr = ts2.pointB[2]-ts2.pointA[2];
        double pxpzr = dxr/dzr;
        double pypzr = dyr/dzr;
        double pzpmagr = 1./sqrt(1. + (dxr*dxr)/(dzr*dzr) + (dyr*dyr)/(dzr*dzr));
        hDxDzReco->Fill(pxpzr);
        hDyDzReco->Fill(pypzr);
        hPzPmagReco->Fill(pzpmagr);

        double acos = ts1v.Dot(ts2v)/(ts1v.Mag()*ts2v.Mag());

	double acosdot = ts1v.X()*ts2v.X() + ts1v.Y()*ts2v.Y() + ts1v.Z()*ts2v.Z();
	double cx = ts1v.Y()*ts2v.Z() - ts1v.Z()*ts2v.Y();
	double cy = -(ts1v.X()*ts2v.Z() - ts1v.Z()*ts2v.X());
	double cz = ts1v.X()*ts2v.Y() - ts1v.Y()*ts2v.X();
	double acos1mag = sqrt(ts1v.X()*ts1v.X() + ts1v.Y()*ts1v.Y() + ts1v.Z()*ts1v.Z());
        double acos2mag = sqrt(ts2v.X()*ts2v.X() + ts2v.Y()*ts2v.Y() + ts2v.Z()*ts2v.Z());

	double cr = (ts1v.Cross(ts2v)).Mag();
	double dt = ts1v.Dot(ts2v);
        double atan2 = TMath::ATan2(cr,dt);

        hatan2scat->Fill(atan2); 
        
        if (acos > 1.){
          hCombo->Fill(atan2);
          //std::cout<<"Trouble: scattering angle from atan2 = "<<atan2<<std::endl;
        }
	else hCombo->Fill(TMath::ACos(acos));

	double cr2 = (ts2v.Mag()*ts1v - ts1v.Mag()*ts2v).Mag();
	double dt2 = (ts2v.Mag()*ts1v + ts1v.Mag()*ts2v).Mag();
	double atan2precise = 2.*TMath::ATan2(cr2,dt2);
	hatan2scatprecise->Fill(atan2precise);

	bool acosflag = false;

        if (acos > 1.){
          std::cout << std::fixed << std::setprecision(15) << acos << std::endl;
          acc++;
          hacosXY->Fill(ts1.pointA[0],ts1.pointA[1]);
          hacosdXdZvsdYdZ->Fill(pxpzr,pypzr);
          acosflag = true;
        }
        else if (acos == 1.) ac1++;
        else acall++;

        acos = TMath::Min(TMath::Max(acos, -1.), 1.);

        recoAngleNew = TMath::ACos(acos);
        hScatteringNew->Fill(recoAngleNew);

	// Cross section calculation
	double N_A = 6.02214076 * TMath::Power(10,23); // mol^-1
        double rho = 1.83; // g/cm^3
        double M = 12.0107; // g/mol
	double target_L = 2.; // cm
        double numberdensity = N_A*rho/M*target_L; // cm^-2
        double cm2tobarns = TMath::Power(10,-27); // cm^2/mb
        double numberdensitymb = numberdensity * cm2tobarns; //mb^-1
	double binwidth = 0.0025; 

        hP2Theta2->Fill(recoAngleNew*recoAngleNew*120.*120.);

        // Extrapolate beam track upstream i.e. track segment 1
        double t = (-100. - trk0._sgmnt[0].pointA[2])/(trk0._sgmnt[0].pointB[2] - trk0._sgmnt[0].pointA[2]) ;
        double x = trk0._sgmnt[0].pointA[0] + t*(trk0._sgmnt[0].pointB[0] - trk0._sgmnt[0].pointA[0]);
        double y = trk0._sgmnt[0].pointA[1] + t*(trk0._sgmnt[0].pointB[1] - trk0._sgmnt[0].pointA[1]);
        double p[3];
        double pbeam = 120.;
        p[2] = trk0.mom.z;
        p[0] = trk0.mom.x;
        p[1] = trk0.mom.y;

        hPzBeam->Fill(p[2]);
        hPxBeam->Fill(p[0]);
        hPyBeam->Fill(p[1]);

        double pr[3];
	pr[2] = trk1.mom.z;
        pr[0] = trk1.mom.x;
        pr[1] = trk1.mom.y;
        hPzReco->Fill(pr[2]);
        hPxReco->Fill(pr[0]);
        hPyReco->Fill(pr[1]);

        hBeamProfileXY->Fill(x,y);

        hVertexX->Fill(trk1.vtx[0]);
        hVertexY->Fill(trk1.vtx[1]);
        hVertexZ->Fill(trk1.vtx[2]);

	if ((TMath::Abs(trk1.vtx[0])) <= 100. && (TMath::Abs(trk1.vtx[1])) <= 50.
	   && trk1.vtx[2] >= 380.5 && trk1.vtx[2] <= 400.5) hScatteringInTarget->Fill(recoAngleNew);
  
        double trk0_dxdz = (trk0._sgmnt[0].pointB[0] - trk0._sgmnt[0].pointA[0]) / (trk0._sgmnt[0].pointB[2] - trk0._sgmnt[0].pointA[2]);
        double trk0_dydz = (trk0._sgmnt[0].pointB[1] - trk0._sgmnt[0].pointA[1]) / (trk0._sgmnt[0].pointB[2] - trk0._sgmnt[0].pointA[2]);      
        hTrk0dXdZ->Fill(trk0_dxdz);
        hTrk0dYdZ->Fill(trk0_dydz);

        double trk1_dxdz = (trk1._sgmnt[0].pointB[0] - trk1._sgmnt[0].pointA[0]) / (trk1._sgmnt[0].pointB[2] - trk1._sgmnt[0].pointA[2]);
        double trk1_dydz = (trk1._sgmnt[0].pointB[1] - trk1._sgmnt[0].pointA[1]) / (trk1._sgmnt[0].pointB[2] - trk1._sgmnt[0].pointA[2]);
        hTrk1dXdZ->Fill(trk1_dxdz);
        hTrk1dYdZ->Fill(trk1_dydz);

	hTrk1dXdZvsX->Fill(trk1.vtx[0],trk1_dxdz);
        hTrk1dYdZvsY->Fill(trk1.vtx[1],trk1_dydz);

	double de0,de1; int pid0,pid1;

	// If there is truth info
	int ssd11=0; int ssd20=0;
        if (rec->truth.truehits.truehits.size() > 0){
          int nGot=0;
	  for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
            caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	    if (h.GetStation == 1 && h.GetPlane == 1) ssd11++;
	    if (h.GetStation == 2 && h.GetPlane == 0) ssd20++;
	  }
//	  std::cout<<"....."<<std::endl;
	  //std::cout<<"Beam: "<<rec->truth.beam.daughters.size()<<std::endl;
	  if (ssd11 != 1 || ssd20 != 1) {
	     std::cout<<"Nope: "<<ssd11+ssd20<<std::endl; 
	     //std::cout<<"Beam: "<<rec->truth.beam.daughters.size()<<std::endl;
	     //std::cout<<"nhits = "<<rec->truth.truehits.truehits.size()<<std::endl;

	     //for (int i=0; i<rec->truth.beam.daughters.size(); i++){
	      //std::cout<<rec->truth.beam.daughters[i].PdgCode()<<std::endl;}
	     continue;}
	 //} 

          for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
            caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];

	    if (h.GetStation == 0 && h.GetPlane == 0) {
              truepos10.SetXYZ(h.GetX,h.GetY,h.GetZ);
	    }
            if (h.GetStation == 1 && h.GetPlane == 1) {
              truep0.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	      truepos0.SetXYZ(h.GetX,h.GetY,h.GetZ);
	      de0 = h.GetDE;
	      pid0 = h.GetPId;
              ++nGot; }
            if (h.GetStation == 2 && h.GetPlane == 0) {
              truep1.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	      truepos1.SetXYZ(h.GetX,h.GetY,h.GetZ);
              de1 = h.GetDE;
              pid1 = h.GetPId;
              ++nGot; }
            if (h.GetStation == 3 && h.GetPlane == 2) {
              truepos21.SetXYZ(h.GetX,h.GetY,h.GetZ);
            }
            if (nGot == 2) break;
          }
	  //std::cout<<"nGot = "<<nGot<<std::endl;
	  //std::cout<<"....."<<std::endl;

          if (nGot == 2) {
	    caf::SRVector3D ts1true((truepos0.X() - truepos10.X()), (truepos0.Y() - truepos10.Y()), (truepos0.Z() - truepos10.Z()));
            caf::SRVector3D ts2true((truepos21.X() - truepos1.X()), (truepos21.Y() - truepos1.Y()), (truepos21.Z() - truepos1.Z()));
            double acoststrue = ts1true.Dot(ts2true)/(ts1true.Mag()*ts2true.Mag());
            trueAngleTS = TMath::ACos(acoststrue);		
	    angleDiffTS = recoAngleNew - trueAngleTS; 

	    double truedx = truep1.X() - truep0.X();
            double truedy = truep1.Y() - truep0.Y();
            double truedz = truep1.Z() - truep0.Z();
	  
	    double acostruepos = truepos0.Dot(truepos1)/(truepos0.Mag()*truepos1.Mag()); 
	    double trueAngleMaybe = TMath::ACos(acostruepos);

            double acostruedot = truep0.X()*truep1.X() + truep0.Y()*truep1.Y() + truep0.Z()*truep1.Z();
            double acostrue1mag = sqrt(truep0.X()*truep0.X() + truep0.Y()*truep0.Y() + truep0.Z()*truep0.Z());
            double acostrue2mag = sqrt(truep1.X()*truep1.X() + truep1.Y()*truep1.Y() + truep1.Z()*truep1.Z());

            double acostrue = truep0.Dot(truep1)/(truep0.Mag()*truep1.Mag());	
	    acostrue = TMath::Min(TMath::Max(acostrue, -1.), 1.);
	    trueAngle = TMath::ACos(acostrue);
            angleDiff = recoAngleNew - trueAngle;
	    //angleDiff = recoAngle - trueAngle;
	    hTrueAngle->Fill(trueAngle);
	    hScatteringAngleResVsTrueAngle->Fill(trueAngle,angleDiff);
	    tScatteringAngleResVsTrueAngle->Fill(trueAngle,angleDiff);
	    hScatteringAngleRecoMinusTrue->Fill(angleDiff);
            hScatteringAngleResVsTrueAngleTS->Fill(trueAngleTS,angleDiffTS);

            double tcr = (truep0.Cross(truep1)).Mag();
            double tdt = truep0.Dot(truep1);
            double tatan2 = TMath::ATan2(tcr,tdt);

	    double angleDiffATan2 = atan2 - tatan2;
	    hATan2AngleResVsTrueAngle->Fill(tatan2,angleDiffATan2);

	    if (angleDiff < -0.0001 ){
	      std::cout<<"Intersection at "<<trk1.vtx[2]<<std::endl;

              drawFlag = true;
              if (recoAngleNew > trueAngle/10.) diag++;
	      std::cout<<std::fixed << std::setprecision(15)<<"Reco angle is "<<recoAngleNew<<" and truth angle is "<<trueAngle<<" so angleDiff is "<<angleDiff<<std::endl;
	      //std::cout<<"nclusters for reco = "<<nclusters<<std::endl;

   	      hP0DiagonalXY->Fill(truepos0.X(),truepos0.Y());
	      hP0DiagonalSlope->Fill(truep0.X()/truep0.Z(),truep0.Y()/truep0.Z());
	      hP1DiagonalXY->Fill(truepos1.X(),truepos1.Y());
              hP1DiagonalSlope->Fill(truep1.X()/truep1.Z(),truep1.Y()/truep1.Z());

	      hRecoP0DiagonalXY->Fill(ts1.pointB[0],ts1.pointB[1]);
              hRecoP0DiagonalSlope->Fill(pxpz,pypz);
              hRecoP1DiagonalXY->Fill(ts2.pointA[0],ts2.pointA[1]);
              hRecoP1DiagonalSlope->Fill(pxpzr,pypzr);

              std::cout<<"........."<<std::endl;
              if (recoAngleNew == 0){
	        std::cout<<"Acos flag is "<<acosflag<<std::endl;
                std::cout<<"........."<<std::endl;
              }
	    }
	    else {
              //std::cout<<"Not Diag:  DE0 = "<<de0<<" and DE1 = "<<de1<<std::endl;
              //std::cout<<"Not Diag:  PID0 = "<<pid0<<" and PID1 = "<<pid1<<std::endl;
              //std::cout<<"Not diag: reco track 1 xy = ("<<ts1.pointB[0]<<", "<<ts1.pointB[1]<<") and reco track 2 xy = ("<<ts2.pointA[0]<<", "<<ts2.pointA[1]<<")"<<std::endl;
	      //std::cout<<"Not diag: dx = "<<ts2.pointA[0]-ts1.pointB[0]<<" and dy = "<<ts2.pointA[1]-ts1.pointB[1]<<" and dz = "<<ts2.pointA[2]-ts1.pointB[2]<<std::endl;
              //std::cout<<"Non Diag: ts1 chi2 = "<<ts1.chi2<<" and ts2 chi2 = "<<ts2.chi2<<std::endl;

	      hP0NonDiagonalXY->Fill(truepos0.X(),truepos0.Y());
	      hP0NonDiagonalSlope->Fill(truep0.X()/truep0.Z(),truep0.Y()/truep0.Z());
              hP1NonDiagonalXY->Fill(truepos1.X(),truepos1.Y());
              hP1NonDiagonalSlope->Fill(truep1.X()/truep1.Z(),truep1.Y()/truep1.Z());

              hRecoP0NonDiagonalXY->Fill(ts1.pointB[0],ts1.pointB[1]);
              hRecoP0NonDiagonalSlope->Fill(pxpz,pypz);
              hRecoP1NonDiagonalXY->Fill(ts2.pointA[0],ts2.pointA[1]);
              hRecoP1NonDiagonalSlope->Fill(pxpzr,pypzr);
	    }
  	  }

          for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
            caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
          }
        }
	else{
	  hRecoP0DiagonalXY->Fill(ts1.pointB[0],ts1.pointB[1]);
          hRecoP0DiagonalSlope->Fill(pxpz,pypz);
          hRecoP1DiagonalXY->Fill(ts2.pointA[0],ts2.pointA[1]);
          hRecoP1DiagonalSlope->Fill(pxpzr,pypzr);

          hRecoP0NonDiagonalXY->Fill(ts1.pointB[0],ts1.pointB[1]);
          hRecoP0NonDiagonalSlope->Fill(pxpz,pypz);
          hRecoP1NonDiagonalXY->Fill(ts2.pointA[0],ts2.pointA[1]);
          hRecoP1NonDiagonalSlope->Fill(pxpzr,pypzr);
	}

        //Plot SSD cluster positions (using line segments)
	
        //if (i<100){
	if (i<300 && i>200){
        //if (drawFlag && drawCount < 10){
          TGraph* gLSXZ = new TGraph(lsvpos.size(),lsz.data(),lsx.data()); 
	  gLSXZ->SetName(Form("Event_%i",i));
	  gLSXZ->SetMarkerStyle(3);
          gLSXZ->SetMarkerSize(2);
	  gLSXZ->SetLineColor(0);
          gLSXZ->GetYaxis()->SetRangeUser(-50,50);
          gLineSegXZ.push_back(gLSXZ);
	//  drawCount++;
        }

    if (drawFlag && drawCount < 10){
            TGraph* gts1xz = new TGraph(2,gts1z.data(),gts1x.data());
            gts1xz->SetName(Form("mTrackSegsXZ_%i",i));
            gts1xz->SetMarkerStyle(8);
            gts1xz->SetMarkerSize(0.75);
            gts1xz->SetMarkerColor(kRed);
            gts1xz->SetLineColor(kRed);
            gts1xz->SetLineWidth(2);
            mTSXZ->Add(gts1xz);
            mLSXZ->Add(gts1xz);

            TGraph* gts2xz = new TGraph(2,gts2z.data(),gts2x.data());
            gts2xz->SetName(Form("mTrackSegsXZ_%i",i));
            gts2xz->SetMarkerStyle(8);
            gts2xz->SetMarkerSize(0.75);
            gts2xz->SetMarkerColor(kBlue);
            gts2xz->SetLineColor(kBlue);
            gts2xz->SetLineWidth(2);
            mTSXZ->Add(gts2xz);
            mLSXZ->Add(gts2xz);

            TGraph* gts3xz = new TGraph(2,gts3z.data(),gts3x.data());
            gts3xz->SetName(Form("mTrackSegsXZ_%i",i));
            gts3xz->SetMarkerStyle(8);
            gts3xz->SetMarkerSize(0.75);
            gts3xz->SetMarkerColor(kGreen);
            gts3xz->SetLineColor(kGreen);
            gts3xz->SetLineWidth(2);
            mTSXZ->Add(gts3xz);
            mLSXZ->Add(gts3xz);

            TGraph* gspxz = new TGraph(nspacepoints,spz.data(),spx.data());
            gspxz->SetMarkerColor(kViolet);
            gspxz->SetMarkerStyle(8);
            gspxz->SetMarkerSize(0.75);
            gspxz->SetLineWidth(0);
            mLSXZ->Add(gspxz);

            int ntruth = rec->truth.truehits.truehits.size();
            //std::cout<<"ntruth = "<<ntruth<<" and truehitx = "<<truehitx.size()<<std::endl;

            if (rec->truth.truehits.truehits.size() > 0){
              TGraph* gthxz = new TGraph(ntruth,truehitz.data(),truehitx.data());
              gthxz->SetMarkerColor(kCyan);
              gthxz->SetMarkerStyle(8);
              gthxz->SetMarkerSize(0.75);
              gthxz->SetLineWidth(0);
              mLSXZ->Add(gthxz);
            }

            TGraph* gts1yz = new TGraph(2,gts1z.data(),gts1y.data());
            gts1yz->SetName(Form("mTrackSegsYZ_%i",i));
            gts1yz->SetMarkerStyle(8);
            gts1yz->SetMarkerSize(0.75);
            gts1yz->SetMarkerColor(kRed);
            gts1yz->SetLineColor(kRed);
            gts1yz->SetLineWidth(2);
            mTSYZ->Add(gts1yz);
            mLSYZ->Add(gts1yz);

            TGraph* gts2yz = new TGraph(2,gts2z.data(),gts2y.data());
            gts2yz->SetName(Form("mTrackSegsYZ_%i",i));
            gts2yz->SetMarkerStyle(8);
            gts2yz->SetMarkerSize(0.75);
            gts2yz->SetMarkerColor(kBlue);
            gts2yz->SetLineColor(kBlue);
            gts2yz->SetLineWidth(2);
            mTSYZ->Add(gts2yz);
            mLSYZ->Add(gts2yz);

            TGraph* gts3yz = new TGraph(2,gts3z.data(),gts3y.data());
            gts3yz->SetName(Form("mTrackSegsYZ_%i",i));
            gts3yz->SetMarkerStyle(8);
            gts3yz->SetMarkerSize(0.75);
            gts3yz->SetMarkerColor(kGreen);
            gts3yz->SetLineColor(kGreen);
            gts3yz->SetLineWidth(2);
            mTSYZ->Add(gts3yz);
            mLSYZ->Add(gts3yz);

            if (rec->truth.truehits.truehits.size() > 0){
              TGraph* gthyz = new TGraph(ntruth,truehitz.data(),truehity.data());
              gthyz->SetMarkerColor(kCyan);
              gthyz->SetMarkerStyle(8);
              gthyz->SetMarkerSize(0.75);
              gthyz->SetLineWidth(0);
              mLSYZ->Add(gthyz);
            }

            mAllXZ.push_back(mLSXZ);
            mAllYZ.push_back(mLSYZ);

            mTrackSegXZ.push_back(mTSXZ);
            mTrackSegYZ.push_back(mTSYZ);
            drawCount++;
          }

      }
    }
  }  

  TFile* caf_out = new TFile("ana.root","RECREATE");
  //hScattering->SetTitle("Scattering Angle Distribution; Scattering Angle [rad]; # Events");
  //hScattering->Write();

  hTrueAngle->Write();

  hScatteringNew->SetTitle("Scattering Angle Distribution; Scattering Angle [rad]; # Events");
  hScatteringNew->Write();

  hP2Theta2->Write();

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

  hBending->SetTitle("Bending Angle Distribution; Bending Angle [rad]; # Events");
  hBending->Write();

  hBeamProfileXY->SetTitle("Beam Profile @ z = - 100; Position (X) [mm]; Position (Y) [mm]");
  hBeamProfileXY->Write();

  hVertexX->SetTitle("Reco Track Vertex X-Position; Position (X) [mm]; # Events");
  hVertexX->Write();

  hVertexY->SetTitle("Reco Track Vertex Y-Position; Position (Y) [mm]; # Events");
  hVertexY->Write();

  hVertexZ->SetTitle("Reco Track Vertex Z-Position; Position (Z) [mm]; # Events");
  hVertexZ->Write();

  hClustersVsPlane->SetTitle("Clusters Vs Plane; Plane; # Clusters");
  hClustersVsPlane->Write();

  hClustersVsPlane->SetTitle("Clusters Distribution; # Clusters; # Events");
  hClusters->Write();

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

  hPxPz->Write();
  hPyPz->Write();
  hPzPmag->Write();
  hDxDzReco->Write();
  hDyDzReco->Write();
  hPzPmagReco->Write();
  hPxBeam->Write();
  hPyBeam->Write();
  hPzBeam->Write();
  hPxReco->Write();
  hPyReco->Write();
  hPzReco->Write();

  hChi2TS1->Write();
  hChi2TS2->Write();
  hChi2TS3->Write();

  hScatteringAngleResVsTrueAngle->Write();
  tScatteringAngleResVsTrueAngle->Write();
  hScatteringAngleRecoMinusTrue->Write();
  hScatteringAngleResVsTrueAngleTS->Write();

  hacosXY->Write();
  hacosdXdZvsdYdZ->Write();

  hatan2scat->Write();
  hCombo->Write();
  hatan2scatprecise->Write();

  hP0DiagonalXY->Write();
  hP0DiagonalSlope->Write();
  hP0NonDiagonalXY->Write();
  hP0NonDiagonalSlope->Write();

  hP1DiagonalXY->Write();
  hP1DiagonalSlope->Write();
  hP1NonDiagonalXY->Write();
  hP1NonDiagonalSlope->Write();

  hRecoP0DiagonalXY->Write();
  hRecoP0DiagonalSlope->Write();
  hRecoP0NonDiagonalXY->Write();
  hRecoP0NonDiagonalSlope->Write();

  hRecoP1DiagonalXY->Write();
  hRecoP1DiagonalSlope->Write();
  hRecoP1NonDiagonalXY->Write();
  hRecoP1NonDiagonalSlope->Write();

  hATan2AngleResVsTrueAngle->Write();

  std::cout<<"Number of plots = "<<gLineSegXZ.size()<<std::endl;
  std::cout<<"Number of >1s for acos (nan in scattering angle) = "<<acc<<std::endl;
  std::cout<<"Number of 1s for acos = "<<ac1<<std::endl;
  std::cout<<"The rest for acos = "<<acall<<std::endl;
  std::cout<<"Total entries = "<<acc+ac1+acall<<std::endl;
  std::cout<<"Diag = "<<diag<<std::endl;
  for (TGraph* graph : gLineSegXZ) {
    graph->Write();
  }
  int cnxz=0;
  int cnyz=0;

  for (TMultiGraph* mg : mAllXZ) {
    mg->SetTitle("Line Segment Position X vs Z");
    mg->GetXaxis()->SetTitle("z [mm]");
    mg->GetYaxis()->SetTitle("x [mm]");
    mg->SetMinimum(-150.);
    mg->SetMaximum(150.);
    mg->Write();
    //mg->Print(Form("mg%d.png",cn));
    cnxz++;
  }
  for (TMultiGraph* mg : mAllYZ) {
    //mg->SetMinimum(-150.);
    //mg->SetMaximum(150.);
    mg->SetTitle("Line Segment Position Y vs Z");
    mg->GetXaxis()->SetTitle("z [mm]");
    mg->GetYaxis()->SetTitle("y [mm]");
    mg->SetMinimum(-150.);
    mg->SetMaximum(150.);
    mg->Write();
    cnxz++;
  }
  for (TMultiGraph* mg : mTrackSegXZ) {
    mg->SetTitle("Track Segment Position X vs Z");
    mg->GetXaxis()->SetTitle("z [mm]");
    mg->GetYaxis()->SetTitle("x [mm]");
    mg->SetMinimum(-150.);
    mg->SetMaximum(150.);
    mg->Write();
  }
    for (TMultiGraph* mg : mTrackSegYZ) {
    mg->SetTitle("Track Segment Position Y vs Z");
    mg->GetXaxis()->SetTitle("z [mm]");
    mg->GetYaxis()->SetTitle("y [mm]");
    mg->SetMinimum(-150.);
    mg->SetMaximum(150.);
    mg->Write();
  }

  caf_out->Close();
}
