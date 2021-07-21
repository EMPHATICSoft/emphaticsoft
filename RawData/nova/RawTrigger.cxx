////////////////////////////////////////////////////////////////////////
/// \brief   RawTrigger class
/// \author  jpaley@indiana.edu
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/RawTrigger.h"

#include <iostream>
#include <string>

namespace rawdata{
  
  //----------------------------------------------------------------------
  
  RawTrigger::RawTrigger() : fTriggerHeader_Source(0),
			     fTriggerHeader_SourceID(0),
			     fTriggerHeader_SourceSubID(0),
			     fTriggerHeader_TriggerNumber(0),
			     fTriggerHeader_MasterTriggerNumber(0),
			     fTriggerTime_GenTime(0),
			     fTriggerTime_ClockSource(0),
			     fTriggerTimingMarker_TimeStart(0),
			     fTriggerTimingMarker_ExtractionStart(0),
			     fTriggerRange_TriggerLength(0),
			     fTriggerMask_MCBit(false),
			     fTriggerMask_TMod(0),
			     fTriggerMask_TriggerType(0),
			     fTriggerMask_Prescale(0)
  {
  }
  
  //----------------------------------------------------------------------
  
  RawTrigger::~RawTrigger(){ }
  
  //----------------------------------------------------------------------
  ///
  /// \brief Return just the lower 32 bits of the timing marker. This is the
  ///        reference "t0" for the RawDigit TDC counters. 
  ///
  /// \todo There may be some issues with roll-overs with this simple
  ///       treatment.
  ///  
  unsigned long long RawTrigger::TDCT0() const 
  {
    unsigned long long mask = 0xFFFFFFFF;
    return fTriggerTimingMarker_TimeStart&mask;
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const RawTrigger& rt)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << "TDCT0 = "                  << std::setw(5) << std::right << rt.TDCT0()
      << " Trigger Header Source = " << std::setw(5) << std::right << rt.fTriggerHeader_Source
      << " Trigger Mask Type = "     << std::setw(5) << std::right << rt.fTriggerMask_TriggerType
      << " Trigger Length = "        << std::setw(5) << std::right << rt.fTriggerRange_TriggerLength
      << " Trigger Time Start = "    << std::setw(5) << std::right << rt.fTriggerTimingMarker_TimeStart;
    
    return o;
  }
  
} // end namespace rawdata
////////////////////////////////////////////////////////////////////////
