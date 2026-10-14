////////////////////////////////////////////////////////////////////////
// \file    SRTrackSegment.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACKSEGMENT_H
#define SRTRACKSEGMENT_H

#include "Math/Vector3D.h"
#include "StandardRecord/SRBaseDefs.h"

namespace caf
{
  /// An SRTrackSegment is a simple descriptor for an TrackSegment.
  class SRTrackSegment
  {
  public:
    SRTrackSegment();
    virtual ~SRTrackSegment();

    ROOT::Math::XYZVector vtx;  // (x,y,z)
    ROOT::Math::XYZVector mom; // momentum vector 
    int nspacepoints;
    caf::Region region;
    ROOT::Math::XYZVector pointA;
    ROOT::Math::XYZVector pointB;
    double chi2;
    double thetaX;
    double thetaY;

    virtual void setDefault();
  };

} // end namespace

#endif // SRTRACKSEGMENT_H
