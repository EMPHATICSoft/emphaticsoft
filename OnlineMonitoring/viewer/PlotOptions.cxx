#include "OnlineMonitoring/viewer/PlotOptions.h"
#include "OnlineMonitoring/viewer/UTCLabel.h"
#include "OnlineMonitoring/viewer/HistoSource.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/util/TickerSubscriber.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include "TLine.h"
#include "TArrow.h"
#include "TPad.h"
#include "TH2F.h"
#include "TPaveText.h"

using namespace emph::onmon;

PlotOptions::PlotOptions()
{
  fLabelText = new TPaveText(0.1, 0.0, 0.5,  0.055, "NDC");
  fSLText    = new TPaveText(0.0, 0.1, 0.09, 0.9,   "NDC");

  fLabelText->SetLineColor(0);
  fLabelText->SetFillColor(0);
  fLabelText->SetBorderSize(1);
  fLabelText->SetMargin(0.0);
  fLabelText->SetTextAlign(11);

  fSLText->SetLineColor(0);
  fSLText->SetFillColor(0);
  fSLText->SetBorderSize(1);

  this->Reset();
}

//......................................................................

void PlotOptions::Reset()
{
  fDrawOpt        = "";
  fZoomHour       = false;
  fAutoZoomX      = false;
  fAutoZoomY      = false;
  fAutoZoomZ      = false;
  fLogx           = false;
  fLogy           = false;
  fLogz           = false;
  fGridx          = false;
  fGridy          = false;
  fHaveXscale     = false;
  fXlo            = 0;
  fXhi            = 0;
  fHaveYscale     = false;
  fYlo            = 0;
  fYhi            = 0;
  fHaveZscale     = false;
  fZlo            = 0;
  fZhi            = 0;
}

//......................................................................
//
// Parse an option in format xscale[-1.3|7.6]
//
void PlotOptions::ParseXscale(const char* opt)
{
  int n;
  float x1, x2;
  n = sscanf(opt,"xscale[%f|%f]",&x1,&x2);
  if (n==2) {
    fHaveXscale = true;
    fXlo = x1;
    fXhi = x2;
  }
  else {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Can't parse " << opt
	      << std::endl;
  }
}

//......................................................................
//
// Parse an option in format yscale[-1.3|7.6]
//
void PlotOptions::ParseYscale(const char* opt)
{
  int n;
  float y1, y2;
  n = sscanf(opt,"yscale[%f|%f]",&y1,&y2);
  if (n==2) {
    fHaveYscale = true;
    fYlo = y1;
    fYhi = y2;
  }
  else {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Can't parse " << opt
	      << std::endl;
  }
}

//......................................................................
//
// Parse an option in format zscale[-1.3|7.6]
//
void PlotOptions::ParseZscale(const char* opt)
{
  int n;
  float z1, z2;
  n = sscanf(opt,"zscale[%f|%f]",&z1,&z2);
  if (n==2) {
    fHaveZscale = true;
    fZlo = z1;
    fZhi = z2;
  }
  else {
    std::cerr << __FILE__ << ":" << __LINE__
	      << " Can't parse " << opt
	      << std::endl;
  }
}

//......................................................................

void PlotOptions::Set(const std::vector<std::string>& opt)
{
  this->Reset();
  unsigned int i;
  for (i=0; i<opt.size(); ++i) {
    bool xscale = (strncmp(opt[i].c_str(),"xscale",6)==0);
    bool yscale = (strncmp(opt[i].c_str(),"yscale",6)==0);
    bool zscale = (strncmp(opt[i].c_str(),"zscale",6)==0);

    if      (opt[i]=="zoomhour")  { fZoomHour  = true; }
    else if (opt[i]=="autozoomx") { fAutoZoomX = true; }
    else if (opt[i]=="autozoomy") { fAutoZoomY = true; }
    else if (opt[i]=="autozoomz") { fAutoZoomZ = true; }
    else if (opt[i]=="logx")      { fLogx      = true; }
    else if (opt[i]=="logy")      { fLogy      = true; }
    else if (opt[i]=="logz")      { fLogz      = true; }
    else if (opt[i]=="gridx")     { fGridx     = true; }
    else if (opt[i]=="gridy")     { fGridy     = true; }
    else if (xscale)              { this->ParseXscale(opt[i].c_str()); }
    else if (yscale)              { this->ParseYscale(opt[i].c_str()); }
    else if (zscale)              { this->ParseZscale(opt[i].c_str()); }
    else {
      //
      // If the options haven't been handled yet, assume they are
      // histogram drawing options
      //
      fDrawOpt += opt[i];
    }
  }
}

//......................................................................

void PlotOptions::SetPad(TPad* p)
{
  p->SetLogx(fLogx);
  p->SetLogy(fLogy);
  p->SetLogz(fLogz);
  p->SetGridx(fGridx);
  p->SetGridy(fGridy);
}

//......................................................................

void PlotOptions::MakeLabels(const TH1* h, const HistoData* hd __attribute__((unused)))
{

  fLabelText->Clear();
  this->MakeLabelText(h);
  fLabelText->Draw();

  fSLText->Clear();


}

//......................................................................

void PlotOptions::AutoScale(TH1F* h)
{
  int i;
  int ilox = 999;
  int ihix = 0;
  double y;
  double ylo =  9e9;
  double yhi = -9e9;
  for (i=1; i<=h->GetNbinsX(); ++i) {
    y = h->GetBinContent(i);
    if (y!=0.0) {
      if (y<ylo) ylo = y;
      if (y>yhi) yhi = y;
      if (i<ilox) ilox = i;
      if (i>ihix) ihix = i;
    }
  }
  if (fZoomHour) {

    // Get the current UTC hour.
    time_t rawtime;
    time(&rawtime);
    struct tm* timestr;
    timestr = gmtime(&rawtime);
    double now = timestr->tm_hour + (timestr->tm_min)/60.0;

    h->GetXaxis()->SetRangeUser(now - 1.0, now);

  }
  if (fAutoZoomX) {
    double xlo = 0.9*h->GetXaxis()->GetBinLowEdge(ilox);
    double xhi = 1.1*h->GetXaxis()->GetBinUpEdge(ihix);
    //
    // If we are on a logarithmic scale make sure we have at least 1
    // order of magnitude. Otherwise no numbers will be shown on the
    // scale
    //
    if (fLogx) {
      if (xlo<=0.0) xlo = 1e-3;
      if (xhi<=xlo) xhi = 10.0*xlo;
      while (xhi/xlo<10.0) { xhi *= 1.1; xlo *= 0.9; }
    }
    h->GetXaxis()->SetRangeUser(xlo, xhi);
  }
  if (fAutoZoomY) {
    ylo = 0.9*ylo;
    yhi = 1.1*yhi;
    if (fLogy) {
      if (ylo<=0.0) ylo = 0.1;
      if (yhi<=ylo) yhi = 10.0*ylo;
      while (yhi/ylo<10.0) { yhi *= 1.1; ylo *= 0.9; }
    }
    h->GetYaxis()->SetRangeUser(ylo, yhi);
  }

  if (fHaveXscale) {
    double lo = fXlo;
    double hi = fXhi;
    h->GetXaxis()->SetRangeUser(lo,hi);
  }
  if (fHaveYscale) {
    double lo = fYlo;
    double hi = fYhi;
    h->GetYaxis()->SetRangeUser(lo,hi);
  }
}

//......................................................................

void PlotOptions::AutoScale(TH2F* h)
{
  //
  // Check options - if none are set, just return now.
  //
  if (fZoomHour  ==false &&
      fAutoZoomX ==false &&
      fAutoZoomY ==false &&
      fAutoZoomZ ==false &&
      fHaveXscale==false &&
      fHaveYscale==false &&
      fHaveZscale==false) {
    return;
  }

  int ilox = 999;
  int ihix = 0;
  int iloy = 999;
  int ihiy = 0;
  double c  = 0;
  double mx = 0;
  double mn = 9e99;

  int i, j;
  for (i=1; i<=h->GetNbinsX(); ++i) {
    for (j=1; j<=h->GetNbinsY(); ++j) {
      c = h->GetBinContent(i,j);
      if (c!=0) {
	if (c>mx) mx = c;
	if (c<mn) mn = c;
	if (i<ilox) ilox = i;
	if (i>ihix) ihix = i;
	if (j<iloy) iloy = j;
	if (j>ihiy) ihiy = j;
      }
    }
  }
  if (fZoomHour) {

    // Get the current UTC hour.
    time_t rawtime;
    time(&rawtime);
    struct tm* timestr;
    timestr = gmtime(&rawtime);
    double now = timestr->tm_hour + (timestr->tm_min)/60.0;

    h->GetXaxis()->SetRangeUser(now - 1.0, now);

  }
  if (fAutoZoomX) {
    double xlo = 0.95*h->GetXaxis()->GetBinLowEdge(ilox);
    double xhi = 1.05*h->GetXaxis()->GetBinUpEdge(ihix);
    if (fLogx) {
      if (xlo<=0.0) xlo = 1e-3;
      if (xhi<=xlo) xhi = 10.0*xlo;
      while (xhi/xlo<10.0) { xhi *= 1.1; xlo *= 0.9; }
    }
    h->GetXaxis()->SetRangeUser(xlo, xhi);
  }
  if (fAutoZoomY) {
    double ylo = 0.95*h->GetYaxis()->GetBinLowEdge(iloy);
    double yhi = 1.05*h->GetYaxis()->GetBinUpEdge(ihiy);
    if (fLogy) {
      if (ylo<=0.0) ylo = 1e-3;
      if (yhi<=ylo) yhi = 10.0*ylo;
      while (yhi/ylo<10.0) { yhi *= 1.1; ylo *= 0.9; }
    }
    h->GetYaxis()->SetRangeUser(ylo, yhi);
  }
  if (fAutoZoomZ) {
    if (fLogz) {
      if (mn<=0.0) mn = 0.1;
      if (mx<=mn)  mx = 10.0*mn;
      while (mx/mn<10.0) { mx *= 1.1; mn *= 0.9; }
    }
    h->GetZaxis()->SetRangeUser(0.95*mn, 1.05*mx);
  }

  if (fHaveXscale) {
    double lo = fXlo;
    double hi = fXhi;
    h->GetXaxis()->SetRangeUser(lo,hi);
  }
  if (fHaveYscale) {
    double lo = fYlo;
    double hi = fYhi;
    h->GetYaxis()->SetRangeUser(lo,hi);
  }
  if (fHaveZscale) {
    double lo = fZlo;
    double hi = fZhi;
    h->GetZaxis()->SetRangeUser(lo,hi);
  }
}

//......................................................................

void PlotOptions::MakeLabelText(const TH1* h)
{

  //
  // Add event numbers and histogram source name
  //
  // Check to see that the histosource is SHM or ROOT and make the
  // appropriate label.
  //
  const char* source = GUIModel::Instance().Data().fHistogramSource.c_str();
  std::string str(source);
  bool is_root = str.find(".root")<str.length();
  bool is_shm  = str.find(".shm")< str.length();

  if ( is_root &&  is_shm) abort();
  if (!is_root && !is_shm) abort();

  char buff[256];
  if(is_shm) {
    sprintf(buff, "%s : source = %s", h->GetName(), source);
    fLabelText->AddText(buff);

    HistoSource f(source);

    std::string snm;
    unsigned int r, s, e;
    pid_t pid;
    long stime, utime, cpu, rsize;
    time_t tp;
    f.GetStatus(snm, &r, &s, &e, &pid, &stime, &utime, &cpu, &rsize, &tp);

    char evtext[256];
    sprintf(evtext, "EventID: %d / %d / %d", r, s, e);
    fLabelText->AddText(evtext);
  }
  else {
    sprintf(buff, "%s :", h->GetName());
    fLabelText->AddText(buff);
    fLabelText->AddText(source);
  }

  //
  // Add date and time stamp
  //
  time_t rawtime;
  time(&rawtime);
  struct tm* timestr;
  timestr = gmtime(&rawtime);
  sprintf(buff, "%s (UTC)",asctime(timestr));

  fLabelText->AddText(buff);
}

//......................................................................

PlotOptions::~PlotOptions()
{
  if(fLabelText) { delete fLabelText; fLabelText = 0; }
  if(fSLText)    { delete fSLText;    fSLText    = 0; }
}

////////////////////////////////////////////////////////////////////////
