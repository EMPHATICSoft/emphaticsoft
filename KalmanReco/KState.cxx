#include "KalmanReco/KState.h"

namespace kalman {

  //*******************************************************************

  KState::KState()
  {
    fZ = 0;
    fChi2 = 0;
    fNDF = 0;
    for (int i=0; i<5; ++i) {
      fState(i) = 0.;
      for (int j=0; j<5; ++j) 
      	fCov(i,j) = 0.;
    }
  }
  
  //*******************************************************************

  KState::KState(const KStateVec& par, const KStateCov& cov, double z)
  {
    fZ = z;
    fChi2 = 0;
    fNDF = 0;
    fState = par;
    fCov = cov;
  }

  //*******************************************************************

  KState& KState::operator= (const KState &s){
    if(this != &s){
      fZ = s.GetZ();
      fState = s.GetPar();
      fCov = s.GetCov();
    }
    return *this;
  }

  //*******************************************************************

  std::ostream& operator << (std::ostream& o, const KState& s)
  {  
    o << "z = " << s.fZ << std::endl;;
    o << "par = " << s.fState << std::endl;
    o << "cov = " << s.fCov << std::endl;
    return o;
   }
}

