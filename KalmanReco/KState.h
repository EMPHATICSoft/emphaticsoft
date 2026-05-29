#ifndef KSTATE_H
#define KSTATE_H

#include <vector>
#include <iostream>
#include "Math/SVector.h"
#include "Math/SMatrix.h"

using namespace std;

typedef ROOT::Math::SVector<double,5> KStateVec;
typedef ROOT::Math::SMatrix<double,5,5> KStateCov;

namespace kalman {
  
  // Kalman Filter state class
class KState {
public:
    KState();
    KState(const KStateVec& s, const KStateCov& cov, double z_pos);
    virtual ~KState() {};
    double GetPar(int i) const { return fState(i); }
    double GetCov(int i, int j) const { return fCov(i,j); }
    KStateCov GetCov() const {return fCov;}
    KStateVec GetPar() const {return fState;}
    double GetZ() const {return fZ;}
    double GetChi2() const {return fChi2;}
    double GetNdf() const {return fNDF;}

    void SetChi2(double c){fChi2=c;}
    void SetNdf(double n){fNDF=n;} 
    void SetPar(KStateVec& p) { fState = p; }
    void SetPar(int i, double val) { fState(i) = val; }
    void SetCov(KStateCov& cov) { fCov = cov; }
    void SetZ(double val){fZ = val;}

    KState& operator= (const KState &s);
    friend std::ostream& operator << (std::ostream& o, const KState& s);

private:
    KStateVec fState;      // State vector: (x, y, dx/dz, dy/dz, q/p)
    KStateCov fCov; // 5x5 covariance matrix
    double fZ;            // z-position of this state
    double fChi2;         // accumulated chi-squared
    int fNDF;             // number of degrees of freedom

  };
}  
  

#endif
