////////////////////////////////////////////////////////////////////////
/// \brief   Definition of 3D space point class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SPACEPOINT_H
#define SPACEPOINT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"

namespace rb {
  
  class SpacePoint {
  public:
    SpacePoint(); // Default constructor
    //    SpacePoint(std::vector<rb::SSDHit> hits); // Default constructor
    virtual ~SpacePoint() {}; //Destructor
    
  private:

    double _x[3]; // rotation angle about the vertical y-axis
    int    fStation; //SSD station of the Space Point
    std::vector<rb::LineSegment> _lineseg; // vector of LineSegments
    std::vector<rb::SSDCluster> _clust; // vector of SSDClusters
    std::vector<double> _lineDistance; // 2D distance to each contributing line segment

  public:
    // Getters
    const double* Pos() const { return _x; }
    int           Station() const { return fStation; }
    const rb::LineSegment* GetLineSegment(int i) const;
    size_t NLineSegments() const { return _lineseg.size(); }
    double LineSegmentDistance(int i) const;
    size_t NLineSegmentDistances() const { return _lineDistance.size(); }
    const rb::SSDCluster* GetSSDCluster(int i) const;
    size_t NSSDClusters() const { return _clust.size(); }

    // Setters
    void SetX(double* x0) { for (int i=0; i<3; ++i) _x[i] = x0[i]; }
    void SetStation(int station) {fStation = station;}   
    void Add(const rb::LineSegment& ls);
    void AddLineSegmentDistance(double distance) { _lineDistance.push_back(distance); }
    void Add(const rb::SSDCluster& cl);
 
    friend std::ostream& operator << (std::ostream& o, const SpacePoint& h);
  };
  
}

#endif // SPACEPOINT_H
