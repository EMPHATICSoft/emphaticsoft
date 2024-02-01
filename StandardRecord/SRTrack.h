////////////////////////////////////////////////////////////////////////
// \file    SRTrack.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACK_H
#define SRTRACK_H

#include "StandardRecord/SRVector3D.h"

namespace caf
{
  /// An SRTrack is a simple descriptor for an Track.
  class SRTrack
  {
  public:
    SRTrack();
    virtual ~SRTrack();

    double vtx[3];  // (x,y,z)
    SRVector3D mom; // momentum vector 

    virtual void setDefault();
  };

} // end namespace

#endif // SRTRACK_H
