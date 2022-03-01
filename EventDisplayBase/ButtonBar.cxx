///
/// \file    ButtonBar.cxx
/// \brief   The button bar at the top of every display window
/// \version $Id: ButtonBar.cxx,v 1.6 2012-03-03 06:48:11 messier Exp $
/// \author  messier@indiana.edu
///
#include "EventDisplayBase/ButtonBar.h"
#include <iostream>
#include <string>
#include <cstdlib>
// ROOT includes
#include "TTimer.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGLayout.h"
#include "TGLabel.h"
#include "TGTextEntry.h"
#include "TGPicture.h"
#include "TGMsgBox.h"
// ART Framework includes
#include "art/Framework/Services/Registry/ServiceHandle.h"
// Local includes
#include "EventDisplayBase/EventDisplay.h"
#include "EventDisplayBase/evdb.h"
#include "EventDisplayBase/PrintDialog.h"
#include "EventDisplayBase/NavState.h"

namespace evdb{

  //......................................................................

  ButtonBar::ButtonBar(TGMainFrame* frame) :
    fTimer(0)
  {
    fButtonBar = new TGCompositeFrame(frame, 60, 20, 
				      kSunkenFrame|kHorizontalFrame);
    fLayout    = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 1, 0);

    // Previous event button
    fPrevEvt = new TGTextButton(fButtonBar, "<- Previous", 150);
    fPrevEvt->SetToolTipText("Go to previous event");
    fPrevEvt->Connect("Clicked()", "evdb::ButtonBar", this, "PrevEvt()");
    fButtonBar->AddFrame(fPrevEvt,
			 new TGLayoutHints(kLHintsTop | kLHintsLeft,
					   2, 0, 2, 2));
  
    // Next event button
    fNextEvt = new TGTextButton(fButtonBar, "Next ----->", 150);
    fNextEvt->SetToolTipText("Go to next event");
    fNextEvt->Connect("Clicked()", "evdb::ButtonBar", this, "NextEvt()");
    fButtonBar->AddFrame(fNextEvt,
			 new TGLayoutHints(kLHintsTop | kLHintsLeft,
					   2, 0, 2, 2));
  
    // Auto advance button
    fAutoAdvance = new TGTextButton(fButtonBar, ">", 150);
    fAutoAdvance->SetToolTipText("Start auto advance");
    fAutoAdvance->Connect("Clicked()", "evdb::ButtonBar", this, "AutoAdvance()");
    fButtonBar->AddFrame(fAutoAdvance,
			 new TGLayoutHints(kLHintsTop | kLHintsLeft,
					   2, 0, 2, 2));
  
    // Reload button
    fReload = new TGTextButton(fButtonBar, "Reload", 150);
    fReload->SetToolTipText("Reload current event");
    fReload->Connect("Clicked()", "evdb::ButtonBar", this, "ReloadEvt()");
    fButtonBar->AddFrame(fReload,
			 new TGLayoutHints(kLHintsTop | kLHintsLeft,
					   2, 0, 2, 2));
  
    fCurrentFile = new TGTextEntry(fButtonBar, new TGTextBuffer(256));
    fCurrentFile->SetToolTipText("Name of current file");
    fCurrentFile->Resize(400, fCurrentFile->GetDefaultHeight());
    fButtonBar->AddFrame(fCurrentFile, 
			 new TGLayoutHints(kLHintsTop | kLHintsLeft,
					   8, 2, 2, 2));

    // Button to list attached files
    const TGPicture* p = evdb::PicturePool()->GetPicture("arrow_down.xpm");
    GContext_t norm = TGPictureButton::GetDefaultGC()();
    fFileList = new TGPictureButton(fButtonBar, p, -1, norm, kRaisedFrame);
    fFileList->SetToolTipText("List files");
    fFileList->Connect("Clicked()", "evdb::ButtonBar", this, "FileList()");
    fButtonBar->AddFrame(fFileList,
			 new TGLayoutHints(kLHintsCenterY,
					   2, 0, 2, 2));

    // Print button
    fPrint = new TGTextButton(fButtonBar, "Print", 150);
    fPrint->SetToolTipText("Print display to a file");
    fPrint->Connect("Clicked()", "evdb::ButtonBar", this, "PrintToFile()");
    fButtonBar->AddFrame(fPrint,
			 new TGLayoutHints(kLHintsTop|kLHintsRight, 
					   2, 0, 2, 2));
  
    // Go To button
    fGoTo = new TGTextButton(fButtonBar, "Go");
    fGoTo->SetToolTipText("Go to event");
    fGoTo->Connect("Clicked()", "evdb::ButtonBar", this, "GoTo()");
    fButtonBar->AddFrame(fGoTo, new TGLayoutHints(kLHintsTop|kLHintsRight, 2, 0, 2, 2));

    // Go to event text entry
    fEventTextEntry = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
    fEventTextEntry->Connect("ReturnPressed()","evdb::ButtonBar",this,"GoTo()");
    fEventTextEntry->Resize(75,20);
    fButtonBar->AddFrame(fEventTextEntry, new TGLayoutHints(kLHintsTop|kLHintsRight,2,0,2,2));

    fRunTextEntry = new TGTextEntry(fButtonBar, new TGTextBuffer(128));
    fRunTextEntry->Connect("ReturnPressed()","evdb::ButtonBar",this,"GoTo()");
    fRunTextEntry->Resize(50,20);
    fButtonBar->AddFrame(fRunTextEntry, new TGLayoutHints(kLHintsCenterY|kLHintsRight,2,0,2,2));

    fRunEvtLabel = new TGLabel(fButtonBar, new TGHotString("[Run/Event]="));
    fButtonBar->AddFrame(fRunEvtLabel, new TGLayoutHints(kLHintsCenterY|kLHintsRight,2,0,2,2));

    // Add button bar to frame
    frame->AddFrame(fButtonBar, fLayout);
  }

  //......................................................................

  ButtonBar::~ButtonBar() 
  {
    if (fTimer) { delete fTimer; fTimer = 0; }

    delete fEventTextEntry; fEventTextEntry=0;
    delete fRunTextEntry;   fRunTextEntry  =0;
    delete fRunEvtLabel;    fRunEvtLabel   =0;
    delete fPrint;          fPrint         =0;
    delete fGoTo;           fGoTo          =0;
    delete fFileList;       fFileList      =0;
    delete fCurrentFile;    fCurrentFile   =0;
    delete fReload;         fReload        =0;
    delete fNextEvt;        fNextEvt       =0;
    delete fPrevEvt;        fPrevEvt       =0;
    delete fLayout;         fLayout        =0;
    delete fButtonBar;      fButtonBar     =0;
  }

  //......................................................................

  void ButtonBar::PrevEvt() 
  { 
    NavState::Set(kPREV_EVENT);
  }

  //......................................................................

  void ButtonBar::NextEvt() 
  { 
    NavState::Set(kNEXT_EVENT);
  }

  //......................................................................

  ///
  /// The timer sets the pace for the auto advance feature
  ///
  Bool_t ButtonBar::HandleTimer(TTimer* t)
  {
    this->NextEvt();

    art::ServiceHandle<evdb::EventDisplay> evd;
    t->SetTime(evd->fAutoAdvanceInterval);

    return kTRUE;
  }

  //......................................................................

  void ButtonBar::AutoAdvance() 
  { 
    if (fTimer==0) {
      //
      // Start the auto-advance feature
      //
      fAutoAdvance->SetText("X");
      fTimer = new TTimer;
      fTimer->SetObject(this);
    
      art::ServiceHandle<evdb::EventDisplay> evd;
      fTimer->Start(evd->fAutoAdvanceInterval);
    }
    else {
      //
      // Stop the auto-advance
      //
      fAutoAdvance->SetText(">");
      fTimer->Stop();
      delete fTimer;
      fTimer = 0;
    }
  }

  //......................................................................

  void ButtonBar::ReloadEvt() 
  { 
    NavState::Set(kRELOAD_EVENT);
  }

  //......................................................................

  void ButtonBar::FileList() 
  {
    // int i;
    // const char* f;
    // for (i=0; (f=IoModule::Instance()->FileName(i)); ++i) {
    // std::cerr << f << std::endl;
    // }
  }

  //......................................................................

  void ButtonBar::PrintToFile() { new evdb::PrintDialog(); }

  //......................................................................

  void ButtonBar::GoTo() 
  {
    int run = atoi(fRunTextEntry  ->GetText());
    int evt = atoi(fEventTextEntry->GetText());
    NavState::SetTarget(run, evt);
    NavState::Set(kGOTO_EVENT);
  }

  //......................................................................

  void ButtonBar::SetRunEvent(int run, int evt) 
  {
    char runtxt[128];
    char evttxt[128];

    sprintf(runtxt,"%d",run);
    sprintf(evttxt,"%d",evt);
    fRunTextEntry  ->SetText(runtxt);
    fEventTextEntry->SetText(evttxt);
  }

  //......................................................................

  int ButtonBar::NoImpl(const char* method)
  {
    std::string s;
    s = "Sorry action '"; s += method; s+= "' is not implemented.\n";
    new TGMsgBox(evdb::TopWindow(), fButtonBar,
		 "No implementation",s.c_str(),kMBIconExclamation);
    return 0;
  }

}// namespace

////////////////////////////////////////////////////////////////////////
