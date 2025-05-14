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
    KalmanAlg(emph::geo::Geometry* g, emph::MagneticField* b);
    ~KalmanAlg() {};
    
    rb::Track Fit(std::vector<rb::LineSegment>& lsv, State& initState);
    void SetVerbosity(int v) {fVerbosity = v; fProp.SetVerbosity(v); }
    
  private:
    void SimpleExtrap(State& state1, State& state2, double dz);
    double ExtrapInField(State& state1, State& state2, double dz);

    void Update1D(State& state, double d, double sigma);
    void Update2D(State& state, double d, double sigma);
    void LinearSmooth(State& APost1, State& APost2, State& APrior2, double d);
    
    emph::geo::Geometry* fGeo;
    emph::MagneticField* fBfield;
    std::vector<State> fApriorState;
    std::vector<State> fApostState;
    std::vector<double> zPos;
    int   fVerbosity;
    double fCosG; // cos(gamma)
    double fSinG; // sin(gamma)
    double fFieldZmin;
    double fFieldZmax;
    K5x5 fF;
    KHMatrix fH;
    KHMatrixTranspose fHT;
    KHMatrix2D fH2D;
    KHMatrixTranspose2D fH2DT;
    Propagator fProp;
  };
}

#endif
