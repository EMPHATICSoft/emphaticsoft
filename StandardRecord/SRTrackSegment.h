////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegment.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACKSEGMENT_H
#define SRTRACKSEGMENT_H

#include "StandardRecord/SRVector3D.h"
#include "RecoBase/RecoBaseDefs.h"

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
    int nspacepoints;
    rb::Region region;
    double pointA[3];
    double pointB[3];
    double chi2;

    virtual void setDefault();
  };

} // end namespace

#endif // SRTRACKSEGMENT_H
