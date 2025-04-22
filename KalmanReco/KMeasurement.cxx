#include "KMeasurement.h"

namespace kalman {

  //*******************************************************************

  Measurement::Measurement()
  {
    fZ = 0;
    fMeas(0) = 0.;
    fCov(0,0) = 0.;

    for (int i=0; i<5; ++i) 
      fHMatrix(0,i) = 0.;
  }
  
  //*******************************************************************
  
  Measurement::Measurement(KMeas& m)
  {
    fZ = 0; 
    fMeas = m;
    fCov(0,0)=sqrt(1./12.)*0.06; // 60 um pitch, this assumes a single strip 
    
    for (int i=0; i<5; ++i) 
      fHMatrix(0,i) = 0.;
    fHMatrix(0,0) = 1.;
  }  

  //*******************************************************************

  Measurement::Measurement(KMeas& m, KMeasCov& c)
  {
    fZ = 0;
    fMeas = m;
    fCov = c;
    for (int i=0; i<5; ++i) 
      fHMatrix(0,i) = 0.;
    fHMatrix(0,0) = 1.;
  }

  //*******************************************************************

  Measurement::Measurement(KMeas& m, KMeasCov& c, KHMatrix& h)
  {
    fMeas = m;
    fCov = c;
    fHMatrix = h;
    fZ = 0;
  }

  //*******************************************************************

  Measurement& Measurement::operator= (Measurement &parIn){
    if(this != &parIn){
      fZ = parIn.GetZ();
      fMeas = parIn.GetMeas();
      fCov = parIn.GetMeasCov();
      fHMatrix = parIn.GetHMatrix();
    }
    return *this;
  }

  //*******************************************************************

   void Measurement::Print() {
    
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout << "z = " << fZ << endl;
    cout << "*****************************MEASURED STRIP*****************************" << endl;
    cout << "m = " << fMeas(0) << endl;
    cout << "*****************************MEASUREMENT UNCERTAINTY *****************************" << endl;
    cout << "sigma_m = " << fCov(0,0) << endl;
    cout << "*****************************H MATRIX *****************************" << endl;
    for(int i = 0; i < 5; i++)
      cout << fHMatrix(0,i) << "       ";
    cout << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
   }
}

