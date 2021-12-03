#include "OnlineMonitoring/viewer/ComparisonBox.h"
#include <iostream>
#include "TGButton.h"
#include "TGListBox.h"
#include "TGTextEntry.h"
#include "TGFileDialog.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/viewer/ComparisonOptions.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/viewer/GUIModelData.h"
using namespace om;

ComparisonBox::ComparisonBox(const TGWindow* win,
			     unsigned int w,
			     unsigned int h,
			     unsigned int opt) :
  TGCompositeFrame(win, w, h, opt),
  GUIModelSubscriber(kCurrentHistogramID|kComparisonOptionsID|kRefFileInitID)
{
  TGLayoutHints* xx = new TGLayoutHints(kLHintsExpandX);

  fCompareWhichFrame = new TGGroupFrame(this, "Comparison to...");
  this->LayoutWhichFrame();
  this->AddFrame(fCompareWhichFrame, xx);

  fCompareHowFrame   = new TGGroupFrame(this, "Comparison method");
  this->LayoutHowFrame();
  this->AddFrame(fCompareHowFrame,   xx);

  fCompareNormFrame  = new TGGroupFrame(this, "Normalization method");
  this->LayoutNormFrame();
  this->AddFrame(fCompareNormFrame,  xx);

  fApplyOptions      = new TGTextButton(this, "Apply Options");
  fApplyOptions->SetToolTipText("Apply chosen comparison options now");
  fApplyOptions->Connect("Clicked()", "om::ComparisonBox",
			 this, "HandleApplyOptions()");
  this->AddFrame(fApplyOptions,      xx);
}

//......................................................................

void ComparisonBox::LayoutWhichFrame() 
{
  unsigned int i;

  TGLayoutHints* fCompWhichLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft);
  for (i=0; i<ComparisonOptions::kMaxCompWhich; ++i) {
    fCompareWhichButtons[i] = 
      new TGRadioButton(fCompareWhichFrame, 
			ComparisonOptions::CompareTo(i),
			i);
    fCompareWhichButtons[i]->Connect("Clicked()",
				     "om::ComparisonBox",
				     this,
				     "HandleCompareWhichButtons()");
    fCompareWhichFrame->AddFrame(fCompareWhichButtons[i],fCompWhichLayout);

    if (i==ComparisonOptions::kReference) {
      this->LayoutReferenceFile();
    }
    
    if (i==ComparisonOptions::kRecent) {
      this->LayoutRecent();
    }

  }
}

//......................................................................
  
//
// Make a horizontal frame to hold the text and button
//
void ComparisonBox::LayoutReferenceFile() 
{
  fReferenceFile = new TGHorizontalFrame(fCompareWhichFrame);
  
  //
  // Layout the two widgets inside the horizontal frame
  //
  fReferenceFileText   = new TGTextEntry(fReferenceFile);
  fReferenceFileText->SetText("choose reference file");

  TGLayoutHints* xx = new TGLayoutHints(kLHintsExpandX);      
  fReferenceFile->AddFrame(fReferenceFileText,   xx);

  TGLayoutHints* yy = new TGLayoutHints(kLHintsRight);
  fReferenceFileBrowse = new TGPictureButton(fReferenceFile,
					     Icons::FolderExplore());
  fReferenceFileBrowse->SetToolTipText("Browse to open a file");
  fReferenceFileBrowse->Connect("Clicked()", "om::ComparisonBox",
				this, "HandleFileBrowse()");
  fReferenceFile->AddFrame(fReferenceFileBrowse, yy);
  
  fCompareWhichFrame->AddFrame(fReferenceFile, xx);
}

//......................................................................

void ComparisonBox::LayoutRecent() 
{
  fLookBack = new TGListBox(fCompareWhichFrame, 89);
  fLookBack->Resize(30,60);
  
  TGLayoutHints* xx = new TGLayoutHints(kLHintsExpandX);
  fCompareWhichFrame->AddFrame(fLookBack, xx);
}

//......................................................................

void ComparisonBox::LayoutHowFrame() 
{
  unsigned int i;

  TGLayoutHints* fCompHowLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft);
  for (i=0; i<ComparisonOptions::kMaxCompHow; ++i) {
    fCompareHowButtons[i] =
      new TGRadioButton(fCompareHowFrame, 
			ComparisonOptions::CompareMethod(i),
			i);
    fCompareHowButtons[i]->Connect("Clicked()",
				   "om::ComparisonBox",
				   this,
				   "HandleCompareHowButtons()");
    fCompareHowFrame->AddFrame(fCompareHowButtons[i],fCompHowLayout);
  }
}

//......................................................................

void ComparisonBox::LayoutNormFrame() 
{
  unsigned int i;

  TGLayoutHints* fCompNormLayout = new TGLayoutHints(kLHintsTop|kLHintsLeft);
  for (i=0; i<ComparisonOptions::kMaxCompNorm; ++i) {
    fCompareNormButtons[i] =
      new TGRadioButton(fCompareNormFrame, 
			ComparisonOptions::CompareNormalize(i),
			i);
    fCompareNormButtons[i]->Connect("Clicked()",
				    "om::ComparisonBox",
				    this,
				    "HandleCompareNormButtons()");
    fCompareNormFrame->AddFrame(fCompareNormButtons[i],fCompNormLayout);
  }
}

//......................................................................

void ComparisonBox::HandleCompareWhichButtons(int id) 
{
  if (id==-1) {
    TGButton* btn = (TGButton*)gTQSender;
    id = btn->WidgetId();
  }

  GUIModel::Instance().SetCompareWhich(id);
}

//......................................................................

void ComparisonBox::HandleCompareHowButtons(int id) 
{
  if (id==-1) {
    TGButton* btn = (TGButton*)gTQSender;
    id = btn->WidgetId();
  }

  GUIModel::Instance().SetCompareMethod(id);

}

//......................................................................

void ComparisonBox::HandleCompareNormButtons(int id) 
{
  if (id==-1) {
    TGButton* btn = (TGButton*)gTQSender;
    id = btn->WidgetId();
  }

  GUIModel::Instance().SetCompareNorm(id);

}

//......................................................................

void ComparisonBox::HandleApplyOptions() 
{
  GUIModel::Instance().SetLookBack(fLookBack->GetSelected());
  GUIModel::Instance().Publish(kComparisonUpdateID);
}

//......................................................................

void ComparisonBox::HandleFileBrowse() 
{
  const char *filetypes[] = { "All ROOT files",   "*.root",
			      "End subrun files", "*endsubrun*.root",
			      "End run files",    "*endrun*.root",
			      "All files",        "*",
			      0,                  0 };
  TGFileInfo fi;
  fi.fFileTypes = filetypes;

  ///\todo: Set the startup directory to an appropriate directory of reference files
  fi.fIniDir    = StrDup(".");
  new TGFileDialog(gClient->GetRoot(), new TGMainFrame(gClient->GetRoot(), 400, 220), kFDOpen, &fi);
  if(fi.fFilename != 0) {
    printf("Open file: %s\n", fi.fFilename);
    fReferenceFileText->SetText(fi.fFilename);
    GUIModel::Instance().SetCompareWhich(ComparisonOptions::kReference);
    GUIModel::Instance().SetReferenceFile(fReferenceFileText->GetText());
  }
}

//......................................................................

void ComparisonBox::GUIModelDataIssue(const GUIModelData& m, 
				      unsigned int which) 
{
  if ( (which&kRefFileInitID) !=0 ) {
    fReferenceFileText->SetText(om::GUIModel::Instance().Data().fComparisonOpt.fReferenceFile.c_str());
  }

  if ( (which&kComparisonOptionsID) != 0 || (which&kRefFileInitID)!= 0 ) {
    unsigned int i;
    for (i=0; i<ComparisonOptions::kMaxCompWhich; ++i) {
      if (m.fComparisonOpt.fWhich == i) {
	fCompareWhichButtons[i]->SetState(kButtonDown);
      }
      else {
	fCompareWhichButtons[i]->SetState(kButtonUp);
      }
    }
    
    for (i=0; i<ComparisonOptions::kMaxCompHow; ++i) {
      if (m.fComparisonOpt.fMethod == i) {
	fCompareHowButtons[i]->SetState(kButtonDown);
      }
      else {
	fCompareHowButtons[i]->SetState(kButtonUp);
      }
    }
    
    for (i=0; i<ComparisonOptions::kMaxCompNorm; ++i) {
      if (m.fComparisonOpt.fNormalize == i) {
	fCompareNormButtons[i]->SetState(kButtonDown);
      }
      else {
	fCompareNormButtons[i]->SetState(kButtonUp);
      }
    }
  }

  if ( (which&kCurrentHistogramID)!=0 ) {
    fLookBack->RemoveAll();
    const HistoData* hd = 
      HistoTable::Instance().LookUp(m.fCurrentHistogram.Current());
    if (hd!=0) {
      for (unsigned int i=0; i<hd->fLookBack; ++i) {
	char buff[256];
	sprintf(buff, "look back %d",i+1);
	fLookBack->AddEntry(buff, i);
      }
    }
    fLookBack->Layout();
  }
}

//......................................................................

ComparisonBox::~ComparisonBox() 
{
  
  unsigned int i;

  if(fCompareWhichFrame) {
    delete fCompareWhichFrame;
    fCompareWhichFrame = 0;
  }
  for (i=0; i<ComparisonOptions::kMaxCompWhich; ++i) {
    if(fCompareWhichButtons[i]) {
      delete fCompareWhichButtons[i];
      fCompareWhichButtons[i] = 0;
    }
  }

  if(fReferenceFile) {
    delete fReferenceFile;
    fReferenceFile = 0;
  }  
  if(fReferenceFileText) {
    delete fReferenceFileText;
    fReferenceFileText = 0;
  }
  if(fReferenceFileBrowse) {
    delete fReferenceFileBrowse;
    fReferenceFileBrowse = 0;
  }

  if(fLookBack) {
    delete fLookBack;
    fLookBack = 0;
  }

  if(fCompareHowFrame) {
    delete fCompareHowFrame;
    fCompareHowFrame = 0;
  }
  for (i=0; i<ComparisonOptions::kMaxCompHow; ++i) {
    if(fCompareHowButtons[i]) {
      delete fCompareHowButtons[i];
      fCompareHowButtons[i] = 0;
    }
  }

  if(fCompareNormFrame) {
    delete fCompareNormFrame;
    fCompareNormFrame = 0;
  }
  for (i=0; i<ComparisonOptions::kMaxCompNorm; ++i) {
    if(fCompareNormButtons[i]) {
      delete fCompareNormButtons[i];
      fCompareNormButtons[i] = 0;
    }
  }

  if(fApplyOptions) {
    delete fApplyOptions;
    fApplyOptions = 0;
  }
  
}

////////////////////////////////////////////////////////////////////////
