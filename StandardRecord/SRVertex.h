////////////////////////////////////////////////////////////////////////
// \file    SRVertex.h
////////////////////////////////////////////////////////////////////////
#ifndef SRVERTEX_H
#define SRVERTEX_H

#include "StandardRecord/SRTrack.h"

namespace caf
{
  /// An SRVertex is a simple descriptor for an Vertex.
  class SRVertex
  {
  public:
    SRVertex();
    virtual ~SRVertex();

    double pos[3];  // (x,y,z)

    bool isPrimary;
    std::vector<SRTrack> trk; // vector of tracks
    int ntrk;
    void Add(SRTrack& t){ trk.push_back(t); ntrk = (int)(trk.size()); }

 
    virtual void setDefault();
  };

} // end namespace

#endif // SRVERTEX_H
