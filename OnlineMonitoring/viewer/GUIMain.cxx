#include "TGTableLayout.h"
#include "TApplication.h"
#include "OnlineMonitoring/viewer/GUIMain.h"
#include "OnlineMonitoring/viewer/MenuBar.h"
#include "OnlineMonitoring/viewer/HeaderBar.h"
#include "OnlineMonitoring/viewer/TabsBox.h"
#include "OnlineMonitoring/viewer/CaptionBox.h"
#include "OnlineMonitoring/viewer/HistogramInfo.h"
#include "OnlineMonitoring/viewer/ButtonBank.h"
#include "OnlineMonitoring/viewer/PlotViewer.h"
#include "OnlineMonitoring/viewer/LogViewer.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/OStream.h"

using namespace emph::onmon;

GUIMain::GUIMain() :
  TGMainFrame(gClient->GetRoot(), 
	      Layout::fWindowSizeX-1, 
	      Layout::fWindowSizeY-1)
{
  int ncol = 3;
  int nrow = 6;
  fLayoutManager = new TGTableLayout(this, nrow, ncol);
  this->SetLayoutManager(fLayoutManager);

  int tl = kLHintsTop|kLHintsLeft;
  int tr = kLHintsTop|kLHintsRight;
  int bl = kLHintsBottom|kLHintsLeft;
  int xx = kLHintsFillX|kLHintsExpandX;
  int xy = kLHintsFillY|kLHintsExpandY;


  //                                           L/R  T/B
  fMenuBarHints       = new TGTableLayoutHints(0,3, 0,1, tl|xx,    2, 2, 2, 2);
  fHeaderBarHints     = new TGTableLayoutHints(0,3, 1,2, tl|xx,    2, 2, 2, 2);
  fButtonBankHints    = new TGTableLayoutHints(1,2, 2,3, tl,       2, 2, 2, 2);
  fHistogramInfoHints = new TGTableLayoutHints(2,3, 2,3, tr|xx,    2, 2, 2, 2);
  fCaptionBoxHints    = new TGTableLayoutHints(0,1, 4,5, bl|xy,    2, 2, 2, 2);
  fLogViewerHints     = new TGTableLayoutHints(0,3, 5,6, bl|xx|xy, 2, 2, 2, 2);
  fTabsBoxHints       = new TGTableLayoutHints(0,1, 3,4, tl|xy,    2, 2, 2, 2);
  fPlotViewerHints    = new TGTableLayoutHints(1,3, 3,5, tl|xx|xy, 2, 2, 2, 2);
  
  fMenuBar = new MenuBar(this);
  this->AddFrame(fMenuBar, fMenuBarHints);

  fHeaderBar = new HeaderBar(this);
  this->AddFrame(fHeaderBar, fHeaderBarHints);
  
  fCaptionBox = new CaptionBox(this);
  this->AddFrame(fCaptionBox, fCaptionBoxHints);

  fHistogramInfo = new HistogramInfo(this);
  this->AddFrame(fHistogramInfo, fHistogramInfoHints);

  fLogViewer = new LogViewer(this);
  this->AddFrame(fLogViewer, fLogViewerHints);
  emph::onmon::cout.SetLogViewer(fLogViewer);
  emph::onmon::cerr.SetLogViewer(fLogViewer);
  emph::onmon::cout << "Starting online monitoring viewer" << "";
  
  fTabsBox = new TabsBox(this);
  this->AddFrame(fTabsBox, fTabsBoxHints);

  fButtonBank = new ButtonBank(this);
  this->AddFrame(fButtonBank, fButtonBankHints);

  fPlotViewer = new PlotViewer(this);
  this->AddFrame(fPlotViewer, fPlotViewerHints);

  this->MapSubwindows();
  this->Resize(Layout::fWindowSizeX+12,Layout::fWindowSizeY+24);
  this->MapWindow();

}

//......................................................................

GUIMain::~GUIMain() 
{
  if (fLogViewer) { delete fLogViewer; fLogViewer = 0; }
  if (fPlotViewer) { delete fPlotViewer; fPlotViewer = 0; }
  if (fButtonBank) { delete fButtonBank; fButtonBank = 0; }
  if (fHistogramInfo) { delete fHistogramInfo; fHistogramInfo = 0; }
  if (fTabsBox) { delete fTabsBox; fTabsBox = 0; }
  if (fHeaderBar) { delete fHeaderBar; fHeaderBar = 0; }
  if (fMenuBar) { delete fMenuBar; fMenuBar = 0; }

  if (fLogViewerHints) { delete fLogViewerHints; fLogViewerHints = 0; }
  if (fPlotViewerHints) { delete fPlotViewerHints; fPlotViewerHints = 0; }
  if (fButtonBankHints) { delete fButtonBankHints; fButtonBankHints = 0; }
  if (fHistogramInfoHints) { delete fHistogramInfoHints; fHistogramInfoHints = 0; }
  if (fTabsBoxHints) { delete fTabsBoxHints; fTabsBoxHints = 0; }
  if (fHeaderBarHints) { delete fHeaderBarHints; fHeaderBarHints = 0; }
  if (fMenuBarHints) { delete fMenuBarHints; fMenuBarHints = 0; }
}

//......................................................................

void GUIMain::CloseWindow() 
{
  gApplication->Terminate();
}

////////////////////////////////////////////////////////////////////////
