////////////////////////////////////////////////////////////////////////
/// \brief   RawTrigger class
/// \author  j
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/RawSumDropMB.h"

#include <iostream>
#include <string>

namespace rawdata{
  
  //----------------------------------------------------------------------
  
  RawSumDropMB::RawSumDropMB() : fSumTrig_NumDroppedMicroblocks(0)
			       ,fSumTrig_TimeStamp(0)
			       ,fSumTrig_TimeStampLow(0)
			       ,fSumTrig_TimeStampHigh(0)
			       ,fSumTrig_BuffId(0)
  {
  }
  
  //----------------------------------------------------------------------
  
  RawSumDropMB::~RawSumDropMB(){ }
      
} // end namespace rawdata
////////////////////////////////////////////////////////////////////////
