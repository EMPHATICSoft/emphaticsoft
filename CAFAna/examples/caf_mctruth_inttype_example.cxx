
// Build a histogram of the interaction types of daughter particles
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
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include <map>

std::multimap<int, std::string> invert(std::map<std::string, int> & mymap)
        {
                std::multimap<int, std::string> multiMap;

                std::map<std::string, int> :: iterator it;
                for (it=mymap.begin(); it!=mymap.end(); it++)
                {
                        multiMap.insert(make_pair(it->second, it->first));
                }

                return multiMap;
        }


void caf_mctruth_inttype_example(){
	TFile *f = new TFile(".caf.root");		// your CAF file here
	TTree *tree = (TTree*)f->Get("recTree");
	std::cout << tree;
	caf::StandardRecord* rec = 0;
	tree-> SetBranchAddress("rec", &rec);
	
	TH1I *h1 = new TH1I("h1","Interaction Type", 10, 0, 10);

	std::map<std::string, int> proc;			// make a map from a process type to an int
	proc["No Interaction"] = 0;
	
	for (int i = 0; i < tree->GetEntries(); ++i)
	{
		
		tree->GetEntry(i);
		
		if(i == 0) {std::cout << "Run #: " << rec->hdr.run << std::endl;}
	
		int ndaughters = rec->truth.beam.daughters.size();
		if (ndaughters == 0){h1->Fill(proc["No Interaction"]);}
		else{
			std:string pname = rec->truth.beam.daughters[0].Process();	// access the process name
			
			if (proc.count(pname)!=0){
				h1->Fill(proc[pname]);			// plot it
			}			
			else{
				proc[pname] = proc.size();		// add it to the map if it isn't there yet
				h1->Fill(proc[pname]);
			}
		}
		rec = 0;
		
	}

	std::multimap<int, std::string> newmap = invert(proc);

	TCanvas *c1 = new TCanvas("c1","c1",800,800);

	auto* legend = new TLegend();
	
	std::multimap<int, std::string>::iterator itr;
	for (itr = newmap.begin(); itr!= newmap.end(); itr++){
		std::cout << itr->second << " " << itr->first << std::endl;
		std::string procnum = std::to_string(itr->first);
		std::string procname = itr->second;
		auto proclegend = procname + " = " + procnum;
		legend->AddEntry((TObject*)0, proclegend.c_str(), "");
	}

	c1->SetLogy(1); 
 	h1->Draw();
	legend->Draw();
  	c1->Print("h1.png");

						
	TFile g("InteractionType.root", "RECREATE");
	h1->Write();
	legend->Write("legend");
	
}

int main(){
	caf_mctruth_inttype_example();
	return 0;
}
