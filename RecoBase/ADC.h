////////////////////////////////////////////////////////////////////////
/// \brief   Definition of ADC functions 
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef ADC_H
#define ADC_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include "../RawData/WaveForm.h"

namespace rb {

  class ADC {

  public:

    ADC(); //Default constructor
    ADC(const emph::rawdata::WaveForm&); 
    ~ADC() {}; //Destructor

  private:

    float _charge;
    float _backovcharge;
    float _time;

  public:
    // Getters
    const float Charge() const { return _charge; }
    const float BACkovCharge() const { return _backovcharge; }
    const float Time() const { return _time; }
    
    // Setters
    void SetCharge(float q) { _charge = q; }
    void SetBACkovCharge(float q) { _backovcharge = q; }
    void SetTime(float t) { _time = t; }

    float CalcCharge(const emph::rawdata::WaveForm&, int adcoffset=0, int nhits=10, int start=10, int nsamp=95)   const;
    float CalcBACkovCharge(const emph::rawdata::WaveForm&, int adcoffset=0, int nhits=10, int start=25,  int nsamp=35, int win_size=8, float ADC_thresh=3)   const;   
    float CalcTime(const emph::rawdata::WaveForm&) const;

  };

}

#endif // ADC_H
