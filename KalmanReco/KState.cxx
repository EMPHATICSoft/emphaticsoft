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

   void State::Print(){
    
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "z = " << fZ << endl;
    cout << "*****************************TRACK PARAMETERS*****************************" << endl;
    for(int i = 0; i < 5; i++){
      cout << "p" << i+1 << " = " << fPar(i) << endl;
    }
    cout << "*****************************COVARIANCE MATRIX*****************************" << endl;
    int ind1 = 0;
    //int ind2 = 0;
    for(int i = 0; i < 5; i++){
      for(int j = 0; j < 5; j++){
	cout << fCov(i,j) << "       ";
      }
      cout << endl;
    }	
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   }
}

