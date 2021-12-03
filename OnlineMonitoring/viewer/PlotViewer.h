///
/// \file    PlotViewer.h
/// \brief   Canvas on which plots and reports are drawn
/// \version $Id: PlotViewer.h,v 1.7 2012-11-10 00:00:43 mbaird42 Exp $
/// \author  messier@indiana.edu
///
#ifndef OM_PLOTVIEWER_H
#define OM_PLOTVIEWER_H
#include "TRootEmbeddedCanvas.h"
#include "TTimer.h"
#include "OnlineMonitoring/viewer/PlotOptions.h"
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"
class TVirtualPad;
class TH1F;
class TH2F;
class TGWindow;
namespace om { class ComparisonOptions; }

namespace om
{
  //
  // The canvas that shows the selected plot of detector status
  //
  class PlotViewer : public TRootEmbeddedCanvas, public GUIModelSubscriber
  {
  public:
    PlotViewer(TGWindow* m);
    ~PlotViewer();

    static void Exec();

    void Update();
    void UpdateCompare(const ComparisonOptions& COpt);

    // Complete the GUIModelSubscriber interface
    void GUIModelDataIssue(const GUIModelData& m, unsigned int which);

  private:
    void ContentProjectionTH1F(const HistoData* hd, TH1F* h);
    void ContentProjectionTH2F(const HistoData* hd, TH2F* h);

  private:
    PlotOptions fPlotOpt;          ///< Options for plotting
    std::string fHistogramSource;  ///< Source to pull histograms from
    std::string fCurrentHistogram; ///< What histogram is being shown?

  ClassDef(PlotViewer,0)
  };
}

#endif
////////////////////////////////////////////////////////////////////////
