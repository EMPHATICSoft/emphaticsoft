////////////////////////////////////////////////////////////////////////
/// \brief   TriggerHit
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <iomanip>

#include "RecoBase/TOFHit.h"

  namespace rb {
    TOFHit::TOFHit(const TriggerHit &trigHit):
    _theTrigHit(trigHit)
    {
     ;
    } 
    std::ostream& operator<< (std::ostream& o, const  TOFHit& r)
       {
        auto prevFlags = o.flags(); auto prevPrecision = o.precision();
        o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        o << " Dump from emph::rb::TOFHit " << std::endl;
        o <<  *(r.TrigHit());
	if (r.NumT0SegmentHits() == 0 ) o << " No T0 Segment hits... " << std::endl;
	else {
	  for (std::vector<T0SegmentHit>::const_iterator it = r.cbeginT0(); it != r.cendT0(); it++) {
	    o << *it;
	  }
	}
	if (r.NumRPCStripHits() == 0 ) o << " No T0 Segment hits... " << std::endl;
	else {
	  for (std::vector<RPCStripHit>::const_iterator it = r.cbeginRPC(); it != r.cendRPC(); it++) {
	    o << (*it);
	  }
	}
	o << " -------------------  End TOFHit ------------------------------ " << std::endl << std::endl;
        o.setf(prevFlags); o.precision(prevPrecision);
        return o;
       }
    
  }  
