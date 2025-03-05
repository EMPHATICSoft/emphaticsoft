#ifndef LAPPDRAWDIGIT_H
#define LAPPDRAWDIGIT_H

#include <vector>
#include <iostream>
#include <inttypes.h>
#include <cstring>

namespace emph {

  namespace rawdata {
    class LAPPDRawDigit {
    public:
      LAPPDRawDigit(); // Default constructor
      LAPPDRawDigit(uint64_t* block);
      LAPPDRawDigit(uint64_t accH, uint64_t acdcH, uint64_t t320, uint64_t twr, uint64_t aH, uint64_t* data);
      virtual ~LAPPDRawDigit() {}; // Destructor

      uint64_t GetACCHeader() const {return fACCHeader; }
      uint64_t GetACDCHeader() const {return fACDCHeader; }
      uint64_t GetTime320() const {return fTime320; }
      uint64_t GetTimeWR() const {return fTimeWR; }
      uint64_t GetAlignHeader() const {return fAlignHeader; }
      const uint64_t* GetData() const {return fData; }
      bool IsNoise() const {return fIsNoise; }

      void     SetACCHeader(uint64_t h) { fACCHeader = h; }
      void     SetACDCHeader(uint64_t h) { fACDCHeader = h; }
      void     SetTime320(uint64_t t) { fTime320 = t; }
      void     SetTimeWR(uint64_t t) { fTimeWR = t; }
      void     SetAlighnHeader(uint64_t h) { fAlignHeader = h; }
      void     SetData(uint64_t* d) { memcpy(fData,d,1440*16); }

    private:
      /// Found fields to hold the header, measurement error words.
      uint64_t fACCHeader; // 0x123456789abcde + ACDC number(8 bit)
      uint64_t fACDCHeader; // Fixed alignment pattern 0xac9c + event number (32 bit) + 8 bit padding (all 0)
      uint64_t fTime320; // 320 MHz counter
      uint64_t fTimeWR; // 32 bit 1 Hz WR counter + 32 bit 250 MHz WR counter
      uint64_t fAlignHeader; // 48 bit padding (all 0) + Fixed alignment pattern 0xcac9
      uint64_t fData[1440]; // 12 bit ADC values for the 256 channels for all 30 channels of the ACDC
      bool fIsNoise;    

      /* 
      inline friend std::ostream& operator<<(std::ostream& os, const LAPPDRawDigit& RawDig)
      {
	os << "Raw dig: (" << RawDig.GetBoardId() << ", " << RawDig.fdetChan  << ", " <<  RawDig.fHitTime << ", " <<  RawDig.fIsNoise << ")" << std::endl; 
	return os;
	}
      */
 
    };

  } // end namespace rawdata
} // end namespace emph
#endif
