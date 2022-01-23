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
    
    int fBaseline;
    int fBLWidth;
    size_t fPeakPos;
    
    bool fIsNeg;      ///< flag to mark polarity of the pulse
    bool fIsMC;       ///< flag to mark data is simulation

  public:
    WaveForm(int channel, int board, std::vector<uint16_t> adclist,
	     uint32_t tstart);
    
    // Getters (although data members are public)    
    size_t   NADC()          const { return fADC.size();       }
    uint16_t ADC(size_t i=0) const; 
    uint32_t TStart()        const { return fTstart; }
    int      Channel()       const { return fChannel;          }
    int      Board()         const { return fBoard; }
    
    /// \brief Best estimator of the hit charge
    ///
    /// The DCS value appropriate for this readout version
    int        Baseline(int adcOffset=0, int nhits=10)  ;
    int        BLWidth(int adcOffset=0, int nhist=10) ;
    int        PeakADC() ;
    int        PeakTDC() ;
    int        PeakWidth() ;
    int        IntegratedADC() ;
    
    // Setters
    void       SetNegative() { fIsNeg = true;}
    void       SetPositive() { fIsNeg = false; }
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
