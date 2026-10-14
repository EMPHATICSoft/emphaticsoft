////////////////////////////////////////////////////////////////////////
/// \brief   Definition of Track Segment
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TRACKSEGMENT_H
#define TRACKSEGMENT_H

#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"

#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"
#include "StandardRecord/SRTrackSegment.h"

namespace rb {
  
  //  class TrackSegment {
  class TrackSegment : public caf::SRTrackSegment{
  public:
    TrackSegment(); // Default constructor
    virtual ~TrackSegment() {}; //Destructor
    
  private:
    
    std::vector<rb::SSDCluster> _clust; // vector of SSD clusters
    std::vector<rb::SpacePoint> _spcpt; // vector 3D space points

  public:
    // Getters
    size_t NSSDClusters() const { return _clust.size(); }    
    const rb::SSDCluster* GetSSDCluster(int i) const; 
    size_t NSpacePoints() const { return _spcpt.size(); }
    const rb::SpacePoint* GetSpacePoint(int i) const;

    // Setters
    void Add(const rb::SSDCluster& cl); 
    void Add(const rb::SpacePoint& sp);
  
    friend std::ostream& operator << (std::ostream& o, const TrackSegment& h);
  };
  
}

#endif // TRACKSEGMENT_H
