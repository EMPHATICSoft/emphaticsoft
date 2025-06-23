////////////////////////////////////////////////////////////////////////
/// \brief   LAPPDRawDigit class
/// \author  jpaley@fnal.gov, fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///  -- Description of the LAPPDRawDigit data format --
///
/// [ACC Header (64 bits)][ACDC Header (64 bits)][Time 320 MHz (64 bits)]
/// [Time WR (64 bits)][Align Header (64 bits)][Data (1440 x 64 bits)]
/// where:
/// - ACC Header: 64 bits - format is 56 bits + 8 bits
/// - ACDC Header: 64 bits - format is 16 bits + 48 bits
/// - System time (320 MHz clock): 64 bits
/// - White rabbit time: 64 bits
/// - Align Header: 64 bits ---!Check 
/// - Data: 1440 x 64 bits (12-bit ADC values for 256 time ticks across 30 ACDC channels)
///       256 ticks x 30 channels x 12 bits = 92160 bits = 1440 64 bit words
/// 
////////////////////////////////////////////////////////////////////////

#ifndef LAPPDRAWDIGIT_H
#define LAPPDRAWDIGIT_H

#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <iostream>
#include <inttypes.h>
#include <cstring>
#include <fstream>
#include <bitset>



namespace lappd {

  const short unsigned int kN64BitHeaderWords = 5; // Number of 64-bit header words
  const short unsigned int kN64BitDataWords = 1440; // Number of 64-bit words in the data section
  const short unsigned int kN64BitTotalWords = kN64BitHeaderWords + kN64BitDataWords; // Total number of 64-bit words in the event

  const short unsigned int kNChannels = 30; // Number of channels in the ACDC
  const short unsigned int kNTicks = 256; // Number of time ticks per channel

  class LAPPDReadoutChannelRawDigit {
  public:
    LAPPDReadoutChannelRawDigit(); // Default constructor initializes channel to 0 and data to 0

    LAPPDReadoutChannelRawDigit(uint8_t channel, uint16_t* data);
    
    virtual ~LAPPDReadoutChannelRawDigit() {} // Destructor

    // Getters
    uint16_t ADC(int tick) const { return fData[tick]; } // Get ADC value for a specific tick
    short unsigned int Channel() const { return fChannel; } // Get channel number
    int NADCs() const { return kNTicks; } // Get number of ADCs (ticks)
    
  private:
    uint16_t fData[kNTicks]; // 12-bit ADC values for the 256 time ticks
    short unsigned int fChannel; // Channel number (0-29)
  };


  class LAPPDRawDigit {
  public:
    LAPPDRawDigit(); // Default constructor
    LAPPDRawDigit(uint64_t* block);
    LAPPDRawDigit(uint64_t accH, uint64_t acdcH, uint64_t t320, uint64_t twr, uint64_t aH, uint64_t* data);
    virtual ~LAPPDRawDigit() {}; // Destructor

    // Getter functions - to access raw data
    uint64_t GetACCHeader() const {return fACCHeader; }
    uint64_t GetACDCHeader() const {return fACDCHeader; }
    uint64_t GetTime320() const {return fTime320; }
    uint64_t GetTimeWR() const {return fTimeWR; }
    uint64_t GetAlignHeader() const {return fAlignHeader; }
    const uint64_t* GetData() const {return fData; }

    // Getter functions - to access decoded data
    uint8_t GetACDCNumber() const {return fACDCNumber; }
    uint32_t GetEventNumber() const {return fEventNumber; }
    uint64_t GetTimeStamp() const {return fTimeStamp; }
    uint32_t GetWRTimePPS() const {return fWRTimePPS; }
    uint32_t GetWRTime250MHz() const {return fWRTime250MHz; }
    LAPPDReadoutChannelRawDigit GetChannel(int i) const;
    int GetNChannels() const { return kNChannels; } // Get number of channels


    bool IsNoise() const {return fIsNoise; }

    // Setters
    void     SetACCHeader(uint64_t h) { fACCHeader = h; }
    void     SetACDCHeader(uint64_t h) { fACDCHeader = h; }
    void     SetTime320(uint64_t t) { fTime320 = t; }
    void     SetTimeWR(uint64_t t) { fTimeWR = t; }
    void     SetAlighnHeader(uint64_t h) { fAlignHeader = h; }
    void     SetData(uint64_t* d) { memcpy(fData,d,1440*sizeof(uint64_t)); }

  private:
    
    // 64b raw words - to be dropped eventually
    uint64_t fACCHeader; // 0x123456789abcde + ACDC number(8 bit)
    uint64_t fACDCHeader; // Fixed alignment pattern 0xac9c + event number (32 bit) + 8 bit padding (all 0)
    uint64_t fTime320; // 320 MHz counter
    uint64_t fTimeWR; // 32 bit 1 Hz WR counter + 32 bit 250 MHz WR counter
    uint64_t fAlignHeader; // 48 bit padding (all 0) + Fixed alignment pattern 0xcac9
    uint64_t fData[kN64BitDataWords]; // 12 bit ADC values for the 256 channels for all 30 channels of the ACDC

    // Decoded data
    uint8_t fACDCNumber; // ACDC number (8 bits)
    uint32_t fEventNumber; // Event number (32 bits)
    uint64_t fTimeStamp; // Timestamp (64 bits) - 320 MHz clock
    uint32_t fWRTimePPS; // White Rabbit time PPS counter
    uint32_t fWRTime250MHz; // White Rabbit time 250 MHz counter
    LAPPDReadoutChannelRawDigit fChannels[kNChannels]; // Readout channels for the 30 ACDC channels

    bool fIsNoise;

  };

} // end namespace lappd

#endif
