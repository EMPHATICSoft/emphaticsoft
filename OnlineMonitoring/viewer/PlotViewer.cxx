///
/// \file    PlotViewer.cxx
/// \brief   Canvas on which reports are drawn
/// \version $Id: PlotViewer.cxx,v 1.40 2012-12-12 17:04:13 messier Exp $
/// \author  messier@indiana.edu
///
#include "OnlineMonitoring/viewer/PlotViewer.h"
#include <iostream>
#include "TMath.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TH1F.h"
#include "TH2F.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/TickerSubscriber.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/HistoSource.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/viewer/GUIModelData.h"
#include "OnlineMonitoring/viewer/PlotClickHandler.h"
#include "OnlineMonitoring/viewer/ComparisonOptions.h"
#include "OnlineMonitoring/viewer/OStream.h"
using namespace emph::onmon;

// ROOT events must be handled globally so I need to provide some
// global access to the plot viewer
static PlotViewer* gsPlotViewer = 0;

//......................................................................

PlotViewer::PlotViewer(TGWindow* m) :
  TRootEmbeddedCanvas("onmon_report",
		      m,
		      Layout::fPlotViewerSizeX,
		      Layout::fPlotViewerSizeY),
  GUIModelSubscriber(kCurrentHistogramID|kPlotViewRefreshID|kComparisonUpdateID|kPrintID)
{
  //
  // Send global root mouse events to this PlotViewer
  //
  gsPlotViewer = this;

  this->GetCanvas()->SetRightMargin(0.12);
  this->GetCanvas()->Draw();
  this->GetCanvas()->AddExec("ex1","emph::onmon::PlotViewer::Exec()");

  this->Resize();
}

//......................................................................

void PlotViewer::Exec()
{
  int evti = gPad->GetEvent();
  bool single_click = (evti==1);  // Single click mouse down
  bool double_click = (evti==61); // Double click mouse down
  if (!(single_click||double_click)) return;

  static PlotClickHandler handler;
  if (single_click) {
    handler.SingleClick(gsPlotViewer->fPlotOpt);
  }
  if (double_click) {
    handler.DoubleClick(gsPlotViewer->fPlotOpt);
  }
}

//......................................................................

void PlotViewer::Update()
{
  if (fHistogramSource =="") return;
  if (fCurrentHistogram=="") return;

  HistoSource f(fHistogramSource.c_str());

  const HistoData* hd =
    HistoTable::Instance().LookUp(fCurrentHistogram.c_str());
  if (hd==0) {
    emph::onmon::cout << "ERROR:  Could not get histogram data "
	     << fCurrentHistogram
	     << "";
    return;
  }

  //
  // Parse out the options for how the plot should be displayed
  //
  fPlotOpt.Set(hd->fOption);

  //
  // Check if we want a content projection
  //
  const GUIModelData& model = GUIModel::Instance().Data();
  if (model.fContentProjection==1) {

    if (hd->fType==kTH1F) {
      TH1F* h = f.GetTH1FCopy(fCurrentHistogram.c_str());
      this->ContentProjectionTH1F(hd,h);
      delete h;
    }

    if (hd->fType==kTH2F) {
      TH2F* h = f.GetTH2FCopy(fCurrentHistogram.c_str());
      this->ContentProjectionTH2F(hd,h);
      delete h;
    }

    return;
  }

  // If "alert" mode = true, set the color pad for this mode.
  if(fPlotOpt.fAlert) {
    Int_t palette[2];
    palette[0] = 3;
    palette[1] = 2;
    gStyle->SetPalette(2,palette);
  }
  else gStyle->SetPalette(1);

  this->GetCanvas()->cd();
  this->GetCanvas()->Clear();
  fPlotOpt.SetPad(this->GetCanvas());

  //
  // Plot 1D histograms
  //
  if (hd->fType==kTH1F) {
    TH1F* h = f.GetTH1FCopy(fCurrentHistogram.c_str());
    if (h==0) {
      emph::onmon::cout << "ERROR:  Could not get copy of histogram "
	       << fCurrentHistogram
	       << "";
      return;
    }

    fPlotOpt.AutoScale(h);
    h->SetLineColor(kBlack);
    h->SetFillColor(kGreen-10);
    h->DrawCopy(fPlotOpt.fDrawOpt.c_str());
    fPlotOpt.MakeLabels(h, hd);
    this->GetCanvas()->Update();

    delete h;
  }

  //
  // Plot 2D histograms
  //
  if (hd->fType==kTH2F) {
    TH2F* h = f.GetTH2FCopy(fCurrentHistogram.c_str());
    if (h==0) {
      emph::onmon::cout << "ERROR:  Could not get copy of histogram "
	       << fCurrentHistogram
	       << "";
      return;
    }

    fPlotOpt.AutoScale(h);
    h->DrawCopy(fPlotOpt.fDrawOpt.c_str());
    fPlotOpt.MakeLabels(h, hd);
    this->GetCanvas()->Update();

    delete h;
  }
}

//......................................................................

void PlotViewer::UpdateCompare(const ComparisonOptions& COpt)
{
  if (fHistogramSource =="") {
    emph::onmon::cout << "ERROR:  Histogram Source not defined"
	     << "";
    return;
  }
  if (fCurrentHistogram=="") {
    emph::onmon::cout << "ERROR:  Current Histogram not defined"
	     << "";
    return;
  }

  if(COpt.fWhich == ComparisonOptions::kNone) {
    this->Update();
    return;
  }

  // Define the sources for the histograms to be compared.
  HistoSource f1(fHistogramSource.c_str());
  HistoSource f2(COpt.fReferenceFile.c_str());

  const HistoData* hd =
    HistoTable::Instance().LookUp(fCurrentHistogram.c_str());
  if (hd==0) return;

  //
  // Parse out the options for how the plot should be displayed and reset the
  // palette in case the previous histogram was an alert histogram.
  //
  fPlotOpt.Set(hd->fOption);
  gStyle->SetPalette(1);

  this->GetCanvas()->cd();
  this->GetCanvas()->Clear();
  fPlotOpt.SetPad(this->GetCanvas());

  //
  // Plot 1D histograms
  //
  if (hd->fType==kTH1F) {

    // POSSIBLY MOVE THIS MAKING THE H2 NAME SECTION OUTSIDE BOTH THE
    // TH1F AND TH2F IF STATEMENTS

    // Create the name string for the comparison histo.  If we are comparing to
    // a reference file, just use the same histo name.  Otherwise, add to the
    // histo name things like "_prev_10min_02".
    std::string CompareHistoName = fCurrentHistogram;
    if(COpt.fWhich == ComparisonOptions::kRecent) {
      CompareHistoName += "_prev_";
      const char* tag = 0;
      switch (hd->fReset) {
      case TickerSubscriber::kRun:    tag = "run";    break;
      case TickerSubscriber::kSubrun: tag = "subrun"; break;
      case TickerSubscriber::k5min:   tag = "5min";   break;
      case TickerSubscriber::k10min:  tag = "10min";  break;
      case TickerSubscriber::k30min:  tag = "30min";  break;
      default: abort();
      }
      CompareHistoName += tag;

      char LB[64];
      sprintf(LB, "_%.2d", COpt.fLookBack);
      CompareHistoName += LB;
    }

    // Get the current histo and rename it.  Otherwise ROOT will have major
    // problems with two histos that have the same name.
    TH1F* h1 = f1.GetTH1FCopy(fCurrentHistogram.c_str());
    if(h1 == 0) {
      emph::onmon::cout << "ERROR:  Could not get copy of current histogram "
	       << fCurrentHistogram
	       << "";
      return;
    }
    h1->SetName("ComparisonHistogram");

    TH1F* h2 = 0;
    if(COpt.fWhich == ComparisonOptions::kRecent)
      h2 = f1.GetTH1FCopy(CompareHistoName.c_str());
    if(COpt.fWhich == ComparisonOptions::kReference)
      h2 = f2.GetTH1FCopy(CompareHistoName.c_str());
    if(h2 == 0) {
      emph::onmon::cout << "ERROR:  Could not get copy of reference histogram "
	       << CompareHistoName
	       << "";
      delete h1;
      return;
    }

    // Calculate the requested normalization (if different from "Absolute") and
    // rescale the reference histo.
    double scale = 1.0;

    if(COpt.fNormalize == ComparisonOptions::kArea) {
      double area1 = h1->Integral();
      double area2 = h2->Integral();
      scale = area1/area2;
      h2->Scale(scale);
    }
    if(COpt.fNormalize == ComparisonOptions::kPeak) {
      unsigned int NXbins = h1->GetNbinsX();
      double bin1;
      double bin2;
      double peak1 = -1.0e9;
      double peak2 = -1.0e9;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	bin1 = h1->GetBinContent(i);
	bin2 = h2->GetBinContent(i);
	if(bin1 > peak1) peak1 = bin1;
	if(bin2 > peak2) peak2 = bin2;
      }

      scale = peak1/peak2;
      h2->Scale(scale);

    }
    /*
    if(COpt.fNormalize == ComparisonOptions::kIntegral) {
    // THIS OPTION IS NOT CURRENTLY IMPLEMENTED.
      double area1 = h1->Integral();
      double area2 = h2->Integral();
      scale = area1/area2;
      h2->Scale(scale);
    }
    */



    if(COpt.fMethod == ComparisonOptions::kOverlay) {
      h1->SetTitle("Overlay Plot: (black = current histo, red = comparison histo)");
      fPlotOpt.AutoScale(h1);
      h1->SetLineColor(kBlack);
      h2->SetLineColor(kRed);

      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());
      // h2->DrawCopy("same");
      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }
    if(COpt.fMethod == ComparisonOptions::kDifference) {

      // (see note in difference section for TH2Fs...)

      unsigned int NXbins = h1->GetNbinsX();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	bin1 = h1->GetBinContent(i);
	bin2 = h2->GetBinContent(i);
	fill = bin1 - bin2;
	err  = sqrt(bin1+bin2);
	h1->SetBinContent(i,fill);
	h1->SetBinError(i,err);
      }

      fPlotOpt.fLogy = false;
      fPlotOpt.SetPad(this->GetCanvas());

      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Difference Comparison: (current - reference)");

      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }
    if(COpt.fMethod == ComparisonOptions::kRatio) {

      // (see note in difference section for TH2Fs...)

      unsigned int NXbins = h1->GetNbinsX();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	bin1 = h1->GetBinContent(i);
	bin2 = h2->GetBinContent(i);
	if(bin2 == 0.0) fill = 0.0;
	else fill = bin1/bin2;
	err = sqrt(bin1+bin2);
	h1->SetBinContent(i,fill);
	h1->SetBinError(i,err);
      }

      fPlotOpt.fLogy = false;
      fPlotOpt.SetPad(this->GetCanvas());

      h1->SetTitle("Ratio Comparison: (current/reference)");
      // fPlotOpt.AutoScale(h1);
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();

    }
    if(COpt.fMethod == ComparisonOptions::kAsymmetry) {
      unsigned int NXbins = h1->GetNbinsX();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	bin1 = h1->GetBinContent(i);
	bin2 = h2->GetBinContent(i);
	if(bin1 == 0.0 && bin2 == 0.0) fill = 0.0;
	else fill = (bin1-bin2)/(bin1+bin2);
	err = sqrt(bin1+bin2);
	h1->SetBinContent(i,fill);
	h1->SetBinError(i,err);
      }
      fPlotOpt.fLogy = false;
      fPlotOpt.SetPad(this->GetCanvas());

      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Asymmetry Comparison: (current - reference)/(current + reference)");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();

    }
    if(COpt.fMethod == ComparisonOptions::kChi) {
      unsigned int NXbins = h1->GetNbinsX();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	bin1 = h1->GetBinContent(i);
	bin2 = h2->GetBinContent(i);
	if(bin2 == 0.0) fill = 0.0;
	else fill = (bin1-bin2)/sqrt(bin2);
	err = sqrt(bin1+bin2);
	h1->SetBinContent(i,fill);
	h1->SetBinError(i,err);
      }

      fPlotOpt.fLogy = false;
      fPlotOpt.SetPad(this->GetCanvas());

      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Chi Comparison: (current - reference)/Sqrt(reference)");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();

    }
    if(COpt.fMethod == ComparisonOptions::kShowRef) {

      unsigned int NXbins = h1->GetNbinsX();

      for(unsigned int i = 1; i <= NXbins; ++i) {
	  h1->SetBinContent(i,h2->GetBinContent(i));
      }

      fPlotOpt.SetPad(this->GetCanvas());

      fPlotOpt.AutoScale(h1);
      h1->SetTitle("Reference Histogram");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }

    //
    // Check if we want a content projection
    //
    const GUIModelData& model = GUIModel::Instance().Data();
    if (model.fContentProjection==1)
      this->ContentProjectionTH1F(hd,h1);
    else {
      h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());
      if(COpt.fMethod == ComparisonOptions::kOverlay)
	h2->DrawCopy("same");
      fPlotOpt.MakeLabels(h1, hd);
      this->GetCanvas()->Update();
    }

    delete h1;
    delete h2;

  }



  //
  // Plot 2D histograms
  //
  if (hd->fType==kTH2F) {

    // Create the name string for the comparison histo.  If we are comparing to
    // a reference file, just use the same histo name.  Otherwise, add to the
    // histo name things like "_prev_10min_02".
    std::string CompareHistoName = fCurrentHistogram;
    if(COpt.fWhich == ComparisonOptions::kRecent) {
      CompareHistoName += "_prev_";
      const char* tag = 0;
      switch (hd->fReset) {
      case TickerSubscriber::kRun:    tag = "run";    break;
      case TickerSubscriber::kSubrun: tag = "subrun"; break;
      case TickerSubscriber::k5min:   tag = "5min";   break;
      case TickerSubscriber::k10min:  tag = "10min";  break;
      case TickerSubscriber::k30min:  tag = "30min";  break;
      default: abort();
      }
      CompareHistoName += tag;

      char LB[64];
      sprintf(LB, "_%.2d", COpt.fLookBack);
      CompareHistoName += LB;
    }

    // Get the current histo and rename it.  Otherwise ROOT will have major
    // problems with two histos that have the same name.
    TH2F* h1 = f1.GetTH2FCopy(fCurrentHistogram.c_str());
    if(h1 == 0) {
      emph::onmon::cout << "ERROR:  Could not get copy of current histogram "
	       << fCurrentHistogram
	       << "";
      return;
    }
    h1->SetName("ComparisonHistogram");

    TH2F* h2 = 0;
    if(COpt.fWhich == ComparisonOptions::kRecent)
      h2 = f1.GetTH2FCopy(CompareHistoName.c_str());
    if(COpt.fWhich == ComparisonOptions::kReference)
      h2 = f2.GetTH2FCopy(CompareHistoName.c_str());
    if(h2 == 0) {
      emph::onmon::cout << "ERROR:  Could not get copy of reference histogram "
	       << CompareHistoName
	       << "";
      delete h1;
      return;
    }

    // Calculate the requested normalization (if different from "Absolute") and
    // rescale the reference histo.
    double scale = 1.0;

    if(COpt.fNormalize == ComparisonOptions::kArea) {
      double area1 = h1->Integral();
      double area2 = h2->Integral();
      scale = area1/area2;
      h2->Scale(scale);
    }
    if(COpt.fNormalize == ComparisonOptions::kPeak) {
      unsigned int NXbins = h1->GetNbinsX();
      unsigned int NYbins = h1->GetNbinsY();
      double bin1;
      double bin2;
      double peak1 = -1.0e9;
      double peak2 = -1.0e9;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	for(unsigned int j = 1; j <= NYbins; ++j) {
	  bin1 = h1->GetBinContent(i,j);
	  bin2 = h2->GetBinContent(i,j);
	  if(bin1 > peak1) peak1 = bin1;
	  if(bin2 > peak2) peak2 = bin2;
	}
      }

      scale = peak1/peak2;
      h2->Scale(scale);

      }
    /*
    if(COpt.fNormalize == ComparisonOptions::kIntegral) {
      // THIS OPTION IS NOT CURRENTLY IMPLEMENTED.
      double area1 = h1->Integral();
      double area2 = h2->Integral();
      scale = area1/area2;
      h2->Scale(scale);
    }
    */

    if(COpt.fMethod == ComparisonOptions::kOverlay) {

      h2->GetXaxis()->SetAxisColor(0);
      h2->GetYaxis()->SetAxisColor(0);
      h2->GetXaxis()->SetLabelColor(0);
      h2->GetYaxis()->SetLabelColor(0);
      h2->GetXaxis()->SetTickLength(0);
      h2->GetYaxis()->SetTickLength(0);

      fPlotOpt.AutoScale(h1);
      h2->SetLineWidth(4);
      h2->SetLineColor(kRed);

      h1->SetTitle("Overlay Plot: (colz = current histo, box = comparison histo)");

      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());
      // h2->DrawCopy("box:same");
      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }

    if(COpt.fMethod == ComparisonOptions::kDifference) {

      // NOTE:  You may be thinking that I could have just used
      //        hComp->Add(hComp,h2,1.0,-1.0) instead of adding the
      //        two histos bin by bin.  BUT I discovered a serious
      //        flaw in Root that causes the drawing of the resulting
      //        histo to be blank.  This is a Root problem and I'm not
      //        going to explain it here, but adding bin by bin is
      //        currently the only way to do this correctly.

      unsigned int NXbins = h1->GetNbinsX();
      unsigned int NYbins = h1->GetNbinsY();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	for(unsigned int j = 1; j <= NYbins; ++j) {
	  bin1 = h1->GetBinContent(i,j);
	  bin2 = h2->GetBinContent(i,j);
	  fill = bin1 - bin2;
	  err  = sqrt(bin1+bin2);
	  h1->SetBinContent(i,j,fill);
	  h1->SetBinError(i,j,err);
	}
      }
      fPlotOpt.fLogz = false;
      fPlotOpt.SetPad(this->GetCanvas());

      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Difference Comparison: (current - reference)");
      // h1->DrawCopy("colz");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }
    if(COpt.fMethod == ComparisonOptions::kRatio) {

      // (See the long note about 20 lines up...)
      unsigned int NXbins = h1->GetNbinsX();
      unsigned int NYbins = h1->GetNbinsY();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	for(unsigned int j = 1; j <= NYbins; ++j) {
	  bin1 = h1->GetBinContent(i,j);
	  bin2 = h2->GetBinContent(i,j);
	  if(bin2 == 0.0) fill = 0.0;
	  else fill = bin1/bin2;
	  err = sqrt(bin1+bin2);
	  h1->SetBinContent(i,j,fill);
	  h1->SetBinError(i,j,err);
	}
      }

      fPlotOpt.fLogz = false;
      fPlotOpt.SetPad(this->GetCanvas());
      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Ratio Comparison: (current/reference)");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());
      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }
    if(COpt.fMethod == ComparisonOptions::kAsymmetry) {
      unsigned int NXbins = h1->GetNbinsX();
      unsigned int NYbins = h1->GetNbinsY();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	for(unsigned int j = 1; j <= NYbins; ++j) {
	  bin1 = h1->GetBinContent(i,j);
	  bin2 = h2->GetBinContent(i,j);
	  if(bin1 == 0.0 && bin2 == 0.0) fill = 0.0;
	  else fill = (bin1-bin2)/(bin1+bin2);
	  err = sqrt(bin1+bin2);
	  h1->SetBinContent(i,j,fill);
	  h1->SetBinError(i,j,err);
	}
      }
      fPlotOpt.fLogz = false;
      fPlotOpt.SetPad(this->GetCanvas());

      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Asymmetry Comparison: (current - reference)/(current + reference)");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();

    }
    if(COpt.fMethod == ComparisonOptions::kChi) {
      unsigned int NXbins = h1->GetNbinsX();
      unsigned int NYbins = h1->GetNbinsY();
      double bin1;
      double bin2;
      double fill = 0.0;
      double err  = 0.0;

      for(unsigned int i = 1; i <= NXbins; ++i) {
	for(unsigned int j = 1; j <= NYbins; ++j) {
	  bin1 = h1->GetBinContent(i,j);
	  bin2 = h2->GetBinContent(i,j);
	  if(bin2 == 0.0) fill = 0.0;
	  else fill = (bin1-bin2)/sqrt(bin2);
	  err = sqrt(bin1+bin2);
	  h1->SetBinContent(i,j,fill);
	  h1->SetBinError(i,j,err);
	}
      }

      fPlotOpt.fLogz = false;
      fPlotOpt.SetPad(this->GetCanvas());

      // fPlotOpt.AutoScale(h1);
      h1->SetTitle("Chi Comparison: (current - reference)/Sqrt(reference)");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();

    }
    if(COpt.fMethod == ComparisonOptions::kShowRef) {

      unsigned int NXbins = h1->GetNbinsX();
      unsigned int NYbins = h1->GetNbinsY();

      for(unsigned int i = 1; i <= NXbins; ++i) {
	for(unsigned int j = 1; j <= NYbins; ++j) {
	  h1->SetBinContent(i,j,h2->GetBinContent(i,j));
	}
      }

      fPlotOpt.SetPad(this->GetCanvas());

      fPlotOpt.AutoScale(h1);
      h1->SetTitle("Reference Histogram");
      // h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());

      // fPlotOpt.MakeLabels(h1, hd);
      // this->GetCanvas()->Update();
    }

    //
    // Check if we want a content projection
    //
    const GUIModelData& model = GUIModel::Instance().Data();
    if (model.fContentProjection==1)
      this->ContentProjectionTH2F(hd,h1);
    else {
      h1->DrawCopy(fPlotOpt.fDrawOpt.c_str());
      if(COpt.fMethod == ComparisonOptions::kOverlay)
	h2->DrawCopy("box:same");
      fPlotOpt.MakeLabels(h1, hd);
      this->GetCanvas()->Update();
    }

    delete h1;
    delete h2;

  }
}

//......................................................................

void PlotViewer::ContentProjectionTH1F(const HistoData* hd, TH1F* h)
{

  HistoSource f(fHistogramSource.c_str());

  TH1F* htmp = 0;

  this->GetCanvas()->cd();
  this->GetCanvas()->Clear();

  if (h==0) {
    emph::onmon::cout << "ERROR:  Could not get copy of histogram "
	     << fCurrentHistogram
	     << "";
    return;
  }

  bool dolog = fPlotOpt.fLogy;

  double mx = h->GetMaximum();
  double mn = h->GetMinimum();

  if(mx > 0.0) mx = 1.3*mx;
  else         mx = 0.7*mx;
  if(mn > 0.0) mn = 0.7*mn;
  else         mn = 1.3*mn;

  if (dolog) {
    if (mx>0.0) mx = log10(mx);
    else        mx = 0;
    if (mn>0.0) mn = log10(mn);
    else        mn = 0;
  }

  if(mx == mn) {
    mx += 1.0;
    mn -= 1.0;
  }

  char buff[256];
  if (dolog) {
    sprintf(buff, "%s;log10(content);number of bins",hd->fName.c_str());
  }
  else {
    sprintf(buff, "%s;content;number of bins",hd->fName.c_str());
  }
  htmp = new TH1F("htmp",buff,100,mn,mx);
  htmp->SetLineColor(kBlack);
  htmp->SetFillColor(kGreen-10);

  int i;
  for (i=1; i<=h->GetNbinsX(); ++i) {
    double c = h->GetBinContent(i);
    double e = h->GetBinError(i);
    if (c > 0.0)
      if (dolog) c = log10(c);

    // NOTE: If the bin content is zero but the error is NOT zero,
    // then we do NOT want to supress this zero.  This is to distinguish
    // an empty bin from a bin filled with a value that was computed to
    // be zero from at least one non-zero value.
    if (c != 0.0 || e != 0.0)
      htmp->Fill(c);
  }
  htmp->DrawCopy();
  this->GetCanvas()->Update();

  delete htmp;

}

//......................................................................

void PlotViewer::ContentProjectionTH2F(const HistoData* hd, TH2F* h)
{

  HistoSource f(fHistogramSource.c_str());

  TH1F* htmp = 0;

  this->GetCanvas()->cd();
  this->GetCanvas()->Clear();

  if (h==0) {
    emph::onmon::cout << "ERROR:  Could not get copy of histogram "
	     << fCurrentHistogram
	     << "";
    return;
  }

  bool dolog = fPlotOpt.fLogz;

  double mx = h->GetMaximum();
  double mn = h->GetMinimum();

  if(mx > 0.0) mx = 1.3*mx;
  else         mx = 0.7*mx;
  if(mn > 0.0) mn = 0.7*mn;
  else         mn = 1.3*mn;

  if (dolog) {
    if (mx>0.0) mx = log10(mx);
    else        mx = 0;
    if (mn>0.0) mn = log10(mn);
    else        mn = 0;
  }

  if(mx == mn) {
    mx += 1.0;
    mn -= 1.0;
  }

  char buff[256];
  if (dolog) {
    sprintf(buff, "%s;log10(content);number of bins",hd->fName.c_str());
  }
  else {
    sprintf(buff, "%s;content;number of bins",hd->fName.c_str());
  }

  int nBins = abs(mx - mn);

  htmp = new TH1F("htmp",buff,nBins,mn,mx);
  htmp->SetFillColor(kGreen-10);

  int i, j;
  for (i=1; i<=h->GetNbinsX(); ++i) {
    for (j=1; j<=h->GetNbinsY(); ++j) {
      double c = h->GetBinContent(i,j);
      double e = h->GetBinError(i,j);
      if (c > 0.0)
	if (dolog) c = log10(c);

      // NOTE: If the bin content is zero but the error is NOT zero,
      // then we do NOT want to supress this zero.  This is to distinguish
      // an empty bin from a bin filled with a value that was computed to
      // be zero from at least one non-zero value.
      if (c != 0.0 || e != 0.0)
	htmp->Fill(c);
    }
  }
  htmp->DrawCopy();
  this->GetCanvas()->Update();

  delete htmp;

}

//......................................................................

void PlotViewer::GUIModelDataIssue(const GUIModelData& m,
				   unsigned int which)
{
  if (this->GetLock()==false) return;

  if ( ((which & kPlotViewRefreshID ) ||
	(which & kCurrentHistogramID) ||
	(which & kHistogramSourceID ))
       && !(which & kComparisonUpdateID) ) {
    fHistogramSource  = m.fHistogramSource;
    fCurrentHistogram = m.fCurrentHistogram.Current();
    this->Update();
  }

  if ( (which & kComparisonUpdateID) ) {
    fHistogramSource  = m.fHistogramSource;
    fCurrentHistogram = m.fCurrentHistogram.Current();
    this->UpdateCompare(m.fComparisonOpt);
  }

  if ( (which & kPrintID) ) {
    std::string n = fCurrentHistogram;
    n += ".png";
    this->GetCanvas()->Print(n.c_str());
    emph::onmon::cout << "Printed to file " << n << "";
  }

  this->ReleaseLock();
}

//......................................................................

PlotViewer::~PlotViewer() { }

////////////////////////////////////////////////////////////////////////
