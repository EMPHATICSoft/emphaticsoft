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

namespace rb {

  class ADC {

  public:

    ADC(); //Default constructor
    ~ADC() {}; //Destructor

  private:

    int _board;
    int _chan;
    float _baseline;
    float _time;
    float _charge;

  public:
    // Getters
    const int Board() const { return _board; }
    const int Chan() const { return _chan; }
    const float Baseline() const { return _baseline; }
    const float Time() const { return _time; }
    const float Charge() const { return _charge; }
    
    // Setters
    void SetBoard(int i) { _board = i; }
    void SetChan(int i) { _chan = i; }
    void SetBaseline(float bl) { _baseline = bl; }
    void SetTime(float t) { _time = t; }
    void SetCharge(float q) { _charge = q; }

  };

}

#endif // ADC_H
