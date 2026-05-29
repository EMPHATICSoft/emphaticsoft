#ifndef KRESIDUAL_H
#define KRESIDUAL_H

#include <vector>
#include <iostream>
#include "Math/SVector.h"
#include "Math/SMatrix.h"
#include "TMath.h"
#include "RecoBase/LineSegment.h"

using namespace std;

typedef ROOT::Math::SVector<double,1> KMeas;
typedef ROOT::Math::SMatrix<double,1,1> KMeasCov;

namespace kalman {
  
  class KResidual {
  public:
    KResidual();
    KResidual(double residual, double sigma, int station, int plane);
    
    ~KResidual() {};

    double GetResidual() const { return fResidual; }
    double GetSigma() const { return fSigma; }
    int GetStation() const { return fStation; }
    int GetPlane() const { return fPlane; }

    void SetResidual(double residual) { fResidual = residual; }
    void SetSigma(double sigma) { fSigma = sigma; }
    void SetStation(int station) { fStation = station; }
    void SetPlane(int plane) { fPlane = plane; }

    friend std::ostream& operator << (std::ostream& o, const KResidual& s);

  private:
    double fResidual;       // distance from track to line
    double fSigma;          // uncertainty on residual
    int    fStation;        // SSD station number (for diagnostics)
    int    fPlane;          // SSD plane number (for diagnostics)
  };
}  
  

#endif
