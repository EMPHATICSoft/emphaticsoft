#include "OnlineMonitoring/viewer/OStream.h"
#include "OnlineMonitoring/viewer/LogViewer.h"
#include <iostream>
using namespace emph::onmon;

OStream emph::onmon::cout;
OStream emph::onmon::cerr;

//......................................................................

OStream::OStream() : fLogViewer(0) { }

//......................................................................

void OStream::Flush() 
{
  if (fLogViewer) {
    fLogViewer->PrintToScreen(fBuf.str());
  }
  else {
    std::cout << fBuf.str() << std::endl;
  }
  fBuf.clear();
}

//......................................................................

void OStream::SetLogViewer(emph::onmon::LogViewer* lv)
{
  fLogViewer = lv;
}

////////////////////////////////////////////////////////////////////////
