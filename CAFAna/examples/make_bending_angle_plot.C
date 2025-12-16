#include "StandardRecord/StandardRecord.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"


void GetInfoFromTree(TTree* tree, TGraph *variable_hist){

  caf::StandardRecord* rec = 0;
  tree->SetBranchAddress("rec", &rec);


  int point_counter = 0;
  for(int i = 0; i < tree->GetEntries(); ++i)
  {
      tree->GetEntry(i);
      if(rec->trks.ntrk != 2)continue;
      point_counter++;
      auto track = rec->trks.trk[1];
	
      auto segments = track._sgmnt;
	
      auto seg2 = segments[0];
      auto seg3 = segments[1];
	

     double cos = seg2.mom.Dot(seg3.mom)/(seg2.mom.Mag() * seg3.mom.Mag());

     variable_hist->SetPoint(point_counter,acos(cos)*1e3, seg3.mom.Mag()); 
	
   }


}

void make_bending_angle_plot() // std::string MC_file, std::string Data_file)
{
  // Define input caf file here
  TFile *file_MC = new TFile("/exp/emph/data/production/prod6/mc/caf/emphmc_r2408_s0010.v6.01.00.artdaq.caf.root","read");
  TFile *file_data = new TFile("/exp/emph/data/users/abhattar/laterCAFs/2408/emphdata_r2408_s0002.artdaq.v6.00.00_caf.root","read");

  // Get the event record tree
  TTree *tree_MC = (TTree*)file_MC->Get("recTree");
  TTree *tree_data = (TTree*)file_data->Get("recTree");

	
  TGraph *angle_MC = new TGraph();angle_MC->SetMarkerColor(kRed); 
  TGraph *angle_data = new TGraph();angle_data->SetMarkerColor(kBlack); angle_data->SetMarkerStyle(6);


  GetInfoFromTree(tree_MC,angle_MC);
  GetInfoFromTree(tree_data, angle_data);  

 
  TCanvas *c1 = new TCanvas("","");
  TLegend *l = new TLegend(0.6,0.6,0.9,0.8);
  l->AddEntry(angle_MC, "MC", "p");
  l->AddEntry(angle_data, "data", "p");


  angle_data->GetYaxis()->SetRangeUser(0,120);
  angle_data->Draw("AP");
  angle_MC->GetYaxis()->SetRangeUser(0,120);
  angle_MC->Draw("P same "); 
  l->Draw("same");


  // Make canvases and draw histograms on them
  // Easier to manipulate how we want things too look
  // Can also now do more histogram and canvas manipulation
  // This is as basic as it comes for now

}
