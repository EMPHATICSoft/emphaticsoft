////////////////////////////////////////////////////////////////////////
// \file    SRSecondaryTrack.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSECONDARYTRACK_H
#define SRSECONDARYTRACK_H  

#include "Math/Vector3D.h"
#include "StandardRecord/SRTrack.h"
#include "StandardRecord/SRSimpleTruth.h"

namespace caf
{
  /// An SRBeamTrack is a simple descriptor for an BeamTrack.
  class SRSecondaryTrack : public SRTrack
  {
  public:
    SRSecondaryTrack();
    SRSecondaryTrack(const SRTrack& t);
    virtual ~SRSecondaryTrack();
    std::vector<SRSimpleTruth> truth;

    virtual void setDefault();
  };

} // end namespace

#endif // SRSECONDARYTRACK_H