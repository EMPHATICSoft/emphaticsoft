////////////////////////////////////////////////////////////////////////
/// \brief   Source file for LAPPDRawDigit class
/// \author  jpaley@fnal.gov, fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"
#include "LAPPDRawDigit.h"
#include <cstdint>
#include <iostream>

namespace lappd{

  //----------------------------------------------------------------------
  LAPPDReadoutChannelRawDigit::LAPPDReadoutChannelRawDigit():
    fChannel(0)
  {
    memset(fData, 0, kNTicks * sizeof(uint16_t)); // Initialize all ADC values to 0
  }

  LAPPDReadoutChannelRawDigit::LAPPDReadoutChannelRawDigit(uint8_t channel, uint16_t* data)
  {
    fChannel = channel; // Set the channel number
    for (int i = 0; i < kNTicks; ++i) {
      fData[i] = data[i]; // Copy the ADC data for each tick
    }
  }
    

  //----------------------------------------------------------------------
  LAPPDRawDigit::LAPPDRawDigit():
    fACCHeader(0), fACDCHeader(0), fTime320(0), fTimeWR(0), 
    fAlignHeader(0), fIsNoise(false)    
  {
    memset(fData,0,1440*sizeof(uint64_t));
  }
  
  //----------------------------------------------------------------------
  LAPPDRawDigit::LAPPDRawDigit(uint64_t* block) : 
    fIsNoise(false)
  {

    // Decode the first block: u56u8
    // 0x123456789abcde + ACDC number(8 bit) 
    fACCHeader = block[0];
    //std::cout << "LAPPDRawDigit ACC Header " << std::hex << fACCHeader << " " << (fACCHeader >> 8) << " " << (fACCHeader & 0xFF) << " " << std::dec << (fACCHeader & 0xFF) << std::endl;
    if( (fACCHeader>>8) != 0x123456789abcde){
      std::cerr << "LAPPDRawDigit: Error in ACC Header, expected 0x123456789abcde, got 0x" 
                << std::hex << (fACCHeader >> 8) << std::dec << std::endl;
      throw std::runtime_error("LAPPDRawDigit: Error in ACC Header");
    }
    fACDCNumber = (fACCHeader & 0xFF) & 0xFF; // Extract the ACDC number (8 bits)

    // Decode the second block: u16p48
    // fixed alignment pattern 0xac9c + event number (32 bit) + 8 bit padding (all 0)
    fACDCHeader = block[1];
    if( (fACDCHeader >> 48) != 0xac9c) {
      std::cerr << "LAPPDRawDigit: Error in ACDC Header, expected 0xac9c000000000000, got 0x" 
                << std::hex << fACDCHeader << std::dec << std::endl;
      throw std::runtime_error("LAPPDRawDigit: Error in ACDC Header");
    }
    fEventNumber = (fACDCHeader >> 16) & 0xFFFFFFFF; // 32 bits for event number?

    // Decode the third block: u64
    fTime320 = block[2];
    fTimeStamp = block[2];

    // Decode the fourth block: u32u32
    fTimeWR = block[3]; 
    fWRTimePPS = block[3] & 0xFFFFFFFF; // 32 bits for PPS counter
    fWRTime250MHz = (block[3] >> 32) & 0xFFFFFFFF; // 32 bits for 250 MHz WR counter

    // Decode the fifth block: p64
    fAlignHeader = block[4];

    // Decode the data blocks: u64[1440]
    int carryOverBits = 0; // Carry over bits from the previous 64-bit word
    uint16_t previousWordLeftover = 0; // Previous word leftover bits

    uint16_t dataChunk[kNTicks];
    short unsigned channel = 0;  // Channel index
    short unsigned int tick = 0;  // Tick index
    
    for(int i = 0; i < kN64BitDataWords; ++i) {

      // Error checking for channel and tick indices
      if (channel >= kNChannels) {  // If we have filled all channels, break
        std::cerr << "LAPPDRawDigit: Error - more than " 
                  << kNChannels << " channels found in data." << std::endl;
        throw std::runtime_error("LAPPDRawDigit: Error - more than 30 channels found in data.");
      }
      if (tick >= kNTicks) {  // If we have filled all ticks for the current channel, reset
        std::cerr << "LAPPDRawDigit: Error - more than " 
                  << kNTicks << " ticks found in data." << std::endl;
        throw std::runtime_error("LAPPDRawDigit: Error - more than 256 ticks found in data.");
      }

      // Process the current 64-bit word
      uint64_t value = block[5 + i];  // Get the current 64-bit value from the block
      
      // Get the first 12 bits from the current word and leftover bits from the previous word
      int bitIx = 12 - carryOverBits;  // Number of bits to process from the current word
      uint16_t thisWord = (value >> (64 - bitIx)) & 0xFFF;  // Extract the bits from the current word
      uint16_t adcValue = (previousWordLeftover << bitIx) | thisWord;  // Combine the bits
      dataChunk[tick] = adcValue;  // Store the ADC value for the current tick
      tick++;  // Move to the next tick
      
      // Get number of remaining full 12-bit chunks in the current word
      int nFullChunks = (64 - bitIx) / 12;  // Number of full 12-bit chunks that can be processed

      for (int j = 0; j < nFullChunks; ++j) {  // Process 4 sets of 12 bits in the current word
        uint16_t adcValue = (value >> (64 - bitIx - 12)) & 0xFFF;  // Extract the next 12 bits
        dataChunk[tick] = adcValue;  // Store the ADC value for the current tick
        tick++;  // Move to the next tick
        bitIx += 12;  // Move to the next 12 bits in the current word
      }
      
      // Store the leftover bits for the next iteration
      carryOverBits = (64 - bitIx);  // Calculate the number of bits left in the current word
      if (carryOverBits > 0) {  // If there are leftover bits in the current word
        previousWordLeftover = (value & ((1 << carryOverBits) - 1)) & 0xFFF;  // Store the leftover bits for the next iteration
      } else {
        previousWordLeftover = 0;  // Reset the leftover bits if there are no carry over bits
      }
      
      // Update channel number if we have filled all ticks for the current channel
      // and tore the data chunk for the current channel when we have filled all ticks
      if (tick >= kNTicks) {  // If we have filled all ticks for the current channel

        // Create a new readout channel raw digit for the current channel
        LAPPDReadoutChannelRawDigit channelRawDigit(channel, dataChunk);
        fChannels[channel] = channelRawDigit; 

        // Reset for the next channel
        tick = 0;  // Reset tick index for the next channel
        channel++;  // Move to the next channel
      }

    }
    
  }

  //----------------------------------------------------------------------
  LAPPDRawDigit::LAPPDRawDigit(uint64_t accH, uint64_t acdcH, uint64_t t320, uint64_t twr, uint64_t aH, uint64_t* data) :
    fACCHeader(accH), fACDCHeader(acdcH), fTime320(t320), 
    fTimeWR(twr), fAlignHeader(aH), fIsNoise(false)          
  {
    memcpy(fData,data,1440*sizeof(uint64_t));
  }

  //----------------------------------------------------------------------
  LAPPDReadoutChannelRawDigit LAPPDRawDigit::GetChannel(int i) const
  {
    if (i < 0 || i >= kNChannels) {
      throw std::out_of_range("LAPPDRawDigit::GetChannel: Channel index out of range");
    }
    return fChannels[i];
  }

} // end namespace rawdata

