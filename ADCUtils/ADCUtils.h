////////////////////////////////////////////////////////////////////////
/// \brief   Definition of ADC functions 
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef ADCUtils_H
#define ADCUtils_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include "../RawData/WaveForm.h"
#include "SignalTime/SignalTime.h"

namespace emph{
namespace adcu{

  class ADCUtils {

  public:

    ADCUtils(); //Default constructor
    ADCUtils(const emph::rawdata::WaveForm&, const emph::st::SignalTime&); 
    ~ADCUtils() {}; //Destructor

  private:

    int _v1720index;
    int _exptime;
    float _baseline;
    float _time;
    float _charge;

  public:
    //Getters
    const int V1720Index() const { return _v1720index; }
    const float ExpectedTime() const { return _exptime; }
    const float Baseline() const { return _baseline; }
    const float Time() const { return _time; }
    const float Charge() const { return _charge; }

    float CalcBaseline(const emph::rawdata::WaveForm&) const;
    int CalcTimeMax(const emph::rawdata::WaveForm&) const;
    float CalcTime(const emph::rawdata::WaveForm&) const;
    float CalcCharge(const emph::rawdata::WaveForm&) const;
  };
}//end namespace adcu
}//end namespace emph

#endif // ADCUtils_H
