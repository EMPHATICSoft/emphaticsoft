#ifndef OM_GUIMAIN_H
#define OM_GUIMAIN_H
#include "TGFrame.h"
#include "RQ_OBJECT.h"

class TGTableLayout;
class TGTableLayoutHints;
namespace om { class MenuBar;       }
namespace om { class HeaderBar;     }
namespace om { class TabsBox;       }
namespace om { class CaptionBox;    }
namespace om { class PlotViewer;    }
namespace om { class LogViewer;     }
namespace om { class HistogramInfo; }
namespace om { class ButtonBank;    }

namespace om {
  class GUIMain : public TGMainFrame {
    RQ_OBJECT("om::GUIMain")
  public:
    GUIMain();
    ~GUIMain();

    void CloseWindow();

  private:
    TGTableLayout*      fLayoutManager;
    TGTableLayoutHints* fMenuBarHints;
    TGTableLayoutHints* fHeaderBarHints;
    TGTableLayoutHints* fTabsBoxHints;
    TGTableLayoutHints* fCaptionBoxHints;
    TGTableLayoutHints* fHistogramInfoHints;
    TGTableLayoutHints* fButtonBankHints;
    TGTableLayoutHints* fPlotViewerHints;
    TGTableLayoutHints* fLogViewerHints;

    MenuBar*       fMenuBar;
    HeaderBar*     fHeaderBar;
    TabsBox*       fTabsBox;
    CaptionBox*    fCaptionBox;
    HistogramInfo* fHistogramInfo;
    ButtonBank*    fButtonBank;
    PlotViewer*    fPlotViewer;
    LogViewer*     fLogViewer;

    ClassDef(GUIMain,0)
  };
}

#endif
////////////////////////////////////////////////////////////////////////
