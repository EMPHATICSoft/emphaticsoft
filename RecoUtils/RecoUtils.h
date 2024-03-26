////////////////////////////////////////////////////////////////////////
/// \brief   Class for reconstruction functions
/// \author  Robert Chirco
/// \date    10/30/23
/// \song    High speed calm air tonight by ML Buch
////////////////////////////////////////////////////////////////////////
#ifndef RECOUTILS_H
#define RECOUTILS_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "Simulation/SSDHit.h" 
#include "RecoBase/TrackSegment.h"

#include "TVector3.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"

namespace ru {
  
  class RecoUtils {
  public:
    RecoUtils(); // Default constructor
    RecoUtils(int num);
    virtual ~RecoUtils() {}; //Destructor
    
  private:
    int fEvtNum;

  public:
    void    ClosestApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double F[3], double l1[3], double l2[3], const char* type);
    void    ClampedApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double l1[3], double l2[3], double sbound[2], double tbound[2], const char* type);
    void    findLine(std::vector<std::vector<double> > v, double lfirst[3], double llast[3]);
    double  findRecoAngle(double p1[3], double p2[3], double p3[3], double p4[3]);
    double  findTruthAngle(std::vector<sim::SSDHit> sim_i, std::vector<sim::SSDHit> sim_f);
    double  getMomentum(double theta);
    void    findTrackIntersection(rb::TrackSegment trk1, rb::TrackSegment trk2, double point[3]);

    void    SetEvtNum(int num){ fEvtNum = num; }   

    friend std::ostream& operator << (std::ostream& o, const RecoUtils& h);
  };
  
}

#endif // RECOUTILS_H
