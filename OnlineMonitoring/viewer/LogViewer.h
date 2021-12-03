#ifndef OM_LOGVIEWER_H
#define OM_LOGVIEWER_H
#include <string>
#include "TGTextView.h"

namespace om {
  class LogViewer : public TGTextView {
  public:
    LogViewer(TGWindow *w);
    ~LogViewer();

    void PrintToScreen(const std::string& s);
    
  private:
  };
}

#endif
////////////////////////////////////////////////////////////////////////
