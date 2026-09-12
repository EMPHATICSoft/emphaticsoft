#include "KalmanReco/KResidual.h"

namespace kalman {

  KResidual::KResidual(double residual, double sigma, int station, int plane) : fResidual(residual), fSigma(sigma), fStation(station), fPlane(plane) {}

  std::ostream& operator << (std::ostream& o, const KResidual& r)
  {
    o << "KResidual (station=" << r.fStation << ", plane=" << r.fPlane << ") = " << r.fResidual 
      << " +/- " << r.fSigma << " mm" << std::endl;
    return o;
  }
}

