#ifndef OM_CAPTIONBOX_H
#define OM_CAPTIONBOX_H
#include "TGTextView.h"
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"

namespace emph { namespace onmon { class GUIModelData; } }

namespace emph { 
  namespace onmon {
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
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
