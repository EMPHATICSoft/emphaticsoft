////////////////////////////////////////////////////////////////////////
// \file    SRLineSegment.h
////////////////////////////////////////////////////////////////////////
#ifndef SRLINESEGMENT_H
#define SRLINESEGMENT_H

#include "Math/Vector3D.h"
//StandardRecord/SRVector3D.h"

namespace caf
{
  /// An SRLineSegment is a simple descriptor for a LineSegment.
  class SRLineSegment
  {
  public:
    SRLineSegment();
    virtual ~SRLineSegment();

    ROOT::Math::XYZVector x0; ///< end of line closesr to origin
    ROOT::Math::XYZVector x1; ///< end of line farther from origin
    double sigma;
    virtual void setDefault();
  };

} // end namespace

#endif // SRLINESEGMENT_H
