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

#include "Math/Vector3D.h"

#include "canvas/Persistency/Common/PtrVector.h"

#include "RecoBase/LineSegment.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoBase/ArichID.h"
#include "StandardRecord/SRPID.h"
#include "StandardRecord/SRTrack.h"

namespace rb {

  class Track : public caf::SRTrack {
  public:
    Track(); // Default constructor
    virtual ~Track() {}; //Destructor
    
  private:
    
    std::vector<rb::LineSegment> lineseg; // vector of SSD linesegments (from SSD clusters)
    std::vector<rb::TrackSegment> sgmnt; // vector of track segments
    std::vector<rb::SpacePoint> spcpt; // vector 3D space points
    // the position of a track will be a linear interpolation between points.
    // the creator of the track is responsible for adding the points
 
    std::vector<ROOT::Math::XYZVector> posAt; // vector of predicted positions
    std::vector<ROOT::Math::XYZVector> momAt; // vector of predicted momenta

    public:

    // Getters
    size_t NSSDLineSegments() const { return lineseg.size(); }    
    const rb::LineSegment* GetSSDLineSegment(int i) const; 
    size_t NTrackSegments() const { return sgmnt.size(); }
    const rb::TrackSegment* GetTrackSegment(int i) const;
    size_t NSpacePoints() const { return spcpt.size(); }
    const rb::SpacePoint* GetSpacePoint(int i) const;

    ROOT::Math::XYZVector PosAt(double z) const;
    ROOT::Math::XYZVector MomAt(double z) const;
    size_t NPos() const { return posAt.size(); }
    size_t NMom() const { return momAt.size(); }
    ROOT::Math::XYZVector PosAt(size_t i) const { return posAt[i]; }
    ROOT::Math::XYZVector MomAt(size_t i) const { return momAt[i]; }

    // Setters
    void Add(const rb::LineSegment& ls); 
    void Add(const rb::TrackSegment& ts);
    void Add(const rb::SpacePoint& sp);
    void AddPos(ROOT::Math::XYZVector &x);
    void AddMom(ROOT::Math::XYZVector &p);

    friend std::ostream& operator << (std::ostream& o, const Track& h);
  };
  
}

#endif // TRACK_H
