////////////////////////////////////////////////////////////////////////
/// \brief   3D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACK3DFCN_H
#define BEAMTRACK3DFCN_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <cmath>
#include "BeamTrackCluster.h" 
#include "BTAlignGeom.h"
#include "BTAlignGeom1c.h"
#include "Minuit2/FCNBase.h"
#include "BTMagneticField.h"

namespace emph{ 
  namespace rbal {
    
    class BeamTrack3DFCN : public ROOT::Minuit2::FCNBase {
    
    struct fFCNcall { 
        int fICall;
	double fChiSq, fx, fslx0, fy, fysly0, fP;
    };
    
    class inDatafDebug {  // Input data and prediction, for a given event. Checking things.. 
      public: 
        explicit inDatafDebug(int kSt, char aView, int aSensor, double aStrip, double rollCorr, double aTm, double aPred); 
        int fStation; 
        char fView;
        int fSensor;
        double fStrip, fRollCorr, fTMeas, fTMeasErr, fTPred, fDeltaChi; 
        void SetDeltaChiSq(double v) {fDeltaChi = v;}
        void SetMeasErr(double v) {fTMeasErr = v;}
    };
    
    
    public:
      BeamTrack3DFCN();
      
    private:
      BTAlignGeom* myGeo;
      BTAlignGeom1c* myGeo1c;
      bool fIsPhase1c;
      bool fIsMC; // Ugly, we are still working on the sign convention and rotation angles signs.. 
      bool fNoMagnet; // to align on 120 GeV. 
      bool fDebugIsOn;
      char fSelectedView;
      size_t fNumSensorsTotal;
      std::vector<BeamTrackCluster>::const_iterator fItCl;
      double fErrorDef, fOneOverSqrt2, fOneOSqrt12; // for Minuit. 
      double fMaxAngleLin; // Maximum rotation angle to trigger the use of exact trig function. 
      double fNominalMomentum, fNominalMomentumDisp;
      mutable double fx5, fy5, fx6, fy6; // 5 for Phase1b, 6 for Phase1c 
      mutable int fNCalls;
      mutable std::vector<double> fResids;
      mutable BTMagneticField *fMagField;
      // To understand the evolution of these 3D track fits 
      mutable std::vector<fFCNcall> fFNHistory;
      mutable std::vector<inDatafDebug> fInDataDbg;
      
    public:
    
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (x,x' in our case.)
     
      inline void SetForPhase1c(bool t=true) { 
        fIsPhase1c = t; 
        if (t) {
	    fNumSensorsTotal = 2*myGeo1c->NumSensorsXorY() + myGeo1c->NumSensorsU() + myGeo1c->NumSensorsV(); 
	    fResids.resize(fNumSensorsTotal);
	}
      }
      inline void SetClusterPtr(std::vector<BeamTrackCluster>::const_iterator it) { fItCl = it; } 
      inline void SetErrorDef(double e) { fErrorDef = e; }
      inline void SetNoMagnet(bool v=true) { fNoMagnet = v; }
      inline void SetMCFlag(bool v) { fIsMC = v; }
      inline void SetNominalMomentum(double v) { fNominalMomentum = v; } 
      inline void SetNominalMomentumDisp(double v) { fNominalMomentumDisp = v; } 
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
      inline void resetNumCalls() { fNCalls = 0; }
      inline void SetSelectedView(char v) { fSelectedView = v; } 
      
      inline void SetDebugOn(bool v=true) { fDebugIsOn = v; }
      inline double GetXAtStation5() const { return fx5; }
      inline double GetYAtStation5() const { return fy5; }
      inline double GetXAtStation6() const { return fx6; }
      inline double GetYAtStation6() const { return fy6; }
     
      void DumpfInDataDbg(bool byStation=true) const; 
      
      // Correcting over-linearization, go back to trig function.. 
      
      inline double exactXPred(double xPred, double yPred, double rollAngle, double rollAngleCenter) const {
        const double x =std::cos(rollAngle)*xPred + (yPred-rollAngleCenter)*std::sin(rollAngle); 
        return x; 
      } 
       inline double exactYPred(double xPred, double yPred, double rollAngle, double rollAngleCenter) const {
        const double y =std::cos(rollAngle)*yPred + (xPred-rollAngleCenter)*std::sin(rollAngle); 
        return y; 
      } 
       inline double exactUPred(double xPred, double yPred, double rollAngle, double rollAngleCenter) const {
       // To be consistent with linearized algorithm, we do in fact two rotations. 
        const double uu =fOneOverSqrt2 * (xPred + yPred);
        const double vv =fOneOverSqrt2 * (xPred - yPred);
	const double  u = uu*std::cos(rollAngle) + (vv-rollAngleCenter)*std::sin(rollAngle); 
        return u; 
      } 
       inline double exactVPred(double xPred, double yPred, double rollAngle, double rollAngleCenter) const  {
        const double uu =fOneOverSqrt2 * (xPred + yPred);
        const double vv =fOneOverSqrt2 * (xPred - yPred);
	const double  v = vv*std::cos(rollAngle) + (uu-rollAngleCenter)*std::sin(rollAngle); 
        return v; 
      } 
     
     };
   }
}
#endif
    
