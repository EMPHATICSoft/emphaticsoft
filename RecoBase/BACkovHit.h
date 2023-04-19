////////////////////////////////////////////////////////////////////////
/// \brief   Definition of BACkov Hit
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef BACkovHit_H
#define BACkovHit_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace rb {
  
  class BACkovHit {
  public:
    BACkovHit(); // Default constructor
    virtual ~BACkovHit() {}; //Destructor
    
  private:

    float _charge[6]; // Charge deposited in BACkov PMTs 
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
    void SetCharge(float x[6]) { for (int i=0; i<6; ++i) _charge[i] = x[i]; }
    void SetPID(bool pid[6]) { for (int i=0; i<5; ++i) _PID[i] = pid[i]; }
    
  };
  
}

#endif // BACkovHit_H
