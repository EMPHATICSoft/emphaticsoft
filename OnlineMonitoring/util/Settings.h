///
/// \file    Settings.h
/// \brief   A simple object that conatins all the global settings
/// \version $Id: Settings.h,v 1.2 2012-05-30 18:43:38 mbaird42 Exp $
/// \author  mbaird42@fnal.gov
///
#ifndef OM_SETTINGS_H
#define OM_SETTINGS_H
#include "OnlineMonitoring/util/HistoData.h"

namespace om {
  class Settings {
  public:
    static Settings& Instance();
  
  public:
    std::string  fCSVFile;
    Detector_t   fDet;
    
  private:
    Settings();

  };
}

#endif
////////////////////////////////////////////////////////////////////////
