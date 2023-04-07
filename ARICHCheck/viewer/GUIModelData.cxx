#include "OnlineMonitoring/viewer/GUIModelData.h"
#include <cstdlib>
extern "C" {
#include <unistd.h>
}
using namespace emph::onmon;

GUIModelData::GUIModelData() :
  fHistogramInfoText("Click on plot to see histogram data"),
  fPlotViewRefreshRate(60000),
  fPlotViewRefreshPaused(false),
  fContentProjection(0)
{ }

//.....................................................................

void GUIModelData::SetCSVFile(std::string file)
{
  //
  // Try to find the location of the onmon-histos.csv file for the
  // descriptions of the histograms
  //
  std::string csvpath;
  for (int itry=0; itry<3; ++itry) {
    switch (itry) {
    case 0: csvpath = "./";      break;
    case 1: csvpath = "./util/"; break;
    case 2:
      csvpath = getenv("CETPKG_SOURCE");
      csvpath += "/OnlineMonitoring/util/";
      break;
    }
    csvpath += file;
    if (access(csvpath.c_str(), F_OK)!=-1) {
      fHistoCSVFile = csvpath;
      break;
    }
  } // loop on directory attempts
}
////////////////////////////////////////////////////////////////////////
