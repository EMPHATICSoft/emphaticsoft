////////////////////////////////////////////////////////////////////////
///
/// \file    ScanWindow.h
/// \brief   A window containing dialog boxes for handscans
/// \author  brebel@fnal.gov
/// \version $Id: ScanWindow.h,v 1.6 2012-08-10 17:01:17 messier Exp $
///
////////////////////////////////////////////////////////////////////////

#ifndef EVDB_SCANWINDOW_H
#define EVDB_SCANWINDOW_H

#include "EventDisplayBase/Canvas.h"
#include "RQ_OBJECT.h"
#include "TGCanvas.h"
#include "TGFrame.h"
#include <vector>
#include <iostream>
#include <fstream>


// Forward declarations
class TGNumberEntry;
class TGTextEntry;
class TGCheckButton;
class TGRadioButton;
class TGCanvas;
class TGMainFrame;
class TGCompositeFrame;
class TGGroupFrame;
class TGMatrixLayout;
class TGLayoutHints;
class TGLabel;
class TGTextButton;
class TGTextEntry;
class TGVScrollBar;

namespace evdb{

  /// Helper class to setup scroll bars in evdb::ScanWindow
  class ScanFrame {
    RQ_OBJECT("evdb::ScanFrame")
    
  public:
    ScanFrame(TGCompositeFrame* f);
    virtual ~ScanFrame();
    
    TGGroupFrame *GetFrame() const { return fFrame; }
    
    void SetCanvas(TGCanvas *canvas) { fCanvas = canvas; }
    void HandleMouseWheel(Event_t *event);
    void RadioButton();
    void ClearFields();
    void Record(std::string outfilename, 
		const char* comments);
    
    int  GetHeight() const;
    int  GetWidth() const;

  private:
    TGGroupFrame*  fFrame;
    TGCanvas*      fCanvas;
    TGLayoutHints* fFrameHints;
    TGLayoutHints* fFieldFrameHints;
    TGLayoutHints* fCatFrameLH;

    std::vector<TGGroupFrame*>      fCatFrames;      ///< Mother for a category
    std::vector<TGHorizontalFrame*> fFieldFrames;    ///< Mother for each field
    std::vector<TGTextEntry*>       fTextBoxes;      ///< Text box fields
    std::vector<TGLabel*>           fNumberLabels;   ///< Label for number fields
    std::vector<TGNumberEntry*>     fNumberBoxes;    ///< Number box fields
    std::vector<TGRadioButton*>     fRadioButtons;   ///< Radio button fields
    std::vector<TGCheckButton*>     fCheckButtons;   ///< Check button fields
    std::vector<int>                fRadioButtonIds; ///< Ids for the radio buttons
    
  };
}

//......................................................................

namespace evdb {

  class ScanWindow : public TGTransientFrame {

  public:
    
    RQ_OBJECT("evdb::ScanWindow")

  public:
    ScanWindow();
    ~ScanWindow();

    void CloseWindow();
    void Rec();
    void Prev();
    void Next();

    void BuildButtonBar(TGHorizontalFrame* f);
    void BuildUserFields(TGCompositeFrame* f);
    void OpenOutputFile();

  private:
    /// Scrollable frame for all user defined fields
    TGCanvas*         fUserFieldsCanvas;
    TGCompositeFrame* fUserFieldsFrame;
    TGLayoutHints*    fUserFieldsHints;

    /// Frame to hold the buttons at the bottom of the window
    TGHorizontalFrame* fButtonBar;
    TGLayoutHints*     fButtonBarHints;
    TGLabel*           fCommentLabel;
    TGTextEntry*       fCommentEntry;
    TGTextButton*      fPrevButton;
    TGTextButton*      fNextButton;
    TGTextButton*      fRcrdButton;
    TGLayoutHints*     fButtonBarHintsL;
    TGLayoutHints*     fButtonBarHintsC;
    TGLayoutHints*     fButtonBarHintsR;
    
    /// The frame containing the scanner check boxes etc.
    ScanFrame*  fScanFrame;
    std::string fOutFileName; ///< Output file name for scan results

    ClassDef(ScanWindow, 0)
  };

}// namespace

#endif //EVD_SCANVIEW_H
