////////////////////////////////////////////////////////////////////////
/// \brief   Definition summary info for the trigger counter and the T0 data. 
///          It is intended to characterize the eventprior to tracking, a "single track" event quality
///          To be filled in ../TOF/TrigToT0Prod_module.    
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TRIGTOT0_H
#define TRIGTOT0_H

#include <stdint.h>
#include <iostream>


namespace rb {
  
  class TrigToT0 {
  public:
    TrigToT0();
    TrigToT0(int nPeakTrig, double sumPMTTrig, int numSegT0, 
                      int segT0, double sumSigUpT0, double sumSigDownT0);
    virtual ~TrigToT0() {}; //Destructor
    
  public:

    int    NPeakTrig()  const { return fnPeakTrig; } // The number of peaks found in the trigger wave forms. Should be 4 
    double SumPMTTrig() const { return fsumPMTTrig; } // The sum of the trigger signal. 
    int NumSegT0() const { return fnumSegT0; } // The number of T0 segments with a unipolar signal above some threshold. 
    					       //  Should be 1 for good evets. 
    int SegT0() const { return fsegT0; } // The T0 segment number with the good unipolar signal.  
    double SumSigUpT0() const { return fsumSigUpT0; }  // The integrated ADC count of the up segments for the selected segment number. 
    double SumSigDownT0() const { return fsumSigDownT0; } // Same, but for the down segment. 
    
    bool isPrettyGood (double trigAmplCut[2], double T0AmplCut[2]) const; //  Very simple requirements, one Trig signal, one T0, amplitude cuts. 
    
    friend std::ostream& operator << (std::ostream& o, const TrigToT0& h);
    
  private:

    int fnPeakTrig; //Should be one, if all 4 PMTs declare on and only one peaks. 
    double fsumPMTTrig;
    int fnumSegT0;
    int fsegT0;
    double fsumSigUpT0;
    double fsumSigDownT0;
    
  };
  
}

#endif //TRIGTOT0_H 
