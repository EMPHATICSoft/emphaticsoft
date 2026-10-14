#include "StandardRecord/StandardRecord.h"

#include "Math/Vector3D.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TH2.h"
#include "THStack.h"
#include "TTree.h"
#include "TFile.h"
#include "TProfile.h"
#include "TLegend.h"
#include "TGraph.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Configuration / labels
// ----------------------------------------------------------------------------
// Index -> PDG code for the three hypotheses we separate (pion, kaon, proton).
std::map<int,int> pdg_pos = {{0,211},{1,321},{2,2212}};

const char* xlabels[3]     = {"True Pion", "True Kaon", "True Proton"};
const char* ylabels[3]     = {"S Pion", "S Kaon", "S Proton"};
const char* backov_pids[5] = {"electron", "muon", "pion", "kaon", "proton"};

// ----------------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------------

// Ratio-log-likelihood PID selector. Returns 0 = pion, 1 = kaon, 2 = proton.
// (Hand-tuned cuts in the LL-ratio vs momentum plane.)
int classifyPID(double LL_pi_p, double LL_pi_K, double pz)
{
  bool prot = LL_pi_p > -0.1 ||
              (LL_pi_p <= -0.1 && LL_pi_K > -0.1 && pz > 6);

  bool kaon = !prot && ((LL_pi_K > -0.2 && pz < 4) ||
                        (pz < 5 && 0.5*pz - 3 > LL_pi_p) ||
                        (pz > 5 && pz < 10 && 0.03*pz - 0.7 > LL_pi_p));

  if (prot) return 2;
  else if (kaon) return 1;
  else return 0; // pion
}

enum Spec { kPi=0, kK=1, kP=2, kN=3, kNo=-1 };  // <-- CONFIRM this index order!
const char* sp[3]={"pi","K","P"};

int PdgToSpec(int pdg){
  switch(std::abs(pdg)){ case 2212:return kP; case 321:return kK; case 211:return kPi; }
  return kNone;
}

int TrueSpec(const std::vector<caf::SRSimpleTruth>& tv){
  int pdg=tv[0].pdgCode;
  return PdgToSpec(pdg);
}

int RecoSpecLL(const caf::SRPID& a){
  if(a.trackID<0) return kNo;
  int best=kNo; double bv=-1e30;
  for(int s=0;s<kN;s++){ double v=a.scoresML[s]; if(v<=-1.0) continue; if(v>bv){bv=v;best=s;} }
  return best;                          // <-- if scoresLL are -2lnL, flip to MIN
}


// Build the three per-truth-species LL-ratio TGraphs and register them in leg.
//   tag    : short name used in the object name (e.g. "pP")
//   ytitle : y-axis title (the specific LL ratio shown)
std::vector<TGraph*> makeLLGraphs(const char* tag, const char* ytitle, TLegend* leg)
{
  std::vector<TGraph*> v;
  for (int i = 0; i < 3; ++i) {
    TGraph* g = new TGraph();
    g->SetName(Form("LL_%s_%d", tag, pdg_pos[i]));
    g->SetTitle("Ratio LogLikelihoods vs Momenta");
    g->GetXaxis()->SetTitle("Reconstructed Pz [GeV/c]");
    g->GetYaxis()->SetTitle(ytitle);
    v.push_back(g);
    leg->AddEntry(g, Form("True %d", pdg_pos[i]), "p");
  }
  return v;
}

// Draw the three MC LL-ratio graphs with the data graph overlaid, save to pdf.
void drawLLCanvas(std::vector<TGraph*>& mc, TGraph* data, TLegend* leg, const char* pdf)
{
  TCanvas* c = new TCanvas("", "", 800, 600);
  mc[0]->SetMarkerColor(kRed+1);
  mc[0]->GetYaxis()->SetRangeUser(-2, 2);
  mc[0]->Draw("AP");
  mc[1]->SetMarkerColor(kGreen-1); mc[1]->Draw("P same");
  mc[2]->SetMarkerColor(kBlue-1);  mc[2]->Draw("P same");
  data->Draw("P same");
  leg->AddEntry(data, "data", "p");
  leg->Draw("same");
  c->SaveAs(pdf);
  delete c;
}

// Draw a 3x3 confusion matrix (truth species vs selected species) to pdf.
TH2D* drawConfusion(const char* name, const float mtx[3][3], const char* pdf)
{
  TH2D* h = new TH2D(name, name, 3,0,3, 3,0,3);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      h->SetBinContent(i+1, j+1, mtx[i][j]);

  TCanvas* c = new TCanvas("", "", 800, 600);
  c->SetRightMargin(0.15);
  for (int i = 1; i <= 3; ++i) {
    h->GetXaxis()->SetBinLabel(i, xlabels[i-1]);
    h->GetYaxis()->SetBinLabel(i, ylabels[i-1]);
  }
  h->SetStats(0);
  h->Draw("text colz");
  c->SaveAs(pdf);
  delete c;
  return h;
}

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
void caf_arichinfo(std::vector<const char *> files, std::string name_out)
{
  // --- input ---
  TChain chain("recTree");
  for (size_t i = 0; i < files.size(); ++i) {
    std::cout << "Adding file: " << files[i] << std::endl;
    chain.Add(files[i]);
  }

  caf::StandardRecord* rec = 0;
  chain.SetBranchAddress("rec", &rec);

  // --- booking: event/vertex counters & kinematics ---
  TH1I* hNTrkSeg  = new TH1I("nTrkSeg", "Number of Reco TrackSegments;;Event", 200,0,200);
  TH1I* hNVtx     = new TH1I("nVtx","Number of Reco Vertices;;Event",2,0,2);
  TH1I* hNBeamTrk = new TH1I("nBeamTrk","Number of Reco Beam Tracks;;Event",2,0,2);
  TH1I* hNSecTrk  = new TH1I("nSecTrk","Number of Reco Sec. Tracks;;Event",2,0,2);
  TH1D* hRecoScattAngle = new TH1D("hRecoScattAngle","Reco Scattering Angle;#theta_{reco}", 400,0.,0.05);
  TH1F* hVertexZ  = new TH1F("hVertexZ","Vertex Z; z (mm)",200,0.,1000.);
  TH2F* hVertexXY = new TH2F("hVertexXY","Vertex (x,y); x (mm); y (mm)",50,-20.,20.,50,-20.,20.);
  TH1F* hPZ_mc    = new TH1F("hPZ_mc","Reco PZ; Pz (GeV/c)",280,0.,140.);
  TH1F* hPZ_data  = new TH1F("hPZ_data","Reco PZ; Pz (GeV/c)",280,0.,140.);
  TH2D* hPzRes    = new TH2D("hPzRes",";True Pz [GeV]; Reco Pz [GeV]", 280,0.,140.,280,0.,140.);
  TH2D* hRvsP	  = new TH2D("hRvsP",";Pz [GeV]; Radii [mm]",240,0,120,40,25,65);

  // --- booking: ARICH ring ---
  TH1F* hrring = new TH1F("hrring","Ring radius;r [mm]",100,0,70);

  // --- booking: nhits (MC by species + data) ---
  TH1F* hnhits_mc_pion  = new TH1F("hnhits_mc_pion","nhits; hits",100,0,100); hnhits_mc_pion->SetLineColor(kRed+1);
  TH1F* hnhits_mc_prot  = new TH1F("hnhits_mc_prot","nhits; hits",100,0,100); hnhits_mc_prot->SetLineColor(kBlue+1);
  TH1F* hnhits_mc_elect = new TH1F("hnhits_mc_elect","nhits; hits",100,0,100); hnhits_mc_elect->SetLineColor(kGreen+1);
  TH1F* hnhits_data     = new TH1F("hnhits_data","nhits; hits",100,0,100);

  // --- booking: BACkov / GASckov charge sums (data) ---
  TH1F* h_backov_low  = new TH1F("h_backov_low","h_backov_low",100,-2,15);
  TH1F* h_backov_mid  = new TH1F("h_backov_mid","h_backov_mid",100,-2,25);
  TH1F* h_backov_high = new TH1F("h_backov_high","h_backov_high",100,-2,80);
  TH1F* h_gasckov_1   = new TH1F("h_gasckov_1","h_gasckov_1",100,-2,50);
  TH1F* h_gasckov_2   = new TH1F("h_gasckov_2","h_gasckov_2",100,-2,50);
  TH1F* h_gasckov_3   = new TH1F("h_gasckov_3","h_gasckov_3",100,-2,150);

  // --- booking: data LL-ratio graphs ---
  // naming convention: numerator+denominator, p=pion, K=kaon, P=proton
  //   pP = pion/proton, pK = pion/kaon, KP = kaon/proton
  TGraph* LL_data_pP = new TGraph(); LL_data_pP->SetName("LL_pP_data"); LL_data_pP->SetMarkerStyle(43); LL_data_pP->SetMarkerColorAlpha(kBlack,0.2);
  TGraph* LL_data_pK = new TGraph(); LL_data_pK->SetName("LL_pK_data"); LL_data_pK->SetMarkerStyle(43); LL_data_pK->SetMarkerColorAlpha(kBlack,0.2);
  TGraph* LL_data_KP = new TGraph(); LL_data_KP->SetName("LL_KP_data"); LL_data_KP->SetMarkerStyle(43); LL_data_KP->SetMarkerColorAlpha(kBlack,0.2);

  // --- booking: MC LL-ratio graphs (3 truth species each) + legends ---
  TLegend* lLLpP = new TLegend(0.75,0.2,0.85,0.4); lLLpP->SetBorderSize(0);
  TLegend* lLLpK = new TLegend(0.75,0.2,0.85,0.4); lLLpK->SetBorderSize(0);
  TLegend* lLLKP = new TLegend(0.75,0.2,0.85,0.4); lLLKP->SetBorderSize(0);
  std::vector<TGraph*> LLpP = makeLLGraphs("pP", "log(LLpion/LLprot)",  lLLpP);
  std::vector<TGraph*> LLpK = makeLLGraphs("pK", "log(LLpion/LLkaon)",  lLLpK);
  std::vector<TGraph*> LLKP = makeLLGraphs("KP", "log(LLkaon/LLprot)",  lLLKP);

  // --- confusion matrices (filled in the loop, drawn at the end) ---
  float selected_signal_ML[3][3]      = {};
  float selected_signal_LL[3][3]      = {};
  float selected_signal_ratioLL[3][3] = {};

  //eff

  TH2D *den[3], *num[3];
  for(int s=0;s<3;s++){
    den[s]=new TH2D(Form("den_%s",sp[s]),Form("%s denom;p [GeV];#theta [rad]",sp[s]),40,0,120,30,0,0.1);
    num[s]=new TH2D(Form("num_%s",sp[s]),Form("%s correct;p [GeV];#theta [rad]",sp[s]),40,0,120,30,0,0.1);
  }
 

  // ==========================================================================
  // Event loop
  // ==========================================================================
  double total_recotracks =  0;
  double total_recorings = 0;
  
  for (Long64_t i = 0; i < chain.GetEntries(); ++i)
  {
    if(i%1000 ==0) std::cout << "Event " << i << std::endl;
    chain.GetEntry(i);

    bool isMC = rec->hdr.ismc;

    // track-segment and vertex multiplicities
    hNTrkSeg->Fill(int(rec->sgmnts.seg.size()));

    int nvtx = int(rec->vtxs.vtx.size());
    total_recotracks+=nvtx;
    hNVtx->Fill(nvtx);

    int nrings = int(rec->ring.arich.size());
    total_recorings+=nrings;


    int nsectrk = 0, nbeamtrk = 0;
    if (nvtx) {
//      std::cout << "Found " << nvtx << " vertex/s " << std::endl;
      nsectrk  = int(rec->vtxs.vtx[0].sectrk.size());
      nbeamtrk = 1;
      hVertexZ->Fill(rec->vtxs.vtx[0].pos.Z());
  //    std::cout << "vertex z loc " << rec->vtxs.vtx[0].pos.Z() << std::endl;
      hVertexXY->Fill(rec->vtxs.vtx[0].pos.X(), rec->vtxs.vtx[0].pos.Y());
    }
    hNSecTrk->Fill(nsectrk);
    hNBeamTrk->Fill(nbeamtrk);


    // select clean single-secondary-track events
    if (nsectrk != 1) continue;

    for (int j = 0; j < nsectrk; ++j)
    {
      caf::SRSecondaryTrack& sectrk = rec->vtxs.vtx[0].sectrk[j];
      std::vector<double> LLs = sectrk.arich.scoresLL;
      double pz = sectrk.mom.Z();
        //std::cout << "sectrk mom = " << pz << std::endl;

      hrring->Fill(rec->ring.arich[0].radius);
 	hRvsP->Fill(pz,rec->ring.arich[0].radius);	
	auto ring = rec->ring.arich[0];

	//std::cout << "P pion from ring " << ring.P_pion << std::endl;
	//std::cout << "P kaon  from ring " <<  ring.P_kaon << std::endl;
	//std::cout << "P prot from ring " << ring.P_proton << std::endl;

      // ---- BACkov / GASckov (data only) ----
      if (!isMC) {
      //  std::cout << "BACkov info" << std::endl;
        //for (int k = 0; k < 5; ++k)std::cout << "    is " << backov_pids[k] << "? " << rec->backov.PID[k] << std::endl;
        double cum_low  = rec->backov.backovhits[0].charge + rec->backov.backovhits[1].charge + rec->backov.backovhits[2].charge;
        double cum_mid  = rec->backov.backovhits[3].charge + rec->backov.backovhits[4].charge;
        double cum_high = rec->backov.backovhits[5].charge;
        h_backov_low->Fill(cum_low);
        h_backov_mid->Fill(cum_mid);
        h_backov_high->Fill(cum_high);
       // std::cout << "  cumulative low index "  << cum_low  << std::endl;
       // std::cout << "  cumulative mid index "  << cum_mid  << std::endl;
       // std::cout << "  cumulative high index " << cum_high << std::endl;

        std::cout << "GASCkov info" << std::endl;
        for (int k = 0; k < 5; ++k)
         // std::cout << "    is " << backov_pids[k] << "? " << rec->gasckov.PID[k] << std::endl;
        h_gasckov_1->Fill(rec->gasckov.gasckovhits[0].charge);
        h_gasckov_2->Fill(rec->gasckov.gasckovhits[1].charge);
        h_gasckov_3->Fill(rec->gasckov.gasckovhits[2].charge);
      }

      // ---- LL ratios + PID picks ----
      double LL_pi_p = log(LLs[0]/LLs[2]);
      double LL_pi_K = log(LLs[0]/LLs[1]);
      double LL_p_K  = log(LLs[1]/LLs[2]);

      int argmax = std::distance(sectrk.arich.scoresML.begin(),
                                 std::max_element(sectrk.arich.scoresML.begin(), sectrk.arich.scoresML.end()));
      int argmin = std::distance(LLs.begin(),
                                 std::min_element(LLs.begin(), LLs.end()));
      int pick   = classifyPID(LL_pi_p, LL_pi_K, pz);


       //std::cout << "ML PDG PICKED "        << pdg_pos[argmax] << std::endl;
       //std::cout << "LL(argmin) PDG PICKED " << pdg_pos[argmin] << std::endl;

      // ---- fill MC vs data ----
      if (isMC) {
 	if(sectrk.truth.size() == 0)continue;
        int track_pdg = sectrk.truth[0].pdgCode;
        int truth_index = -1;
        if      (track_pdg == 211)  truth_index = 0;
        else if (track_pdg == 321)  truth_index = 1;
        else if (track_pdg == 2212) truth_index = 2;
         //std::cout << "PDG " << track_pdg << " true P " << sectrk.truth[0].mom.Z() << std::endl;

        if (truth_index >= 0) {
          selected_signal_ML[truth_index][argmax]++;
          selected_signal_LL[truth_index][argmin]++;
          selected_signal_ratioLL[truth_index][pick]++;
        }

        if (track_pdg == 211) {
          LLpP[0]->AddPoint(pz, LL_pi_p); LLpK[0]->AddPoint(pz, LL_pi_K); LLKP[0]->AddPoint(pz, LL_p_K);
          hnhits_mc_pion->Fill(rec->ring.arich[0].nhit);
        } else if (track_pdg == 321) {
          LLpP[1]->AddPoint(pz, LL_pi_p); LLpK[1]->AddPoint(pz, LL_pi_K); LLKP[1]->AddPoint(pz, LL_p_K);
        } else if (track_pdg == 2212) {
          LLpP[2]->AddPoint(pz, LL_pi_p); LLpK[2]->AddPoint(pz, LL_pi_K); LLKP[2]->AddPoint(pz, LL_p_K);
          hnhits_mc_prot->Fill(rec->ring.arich[0].nhit);
        } else if (abs(track_pdg) == 11) {
          hnhits_mc_elect->Fill(rec->ring.arich[0].nhit);
        }

        hPzRes->Fill(sectrk.truth[0].mom.Z(), pz);
        hPZ_mc->Fill(pz);


	int tS = TrueSpec(sectrk.truth);                  // truth species
    	int rS = RecoSpecLL(sectrk.arich);                // reco species (LL)

	const double p     = std::sqrt(sectrk.momTrgt.Mag2());  // recoP
        const double theta = sectrk.momTrgt.Theta();

       den[tS]->Fill(p,theta);                        // denominator = all true-species tracks
       if(rS==tS) num[tS]->Fill(p,theta);             // numerator = correctly identified
      }
      else {
        LL_data_pP->AddPoint(pz, LL_pi_p);
        LL_data_pK->AddPoint(pz, LL_pi_K);
        LL_data_KP->AddPoint(pz, LL_p_K);
        hnhits_data->Fill(rec->ring.arich[0].nhit);
        hPZ_data->Fill(pz);
      }
    }
  }
 
  //std::cout << "eff reco tracks " << total_recotracks/chain.GetEntries() << std::endl;
  //std::cout << "eff reco rings " << total_recorings/chain.GetEntries() << std::endl;
  TH2D* ML = drawConfusion("ML",      selected_signal_ML,      "ML.pdf");
  TH2D* LL = drawConfusion("LL",      selected_signal_LL,      "LL.pdf");
  TH2D* cuts = drawConfusion("ratioLL", selected_signal_ratioLL, "ratioLL.pdf");


  // ==========================================================================
  // Write histograms / graphs to the output file
  // ==========================================================================
  TFile* caf_out = new TFile(Form("%s.root", name_out.c_str()), "RECREATE");
  hPzRes->Write();
  for (int i : {0,1,2}) { LLpP[i]->Write(); LLpK[i]->Write(); LLKP[i]->Write(); }
  for(int s=0;s<3;s++){
    TH2D* eff=(TH2D*)num[s]->Clone(Form("eff_%s",sp[s]));
    eff->Divide(den[s]); eff->Write(); den[s]->Write(); num[s]->Write();
  }


  hrring->Write();
  hRvsP->Write();
  hNTrkSeg->Write();
  hNVtx->Write();
  hNBeamTrk->Write();
  hNSecTrk->Write();
  hVertexZ->Write();
  hVertexXY->Write();
  hPZ_mc->Write();
  hPZ_data->Write();
  ML->Write();
  LL->Write();
  cuts->Write();
  hnhits_mc_pion->Write();
  hnhits_mc_prot->Write();
  hnhits_data->Write();
  LL_data_pP->Write();
  LL_data_pK->Write();
  LL_data_KP->Write();
  h_backov_low->Write();
  h_backov_mid->Write();
  h_backov_high->Write();
  h_gasckov_1->Write();
  h_gasckov_2->Write();
  h_gasckov_3->Write();
  caf_out->Close();

  // ==========================================================================
  // Plots
  // ==========================================================================
  // LL-ratio scatter (MC species + data overlay)
  drawLLCanvas(LLpP, LL_data_pP, lLLpP, "LL_pion_prot.pdf");
  drawLLCanvas(LLpK, LL_data_pK, lLLpK, "LL_pion_kaon.pdf");
  drawLLCanvas(LLKP, LL_data_KP, lLLKP, "LL_kaon_prot.pdf");

  // Pz: MC vs data (area normalized)
  TCanvas* c6 = new TCanvas("", "", 800, 600);
  c6->SetRightMargin(0.15);
  TLegend* l3 = new TLegend(0.55,0.5,0.8,0.7);
  hPZ_mc->Scale(1./hPZ_mc->Integral());     hPZ_mc->SetStats(0);
  hPZ_data->Scale(1./hPZ_data->Integral()); hPZ_data->SetMarkerStyle(43);
  l3->AddEntry(hPZ_mc,   "MC",   "l");
  l3->AddEntry(hPZ_data, "data", "p");
  hPZ_mc->Draw("hist");
  hPZ_data->Draw("P hist same");
  l3->Draw("same");
  c6->SaveAs("Pz.pdf");
  delete c6;

  // nhits: MC (pions + electrons, scaled) vs data
  TCanvas* c7 = new TCanvas("", "", 800, 600);
  c7->SetRightMargin(0.15);
  TLegend* l4 = new TLegend(0.55,0.5,0.8,0.7);
  hnhits_mc_pion ->Scale(hnhits_data->Integral()*0.35/hnhits_mc_pion->Integral());  hnhits_mc_pion->SetStats(0);
  hnhits_mc_elect->Scale(hnhits_data->Integral()*0.65/hnhits_mc_elect->Integral()); hnhits_mc_elect->SetStats(0);
  hnhits_data->SetMarkerStyle(43);
  l4->AddEntry(hnhits_mc_pion,  "MC pions",     "l");
  l4->AddEntry(hnhits_mc_elect, "MC electrons", "l");
  l4->AddEntry(hnhits_data,     "data",         "p");
  hnhits_data ->Draw("P hist");
  hnhits_mc_elect->Draw("hist same");
  hnhits_mc_pion ->Draw("hist same");
  l4->Draw("same");
  c7->SaveAs("nHits.pdf");
  delete c7;
}
