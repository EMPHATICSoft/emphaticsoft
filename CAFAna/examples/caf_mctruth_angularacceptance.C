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
#include <cmath>
#include <string>

void caf_mctruth_angularacceptance(std::string fileName) {

  TFile *f = new TFile(fileName.c_str());
  TTree *tree = (TTree*)f->Get("recTree");
  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);
	
  TH1D *hAll = new TH1D("All_Target","Angular Distribution of All Daughters from Target;#theta/rad;N", 100, 0., M_PI/2);
  TH1D *hObs = new TH1D("Obs_Target","Angular Distribution of Observed Daughters from Target;#theta/rad;N", 100, 0., M_PI/2);

  TH1D* hVtxZ = new TH1D("VtxZ","Vertex Z Distribution; z (mm)",2000,-500.,1500.);
  TH1D* hVtxX = new TH1D("VtxX","Vertex X Distribution; x (mm)",250,-50.,50.);
  TH1D* hVtxY = new TH1D("VtxY","Vertex Y Distribution; y (mm)",250,-50.,50.);
  TH2D* hVtxXY = new TH2D("VtxXY","Vertex (x,y) Distribution; x (mm); y (mm)",250,-50.,50.,250,-50.,50.);

  for (int i = 0; i < tree->GetEntries(); ++i)		// for each event
    {		
      tree->GetEntry(i);				// get the tree for the event
      
      if(i == 0) {
	std::cout << "Run #: " << rec->hdr.run << std::endl;}	// do this just once
      
      int ndaughters = rec->truth.beam.daughters.size();	// get the number of daughter particles in the event

      if (ndaughters == 0){rec=0; continue;}

      std::cout << "Event " << i << ": " << ndaughters << " daughters" << std::endl;
      double vtxz = rec->truth.beam.daughters[0].Vz(0);
      double vtxx = rec->truth.beam.daughters[0].Vx(0);
      double vtxy = rec->truth.beam.daughters[0].Vy(0);
      hVtxZ->Fill(vtxz);
      hVtxX->Fill(vtxx);
      hVtxY->Fill(vtxy);
      hVtxXY->Fill(vtxx,vtxy);

      // example of a cut you can place on the histogram-level, in this case,
      // its a cut that daughters should be in the target

      if (rec->truth.beam.daughters[0].Vz(0) <= 370. || 
      	  rec->truth.beam.daughters[0].Vz(0) >= 391.) {rec=0; continue;}

      for (int idx = 0; idx < ndaughters; ++idx)	// for each daughter particle
	{
	  
	  int pdg = rec->truth.beam.daughters[idx].PdgCode();		// get the particle type
	  auto p = rec->truth.beam.daughters[idx].P(0);		// total momentum of the daughter
	  auto E = rec->truth.beam.daughters[idx].E();		// total energy of the daughter
	  auto theta = asin(rec->truth.beam.daughters[idx].Pt(0)/p);	// calculate the angle w/ respect to the beam axis
	  // first, fill angular distribution for all daughters
	  hAll->Fill(theta);
	  // now fill angular distribution for only those daughters that hit _all_ SSD planes (work in progress)
	}	

      rec = 0;	// reset the record to avoid a segmentation error
		
    }							// end for each event

	
  TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  hAll->Draw();
  c1->Print("hAll.png");
	
  /*					
					TFile g("AngularDists.root","RECRATE");
					h1->Write();
					h2->Write();
					h3->Write();
					h4->Write();
					h5->Write();
					h6->Write();
  */
}
