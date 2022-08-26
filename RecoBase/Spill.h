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
    float  fMT5CPR, fMT6CPR;// gas Ckov pressure readings
    float  fMT5CP2, fMT6CP2;// iFix readings
    float  fTBCCDI;         // TestBeam ckov counts inner
    float  fTBCCDO;         // TestBeam ckov counts outer
    float  fMT6CA1;         // MT6 ckov east mirror counts
    float  fMT6CA2;         // MT6 ckov west mirror counts
    float  fMT6CA3;         // MT6 ckov top mirror counts

  public:
    // Getters
    float  Intensity()   const { return fIntensity; }
    float  Momentum()    const { return fMomentum; }
    float  MT5CPR()      const { return fMT5CPR; } 
    float  MT6CPR()      const { return fMT6CPR; } 
    float  MT5CP2()      const { return fMT5CP2; } 
    float  MT6CP2()      const { return fMT6CP2; } 
    float  TBCCDI()      const { return fTBCCDI; }
    float  TBCCDO()      const { return fTBCCDO; }
    float  MT6CA1()      const { return fMT6CA1; }
    float  MT6CA2()      const { return fMT6CA2; }
    float  MT6CA3()      const { return fMT6CA3; }
    uint32_t Timestamp() const { return fTimestamp; }
    
    // Setters
    void SetIntensity(float ppp) { fIntensity = ppp; }
    void SetMomentum(float mom) { fMomentum = mom; }
    void SetTimestamp(uint32_t t) { fTimestamp = t; }
    void SetMT5CPR(float v)   { fMT5CPR = v; } 
    void SetMT6CPR(float v)   { fMT6CPR = v; } 
    void SetMT5CP2(float v)   { fMT5CP2 = v; } 
    void SetMT6CP2(float v)   { fMT6CP2 = v; } 
    void SetTBCCDI(float v)   { fTBCCDI = v; }
    void SetTBCCDO(float v)   { fTBCCDO = v; }
    void SetMT6CA1(float v)   { fMT6CA1 = v; }
    void SetMT6CA2(float v)   { fMT6CA2 = v; }
    void SetMT6CA3(float v)   { fMT6CA3 = v; }
    
    friend std::ostream& operator << (std::ostream& o, const Spill& h);
  };
  
}

#endif // RBSPILL_H
