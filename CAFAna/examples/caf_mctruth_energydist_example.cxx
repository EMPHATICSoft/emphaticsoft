// Build a histogram of the kinetic energies of the daughter particles
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




void caf_mctruth_energydist_example(){
	TFile *f = new TFile(".caf.root");	// your CAF file here
	TTree *tree = (TTree*)f->Get("recTree");
	std::cout << tree;
	caf::StandardRecord* rec = 0;
	tree->SetBranchAddress("rec", &rec);
	
	float beamEnergy = 120.;		// plug in your beam energy, makes plotting nicer
	
        TH1D *h1 = new TH1D("All","Daughter Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);	// histograms for each particle
        TH1D *h2 = new TH1D("Electron","Electron Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);
        TH1D *h3 = new TH1D("Piplus","Pi+ Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);
        TH1D *h4 = new TH1D("Piminus","Pi- Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);
        TH1D *h5 = new TH1D("Kaon","Kaon Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);
        TH1D *h6 = new TH1D("Proton","Proton Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);
	TH1D *h7 = new TH1D("Neutron","Neutron Kinetic Energy Distribution;KE/GeV;N", 1000, 0., beamEnergy);
	
	for (int i = 0; i < tree->GetEntries(); ++i)		// for each event
	{
		
		tree->GetEntry(i);				// get the tree for the event
		
		if(i == 0) {std::cout << "Run #: " << rec->hdr.run << std::endl;}	// do this once just once
	
		int ndaughters = rec->truth.beam.daughters.size();	// get the number of daughter particles in the event

		for (int idx = 0; idx < ndaughters; ++idx)	// for each daughter particle
		{
			int pdg = rec->truth.beam.daughters[idx].PdgCode();		// get the particle type
			auto E = rec->truth.beam.daughters[idx].E(0);		// total energy of the daughter in MeV
			auto M = rec->truth.beam.daughters[idx].Mass();		// mass of the daughter, in GeV
			h1->Fill(E/1000 - M));					// plot it
			
			if(pdg  == 11 || pdg == -11){
                                h2->Fill(E/1000 - M);
                        }
                        else if(pdg  == 211){
                                h3->Fill(E/1000 - M);
                        }
                        else if(pdg  == -211){
                                h4->Fill(E/1000 - M);
                        }
                        else if(pdg  == 321 || pdg == -321 || pdg == 130 || pdg == 311){
                                h5->Fill(E/1000 - M);
                        }
                        else if(pdg  == 2212){
                                h6->Fill(E/1000 - M);
                        }
			else if (pdg == 2112){
				h7->Fill(E/1000 - M);
			}		
		 
		}	
		
		rec = 0;	// reset the record to avoid a segmentation error
		
	}							// end for each event
	
	
	TCanvas *c1 = new TCanvas("c1","c1",800,800);
  	h1->Draw();
  	c1->Print("h1.png");
	c1->SetLogy();
	
	// this is to make a root file that holds the histograms
	/*					
	TFile g("EnergyDists.root","RECREATE");
	h1->Write();
	h2->Write();
        h3->Write();
        h4->Write();
        h5->Write();
        h6->Write();
	h7->Write();
	*/
}

int main(){
	caf_mctruth_energydist_example();
	return 0;
}
