////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD raw digits
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDRAWDIGIT_H
#define SSDRAWDIGIT_H

#include <vector>
#include <utility>
#include <stdint.h>
#include <iostream>

namespace emph {
namespace rawdata {
  
  class SSDRawDigit {
  public:
    SSDRawDigit(); // Default constructor
    ~SSDRawDigit() {}; //Destructor

    SSDRawDigit(int32_t station, int32_t module, int32_t chip, int32_t set, int32_t strip,
            int32_t t, int32_t adc, int32_t trigtype);

    int32_t Station() const { return fStation; }
    int32_t Module() const { return fModule; }
    int32_t Chip() const { return fChip; }
    int32_t Set() const { return fSet; }
    int32_t Strip() const { return fStrip; }
    int32_t Time() const { return fTime; }
    int32_t ADC() const { return fADC; }
    int32_t TrigType() const { return fTrigType; }
    int32_t Row() const { return fRow; }
    
    friend std::ostream& operator << (std::ostream& o, const SSDRawDigit& r);

    static uint32_t getSensorRow(int, int, int);

  private:
    int32_t  fStation;
    int32_t  fModule;
    int32_t  fChip;
    int32_t  fSet;
    int32_t  fStrip;
    int32_t  fTime;
    int32_t  fADC;
    int32_t  fTrigType;
    int32_t  fRow;

    uint32_t getSensorRow() const;
  };

}
}
#endif // SSDRAWDIGIT_H
