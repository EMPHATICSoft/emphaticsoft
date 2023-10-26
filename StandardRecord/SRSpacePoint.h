////////////////////////////////////////////////////////////////////////
// \file    SRSpacePoint.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSPACEPOINT_H
#define SRSPACEPOINT_H

namespace caf
{
  /// An SRSpacePoint is a simple descriptor for an SpacePoint.
  class SRSpacePoint
  {
  public:
    SRSpacePoint();
    virtual ~SRSpacePoint();

    double x[3];  // (x,y,z)
    int    station; // which SSD station did this come from?

    virtual void setDefault();
  };

} // end namespace

#endif // SRSPACEPOINT_H
