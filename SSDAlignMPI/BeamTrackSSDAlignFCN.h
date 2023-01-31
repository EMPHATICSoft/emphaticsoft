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
      BTAlignGeom* myGeo;
      mutable SSDAlignParams* myParams;
      emph::rbal::BTAlignInput *myBTIn;
      std::string fFitType;
      bool fStrictSt6;
      bool fBeamConstraint;
      double fBeamBetaFunctionY, fBeamBetaFunctionX;
      double fBeamAlphaFunctionY, fBeamAlphaFunctionX;
      double fBeamGammaX, fBeamGammaY;
      bool fSoftLimits;
      double fUpLimForChiSq; // Upper limit for accepting fitted tracks.
      bool fDebugIsOn; 
      mutable int fNCalls;
      double fErrorDef; // for Minuit. , and for pitch err.. 
      mutable std::vector<double> fResids;
      mutable std::ofstream fFOutHistory;
      
    public:
      // Setters
      void SetPtrInput(emph::rbal::BTAlignInput *in) { myBTIn = in;}
      void SetUpLimForChiSq(double u) {  fUpLimForChiSq = u;}
      void SetFitType(const std::string &aft) { fFitType = aft; } 
      void SetBeamConstraint(bool v) { fBeamConstraint = v; } 
      void SetBeamAlphaBetaFunctionY (double a, double b) {fBeamAlphaFunctionY = a; fBeamBetaFunctionY = b;}
      void SetBeamAlphaBetaFunctionX (double a, double b) {fBeamAlphaFunctionX = a; fBeamBetaFunctionX = b;}
      void SetStrictSt6(bool v) { fStrictSt6 = v; } 
      void SetSoftLimits(bool v) { fSoftLimits = v; } 
      void SetUpError(double v) { fErrorDef = v; }
      inline void SetDebug(bool d=true) {fDebugIsOn = true;} 
      // Get, basic operators, interface to Minuit2 
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
      double BeamConstraintX(const emph::rbal::BeamTracks &btrs) const;  
      double SurveyConstraints(const std::vector<double> &pars) const; // for all parameter.  In this case, parameters are not Minuit limited.  
    };
  }
}
#endif
    
