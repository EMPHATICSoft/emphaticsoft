extern "C" {
#include <unistd.h>
}
#include <iostream>
#include "TROOT.h"
#include "TApplication.h"
#include "TStyle.h"
#include "OnlineMonitoring/viewer/GUIModel.h"
#include "OnlineMonitoring/viewer/GUIModelData.h"
#include "OnlineMonitoring/viewer/GUIMain.h"

static void usage()
{
  std::cout << "\nUsage: onmon_viewer"
	    << " -f [input: file.root or NAME.shm]"
	    << " -d [detector]"
	    << " -v [csv histo formats file]"
	    << " -r [reference file]\n"
	    << std::endl;
  exit(0);
}

//......................................................................

int main(int argc, char** argv)
{
  int c;
  std::string det      = "EMPH";
  std::string histosrc = "EMPH.shm";
  std::string histocsv = "onmon-histos.csv";
  std::string reffile  = "NONE";

  while ((c = getopt(argc, argv, "f:d:v:r:h"))!=-1) {
    switch (c) {
    case 'f':
      histosrc = optarg;
      break;
    case 'd':
      det = optarg;
      break;
    case 'v':
      histocsv = optarg;
      break;
    case 'r':
      reffile = optarg;
      break;
    case 'h':
      usage();
      break;
    default:
      usage();
      break;
    }
  }

  TApplication app("onmon_viewer", &argc, argv);
  gROOT->SetStyle("Plain");
  // uncomment if you want to set a custom palette
  //gStyle->SetPalette(1);
  gStyle->SetOptStat(0);

  emph::onmon::GUIMain m;

  emph::onmon::GUIModel::Instance().SetDetector(det.c_str());
  emph::onmon::GUIModel::Instance().SetCSVFile(histocsv);
  emph::onmon::GUIModel::Instance().SetHistogramSource(histosrc.c_str());

  if(reffile != "NONE") {
    emph::onmon::GUIModel::Instance().SetReferenceFile(reffile.c_str());
    emph::onmon::GUIModel::Instance().Publish(emph::onmon::kRefFileInitID);
  }
  emph::onmon::GUIModel::Instance().Init();

  bool is_shm  = histosrc.find(".shm")< histosrc.length();
  if(is_shm) {
    std::string Wname = "OnMon Viewer ";
    Wname += histosrc;
    m.SetWindowName(Wname.c_str());
  }
  else
    m.SetWindowName("OnMon Viewer");

  app.Run();

  return 0;
}

////////////////////////////////////////////////////////////////////////
