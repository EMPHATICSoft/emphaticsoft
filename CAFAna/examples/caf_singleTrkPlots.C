#include "StandardRecord/StandardRecord.h"
#include "StandardRecord/SRVector3D.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"

void caf_singleTrkPlots(std::string fname)
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
  TH1I* hNTrkSeg = new TH1I("nTrkSeg", "", 20,0,20);
  TH1D* hScattAngleRes = new TH1D("hScattAngleRes","Scattering Angle Resolution;#theta_{reco}-#theta_{true} ", 100,-0.005,0.005);
  TH1D* hBendAngleRes = new TH1D("hBendAngleRes","Bending Angle Resolution;#theta_{reco}-#theta_{true}", 100,-0.0025,0.0025);

  double recoAngle, trueAngle, angleDiff;
  caf::SRVector3D truep0(0,0,0);
  caf::SRVector3D truep1(0,0,0);
  caf::SRVector3D truep2(0,0,0);

  // Loop over our tree and each time we call GetEntries, the data
  // in the tree is copied to rec.
  for(int i = 0; i < tree->GetEntries(); ++i)
  {
      tree->GetEntry(i);
      // Spit out Run number for the first entry only
      if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;
            
      // Find how many track segments there are
      int ntrkseg = int(rec->sgmnts.seg.size());
      hNTrkSeg->Fill(ntrkseg);

      // only look at events with three track segments
      if (ntrkseg == 3) {
	// first fill scattering angle resolution plot
	caf::SRVector3D& p0 = rec->sgmnts.seg[0].mom;
	caf::SRVector3D& p1 = rec->sgmnts.seg[1].mom;
	recoAngle = TMath::ACos(p0.Dot(p1)/(p0.Mag()*p1.Mag()));

	int nGot=0;
	for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
	  caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	  if (h.GetStation == 1 && h.GetPlane == 1) {
	    truep0.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (h.GetStation == 2 && h.GetPlane == 0) {
	    truep1.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (nGot == 2) break;
	}

	if (nGot == 2) {
	  trueAngle = TMath::ACos(truep0.Dot(truep1)/(truep0.Mag()*truep1.Mag()));
	  angleDiff = recoAngle - trueAngle;
	  hScattAngleRes->Fill(angleDiff);
	}

	// now fill bending angle resolution plot
	caf::SRVector3D& p2 = rec->sgmnts.seg[2].mom;
	recoAngle = TMath::ACos(p1.Dot(p2)/(p1.Mag()*p2.Mag()));
	nGot=0;
	for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
	  caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	  if (h.GetStation == 3 && h.GetPlane == 2) {
	    truep1.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (h.GetStation == 6 && h.GetPlane == 0) {
	    truep2.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	    ++nGot; }
	  if (nGot == 2) break;
	}
	if (nGot == 2) {
	  trueAngle = TMath::ACos(truep1.Dot(truep2)/(truep1.Mag()*truep2.Mag()));
	  angleDiff = recoAngle - trueAngle;
	  hBendAngleRes->Fill(angleDiff);
	}
      }
  } // end loop over entries
  
  // Make canvases and draw histograms on them
  // Easier to manipulate how we want things too look
  // Can also now do more histogram and canvas manipulation
  // This is as basic as it comes for now
  TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  hNTrkSeg->SetLineWidth(2);
  hNTrkSeg->Draw();
  c1->Print("hNTrkSeg.png");

  hScattAngleRes->SetLineWidth(2);
  hScattAngleRes->Draw();
  c1->Print("ScattAngleRes.png");

  hBendAngleRes->SetLineWidth(2);
  hBendAngleRes->Draw();
  c1->Print("BendAngleRes.png");

}
