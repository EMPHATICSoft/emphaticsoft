#ifndef KTRACKER_H
#define KTRACKER_H

#include <iostream>
#include <string>

#include "KalmanReco/KState.h"
#include "KalmanReco/KLSMeasurement.h"
#include "KalmanReco/KResidual.h"
#include "MagneticField/MagneticField.h"
#include "Geometry/Geometry.h"

using namespace std;

typedef ROOT::Math::SMatrix<double,1,5> KHMatrix;
typedef ROOT::Math::SMatrix<double,5,1> KHMatrixTranspose;

namespace kalman {

  class KTracker{
    
  public:
    KTracker();
    ~KTracker() {};
    void AddMeasurement(KLSMeasurement& meas) { fMeasurements.push_back(meas); };
    bool RunForwardFilter(int ioffset = 0);
    bool RunBackwardFilter();
    bool RunSmoother();  // backward pass
    // Get results
    const std::vector<KState>& GetFilteredStates() const { return fFiltered_states; }
    const std::vector<KState>& GetSmoothedStates() const { return fSmoothed_states; }
    const KState& GetFinalState() const { return fFiltered_states.back(); }

    // Calculate chi2 from smoothed states (independent verification)
    std::pair<double, int> CalculateChi2FromSmoothedStates(std::vector<KResidual>& residuals);

    // Setters

    void SetVerbosity(int v) { fVerbosity = v; }
    void SetBField(emph::MagneticField* field) { fBfield = field; }
//    void SetGeometry(emph::geo::Geometry* geo) { fGeo = geo; }
    void SetSpeedOfLight(double c) { fSOL = c; }
//    void SetProcessNoise(double pos, double angle, double qp);
    void SetRK4Parameters(int max_steps, double max_step_size);
    void AddRadLength(double radLength) { fRadLengths.push_back(radLength); }
    void SetInitialState(KState& state) { fFiltered_states.clear(); fFiltered_states.push_back(state); }
    void Clear();
    void ClearRadLengths() { fRadLengths.clear(); }
    void ClearSmoothStates() { fSmoothed_states.clear(); }
    void ClearFilteredStates() { fFiltered_states.clear(); }

  private:
    KState Predict( KState& sstart, double z, int imeas);
    KState Update(KState& pstate, KLSMeasurement& meas);
    double Propagate( KStateVec& state_in, double z_start, double z_end, KStateVec& state_out);
    // RK4 propagation of state vector, use when in B field
    double PropagateRK4( KStateVec& state_in, double z_start, double z_end, KStateVec& state_out);
    KStateCov GetNoise(KStateVec& state, double dz, double len);
    KStateCov CalculateJacobian( KStateVec& state, double z_start, double z_end);
    // Calculate transport matrix (Jacobian) using RK4, use when in B field
    KStateCov CalculateJacobianRK4( KStateVec& state, double z_start, double z_end);
    
    // Equations of motion: d(state)/dz
    void EquationsOfMotion(KStateVec& state, double z, KStateVec& derivatives, double sgn = 1.0);

    KState Smooth( KState& filtered,  KState& predicted_next,
                  KState& smoothed_next, KStateCov& transport_matrix);

//    KStateCov GetProcessNoise(double dz);

    // Get measurement matrix H for line segment measurement
    // H maps state (x,y,dx/dz,dy/dz,q/p) to measurement (distance to line)
    KMeas GetMeasurementPrediction( KState& state,
                                   KLSMeasurement& meas);
    KHMatrix GetMeasurementJacobian( KState& state,
                                 KLSMeasurement& meas);
  private:
    int fVerbosity;
    emph::MagneticField* fBfield;
//    emph::geo::Geometry* fGeo;
    std::vector<KLSMeasurement> fMeasurements;
    std::vector<double> fRadLengths; // radiation lengths between each measurement layer
    std::vector<KState> fFiltered_states;
    std::vector<KState> fSmoothed_states;

//    double fProcessNoisePosition;
//    double fProcessNoiseQp;

    // RK4 parameters
    int fMaxRK4Steps;
    double fMaxStepSize;
    double fSOL; // speed of light
  };
}

#endif
