#include "OnlineMonitoring/viewer/CaptionBox.h"
#include <iostream>
#include <sstream>
#include "TGText.h"
#include "TGTextView.h"
#include "OnlineMonitoring/util/HistoTable.h"
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/util/TickerSubscriber.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/GUIModelData.h"
using namespace emph::onmon;

CaptionBox::CaptionBox(TGWindow* w) :
  TGTextView(w, 
	     Layout::fCaptionBoxSizeX,
	     Layout::fCaptionBoxSizeY),
  GUIModelSubscriber(kCurrentHistogramID)
{ 
  fText = new TGText("Histogram caption");
  TGFont* f =
    gClient->GetFont("-adobe-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");
  this->SetFont(f->GetFontStruct());
}

//......................................................................

CaptionBox::~CaptionBox() 
{
  delete fText;
}

//......................................................................

void CaptionBox::GUIModelDataIssue(const GUIModelData& d,
				   unsigned int which) 
{
  if (this->GetLock()==false) return;
  
  if ( (which&kCurrentHistogramID)==0 ) {
    this->ReleaseLock();
    return;
  }

  std::ostringstream oss;
  
  const char* hname = d.fCurrentHistogram.Current();
  oss << "Histogram " << hname << ":\n\n";
  
  const HistoData* hd = HistoTable::Instance().LookUp(hname);
  
  if (hd!=0) {
    static const unsigned int kNcharPerLine = 40;
    const char* c = hd->fCaption.c_str();
    unsigned int i=0;
    for (;*c!='\0';++c) {
      ++i;
      if (i>kNcharPerLine && *c==' ') { oss << '\n'; i=0; }
      else                            { oss << *c;        }
    }
    
    switch (hd->fReset) {
    case TickerSubscriber::k24hr:   oss << "\nReset at 0h GMT, ";                         break;
    case TickerSubscriber::kRun:    oss << "\nReset every run, ";                         break;
    case TickerSubscriber::kSubrun: oss << "\nReset every subrun, ";                      break;
    case TickerSubscriber::k30sec:  oss << "\nReset on the half minutes, ";               break;
    case TickerSubscriber::k1min:   oss << "\nReset on the minute, ";                     break;
    case TickerSubscriber::k5min:   oss << "\nReset on the 5's, ";                        break;
    case TickerSubscriber::k10min:  oss << "\nReset on the 10's, ";                       break;
    case TickerSubscriber::k30min:  oss << "\nReset on the hour and half-past the hour, ";break;
    case TickerSubscriber::kHour:   oss << "\nReset on the hour, ";                       break;
    case TickerSubscriber::kUTC:    oss << "\nShows the last 23 hours, ";                 break;
    }
    oss << "\n" << hd->fLookBack << " copies saved in history.\n\n";
    
    if (hd->fType==kTH1F) {
      oss << "1D Histogram\n"
	  << hd->fNx << " bins between "
	  << "x=" << hd->fX1 << " and "
	  << "x=" << hd->fX2 << "\n";
    }
    if (hd->fType==kTH2F) {
      oss << "2D Histogram\n"
	  << hd->fNx << " bins between "
	  << "x=" << hd->fX1 << " and "
	  << "x=" << hd->fX2 << "\n"
	  << hd->fNy << " bins between "
	  << "y=" << hd->fY1 << " and "
	  << "y=" << hd->fY2 << "\n";
    }
    oss << std::endl;
  }
  this->LoadBuffer(oss.str().c_str());
  this->Layout();

  this->ReleaseLock();
}

////////////////////////////////////////////////////////////////////////
