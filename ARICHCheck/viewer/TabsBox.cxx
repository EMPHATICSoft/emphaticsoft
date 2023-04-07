#include "OnlineMonitoring/viewer/TabsBox.h"
#include "OnlineMonitoring/viewer/Layout.h"
#include "OnlineMonitoring/viewer/HistogramBrowser.h"
#include "OnlineMonitoring/viewer/ComparisonBox.h"
using namespace emph::onmon;

TabsBox::TabsBox(TGWindow* w) :
  TGTab(w, Layout::fBrowserTabSizeX, Layout::fBrowserTabSizeY)
{
  TGCompositeFrame* hb = this->AddTab("Histograms");
  TGCompositeFrame* ct = this->AddTab("Comparison");

  fHistoBrowser = new HistogramBrowser(hb,
				       Layout::fBrowserTabSizeX-8,
				       Layout::fBrowserTabSizeY-48,
				       kVerticalFrame);
  fHistoBrowserHints  = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY);
  hb->AddFrame(fHistoBrowser, fHistoBrowserHints);

  fComparisonBox = new ComparisonBox(ct,
				     Layout::fBrowserTabSizeX-8,
				     Layout::fBrowserTabSizeY-48,
				     kVerticalFrame);
  fComparisonBoxHints = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY);
  ct->AddFrame(fComparisonBox, fComparisonBoxHints);

  // this->AddTab("Summaries");
}

//......................................................................

TabsBox::~TabsBox()
{

  if(fHistoBrowser) {
    delete fHistoBrowser;
    fHistoBrowser = 0;
  }
  if(fHistoBrowserHints) {
    delete fHistoBrowserHints;
    fHistoBrowserHints = 0;
  }

  if(fComparisonBox) {
    delete fComparisonBox;
    fComparisonBox = 0;
  }
  if(fComparisonBoxHints) {
    delete fComparisonBoxHints;
    fComparisonBoxHints = 0;
  }

}

////////////////////////////////////////////////////////////////////////
