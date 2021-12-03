#include "OnlineMonitoring/viewer/OStream.h"
#include "OnlineMonitoring/viewer/LogViewer.h"
#include <iostream>
using namespace om;

OStream om::cout;
OStream om::cerr;

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

void OStream::SetLogViewer(om::LogViewer* lv)
{
  fLogViewer = lv;
}

////////////////////////////////////////////////////////////////////////
