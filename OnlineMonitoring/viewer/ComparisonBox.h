#ifndef OM_COMPARISONBOX_H
#define OM_COMPARISONBOX_H
#include "RQ_OBJECT.h"
#include "TGFrame.h"
#include "OnlineMonitoring/viewer/ComparisonOptions.h"
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"
#include "OnlineMonitoring/viewer/Icons.h"
class TGWindow;
class TGRadioButton;
class TGPictureButton;
class TGTextButton;
class TGListBox;
class TGTextEntry;

namespace emph { 
  namespace onmon {
    /// A box showing options used to make comparisons between current
    /// and past histograms
    class ComparisonBox : public TGCompositeFrame, public GUIModelSubscriber {
      RQ_OBJECT("ComparisonBox")
    public:
      ComparisonBox(const TGWindow* win,
  		  unsigned int w,
  		  unsigned int h,
  		  unsigned int opt);
      ~ComparisonBox();
      void HandleCompareWhichButtons(int i=-1);
      void HandleCompareHowButtons(int i=-1);
      void HandleCompareNormButtons(int i=-1);
      void HandleApplyOptions();
      void HandleFileBrowse();

      void GUIModelDataIssue(const GUIModelData& m,
  			   unsigned int which);

    private:
      void UpdateButtonStates();

      void LayoutWhichFrame();
      void LayoutHowFrame();
      void LayoutNormFrame();
      void LayoutReferenceFile();
      void LayoutRecent();

    private:
      TGGroupFrame*    fCompareWhichFrame;
      TGRadioButton*   fCompareWhichButtons[ComparisonOptions::kMaxCompWhich];

      TGHorizontalFrame* fReferenceFile;
      TGTextEntry*       fReferenceFileText;
      TGPictureButton*   fReferenceFileBrowse;

      TGListBox*         fLookBack;

      TGGroupFrame*  fCompareHowFrame;
      TGRadioButton* fCompareHowButtons[ComparisonOptions::kMaxCompHow];

      TGGroupFrame*  fCompareNormFrame;
      TGRadioButton* fCompareNormButtons[ComparisonOptions::kMaxCompNorm];

      TGTextButton*  fApplyOptions;

    ClassDef(ComparisonBox,0)
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
