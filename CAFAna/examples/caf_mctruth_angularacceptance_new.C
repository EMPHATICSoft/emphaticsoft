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
	
  TH1D *hAll = new TH1D("All_Target","Angular Distribution of All Daughters from Target;#theta/rad;N", 100, 0., M_PI/2);
  TH1D *hObs = new TH1D("Obs_Target","Angular Distribution of Observed Daughters from Target;#theta/rad;N", 100, 0., M_PI/2);

  TH1D* hVtxZ = new TH1D("VtxZ","Vertex Z Distribution; z (mm)",3300,-1000.,2300.);
  TH1D* hVtxX = new TH1D("VtxX","Vertex X Distribution; x (mm)",160,-80.,80.);
  TH1D* hVtxY = new TH1D("VtxY","Vertex Y Distribution; y (mm)",160,-80.,80.);
  TH2D* hVtxXY = new TH2D("VtxXY","Vertex (x,y) Distribution; x (mm); y (mm)",160,-80.,80,160,-80.,80.);
  TH2D* hVtxZX = new TH2D("VtxZX","Vertex (z,x) Distribution; z (mm); x (mm)",3300,-1000.,2300.,160,-80.,80.);

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

      if (ndaughters == 0){rec=0; continue;}

      //      std::cout << "Event " << i << ": " << ndaughters << " daughters" << std::endl;
      double vtxz = rec->truth.beam.daughters[0].Vz(0);
      double vtxx = rec->truth.beam.daughters[0].Vx(0); 
      double vtxy = rec->truth.beam.daughters[0].Vy(0); 
      hVtxZ->Fill(vtxz);
      hVtxY->Fill(vtxy);
      hVtxX->Fill(vtxx);
      hVtxXY->Fill(vtxx,vtxy);
      hVtxZX->Fill(vtxz,vtxx);

      // example of a cut you can place on the histogram-level, in this case,
      // its a cut that daughters should be in the target

      if (rec->truth.beam.daughters[0].Vz(0) <= -45. || 
      	  rec->truth.beam.daughters[0].Vz(0) >= -25.) {rec=0; continue;}

      std::unordered_map<int,int> trackNPlanes;
      std::unordered_map<int,double> trackAngle;
      for (int idx = 0; idx < ndaughters; ++idx)	// for each daughter particle
	{
	  
	  auto daughter = rec->truth.beam.daughters[idx];

	  int trkId = daughter.TrackId();
	  trackNPlanes[trkId] =0;
	  //	  std::cout << "trkId = " << trkId << std::endl;
	  int pdg = daughter.PdgCode();		// get the particle type
	  auto p = daughter.P(0);	// total momentum of the daughter
	  auto E = daughter.E();	// total energy of the daughter
	  auto theta = asin(daughter.Pt(0)/p);	// calculate the angle w/ respect to the beam axis
	  // first, fill angular distribution for all daughters
	  hAll->Fill(theta);
	  trackAngle[trkId] = theta;
	}
      
      // now fill angular distribution for only those daughters that hit _all_ SSD planes (work in progress)
      //      std::cout << "Found " << rec->truth.truehits.truehits.size() << " hits" << std::endl;
      for (size_t j=0; j<rec->truth.truehits.truehits.size(); ++j) {
	caf::SRTrueSSDHits& h = rec->truth.truehits.truehits[j];
	int trkId = h.GetTrackID;
	if (trackNPlanes.find(trkId) != trackNPlanes.end()) {
	  trackNPlanes[trkId] += 1;
	}
      }
      
      // now we go _back_ to the list of particles and fill in the histogram 
      // for those particles we "saw" in our spectrometer
      for ( auto itrk : trackNPlanes) {
	if (trackNPlanes[itrk.first] >= 5) {
	  hObs->Fill(trackAngle[itrk.first]);
	}
      }
      
      rec = 0;	// reset the record to avoid a segmentation error
      
    }						// end for each event
  
  
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
