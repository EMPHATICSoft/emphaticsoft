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
#include "SignalTime/SignalTime.h"

namespace rb {

  class ADC {

  public:

    ADC(); //Default constructor
    ADC(const emph::rawdata::WaveForm&,const emph::st::SignalTime&); 
    ~ADC() {}; //Destructor

  private:

    int _v1720index;
    int _exptime;
    float _charge;
    float _swcharge;
    float _time;

  public:
    // Getters
    const int V1720Index() const { return _v1720index; }
    const float ExpectedTime() const { return _exptime; }
    const float Time() const { return _time; }
    const float Charge() const { return _charge; }
    const float SWCharge() const { return _swcharge; }
    
    // Setters
    void SetV1720Index(int i) { _v1720index = i; }
    void SetExpectedTime(int t) { _exptime = t; }
    void SetTime(float t) { _time = t; }
    void SetCharge(float q) { _charge = q; }
    void SetSWCharge(float q) { _swcharge = q; }

    float CalcTime(const emph::rawdata::WaveForm&) const;
    float CalcCharge(const emph::rawdata::WaveForm&) const;
    float CalcSWCharge(const emph::rawdata::WaveForm&) const;
    //float CalcBACkovCharge(const emph::rawdata::WaveForm&, int adcoffset=0, int nhits=10, int start=25,  int nsamp=35, int win_size=8, float ADC_thresh=3)   const;   

  };

}

#endif // ADC_H
