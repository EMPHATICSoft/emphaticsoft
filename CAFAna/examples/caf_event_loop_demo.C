#include "StandardRecord/StandardRecord.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"

void caf_event_loop_demo()
{
  TFile *file = new TFile("emphdata_v02.caf.root", "read");

  // Get the event record tree
  TTree *tree = (TTree*)file->Get("recTree");

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  TH1F *hNhits = new TH1F("nHits", "", 100000, 0, 30000);
  
  for(int i = 0; i < tree->GetEntries(); ++i)
  {
      tree->GetEntry(i);
      if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;
      std::cout << "Event #:" << rec->hdr.evt << std::endl;

      
      // Here we could loop over the vector of arich rings
      std::cout << "# arings: " << rec->ring.arich.size() << std::endl;
      int narings = rec->ring.arich.size();

      // loop over arings
      for(int idx = 0; idx < narings; ++idx)
      {
	// print number of hits for this aring
	std::cout << rec->ring.arich[idx].nhit << std::endl;

	// Fill our histogram with this.
        hNhits->Fill(rec->ring.arich[idx].nhit);
	
      } // loop over arings

  } // loop over entries

  // Make canvases and draw histograms on them
  // Easier to manipulate how we want things too look
  TCanvas *c1 = new TCanvas("c1","c1",800,1000);
  hNhits->Draw();
  c1->Print("hNhits.png");

}
