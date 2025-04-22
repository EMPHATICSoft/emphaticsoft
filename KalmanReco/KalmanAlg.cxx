#include <iostream> 
#include <cmath> 
#include <algorithm> 

#include "KalmanAlg.h"
#include "KMeasurement.h"
#include "KState.h"
#include "Propagator.h"

namespace kalman {

  KalmanAlg::KalmanAlg() : fGeo(NULL), fBfield(NULL)
  {
    fVerbosity=0;
  }

  //-----------------------------------------------------------------

  KalmanAlg::KalmanAlg(const emph::geo::Geometry* g,
		       const emph::MagneticField* b) : fGeo(g), fBfield(b)
  {
    fVerbosity=0;    
  }

  //-----------------------------------------------------------------

  rb::Track KalmanAlg::Fit(std::vector<rb::LineSegment>& linesegv) 
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
	break;
      }
    }

    if (!isSorted) {
      std::cerr << "ERROR: linesegments are not ordered in z!" << std::endl;
      abort(); // add a sort function later 
    }

    // loop over linesegments 

    KPar par;
    K5x5 parCov;
    par[0] = 0.;
    par[1] = 0.;
    par[2] = 0.;
    par[3] = 0.;
    par[4] = 0.2; // 5 GeV/c
    parCov[0][0] = 100.;
    parCov[1][1] = 100.;
    parCov[2][2] = 1.;
    parCov[3][3] = 1.;
    parCov[4][4] = 10000.;
    kalman::State state1(-1000.,par,parCov);

    kalman::Measurement meas;
    KMeas m;
    KMeasCov mCov;
    KHMatrix mH;
    KHMatrixTranspose mHT;
    //    typedef ROOT::Math::SVector<double,1> KMeas;
    //    typedef ROOT::Math::SMatrix<double,1,1> KMeasCov;
    //    typedef ROOT::Math::SMatrix<double,1,5> KHMatrix;
    kalman::Propagator prop;
    //    State Extrapolate(State sstart, double step, double B[3]);
    double B[3] = {0.,0.,0.};
    // double alpha, beta;
    double gamma;
    double dx, dy, dz;

    for (auto & ls : linesegv) {

      if (fVerbosity)
	state1.Print();

      double avgz = (ls.X0().Z()+ls.X1().Z())/2.;
      
      // to-do:
      // 1. determine if particle is in the magnetic field
      // 2. calculate the amount of material the particle traverse to the next position

      kalman::State state2 = prop.Extrapolate(state1,avgz-state1.GetZ(),B);

      if (fVerbosity) {
	std::cout << "Extrapolated to: ";
	state2.Print(); 
	std::cout << std::endl;
      }

      dx = ls.X1().X() - ls.X0().X();
      dy = ls.X1().Y() - ls.X0().Y();
      //      dz = ls.X1().Z() - ls.X0().Z();
      
      //      alpha = atan2(dy,dz); // rotation about the x-axis
      //      beta = atan2(dx,dz); // rotation about the y-axis
      gamma = atan2(dy,dx); // rotation about the z-axis
      double cos_g = cos(gamma);
      double sin_g = sin(gamma);

      // for now, deal only with rotations about the z-axis, as this is the biggest effect
      mH[0][0] = cos_g;
      mH[0][1] = sin_g;
      mH[0][2] = 0;
      mH[0][3] = 0;
      mH[0][4] = 0;
      mHT = ROOT::Math::Transpose(mH);
      
      mCov[0][0] = ls.Sigma();
      /*      mHT[0][0] = mH[0][0];
      mHT[1][0] = mH[0][1];
      mHT[2][0] = mH[0][2];
      mHT[3][0] = mH[0][3];
      mHT[4][0] = mH[0][4];
      */

      // compute Kalman gain
      KMeasCov tCov = mH*(state2.GetCov()*mHT) + mCov;;      
      int ifail;
      KMeasCov tCovInv = tCov.Inverse(ifail);
      KHMatrixTranspose KG = mHT*tCovInv;
      KG = state2.GetCov()*KG;

      // compute "measurement" (distance of projected state to the measured line segment)
      m[0] = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));

      // get updated state and its cov. matrix
      KPar newPar = state2.GetPar() + KG*m;
      state1.SetPar(newPar);
      K5x5 newCov = KG * (mH * parCov);
      newCov = parCov - newCov;
      state1.SetCov(newCov);

      if (fVerbosity) {
	std::cout << "Updated state to: ";
	state1.Print(); 
	std::cout << std::endl;
      }
      
    }

    // now go in reverse

    for ( auto it = linesegv.rbegin(); it != linesegv.rend(); --it) {
      auto& ls = *it;
      double avgz = (ls.X0().Z()+ls.X1().Z())/2.;
      
      // to-do:
      // 1. determine if particle is in the magnetic field
      // 2. calculate the amount of material the particle traverse to the next position

      kalman::State state2 = prop.Extrapolate(state1,avgz-state1.GetZ(),B);

      dx = ls.X1().X() - ls.X0().X();
      dy = ls.X1().Y() - ls.X0().Y();
      //      dz = ls.X1().Z() - ls.X0().Z();
      
      //      alpha = atan2(dy,dz); // rotation about the x-axis
      //      beta = atan2(dx,dz); // rotation about the y-axis
      gamma = atan2(dy,dx); // rotation about the z-axis
      double cos_g = cos(gamma);
      double sin_g = sin(gamma);

      // for now, deal only with rotations about the z-axis, as this is the biggest effect
      mH[0][0] = cos_g;
      mH[0][1] = sin_g;
      mH[0][2] = 0;
      mH[0][3] = 0;
      mH[0][4] = 0;
      mHT = ROOT::Math::Transpose(mH);
      
      mCov[0][0] = ls.Sigma();
      /*      mHT[0][0] = mH[0][0];
      mHT[1][0] = mH[0][1];
      mHT[2][0] = mH[0][2];
      mHT[3][0] = mH[0][3];
      mHT[4][0] = mH[0][4];
      */

      // compute Kalman gain
      KMeasCov tCov = mH*(state2.GetCov()*mHT) + mCov;;      
      int ifail;
      KMeasCov tCovInv = tCov.Inverse(ifail);
      KHMatrixTranspose KG = mHT*tCovInv;
      KG = state2.GetCov()*KG;

      // compute "measurement" (distance of projected state to the measured line segment)
      m[0] = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));

      // get updated state and its cov. matrix
      KPar newPar = state2.GetPar() + KG*m;
      state1.SetPar(newPar);
      K5x5 newCov = KG * (mH * parCov);
      newCov = parCov - newCov;
      state1.SetCov(newCov);
      
    }

    return track;
  }

} // end namespace kalman
