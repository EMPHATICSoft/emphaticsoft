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

//#include "RawData/RawDigit.h"

namespace rb {
  
  class BACkovHit {
  public:
    BACkovHit(); // Default constructor
    virtual ~BACkovHit() {}; //Destructor
    
  private:

    float _charge[6]; // Charge deposited in BACkov PMTs 
    const char* _PID;
    
  public:
    // Getters
    const float* Charge() const { return _charge; }
    const char*  PID() const { return _PID; }

    // Setters
    void SetCenter(float x[3]) { for (int i=0; i<3; ++i) _center[i] = x[i]; }
    void SetRadius(float r) { _radius = r; }
    void SetNHits(int nh) { _nhits = nh; }
    
    //    friend std::ostream& operator << (std::ostream& o, const ARing& h);
  };
  
}

#endif // BACkovHit_H
