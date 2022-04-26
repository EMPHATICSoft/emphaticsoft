#include <iostream>
#include "OnlineMonitoring/viewer/HeaderBar.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/viewer/HistoSource.h"
#include <ctime>
#include "TGLabel.h"
#include "TGWidget.h"
#include "TTimer.h"

using namespace emph::onmon;

static unsigned int kTick = 2000; // ms/tick

HeaderBar::HeaderBar(TGWindow* w) :
  TGGroupFrame(w, "Monitoring Status", kHorizontalFrame)
{
  fProdLabel = new TGLabel(this,
			   "Plotter: ------ pid:------ s/utime=------/------ rsize=------k"
			   " Run/Subrun/Event=------/----/---------");
  fProdLabel->SetTextJustify(kTextLeft);
  this->AddFrame(fProdLabel, new TGLayoutHints(kLHintsLeft|kLHintsTop, 16, 16, 2, 2));

  fDateLabel = new TGLabel(this, "--------------------------");
  fDateLabel->SetTextJustify(kTextRight);
  this->AddFrame(fDateLabel, new TGLayoutHints(kLHintsRight|kLHintsTop, 16, 16, 2, 2));

  fTimer = new TTimer;
  fTimer->SetObject(this);
  fTimer->Start(kTick);
}

//......................................................................

HeaderBar::~HeaderBar()
{
  if(fProdLabel) { delete fProdLabel; fProdLabel = 0; }
  if(fDateLabel) { delete fDateLabel; fDateLabel = 0; }
  if(fTimer)     { delete fTimer;     fTimer = 0; }
}

//......................................................................

Bool_t HeaderBar::HandleTimer(TTimer *timer __attribute__((unused))) 
{

  // Check to see that the histosource is SHM
  // If the source is a root file, return.
  const char* source = GUIModel::Instance().Data().fHistogramSource.c_str();
  std::string str(source);
  bool is_root = str.find(".root")<str.length();
  bool is_shm  = str.find(".shm")< str.length();

  if (is_root && is_shm) abort();
  if (is_root) {
    this->Layout();
    fTimer->SetTime(kTick);
    return true;
  }

  HistoSource f(source);

  std::string snm;
  unsigned int r, s, e;
  pid_t pid;
  long stime, utime, cpu, rsize;
  time_t tp;
  f.GetStatus(snm, &r, &s, &e, &pid, &stime, &utime, &cpu, &rsize, &tp);

  char evtext[1024];
  sprintf(evtext,
	  "Plotter: %6s pid:%6d s/utime=%lu/%lu rsize=%6.2fk"
	  " Run/Subrun/Event=%d/%d/%d",
	  snm.c_str(), pid, stime, utime, rsize/1024.0,
	  r, s, e);
  fProdLabel->ChangeText(evtext);

  time_t t = time(NULL);
  char datext[1024];
  sprintf(datext, "UTC: %s", asctime(gmtime(&t)));
  for (unsigned int i=0; i<1024; ++i) {
    if (datext[i]=='\n') datext[i] = '\0';
  }
  fDateLabel->ChangeText(datext);

  //
  // Documentation says we have to call a Layout when the text changes
  //
  this->Layout();

  fTimer->SetTime(kTick);

  return true;
}
////////////////////////////////////////////////////////////////////////
