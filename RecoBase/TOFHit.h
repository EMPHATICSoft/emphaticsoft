////////////////////////////////////////////////////////////////////////
/// \brief   Summary of the Trigger Counter data, developed in ../TOF
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef TOFHIT_H
#define TOFHIT_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>
#include "RecoBase/TriggerHit.h"
#include "RecoBase/T0SegmentHit.h"
#include "RecoBase/RPCStripHit.h"

namespace rb {
  
  class TOFHit {
  public: 
    explicit TOFHit(const TriggerHit &trigHit);
    
  private:
    TriggerHit _theTrigHit; // There is only one counter, unique thing. 
    std::vector<T0SegmentHit> _T0SegmentHits;   
    std::vector<RPCStripHit> _RPCStripHits; 
    
  public:
//
// Filling it up.. 
//  
    inline void addT0SegmentHit(const T0SegmentHit &aHit) { _T0SegmentHits.push_back(aHit); }
    inline void addRPCStripHit(const RPCStripHit &aHit) { _RPCStripHits.push_back(aHit); }
//
// Getter.. by const_iterator. and size
//
    inline const TriggerHit* TrigHit() const { return &_theTrigHit; }
    inline size_t NumT0SegmentHits () const {return _T0SegmentHits.size(); }
    inline size_t NumRPCStripHits () const {return _RPCStripHits.size(); }
    inline std::vector<T0SegmentHit>::const_iterator cbeginT0() const { return _T0SegmentHits.cbegin(); } 
    inline std::vector<T0SegmentHit>::const_iterator cendT0() const { return _T0SegmentHits.cend(); } 
    inline std::vector<RPCStripHit>::const_iterator cbeginRPC() const  { return _RPCStripHits.cbegin(); } 
    inline std::vector<RPCStripHit>::const_iterator cendRPC() const { return _RPCStripHits.cend(); } 
    
    friend std::ostream& operator << (std::ostream& o, const rb::TOFHit& r);
  
  };
  
} // namespace 
  
#endif // TOFHIT_H
