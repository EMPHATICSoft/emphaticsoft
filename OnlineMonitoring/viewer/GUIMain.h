#ifndef OM_GUIMAIN_H
#define OM_GUIMAIN_H
#include "TGFrame.h"
#include "RQ_OBJECT.h"

class TGTableLayout;
class TGTableLayoutHints;
namespace emph { namespace onmon { class MenuBar;       } }
namespace emph { namespace onmon { class HeaderBar;     } }
namespace emph { namespace onmon { class TabsBox;       } }
namespace emph { namespace onmon { class CaptionBox;    } }
namespace emph { namespace onmon { class PlotViewer;    } }
namespace emph { namespace onmon { class LogViewer;     } }
namespace emph { namespace onmon { class HistogramInfo; } }
namespace emph { namespace onmon { class ButtonBank;    } }

namespace emph { 
  namespace onmon {
    class GUIMain : public TGMainFrame {
      RQ_OBJECT("emph::onmon::GUIMain")
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
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
