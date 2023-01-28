////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a single SSD Alignment parameter (Transverse shifts, rolls, angles.. )  
///          intendded as  Minuit2 Parameters (not sure if I could consider Minuit2 parameter class as a base class)
///          A simpler implementation would be based on a map, Minuit2 parameter number, but we'll be missing information 
///          to update the geometry (done in this class) 
///          Used by BTFitAlign classes  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef SSDALIGNPARAM_H
#define SSDALIGNPARAM_H

#include <string>
#include <utility>
#include <stdint.h>
#include <iostream>

#include "BTAlignGeom.h"

namespace emph{ 
  namespace rbal {
  
    enum paramType {
      NONE = 0,
      TRSHIFT = 1, /// We will move the sensor laterally 
      ZSHIFT,      /// We will move the sensor longitudinally 
      PITCHCORR,   /// We will tilt the sensor, either a Pitch angle (Y view) or Yaw angle (X view), thereby reducing the apparent SSD strip pitch 
      ROLL, /// We will rotate the sensor about the Z axis. 
      ZMAGC, /// The shift of the magnetic center 
      KICKMAGN // The strength of the magnetic kick.  
    };
    
    class SSDAlignParam {
    
    public:
      SSDAlignParam();
      std::string CheckAndComposeName();
     
    private:
      BTAlignGeom* myGeo;
      int fMinNum;  // The Minuit parameter number 
      std::string fName; // full name 
      char fView; //
      size_t fSensor; 
      paramType fType; // TrShift, PitchCorr.. see above.
      mutable double fValue; 
      std::pair<double, double> fLimits; // All them will have limits.. angle are less than 2Pi, for instance. In practice, much smaller, one hopes. 
         
    public:
    
      inline void SetMinuitNumber(int i) { fMinNum=i; }
      inline void SetView(char v) { fView = v; }
      inline void SetSensor(size_t s) { fSensor = s; }
      inline void SetType(paramType t) { fType = t; }
      inline void SetValue(double v) { fValue = v; this->UpdateGeom();}
      inline void SetUpLimit(double u) { fLimits.second = u; }
      inline void SetDownLimit(double d) { fLimits.first = d; }
      inline void SetLimits(std::pair<double, double> p) { fLimits = p;}
      //
      inline int MinuitNumber() const { return fMinNum; }
      inline char View() const { return fView; }
      inline size_t SensorS() const { return fSensor; }
      inline int SensorI() const { return static_cast<int>(fSensor); }
      inline paramType Type() const {return fType; } 
      inline std::string Name() const { return fName; }
      inline double Value() const { return fValue; } 
      inline std::pair<double, double> Limits() const { return fLimits; }
      inline double DownLimit() const { return fLimits.first; } 
      inline double UpLimit() const { return fLimits.second; } 
    
      void UpdateGeom() const; 

    };
  }
}  
#endif
