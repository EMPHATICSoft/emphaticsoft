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
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/SpacePoint.h"
#include "RecoBase/TrackSegment.h"
#include "Simulation/SSDHit.h"

/*
#include "Simulation/SSDHit.h" 
#include "RecoBase/TrackSegment.h"

#include "TVector3.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"
*/

namespace emph {

  class SingleTrackAlgo {
  public:
    SingleTrackAlgo(); // Default constructor
    SingleTrackAlgo(int fevtnum, size_t nstations, size_t nplanes, std::string fg4label, std::string fclusterlabel, std::string fclustercut);
    SingleTrackAlgo(size_t nstations, size_t nplanes);
    virtual ~SingleTrackAlgo() {}; //Destructor
    //~SingleTrackAlgo(); // Destructor
    int fEvtNum;
    size_t nStations;
    size_t nPlanes;
    std::string fG4Label;
    std::string fClusterLabel;
    std::string fClusterCut;

  private:
    std::map<std::pair<int, int>, int> clustMap;
    std::vector<const rb::SSDCluster*> clusters;
    std::vector<const rb::LineSegment*> linesegments;
    art::Handle< std::vector<rb::SSDCluster> > clustH;
    art::Handle< std::vector<rb::LineSegment> > lsH;
    art::Handle< std::vector<sim::SSDHit> > ssdHitH;
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group;
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group;
    int         goodclust = 0;
    int         badclust = 0;
    int st;
    bool goodEvent;
    std::vector<rb::SpacePoint> spv; 
    std::vector<std::vector<double>> sp1;
    std::vector<std::vector<double>> sp2;
    std::vector<std::vector<double>> sp3;
    double sectrkp[3];
    double sectrkvtx[3];
    std::vector<rb::TrackSegment> tsv;

  public:
    // Define functions here
    bool MakeSelection(art::Event& evt, std::string fClusterCut);  
    void MakeSegment(const rb::SSDCluster& cl, rb::LineSegment& ls);
    std::vector<rb::SpacePoint> MakeHits(std::vector<std::vector<std::vector<const rb::SSDCluster*> > > cl_group, std::vector<std::vector<std::vector<const rb::LineSegment*> > > ls_group);
    bool MakeRegions(std::vector<rb::SpacePoint> spv);
    std::vector<rb::TrackSegment> MakeLines(std::vector<std::vector<double>> sp1, std::vector<std::vector<double>> sp2, std::vector<std::vector<double>> sp3);
   
    std::vector<std::vector<std::vector<const rb::SSDCluster*> > > GetCLGroup(){ return cl_group; }
    std::vector<std::vector<std::vector<const rb::LineSegment*> > > GetLSGroup(){ return ls_group; }
    std::vector<std::vector<double>> GetSP1(){ return sp1; }
    std::vector<std::vector<double>> GetSP2(){ return sp2; }
    std::vector<std::vector<double>> GetSP3(){ return sp3; }

    void SetSecTrkP(const double* p) { for (int i=0; i<3; ++i) sectrkp[i] = p[i]; };
    void SetSecTrkVtx(const double* x0) { for (int i=0; i<3; ++i) sectrkvtx[i] = x0[i]; };
    const double* GetSecTrkP() const { return sectrkp; }
    const double* GetSecTrkVtx() const { return sectrkvtx; }

    friend std::ostream& operator << (std::ostream& o, const SingleTrackAlgo& h);
  };
  
}

#endif // SINGLETRACKALGO_H
