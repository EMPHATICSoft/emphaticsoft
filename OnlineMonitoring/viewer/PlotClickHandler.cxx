#include "OnlineMonitoring/viewer/PlotClickHandler.h"
#include <cstdio>
#include <cmath>
#include <iostream>
#include "TObject.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/viewer/PlotOptions.h"

using namespace emph::onmon;

PlotClickHandler::PlotClickHandler()
{
  //fHwMap->SetupDet(GUIModel::Instance().Data().fDetector);
}

//......................................................................

PlotClickHandler::~PlotClickHandler() {
  //if(fHwMap) {delete fHwMap; fHwMap = 0;}
}

//......................................................................

void PlotClickHandler::SingleClick(const PlotOptions& plotopt __attribute__((unused)))
{
  int decoded = this->DecodeClick();
  if (decoded==0) return;

  this->DefaultHistogramInfo();
}

//......................................................................

void PlotClickHandler::DoubleClick(const PlotOptions& plotopt __attribute__((unused)))
{
  int decoded = this->DecodeClick();
  if (decoded==0) return;
}

//......................................................................

int PlotClickHandler::DecodeClick()
{
  //
  // Get current histo name
  //
  fHistoName = GUIModel::Instance().Data().fCurrentHistogram.Current();

  //
  // Find out where the click occured
  //
  TObject* h = gPad->GetSelected();
  if (h==0) return 0;

  fH1 = 0;
  if (h->InheritsFrom(TH1::Class())) fH1 = (TH1*)h;
  if (fH1==0) return 0;

  fH2 = 0;
  if (h->InheritsFrom(TH2::Class())) fH2 = (TH2*)h;
  if (fH2==0) return 0;


  int px = gPad->GetEventX();
  double xx = gPad->AbsPixeltoX(px);

  fX = gPad->PadtoX(xx);
  if (fX<fH1->GetXaxis()->GetXmin()) return 0;
  if (fX>fH1->GetXaxis()->GetXmax()) return 0;

  //
  // Handle TH2s here. Do them first since ROOT defines TH2 as a
  // subset of TH1's
  //
  fY    = 0;
  fYbin = 0;
  if (fH2) {
    int    py = gPad->GetEventY();
    double yy = gPad->AbsPixeltoY(py);

    fY = gPad->PadtoY(yy);

    double ymin = fH2->GetYaxis()->GetXmin();
    if (fY<ymin) return 0;

    double ymax = fH2->GetYaxis()->GetXmax();
    if (fY>ymax) return 0;

    fXbin = fH2->GetXaxis()->FindBin(fX);
    fYbin = fH2->GetYaxis()->FindBin(fY);
    return 1;
  }
  if (fH1) {
    fXbin = fH1->GetXaxis()->FindBin(fX);
    return 1;
  }

  return 0;
}

//......................................................................

void PlotClickHandler::DefaultHistogramInfo()
{
  if (fH2) {
    char buff[256];
    sprintf(buff, "Histogram %s: bin %d,%d (x=%.3f,y=%.3f) content=%.3f",
	    fHistoName.c_str(), fXbin, fYbin, fX, fY,
	    fH2->GetBinContent(fXbin,fYbin));
    GUIModel::Instance().SetHistogramInfoText(buff);
    return;
  }
  if (fH1) {
    char buff[256];
    sprintf(buff, "Histogram %s: Bin %d (x=%.3f) content=%.3f",
	    fHistoName.c_str(), fXbin, fX, fH1->GetBinContent(fXbin));
    GUIModel::Instance().SetHistogramInfoText(buff);
    return;
  }
}

////////////////////////////////////////////////////////////////////////
