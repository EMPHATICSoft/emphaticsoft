#include "StandardRecord/StandardRecord.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"

void caf_event_loop_demo()
{
  // Define input caf file here
  TFile *file = new TFile("emphdata_v03.caf.root", "read");

  // Get the event record tree
  TTree *tree = (TTree*)file->Get("recTree");

  // We must create a StandardRecord object and set the branch address
  // using our tree.
  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);

  // Initalize a simple histogram
  TH1F *hNhits = new TH1F("nHits", "", 100000, 0, 30000);

  // Loop over our tree and each time we call GetEntries, the data
  // in the tree is copied to rec.
  for(int i = 0; i < tree->GetEntries(); ++i)
  {
      tree->GetEntry(i);
      // Spit out Run number for the first entry only
      if(i == 0) std::cout << "Run #:" << rec->hdr.run << std::endl;
      
      // Overkill, but spit out the event number for every entry
      std::cout << "Event #:" << rec->hdr.evt << std::endl;

      // Find how many arings there are
      std::cout << "# arings: " << rec->ring.arich.size() << std::endl;
      int narings = rec->ring.arich.size();

      // loop over arings
      for(int idx = 0; idx < narings; ++idx)
      {
	// print number of hits for this aring
	std::cout << rec->ring.arich[idx].nhit << std::endl;

	// Fill our histogram with nhit
        hNhits->Fill(rec->ring.arich[idx].nhit);
	
      } // end loop over arings

  } // end loop over entries

  // Make canvases and draw histograms on them
  // Easier to manipulate how we want things too look
  // Can also now do more histogram and canvas manipulation
  // This is as basic as it comes for now
  TCanvas *c1 = new TCanvas("c1","c1",800,1000);
  hNhits->Draw();
  c1->Print("hNhits.png");

}
