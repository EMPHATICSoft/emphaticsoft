////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD raw digits
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRAWDIGIT_H
#define SSDRAWDIGIT_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace rawdata {
  
  class SSDRawDigit {
  public:
    SSDRawDigit(); // Default constructor
    ~SSDRawDigit() {}; //Destructor

  private:
    
    int32_t  fStation;
    int32_t  fModule;
    int32_t  fRow;
    int32_t  fTime;
    int32_t  fADC;
    int32_t  fTrigType;
    
  public:
    SSDRawDigit(int32_t station, int32_t module, int32_t row, int32_t t,
		int32_t adc, int32_t trigtype);

    int32_t Station()  const {return fStation; }
    int32_t Module()   const {return fModule; }
    int32_t Row()      const {return fRow; }
    int32_t Time()     const {return fTime; }
    int32_t ADC()      const {return fADC; }
    int32_t TrigType() const {return fTrigType; }
    
    friend std::ostream& operator << (std::ostream& o, const SSDRawDigit& r);
  };
  
}

#endif // SSDRAWDIGIT_H
