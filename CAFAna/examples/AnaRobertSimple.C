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
  ROOT::Math::XYZVector truep0sig(0,0,0);
  ROOT::Math::XYZVector truep1sig(0,0,0);
  TH1F* hClusters = new TH1F("hClusters","hClusters",50,0,50);
  TH2F* hClustersVsPlane = new TH2F("hClustersVsPlane","hClustersVsPlane",20,0,20,50,0,50);
  TH1F* hNSigTruth = new TH1F("hNSigTruth","hNSigTruth",180,0.,0.018);
  TH1F* hNSigSelTruth = new TH1F("hNSigSelTruth","hNSigSelTruth",180,0.,0.018);
  TH1F* hNSelReco = new TH1F("hNSelReco","hNSelReco",180,0.,0.018);
  TH1F* hNSelSigReco = new TH1F("hNSelSigReco","hNSelSigReco",180,0.,0.018);

  TH1F* hRecoScattering = new TH1F("hRecoScattering","hRecoScattering",180,0.,0.018);
  TH1F* hRecoBending = new TH1F("hRecoBending","hRecoBending",500,0.,0.05);
  TH2F* hBeamProfileXY = new TH2F("hBeamProfileXY","hBeamProfileXY",100,-50.,50.,100,-50.,50.);
  TH1F* hVertexX = new TH1F("hVertexX","hVertexX",100,-50.,50.);
  TH1F* hVertexY = new TH1F("hVertexY","hVertexY",100,-50.,50.);
  TH1F* hVertexZ = new TH1F("hVertexZ","hVertexZ",200,0.,2000.);
  TH1F* hP2Theta2 = new TH1F("hP2Theta2","hP2Theta2",60,0.,0.15);
  TH1F* hCrossSection = new TH1F("hCrossSection","hCrossSection",60,0.,0.15);
  TH1F* hTrk0dXdZ = new TH1F("hTrk0dXdZ","hTrk0dXdZ",100,-0.005,0.005);
  TH1F* hTrk0dYdZ = new TH1F("hTrk0dYdZ","hTrk0dYdZ",100,0.005,0.015);
  TH1F* hTrk1dXdZ = new TH1F("hTrk1dXdZ","hTrk1dXdZ",100,-0.005,0.005);
  TH1F* hTrk1dYdZ = new TH1F("hTrk1dYdZ","hTrk1dYdZ",100,0.0,0.01);
  TH2F* hTrk1dXdZvsX = new TH2F("hTrk1dXdZvsX","hTrk1dXdZvsX",100,-50.,50.,100,-0.005,0.005);
  TH2F* hTrk1dYdZvsY = new TH2F("hTrk1dYdZvsY","hTrk1dYdZvsY",100,-50.,50.,100,0.,0.01);
  TH2F* hScatteringAngleResVsTrueAngle = new TH2F("hScatteringAngleResVsTrueAngle","hScatteringAngleResVsTrueAngle",180,0.,0.018,100,-0.005,0.005);
  TProfile* tScatteringAngleResVsTrueAngle = new TProfile("tScatteringAngleResVsTrueAngle","tScatteringAngleResVsTrueAngle",180,0.,0.018,-0.005,0.005);
  TH2F* hRecoTS1XY = new TH2F("hRecoTS1XY","hRecoTS1XY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoTS2XY = new TH2F("hRecoTS2XY","hRecoTS2XY",100,-50.,50.,100,-50.,50.);
  TH2F* hRecoTS3XY = new TH2F("hRecoTS3XY","hRecoTS3XY",100,-50.,50.,100,-50.,50.);

  int nsig = 0;
  int nsigsel = 0;
  int dontcare = 0;
  std::map<std::pair<int, int>, int> clustMap;

  caf::StandardRecord* rec = 0;
  chain_evts->SetBranchAddress("rec", &rec);
  int nentries  = chain_evts->GetEntries();

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
    //std::cout<<"Entry "<<i<<std::endl;
    chain_evts->GetEntry(i);

    clustMap.clear();

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

    int nvtx = int(rec->vtxs.vtx.size());
    int nsectrk = 0;
    int nbeamtrk = 0;
    if (IsSig(rec->truth)){
      nsig++;

      for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
        caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
        if (h.station == 1 && h.plane == 1 && h.pid == 2212) {
          truep0sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z()); 
        }
        if (h.station == 2 && h.plane == 0 && h.pid == 2212) {
          truep1sig.SetXYZ(h.mom.X(),h.mom.Y(),h.mom.Z()); 
        }
      }
      double trueScatteringSig= TMath::ACos(truep0sig.Unit().Dot(truep1sig.Unit()));          
      hNSigTruth->Fill(trueScatteringSig);
      if (IsSel(rec->vtxs.vtx)){
        hNSigSelTruth->Fill(trueScatteringSig);
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

        hRecoScattering->Fill(recoScatteringSigSel);
        hP2Theta2->Fill(recoScatteringSigSel*recoScatteringSigSel*120.*120.);

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

  hClustersVsPlane->SetTitle("Clusters Vs Plane; Plane; # Clusters");
  hClustersVsPlane->Write();

  hClustersVsPlane->SetTitle("Clusters Distribution; # Clusters; # Events");
  hClusters->Write();

  hRecoScattering->SetTitle("Scattering Angle Distribution; Scattering Angle [rad]; # Events");
  hRecoScattering->Write();

  hRecoBending->SetTitle("Bending Angle Distribution; Bending Angle [rad]; # Events");
  hRecoBending->Write();

  hBeamProfileXY->SetTitle("Beam Profile @ z = - 100; Position (X) [mm]; Position (Y) [mm]");
  hBeamProfileXY->Write();

  hVertexX->SetTitle("Reco Track Vertex X-Position; Position (X) [mm]; # Events");
  hVertexX->Write();

  hVertexY->SetTitle("Reco Track Vertex Y-Position; Position (Y) [mm]; # Events");
  hVertexY->Write();

  hVertexZ->SetTitle("Reco Track Vertex Z-Position; Position (Z) [mm]; # Events");
  hVertexZ->Write();

  hP2Theta2->Write();

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

  hRecoTS1XY->Write();
  hRecoTS2XY->Write();
  hRecoTS3XY->Write();

  std::cout<<"Nsig = "<<nsig<<std::endl;
  std::cout<<"Nsigsel = "<<nsigsel<<std::endl;

  //std::cout<<"Two or more clusters but after the target = "<<dontcare<<std::endl;

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
