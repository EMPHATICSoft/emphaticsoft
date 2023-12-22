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
    void    ClosestApproach(TVector3 A,TVector3 B, TVector3 C, TVector3 D, double F[3], double l1[3], double l2[3]);
    void    findLine(std::vector<std::vector<double> > v, double lfirst[3], double llast[3]);
    double  findRecoAngle(double p1[3], double p2[3], double p3[3], double p4[3]);
    double  findTruthAngle(std::vector<sim::SSDHit> sim_i, std::vector<sim::SSDHit> sim_f);

    void    SetEvtNum(int num){ fEvtNum = num; }   

    friend std::ostream& operator << (std::ostream& o, const RecoUtils& h);
  };
  
}

#endif // RECOUTILS_H
