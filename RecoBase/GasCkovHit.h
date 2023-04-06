////////////////////////////////////////////////////////////////////////
/// \brief   Definition of GasCkov Hit
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef GasCkovHit_H
#define GasCkovHit_H

#include <vector>
#include <stdint.h>
#include <iostream>

//#include "RawData/RawDigit.h"

namespace rb {
  
  class GasCkovHit {
  public:
    GasCkovHit(); // Default constructor
    virtual ~GasCkovHit() {}; //Destructor
    
  private:

    float _charge[3]; // Charge deposited in GasCkov PMTs 
    bool _PID[5]; //PID numbering scheme from PDG (0 is "unknown")
    
  public:
    // Getters
    const float* Charge() const { return _charge; }
    float ChargeChan(int chan) const { return _charge[chan]; }
    const bool*  PID() const { return _PID; }
    bool IsElectron() const { return _PID[0]; }
    bool IsMuon() const { return _PID[1]; }
    bool IsPion() const { return _PID[2]; }
    bool IsKaon() const { return _PID[3]; }
    bool IsProton() const { return _PID[4]; }
    
    // Setters
    void SetCharge(float x[3]) { for (int i=0; i<3; ++i) _charge[i] = x[i]; }
    void SetPID(bool pid[6]) { for (int i=0; i<5; ++i) _PID[i] = pid[i]; }
    
  };
  
}

#endif // GasCkovHit_H
