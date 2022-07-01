///
/// \file    HistoSet.cxx
/// \brief   Hold the collection of histograms created by the producer
/// \version $Id: HistoSet.cxx,v 1.15 2012-11-13 16:19:49 mbaird42 Exp $
/// \author  messier@indiana.edu, mbaird42@fnal.gov
///
#include "OnlineMonitoring/plotter/HistoSet.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/RegexMatch.h"
using namespace emph::onmon;

HistoSet::HistoSet() : TickerSubscriber(TickerSubscriber::kAll)
{ }

//......................................................................

HistoSet& HistoSet::Instance()
{
  static HistoSet* robertplant = 0;
  if (robertplant == 0) robertplant = new HistoSet();

  TH1::SetDefaultSumw2(kFALSE);

  return *robertplant;
}

//......................................................................

HistoSet::~HistoSet() { }

//......................................................................

TH1* HistoSet::FindTH1(const char* nm)
{
  TH1* h=0;
  h = this->FindTH1F(nm); if (h!=0) return h;
  h = this->FindTH2F(nm); if (h!=0) return h;
  return 0;
}

//......................................................................

TH1F* HistoSet::FindTH1F(const char* nm)
{
  std::map<std::string,TH1F*>::iterator itr(fTH1F.find(nm));
  if (itr!=fTH1F.end()) return itr->second;
  return 0;
}

//......................................................................

TH2F* HistoSet::FindTH2F(const char* nm)
{
  std::map<std::string,TH2F*>::iterator itr(fTH2F.find(nm));
  if (itr!=fTH2F.end()) return itr->second;
  return 0;
}

//......................................................................

TH1F* HistoSet::GetTH1F(const char* nm)
{
  // Check if we've alread booked this histogram
  TH1F* h = this->FindTH1F(nm);
  if (h!=0) return h;

  // Histogram not found. Get the data we need to book it
  const HistoData* d = HistoTable::Instance().LookUp(nm);
  if (d==0) {
    // Nothing is known about the named histogram?!
    std::cerr << "\nNothing known about requested histogram " << nm
	      << "\n" << std::endl;
    return 0;
  }
  h = new TH1F(nm,d->fTitle.c_str(),d->fNx,d->fX1,d->fX2);
  fTH1F[nm] = h;
  return h;
}

//......................................................................

TH2F* HistoSet::GetTH2F(const char* nm)
{
  TH2F* h = this->FindTH2F(nm);
  if (h!=0) return h;

  // Histogram not found. Get the data we need to book it
  const HistoData* d = HistoTable::Instance().LookUp(nm);
  if (d==0) {
    // Nothing is known about the named histogram?!
    std::cerr << "\nNothing known about requested histogram " << nm
	      << "\n" << std::endl;
    return 0;
  }
  h = new TH2F(nm, d->fTitle.c_str(),
	       d->fNx, d->fX1, d->fX2,
	       d->fNy, d->fY1, d->fY2);
  fTH2F[nm] = h;
  return h;
}

//......................................................................

void HistoSet::GetNames(std::list<std::string>& h)
{
  std::map<std::string,TH1F*>::iterator i1   (fTH1F.begin());
  std::map<std::string,TH1F*>::iterator i1end(fTH1F.end());
  for (; i1!=i1end; ++i1) {
    if(i1->second != 0) h.push_back(i1->first);
  }
  std::map<std::string,TH2F*>::iterator i2   (fTH2F.begin());
  std::map<std::string,TH2F*>::iterator i2end(fTH2F.end());
  for (; i2!=i2end; ++i2) {
    if(i2->second != 0) h.push_back(i2->first);
  }
}

//......................................................................

/// \todo - Should make folders inside the file to keep the histograms
/// organized
void HistoSet::WriteToRootFile(TFile* f)
{
  f->cd();
  std::map<std::string,TH1F*>::iterator i1   (fTH1F.begin());
  std::map<std::string,TH1F*>::iterator i1end(fTH1F.end());

  // add each histos to file as a new histo (use for debugging)
  // for (; i1!=i1end; ++i1) i1->second->Write();

  // overwrite old histos in file with the same name. QUESTION: should
  // buffer size be zero??? (3rd argument - 0 is the default value)
  for (; i1!=i1end; ++i1) {
    i1->second->Write(i1->first.c_str(),TObject::kOverwrite,0);
  }

  std::map<std::string,TH2F*>::iterator i2   (fTH2F.begin());
  std::map<std::string,TH2F*>::iterator i2end(fTH2F.end());

  // add each histos to file as a new histo (use for debugging)
  // for (; i2!=i2end; ++i2) i2->second->Write();

  // overwrite old histos in file with the same name. QUESTION: should
  // buffer size be zero??? (3rd argument - 0 is the default value)
  for (; i2!=i2end; ++i2) {
    i2->second->Write(i2->first.c_str(),TObject::kOverwrite,0);
  }
}

//......................................................................

void HistoSet::CopyAndResetAll(unsigned int which)
{
  const std::string prev("*_prev_*");
  //
  // Besure to use copies of the TH1F and TH2F histogram sets since
  // the original versions will grow if copies get made.
  //
  std::map<std::string,TH1F*> fTH1Fcopy(fTH1F);
  std::map<std::string,TH1F*>::iterator i1   (fTH1Fcopy.begin());
  std::map<std::string,TH1F*>::iterator i1end(fTH1Fcopy.end());
  for (; i1!=i1end; ++i1) {
    //
    // Don't make copies of copies
    //
    if (regex_match(i1->first, prev)!=0) {
      this->CopyAndResetOne(i1->first, which);
    }
  }

  std::map<std::string,TH2F*> fTH2Fcopy(fTH2F);
  std::map<std::string,TH2F*>::iterator i2   (fTH2Fcopy.begin());
  std::map<std::string,TH2F*>::iterator i2end(fTH2Fcopy.end());
  for (; i2!=i2end; ++i2) {
    //
    // Don't make copies of copies
    //
    if (regex_match(i2->first, prev)!=0) {
      this->CopyAndResetOne(i2->first, which);
    }
  }
}

//......................................................................

///
/// Manage the collection of copies. If we are supposed to be able to
/// look back through "lookback" number of copies, then search for
/// histograms matching the pattern "base_tag_i". If I find
/// base_tag_7, rename it base_tag_8 and so on. The last histogram in
/// the list gets deleted to make room for the new ones.
///
void HistoSet::MakeTH1FCopies(const char* base,
			      const char* tag,
			      unsigned int lookback)
{
  if (lookback==0) return;

  unsigned int i, j;
  TH1F* h1i;
  TH1F* h1j;
  char ni[256], nj[256];
  for (j=lookback+1; j>1; --j) {
    //
    // Find the histogram to be renamed / copied
    //
    i = j-1;
    sprintf(ni, "%s_%s_%.2d", base, tag, i);
    h1i = this->FindTH1F(ni);
    if (h1i==0) continue;

    //
    // If we've reached the look back count don't copy the histogram,
    // delete it
    //
    if (i==lookback) {
      delete h1i;
      fTH1F[ni] = 0;
      continue;
    }

    //
    // Look at the target destination. If its not empty, empty it.
    //
    sprintf(nj, "%s_%s_%.2d", base, tag, j);
    h1j = this->FindTH1F(nj);
    if (h1j!=0) { delete h1j; fTH1F[nj] = 0; }

    //
    // Rename the first histogram as the second and move its place in
    // the table
    //
    h1i->SetName(nj);
    fTH1F[nj] = h1i;
    fTH1F[ni] = 0;
  }
}

//......................................................................

void HistoSet::MakeTH2FCopies(const char* base,
			      const char* tag,
			      unsigned int lookback)
{
  if (lookback==0) return;

  unsigned int i, j;
  TH2F* h2i;
  TH2F* h2j;
  char ni[256], nj[256];
  for (j=lookback+1; j>1; --j) {
    //
    // Find the histogram to be renamed / copied
    //
    i = j-1;
    sprintf(ni, "%s_%s_%.2d", base, tag, i);
    h2i = this->FindTH2F(ni);
    if (h2i==0) continue;

    //
    // If we've reached the look back count don't copy the histogram,
    // delete it
    //
    if (i==lookback) {
      delete h2i;
      fTH2F[ni] = 0;
      continue;
    }

    //
    // Look at the target destination. If its not empty, empty it.
    //
    sprintf(nj, "%s_%s_%.2d", base, tag, j);
    h2j = this->FindTH2F(nj);
    if (h2j!=0) { delete h2j; fTH2F[nj] = 0; }

    //
    // Rename the first histogram as the second and move its place in
    // the table
    //
    h2i->SetName(nj);
    fTH2F[nj] = h2i;
    fTH2F[ni] = 0;
  }
}

//......................................................................

void HistoSet::CopyAndResetOne(const std::string& nm,
			       unsigned int       which)
{
  //
  // Find the specifications for this histogram
  //
  HistoTable&      ht = HistoTable::Instance();
  const HistoData* hd = 0;
  hd = ht.LookUp(nm.c_str());
  if (hd==0) return;

  //
  // Check if this histogram is scheduled to be reset
  //
  if ((hd->fReset & which)==0) return;

  //
  // Reset is scheduled - make a copy and reset the histogram
  //
  // Find the histogram in the collections
  //
  TH1F* h1 = 0;
  TH2F* h2 = 0;
  if (hd->fType == kTH1F) h1 = this->FindTH1F(nm.c_str());
  if (hd->fType == kTH2F) h2 = this->FindTH2F(nm.c_str());
  if (h1==0 && h2==0) {
    std::cerr << __FILE__ << ":" << __LINE__
	      << "Can't find histogram " << nm << std::endl;
    abort();
  }

  //
  // Prepare a name for the copy
  //
  std::string copyname = nm;
  const char* tag=0;
  switch (which) {
  case TickerSubscriber::kRun:    tag = "prev_run";    break;
  case TickerSubscriber::kSubrun: tag = "prev_subrun"; break;
  case TickerSubscriber::k30sec:  tag = "prev_30sec";  break;
  case TickerSubscriber::k1min:   tag = "prev_1min";   break;
  case TickerSubscriber::k5min:   tag = "prev_5min";   break;
  case TickerSubscriber::k10min:  tag = "prev_10min";  break;
  case TickerSubscriber::k30min:  tag = "prev_30min";  break;
  case TickerSubscriber::kHour:   tag = "prev_hour";   break;
  case TickerSubscriber::k24hr:   tag = "prev_24hr";   break;
  default: abort();
  }
  copyname += "_"; copyname += tag; copyname += "_01";

  this->MakeTH1FCopies(nm.c_str(), tag, hd->fLookBack);
  this->MakeTH2FCopies(nm.c_str(), tag, hd->fLookBack);

  //
  // If the copy histogram does not exist, we'll need to make it. Once
  // we're sure we have a destination, make the copy and reset the
  // original histogram.
  //
  /// \todo The code fragments below are carbon copies of one
  /// another with 1->2. Is there a more elegant solution to this?
  if (h1) {
    TH1F* h1copy = 0;
    h1copy = this->FindTH1F(copyname.c_str());
    if (h1copy==0) {
      h1copy = new TH1F(*h1);
      fTH1F[copyname] = h1copy;
    }
    h1->Copy(*h1copy);
    h1copy->SetName(copyname.c_str());
    h1->Reset();
  }
  if (h2) {
    TH2F* h2copy = 0;
    h2copy = this->FindTH2F(copyname.c_str());
    if (h2copy==0) {
      h2copy = new TH2F(*h2);
      fTH2F[copyname] = h2copy;
    }
    h2->Copy(*h2copy);
    h2copy->SetName(copyname.c_str());
    h2->Reset();
  }
}

//......................................................................

void HistoSet::UTCReset(int UTCHour)
{
  HistoTable&      ht = HistoTable::Instance();
  const HistoData* hd = 0;

  std::map<std::string,TH1F*>::iterator i1   (fTH1F.begin());
  std::map<std::string,TH1F*>::iterator i1end(fTH1F.end());
  for (; i1!=i1end; ++i1) {
    hd = ht.LookUp(i1->first.c_str());
    if (hd==0) continue;
    if (hd->fReset == kUTC) {
      this->UTCResetTH1F(UTCHour, i1->second);
    }
  }

  std::map<std::string,TH2F*>::iterator i2   (fTH2F.begin());
  std::map<std::string,TH2F*>::iterator i2end(fTH2F.end());
  for (; i2!=i2end; ++i2) {
    hd = ht.LookUp(i2->first.c_str());
    if (hd==0) continue;
    if (hd->fReset == kUTC) {
      this->UTCResetTH2F(UTCHour, i2->second);
    }
  }
}

//......................................................................

void HistoSet::UTCResetTH1F(int UTCHour, TH1F* h)
{
  //
  // Schedule for reset is to zero the plot not for *this* UTC hour,
  // but for the next UTC hour. So, if we are to start filling the
  // hour between 8 and 9 we clear the bins between 9 and 10.
  //
  int reset_hour = (UTCHour+1)%24;

  int    i;
  int    nbinsx = h->GetNbinsX();
  double binutc;  // UTC hour at center of bin
  int    binutci; // UTC hour at center of bin, floored to int

  //
  // Loop over x-axis looking for bins that match the UTC hour we
  //
  for (i=1; i<=nbinsx; ++i) {
    binutc = h->GetXaxis()->GetBinCenter(i);
    //
    // If the histogram has been binned to include UTC hours out of
    // the standard range of [0...24), restore the hour to something
    // modulo 24 hours.
    //
    while (binutc<  0.0) binutc += 24.0;
    while (binutc>=24.0) binutc -= 24.0;
    binutci = (int)floor(binutc);
    if (binutci==reset_hour) {
      h->SetBinContent(i,0);
    }
  }
}

//......................................................................

void HistoSet::UTCResetTH2F(int UTCHour, TH2F* h)
{
  //
  // Schedule for reset is to zero the plot not for *this* UTC hour,
  // but for the next UTC hour. So, if we are to start filling the
  // hour between 8 and 9 we clear the bins between 9 and 10.
  //
  int reset_hour = (UTCHour+1)%24;

  int    i, j;
  int    nbinsx = h->GetNbinsX();
  int    nbinsy = h->GetNbinsY();
  double binutc;  // UTC hour at center of bin
  int    binutci; // UTC hour at center of bin, floored to int

  //
  // Loop over x-axis looking for bins that match the UTC hour we
  //
  for (i=1; i<=nbinsx; ++i) {
    binutc = h->GetXaxis()->GetBinCenter(i);
    //
    // If the histogram has been binned to include UTC hours out of
    // the standard range of [0...24), restore the hour to something
    // modulo 24 hours.
    //
    while (binutc<  0.0) binutc += 24.0;
    while (binutc>=24.0) binutc -= 24.0;
    binutci = (int)floor(binutc);

    //
    // Reset the columns to zero if the hours match
    //
    if (binutci==reset_hour) {
      for (j=1; j<=nbinsy; ++j) {
	h->SetBinContent(i,j,0);
      }
    }
  }
}

//......................................................................

void HistoSet::RunTicker()
{
  this->CopyAndResetAll(TickerSubscriber::kRun);
}

//......................................................................

void HistoSet::SubrunTicker()
{
  this->CopyAndResetAll(TickerSubscriber::kSubrun);
}

//......................................................................

void HistoSet::ThirtySecTicker()
{
  this->CopyAndResetAll(TickerSubscriber::k30sec);
}

//......................................................................

void HistoSet::OneMinTicker()
{
  this->CopyAndResetAll(TickerSubscriber::k1min);
}

//......................................................................

void HistoSet::FiveMinTicker()
{
  this->CopyAndResetAll(TickerSubscriber::k5min);
}

//......................................................................

void HistoSet::TenMinTicker()
{
  this->CopyAndResetAll(TickerSubscriber::k10min);
}

//......................................................................

void HistoSet::ThirtyMinTicker()
{
  this->CopyAndResetAll(TickerSubscriber::k30min);
}

//......................................................................

void HistoSet::HourTicker()
{
  this->CopyAndResetAll(TickerSubscriber::kHour);

  time_t    tt = time(0);
  struct tm t;
  gmtime_r(&tt, &t);
  this->UTCReset(t.tm_hour);
}

//......................................................................

void HistoSet::TwentyFourHrTicker()
{
  this->CopyAndResetAll(TickerSubscriber::k24hr);
}

//......................................................................

void HistoSet::DeleteTH1F(TH1F* h)
{
  std::map<std::string,TH1F*>::iterator i1   (fTH1F.begin());
  std::map<std::string,TH1F*>::iterator i1end(fTH1F.end());
  for (; i1!=i1end; ++i1) {
    if(i1->second == h) {
      break;
    }
  }

  if(i1!=i1end) {
    delete i1->second;
    i1->second = 0;
    fTH1F.erase(i1);
  }
}

//......................................................................

void HistoSet::DeleteTH2F(TH2F* h)
{
  std::map<std::string,TH2F*>::iterator i2   (fTH2F.begin());
  std::map<std::string,TH2F*>::iterator i2end(fTH2F.end());
  for (; i2!=i2end; ++i2) {
    if(i2->second == h) {
      break;
    }
  }

  if(i2!=i2end) {
    delete i2->second;
    i2->second = 0;
    fTH2F.erase(i2);
  }
}

////////////////////////////////////////////////////////////////////////
