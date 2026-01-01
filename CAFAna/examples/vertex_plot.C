#include "StandardRecord/StandardRecord.h"
#include "StandardRecord/SRVector3D.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TMultiGraph.h"

void vertex_plot(std::string fname)
{
  TChain *chain_evts = new TChain("recTree");

  // Check if input is a ROOT file or a file list
  if (fname.find(".root") != std::string::npos) {
    // Single ROOT file
    chain_evts->Add(fname.c_str());
    std::cout << "Added ROOT file: " << fname << std::endl;
  } else {
    // File list
    std::ifstream inputFile(fname);
    std::string filename;

    if (inputFile.is_open()) {
      while (std::getline(inputFile, filename)) {
        chain_evts->Add(filename.c_str());
      }
      inputFile.close();
      std::cout << "Loaded file list: " << fname << std::endl;
    }
    else {
      std::cerr << "Error opening file: " << fname << std::endl;
      return;
    }
  }

  caf::StandardRecord* rec = 0;
  chain_evts->SetBranchAddress("rec", &rec);
  int nentries  = chain_evts->GetEntries();

  double recoAngle;
  double recoAngleNew;
  double recoBend;
  
  // Create histograms
  TH1F *hVertexZ = new TH1F("hVertexZ", "Track Vertex Z Position;Z [mm];Events", 200, -200, 800);
  TH1F *hResidual = new TH1F("hResidual", "Track Residual at Target;Residual [mm];Events", 2000, 0, 1);
  TH2F *hVertexZVsResidual = new TH2F("hVertexZVsResidual", "Vertex Z vs Residual;Z [mm];Residual [mm]", 50, -200, 800, 50, 0, 0.2);


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

  TFile* caf_out = new TFile("ana.root","RECREATE");

  for(int i=0;i<nentries;i++){

    chain_evts->GetEntry(i);

    // Select 70% of data entries not already used for alignment
    std::string digitStr = std::to_string(rec->hdr.evt);

    // Spit out Run number for the first entry only
    if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;

    if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
      continue;
    }
    int nclusters = int(rec->cluster.clust.size());
    std::cout<<"Event: "<<i<<"  Num Clusters: "<<nclusters<<std::endl;
    std::cout<<"Num Tracks: "<<rec->trks.trk.size()<<std::endl;
    
    if (rec->trks.trk.size() == 2){
 
      // Tracks 
      caf::SRTrack& trk0 = rec->trks.trk[0]; // beam track
      caf::SRTrack& trk1 = rec->trks.trk[1]; // reconstructed track

      // Track segments
      caf::SRTrackSegment& ts1 = trk0._sgmnt[0];
      caf::SRTrackSegment& ts2 = trk1._sgmnt[0];
      caf::SRTrackSegment& ts3 = trk1._sgmnt[1];
      std::cout<<"Event: "<<i<<"  Trk0 Vtx Z: "<<trk0.vtx[2]<<"  Trk1 Vtx Z: "<<trk1.vtx[2]<<std::endl;
      hVertexZ->Fill(trk1.vtx[2]);
      hResidual->Fill(trk1.vtx_residual);
      hVertexZVsResidual->Fill(trk1.vtx[2], trk1.vtx_residual);
    
    }
  } 

  hVertexZ->Write();
  hResidual->Write();
  hVertexZVsResidual->Write();
  caf_out->Close();
}

