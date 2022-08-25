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
    int _PID[5]; //PID numbering scheme from PDG (0 is "unknown")
    
  public:
    // Getters
    const float* Charge() const { return _charge; }
    float ChargeChan(int chan) const { return _charge[chan]; }
    const int*  PID() const { return _PID; }
    float PIDindex(int index) const { return _PID[index]; }

    // Setters
    void SetCharge(float x[3]) { for (int i=0; i<3; ++i) _charge[i] = x[i]; }
    void SetPID(int pid[6]) { for (int i=0; i<5; ++i) _PID[i] = pid[i]; }
    
  };
  
}

#endif // GasCkovHit_H
