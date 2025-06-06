////////////////////////////////////////////////////////////////////////
// \file    SRTrack.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACK_H
#define SRTRACK_H

#include "StandardRecord/SRVector3D.h"
#include "StandardRecord/SRArichID.h"
#include "StandardRecord/SRTrackSegment.h"

namespace caf
{
  /// An SRTrack is a simple descriptor for an Track.
  class SRTrack
  {
  public:
    SRTrack();
    virtual ~SRTrack();

    double vtx[3];  // (x,y,z)
    SRVector3D mom; // momentum vector   
    SRArichID arich; // arich loglikelihooods values 
//  SRArichID arML; // arich ML pred values   
 
    int label;
    int nspacepoints;
    std::vector<SRTrackSegment> _sgmnt; // vector of track segments
    void Add(SRTrackSegment& ts){ _sgmnt.push_back(ts); };

 
    virtual void setDefault();
  };

} // end namespace

#endif // SRTRACK_H
