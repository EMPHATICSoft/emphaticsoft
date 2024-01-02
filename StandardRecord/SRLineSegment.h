////////////////////////////////////////////////////////////////////////
// \file    SRLineSegment.h
////////////////////////////////////////////////////////////////////////
#ifndef SRLINESEGMENT_H
#define SRLINESEGMENT_H

namespace caf
{
  /// An SRLineSegment is a simple descriptor for a LineSegment.
  class SRLineSegment
  {
  public:
    SRLineSegment();
    virtual ~SRLineSegment();

    double x0[3]; // end of line closesr to origin
    double x1[3]; // end of line farther from origin
    virtual void setDefault();
  };

} // end namespace

#endif // SRLINESEGMENT_H
