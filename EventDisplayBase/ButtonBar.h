///
/// \file    ButtonBar.h
/// \brief   A bar containing the next event and related buttons
//  \version $Id: ButtonBar.h,v 1.4 2011-07-12 15:53:25 messier Exp $
/// \author  messier@indiana.edu
///
#ifndef EVDB_BUTTONBAR_H
#define EVDB_BUTTONBAR_H
#include "TQObject.h"
#include "RQ_OBJECT.h"
#include "TGButton.h"
#include "TGTextEntry.h"
class TGMainFrame;
class TGCompositeFrame;
class TGPictureButton;
class TGLayoutHints;
class TGLabel;

namespace evdb {
  class ButtonBar : public TObject {
    RQ_OBJECT("evdb::ButtonBar")

  public:
    ButtonBar(TGMainFrame* frame);
    virtual ~ButtonBar();

  public:
    void PrevEvt();
    void NextEvt();
    void ReloadEvt();
    void AutoAdvance();
    void FileList();
    void GoTo();
    void PrintToFile();
    int  NoImpl(const char* c);

    void SetRunEvent(int run, int event);

    Bool_t HandleTimer(TTimer* t);

  private:
    TTimer* fTimer; ///< Timer to handle auto advancing

  private:
    TGCompositeFrame* fButtonBar;   ///< The top button bar
    TGLayoutHints*    fLayout;      ///< Layout for button bar
    TGTextButton*     fPrevEvt;     ///< Goto to previous event
    TGTextButton*     fNextEvt;     ///< Goto to next event
    TGTextButton*     fAutoAdvance; ///< Start to auto advance
    TGTextButton*     fReload;      ///< Reload current event
    TGTextEntry*      fCurrentFile; ///< Currently loaded file
    TGPictureButton*  fFileList;    ///< Access to the list of files attached

    TGLabel*          fRunEvtLabel;    ///< Run/Event number label
    TGTextEntry*      fRunTextEntry;   ///< Run number text entry
    TGTextEntry*      fEventTextEntry; ///< Event number text entry

    TGTextButton*     fGoTo;  ///< Go To event button
    TGTextButton*     fPrint; ///< Print button

    ClassDef(ButtonBar, 0);
  };
}

#endif // EVDB_BUTTONBAR_H
////////////////////////////////////////////////////////////////////////
