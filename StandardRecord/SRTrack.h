////////////////////////////////////////////////////////////////////////
// \file    SRTrack.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTRACK_H
#define SRTRACK_H

#include "Math/Vector3D.h"
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

    ROOT::Math::XYZVector vtx;  // (x,y,z)
    ROOT::Math::XYZVector mom; // momentum vector
    ROOT::Math::XYZVector posTrgt;
    ROOT::Math::XYZVector momTrgt;
    ROOT::Math::XYZVector posARICH;
    ROOT::Math::XYZVector momARICH;
    ROOT::Math::XYZVector posMagnet;
    ROOT::Math::XYZVector momMagnet;
    // 8 stations; 3 planes per station
    ROOT::Math::XYZVector posSSD[8][3];
    ROOT::Math::XYZVector momSSD[8][3];
    double pullSSD[8][3]; // residual distance from track to point in SSD
    double uncPull[8][3]; // uncertainty on residual

    //    SRVector3D mom; // momentum vector
    SRArichID arich; // arich loglikelihooods values
//  SRArichID arML; // arich ML pred values
    double chi2;
    int ndf;

    int label;
    int ntrkseg;
    std::vector<SRTrackSegment> sgmnt; // vector of track segments
    void Add(SRTrackSegment& ts){ sgmnt.push_back(ts); ntrkseg=sgmnt.size(); };


    virtual void setDefault();
    virtual void Clear();
  };

} // end namespace

#endif // SRTRACK_H
