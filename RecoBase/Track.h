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

#include "RecoBase/SSDCluster.h"
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
    
    //    double _vtx[3]; // vertexrotation angle about the vertical y-axis
    //    double _p[3];   // momentum three vector
    std::vector<rb::SSDCluster> _clust; // vector of SSD clusters
    std::vector<rb::TrackSegment> _sgmnt; // vector of track segments
    std::vector<rb::SpacePoint> _spcpt; // vector 3D space points
    // the position of a track will be a linear interpolation between points.
    // the creator of the track is responsible for adding the points
 
    std::vector<ROOT::Math::XYZVector> _pos; // vector of predicted positions
    std::vector<ROOT::Math::XYZVector> _mom;
    //    double _chi2;

  public:

    // Getters
    //    const double* Vtx() const { return _vtx; }
    //    const double* P() const { return _p; }
    size_t NSSDClusters() const { return _clust.size(); }    
    const rb::SSDCluster* GetSSDCluster(int i) const; 
    size_t NTrackSegments() const { return _sgmnt.size(); }
    const rb::TrackSegment* GetTrackSegment(int i) const;
    size_t NSpacePoints() const { return _spcpt.size(); }
    const rb::SpacePoint* GetSpacePoint(int i) const;

    //    const double Chi2() const { return _chi2; }
    ROOT::Math::XYZVector PosAt(double z) const;
    ROOT::Math::XYZVector MomAt(double z) const;
    size_t NPos() const { return _pos.size(); }
    size_t NMom() const { return _mom.size(); }
    ROOT::Math::XYZVector Pos(size_t i) const { return _pos[i]; }
    ROOT::Math::XYZVector Mom(size_t i) const { return _mom[i]; }
    //    ROOT::Math::XYZVector PosAtTrgt() const { return _posTrgt; }
    //    ROOT::Math::XYZVector MomAtTrgt() const { return _momTrgt; }

    // Setters
    void Add(const rb::SSDCluster& cl); 
    void Add(const rb::TrackSegment& ts);
    void Add(const rb::SpacePoint& sp);
    void AddPos(ROOT::Math::XYZVector &x);
    void AddMom(ROOT::Math::XYZVector &p);

    //    void SetPosAtTrgt(const double* x0) { for (int i=0; i<3; ++i) _posTrgt[i] = x0[i]; }
    //    void SetMomAtTrgt(const double* p0) { for (int i=0; i<3; ++i) _momTrgt[i] = p0[i]; }

    //    void SetVtx(const double* x0) { for (int i=0; i<3; ++i) _vtx[i] = x0[i]; }
    //    void SetP(const double* p) { for (int i=0; i<3; ++i) _p[i] = p[i]; }
    //    void SetChi2(double chi2) { _chi2 = chi2; }
    
    friend std::ostream& operator << (std::ostream& o, const Track& h);
  };
  
}

#endif // TRACK_H
