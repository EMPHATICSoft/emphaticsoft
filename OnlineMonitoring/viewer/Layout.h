#ifndef OM_LAYOUT_H
#define OM_LAYOUT_H

namespace om {
  class Layout {
  public:
    static const unsigned int fWindowSizeX = 1200;
    static const unsigned int fWindowSizeY = 800;
    
    static const unsigned int fMenuBarSizeX = fWindowSizeX;
    static const unsigned int fMenuBarSizeY = 30;

    static const unsigned int fHeaderSizeX = fWindowSizeX;
    static const unsigned int fHeaderSizeY = 50;
    
    static const unsigned int fLogViewerSizeX = fWindowSizeX;
    static const unsigned int fLogViewerSizeY = 80;

    static const unsigned int fCaptionBoxSizeX = 290;
    static const unsigned int fCaptionBoxSizeY = 150;
    
    // static const unsigned int fBrowserTabSizeX = fCaptionBoxSizeX;
    static const unsigned int fBrowserTabSizeX = 260;
    static const unsigned int fBrowserTabSizeY = 
      fWindowSizeY - 
      (fMenuBarSizeY+fHeaderSizeY+fCaptionBoxSizeY+fLogViewerSizeY);

    static const unsigned int fButtonBankSizeX = 6*24;
    static const unsigned int fButtonBankSizeY = 36;

    static const unsigned int fHistogramInfoSizeX = 
      fWindowSizeX - 
      (fBrowserTabSizeX+fButtonBankSizeX);
    static const unsigned int fHistogramInfoSizeY = fButtonBankSizeY;
    
    static const unsigned int fPlotViewerSizeX = fHistogramInfoSizeX-150;
    static const unsigned int fPlotViewerSizeY = 
      fWindowSizeY - 
      (fMenuBarSizeY+fHeaderSizeY+fLogViewerSizeY+fHistogramInfoSizeY+100);
  };
}

#endif
////////////////////////////////////////////////////////////////////////
