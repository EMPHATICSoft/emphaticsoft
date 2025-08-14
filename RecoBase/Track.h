////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Track, which can be made of a vector track
///          segments, a vector of 3D spacepoints, or a vector of SSD
///          clusters
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TRACK_H
#define TRACK_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"

#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/ArichID.h"
#include "StandardRecord/SRPID.h"
#include "TVector3.h"

namespace rb {

  class Track {
  public:
    Track(); // Default constructor
    virtual ~Track() {}; //Destructor
    
  private:
    
    double _vtx[3]; // vertexrotation angle about the vertical y-axis
    double _p[3];   // momentum three vector
    std::vector<rb::SSDCluster> _clust; // vector of SSD clusters
    std::vector<rb::TrackSegment> _sgmnt; // vector of track segments
    std::vector<rb::SpacePoint> _spcpt; // vector 3D space points
    // the position of a track will be a linear interpolation between points.
    // the creator of the track is responsible for adding the points
 
    std::vector<TVector3> _pos; // vector of predicted positions
    std::vector<TVector3> _mom;

  public:

    // Getters
    const double* Vtx() const { return _vtx; }
    const double* P() const { return _p; }
    size_t NSSDClusters() const { return _clust.size(); }    
    const rb::SSDCluster* GetSSDCluster(int i) const; 
    size_t NTrackSegments() const { return _sgmnt.size(); }
    const rb::TrackSegment* GetTrackSegment(int i) const;
    size_t NSpacePoints() const { return _spcpt.size(); }
    const rb::SpacePoint* GetSpacePoint(int i) const;

    TVector3 PosAt(double z) const;
    TVector3 MomAt(double z) const;
    size_t NPos() const { return _pos.size(); }
    size_t NMom() const { return _mom.size(); }
    TVector3 Pos(size_t i) const { return _pos[i]; }
    TVector3 Mom(size_t i) const { return _mom[i]; }

    // Setters
    void Add(const rb::SSDCluster& cl); 
    void Add(const rb::TrackSegment& ts);
    void Add(const rb::SpacePoint& sp);
    void AddPos(TVector3 &x);
    void AddMom(TVector3 &p);

    void SetVtx(const double* x0) { for (int i=0; i<3; ++i) _vtx[i] = x0[i]; }
    void SetP(const double* p) { for (int i=0; i<3; ++i) _p[i] = p[i]; }
    
    friend std::ostream& operator << (std::ostream& o, const Track& h);
  };
  
}

#endif // TRACK_H
