////////////////////////////////////////////////////////////////////////
// \file    SRGasCkov.h
////////////////////////////////////////////////////////////////////////
#ifndef SRGASCKOV_H
#define SRGASCKOV_H

namespace caf
{
  /// An SRGasCkov is a simple descriptor for the Gas Cherenkov 
  class SRGasCkov
  {
  public:
    SRGasCkov();
    virtual ~SRGasCkov();

    float charge;    ///< charge recorded for Gas Cherenkov PMT
    float time;     ///< time of hit recorded for Gas Cherenkov PMT

    virtual void setDefault();
  };

} // end namespace

#endif // SRGASCKOV_H
