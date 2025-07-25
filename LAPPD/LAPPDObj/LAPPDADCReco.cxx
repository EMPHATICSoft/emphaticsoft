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


  //----------------------------------------------------------------------
  LAPPDADCWaveform LAPPDADCReco::GetChannel(int channel) const
  {
    if (channel < 0 || channel >= kNChannels) {
      std::cerr << "Error: Invalid channel number " << channel << ". Must be between 0 and " 
                << kNChannels - 1 << "." << std::endl;
      return LAPPDADCWaveform(); // Return an empty waveform
    }
    return fChannels[channel];
  }

  //----------------------------------------------------------------------
  LAPPDADCWaveform& LAPPDADCReco::GetChannel(int channel)
  {
    if (channel < 0 || channel >= kNChannels) {
      std::cerr << "Error: Invalid channel number " << channel << ". Must be between 0 and " 
                << kNChannels - 1 << "." << std::endl;
      throw std::out_of_range("Invalid channel number");
    }
    return fChannels[channel];
  }

  //----------------------------------------------------------------------
  void LAPPDADCReco::SetADCValue(int channel, int tick, float value)
  {
    if (channel < 0 || channel >= kNChannels) {
      std::cerr << "Error: Invalid channel number " << channel << ". Must be between 0 and " 
                << kNChannels - 1 << "." << std::endl;
      return;
    }
    if (tick < 0 || tick >= kNTicks) {
      std::cerr << "Error: Invalid tick number " << tick << ". Must be between 0 and " 
                << kNTicks - 1 << "." << std::endl;
      return;
    }
    fChannels[channel].fData[tick] = value; // Set the ADC value for the specified channel and tick
  }


}
