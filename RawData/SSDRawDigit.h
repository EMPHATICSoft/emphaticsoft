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
    SSDRawDigit(int32_t station, int32_t module, int32_t row, int32_t t, 
		            int32_t adc, int32_t trigtype);

    int32_t FER() const { return fFER; }
    int32_t Module() const { return fModule; }
    int32_t Chip() const { return fChip; }
    int32_t Set() const { return fSet; }
    int32_t Strip() const { return fStrip; }
    int32_t Time() const { return fTime; }
    int32_t ADC() const { return fADC; }
    int32_t TrigNum() const { return fTrigNum; }
    int32_t Row() const { return fRow; }

    // For Simulation:  We seemingly don't have a map for converting a row number, as given by a 
    // coordinate on the sensor plane, in the simulation, to a set, chip and strip number. The reciprocal
    // of the function getSensorRow is not uniquely determined.
    // So, in the converter of a Sim::SSDHit (original, or Autre), we need a more direct way to set the
    // Row number, as the chip, set and strip number are not used in the reconstruction code.
    // Paul Lebrun March 20 2023
    inline void SetRow(uint32_t aRow) { fRow = aRow; }   
    friend std::ostream& operator << (std::ostream& o, const SSDRawDigit& r);

    static uint32_t getSensorRow(int, int, int);

  private:
    int32_t  fFER;
    int32_t  fModule;
    int32_t  fChip;
    int32_t  fSet;
    int32_t  fStrip;
    int32_t  fTime;
    int32_t  fADC;
    int32_t  fTrigNum;
    int32_t  fRow;
    int32_t  fBco2s;

    uint32_t getSensorRow() const;
  };

}
}
#endif // SSDRAWDIGIT_H
