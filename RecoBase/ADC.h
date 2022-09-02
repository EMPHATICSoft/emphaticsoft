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

  class ADC: public emph::rawdata::WaveForm {
  
  using emph::rawdata::WaveForm::WaveForm;
  
  private:
    //emph::rawdata::WaveForm* wvfm;

  public:

    ADC(); //Default constructor
    ~ADC() {}; //Destructor
    float        Charge(int adcoffset=0, int nhits=10, int start=10, int nsamp=95)   const;
    float        BACkovCharge(int adcoffset=0, int nhits=10, int start=25,  int nsamp=35, int win_size=8, float ADC_thresh=3)   const;   
  };

}

#endif // ADC_H
