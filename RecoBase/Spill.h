////////////////////////////////////////////////////////////////////////
/// \brief   Definition of Spill
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef RBSPILL_H
#define RBSPILL_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace rb {
  
  class Spill {
  public:
    Spill(); // Default constructor
    virtual ~Spill() {}; //Destructor
    
  private:
    uint32_t fTimestamp;
    float  fIntensity;
    float  fMomentum;
    
  public:
    // Getters
    float  Intensity() const { return fIntensity; }
    float  Momentum()  const { return fMomentum; }
    uint32_t Timestamp() const { return fTimestamp; }

    // Setters
    void SetIntensity(float ppp) { fIntensity = ppp; }
    void SetMomentum(float mom) { fMomentum = mom; }
    void SetTimestamp(uint32_t t) { fTimestamp = t; }
    
    friend std::ostream& operator << (std::ostream& o, const Spill& h);
  };
  
}

#endif // RBSPILL_H
