// Build a histogram of the angular acceptance of the daughter particles
// 

#include "TCanvas.h"
#include "TROOT.h"
#include "TF1.h"
#include "TLegend.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TH1F.h"
#include "StandardRecord/StandardRecord.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TChain.h"

#include <cmath>
#include <cstddef>
#include <string>
#include <unordered_map>

void caf_mctruth_angularacceptance(std::string fileName, int nEntries=-1) 
{
  
  //  TFile *f = new TFile(fileName.c_str());
  TChain *chain = new TChain("recTree");
  char fname[128];
  for (int i=1; i<=20; i++){
    sprintf(fname,"%s_%d..caf.root",fileName.c_str(),i);
    std::cout << "Adding " << fname << "to the chain" << std::endl;
    chain->Add(fname);
  }

//  TTree *tree = (TTree*)f->Get("recTree");
  caf::StandardRecord* rec = 0;
  chain->SetBranchAddress("rec", &rec);
	
  TH1D *hAll = new TH1D("All_Target","Angular Distribution of All Daughters from Target;#theta/rad;N", 200, 0,0.05);
  TH1D *hObs = new TH1D("Obs_Target","Angular Distribution of Observed Daughters from Target;#theta/rad;N", 200, 0,0.05);
  TH1F *hNHit = new TH1F("NHitDist","Number of True SSD Hits Per Event;Num Hits",200,0,200);

  int nEnt = chain->GetEntries();
  if (nEntries > 0) nEnt = nEntries;
  std::cout << "Looping over " << nEnt << " entries in the chain" << std::endl;

  for (int i = 0; i < nEnt; ++i)		// for each event
    {		
      chain->GetEntry(i);				// get the tree for the event
      
      if(i == 0) {
	std::cout << "Run #: " << rec->hdr.run << std::endl;}	// do this just once
      if ((i%10000) == 0)
	std::cout << "Event " << i << std::endl;

      int ndaughters = rec->truth.beam.daughters.size();	// get the number of daughter particles in the event

      if (ndaughters > 0){rec=0; continue;}

      int nhitstation[8][3][2];
      for (int ist = 0; ist<8; ++ist)
	for (int ipl = 0; ipl<3; ++ipl)
	  for (int ise = 0; ise<2; ++ise)
	    nhitstation[ist][ipl][ise] = 0;

      hNHit->Fill(rec->truth.truehits.truehits.size());

      TVector3 v0;
      TVector3 v1;
      int nFound = 0;
      for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
	caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	nhitstation[h.GetStation][h.GetPlane][h.GetSensor]++;
	if ((h.GetStation == 1) && (h.GetPlane == 1)) { // get incoming angle
	  nFound++;
	  v0.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	}
	if ((h.GetStation == 2) && (h.GetPlane == 0)) { // get outgoing angle
	  nFound++;
	  v1.SetXYZ(h.GetPx,h.GetPy,h.GetPz);
	}
	
      }

      bool isOk;
      for (int ist = 0; ist<8; ++ist)
	for (int ipl = 0; ipl<3; ++ipl)
	  for (int ise = 0; ise<2; ++ise)
	    if (nhitstation[ist][ipl][ise] > 1)
	      isOk = false;
      if (rec->truth.truehits.truehits.size()<30 &&
	  rec->truth.truehits.truehits.size()>=20) isOk = true;
      
      if (nFound == 2) {	
	double dtheta = v0.Angle(v1);
	// first, fill angular distribution for all daughters
	hAll->Fill(dtheta);
	if (isOk)
	  hObs->Fill(dtheta);
      }

      rec = 0;	// reset the record to avoid a segmentation error
      
    }
  
  TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  hAll->Draw();
  c1->Print("hAll.png");
  TCanvas *c2 = new TCanvas("c2","c2",1000,800);
  hObs->Draw();
  c2->Print("hObsAll.png");
  TH1D* hRatio = (TH1D*)hObs->Clone("hRatio");
  hRatio->Divide(hAll);
  TCanvas *c3 = new TCanvas("c3","c3",1000,800);
  hRatio->Draw();


}
