#include "OnlineMonitoring/viewer/ButtonBank.h"
#include <cstdlib>
#include <iostream>
#include "TGButton.h"
#include "TGLayout.h"
#include "TTimer.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/Icons.h"
using namespace om;

//......................................................................

ButtonBank::ButtonBank(TGWindow* mf) :
  TGCompositeFrame(mf,
		   Layout::fButtonBankSizeX,
		   Layout::fButtonBankSizeY,
		   kHorizontalFrame),
  GUIModelSubscriber(kCurrentHistogramID|kPlotViewRefreshPausedID)
{
  fLhint = new TGLayoutHints(kLHintsLeft, 2, 4);

  fBack = new TGPictureButton(this, Icons::Rewind());
  fBack->Connect("Clicked()", "om::ButtonBank", this, "Back()");
  fBack->SetToolTipText("Move backward in history");
  fBack->SetState(kButtonDisabled);
  this->AddFrame(fBack, fLhint);

  fForward = new TGPictureButton(this, Icons::FastForward());
  fForward->SetToolTipText("Move forward in history");
  fForward->Connect("Clicked()", "om::ButtonBank", this, "Forward()");
  fForward->SetState(kButtonDisabled);
  this->AddFrame(fForward, fLhint);

  fResume = new TGPictureButton(this, Icons::Play());
  fResume->SetToolTipText("Resume automatic plot updates");
  fResume->Connect("Clicked()", "om::ButtonBank", this, "Resume()");
  fResume->SetState(kButtonDisabled);
  this->AddFrame(fResume, fLhint);

  fPause = new TGPictureButton(this, Icons::Pause());
  fPause->SetToolTipText("Pause automatic plot updates");
  fPause->Connect("Clicked()", "om::ButtonBank", this, "Pause()");
  fPause->SetState(kButtonDisabled);
  this->AddFrame(fPause, fLhint);

  fUpdate = new TGPictureButton(this, Icons::Repeat());
  fUpdate->SetToolTipText("Force plot update now");
  fUpdate->Connect("Clicked()", "om::ButtonBank", this, "Update()");
  fUpdate->SetState(kButtonDisabled);
  this->AddFrame(fUpdate, fLhint);

  fHisto = new TGPictureButton(this, Icons::Histo());
  fHisto->SetToolTipText("Show 1D histogram of bin contents");
  fHisto->Connect("Clicked()", "om::ButtonBank", this, "Histo()");
  fHisto->SetState(kButtonUp);
  this->AddFrame(fHisto, fLhint);

  fPrint = new TGPictureButton(this, Icons::Print());
  fPrint->SetToolTipText("Print to a file");
  fPrint->Connect("Clicked()", "om::ButtonBank", this, "PrintPlot()");
  fPrint->SetState(kButtonUp);
  this->AddFrame(fPrint, fLhint);

  this->MapSubwindows();
  this->Resize();
  this->MapWindow();

  fTimer = new TTimer;
  fTimer->SetObject(this);
  fTimer->Start(GUIModel::Instance().Data().fPlotViewRefreshRate);
}

//......................................................................

void ButtonBank::Back()
{
  GUIModel::Instance().HistogramBack();
}

//......................................................................

void ButtonBank::Forward()
{
  GUIModel::Instance().HistogramForward();
}

//......................................................................

void ButtonBank::Pause()
{
  GUIModel::Instance().SetPlotViewRefreshPaused(true);
}

//......................................................................

void ButtonBank::Resume()
{
  GUIModel::Instance().SetPlotViewRefreshPaused(false);
}

//......................................................................

void ButtonBank::Histo()
{
  GUIModel::Instance().DrawContentProjection();
  fHisto->SetState(kButtonUp);
}

//......................................................................

void ButtonBank::PrintPlot()
{
  GUIModel::Instance().SendPrint();
}

//......................................................................

void ButtonBank::Update()
{
  GUIModel::Instance().SendUpdates();
}

//......................................................................

Bool_t ButtonBank::HandleTimer(TTimer* t __attribute__((unused)))
{

  if(GUIModel::Instance().Data().fPlotViewRefreshPaused == true) return true;

  this->Update();
  fTimer->SetTime(GUIModel::Instance().Data().fPlotViewRefreshRate);

  return true;

}

//......................................................................

ButtonBank::~ButtonBank()
{
  if(fBack)    { delete fBack;    fBack = 0; }
  if(fForward) { delete fForward; fForward = 0; }
  if(fUpdate)  { delete fUpdate;  fUpdate = 0; }
  if(fPause)   { delete fPause;   fPause = 0; }
  if(fResume)  { delete fResume;  fResume = 0; }
  if(fTimer)   { delete fTimer;   fTimer = 0; }
}

//......................................................................

void ButtonBank::GUIModelDataIssue(const GUIModelData& d,
				   unsigned int which)
{
  if (this->GetLock()==false) return;

  if ((which & kCurrentHistogramID)!=0) {
    bool back_ok = d.fCurrentHistogram.BackwardOK();
    bool back_en = (fBack->GetState()!=kButtonDisabled);
    if ( back_ok && !back_en) fBack->SetState(kButtonUp);
    if (!back_ok &&  back_en) fBack->SetState(kButtonDisabled);

    bool forward_ok = d.fCurrentHistogram.ForwardOK();
    bool forward_en = (fForward->GetState()!=kButtonDisabled);
    if ( forward_ok && !forward_en) fForward->SetState(kButtonUp);
    if (!forward_ok &&  forward_en) fForward->SetState(kButtonDisabled);

    bool update_ok = d.fCurrentHistogram.ReloadOK();
    bool update_en = (fUpdate->GetState()!=kButtonDisabled);
    if ( update_ok && !update_en) fUpdate->SetState(kButtonUp);
    if (!update_ok &&  update_en) fUpdate->SetState(kButtonDisabled);
  }
  if ((which | kPlotViewRefreshPausedID)) {
    bool paused = d.fPlotViewRefreshPaused;
    bool pause_en  = (fPause-> GetState()!=kButtonDisabled);
    bool resume_en = (fResume->GetState()!=kButtonDisabled);
    if ( paused &&  pause_en)  fPause-> SetState(kButtonDisabled);
    if (!paused && !pause_en)  fPause-> SetState(kButtonUp);
    if ( paused && !resume_en) fResume->SetState(kButtonUp);
    if (!paused &&  resume_en) fResume->SetState(kButtonDisabled);
  }

  this->ReleaseLock();
}

////////////////////////////////////////////////////////////////////////
