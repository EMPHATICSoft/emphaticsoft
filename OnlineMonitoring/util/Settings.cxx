///
/// \file    Settings.cxx
/// \brief   A simple object that conatins all the global settings
/// \version $Id: Settings.cxx,v 1.2 2012-05-30 18:43:38 mbaird42 Exp $
/// \author  mbaird42@fnal.gov
///

#include "OnlineMonitoring/util/Settings.h"

using namespace emph::onmon;

Settings& Settings::Instance() 
{
  static Settings snoopdog;
  return snoopdog;

  // Note to anyone who cares:  This naming convention began as a joke between
  // Mark and I.  For the record, he started it (see RawEventUnpacker.cxx).

}

//......................................................................

Settings::Settings()
{ }

//......................................................................

////////////////////////////////////////////////////////////////////////
