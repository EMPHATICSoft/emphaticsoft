////////////////////////////////////////////////////////////////////////
// \file    SRBeamTrack.h
////////////////////////////////////////////////////////////////////////
#ifndef SRBEAMTRACK_H
#define SRBEAMTRACK_H

#include "Math/Vector3D.h"
#include "StandardRecord/SRTrack.h"
#include "StandardRecord/SRSimpleTruth.h"

namespace caf
{
  /// An SRBeamTrack is a simple descriptor for an BeamTrack.
  class SRBeamTrack : public SRTrack
  {
  public:
    SRBeamTrack();
    SRBeamTrack(const SRTrack& t);
    virtual ~SRBeamTrack();

    SRSimpleTruth truth;

    virtual void setDefault();
  };

} // end namespace

#endif // SRBEAMTRACK_H
