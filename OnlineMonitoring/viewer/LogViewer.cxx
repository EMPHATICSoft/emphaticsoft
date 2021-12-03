#include "OnlineMonitoring/viewer/LogViewer.h"
#include "OnlineMonitoring/viewer/Layout.h"
using namespace om;

LogViewer::LogViewer(TGWindow *w) : 
  TGTextView(w,Layout::fWindowSizeX,Layout::fLogViewerSizeY)
{
  const TGFont* font =
    gClient->GetFont("-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*");
  this->SetFont(font->GetFontStruct());
}

//......................................................................

LogViewer::~LogViewer() { }

void LogViewer::PrintToScreen(const std::string& s) 
{
  this->AddLine(s.c_str());
  this->ShowBottom();
  this->Update();
}

////////////////////////////////////////////////////////////////////////
