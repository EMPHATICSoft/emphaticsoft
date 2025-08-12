////////////////////////////////////////////////////////////////////////
/// \brief   Track segment class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/BeamTrack.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <climits>
#include <cfloat>

namespace rb {
  
  //----------------------------------------------------------------------
  
  BeamTrack::BeamTrack() : 
    fTrType (rb::NONE), fUserFlag(INT_MAX), 
   fTrXOffset(DBL_MAX), fTrYOffset(DBL_MAX), fTrXSlope(DBL_MAX), fTrYSlope(DBL_MAX),  
   fTrXOffsetErr(DBL_MAX), fTrYOffsetErr(DBL_MAX), fTrXSlopeErr(DBL_MAX), fTrYSlopeErr(DBL_MAX),
   fTrXCovOffSl(DBL_MAX), fTrYCovOffSl(DBL_MAX), fChiSqX(DBL_MAX), fChiSqY(DBL_MAX), fMomentum(DBL_MAX), fMomentumErr(DBL_MAX)
  {
   ;
  }
  void BeamTrack::Reset() {
    
   fTrType = rb::NONE; 
   fTrXOffset = DBL_MAX; fTrYOffset = DBL_MAX; fTrXSlope = DBL_MAX; fTrYSlope = DBL_MAX;  
   fTrXOffsetErr = DBL_MAX; fTrYOffsetErr = DBL_MAX; fTrXSlopeErr = DBL_MAX; fTrYSlopeErr = DBL_MAX;
   fTrXCovOffSl = DBL_MAX; fTrYCovOffSl = DBL_MAX; fChiSqX = DBL_MAX; fChiSqY = DBL_MAX;
   fMomentum = DBL_MAX; fMomentumErr = DBL_MAX;
  
  }
  //----------------------------------------------------------------------
  
  std::ostream& operator<< (std::ostream& o, const BeamTrack& h)
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
