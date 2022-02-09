#ifndef OM_TABS_BOX
#define OM_TABS_BOX
#include "TGTab.h"
namespace emph { namespace onmon { class HistogramBrowser; } }
namespace emph { namespace onmon { class ComparisonBox;    } }

namespace emph { 
  namespace onmon {
    class TabsBox : public TGTab {
    public:
      TabsBox(TGWindow* w);
      ~TabsBox();
    private:
      HistogramBrowser* fHistoBrowser;
      ComparisonBox*    fComparisonBox;
      TGLayoutHints*    fHistoBrowserHints;
      TGLayoutHints*    fComparisonBoxHints;
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
