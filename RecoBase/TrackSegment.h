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
#include "RecoBase/RecoBaseDefs.h"
#include "StandardRecord/SRTrackSegment.h"

namespace rb {
  
  //  class TrackSegment {
  class TrackSegment : public caf::SRTrackSegment{
  public:
    TrackSegment(); // Default constructor
    virtual ~TrackSegment() {}; //Destructor
    
  private:
    
    //    double _vtx[3]; // vertexrotation angle about the vertical y-axis
    //    double _p[3];   // momentum three vector
    //    double _pointA[3];
    //    double _pointB[3];
    std::vector<rb::SSDCluster> _clust; // vector of SSD clusters
    std::vector<rb::SpacePoint> _spcpt; // vector 3D space points
    //    double _chi2;
    //    rb::Region _region;

  public:
    // Getters
    //    const double* Vtx() const { return _vtx; }
    //    const double* P() const { return _p; }
    size_t NSSDClusters() const { return _clust.size(); }    
    const rb::SSDCluster* GetSSDCluster(int i) const; 
    size_t NSpacePoints() const { return _spcpt.size(); }
    const rb::SpacePoint* GetSpacePoint(int i) const;
    //    const double* A() const { return _pointA; }
    //    const double* B() const { return _pointB; }
    //    const rb::Region RegLabel() const { return _region; }
    //    const double Chi2() const { return _chi2; }

    // Setters
    void Add(const rb::SSDCluster& cl); 
    void Add(const rb::SpacePoint& sp);

    //    void SetVtx(double* x0) { for (int i=0; i<3; ++i) _vtx[i] = x0[i]; }
    //    void SetP(double* p) { for (int i=0; i<3; ++i) _p[i] = p[i]; }

    //    void SetA(double* pA) { for (int i=0; i<3; ++i) _pointA[i] = pA[i]; }
    //    void SetB(double* pB) { for (int i=0; i<3; ++i) _pointB[i] = pB[i]; }

    //    void SetRegLabel(rb::Region region) { _region = region; }
    //    void SetChi2(double chi2) { _chi2 = chi2; } 
  
    friend std::ostream& operator << (std::ostream& o, const TrackSegment& h);
  };
  
}

#endif // TRACKSEGMENT_H
