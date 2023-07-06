////////////////////////////////////////////////////////////////////////
// \file    SRCaloHit.h
////////////////////////////////////////////////////////////////////////
#ifndef SRCALOHIT_H
#define SRCALOHIT_H

#include "StandardRecord/SRVector3D.h"

namespace caf
{
  /// An SRCaloHit is a simple descriptor for an CaloHit.
  class SRCaloHit
  {
  public:
    SRCaloHit();
    virtual ~SRCaloHit();

    int _channel; //block number (0-8, 4 is the center)
    int _time; //where the peak of the signal waveform occurs
    float _intchg; //integrated charge

    virtual void setDefault();
  };

} // end namespace

#endif // SRCALOHIT_H
