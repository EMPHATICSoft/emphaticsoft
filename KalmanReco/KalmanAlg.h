#ifndef KALMANALG_H
#define KALMANALG_H

#include "RecoBase/LineSegment.h"
#include "RecoBase/Track.h"
#include "Geometry/Geometry.h"
#include "MagneticField/MagneticField.h"

using namespace std;

namespace kalman {

  class KalmanAlg{
    
  public:
    KalmanAlg();
    KalmanAlg(const emph::geo::Geometry* g, const emph::MagneticField* b);
    ~KalmanAlg() {};

    rb::Track Fit(std::vector<rb::LineSegment>& lsv);
    void SetVerbosity(int v) {fVerbosity = v; }

  private:
    
    const emph::geo::Geometry* fGeo;
    const emph::MagneticField* fBfield;
    int   fVerbosity;
  };
}

#endif
