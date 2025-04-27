#include "KState.h"

namespace kalman {

  //*******************************************************************

  State::State()
  {
    fZ = 0;
    for (int i=0; i<5; ++i) {
      fPar(i) = 0.;
      for (int j=0; j<5; ++j) 
	fCov(i,j) = 0.;
    }
  }
  
  //*******************************************************************
  
  State::State(double z, KPar& par)
  {
    fZ = z;
    fPar = par;
    for(int i = 0; i < 5; i++)
      for (int j=0; j<5; ++j) 
	fCov(i,j) = 0;
  }  

  //*******************************************************************

  State::State(double z, KPar& par, K5x5& cov)
  {
    fZ = z;
    fPar = par;
    fCov = cov;
  }

  //*******************************************************************

  State& State::operator= (State &parIn){
    if(this != &parIn){
      fZ = parIn.GetZ();
      fPar = parIn.GetPar();
      fCov = parIn.GetCov();
    }
    return *this;
  }

  //*******************************************************************

  std::ostream& operator << (std::ostream& o, const State& s)
  {  
    o << "z = " << s.fZ << std::endl;;
    o << "par = " << s.fPar << std::endl;
    o << "cov = " << s.fCov << std::endl;
    return o;
   }
}

