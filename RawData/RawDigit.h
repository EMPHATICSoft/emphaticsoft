////////////////////////////////////////////////////////////////////////
/// \brief   Definition of basic raw digits, vectors of TDC and ADC
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef RAWDIGIT_H
#define RAWDIGIT_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace rawdata {
  
  class RawDigit {
  public:
    RawDigit(); // Default constructor
    virtual ~RawDigit() {}; //Destructor
    
  private:
    
    std::vector<int16_t> fADC;        ///< list of ADC(-like) charge values
    std::vector<int64_t> fTDC;        ///< TDC(-like) time value. 
    uint8_t   fVersion;   ///< version     
    uint32_t  fChannel;    ///< channel ID
    bool      fIsMC;       ///< flag to mark data is simulation

  public:
    RawDigit(uint32_t channel,
	     std::vector<int16_t> adclist,
	     int32_t tdc);
    
    // Getters (although data members are public)    
    uint32_t   NADC()          const { return fADC.size();       }
    int16_t    ADC(uint32_t i) const; 
    
    uint32_t   Channel()       const { return fChannel;          }
    uint8_t    Version()       const { return fVersion;          }
    
    /// \brief Best estimator of the hit charge
    ///
    /// The DCS value appropriate for this readout version
    int16_t    ADC()           const { return fADC[0];           }
    /// The time of the last baseline sample
    int32_t    TDC()           const { return fTDC[0];           }

    // Setters
    void       SetADC(uint32_t i, int16_t iADC);
    void       SetTDC(int32_t iTDC);
    void       SetChannel(uint32_t    iChan) { fChannel    = iChan;      }
    void       SetVersion(uint8_t v) { fVersion = v; }

    // Below here is stuff related to MC hits.
    // Unpack some of the information from fMCBits

    void     SetMC(bool isMC=true) { fIsMC = isMC; }
    bool     IsRealData() const    { return !this->IsMC();     }
    bool     IsMC()       const    { return fIsMC; }
    
    friend std::ostream& operator << (std::ostream& o, const RawDigit& r);
  };
  
}

#endif // RAWDIGIT_H
