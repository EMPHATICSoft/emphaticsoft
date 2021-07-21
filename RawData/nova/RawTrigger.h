/////////////////////////////////////////////////////////////////////////////////////////
/// \brief   Definition of basic raw Trigger information.
///          See definition of all trigger fields in the
///          DAQDataFormats documentation, NOvA-DocDB-4390 in the section "Trigger Formats".
/// \author  denis@fnal.gov
/// \date
/////////////////////////////////////////////////////////////////////////////////////////
#ifndef RAWTRIGGER_H
#define RAWTRIGGER_H

#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace rawdata {
  
  class RawTrigger 
  {
  public:
    RawTrigger(); // Default constructor
    ~RawTrigger();
    
    // using unsigned long long instead of uint64_t to ensure the same
    // behavior with OSX and Linux
    unsigned long long TDCT0() const;
    
  public:
    uint8_t            fTriggerHeader_Source;	
    uint16_t 	       fTriggerHeader_SourceID;	
    uint32_t  	       fTriggerHeader_SourceSubID;
    unsigned long long fTriggerHeader_TriggerNumber;
    unsigned long long fTriggerHeader_MasterTriggerNumber;
    unsigned long long fTriggerTime_GenTime;
    uint8_t            fTriggerTime_ClockSource;
    unsigned long long fTriggerTimingMarker_TimeStart;
    unsigned long long fTriggerTimingMarker_ExtractionStart;
    uint32_t           fTriggerRange_TriggerLength;
    bool     	       fTriggerMask_MCBit;	 
    uint8_t  	       fTriggerMask_TMod;		 
    uint8_t  	       fTriggerMask_TriggerType;	 
    uint32_t 	       fTriggerMask_Prescale;      
    
    friend std::ostream& operator << (std::ostream& o, const RawTrigger& rt);
    
  private:
    
  };
}

#endif // RAWTRIGGER_H
///////////////////////////////////////////////////////////////////////////////////////////
