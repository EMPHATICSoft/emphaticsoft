#include <iostream> 
#include <cmath> 
#include <algorithm> 

#include "KalmanReco/KalmanAlg.h"
#include "KalmanReco/Propagator.h"

namespace kalman {

  KalmanAlg::KalmanAlg() : fGeo(NULL), fBfield(NULL)
  {
    fVerbosity=0;
    for (int i=0; i<5; ++i)
      for (int j=0; j<5; ++j)
	fF[i][j] = 0.;
    fF[0][0] = 1;
    fF[1][1] = 1;
    fF[2][2] = 1.;
    fF[3][3] = 1.;
    for (int i=0; i<5; ++i) {
      fH[0][i] = fHT[i][0] = 0.;
      fH[1][i] = fHT[i][1] = 0.;
    }
  }

  //-----------------------------------------------------------------

  KalmanAlg::KalmanAlg(const emph::geo::Geometry* g,
		       const emph::MagneticField* b) : fGeo(g), fBfield(b)
  {
    fVerbosity=0;    
    for (int i=0; i<5; ++i)
      for (int j=0; j<5; ++j)
	fF[i][j] = 0.;
    fF[0][0] = 1;
    fF[1][1] = 1;
    fF[2][2] = 1.;
    fF[3][3] = 1.;
    for (int i=0; i<5; ++i) {
      fH[0][i] = fHT[i][0] = 0.;
      fH[1][i] = fHT[i][1] = 0.;
    }
  }

  //-----------------------------------------------------------------
  void KalmanAlg::LoopOverMeasurements(std::vector<rb::LineSegment>& lsv, 
				       State& state1, int dir) 
  {
    if (dir == 0) {
      std::cout << "LoopOverMeasurements called with no direction.  Must be non-zero" << std::endl;
      return;
    }

    kalman::Measurement meas;
    KMeas m;
    KMeasCov mCov;
    kalman::Propagator prop;
    double B[3] = {0.,0.,0.};
    double gamma;
    double dx, dy, dz;
    
    // loop over linesegments 
    int idx = 0;
    if (dir<0) idx = (int)lsv.size()-2;
    bool atEnd = false;
    while (!atEnd) {      
      auto ls = lsv[idx];
      if (fVerbosity) {
	std::cout << "Current state: " << std::endl;
	std::cout << state1 << std::endl;
      }

      // get Predicted state
      K5x5 parCov = state1.GetCov();
      double avgz = (ls.X0().Z()+ls.X1().Z())/2.;
      dz = avgz - state1.GetZ();
      // to-do:
      // 1. determine if particle is in the magnetic field
      // 2. calculate the amount of material the particle traverse to the next position

      //      if (B[0] == 0 && B[1] == 0 && B[2] == 0) {
      fF[0][2] = dz;
      fF[1][3] = dz;
      KPar st2 = fF * state1.GetPar();
      // to add in energy loss, need to add B * u to st2
      K5x5 cov2 = fF * (parCov * ROOT::Math::Transpose(fF));
      // to add in multiple scattering, need to add Q to cov2
      kalman::State state2(avgz,st2,cov2);	
      
      if (fVerbosity) {
	std::cout << "Predicted state: ";
	std::cout << state2 << std::endl;
      }

      // now get measurement
      dx = ls.X1().X() - ls.X0().X();
      dy = ls.X1().Y() - ls.X0().Y();
      //      dz = ls.X1().Z() - ls.X0().Z();
      
      //      alpha = atan2(dy,dz); // rotation about the x-axis
      //      beta = atan2(dx,dz); // rotation about the y-axis
      gamma = atan2(dy,dx); // rotation about the z-axis
      double cos_g = cos(gamma);
      double sin_g = sin(gamma);
      std::cout << "cos(gamma) = " << cos_g << ", sin(gamma) = " << sin_g << std::endl;

      // for now, deal only with rotations about the z-axis, as this is the biggest effect
      fH[0][0] = cos_g;
      fH[0][1] = sin_g;
      fHT = ROOT::Math::Transpose(fH);

      mCov[0][0] = ls.Sigma()*ls.Sigma();

      // compute Kalman gain
      KMeasCov S = fH*(cov2*fHT) + mCov;;      
      int ifail;
      KMeasCov SInv = S.Inverse(ifail);
      std::cout << "S^-1 = " << SInv << std::endl;
      KHMatrixTranspose K = fHT*SInv;
      
      K = cov2*K;      

      if (fVerbosity) {
	std::cout << "K = " << K << std::endl;
      }
      // compute "measurement" (distance of projected state to the measured line segment)
      double d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));
      m[0] = d;//*cos_g;
      //      m[1] = d*sin_g;

      std::cout << "m = " << m << std::endl;//(" << m[0] << "\t" << m[1] << ")" << std::endl;

      if (fVerbosity) {
	KPar Km = K*m;
	std::cout << "K x m = " << Km << std::endl;
      }

      // get updated state and its cov. matrix
      KPar newPar = state2.GetPar() + K*m;
      state1.SetPar(newPar);
      K5x5 newCov = K * (fH * cov2);
      newCov = parCov - newCov;
      state1.SetCov(newCov);
      state1.SetZ(avgz);
      if (fVerbosity) {
	std::cout << "Updated state: ";
	std::cout << state1 << std::endl;
      }

      if (dir>0) 
	if (++idx == (int)lsv.size()) atEnd = true;
      if (dir<0)
	if (--idx == -1) atEnd = true;
    }

  }

  //-----------------------------------------------------------------

  rb::Track KalmanAlg::Fit(std::vector<rb::LineSegment>& linesegv, 
			   State& state1) 
  {

    rb::Track track;

    if (fGeo == NULL) {
      std::cerr << "AHHH!  NULL pointer to Geometry!  You forgot to set it..." << std::endl;
      abort();
    }
    if (fBfield == NULL) {
      std::cerr << "WARNING: NULL pointer to MagneticField!  Continuing assuming no magnet..." << std::endl;
    }

    // first make sure that the line segments are sorted in "z"
    bool isSorted = true;
    for (size_t i=0; i<linesegv.size()-1; ++i) {
      if (linesegv[i+1].X0().Z() < linesegv[i].X0().Z()) {
	isSorted = false;
	std::cout << "linesegments not sorted!" << std::endl;
	break;
      }
    }
    
    if (!isSorted) {
      std::cerr << "ERROR: linesegments are not ordered in z!" << std::endl;
      abort(); // add a sort function later 
    }

    LoopOverMeasurements(linesegv,state1,1);
    std::cout << "%%%%% GOING BACKWARD NOW %%%%%" << std::endl;

    LoopOverMeasurements(linesegv,state1,-1);
      
    return track;
  }

} // end namespace kalman
