#include "OnlineMonitoring/viewer/HistogramInfo.h"
#include <iostream>
#include "TGText.h"
#include "TGTextView.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/GUIModelData.h"
using namespace emph::onmon;

HistogramInfo::HistogramInfo(TGWindow* w) :
  TGTextView(w, 
	     Layout::fPlotViewerSizeX,
	     Layout::fHistogramInfoSizeY),
  GUIModelSubscriber(kHistogramInfoTextID)
{ 
  fText = new TGText("Click on plot for histogram data");
  TGFont* f =
    gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
  this->SetFont(f->GetFontStruct());
}

//......................................................................

HistogramInfo::~HistogramInfo() 
{
  delete fText;
}

//......................................................................

void HistogramInfo::GUIModelDataIssue(const GUIModelData& d,
				      unsigned int which) 
{
  if (this->GetLock()==false) return;
  
  if ((which&kHistogramInfoTextID)==0) {
    this->ReleaseLock();
    return;
  }
  
  if (fBuffer == d.fHistogramInfoText) {
    this->ReleaseLock();
    return;
  }
  fBuffer = d.fHistogramInfoText;
  this->LoadBuffer(fBuffer.c_str());
  
  this->ReleaseLock();
}

////////////////////////////////////////////////////////////////////////
