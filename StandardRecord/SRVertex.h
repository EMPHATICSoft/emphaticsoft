////////////////////////////////////////////////////////////////////////
// \file    SRVertex.h
////////////////////////////////////////////////////////////////////////
#ifndef SRVERTEX_H
#define SRVERTEX_H

#include "Math/Vector3D.h"
#include "StandardRecord/SRTrack.h"

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
    std::vector<SRTrack> trk; // vector of tracks
    int ntrk;
    double chi2;
    void Add(SRTrack& t){ trk.push_back(t); ntrk = (int)(trk.size()); }

 
    virtual void setDefault();
  };

} // end namespace

#endif // SRVERTEX_H
