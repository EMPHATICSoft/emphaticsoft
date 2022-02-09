#include "OnlineMonitoring/util/HistoData.h"
using namespace emph::onmon;

bool HistoData::IsValid() const
{
  bool detok = false;
  if (fDetector == kEMPH)     detok = true;
  if (fDetector == kALLDET)   detok = true;
  if (!detok) return false;

  bool typeok = false;
  if (fType == kTH1F) typeok = true;
  if (fType == kTH2F) typeok = true;
  if (!typeok) return false;

  if (fNx<1)   return false;
  if (fX2<fX1) return false;

  if (fType==kTH2F) {
    if (fNy<1)  return false;
    if (fY2<fY1) return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////
