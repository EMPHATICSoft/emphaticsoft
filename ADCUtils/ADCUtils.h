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
    /// \param slopeMin  minimum |dADC/dtick| (positive) for the hit time to be
    ///                  taken from the waveform itself.  0 (the default) keeps
    ///                  the historical behaviour: the window always comes from
    ///                  the SignalTime map.
    ADCUtils(const emph::rawdata::WaveForm&, const emph::st::SignalTime&, const int&, const int&,
             const int& slopeMin = 0);
    ~ADCUtils() {}; //Destructor

  private:

    int _v1720index;
    int _exptime;      ///< expected hit sample from the SignalTime map
    int _hittime;      ///< sample the window is actually built around
    bool _selftimed;   ///< true if _hittime came from the waveform, not the map
    float _baseline;
    float _time;
    float _charge;

  public:
    //Getters
    const int V1720Index() const { return _v1720index; }
    const float ExpectedTime() const { return _exptime; }
    const int HitTime() const { return _hittime; }
    const bool IsSelfTimed() const { return _selftimed; }
    const float Baseline() const { return _baseline; }
    const float Time() const { return _time; }
    const float Charge() const { return _charge; }

    float CalcBaseline(const emph::rawdata::WaveForm&) const;
    int CalcTimeMax(const emph::rawdata::WaveForm&) const;
    float CalcTime(const emph::rawdata::WaveForm&) const;
    /// Sample of the steepest falling edge in the 5 ticks up to the minimum,
    /// or -1 if no slope reaches slopeMin.  slopeMin<=0 accepts any fall.
    int CalcTimeSample(const emph::rawdata::WaveForm&, const int& slopeMin) const;
    float CalcCharge(const emph::rawdata::WaveForm&, const int&, const int&) const;
  };
}//end namespace adcu
}//end namespace emph

#endif // ADCUtils_H
