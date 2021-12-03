#include "OnlineMonitoring/viewer/GUIModel.h"
#include <iostream>
#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"
#include "OnlineMonitoring/viewer/GUIModelData.h"
using namespace om;

static bool         gsLocked = false;

//......................................................................

GUIModel& GUIModel::Instance()
{
  static GUIModel nico;
  return nico;
}

//......................................................................

void GUIModel::Init()
{
  unsigned int whichInit = 0xFFFF;

  // subtract off things that should NOT be initialized
  whichInit -= kCurrentHistogramID;
  whichInit -= kPlotViewRefreshID;
  whichInit -= kComparisonUpdateID;
  whichInit -= kPrintID;
  whichInit -= kRefFileInitID;

  this->Publish(whichInit);
}

//......................................................................

void GUIModel::Subscribe(GUIModelSubscriber* s, unsigned int which)
{
  fSubscriber.push_back(s);
  fWhich.     push_back(which);
}

//......................................................................

void GUIModel::Unsubscribe(GUIModelSubscriber* s)
{
  unsigned int i;
  for (i=0; i<fSubscriber.size(); ++i) {
    if (fSubscriber[i]==s) fSubscriber[i]=0;
  }
}

//......................................................................

GUIModel::GUIModel() { }

//......................................................................

void GUIModel::SetDetector(const char* n)
{
  std::string nm(n);
  if (nm=="EPMHATIC" || nm=="emphatic" || nm=="EMPH" || nm=="emph")
    fData.fDetector = kEMPH;
  else if (nm=="SSD" || nm=="ssd")
    fData.fDetector = kSSD;
  else {
    std::cout << "\nERROR:   " << nm << " is not a valid detector type.\n\n";
    abort();
  }

  this->Publish(kDetectorID);
}

//......................................................................

void GUIModel::SetCSVFile(std::string v)
{
  fData.SetCSVFile(v);
}

//......................................................................

void GUIModel::SetHistogramSource(const char* s)
{
  //
  // If the histogram source happens to be a .root file, automatically
  // pause the automatic updates. If its .shm automatically unpause
  //
  std::string ss(s);
  std::string ext(ss.substr(ss.find_last_of(".")+1));
  if (ext=="root") { this->SetPlotViewRefreshPaused(true);  }
  if (ext=="shm")  { this->SetPlotViewRefreshPaused(false); }

  fData.fHistogramSource = s;
  this->Publish(kHistogramSourceID);
}

//......................................................................

void GUIModel::SetCurrentHistogram(const char* h)
{
  fData.fCurrentHistogram.Set(h);
  if(fData.fComparisonOpt.fWhich == ComparisonOptions::kNone)
    this->Publish(kCurrentHistogramID);
  else
    this->Publish(kComparisonUpdateID);
}

//......................................................................

void GUIModel::HistogramBack()
{
  fData.fCurrentHistogram.Backward();
  this->Publish(kCurrentHistogramID);
}

//......................................................................

void GUIModel::HistogramForward()
{
  fData.fCurrentHistogram.Forward();
  this->Publish(kCurrentHistogramID);
}

//......................................................................

void GUIModel::SetHistogramInfoText(const char* t)
{
  fData.fHistogramInfoText = t;
  this->Publish(kHistogramInfoTextID);
}

//......................................................................

void GUIModel::SetPlotViewRefreshPaused(bool paused)
{
  if (fData.fPlotViewRefreshPaused!=paused) {
    fData.fPlotViewRefreshPaused = paused;
    this->Publish(kPlotViewRefreshPausedID);
  }
}

//......................................................................

void GUIModel::SetCompareWhich(unsigned int i)
{
  fData.fComparisonOpt.fWhich = i;
  this->Publish(kComparisonOptionsID);
}

//......................................................................

void GUIModel::SetReferenceFile(const char* RefFile)
{
  fData.fComparisonOpt.fReferenceFile = RefFile;
  this->Publish(kComparisonOptionsID);
}

//......................................................................

void GUIModel::SetLookBack(int lb)
{
  // The comparison box returns a number 0 through (n-1) but the look back
  // number is defined from 1 to n, so we will add 1.
  fData.fComparisonOpt.fLookBack = lb + 1;
  this->Publish(kComparisonOptionsID);
}

//......................................................................

void GUIModel::SetCompareMethod(unsigned int i)
{
  fData.fComparisonOpt.fMethod = i;
  this->Publish(kComparisonOptionsID);
}

//......................................................................

void GUIModel::SetCompareNorm(unsigned int i)
{
  fData.fComparisonOpt.fNormalize = i;
  this->Publish(kComparisonOptionsID);
}

//......................................................................

void GUIModel::SendPrint()
{
  this->Publish(kPrintID);
}

//......................................................................

void GUIModel::DrawContentProjection()
{
  fData.fContentProjection = 1;

  if(fData.fComparisonOpt.fWhich == ComparisonOptions::kNone)
    this->Publish(kPlotViewRefreshID);
  else
    this->Publish(kComparisonUpdateID);

  fData.fContentProjection = 0;
}

//......................................................................

void GUIModel::SendUpdates() const
{
  if(fData.fComparisonOpt.fWhich == ComparisonOptions::kNone)
    this->Publish(kPlotViewRefreshID);
  else
    this->Publish(kComparisonUpdateID);
}

//......................................................................

void GUIModel::Publish(unsigned int which) const
{
  if (gsLocked==true) return;

  gsLocked = true;

  unsigned int i;
  for (i=0; i<fSubscriber.size(); ++i) {
    if ((which&fWhich[i])!=0 && fSubscriber[i]!=0) {
      fSubscriber[i]->GUIModelDataIssue(fData, which);
    }
  }

  gsLocked = false;
}

////////////////////////////////////////////////////////////////////////
