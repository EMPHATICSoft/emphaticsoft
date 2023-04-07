#ifndef OM_LOGVIEWER_H
#define OM_LOGVIEWER_H
#include <string>
#include "TGTextView.h"

namespace emph { 
  namespace onmon {
    class LogViewer : public TGTextView {
    public:
      LogViewer(TGWindow *w);
      ~LogViewer();

      void PrintToScreen(const std::string& s);
      
    private:
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
