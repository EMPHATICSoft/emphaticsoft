#ifndef KTRACKER_H
#define KTRACKER_H

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

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
    // Independent scalar innovations from the latest linear-Gaussian
    // forward-filter pass. Their variances include predicted process noise.
    // The vectors correspond to GetMeasurements()[GetForwardInnovationOffset()+i].
    const std::vector<double>& GetForwardInnovations() const
    {
      return fForwardInnovations;
    }
    const std::vector<double>& GetForwardInnovationVariances() const
    {
      return fForwardInnovationVariances;
    }
    // d(innovation)/d(alignment parameter), when measurement-prediction
    // derivatives were configured before the forward pass.
    const std::vector<std::vector<double>>& GetForwardInnovationJacobians() const
    {
      return fForwardInnovationJacobians;
    }
    int GetForwardInnovationOffset() const { return fForwardInnovationOffset; }
    const std::vector<KLSMeasurement>& GetMeasurements() const
    {
      return fMeasurements;
    }
    // Set derivatives of the measurement prediction with respect to the
    // alignment parameters, one row per AddMeasurement call. Rows are reordered
    // with measurements by RunForwardFilter. The initial track state is treated
    // as independent of alignment parameters; nonzero offsets are unsupported
    // when sensitivities are enabled. Sensitivities use the first-order
    // transport Jacobian and hold the Kalman gain fixed during each update.
    void SetMeasurementPredictionDerivatives(
      const std::vector<std::vector<double>>& derivatives);
    const KState& GetFinalState() const { return fFiltered_states.back(); }

    // Calculate chi2 from smoothed states (independent verification)
    std::pair<double, int> CalculateChi2FromSmoothedStates(std::vector<KResidual>& residuals);

    // Setters

    void SetVerbosity(int v) { fVerbosity = v; }
    void SetBField(emph::MagneticField* field) { fBfield = field; }
    // Straight-line tracking without a field map (for unit tests).
    void SetFieldFree(bool b) { fFieldFree = b; }
    // Replaces the field map by a function B(pos) in kGauss, valid for
    // zMin < z < zMax (for unit tests).
    using FieldFunction = std::function<void(const double*, double*)>;
    void SetFieldFunction(FieldFunction f, double zMin, double zMax)
    {
      fFieldFunction = std::move(f);
      fFieldZMin = zMin;
      fFieldZMax = zMax;
    }
    // Translation (mm) of the field map in x and y: the field at position r is
    // the map evaluated at r - offset.
    void SetFieldOffset(double dx, double dy) { fFieldOffsetX = dx; fFieldOffsetY = dy; }
    // Indices, in the alignment parameter vector of the derivative rows, of
    // the field x and y translations (-1 if not fitted). The sensitivity of
    // the propagation is a central finite difference with the given step (mm).
    void SetFieldShiftParameters(int ixIndex, int iyIndex, double step = 1.0)
    {
      fFieldShiftIndex[0] = ixIndex;
      fFieldShiftIndex[1] = iyIndex;
      fFieldShiftStep = step;
    }
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
    KState Predict(KState& sstart, double z, int imeas,
                   std::vector<KStateVec>* stateDerivatives = nullptr);
    KState Update(KState& pstate, KLSMeasurement& meas,
                  double* innovation = nullptr,
                  double* innovationVariance = nullptr,
                  const std::vector<double>* predictionDerivatives = nullptr,
                  std::vector<KStateVec>* stateDerivatives = nullptr,
                  std::vector<double>* innovationDerivatives = nullptr);
    bool InField(double z_start, double z_end);
    bool HasField() const { return fBfield || fFieldFunction; }
    void GetField(double x, double y, double z, double B[3]);
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
    std::vector<double> fForwardInnovations;
    std::vector<double> fForwardInnovationVariances;
    std::vector<std::vector<double>> fForwardInnovationJacobians;
    std::vector<std::vector<double>> fMeasurementPredictionDerivatives;
    int fForwardInnovationOffset = 0;
    bool fFieldFree = false;
    FieldFunction fFieldFunction;
    double fFieldZMin = 0.;
    double fFieldZMax = 0.;
    double fFieldOffsetX = 0.;
    double fFieldOffsetY = 0.;
    int fFieldShiftIndex[2] = {-1, -1};
    double fFieldShiftStep = 1.0;

//    double fProcessNoisePosition;
//    double fProcessNoiseQp;

    // RK4 parameters
    int fMaxRK4Steps;
    double fMaxStepSize;
    double fSOL; // speed of light
  };
}

#endif
