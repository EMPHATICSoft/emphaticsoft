////////////////////////////////////////////////////////////////////////
// \file    SRHeader.h
////////////////////////////////////////////////////////////////////////
#ifndef SRHEADER_H
#define SRHEADER_H

#include <stdint.h>

namespace caf
{
  /// Header representing overview information for the current event/spill
  class SRHeader
  {
  public:
    SRHeader();
    ~SRHeader();

    unsigned int   run;       ///< run number
    unsigned int   subrun;    ///< subrun number
    unsigned int   evt;       ///< ART event number
    uint32_t timestamp;
    float  intensity;
    float  momentum;
    float  MT5CPR, MT6CPR;// gas Ckov pressure readings
    float  MT5CP2, MT6CP2;// iFix readings
    float  TBCCDI;         // TestBeam ckov counts inner
    float  TBCCDO;         // TestBeam ckov counts outer
    float  MT6CA1;         // MT6 ckov east mirror counts
    float  MT6CA2;         // MT6 ckov west mirror counts
    float  MT6CA3;         // MT6 ckov top mirror counts

    void setDefault();
  };

} // end namespace

#endif // SRHEADER_H
