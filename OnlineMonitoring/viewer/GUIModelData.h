#ifndef OM_GUIMODELDATA_H
#define OM_GUIMODELDATA_H
#include "OnlineMonitoring/util/HistoData.h"
#include "OnlineMonitoring/viewer/CurrentHistogram.h"
#include "OnlineMonitoring/viewer/ComparisonOptions.h"
#include <string>

namespace om
{

  ///
  /// Define codes so subscribers can tell which pieces of information
  /// they want to receive updates about
  ///
  static const unsigned int kCurrentHistogramID      = 1<<0;
  static const unsigned int kHistogramInfoTextID     = 1<<1;
  static const unsigned int kPlotViewRefreshID       = 1<<2;
  static const unsigned int kPlotViewRefreshPausedID = 1<<3;
  static const unsigned int kPlotViewRefreshRateID   = 1<<4;
  static const unsigned int kHistogramCSVFileID      = 1<<5;
  static const unsigned int kComparisonOptionsID     = 1<<6;
  static const unsigned int kHistogramSourceID       = 1<<7;
  static const unsigned int kTickID                  = 1<<8;
  static const unsigned int kComparisonUpdateID      = 1<<9;
  static const unsigned int kDetectorID              = 1<<10;
  static const unsigned int kPrintID                 = 1<<11;
  static const unsigned int kRefFileInitID           = 1<<12;
  ///
  /// All the data that controls what user sees on the gui
  ///
  class GUIModelData {
  public:
    Detector_t        fDetector;
    std::string       fHistogramSource;
    std::string       fHistogramInfoText;
    CurrentHistogram  fCurrentHistogram;
    unsigned int      fPlotViewRefreshRate;
    bool              fPlotViewRefreshPaused;
    std::string       fHistoCSVFile;
    ComparisonOptions fComparisonOpt;
    unsigned int      fContentProjection;
  public:
    GUIModelData();
    void SetCSVFile(std::string v);
  };
}

#endif
////////////////////////////////////////////////////////////////////////
