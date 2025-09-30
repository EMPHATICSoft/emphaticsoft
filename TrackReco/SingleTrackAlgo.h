////////////////////////////////////////////////////////////////////////
/// \brief   Class for single track algorithm
/// \author  Robert Chirco
/// \date    5/30/24
/// \song    Dejavu by Lolina
////////////////////////////////////////////////////////////////////////
#ifndef SINGLETRACKALGO_H
#define SINGLETRACKALGO_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "RecoUtils/RecoUtils.h"

namespace emph {

  class SingleTrackAlgo {
  public:
    SingleTrackAlgo(); // Default constructor
    SingleTrackAlgo(int num, size_t nstations, size_t nplanes);
    virtual ~SingleTrackAlgo() {}; //Destructor

    int fEvtNum = -1;
    size_t nStations;
    size_t nPlanes;

  private:
    ru::RecoUtils recoFcn = ru::RecoUtils(fEvtNum);

    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group;
    std::vector<rb::SpacePoint> spv; 
    std::vector<std::vector<double>> sp1;
    std::vector<std::vector<double>> sp2;
    std::vector<std::vector<double>> sp3;
    double beamtrkp[3];
    double beamtrkvtx[3];
    double sectrkp[3];
    double sectrkvtx[3];
    std::vector<rb::TrackSegment> tsv;

  public:
    // Define functions here
    std::vector<rb::SpacePoint> MakeHits(std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group, std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group);
    std::vector<rb::TrackSegment> MakeLines(std::vector<std::vector<double>> sp1, std::vector<std::vector<double>> sp2, std::vector<std::vector<double>> sp3);
    std::vector<rb::TrackSegment> MakeTrackSeg(std::vector<rb::SpacePoint> spacepoints);
    void SetBeamTrk(rb::TrackSegment &ts1, double pbeam);
    void SetRecoTrk(rb::TrackSegment &ts2, rb::TrackSegment &ts3, int pm);
    void SetPtmp(rb::TrackSegment& ts);
    void getCombinations(std::vector<std::vector<rb::SpacePoint>> &matrix, int row, std::vector<rb::SpacePoint> &combination, std::vector<std::vector<rb::SpacePoint>> &result, int stop, int s);
    void doTwoPlanes(const rb::LineSegment* ls1, const rb::LineSegment* ls2, double x[3]);
    void doThreePlanes(const rb::LineSegment* ls1, const rb::LineSegment* ls2, const rb::LineSegment* ls3, double x[3]);
    std::vector<rb::SpacePoint> MakeHitsOrig(std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group);
   
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > GetCLGroup(){ return cl_group; }
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > GetLSGroup(){ return ls_group; }
    std::vector<std::vector<double>> GetSP1(){ return sp1; }
    std::vector<std::vector<double>> GetSP2(){ return sp2; }
    std::vector<std::vector<double>> GetSP3(){ return sp3; }

    void SetSecTrkP(const double* p) { for (int i=0; i<3; ++i) sectrkp[i] = p[i]; };
    void SetSecTrkVtx(const double* x0) { for (int i=0; i<3; ++i) sectrkvtx[i] = x0[i]; };
    const double* GetSecTrkP() const { return sectrkp; }
    const double* GetSecTrkVtx() const { return sectrkvtx; }
    double* SetTrackInfo(rb::TrackSegment &ts1, rb::TrackSegment &ts2);

    int GetEvtNum() const { return fEvtNum; }
    size_t NStations() const { return nStations; }
    size_t NPlanes() const { return nPlanes; } 

    friend std::ostream& operator << (std::ostream& o, const SingleTrackAlgo& h);
  };
  
}

#endif // SINGLETRACKALGO_H
