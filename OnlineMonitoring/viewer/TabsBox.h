#ifndef OM_TABS_BOX
#define OM_TABS_BOX
#include "TGTab.h"
namespace om { class HistogramBrowser; }
namespace om { class ComparisonBox;    }

namespace om {
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
}

#endif
////////////////////////////////////////////////////////////////////////
