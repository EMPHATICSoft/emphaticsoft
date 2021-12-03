#ifndef OM_HISTOGRAMINFO_H
#define OM_HISTOGRAMINFO_H
#include "TGTextView.h"
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"

namespace om { class GUIModelData; }

namespace om {
  class HistogramInfo : public TGTextView, public GUIModelSubscriber {
  public:
    HistogramInfo(TGWindow* w);
    ~HistogramInfo();

    ///
    /// Complete the GUIModelSubscriber interface
    ///
    void GUIModelDataIssue(const GUIModelData& d, unsigned int which);
    
  private:
    std::string fBuffer; ///< The text insider the buffer
    TGText*     fText;   ///< The text inside the box
  };
}
#endif
////////////////////////////////////////////////////////////////////////
