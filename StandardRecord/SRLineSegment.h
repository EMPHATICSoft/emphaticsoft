////////////////////////////////////////////////////////////////////////
// \file    SRLineSegment.h
////////////////////////////////////////////////////////////////////////
#ifndef SRLINESEGMENT_H
#define SRLINESEGMENT_H

#include "StandardRecord/SRVector3D.h"

namespace caf
{
  /// An SRLineSegment is a simple descriptor for a LineSegment.
  class SRLineSegment
  {
  public:
    SRLineSegment();
    virtual ~SRLineSegment();

    SRVector3D x0; // end of line closesr to origin
    SRVector3D x1; // end of line farther from origin
    virtual void setDefault();
  };

} // end namespace

#endif // SRLINESEGMENT_H
