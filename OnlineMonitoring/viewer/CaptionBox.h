#ifndef OM_CAPTIONBOX_H
#define OM_CAPTIONBOX_H
#include "TGTextView.h"
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"

namespace om { class GUIModelData; }

namespace om {
  class CaptionBox : public TGTextView, public GUIModelSubscriber {
  public:
    CaptionBox(TGWindow* w);
    ~CaptionBox();

    ///
    /// Complete the GUIModelSubscriber interface
    ///
    void GUIModelDataIssue(const GUIModelData& d, unsigned int which);
    
  private:
    TGText* fText;   ///< The text inside the box
  };
}
#endif
////////////////////////////////////////////////////////////////////////
