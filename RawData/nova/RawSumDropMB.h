/////////////////////////////
/// \author  atsaris@fnal.gov
/// \date
////////////////////////////

#ifndef RAWSUMDROPMB_H
#define RAWSUMDROPMB_H

#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace rawdata {
  
  class RawSumDropMB
  {
  public:
    RawSumDropMB(); // Default constructor
    ~RawSumDropMB();
        
  public:
    uint32_t           fSumTrig_NumDroppedMicroblocks;
    uint64_t           fSumTrig_TimeStamp;
    uint32_t           fSumTrig_TimeStampLow;
    uint32_t           fSumTrig_TimeStampHigh;
    uint16_t           fSumTrig_BuffId;
    
  private:
    
  };
}

#endif // RAWSUMDROPMB_H
///////////////////////////////////////////////////////////////////////////////////////////
