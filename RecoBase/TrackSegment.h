////////////////////////////////////////////////////////////////////////
/// \brief   Definition of Track Segment
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TRACKSEGMENT_H
#define TRACKSEGMENT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"

#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"

namespace rb {
  
  class TrackSegment {
  public:
    TrackSegment(); // Default constructor
    virtual ~TrackSegment() {}; //Destructor
    
  private:
    
    double _vtx[3]; // vertexrotation angle about the vertical y-axis
    double _p[3];   // momentum three vector
    std::vector<rb::SSDCluster> _clust; // vector of SSD clusters
    std::vector<rb::SpacePoint> _spcpt; // vector 3D space points

  public:
    // Getters
    const double* Vtx() const { return _vtx; }
    const double* P() const { return _p; }
    size_t NSSDClusters() const { return _clust.size(); }    
    const rb::SSDCluster* GetSSDCluster(int i) const; 
    size_t NSpacePoints() const { return _spcpt.size(); }
    const rb::SpacePoint* GetSpacePoint(int i) const;

    // Setters
    void Add(const rb::SSDCluster& cl); 
    void Add(const rb::SpacePoint& sp);

    void SetVtx(double* x0) { for (int i=0; i<3; ++i) _vtx[i] = x0[i]; }
    void SetP(double* p) { for (int i=0; i<3; ++i) _p[i] = p[i]; }
    
    friend std::ostream& operator << (std::ostream& o, const TrackSegment& h);
  };
  
}

#endif // TRACKSEGMENT_H
