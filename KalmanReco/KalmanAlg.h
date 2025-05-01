#ifndef KALMANALG_H
#define KALMANALG_H

#include "RecoBase/LineSegment.h"
#include "RecoBase/Track.h"
#include "Geometry/Geometry.h"
#include "MagneticField/MagneticField.h"
#include "KalmanReco/KState.h"
#include "KalmanReco/KMeasurement.h"
#include "KalmanReco/Propagator.h"

using namespace std;

namespace kalman {
  
  class KalmanAlg{
    
  public:
    KalmanAlg();
    KalmanAlg(const emph::geo::Geometry* g, const emph::MagneticField* b);
    ~KalmanAlg() {};
    
    rb::Track Fit(std::vector<rb::LineSegment>& lsv, State& initState);
    void SetVerbosity(int v) {fVerbosity = v; }
    
  private:
    
    const emph::geo::Geometry* fGeo;
    const emph::MagneticField* fBfield;
    int   fVerbosity;
    K5x5 fF;
    KHMatrix fH;
    KHMatrixTranspose fHT;
    KHMatrix2 fH2;
    KHMatrixTranspose2 fH2T;
    Propagator fProp;
  };
}

#endif
