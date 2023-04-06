////////////////////////////////////////////////////////////////////////
/// \brief   Track segment class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/BeamTrackAlgo1.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <climits>
#include <cfloat>

namespace rb {
  
  //----------------------------------------------------------------------
  
  BeamTrackAlgo1::BeamTrackAlgo1() : 
    fTrType (rb::NONE), 
   fTrXOffset(DBL_MAX), fTrYOffset(DBL_MAX), fTrXSlope(DBL_MAX), fTrYSlope(DBL_MAX),  
   fTrXOffsetErr(DBL_MAX), fTrYOffsetErr(DBL_MAX), fTrXSlopeErr(DBL_MAX), fTrYSlopeErr(DBL_MAX),
   fTrXCovOffSl(DBL_MAX), fTrYCovOffSl(DBL_MAX), fChiSqX(DBL_MAX), fChiSqY(DBL_MAX)
  {
   ;
  }
  void BeamTrackAlgo1::Reset() {
    
   fTrType = rb::NONE; 
   fTrXOffset = DBL_MAX; fTrYOffset = DBL_MAX; fTrXSlope = DBL_MAX; fTrYSlope = DBL_MAX;  
   fTrXOffsetErr = DBL_MAX; fTrYOffsetErr = DBL_MAX; fTrXSlopeErr = DBL_MAX; fTrYSlopeErr = DBL_MAX;
   fTrXCovOffSl = DBL_MAX; fTrYCovOffSl = DBL_MAX; fChiSqX = DBL_MAX; fChiSqY = DBL_MAX;
  
  }
  //----------------------------------------------------------------------
  
  std::ostream& operator<< (std::ostream& o, const BeamTrackAlgo1& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Beam Track --> x0, y0 " << h.XOffset() << " +- " << h.XOffsetErr() 
                           << " ," << h.YOffset() << " +- " << h.YOffsetErr() << "  slopes "  
                           << h.XSlope() << " +- " << h.XSlopeErr() << " " << h.YSlope()  
			   << " +- " << h.YSlopeErr()  << std::endl;
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
