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
#include "TF1.h"
#include "TH1F.h"
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
    float _baseline;
    float _time;
    float _charge;

  public:
    // Getters
    const int V1720Index() const { return _v1720index; }
    const float ExpectedTime() const { return _exptime; }
    const float Baseline() const { return _baseline; }
    const float Time() const { return _time; }
    const float Charge() const { return _charge; }
    
    // Setters
    void SetV1720Index(int i) { _v1720index = i; }
    void SetExpectedTime(int t) { _exptime = t; }
    void SetBaseline(int bl) { _baseline = bl; }
    void SetTime(float t) { _time = t; }
    void SetCharge(float q) { _charge = q; }

  };

}

#endif // ADC_H
