////////////////////////////////////////////////////////////////////////
/// \brief   Source file for LAPPDADCReco class
/// \author  fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "LAPPD/LAPPDObj/LAPPDADCReco.h"

namespace lappd{

  //----------------------------------------------------------------------
  LAPPDADCWaveform::LAPPDADCWaveform()
  { 
  }

  //----------------------------------------------------------------------
  LAPPDADCReco::LAPPDADCReco()
  {
    fACDCNumber = 0;
    fEventNumber = 0;
    fTimeStamp = 0;
    fWRTimePPS = 0;
    fWRTime250MHz = 0;

    for (int i = 0; i < kNChannels; ++i) {
      fChannels[i] = LAPPDADCWaveform(); // Initialize each channel
    }
  }

  //----------------------------------------------------------------------
  LAPPDADCReco::LAPPDADCReco(const LAPPDRawDigit& rawDigit)
  {
    fACDCNumber = rawDigit.GetACDCNumber();
    fEventNumber = rawDigit.GetEventNumber();
    fTimeStamp = rawDigit.GetTimeStamp();
    fWRTimePPS = rawDigit.GetWRTimePPS();
    fWRTime250MHz = rawDigit.GetWRTime250MHz();

    // Initialize each channel waveform
    for (int i = 0; i < kNChannels; ++i) {
      fChannels[i] = LAPPDADCWaveform();
      for (int tick = 0; tick < kNTicks; ++tick) {
        fChannels[i].fData[tick] = static_cast<float>(rawDigit.GetChannel(i).ADC(tick));
      }
    }
  }


}
