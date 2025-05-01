#ifndef KMEASUREMENT_H
#define KMEASUREMENT_H

#include <vector>
#include <iostream>
#include "Math/SVector.h"
#include "Math/SMatrix.h"

using namespace std;

typedef ROOT::Math::SVector<double,1> KMeas;
typedef ROOT::Math::SMatrix<double,1,1> KMeasCov;
typedef ROOT::Math::SVector<double,2> KMeas2;
typedef ROOT::Math::SMatrix<double,2,2> KMeasCov2;
typedef ROOT::Math::SMatrix<double,1,5> KHMatrix;
typedef ROOT::Math::SMatrix<double,5,1> KHMatrixTranspose;
typedef ROOT::Math::SMatrix<double,2,5> KHMatrix2;
typedef ROOT::Math::SMatrix<double,5,2> KHMatrixTranspose2;

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
    double GetZ(){ return fZ; }

    void SetMeas(KMeas& m) { fMeas = m; }
    void SetMeasCov(KMeasCov& cov) { fCov = cov; }
    void SetHMatrix(KHMatrix& h) { fHMatrix = h; }
    void SetZ(double z) { fZ = z;}

    friend std::ostream& operator << (std::ostream& o, const Measurement& s);

  private:
    
    double fZ;
    KMeas fMeas;
    KMeasCov fCov;
    KHMatrix fHMatrix;
        
  };
}  
  

#endif
