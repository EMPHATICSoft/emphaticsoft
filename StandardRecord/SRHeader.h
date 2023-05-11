////////////////////////////////////////////////////////////////////////
// \file    SRHeader.h
////////////////////////////////////////////////////////////////////////
#ifndef SRHEADER_H
#define SRHEADER_H

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

    void setDefault();
  };

} // end namespace

#endif // SRHEADER_H
