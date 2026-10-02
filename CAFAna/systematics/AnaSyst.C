#include "StandardRecord/StandardRecord.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include "TMultiGraph.h"
#include "TChain.h"
#include "TChainElement.h"

bool IsSel(std::vector<caf::SRVertex> vtx);
void AnaSyst(std::string fname)
{
TH1::AddDirectory(false);

  TChain *chain = new TChain("recTree");

  std::ifstream inputFile(fname);
  std::string filename;

  if (inputFile.is_open()) {
    while (std::getline(inputFile, filename)) {
    std::cout << "Adding: " << filename << std::endl;
      chain->Add(filename.c_str());
    }
    inputFile.close();
  }
  else {
    std::cerr << "Error opening " << fname << std::endl;
  }

std::cout << "Files: " << chain->GetListOfFiles()->GetEntries() << std::endl;
std::cout << "Entries: " << chain->GetEntries() << std::endl;

  caf::StandardRecord* rec = 0;
  chain->SetBranchAddress("rec", &rec);

  TObjArray *fileElements=chain->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl=0;
  int j=0;
  TH1F* hScatteringAll = new TH1F("hScatteringAll","hScatteringAll",100,0.0002,0.0003); //,0.,0.0005);
  TH2F* hScatteringSpread = new TH2F("hScatteringSpread","hScatteringSpread",18,0,0.018,200000,0,200000);
  TH2F* hScatteringSpreadRelative = new TH2F("hScatteringSpreadRelative","hScatteringSpreadRelative",18,0,0.018,100,0,1);

  std::vector<TH1F*> hScatteringUniverses;
  //std::vector<TH1F*> hScatteringBins;

  while (( chEl=(TChainElement*)next() )) {
    TFile* f = new TFile(chEl->GetTitle(),"UPDATE");
    TTree* tree = (TTree*)f->Get(chain->GetName());

    caf::StandardRecord* rec = 0;
    tree->SetBranchAddress("rec", &rec);

    int nentries  = tree->GetEntries();

    TH1F* hScatteringTemp = new TH1F("hScatteringTemp","hScatteringTemp",180,0.,0.018);
    TH1F* hScatteringTemp2 = new TH1F("hScatteringTemp2","hScatteringTemp2",18,0.,0.018);
    TH1F* hXZ = new TH1F("hXZ","hXZ",180,-0.018,0.018);
    TH1F* hYZ = new TH1F("hYZ","hYZ",180,-0.018,0.018);

    for(int i=0;i<nentries;i++){
      tree->GetEntry(i);

      bool useEvent = false;
      if (rec->evtqual.hasssdhits && rec->evtqual.hast0trb3 && rec->evtqual.hast0caen) useEvent = true;
      if (!useEvent) continue;

      // First create Scattering for each file
      if (IsSel(rec->vtxs.vtx)){
        // Track
        caf::SRTrack& trk0 = rec->vtxs.vtx[0].beamtrk;
        caf::SRTrack& trk1 = rec->vtxs.vtx[0].sectrk[0];

        // Track segments
        caf::SRTrackSegment& ts1 = trk0.sgmnt[0];
        caf::SRTrackSegment& ts2 = trk1.sgmnt[0];
        caf::SRTrackSegment& ts3 = trk1.sgmnt[1];

        ROOT::Math::XYZVector ts1v((ts1.pointB.X() - ts1.pointA.X()), (ts1.pointB.Y() - ts1.pointA.Y()), (ts1.pointB.Z() - ts1.pointA.Z()));
        ROOT::Math::XYZVector ts2v((ts2.pointB.X() - ts2.pointA.X()), (ts2.pointB.Y() - ts2.pointA.Y()), (ts2.pointB.Z() - ts2.pointA.Z()));

        double acos = ts2v.Unit().Dot(ts1v.Unit());
        acos = TMath::Min(TMath::Max(acos, -1.), 1.);
        double recoScatteringSel = TMath::ACos(acos);

        hScatteringTemp->Fill(recoScatteringSel);
	hScatteringTemp2->Fill(recoScatteringSel);

        //TH1F* hUniverse = (TH1F*)hScatteringTemp2->Clone(Form("hScatteringUniverse_%zu",hScatteringUniverses.size()));
        //hScatteringUniverses.push_back(hUniverse);

        ROOT::Math::XYZVector u1 = ts1v.Unit();
        ROOT::Math::XYZVector u2 = ts2v.Unit();
        double thetaXZ = std::atan2(u2.X(), u2.Z()) - std::atan2(u1.X(), u1.Z());
        double thetaXZ2 = thetaXZ = std::atan2(std::sin(thetaXZ), std::cos(thetaXZ));
        
        double thetaYZ = std::atan2(u2.Y(), u2.Z()) - std::atan2(u1.Y(), u1.Z());
        double thetaYZ2 = std::atan2(std::sin(thetaYZ), std::cos(thetaYZ));

	hXZ->Fill(thetaXZ2);
        hYZ->Fill(thetaYZ2);
	//std::cout<<"hXY, hYZ = "<<thetaXZ2<<", "<<thetaYZ2<<std::endl;
      }    
    } //Entries 

        TH1F* hUniverse = (TH1F*)hScatteringTemp2->Clone(Form("hScatteringUniverse_%zu",hScatteringUniverses.size()));
        hScatteringUniverses.push_back(hUniverse);

    // Get mean and fill hScatteringAll
    Double_t p1 = hScatteringTemp->GetMean(1);
    Double_t txz = hXZ->GetMean(1);
    Double_t tyz = hYZ->GetMean(1);
    std::cout<< std::fixed << setprecision(15) << "Univ "<<j<<": Mean is "<<p1<<std::endl;
    std::cout<<"Theta_xz, Theta_yz = "<<txz<<", "<<tyz<<std::endl;
    hScatteringAll->Fill(p1);

    // Loop through hScatteringTemp bins
    // Get bin and content and plot in hScatteringSpread
    hScatteringTemp2->Sumw2();
    //hScatteringTemp2->Scale(1.0 / hScatteringTemp2->Integral("width"));
    for (int i=1; i<hScatteringTemp2->GetNbinsX()+1; i++){
      double x_i = hScatteringTemp2->GetBinCenter(i);
      double N_i = hScatteringTemp2->GetBinContent(i);
      std::cout<<"x_i,N_i = "<<x_i<<", "<<N_i<<std::endl;
      //std::cout<<"x_i,N_i,txz_i,tyz_i = "<<x_i<<", "<<N_i<<", "<<txz<<", "<<tyz<<std::endl;
      hScatteringSpread->Fill(x_i,N_i);
    }

    // is this okay?
    /*
    for (int i=1; i<hScatteringSpread->GetNbinsX()+1; i++){
      double e_i = hScatteringSpread->ProfileX("pfx",1,-1,"RMS")->GetBinError(i);
      double N_i = hScatteringSpread->ProfileX("pfx",1,-1,"RMS")->GetBinContent(i);
      double c_i = hScatteringSpread->ProfileX("pfx",1,-1,"RMS")->GetBinCenter(i);
      hScatteringSpreadRelative->Fill(c_i,e_i/N_i);
    }
*/
    j++;
    f->Close();
  }

  std::cout<<"Okay done iterating"<<std::endl;

  // Write out all universes?

  const int nUniverses = hScatteringUniverses.size();
  int nBins = hScatteringUniverses[0]->GetNbinsX();

std::vector<TGraph*> gBinUniverses;

for (int iBin = 1; iBin <= nBins; ++iBin) {

  TGraph* g = new TGraph();
  g->SetName(Form("gBinUniverses_%d", iBin));
  g->SetTitle(Form("Bin %d;Universe;Bin content", iBin));

  for (int iUni = 0; iUni < nUniverses; ++iUni) {

    double value = hScatteringUniverses[iUni]->GetBinContent(iBin);

    g->SetPoint(iUni, iUni, value);
  }

  g->SetMarkerStyle(20);
  g->SetMarkerSize(0.8);
  g->SetLineColor(kBlue);
  g->SetMarkerColor(kBlue);

  gBinUniverses.push_back(g);
}

  TH1F* hMean = (TH1F*)hScatteringUniverses[0]->Clone("hMean");
  TH1F* hRMS = (TH1F*)hScatteringUniverses[0]->Clone("hRMS");
  TH1F* hFrac = (TH1F*)hScatteringUniverses[0]->Clone("hFrac");

  hMean->Reset();
  hRMS->Reset();
  hFrac->Reset();

  for (int iBin = 1; iBin <= nBins; ++iBin) {
    double sum = 0.0;
    double sumSq = 0.0;

    // Loop over universes
    for (int iUni = 0; iUni < nUniverses; ++iUni) {
      double N = hScatteringUniverses[iUni]->GetBinContent(iBin);
      sum   += N;
      sumSq += N * N;
    }

    // Mean yield
    double mean = sum / nUniverses;

    // Sample variance
    double variance = (sumSq - nUniverses * mean * mean) / (nUniverses - 1);
    variance = std::max(0.0, variance);

    double rms = std::sqrt(variance);

    // Fractional systematic uncertainty
    double frac = 0.0;

    if (mean > 0.0) frac = rms / mean;

    hMean->SetBinContent(iBin, mean);
    hRMS->SetBinContent(iBin, rms);
    hFrac->SetBinContent(iBin, frac);
  }

  hFrac->SetTitle("Survey systematic uncertainty");
  hFrac->GetXaxis()->SetTitle("Scattering angle [rad]");
  hFrac->GetYaxis()->SetTitle("#sigma_{universe}/#bar{N}");

  hFrac->SetMinimum(0);
  hFrac->Draw("HIST");

  double v_c[18];
  double v_v[18];

    for (int i=1; i<hScatteringSpread->GetNbinsX()+1; i++){
      double c_i = hScatteringSpread->GetXaxis()->GetBinCenter(i); //ProfileX("pfx",1,-1,"RMS")->GetBinCenter(i);
      double e_i = hScatteringSpread->ProfileX("pfx",1,-1,"s")->GetBinError(i);
//      double e_i = hScatteringSpread->ProfileX("pfx",1,-1,"RMS")->GetBinError(i);
      //double N_i = hScatteringSpread->ProfileX("pfx",1,-1,"RMS")->GetBinContent(i);
      //TProfile *pMean = hScatteringSpread->ProfileX("pMean", 1, -1);
      double N_i = hScatteringSpread->ProfileX("pMean", 1, -1)->GetBinContent(i);

      v_c[i] = c_i;
      v_v[i] = e_i/N_i;
      //double c_i = hScatteringSpread->GetBinCenter(i); //ProfileX("pfx",1,-1,"RMS")->GetBinCenter(i);
      //std::cout<<c_i<<" = "<<e_i/N_i<<std::endl;
      //std::cout<<"x_i, N_i, e_i, e_i/N_i = "<<c_i<<", "<<N_i<<", "<<e_i<<", "<<e_i/N_i<<std::endl;
      //std::cout<<v_c[i]<<", "<<v_v[i]<<std::endl;
      hScatteringSpreadRelative->Fill(c_i,e_i/N_i);
    }
  //hScatteringSpreadRelative->SetMarkerStyle(20);
  TGraph* gScatteringSpreadRelative = new TGraph(18,v_c,v_v);
  gScatteringSpreadRelative->SetMarkerStyle(20);

  TFile* caf_out = new TFile("systAna.root","RECREATE");  
  hScatteringAll->Write();
  hScatteringSpread->Write();
  hScatteringSpreadRelative->Write();
  //gScatteringSpreadRelative->Write();
  hFrac->Write();

for (int iBin = 0; iBin < nBins; ++iBin) {
  gBinUniverses[iBin]->Write();
}

  caf_out->Close();

}
bool IsSel(std::vector<caf::SRVertex> vtx)
{
  bool sigsel = false;
  if (vtx.size() == 1 && vtx[0].nsectrk == 1) sigsel = true;
  return sigsel;}
