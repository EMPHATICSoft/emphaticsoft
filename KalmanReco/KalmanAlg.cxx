#include <iostream> 
#include <cmath> 
#include <algorithm> 

#include "KalmanReco/KalmanAlg.h"

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
    fF[4][4] = 1.;
    for (int i=0; i<5; ++i) {
      fH[0][i] = fHT[i][0] = 0.;
      fH2[0][i] = fH2T[i][0] = 0.;
      fH2[1][i] = fH2T[i][1] = 0.;
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
    fF[4][4] = 1.;
    for (int i=0; i<5; ++i) {
      fH[0][i] = fHT[i][0] = 0.;
      fH2[0][i] = fH2T[i][0] = 0.;
      fH2[1][i] = fH2T[i][1] = 0.;
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

    kalman::Measurement meas;
    KMeas m;
    KMeas2 m2;
    KMeasCov mCov;
    KMeasCov2 mCov2;
    double B[3] = {0.,0.,0.};
    double gamma;
    double dx, dy, dz;
    double chi2=0.;
    int dof = 0;
    
    // loop over linesegments in the forward direction (Kalman Filtering)
    bool isFirst = true;
    std::vector<KPar> apriorPar;
    std::vector<KPar> apostPar;    
    std::vector<K5x5> apriorCov;
    std::vector<K5x5> apostCov;    
    std::vector<double> zPos;

    if (fVerbosity) {
      for (size_t idx = 0; idx<linesegv.size(); ++idx) {
	auto & ls = linesegv[idx];
	std::cout << "linesegment at z = " << ls.X0().Z() << std::endl;
      }
    }

    for (size_t idx = 0; idx<linesegv.size(); ++idx) {
      auto & ls = linesegv[idx];

      if (fVerbosity) {
	std::cout << "Current state: " << std::endl;
	std::cout << std::setprecision(4) << std::scientific;
	std::cout << state1 << std::endl;
	std::cout << std::defaultfloat; 
      }
      
      // get Predicted state
      K5x5 parCov = state1.GetCov();
      double avgz = (ls.X0().Z()+ls.X1().Z())/2.;
      dz = avgz - state1.GetZ();
      // to-do:
      // 1. determine if particle is in the magnetic field

      //      if (B[0] == 0 && B[1] == 0 && B[2] == 0) {
      fF[0][2] = dz;
      fF[1][3] = dz;
      KPar st2 = fF * state1.GetPar();

      // to add in energy loss, need to add B * u to st2

      K5x5 cov2 = parCov * ROOT::Math::Transpose(fF);
      cov2 = fF * cov2;

      zPos.push_back(avgz);
      apriorPar.push_back(st2);
      apriorCov.push_back(cov2);
      kalman::State state2(avgz,st2,cov2);	
      
      // to add in noise to multiple scattering, need to add Q to cov2
      double radLength=0.;      
      if (idx>0 && idx < linesegv.size()-1 ) {
	if (fVerbosity)
	  std::cout << "Adding multiple scattering noise at SSD station " << ls.SSDStation() << ", Plane " << ls.SSDPlane() << std::endl;
	int ssdId;
	ssdId = ls.SSDStation()*10 + ls.SSDPlane();
	radLength = fGeo->GetRadLength(ssdId);
      }	
      if (radLength>0.) {
	fProp.AddNoise(state2,fabs(dz),radLength);
	cov2 = state2.GetCov();
      }

      if (fVerbosity) {
	std::cout << std::setprecision(4) << std::scientific;
	std::cout << "Predicted state: ";
	std::cout << state2 << std::endl;
	std::cout << std::defaultfloat; 
      }
      
      // now get measurement
      dx = ls.X1().X() - ls.X0().X();
      dy = ls.X1().Y() - ls.X0().Y();
      //      dz = ls.X1().Z() - ls.X0().Z();
      
      //      alpha = atan2(dy,dz); // rotation about the x-axis
      //      beta = atan2(dx,dz); // rotation about the y-axis
      // for now, deal only with rotations about the z-axis, as this is the biggest effect
      gamma = atan2(dy,dx); // rotation about the z-axis
      double cos_g = cos(gamma);
      double sin_g = sin(gamma);
      if (fVerbosity) {
	std::cout << std::setprecision(8);
	std::cout << "cos(gamma) = " << cos_g << ", sin(gamma) = " << sin_g << std::endl;
	std::cout << std::defaultfloat;
      }

      bool is1D = false;
      if (fabs(cos_g) < 1.e-6) {
	cos_g = 0.;
	sin_g = 1.;
	is1D = true;
      } 
      if (fabs(sin_g) < 1.e-6) {
	sin_g = 0.;
	cos_g = 1.;
	is1D = true;
      }

      if (is1D) {
	std::cout << "is1D" << std::endl;
	fH[0][0] = cos_g;
	fH[0][1] = sin_g;
	fHT = ROOT::Math::Transpose(fH);
	// compute "measurement" (distance of projected state to the measured line segment)
	double d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));
	m[0] = d;//*cos_g;
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
	
	std::cout << "m = " << m << std::endl;
	std::cout << "mCov = " << mCov << std::endl;
	
	if (fVerbosity) {
	  KPar Km = K*m;
	  std::cout << "K x m = " << Km << std::endl;
	}
	
	// get updated state and its cov. matrix
	KPar newPar = state2.GetPar() + K*m;
	state1.SetPar(newPar);
	K5x5 newCov = (K * fH) * cov2;
	std::cout << "KxH = " << K*fH << std::endl;
	newCov = cov2 - newCov;

	state1.SetCov(newCov);
	state1.SetZ(avgz);
	apostPar.push_back(newPar);
	apostCov.push_back(newCov);
	if (!isFirst) {
	  d = ls.DistanceToPoint(state1.GetPar(0),state1.GetPar(1));
	  m[0] = d;
	  chi2 += ROOT::Math::Similarity(m,SInv);
	  dof++;
	}
      } // end if is1D
      else { // 2D
	fH2[0][0] = cos_g;
	fH2[1][1] = sin_g;
	fH2T = ROOT::Math::Transpose(fH2);
	mCov2[0][0] = cos_g*cos_g*ls.Sigma()*ls.Sigma();
	mCov2[1][1] = sin_g*sin_g*ls.Sigma()*ls.Sigma();
	
	// compute Kalman gain
	KMeasCov2 S = fH2*(cov2*fH2T) + mCov2;      
	int ifail;
	KMeasCov2 SInv = S.Inverse(ifail);
	std::cout << "S^-1 = " << SInv << std::endl;
	KHMatrixTranspose2 K = fH2T*SInv;

	if (fVerbosity) {
	  std::cout << "fH2T x S^-1 = " << K << std::endl;
	}
	
	K = cov2*K;      
	
	if (fVerbosity) {
	  std::cout << "K = " << K << std::endl;
	}
	// compute "measurement" (distance of projected state to the measured line segment)
	double d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));
	m2[0] = d*cos_g;
	m2[1] = d*sin_g;
		
	std::cout << "m = " << m2 << std::endl;
	
	if (fVerbosity) {
	  KPar Km = K*m2;
	  std::cout << "K x m = " << Km << std::endl;
	}
	
	// get updated state and its cov. matrix
	KPar newPar = state2.GetPar() + K*m2;
	state1.SetPar(newPar);
	K5x5 newCov = K * (fH2 * cov2);
	std::cout << "KxH = " << K*fH2 << std::endl;
	newCov = cov2 - newCov;
	state1.SetCov(newCov);
	state1.SetZ(avgz);
	apostPar.push_back(newPar);
	apostCov.push_back(newCov);
	//	if (fVerbosity) {
	//	  std::cout << "Updated state: ";
	//	  std::cout << state1 << std::endl;
	//	}
	if (!isFirst) {
	  d = ls.DistanceToPoint(state1.GetPar(0),state1.GetPar(1));
	  m2[0] = d*cos_g;
	  m2[1] = d*sin_g;
	  chi2 += ROOT::Math::Similarity(m2,SInv);
	  dof++;
	}
      }
      
      if (fVerbosity) {
	std::cout << "Updated state: ";
	std::cout << std::setprecision(4) << std::scientific;
	std::cout << state1 << std::endl;
	std::cout << std::defaultfloat; 
      }
      
      if (fVerbosity) {
	if (!isFirst) {
	  std::cout << "chi^2 / dof = " << chi2/float(dof) << std::endl;
	}
      }
    }

    std::cout << "Now going backwards for smoothing..." << std::endl;
    int ifail;

    // now loop backwards and update states (Kalman Smoothing)
    for (int idx = (int)linesegv.size()-2; idx>0; --idx) {

      auto & ls = linesegv[idx];
      if (fVerbosity) {
	std::cout << "At z = " << ls.X0().Z() << " (" << zPos[idx] 
		  << ")" << std::endl;
	std::cout << "Previous state: " << apostPar[idx] << "\n" 
		  << apostCov[idx] << std::endl;
      }

      dz = linesegv[idx+1].X0().Z()-ls.X0().Z();
      fF[0][2] = dz;
      fF[1][3] = dz;
      K5x5 C = apostCov[idx] * (ROOT::Math::Transpose(fF) * apriorCov[idx+1].Inverse(ifail));

      apostPar[idx] = apostPar[idx] + C * (apostPar[idx+1] - apriorPar[idx+1]);
      apostCov[idx] = apostCov[idx] + C * (apostCov[idx+1] - apriorCov[idx+1]) * ROOT::Math::Transpose(C);

      if (fVerbosity) {
	std::cout << std::setprecision(4) << std::scientific;

	std::cout << "Smoothed state: " << apostPar[idx] << "\n" << apostCov[idx] << std::endl;
	std::cout << "d = " << ls.DistanceToPoint(apostPar[idx][0],
						  apostPar[idx][1]) 
		  << std::endl;
	std::cout << "-----------------" << std::endl;
	std::cout << std::defaultfloat; 
      }
    }
      
    return track;
  }

} // end namespace kalman
