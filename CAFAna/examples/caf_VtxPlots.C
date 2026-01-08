#include "StandardRecord/StandardRecord.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TProfile.h"
#include "TGraph.h"

void caf_VtxPlots(std::string fname)
{
  // Define input caf file here
  TFile *file = new TFile(fname.c_str(), "read");

  // Get the event record tree
  TTree *tree = (TTree*)file->Get("recTree");

  // We must create a StandardRecord object and set the branch address
  // using our tree.
  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  // Initalize a simple histogram
  TH1I* hNTrkSeg = new TH1I("nTrkSeg", "Number of Reco TrackSegments;;Event", 200,0,200);
  TH1I* hNVtx = new TH1I("nVtx","Number of Reco Vertices;;Event",2,0,2);
  TH1I* hNBeamTrk = new TH1I("nBeamTrk","Number of Reco Beam Tracks;;Event",2,0,2);
  TH1I* hNSecTrk = new TH1I("nSecTrk","Number of Reco Sec. Tracks;;Event",2,0,2);
  TH1D* hRecoScattAngle = new TH1D("hRecoScattAngle","Reco Scattering Angle;#theta_{reco}", 400,0.,0.05);
  TH1F* hVertexZ = new TH1F("hVertexZ","Vertex Z; z (mm)",200,0.,1000.);
  TH2F* hVertexXY = new TH2F("hVertexXY","Vertex (x,y); x (mm); y (mm)",50,-20.,20.,50,-20.,20.);
  TH1D* hTrueScattAngle = NULL;
  TH2D* hScattAngleRes = NULL;

  bool isMC = true;
  for (int i = 0; i < tree->GetEntries(); ++i)
  {
    tree->GetEntry(i);
    // Spit out Run number for the first entry only

    if (i == 0) {
      std::cout << "Run #:" << rec->hdr.run << std::endl;
      isMC = rec->hdr.ismc;
      if (isMC) {
        std::cout << "This is MC" << std::endl;
        hTrueScattAngle = new TH1D("hTrueScattAngle","True Scattering Angle;#theta_{reco}", 400,0.,0.02);
        hScattAngleRes = new TH2D("hScattAngleRes","Scattering Angle Resolution;#theta_{reco}", 400,0.,0.02,200,-0.02,0.02);
      }
    }

    // Find how many track segments there are
    int ntrkseg = int(rec->sgmnts.seg.size());
    hNTrkSeg->Fill(ntrkseg);

    // Find how many vertices and sec. tracks there are
    int nvtx = int(rec->vtxs.vtx.size());
    hNVtx->Fill(nvtx);
    int nsectrk = 0;
    int nbeamtrk = 0;
    if (nvtx)
    {
      nsectrk = int(rec->vtxs.vtx[0].sectrk.size());
      nbeamtrk = 1;
      hVertexZ->Fill(rec->vtxs.vtx[0].pos.Z());
      hVertexXY->Fill(rec->vtxs.vtx[0].pos.X(), rec->vtxs.vtx[0].pos.Y());
    }
    hNSecTrk->Fill(nsectrk);
    hNBeamTrk->Fill(nbeamtrk);

    if (nsectrk)
    {
      caf::SRBeamTrack &beamtrk = rec->vtxs.vtx[0].beamtrk;
//      std::cout << "beamtrk.mom = " << beamtrk.momTrgt << std::endl;
      for (int j = 0; j < nsectrk; ++j)
      {
        caf::SRSecondaryTrack &sectrk = rec->vtxs.vtx[0].sectrk[j];
//        std::cout << "sectrk.mom = " << sectrk.momTrgt << std::endl;
        double theta = TMath::ACos(sectrk.momTrgt.Unit().Dot(beamtrk.momTrgt.Unit()));
        hRecoScattAngle->Fill(theta);
        if (isMC) {
          caf::SRSimpleTruth& beamtruth = beamtrk.truth;         
          std::cout << "beamtruth mom = " << beamtruth.mom << std::endl;
          caf::SRSimpleTruth& sectruth = sectrk.truth[0];
          std::cout << "sectruth mom = " << sectruth.mom << std::endl;
          double thetatrue = TMath::ACos(sectruth.mom.Unit().Dot(beamtruth.mom.Unit()));          
          hTrueScattAngle->Fill(thetatrue);
          hScattAngleRes->Fill(thetatrue,thetatrue-theta);
        }
      }
    }
  }

  // make new output root file?
  TFile *caf_out = new TFile("caf_VtxPlots.root", "RECREATE");
  hRecoScattAngle->Write();
  if (isMC) {
    hTrueScattAngle->Write();
    hScattAngleRes->Write();
  }
  hNTrkSeg->Write();
  hNVtx->Write();
  hNBeamTrk->Write();
  hNSecTrk->Write();
  hVertexZ->Write();
  hVertexXY->Write();
}
