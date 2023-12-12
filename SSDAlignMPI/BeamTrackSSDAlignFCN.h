////////////////////////////////////////////////////////////////////////
/// \brief   2D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACKSSDALIGNFCN_H
#define BEAMTRACKSSDALIGNFCN_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "BTAlignGeom.h"
#include "BTAlignGeom1c.h"
#include "SSDAlignParams.h"
#include "BeamTracks.h"
#include "BTAlignInput.h"
#include "Minuit2/FCNBase.h"

namespace emph{ 
  namespace rbal {
    
    class BeamTrackSSDAlignFCN : public ROOT::Minuit2::FCNBase {
    
    public:
      BeamTrackSSDAlignFCN(const std::string &afitType, emph::rbal::BTAlignInput *DataIn);
      
    private:
      bool fIsPhase1c; 
      BTAlignGeom* myGeo;
      BTAlignGeom1c* myGeo1c;
      mutable SSDAlignParams* myParams;
      emph::rbal::BTAlignInput *myBTIn;
      std::string fFitType;
      bool fIsMC; // Very ugly, not sure about sign conventions.. 
      bool fNoMagnet; 
      bool fStrictSt6;
      bool fBeamConstraint;
      bool fAlignMode;
      bool fDoAllowLongShiftByStation;
      bool fDoAntiPencilBeam; 
      char fSelectedView;
      double fAssumedSlopeSigma;
      double fBeamBetaFunctionY, fBeamBetaFunctionX;
      double fBeamAlphaFunctionY, fBeamAlphaFunctionX;
      double fBeamGammaX, fBeamGammaY;
      double fSoftLimitDoubleSensorCrack;
      double fMinimumDoubleSensorCrack;
      double fExpectedEpsilY;
      double fNominalMomentum, fNominalMomentumDisp;
      bool fSoftLimits;
      double fUpLimForChiSq; // Upper limit for accepting fitted tracks.
      bool fDebugIsOn;
      mutable bool fDumpBeamTracksForR;  
      mutable int fNCalls;
      double fErrorDef; // for Minuit. , and for pitch err.. 
      mutable std::vector<double> fResids;
      mutable std::string fNameForBeamTracks;
      mutable std::ofstream fFOutHistory;
      mutable std::vector<bool> fIsOK; // dimensioned on the number of events, on a given rank. 
      //  States that this track ought to contribute to the chiSq, based solely on the first iteration.  
      mutable std::vector<double> fGapValues; // The distance between strip 0 (or 639), ignoring the Roll angle. 
      
    public:
      // Setters
      void SetMCFlag(bool v) {
         fIsMC = v; 
         if (v) std::cerr << " BeamTrackSSDAlignFCN, This is will be a Monte Carlo study " << std::endl;
	 else  std::cerr << " BeamTrackSSDAlignFCN, This is will be a study on Real Data" << std::endl;
      } 
      inline void SetForPhase1c(bool t=true) { 
        fIsPhase1c = t; 
	
//        if (t) {
//	  
//	}
      }
      void SetSelectedView(char v = 'A') {fSelectedView = v;}
      void SetPtrInput(emph::rbal::BTAlignInput *in) { myBTIn = in;}
      void SetUpLimForChiSq(double u) {  fUpLimForChiSq = u;}
      void SetFitType(const std::string &aft) { fFitType = aft; } 
      void SetBeamConstraint(bool v) { fBeamConstraint = v; } 
      void SetAlignMode(bool v=true) { fAlignMode = v; } 
      void SetAntiPencilBeam(bool v=true) {fDoAntiPencilBeam = v;}
      void SetAllowLongShiftByStation(bool v=true) { fDoAllowLongShiftByStation = true; } 
      void SetNoMagnet(bool v=true) {fNoMagnet = v;}
      void SetAssumedEpsilY(double a) {fExpectedEpsilY = a; fBeamConstraint=true;  }
      void SetBeamAlphaBetaFunctionY (double a, double b) {fBeamAlphaFunctionY = a; fBeamBetaFunctionY = b;}
      void SetBeamAlphaBetaFunctionX (double a, double b) {fBeamAlphaFunctionX = a; fBeamBetaFunctionX = b;}
      void SetStrictSt6(bool v) { fStrictSt6 = v; } 
      void SetSoftLimits(bool v) { fSoftLimits = v; } 
      void SetUpError(double v) { fErrorDef = v; }
      void SetNominalMomentum(double p) { fNominalMomentum = p; } 
      void SetNominalMomentumDisp(double p) { fNominalMomentumDisp = p; } 
      void SetDumpBeamTracksForR(bool v) { fDumpBeamTracksForR = v; } 
      void SetNameForBeamTracks (const std::string &aName) { fNameForBeamTracks = aName; } 
      inline void SetAssumedSlopeSigma(double v) { fAssumedSlopeSigma = v; }
      inline void SetSoftLimitDoubleSensorCrack(double v) {fSoftLimitDoubleSensorCrack = v; }
      inline void SeMinimumDoubleSensorCrack(double v) {fMinimumDoubleSensorCrack = v; }
      inline void SetDebug(bool d=true) {fDebugIsOn = true;}
      inline void ResetOKFlags() { fIsOK.clear();}
   
      // Get, basic operators, interface to Minuit2 
      inline bool GetMCFlag() const { return fIsMC; } 
      virtual double Up() const {return fErrorDef;}
      virtual double operator()(const std::vector<double>&) const; // argument is the parameter vector. (from 12 to 64 parameters, depending on the 
     
      inline double Resid(size_t kSe) const {
        if (kSe < fResids.size()) return fResids[kSe];
	return DBL_MAX;
      }
      void OpenChiSqHistoryFile(const std::string &token);
      void CloseChiSqHistoryFile();
      inline int NCalls() const { return fNCalls; } 
      
    private:  
    
      double BeamConstraintY(const emph::rbal::BeamTracks &btrs) const;  
      double BeamConstraintX(const emph::rbal::BeamTracks &btrs) const;  // this include the nominal momentum. 
      double BeamConstraintEpsilY(const emph::rbal::BeamTracks &btrs) const;  // this include the nominal momentum. 
      double SlopeConstraintAtStation0(const emph::rbal::BeamTracks &btrs) const;
      double SurveyConstraints(const std::vector<double> &pars) const; // for all parameter.  In this case, parameters are not Minuit limited.  
    };
  }
}
#endif
    
