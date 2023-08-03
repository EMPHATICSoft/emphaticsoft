// Build a histogram of the angular distribution of the daughter particles
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




void caf_mctruth_angulardist_example(){
	TFile *f = new TFile(".caf.root");
	TTree *tree = (TTree*)f->Get("recTree");
	std::cout << tree;
	caf::StandardRecord* rec = 0;
	tree->SetBranchAddress("rec", &rec);
	
	TH1D *h1 = new TH1D("All_Target","Daughter in Target Angular Distribution;#theta/rad;N", 100, 0., M_PI/2);
	TH1D *h2 = new TH1D("Electron_Target","Electron in Target Angular Distribution;#theta/rad;N", 100, 0., M_PI/2);
	TH1D *h3 = new TH1D("Piplus_Target","Pi+ in Target, 1 GeV Angular Distribution;#theta/rad;N", 100, 0., M_PI/2);
	TH1D *h4 = new TH1D("Piminus_Target","Pi- in Target Angular Distribution;#theta/rad;N", 100, 0., M_PI/2);
	TH1D *h5 = new TH1D("Kaon_Target","Kaon in Target Angular Distribution;#theta/rad;N", 100, 0., M_PI/2);
	TH1D *h6 = new TH1D("Proton_Target","Proton in Target Angular Distribution;#theta/rad;N", 100, 0., M_PI/2);
	
	for (int i = 0; i < tree->GetEntries(); ++i)		// for each event
	{		
		tree->GetEntry(i);				// get the tree for the event
		
		if(i == 0) {std::cout << "Run #: " << rec->hdr.run << std::endl;}	// do this just once
			
		int ndaughters = rec->truth.beam.daughters.size();	// get the number of daughter particles in the event
		if (ndaughters == 0){rec=0; continue;}		
									// example of a cut you can place on the histogram-level, in this case,
									// its a cut that daughters should be in the target
		if (rec->truth.beam.daughters[0].Vz(0) <= 187 || rec->truth.beam.daughters[0].Vz(0) >= 212){rec=0; continue;}
		
		for (int idx = 0; idx < ndaughters; ++idx)	// for each daughter particle
		{
			
			int pdg = rec->truth.beam.daughters[idx].PdgCode();		// get the particle type
			auto p = rec->truth.beam.daughters[idx].P(0);		// total momentum of the daughter
			auto E = rec->truth.beam.daughters[idx].E();		// total energy of the daughter
			auto theta = asin(rec->truth.beam.daughters[idx].Pt(0)/p);	// calculate the angle w/ respect to the beam axis
			h1->Fill(theta);					// plot it
				
			// make specific plots for each particle, you can add anything you want
			if(pdg  == 11 || pdg == -11){                                                     
                                h2->Fill(theta);                                        
                        }
                        else if(pdg  == 211){
                                h3->Fill(theta);
                        }
                        else if(pdg  == -211){
                                h4->Fill(theta);
                        }
                        else if(pdg  == 321 || pdg == -321){
                                h5->Fill(theta);
			}
                        else if(pdg  == 2212){
                                h6->Fill(theta);
                        }					
		}	
		
		rec = 0;	// reset the record to avoid a segmentation error
		
	}							// end for each event

	
	TCanvas *c1 = new TCanvas("c1","c1",1000,800);
  	h1->Draw();
  	c1->Print("h1.png");
	
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

int main(){
	caf_mctruth_angulardist_example();
	return 0;
}
