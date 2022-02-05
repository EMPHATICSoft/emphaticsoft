////////////////////////////////////////////////////////////////////////
/// \brief   Definition of basic raw data waveform (eg, from CAEN V1720s)
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef RAWWAVEFORM_H
#define RAWWAVEFORM_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace emph { 
  namespace rawdata {
  
    class WaveForm {
    public:
      WaveForm(); // Default constructor
      ~WaveForm() {}; //Destructor
    
  private:
    
    std::vector<uint16_t> fADC;        ///< list of ADC(-like) charge values
    uint32_t fTstart;
    
    int fChannel;    ///< channel ID
    int fBoard;      ///< board ID
        
    bool fIsMC;       ///< flag to mark data is simulation

    uint64_t fFragmentTimestamp;
    
  public:
    WaveForm(int channel, int board, std::vector<uint16_t> adclist,
	     uint32_t tstart, uint64_t fragTS);
    
    // Getters (although data members are public)    
    size_t   NADC()          const { return fADC.size();       }
    uint16_t ADC(size_t i=0) const; 
    uint32_t TStart()        const { return fTstart; }
    int      Channel()       const { return fChannel;          }
    int      Board()         const { return fBoard; }
    uint64_t FragmentTime()  const { return fFragmentTimestamp; }
    std::vector<uint16_t> AllADC() const { return fADC; }
    
    /// \brief Best estimator of the hit charge
    ///
    /// The DCS value appropriate for this readout version
    float        Baseline(int adcOffset=0, int nhits=10) const;
    float        BLWidth(int adcOffset=0, int nhist=10)  const;
    int          PeakADC(bool isNegative=true)           const;
    int          PeakTDC(bool isNegative=true)           const;
    int          PeakWidth(bool isNegative=true)         const;
    float        IntegratedADC(int x1=0, int nsamp=40)   const;
    
    // Setters
    void       SetChannel(uint32_t    iChan) { fChannel    = iChan;      }

    // Below here is stuff related to MC hits.
    // Unpack some of the information from fMCBits

    void     SetMC(bool isMC=true) { fIsMC = isMC; }
    bool     IsRealData() const    { return !this->IsMC();     }
    bool     IsMC()       const    { return fIsMC; }
    
    friend std::ostream& operator << (std::ostream& o, const WaveForm& r);
  };
  
  }
}

#endif // RAWWAVEFORM_H
