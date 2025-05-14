#ifndef KMEASUREMENT_H
#define KMEASUREMENT_H

#include <vector>
#include <iostream>
#include "Math/SVector.h"
#include "Math/SMatrix.h"

using namespace std;

typedef ROOT::Math::SVector<double,1> KMeas;
typedef ROOT::Math::SMatrix<double,1,1> KMeasCov;
typedef ROOT::Math::SMatrix<double,1,5> KHMatrix;
typedef ROOT::Math::SMatrix<double,5,1> KHMatrixTranspose;
typedef ROOT::Math::SVector<double,2> KMeas2D;
typedef ROOT::Math::SMatrix<double,2,2> KMeasCov2D;
typedef ROOT::Math::SMatrix<double,2,5> KHMatrix2D;
typedef ROOT::Math::SMatrix<double,5,2> KHMatrixTranspose2D;

namespace kalman {
  
  class Measurement {
  public:
    Measurement();
    Measurement(KMeas& meas, KMeasCov& cov);
    Measurement(KMeas& meas, KMeasCov& cov, KHMatrix& h);
    ~Measurement() {};

    Measurement& operator= (Measurement &parIn);

    KMeas GetMeas() { return fMeas;}
    KMeasCov GetMeasCov() { return fCov;}
    KHMatrix GetHMatrix() { return fHMatrix;}
    KMeas2D GetMeas2D() { return fMeas2D;}
    KMeasCov2D GetMeasCov2D() { return fCov2D;}
    KHMatrix2D GetHMatrix2D() { return fHMatrix2D;}
    double GetZ(){ return fZ; }
    bool Is1D() { return fIs1D; }

    void SetMeas(KMeas& m) { fMeas = m; }
    void SetMeasCov(KMeasCov& cov) { fCov = cov; }
    void SetHMatrix(KHMatrix& h) { fHMatrix = h; }
    void SetMeas2D(KMeas2D& m) { fMeas2D = m; }
    void SetMeasCov2D(KMeasCov2D& cov) { fCov2D = cov; }
    void SetHMatrix2D(KHMatrix2D& h) { fHMatrix2D = h; }
    void SetZ(double z) { fZ = z;}

    friend std::ostream& operator << (std::ostream& o, const Measurement& s);

  private:
    
    bool   fIs1D;
    double fZ;
    KMeas fMeas;
    KMeasCov fCov;
    KHMatrix fHMatrix;
    KMeas2D fMeas2D;
    KMeasCov2D fCov2D;
    KHMatrix2D fHMatrix2D;
        
  };
}  
  

#endif
