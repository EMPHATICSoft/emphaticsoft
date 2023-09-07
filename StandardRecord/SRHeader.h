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
    float  gcp5, gcp5iFix;
    float  gcp6, gcp6iFix;
    float  tbccdi, tbccdo;
    float  mt6ca1, mt6ca2, mt6ca3;

    void setDefault();
  };

} // end namespace

#endif // SRHEADER_H
