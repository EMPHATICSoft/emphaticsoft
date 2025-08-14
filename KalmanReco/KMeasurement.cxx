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
      fMeas2D = parIn.GetMeas2D();
      fCov2D = parIn.GetMeasCov2D();
      fHMatrix2D = parIn.GetHMatrix2D();
      fIs1D = parIn.Is1D();
    }
    return *this;
  }

  //*******************************************************************

  std::ostream& operator << (std::ostream& o, const Measurement& m)
  {
    o << "z = " << m.fZ << endl;
    if (m.fIs1D) {
      o << "m = " << m.fMeas << endl;
      o << "sigma_m = " << m.fCov << std::endl;
      o << "H = " << m.fHMatrix << std::endl;
    }
    else {
      o << "m = " << m.fMeas2D << endl;
      o << "sigma_m = " << m.fCov2D << std::endl;
      o << "H = " << m.fHMatrix2D << std::endl;
    }
    return o;
 }

}

