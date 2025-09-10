#include <iostream> 
#include <cmath> 
#include <algorithm> 

#include "KalmanReco/KalmanAlg.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

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
      fH2D[0][i] = fH2DT[i][0] = 0.;
      fH2D[1][i] = fH2DT[i][1] = 0.;
    }
    fProp.SetVerbosity(0);
    fFieldZmin = 0.;
    fFieldZmax = 0.;
  }

  //-----------------------------------------------------------------

  KalmanAlg::KalmanAlg(emph::geo::Geometry* g,
		       emph::MagneticField* b) : fGeo(g), fBfield(b)
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
      fH2D[0][i] = fH2DT[i][0] = 0.;
      fH2D[1][i] = fH2DT[i][1] = 0.;
    }
    fProp.SetVerbosity(0);
    fFieldZmin = fBfield->FieldMinZ();
    fFieldZmax = fBfield->FieldMaxZ();

    mf::LogInfo("KalmanAlg") << "%%%%% (min,maz) z position of B-field: (" << fFieldZmin << "," << fFieldZmax << ")";
  }

  //-----------------------------------------------------------------

  void KalmanAlg::SimpleExtrap(State& state1, State& state2, double dz)
  {
    KPar par2;
    K5x5 cov2;
    K5x5 parCov = state1.GetCov();

    fF[0][2] = dz;
    fF[1][3] = dz;
    par2 = fF * state1.GetPar();
    cov2 = parCov * ROOT::Math::Transpose(fF);
    cov2 = fF * cov2;
    state2.SetPar(par2);
    state2.SetCov(cov2);
    state2.SetZ(state1.GetZ()+dz);
  }

  //-----------------------------------------------------------------

  double KalmanAlg::ExtrapInField(State& state1, State& state2, double totL)
  {
    double ds = 0;
    if (fVerbosity)
      mf::LogInfo("KalmanAlg") << "%%%%% Passing through magnetic field %%%%%"; 
    // extrapolate through magnetic field
    int nsteps = int(fabs(totL));
    double dz = totL/(double)nsteps;
    double tpos[3];
    state2 = state1;

    double B[3];

    for (int iB=0; iB<nsteps; ++iB) {
      tpos[0] = state2.GetPar(0);
      tpos[1] = state2.GetPar(1);
      tpos[2] = state2.GetZ();
      fBfield->GetFieldValue(tpos,B);
      if (totL > 0.) {
	for (int k=0; k<3; ++k) // convert to kG
	  B[k] *= 1.e3;
      }
      else { // flip the sign of the field if we are going backward
	for (int k=0; k<3; ++k) // convert to kG
	  B[k] *= -1.e3;
      }
      if (fVerbosity)
	mf::LogInfo("KalmanAlg") << "B(" << tpos[0] << "," << tpos[1] 
				 << "," << tpos[2] 
				 << ") = (" << B[0] << "," << B[1] << "," 
				 << B[2] << ")";
      if (dz < 0)
	for (int k=0; k<3; ++k) // going backward, flip the sign of the field
	  B[k] *= -1.;
      
      ds += fProp.Extrapolate(state2,dz,B);
    }
    if (fVerbosity)
      mf::LogInfo("KalmanAlg") << "State after bend magnet:\n" 
			       << state1 << std::endl;
    return ds;
  }

  //-----------------------------------------------------------------

  void KalmanAlg::Update1D(State& state, double d, double sigma) 
  {
    
    fH[0][0] = fCosG;
    fH[0][1] = fSinG;
    fHT = ROOT::Math::Transpose(fH);
    KMeas m;
    KMeasCov mCov;
    m[0] = d;
    mCov[0][0] = sigma*sigma;
    K5x5 cov2 = state.GetCov();

    // compute Kalman gain
    KMeasCov S = fH*(cov2*fHT) + mCov;;      
    int ifail;
    KMeasCov SInv = S.Inverse(ifail);
    if (fVerbosity)
      mf::LogInfo("KalmanAlg") << "S^-1 = " << SInv;
    KHMatrixTranspose K = fHT*SInv;
    
    K = cov2*K;      
    
    if (fVerbosity) {
      mf::LogInfo("KalmanAlg") << "K = " << K << std::endl;
    }
    
    mf::LogInfo("KalmanAlg") << "m = " << m << std::endl;
    mf::LogInfo("KalmanAlg") << "mCov = " << mCov << std::endl;
    
    if (fVerbosity) {
      KPar Km = K*m;
      mf::LogInfo("KalmanAlg") << "K x m = " << Km << std::endl;
    }
    
    // get updated state and its cov. matrix
    KPar newPar = state.GetPar() + K*m;
    state.SetPar(newPar);
    K5x5 newCov = (K * fH) * cov2;
    mf::LogInfo("KalmanAlg") << "KxH = " << K*fH << std::endl;
    newCov = cov2 - newCov;
    
    state.SetCov(newCov);
    fApostState.push_back(state);
  }

  //-----------------------------------------------------------------

  void KalmanAlg::Update2D(State& state, double d, double sigma) 
  {
    fH2D[0][0] = 1; //fCosG;
    fH2D[1][1] = 1; //fSinG;
    fH2DT = ROOT::Math::Transpose(fH2D);
    KMeas2D m2D;
    KMeasCov2D mCov2D;
    m2D[0] = d*fCosG;
    m2D[1] = d*fSinG;
    mCov2D[0][0] = fCosG*fCosG*sigma*sigma;
    mCov2D[1][1] = fSinG*fSinG*sigma*sigma;

    if (fVerbosity)
      mf::LogInfo("KalmanAlg") << "m = " << m2D << std::endl;
        
    // compute Kalman gain
    K5x5 cov2 = state.GetCov();
    KMeasCov2D S = fH2D*(cov2*fH2DT) + mCov2D;      
    int ifail;
    KMeasCov2D SInv = S.Inverse(ifail);
    mf::LogInfo("KalmanAlg") << "S^-1 = " << SInv << std::endl;
    KHMatrixTranspose2D K = fH2DT*SInv;
    
    if (fVerbosity) 
      mf::LogInfo("KalmanAlg") << "fH2DT x S^-1 = " << K << std::endl;
    
    K = cov2*K;      
    
    if (fVerbosity) {
      mf::LogInfo("KalmanAlg") << "K = " << K << std::endl;
    }
    
    if (fVerbosity) {
      KPar Km = K*m2D;
      mf::LogInfo("KalmanAlg") << "K x m = " << Km << std::endl;
    }
	
    // get updated state and its cov. matrix
    KPar newPar = state.GetPar() + K*m2D;
    state.SetPar(newPar);
    K5x5 newCov = K * (fH2D * cov2);
    mf::LogInfo("KalmanAlg") << "KxH = " << K*fH2D << std::endl;
    newCov = cov2 - newCov;
    state.SetCov(newCov);

  }

  //-----------------------------------------------------------------

  void KalmanAlg::Smooth(State& ApostState1, State& ApostState2, 
			 State& ApriorState2, double dz)
  {
    fF[0][2] = dz;
    fF[1][3] = dz;

    K5x5 apostCov = ApostState1.GetCov();
    KPar apostPar = ApostState1.GetPar();
    K5x5 apostCov1 = ApostState2.GetCov();
    KPar apostPar1 = ApostState2.GetPar();
    K5x5 apriorCov1 = ApriorState2.GetCov();
    KPar apriorPar1 = ApriorState2.GetPar();
    int ifail;

    K5x5 C = apostCov * (ROOT::Math::Transpose(fF) * apriorCov1.Inverse(ifail));
    
    apostPar = apostPar + C * (apostPar1 - apriorPar1);
    apostCov = apostCov + C * (apostCov1 - apriorCov1) * ROOT::Math::Transpose(C);
    ApostState1.SetPar(apostPar);
    ApostState1.SetCov(apostCov);
  }
  
  //-----------------------------------------------------------------

  rb::Track KalmanAlg::Fit(std::vector<rb::LineSegment>& linesegv, 
			   State& state1) 
  {
    rb::Track track;

    fApriorState.clear();
    fApostState.clear();

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
	mf::LogInfo("KalmanAlg") << "linesegments not sorted!" << std::endl;
	break;
      }
    }
    
    if (!isSorted) {
      std::cerr << "ERROR: linesegments are not ordered in z!" << std::endl;
      abort(); // add a sort function later 
    }

    kalman::Measurement meas;
    double B[3] = {0.,0.,0.};
    double gamma;
    double dx, dy, dz;
    double chi2=0.;
    int dof = 0;
    
    // loop over linesegments in the forward direction (Kalman Filtering)

    if (fVerbosity) {
      for (size_t idx = 0; idx<linesegv.size(); ++idx) {

	auto & ls = linesegv[idx];
	mf::LogInfo("KalmanAlg") << "linesegment at z = " << ls.X0().Z() << std::endl;
      }
    }

    for (int ichi2=0; ichi2<3; ++ichi2) {

      for (size_t idx = 0; idx<linesegv.size(); ++idx) {
	auto & ls = linesegv[idx];
	
	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << "Current state: " << "\n"
				   << std::setprecision(4) << std::scientific
				   << state1 << std::defaultfloat; 
	}
      
	// get Predicted state
	double nextz = ls.X0().Z();
	dz = nextz - state1.GetZ();
	bool InsideField = fBfield&&((state1.GetZ() >= fFieldZmin) && (nextz < fFieldZmax));
	bool EnteringField = fBfield&&((state1.GetZ() < fFieldZmin) && (nextz > fFieldZmin));
	bool ExitingField = fBfield&&((state1.GetZ() < fFieldZmax) && (nextz > fFieldZmax));
	double ds = 0.;
      
	// get radLength for multiple scattering noise calculation
	double radLength=0.;      
	if (idx>0 && idx < linesegv.size()-1 ) {
	  if (fVerbosity)
	    mf::LogInfo("KalmanAlg") << "Adding multiple scattering noise at SSD station " << ls.SSDStation() << ", Plane " << ls.SSDPlane() << std::endl;
	  int ssdId;
	  ssdId = ls.SSDStation()*10 + ls.SSDPlane();
	  radLength = fGeo->GetRadLength(ssdId);
	}	

	// check to see if the next measurement is in the field
	kalman::State state2;      
	if (InsideField) {
	  mf::LogInfo("KalmanAlg") << "%%%%% INSIDE FIELD %%%%%" << std::endl;
	  dz = nextz - state1.GetZ();
	  ds = ExtrapInField(state1, state2, dz);
	}
	else if (EnteringField) {
	  mf::LogInfo("KalmanAlg") << "%%%%% ENTERING FIELD %%%%%" << std::endl;
	  dz = fFieldZmin - state1.GetZ();
	  ds = dz;
	  SimpleExtrap(state1, state2, dz);	
	  state1 = state2;
	  dz = nextz - fFieldZmin;
	  ds += ExtrapInField(state1, state2, dz);
	}
	else if (ExitingField) {
	  mf::LogInfo("KalmanAlg") << "%%%%% EXITING FIELD %%%%%" << std::endl;
	  dz = fFieldZmax - state1.GetZ();
	  ds = ExtrapInField(state1, state2, dz);
	  state1 = state2;
	  dz = nextz - fFieldZmax;
	  SimpleExtrap(state1, state2, dz);
	  ds += dz;
	}
	else { // both current state and next meas. are outside of field region
	  mf::LogInfo("KalmanAlg") << "%%%%% OUTSIDE FIELD %%%%%" << std::endl;
	  dz = nextz - state1.GetZ();
	  SimpleExtrap(state1, state2, dz);
	  ds = dz;
	}

	// now add in extra noise due to multiple scattering
	if (radLength>0.) {
	  fProp.AddNoise(state2,fabs(ds),radLength);
	}
      
	zPos.push_back(nextz);
	fApriorState.push_back(state2);
      
	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << std::setprecision(4) << std::scientific;
	  mf::LogInfo("KalmanAlg") << "Predicted state: ";
	  mf::LogInfo("KalmanAlg") << state2 << std::endl;
	  mf::LogInfo("KalmanAlg") << std::defaultfloat; 
	}
      
	// now get measurement
	double d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));

	dx = ls.X1().X() - ls.X0().X();
	dy = ls.X1().Y() - ls.X0().Y();
	//      dz = ls.X1().Z() - ls.X0().Z();
      
	//      alpha = atan2(dy,dz); // rotation about the x-axis
	//      beta = atan2(dx,dz); // rotation about the y-axis
	// for now, deal only with rotations about the z-axis, as this is the biggest effect
	gamma = atan2(dy,dx); // rotation about the z-axis
	fCosG = cos(gamma);
	fSinG = sin(gamma);
	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << std::setprecision(8);
	  mf::LogInfo("KalmanAlg") << "cos(gamma) = " << fCosG 
				   << ", sin(gamma) = " << fSinG << std::endl;
	  mf::LogInfo("KalmanAlg") << std::defaultfloat;
	}

	bool is1D = false;
	if (fabs(fCosG) < 1.e-6) {
	  fCosG = 0.;
	  fSinG = 1.;
	  is1D = true;
	} 
	if (fabs(fSinG) < 1.e-6) {
	  fSinG = 0.;
	  fCosG = 1.;
	  is1D = true;
	}

	if (is1D) 
	  Update1D(state2, d, ls.Sigma());
	else
	  Update2D(state2, d, ls.Sigma());

	state2.SetZ(nextz);
	fApostState.push_back(state2);

	// chi2 += ROOT::Math::Similarity(m,SInv);

	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << "Updated state: "
				   << std::setprecision(4) << std::scientific
				   << state2 << std::defaultfloat; 
	}
      
	state1 = state2;

	if (idx == linesegv.size()-1) {
	  d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));
	  chi2 += d*d/(ls.Sigma()*ls.Sigma());
	}
	
      } // end forward loop through measurements
      
      if (fVerbosity) 
	mf::LogInfo("KalmanAlg") << "%%%%% Now going backwards for smoothing... %%%%%";
      
      int ifail;
      // now loop backwards and update states (Kalman Smoothing)
      for (int idx = (int)linesegv.size()-2; idx>0; --idx) {
	
	auto & ls = linesegv[idx];
      
	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << "At z = " << ls.X0().Z() << " (" 
				   << zPos[idx] << ")";
	}

	double nextz = ls.X0().Z();
	bool InsideField   = fBfield&&((state1.GetZ() < fFieldZmax) && (nextz >= fFieldZmin));
	bool EnteringField = fBfield&&((state1.GetZ() > fFieldZmax) && (nextz < fFieldZmax));
	bool ExitingField  = fBfield&&((state1.GetZ() > fFieldZmin) && (nextz < fFieldZmin));

	// get radLength for multiple scattering noise calculation
	// double radLength=0.;      
	auto & ls2 = linesegv[idx-1];
	if (fVerbosity)
	  mf::LogInfo("KalmanAlg") << "Adding multiple scattering noise at SSD station " << ls2.SSDStation() << ", Plane " << ls2.SSDPlane() << std::endl;
	// int ssdId;
	// ssdId = ls2.SSDStation()*10 + ls2.SSDPlane(); // unused variable removed
	// radLength = fGeo->GetRadLength(ssdId); // unused variable removed

	double ds = 0.;
      
	kalman::State state2;      
	if (InsideField) {
	  mf::LogInfo("KalmanAlg") << "%%%%% INSIDE FIELD %%%%%" << std::endl;	
	  dz = nextz - state1.GetZ();
	  ExtrapInField(state1, state2, dz);
	}
	else if (EnteringField) {
	  mf::LogInfo("KalmanAlg") << "%%%%% ENTERING FIELD %%%%%" << std::endl;
	  dz = fFieldZmax - state1.GetZ();
	  SimpleExtrap(state1, state2, dz);	
	  state1 = state2;
	  dz = nextz - fFieldZmax;
	  ExtrapInField(state1, state2, dz);
	}
	else if (ExitingField) {
	  mf::LogInfo("KalmanAlg") << "%%%%% EXITING FIELD %%%%%" << std::endl;
	  dz = fFieldZmin - state1.GetZ();
	  ExtrapInField(state1, state2, dz);
	  state1 = state2;
	  dz = nextz - fFieldZmin;
	  SimpleExtrap(state1, state2, dz);
	}
	else { // both current state and next meas. are outside of field region
	  mf::LogInfo("KalmanAlg") << "%%%%% OUTSIDE FIELD %%%%%" << std::endl;
	  dz = nextz - state1.GetZ();
	  SimpleExtrap(state1, state2,dz);
	}

	// now get measurement
	double d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));

	dx = ls.X1().X() - ls.X0().X();
	dy = ls.X1().Y() - ls.X0().Y();
	//      dz = ls.X1().Z() - ls.X0().Z();
      
	//      alpha = atan2(dy,dz); // rotation about the x-axis
	//      beta = atan2(dx,dz); // rotation about the y-axis
	// for now, deal only with rotations about the z-axis, as this is the biggest effect
	gamma = atan2(dy,dx); // rotation about the z-axis
	fCosG = cos(gamma);
	fSinG = sin(gamma);
	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << std::setprecision(8)
				   << "cos(gamma) = " << fCosG 
				   << ", sin(gamma) = " << fSinG 
				   << std::defaultfloat;
	}
      
	bool is1D = false;
	if (fabs(fCosG) < 1.e-6) {
	  fCosG = 0.;
	  fSinG = 1.;
	  is1D = true;
	} 
	if (fabs(fSinG) < 1.e-6) {
	  fSinG = 0.;
	  fCosG = 1.;
	  is1D = true;
	}

	if (is1D) 
	  Update1D(state2, d, ls.Sigma());
	else
	  Update2D(state2, d, ls.Sigma());

	state2.SetZ(nextz);
	fApostState.push_back(state2);

	// chi2 += ROOT::Math::Similarity(m,SInv);

	if (fVerbosity) {
	  mf::LogInfo("KalmanAlg") << "Updated state: "  << std::setprecision(4) 
				   << std::scientific << state2 
				   << std::defaultfloat; 
	}
	
	state1 = state2;
	
	// update chi^2
	d = ls.DistanceToPoint(state2.GetPar(0),state2.GetPar(1));
	chi2 += d*d/(ls.Sigma()*ls.Sigma());
	
	// end reverse loop
      }
      std::cout << "%%%%% chi^2 = " << chi2 << "%%%%%" << std::endl;

    }
    return track;
  }

}// end namespace kalman
