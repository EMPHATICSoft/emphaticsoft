#ifndef KSTATE_H
#define KSTATE_H

#include <vector>
#include <iostream>
#include "Math/SVector.h"
#include "Math/SMatrix.h"

using namespace std;

typedef ROOT::Math::SVector<double,5> KPar;
typedef ROOT::Math::SMatrix<double,5,5> K5x5;

namespace kalman {
  
  class State {
  public:
    State();
    State(double z, KPar& par);
    State(double z, KPar& par, K5x5& cov);
    ~State(){};

    State& operator= (State &parIn);

    double GetPar(int i) { return fPar(i); }
    double GetCov(int i, int j) { return fCov(i,j); }
    K5x5 GetCov(){return fCov;}
    KPar GetPar(){return fPar;}
    double GetZ(){return fZ;}
    void SetPar(KPar& p) { fPar = p; }
    void SetCov(K5x5& cov) { fCov = cov; }
    void SetZ(double val){fZ = val;}
    void Print();
  private:
    
    double fZ;
    
    KPar fPar;
    K5x5 fCov;
        
  };
}  
  

#endif
