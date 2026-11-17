// Draws the covariance, signed-sqrt covariance and correlation matrices of the free alignment
// parameters from the "KalmanAlign" tree written by the KalmanAlign module.
//
// Usage:
//   root -l -b -q 'plot_covariance.C("run_kalmanalign_hist.root")'
//   root -l 'plot_covariance.C("hist.root", "cov_corr.pdf")'
//   root -l -b -q 'plot_covariance.C("hist.root", "cov_corr.pdf", "cov_corr.root")'
// The optional third argument is a ROOT file that receives the histograms
// hCovariance, hSignedSqrt and hCorrelation and the canvas cCov. The tree
// name is an optional fourth argument.
//
// The middle panel shows sign(C_ij)*sqrt(|C_ij|): the diagonal is sigma_i
// (mm or rad) and off-diagonals are the signed geometric mean scale of sigma,
// so all entries are comparable in size. Dividing by sigma_i*sigma_j instead
// would give the correlation (third panel).
//
// The tree has one entry (filled in endJob). Axis labels are the parameter
// labels, e.g. "st1 pl0 sn0 Tn"; units are mm for translations and rad for Rz.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TTree.h"

namespace {
  std::string Pretty(const std::string& label)
  {
    std::string s = label;
    std::replace(s.begin(), s.end(), '_', ' ');
    return s;
  }
}

void plot_covariance(const char* fileName, const char* outName = "",
                     const char* rootOutName = "",
                     const char* treeName = "kalmanalign/KalmanAlign")
{
  TFile* file = TFile::Open(fileName);
  if (!file || file->IsZombie()) {
    std::cerr << "Cannot open " << fileName << std::endl;
    return;
  }
  TTree* tree = dynamic_cast<TTree*>(file->Get(treeName));
  if (!tree) {
    std::cerr << "Cannot find tree " << treeName << " in " << fileName
              << std::endl;
    return;
  }

  std::vector<std::string>* labels = nullptr;
  std::vector<double>* covariance = nullptr;
  std::vector<double>* correlation = nullptr;
  tree->SetBranchAddress("labels", &labels);
  tree->SetBranchAddress("covariance", &covariance);
  tree->SetBranchAddress("correlation", &correlation);
  if (tree->GetEntries() < 1) {
    std::cerr << "Tree " << treeName << " is empty" << std::endl;
    return;
  }
  tree->GetEntry(tree->GetEntries() - 1);

  const int n = static_cast<int>(labels->size());
  if (n == 0 || static_cast<int>(covariance->size()) != n * n ||
      static_cast<int>(correlation->size()) != n * n) {
    std::cerr << "Inconsistent matrix sizes (n = " << n << ")" << std::endl;
    return;
  }

  TH2D* hCov = new TH2D("hCovariance", "Covariance of alignment parameters;;",
                        n, 0, n, n, 0, n);
  TH2D* hSig = new TH2D("hSignedSqrt",
                        "sign(C_{ij})#sqrt{|C_{ij}|} (diagonal = #sigma);;",
                        n, 0, n, n, 0, n);
  TH2D* hCor = new TH2D("hCorrelation",
                        "Correlation of alignment parameters;;", n, 0, n, n,
                        0, n);
  for (int i = 0; i < n; ++i) {
    const std::string label = Pretty((*labels)[i]);
    hCov->GetXaxis()->SetBinLabel(i + 1, label.c_str());
    hCor->GetXaxis()->SetBinLabel(i + 1, label.c_str());
    hSig->GetXaxis()->SetBinLabel(i + 1, label.c_str());
    hSig->GetYaxis()->SetBinLabel(n - i, label.c_str());
    // Row 0 at the top of the plot, matching the usual matrix layout.
    hCov->GetYaxis()->SetBinLabel(n - i, label.c_str());
    hCor->GetYaxis()->SetBinLabel(n - i, label.c_str());
    for (int j = 0; j < n; ++j) {
      hCov->SetBinContent(j + 1, n - i, (*covariance)[i * n + j]);
      hCor->SetBinContent(j + 1, n - i, (*correlation)[i * n + j]);
      const double c = (*covariance)[i * n + j];
      hSig->SetBinContent(j + 1, n - i, (c < 0 ? -1. : 1.) * std::sqrt(std::abs(c)));
    }
  }
  hCor->SetMinimum(-1.);
  hCor->SetMaximum(1.);
  double maxCov = 0.;
  for (int i = 0; i < n; ++i)
    maxCov = std::max(maxCov, std::abs((*covariance)[i * n + i]));
  hCov->SetMinimum(-maxCov);
  hCov->SetMaximum(maxCov);
  hSig->SetMinimum(-std::sqrt(maxCov));
  hSig->SetMaximum(std::sqrt(maxCov));

  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);
  const int size = std::min(1400, std::max(700, 20 * n + 300));
  TCanvas* c = new TCanvas("cCov", "Alignment covariance and correlation",
                           3 * size, size);
  c->Divide(3, 1);
  for (int pad = 1; pad <= 3; ++pad) {
    c->cd(pad);
    gPad->SetLeftMargin(0.2);
    gPad->SetBottomMargin(0.2);
    gPad->SetRightMargin(0.15);
  }
  const double labelSize = std::min(0.03, 0.9 / n);
  for (TH2D* h : {hCov, hSig, hCor}) {
    h->GetXaxis()->SetLabelSize(labelSize);
    h->GetYaxis()->SetLabelSize(labelSize);
    h->GetXaxis()->LabelsOption("v");
  }
  c->cd(1);
  hCov->Draw("COLZ");
  c->cd(2);
  hSig->Draw("COLZ");
  c->cd(3);
  hCor->Draw("COLZ");
  c->Update();

  if (outName && std::string(outName) != "") c->SaveAs(outName);

  if (rootOutName && std::string(rootOutName) != "") {
    TFile* out = TFile::Open(rootOutName, "RECREATE");
    if (!out || out->IsZombie()) {
      std::cerr << "Cannot create " << rootOutName << std::endl;
      return;
    }
    // The histograms were created while the input file was the current
    // directory, so write them explicitly into the output file.
    out->cd();
    hCov->Write();
    hSig->Write();
    hCor->Write();
    c->Write();
    out->Close();
    delete out;
    std::cout << "Wrote histograms to " << rootOutName << std::endl;
  }
}
