////////////////////////////////////////////////////////////////////////
// \file    SRVertex.h
////////////////////////////////////////////////////////////////////////
#ifndef SRVERTEX_H
#define SRVERTEX_H

#include "Math/Vector3D.h"
#include "StandardRecord/SRBeamTrack.h"
#include "StandardRecord/SRSecondaryTrack.h"

namespace caf
{
  /// An SRVertex is a simple descriptor for an Vertex.
  class SRVertex
  {
  public:
    SRVertex();
    virtual ~SRVertex();

    ROOT::Math::XYZVector pos;  // (x,y,z)

    bool isPrimary;
    SRBeamTrack beamtrk; // vector of secondary tracks
    std::vector<SRSecondaryTrack> sectrk; // vector of secondary tracks
    int nsectrk;
    double chi2;
    void Add(SRSecondaryTrack& t){ sectrk.push_back(t); nsectrk = (int)(sectrk.size()); }
    void SetBeamTrack(SRBeamTrack& t){ beamtrk = t; }
 
    virtual void setDefault();
  };

} // end namespace

#endif // SRVERTEX_H
