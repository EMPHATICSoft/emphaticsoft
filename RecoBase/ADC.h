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
    ~ADC() {}; //Destructor
    float        Charge(const emph::rawdata::WaveForm*, int adcoffset=0, int nhits=10, int start=10, int nsamp=95)   const;
    float        BACkovCharge(const emph::rawdata::WaveForm*, int adcoffset=0, int nhits=10, int start=25,  int nsamp=35, int win_size=8, float ADC_thresh=3)   const;   
  };

}

#endif // ADC_H
