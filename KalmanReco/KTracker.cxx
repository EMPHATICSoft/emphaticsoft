#include "KalmanReco/KTracker.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace kalman {

  KTracker::KTracker()
  {
    fVerbosity = 0;
    fBfield = nullptr;
//    fGeo = nullptr;
 //   fProcessNoisePosition = 0.001;
 //   fProcessNoiseQp = 0.0;

    fMaxRK4Steps = 800;
    fMaxStepSize = 1.0; // mm
    fSOL = 0.299792458e-4; // 1 T.m = 0.29979 GeV/c; convert to kGauss (10kG/T) and mm (1000mm/m) gives x1^4
  }

  // ------------------------------------------------------------
  void KTracker::Clear()
  {
    fMeasurements.clear();
    fRadLengths.clear();
    fFiltered_states.clear();
    fSmoothed_states.clear();
  }

  // ------------------------------------------------------------
/*  void KTracker::SetProcessNoise(double pos, double angle, double qp)
  {
    fProcessNoisePosition = pos;
    fProcessNoiseQp = qp;
  }
    */ 
  // ------------------------------------------------------------
    void KTracker::SetRK4Parameters(int max_steps, double max_step_size)
  {
    fMaxRK4Steps = max_steps;
    fMaxStepSize = max_step_size;
  }    
  // ------------------------------------------------------------
  
  void KTracker::EquationsOfMotion(KStateVec& state, double z, KStateVec& derivatives, double sgn)
  {
    // d(state)/dz = derivatives

    double x = state[0];
    double y = state[1];
    double tx = state[2];
    double ty = state[3];
    double qp = state[4];

    derivatives[0] = tx;
    derivatives[1] = ty;

    double u = 1 + tx*tx + ty*ty;
    double B[3] = {0.0, 0.0, 0.0};
    double pos[3] = {x, y, z};
    if (fBfield) {
      fBfield->GetFieldValue(pos,B);
      // convert back to units we can use
      for (int i=0; i<3; ++i) B[i] /= CLHEP::kilogauss; //*sgn;
    }
    if (fVerbosity>100)
      std::cout << "KTracker::EquationsOfMotion at (x,y,z) = (" << x << ", " << y << ", " << z << ") mm, B = (" << B[0] << ", " << B[1] << ", " << B[2] << ") kGauss" << std::endl;

    derivatives[2] = u * qp * fSOL * (ty*B[2] - B[1] + tx*ty*B[0]);
    derivatives[3] = u * qp * fSOL * (B[0] - tx*B[2] - tx*ty*B[1]);
    
    derivatives[4] = 0.0;
  }

  // ------------------------------------------------------------
  
  double KTracker::Propagate(KStateVec& state_in, double z_start, double z_end, KStateVec& state_out)
  {
    // Simple straight-line propagation (no B field)
    double dz = z_end - z_start;
    state_out = state_in;
    state_out[0] += state_in[2] * dz; // x += dx/dz * dz
    state_out[1] += state_in[3] * dz; // y += dy/dz * dz
    return dz * sqrt(1 + state_in[2]*state_in[2] + state_in[3]*state_in[3]); // path length = dz * sqrt(1 + tx^2 + ty^2)
    // dx/dz, dy/dz, q/p remain unchanged
  }

  // ------------------------------------------------------------

  double KTracker::PropagateRK4(KStateVec& state_in, double z_start, double z_end, KStateVec& state_out)
  {
    if (fVerbosity)
      std::cout << "Propagating from z = " << z_start << " to z = " << z_end << " with RK4" << std::endl; 
    // RK4 propagation
    KStateVec k1, k2, k3, k4;
    KStateVec temp_state;
    double dz = z_end - z_start;
    double sgn = (dz > 0) ? 1.0 : -1.0;
    double zpos = z_start+1.e-6*sgn; // add small offset to avoid numerical issues if starting exactly at measurement plane

    state_out = state_in;

    if (fVerbosity)
      std::cout << "Propagating with RK4: dz = " << dz << ", step size = " << fMaxStepSize << std::endl;
    double ds = 0.;

    bool isLast = false;
    while (zpos >= z_start && zpos <= z_end) {      
      EquationsOfMotion(state_out, zpos + fMaxStepSize*sgn, k1, sgn);
      temp_state = state_out + 0.5 * fMaxStepSize * k1;
      if (fVerbosity > 2)
        std::cout << "\t k1 = " << k1 << ", temp_state = " << temp_state << std::endl;
      EquationsOfMotion(temp_state, zpos + 0.5 * fMaxStepSize*sgn, k2, sgn);
      temp_state = state_out + 0.5 * fMaxStepSize * k2;
      if (fVerbosity > 2)
        std::cout << "\t k2 = " << k2 << ", temp_state = " << temp_state << std::endl;  
      EquationsOfMotion(temp_state, zpos + 0.5 * fMaxStepSize*sgn, k3, sgn);
      temp_state = state_out + fMaxStepSize * k3;
      if (fVerbosity > 2)
        std::cout << "\t k3 = " << k3 << ", temp_state = " << temp_state << std::endl;
      EquationsOfMotion(temp_state, zpos + fMaxStepSize*sgn, k4, sgn);
      if (fVerbosity > 2)
        std::cout << "\t k4 = " << k4 << std::endl;

      state_out += (fMaxStepSize / 6.0) * (k1 + 2*k2 + 2*k3 + k4);
      ds += fMaxStepSize * sqrt(1 + state_out[2]*state_out[2] + state_out[3]*state_out[3]);

      if (isLast) break; // if we just took the last step, exit loop

      zpos += fMaxStepSize*sgn;
      if (zpos > z_end) {
        zpos = z_end;
        isLast = true;
      }
      if (zpos < z_start) {
        zpos = z_start;
        isLast = true;
      }
//      std::cout << "zpos = " << zpos << ", state_out = " << state_out << std::endl;
    }
    if (fVerbosity)
      std::cout << "Finished RK4 propagation to z = " << z_end << ", final state: " << state_out << std::endl;
    if (fVerbosity)
      std::cout << "state_out = " << state_out << std::endl;
    return ds;

  }

    // ------------------------------------------------------------
  
  KStateCov KTracker::CalculateJacobian(KStateVec& state_in, double z_start, double z_end)
  {
    // Simple Jacobian calculation (no B field)
    KStateCov jacobian;
    jacobian[0][0] = 1.0;
    jacobian[1][1] = 1.0;
    jacobian[2][2] = 1.0;
    jacobian[3][3] = 1.0;
    jacobian[4][4] = 1.0;
    jacobian[0][2] = z_end - z_start; // dx/d(dx/dz)
    jacobian[1][3] = z_end - z_start; // dy/d(dy/dz)
    return jacobian;
  }

  // ------------------------------------------------------------
  
  KStateCov KTracker::CalculateJacobianRK4(KStateVec& state_in, double z_start, double z_end)
  {
    KStateCov F;
    const double epsilon = 1e-6;

    KStateVec state_nominal;
    PropagateRK4(state_in, z_start, z_end, state_nominal);

    for (int j = 0; j < 5; j++) {
        KStateVec state_pert = state_in;
        double delta = epsilon;
        if (j == 0 || j == 1) {
            delta = 1e-4;
        } else if (j == 2 || j == 3) {
            delta = 1e-6;
        } else if (j == 4) {
            delta = TMath::Abs(state_in[j]) * 1e-4;
            if (delta < 1e-8) delta = 1e-8;
        }

        state_pert[j] += delta;
        KStateVec state_pert_out;
        PropagateRK4(state_pert, z_start, z_end, state_pert_out);
        for (int i = 0; i < 5; i++) {
            F[i][j] = (state_pert_out[i] - state_nominal[i]) / delta;
        }
    }
    return F;
  }

  // ------------------------------------------------------------
  /*
  KStateCov KTracker::GetProcessNoise(double dz)
  {
    KStateCov Q;
    double sigma = fProcessNoisePosition; // Process noise standard deviation
    double sigma2 = sigma * sigma;
    Q[0][0] = sigma2 * dz;
    Q[1][1] = sigma2 * dz;

    return Q;
  }
*/
  // ------------------------------------------------------------

  KStateCov KTracker::GetNoise(KStateVec& tstate, double ds, double X0)
  {
    double SigTheta = 0.0136*fabs(tstate(4)) * sqrt(X0) * (1.+0.038*log(X0)); // Highland formula, gives RMS scattering angle in radians
    if (fVerbosity > 2)
      std::cout << "ds = " << ds << ", X0 = " << X0 << ", SigTheta x 1e6 = " << SigTheta*1.e6 << std::endl;
    
    double p3 = tstate(2);
    double p4 = tstate(3);	
  	
    double Vartx   = SigTheta*SigTheta * (1 + p3*p3);
    double Varty   = SigTheta*SigTheta * (1 + p4*p4);
    double Covtxty = SigTheta*SigTheta * p3*p4;

    KStateCov C2;
    C2[0][0] = ds*ds*Vartx;
    C2[0][1] = ds*ds*Covtxty;
    C2[0][2] = -ds*Vartx;
    C2[0][3] = -ds*Covtxty;
    C2[1][1] = ds*ds*Varty;
    C2[1][2] = -ds*Covtxty;
    C2[1][3] = -ds*Varty;
    C2[2][2] = Vartx;
    C2[2][3] = Covtxty;
    C2[3][3] = Varty;

    C2[1][0] = C2[0][1]; // Copy symmetric element
    C2[2][0] = C2[0][2]; // Copy symmetric element
    C2[3][0] = C2[0][3]; // Copy symmetric element
    C2[2][1] = C2[1][2]; // Copy symmetric element
    C2[3][1] = C2[1][3]; // Copy symmetric element
    C2[3][2] = C2[2][3]; // Copy symmetric element

    return C2;
  		
  }
  // ------------------------------------------------------------
  
  KState KTracker::Predict(KState& sstart, double z_end, int imeas)
  {  

    double z_start = sstart.GetZ();
    
    // Propagate state using RK4
    KStateVec predicted_state;
    KStateVec state_in = sstart.GetPar();
    bool inField = (z_start > fBfield->FieldMinZ()) && (z_end < fBfield->FieldMaxZ());
    double ds = 0.;
    if (inField) {
      if (fVerbosity) std::cout << "Propagating from z = " << z_start << " to z = " << z_end << " with RK4" << std::endl; 
      ds = PropagateRK4(state_in, z_start, z_end, predicted_state);
    }
    else {
      ds = Propagate(state_in, z_start, z_end, predicted_state);
    }
    if (fVerbosity > 2) std::cout << "ds = " << ds << std::endl;

    // Calculate Jacobian using RK4
    KStateCov F;
    if (inField)
      F = CalculateJacobianRK4(state_in, z_start, z_end);
    else  
      F = CalculateJacobian(state_in, z_start, z_end);    

    if (fVerbosity > 2) std::cout << "F = " << F << std::endl;
      // Propagate covariance: P_pred = F * P * F^T + Q
    KStateCov F_T = ROOT::Math::Transpose(F);
    KStateCov predicted_cov = F * sstart.GetCov() * F_T;
    
    // Add process noise
    KStateCov Q;
    if (imeas > 0) {
      Q = GetNoise(predicted_state, ds, fRadLengths[imeas-1]);
    }
    else {
      Q = GetNoise(predicted_state, ds, 0.1); // no material before first measurement, put something arbitrary but small here to avoid singular covariance
    }
    if (fVerbosity > 2) std::cout << "Q = " << Q << std::endl;
    predicted_cov += Q;
    
    // Ensure covariance is symmetric
    KStateCov predicted_cov_T = ROOT::Math::Transpose(predicted_cov);
    predicted_cov = 0.5 * (predicted_cov + predicted_cov_T);
    
    KState predicted(predicted_state, predicted_cov, z_end);
    predicted.SetChi2(sstart.GetChi2());
    predicted.SetNdf(sstart.GetNdf());
    
    return predicted;
  }

  // ------------------------------------------------------------

  KState KTracker::Update(KState& predicted, KLSMeasurement& meas) {
    // Measurement model: we measure a line segment at this z position
    // H maps state to measurement space: z_meas = H * x

    if (fVerbosity) std::cout << "KTracker::Update" << "\nMeasurement: " << meas << std::endl;
    KHMatrix H = GetMeasurementJacobian(predicted, meas);
    if (fVerbosity > 2) std::cout << "H = " << H << std::endl;    
    KMeas prediction = GetMeasurementPrediction(predicted, meas);
    if (fVerbosity > 2) std::cout << "Measurement prediction = " << prediction << std::endl;
    
    // Innovation
    KMeas innovation = -1*prediction;
    if (fVerbosity) 
      std::cout << "Measurement at (" << meas.SSDStation() << ", " << meas.SSDPlane() << "), Innovation = " << innovation << std::endl;

    // Measurement covariance
    KMeasCov R;
    R[0][0] = meas.Sigma() * meas.Sigma();

    // Innovation covariance
    KHMatrixTranspose H_T = ROOT::Math::Transpose(H);
    if (fVerbosity > 2) std::cout << "H_T = " << H_T << std::endl;
    KMeasCov S = H * predicted.GetCov() * H_T + R;
    if (fVerbosity > 2) std::cout << "S = " << S << std::endl;
    if (fVerbosity > 2)    std::cout << "Innovation covariance S[0][0] = " << S[0][0] << std::endl;
    
    // Kalman gain
    int ifail;
    KMeasCov S_inv = S.Inverse(ifail);
    if (fVerbosity > 2) std::cout << "S_inv = " << S_inv << std::endl;
    auto predCov = predicted.GetCov();
    if (fVerbosity > 2) std::cout << "predicted.GetCov() = " << predCov << std::endl;
    auto K1 = predCov * H_T;
    if (fVerbosity > 2) std::cout << "predicted.GetCov() * H_T = " << K1 << std::endl;
    auto K = K1 * S_inv;
    if (fVerbosity > 2) std::cout << "Kalman Gain K = " << K << std::endl;
    // Updated state
    auto predPar = predicted.GetPar();
    if (fVerbosity > 2) std::cout << "predicted.GetPar() = " << predPar << std::endl;
    KStateVec updated_state = predPar + K * innovation;
    if (fVerbosity > 2) std::cout << "Updated state = " << updated_state << std::endl;

    // Updated covariance (Joseph form for numerical stability)
    KStateCov I = ROOT::Math::SMatrixIdentity();
    KStateCov IKH = I - K * H;
    KStateCov IKH_T = ROOT::Math::Transpose(IKH);
    auto K_T = ROOT::Math::Transpose(K);
    KStateCov updated_cov = IKH * predicted.GetCov() * IKH_T + K * R * K_T;
    // Ensure symmetry
    KStateCov updated_cov_T = ROOT::Math::Transpose(updated_cov);
    updated_cov = 0.5 * (updated_cov + updated_cov_T);

    /*
     // get updated state and its cov. matrix
    KPar newPar = state.GetPar() + K*m;
    state.SetPar(newPar);
    K5x5 newCov = (K * fH) * cov2;
    mf::LogInfo("KalmanAlg") << "KxH = " << K*fH << std::endl;
    newCov = cov2 - newCov;
    
  */
    KState updated(updated_state, updated_cov,predicted.GetZ());

    // Update chi-squared
    double chi2_increment = 0.0;
    chi2_increment = innovation[0] * S_inv[0][0] * innovation[0];
/*
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 1; j++) {
            chi2_increment += innovation[i] * S_inv[i][j] * innovation[j];
        }
    }
*/
    updated.SetChi2(predicted.GetChi2() + chi2_increment);
    updated.SetNdf(predicted.GetNdf() + 1);  // 1 measurement (distance to line)
    if (fVerbosity) {
      std::cout << "Innovation = " << innovation[0] << ", chi2 increment = " << chi2_increment << std::endl;
      std::cout << "Updated chi2 = " << updated.GetChi2() << ", ndf = " << updated.GetNdf() << std::endl;
    }    
    return updated;
}

//------------------------------------------------------------

KMeas KTracker::GetMeasurementPrediction(KState& state, KLSMeasurement& meas)
{
    // Measurement is the perpendicular distance from track to line
    // m = u*x + v*y - d

    KMeas prediction;
    prediction[0] = meas.DistanceToPoint(state.GetPar(0), state.GetPar(1));

    return prediction;
  }

  // ------------------------------------------------------------

  KHMatrix KTracker::GetMeasurementJacobian(KState& state,KLSMeasurement& meas)
  {
  // H maps state (x, y, dx/dz, dy/dz, q/p) to measurement (distance)
  // dm/dx = u
  // dm/dy = v
  // dm/d(dx/dz) = 0
  // dm/d(dy/dz) = 0
  // dm/d(q/p) = 0

  KHMatrix H;
  H[0][0] = meas.GetU();  // dm/dx
  H[0][1] = meas.GetV();  // dm/dy
  // H[0][2] = 0;    // dm/d(dx/dz) = 0
  // H[0][3] = 0;    // dm/d(dy/dz) = 0
  // H[0][4] = 0;    // dm/d(q/p) = 0

  return H;
  }

  // ------------------------------------------------------------

  bool KTracker::RunForwardFilter(int ioffset)
  {
     if (fMeasurements.empty()) {
        std::cerr << "Error: No measurements added" << std::endl;
        return false;
    }

    if (fFiltered_states.empty()) {
        std::cerr << "Error: Initial state not set" << std::endl;
        return false;
    }

    fFiltered_states.resize(fMeasurements.size()+1); // +1 for initial state

    if (!fBfield) {
        std::cerr << "Error: Magnetic field map not set" << std::endl;
        return false;
    }

    std::sort(fMeasurements.begin(), fMeasurements.end(),
              [](const KLSMeasurement& a, const KLSMeasurement& b)
              {
                  return a.GetZ() < b.GetZ();
              });

    if (fVerbosity)
      std::cout << "Running Kalman filter with " << fMeasurements.size()
                << " line segment measurements..." << std::endl;

    for (size_t i = ioffset; i < fMeasurements.size(); i++) {
        KLSMeasurement& meas = fMeasurements[i];
        KState& current = fFiltered_states[i]; //.back();

        if (fVerbosity > 1)
          std::cout << "  Processing measurement " << i+1 << "/" << fMeasurements.size()
                    << " at z = " << meas.GetZ() << " mm" << std::endl;
  
        if (fVerbosity > 2) std::cout << meas << std::endl;
        KState predicted = Predict(current, meas.GetZ(),i);
        if (fVerbosity > 2) std::cout << "Predicted = " << predicted << std::endl;
        KState updated = Update(predicted, meas);
        if (fVerbosity > 2) std::cout << "Updated = " << updated << std::endl;
        fFiltered_states[i+1] = updated;
    }

    if (fVerbosity)
      std::cout << "Kalman filter complete." << std::endl;
    return true;
  }

  // ------------------------------------------------------------

  bool KTracker::RunBackwardFilter()
  {
    if (fFiltered_states.size() < 2) {
        std::cerr << "Error: Need at least 2 filtered states to run backward filter" << std::endl;
        return false;
    }

    if (fMeasurements.empty()) {
        std::cerr << "Error: No measurements available for backward filter" << std::endl;
        return false;
    }

    if (!fBfield) {
        std::cerr << "Error: Magnetic field map not set" << std::endl;
        return false;
    }

    if (fFiltered_states.size() != fMeasurements.size() + 1) {
        std::cerr << "Error: Filtered state vector size does not match measurements" << std::endl;
        return false;
    }

    if (fVerbosity)
      std::cout << "Running Kalman backward filter..." << std::endl;

    KState current = fFiltered_states.back();
    int nmeas = static_cast<int>(fMeasurements.size());

    for (int i = nmeas - 1; i >= 0; --i) {
        KLSMeasurement& meas = fMeasurements[i];
        int imeas = (i == nmeas - 1) ? i : i + 1;

        if (fVerbosity)
          std::cout << "  Processing measurement " << i + 1 << "/" << nmeas
                    << " at z = " << meas.GetZ() << " mm" << std::endl;

        if (fVerbosity > 1) std::cout << meas << std::endl;

        KState predicted = Predict(current, meas.GetZ(), imeas);
        if (fVerbosity > 2) std::cout << "Predicted:\n" << predicted << std::endl;
        KState updated = Update(predicted, meas);
        if (fVerbosity > 2) std::cout << "Updated:\n" << updated << std::endl;

        fFiltered_states[i+1] = updated;
        current = updated;
    }

    if (fVerbosity)
      std::cout << "Kalman backward filter complete." << std::endl;
    return true;

  }

  // ------------------------------------------------------------

  KState KTracker::Smooth(KState& filtered, KState& predicted_next,
                 KState& smoothed_next, KStateCov& transport_matrix)
  {
    KStateCov F = transport_matrix;
    KStateCov F_T = ROOT::Math::Transpose(F);

    int ifail;
    KStateCov P_pred_inv = predicted_next.GetCov().Inverse(ifail);
    if (ifail != 0) {
        std::cerr << "Error inverting p2redicted covariance matrix" << std::endl;
        return filtered;
    }
    KStateCov A = filtered.GetCov() * F_T * P_pred_inv;

    KStateVec state_diff = smoothed_next.GetPar() - predicted_next.GetPar();
    KStateVec smoothed_state = filtered.GetPar() + A * state_diff;

    KStateCov A_T = ROOT::Math::Transpose(A);
    KStateCov cov_diff = smoothed_next.GetCov() - predicted_next.GetCov();
    KStateCov smoothed_cov = filtered.GetCov() + A * cov_diff * A_T;

    KStateCov smoothed_cov_T = ROOT::Math::Transpose(smoothed_cov);
    smoothed_cov = 0.5 * (smoothed_cov + smoothed_cov_T);

    KState smoothed(smoothed_state, smoothed_cov, filtered.GetZ());
    smoothed.SetChi2(filtered.GetChi2());
    smoothed.SetNdf(filtered.GetNdf());

    return smoothed;
  }
  
  // ------------------------------------------------------------

  bool KTracker::RunSmoother()
  {
    if (fFiltered_states.size() < 2) {
        std::cerr << "Error: Need at least 2 filtered states to run smoother" << std::endl;
        return false;
    }

    if (fVerbosity)
      std::cout << "Running Kalman smoother..." << std::endl;

    fSmoothed_states.clear();
    fSmoothed_states.resize(fFiltered_states.size());

    std::vector<KStateCov> transport_matrices(fFiltered_states.size() - 1);
    std::vector<KState> predicted_states(fFiltered_states.size() - 1);

    for (size_t i = 0; i < fFiltered_states.size() - 1; i++) {
        KStateVec filtered_state = fFiltered_states[i].GetPar();
        double z_start = fFiltered_states[i].GetZ();
        double z_end = fFiltered_states[i+1].GetZ();
        bool inField = (z_start > fBfield->FieldMinZ()) && (z_end < fBfield->FieldMaxZ());
        if (inField)
          transport_matrices[i] = CalculateJacobianRK4(filtered_state, z_start, z_end);
        else
          transport_matrices[i] = CalculateJacobian(filtered_state, z_start, z_end);      

        KStateVec predicted_state;
        double ds = 0;
        if (inField)
          ds = PropagateRK4(filtered_state, z_start, z_end, predicted_state);
        else  
          ds = Propagate(filtered_state, z_start, z_end, predicted_state);

        KStateCov F = transport_matrices[i];
        KStateCov F_T = ROOT::Math::Transpose(F);
        KStateCov predicted_cov = F * fFiltered_states[i].GetCov() * F_T;

        KStateCov Q;
        if (i > 0) {
          Q = GetNoise(predicted_state, ds, fRadLengths[i-1]);
        }
        else {
          Q = GetNoise(predicted_state, ds, 0.1); // no material before first measurement
        }

        predicted_cov += Q;

        predicted_states[i] = KState(predicted_state, predicted_cov, fFiltered_states[i+1].GetZ());
    }

    fSmoothed_states.back() = fFiltered_states.back();

    for (int i = fFiltered_states.size() - 2; i >= 0; i--) {
        fSmoothed_states[i] = Smooth(fFiltered_states[i],
                                     predicted_states[i],
                                     fSmoothed_states[i+1],
                                     transport_matrices[i]);
    }

    if (fVerbosity)
      std::cout << "Kalman smoother complete." << std::endl;
    return true;
}

  // ============================================================
  // Calculate chi2 from smoothed states (independent verification)
  // Loops through all measurements and computes residuals using
  // the final smoothed states, then sums chi2 = residual^2/sigma^2
  // ============================================================
  std::pair<double, int> KTracker::CalculateChi2FromSmoothedStates(std::vector<KResidual>& residuals)
  {
    double chi2_total = 0.0;
    int ndof = 0;

    // Check that we have smoothed states
    if (fSmoothed_states.empty() || fMeasurements.empty()) {
      if (fVerbosity > 0) {
        std::cout << "CalculateChi2FromSmoothedStates: No smoothed states or measurements" << std::endl;
      }
      return std::make_pair(0.0, 0);
    }

    // Sort measurements by z (same as in RunForwardFilter)
    std::vector<size_t> meas_indices(fMeasurements.size());
    for (size_t i = 0; i < fMeasurements.size(); i++) meas_indices[i] = i;
    std::sort(meas_indices.begin(), meas_indices.end(),
              [this](size_t a, size_t b) {
                  return fMeasurements[a].GetZ() < fMeasurements[b].GetZ();
              });

    // For each measurement, calculate residual using smoothed state
    for (size_t i = 0; i < fMeasurements.size(); i++) {
      size_t meas_idx = meas_indices[i];
      KLSMeasurement& meas = fMeasurements[meas_idx];

      // Get the smoothed state at this measurement (fSmoothed_states[i+1])
      // fSmoothed_states[0] is the initial state, fSmoothed_states[i+1] is after measurement i
      if (i + 1 >= fSmoothed_states.size()) {
        if (fVerbosity > 0) {
          std::cout << "CalculateChi2FromSmoothedStates: Warning, not enough smoothed states" << std::endl;
        }
        break;
      }

      KState& smoothed_state = fSmoothed_states[i + 1];

      // Calculate measurement prediction (residual = distance from state to measurement)
      KMeas prediction = GetMeasurementPrediction(smoothed_state, meas);
      double residual = prediction[0];
      // Get measurement sigma
      double sigma = meas.Sigma();
      double variance = sigma * sigma;
      KResidual res(residual, sigma, meas.SSDStation(), meas.SSDPlane());
      residuals.push_back(res);

      // Add to chi2
      if (variance > 1e-10) {
        double chi2_contrib = (residual * residual) / variance;
        chi2_total += chi2_contrib;
        ndof++;

        if (fVerbosity > 100) {
          std::cout << "Measurement " << (i+1) << ": residual = " << residual
                    << " mm, sigma = " << sigma << " mm, chi2_contrib = " << chi2_contrib << std::endl;
        }
      }
    }

    if (fVerbosity > 0) {
      std::cout << "Chi2 calculated from smoothed states: " << chi2_total
                << " / " << ndof << " = " << (ndof > 0 ? chi2_total/ndof : 0.0) << std::endl;
    }

    return std::make_pair(chi2_total, ndof);
  }


} // end namespace kalman
