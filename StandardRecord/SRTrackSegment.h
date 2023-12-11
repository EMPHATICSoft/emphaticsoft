////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegment.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACKSEGMENT_H
#define SRTRACKSEGMENT_H

#include "StandardRecord/SRVector3D.h"

namespace caf
{
  /// An SRTrackSegment is a simple descriptor for an TrackSegment.
  class SRTrackSegment
  {
  public:
    SRTrackSegment();
    virtual ~SRTrackSegment();

    double vtx[3];  // (x,y,z)
    SRVector3D mom; // momentum vector 

    virtual void setDefault();
  };

} // end namespace

#endif // SRTRACKSEGMENT_H
