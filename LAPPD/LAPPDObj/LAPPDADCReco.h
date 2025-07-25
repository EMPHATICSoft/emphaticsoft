////////////////////////////////////////////////////////////////////////
/// \brief   LAPPDRawDigit class
/// \author  fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///  -- Description of the LAPPDADCReco data format --
/// 
////////////////////////////////////////////////////////////////////////

#ifndef LAPPDADCReco_H
#define LAPPDADCReco_H

#include <iostream>

#include "LAPPD/LAPPDObj/LAPPD_format.h"
#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"

namespace lappd {

  class LAPPDADCWaveform {
  public:
    LAPPDADCWaveform();
    
    virtual ~LAPPDADCWaveform() {} // Destructor

    // Getters
    float ADC(int tick) const { return fData[tick]; } // Get ADC value for a specific tick
    short unsigned int Channel() const { return fChannel; } // Get channel number
    int NADCs() const { return kNTicks; } // Get number of ADCs (ticks)
    
  private:
    float fData[kNTicks]; // 12-bit ADC values for the 256 time ticks
    short unsigned int fChannel; // Channel number (0-29)

    friend class LAPPDADCReco; // Allow LAPPDADCReco to access private members
  };

  class LAPPDADCReco {
  public:
    
    // Default constructor
    LAPPDADCReco();
    
    // Constructor with LAPPDRawDigit
    LAPPDADCReco(const LAPPDRawDigit& rawDigit);

    // Destructor
    virtual ~LAPPDADCReco() {};

    // Getters
    uint8_t GetACDCNumber() const { return fACDCNumber; } // Get ACDC number
    uint32_t GetEventNumber() const { return fEventNumber; } // Get event number
    uint64_t GetTimeStamp() const { return fTimeStamp; } // Get timestamp
    uint32_t GetWRTimePPS() const { return fWRTimePPS; } // Get White Rabbit time PPS counter
    uint32_t GetWRTime250MHz() const { return fWRTime250MHz; } // Get White Rabbit time 250 MHz counter
    LAPPDADCWaveform GetChannel(int channel) const; // Get waveform for a specific channel
    LAPPDADCWaveform& GetChannel(int channel); // Get waveform for a specific channel (non-const)
    int GetNChannels() const { return kNChannels; } // Get number of channels

    // ADC setters (for calibration or processing)
    void SetADCValue(int channel, int tick, float value);

  private:
    
    // Decoded data
    uint8_t fACDCNumber; // ACDC number (8 bits)
    uint32_t fEventNumber; // Event number (32 bits)
    uint64_t fTimeStamp; // Timestamp (64 bits) - 320 MHz clock
    uint32_t fWRTimePPS; // White Rabbit time PPS counter
    uint32_t fWRTime250MHz; // White Rabbit time 250 MHz counter
    LAPPDADCWaveform fChannels[kNChannels]; // Readout channels for the 30 ACDC channels

  };

} // end namespace lappd

#endif
