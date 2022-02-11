///
/// \file  PrintDialog.h
/// \brief Pop up dialog for printing
///
/// \author  messier@indiana.edu
/// \version $Id: PrintDialog.cxx,v 1.2 2011-07-12 15:53:25 messier Exp $
///
#include <map>
#include <string>
// ROOT includes
#include "TGWindow.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGButton.h"
#include "TGTextEntry.h"

#include "EventDisplayBase/PrintDialog.h"
#include "EventDisplayBase/Printable.h"
#include "EventDisplayBase/EventHolder.h"
#include "EventDisplayBase/evdb.h"

namespace evdb{

  static std::map<std::string,bool> gsPrintableSelection;
  static std::map<std::string,bool> gsFormatSelection;

  //......................................................................

  PrintDialog::PrintDialog() : 
    TGTransientFrame(0,0,800,300,0),
    fNprintable(0)
  {
    fL1 = new TGLayoutHints(kLHintsLeft|kLHintsTop|kLHintsExpandX, 2, 2, 2, 2);
    fL2 = new TGLayoutHints(kLHintsLeft|kLHintsTop, 2, 2, 2, 2);
  
    // Add list of printable object to dialog box. Make a check box for
    // each one:
    //
    // Description     | Filename                   Format
    // ----------------| -------------------------------------------------
    // [*] Main window | evd.main.001234.000123456 [ ].eps [ ].ps [*].gif
    // [*] TPC display | evd.tpc.001234.000123456  [ ].eps [ ].ps [*].gif
    //
    int wPrintable  = 500; // Width of description field
    int wFilename   = 200; // Width of filename field
    int wCheckBox   = 100; // Width of check boxes
    int h           = 20;  // Height of fields
    int i           = 0;   // Counter
    std::map<std::string,Printable*>& 
      printables = Printable::GetPrintables();
    std::map<std::string,Printable*>::iterator itr(printables.begin());
    std::map<std::string,Printable*>::iterator itrEnd(printables.end());
    for (; itr!=itrEnd; ++itr) {
      fPrintTag[i]   = itr->first;
      fPrintable[i]  = itr->second;
      fPrintFrame[i] = new TGHorizontalFrame(this,20,20);

      std::string ptag;
      std::string base;
      std::string filename;
      base =  "evd.";
      base += itr->second->PrintTag();

      char runevt[128];
      sprintf(runevt,".%d.%d",
	      evdb::EventHolder::Instance()->GetEvent()->run(),
	      evdb::EventHolder::Instance()->GetEvent()->id().event());
      base += runevt;

      // Title of the printable object
      fPrintableCB[i] = new TGCheckButton(fPrintFrame[i], itr->first.c_str());
      fPrintableCB[i]->Resize(wPrintable, h);
      fPrintFrame[i]->AddFrame(fPrintableCB[i], fL1);
      if (gsPrintableSelection[fPrintTag[i]]) {
	fPrintableCB[i]->SetState(kButtonDown);
      }

      // Base file name to use during print
      fFilename[i] = new TGTextEntry(fPrintFrame[i], new TGTextBuffer(256));
      fFilename[i]->SetToolTipText("Base file name for print");
      fFilename[i]->SetText(base.c_str());
      fFilename[i]->Resize(wFilename,h);
      fPrintFrame[i]->AddFrame(fFilename[i], fL2);

      // PNG check box
      fDoPNG[i] = new TGCheckButton(fPrintFrame[i], ".png");
      fDoPNG[i]->Resize(wCheckBox,h);
      fPrintFrame[i]->AddFrame(fDoPNG[i], fL2);
      ptag = fPrintTag[i]; ptag += ".png";
      if (gsFormatSelection[ptag]) fDoPNG[i]->SetState(kButtonDown);

      // GIF check box
      fDoGIF[i] = new TGCheckButton(fPrintFrame[i], ".gif");
      fDoGIF[i]->Resize(wCheckBox,h);
      fPrintFrame[i]->AddFrame(fDoGIF[i], fL2);
      ptag = fPrintTag[i]; ptag += ".gif";
      if (gsFormatSelection[ptag]) fDoGIF[i]->SetState(kButtonDown);

      // PDF check box
      fDoPDF[i] = new TGCheckButton(fPrintFrame[i], ".pdf");
      fDoPDF[i]->Resize(wCheckBox,h);
      fPrintFrame[i]->AddFrame(fDoPDF[i], fL2);
      ptag = fPrintTag[i]; ptag += ".pdf";
      if (gsFormatSelection[ptag]) fDoPDF[i]->SetState(kButtonDown);

      // EPS check box
      fDoEPS[i] = new TGCheckButton(fPrintFrame[i], ".eps");
      fDoEPS[i]->Resize(wCheckBox,h);
      fPrintFrame[i]->AddFrame(fDoEPS[i], fL2);
      ptag = fPrintTag[i]; ptag += ".eps";
      if (gsFormatSelection[ptag]) fDoEPS[i]->SetState(kButtonDown);
    
      ++i;
    }
    fNprintable = i;

    // Build the button frame along the bottom
    fButtonFrame = new TGHorizontalFrame(this,20,20);

    fPrintButton = new TGTextButton(fButtonFrame,"&Print",150);
    fPrintButton->Connect("Clicked()", 
			  "evdb::PrintDialog",
			  this,
			  "PrintToFile()");
  
    fButtonFrame->AddFrame(fPrintButton, 
			   new TGLayoutHints(kLHintsLeft,4,4,4,4));

    fCancelButton = new TGTextButton(fButtonFrame,"&Cancel",150);
    fCancelButton->Connect("Clicked()", "evdb::PrintDialog", this, "Cancel()");
    fButtonFrame->AddFrame(fCancelButton, 
			   new TGLayoutHints(kLHintsRight,4,4,4,4));
  
    // Layout the main frame
    for (int i=0; i<fNprintable; ++i) this->AddFrame(fPrintFrame[i],fL1);
    this->AddFrame(fButtonFrame);
    this->MapSubwindows();
    this->Resize(500, fNprintable*(h+8)+38);
  
    this->SetWindowName("Print Dialog");
    this->MapWindow();
  
    this->Connect("CloseWindow()","evdb::PrintDialog",this,"CloseWindow()");
  }

  //......................................................................

  void PrintDialog::CloseWindow() { delete this; }

  //......................................................................

  PrintDialog::~PrintDialog() 
  {
    for (int i=0; i<fNprintable; ++i) {
      delete fDoPNG[i]; 
      delete fDoGIF[i]; 
      delete fDoEPS[i];
      delete fDoPDF[i];
      delete fFilename[i];
      delete fPrintableCB[i];
      delete fPrintFrame[i];
    }
    delete fPrintButton; 
    delete fCancelButton; 
    delete fButtonFrame;
    delete fL1;
    delete fL2;
  }

  //......................................................................

  void PrintDialog::Cancel() { this->SendCloseMessage(); }

  //......................................................................

  void PrintDialog::PrintToFile() 
  {
    int         nFormats = 4;
    std::string format[4] = { ".png", ".gif", ".pdf", ".eps" };
    bool        doPrint[4];

    for (int i=0; i<fNprintable; ++i) {
      bool printMe = (fPrintableCB[i]->GetState() == kButtonDown);

      // Remember which printables are selected for use next time
      gsPrintableSelection[fPrintTag[i]] = printMe;
    
      // Handle the print
      if (printMe) {
	std::string base = fFilename[i]->GetText();

	doPrint[0] = (fDoPNG[i]->GetState() == kButtonDown);
	doPrint[1] = (fDoGIF[i]->GetState() == kButtonDown);
	doPrint[2] = (fDoPDF[i]->GetState() == kButtonDown);
	doPrint[3] = (fDoEPS[i]->GetState() == kButtonDown);
	for (int j=0; j<nFormats; ++j) {
	  // Printable tag (with format) and file name
	  std::string ftag(fPrintTag[i]); ftag += format[j];
	  std::string f(base); f += format[j];
	
	  // Remember format choices for next time
	  gsFormatSelection[ftag] = doPrint[j];
	
	  // Actually do the print
	  if (doPrint[j]) {
	    fPrintable[i]->Print(f.c_str());
	  }

	} // Loop on formats
      } // If printable is checked
    } // Loop on printables

    // Done printing. Cancel the window
    this->Cancel();
  }

}// namespace
////////////////////////////////////////////////////////////////////////
