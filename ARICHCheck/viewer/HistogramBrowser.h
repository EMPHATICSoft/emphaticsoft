#ifndef OM_HISTOGRAMBROWSER_H
#define OM_HISTOGRAMBROWSER_H
#include <string>
#include <map>
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"
#include "RQ_OBJECT.h"
#include "TGFrame.h"

class TGCanvas;
class TGListTree;
class TGListTreeItem;
class TGPicture;
class TGTextButton;

namespace emph { namespace onmon { class HistoData; } }

namespace emph { 
  namespace onmon {
    class HistogramBrowser : public TGCompositeFrame, public GUIModelSubscriber {
      RQ_OBJECT("HistogramBrowser")
    public:
      HistogramBrowser(const TGWindow* p,
  		     unsigned int w,
  		     unsigned int h,
  		     unsigned int opt);
      ~HistogramBrowser();

      TGListTreeItem* FindCategory(const char* nm);
      TGListTreeItem* MakeCategory(const char* nm);

      void DoubleClicked(TGListTreeItem*, Int_t i);

      void HandleRefresh();

      // Complete the GUIModelSubscriber interface
      void GUIModelDataIssue(const GUIModelData& d, unsigned int which);

    private:
      void BuildTree();
      void ClearBrowser();
      void MakeCategories();
      void Populate();

    private:
      TGCanvas*                             fCanvas;
      TGListTree*                           fListTree;
      std::map<std::string,TGListTreeItem*> fCategories;
      std::map<std::string,TGListTreeItem*> fHistograms;

      TGTextButton*                         fRefresh;

    ClassDef(HistogramBrowser,0)
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
