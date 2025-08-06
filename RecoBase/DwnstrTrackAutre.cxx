////////////////////////////////////////////////////////////////////////
/// \brief   Downstream Track segment class
/// \author  jpaley@fnal.gov, lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/DwnstrTrackAutre.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <climits>
#include <cfloat>

namespace rb {
  
  //----------------------------------------------------------------------
  
  DwnstrTrackAutre::DwnstrTrackAutre() : 
    fTrType (rb::TRDWNNONE), fId(INT_MAX), fUserFlag(INT_MAX), 
   fTrXOffset(DBL_MAX), fTrYOffset(DBL_MAX), fTrXSlope(DBL_MAX), fTrYSlope(DBL_MAX), fTrMom(120.),  
   fTrXOffsetErr(DBL_MAX), fTrYOffsetErr(DBL_MAX), fTrXSlopeErr(DBL_MAX), fTrYSlopeErr(DBL_MAX),
   fTrMomErr(DBL_MAX),
   fCovXY(25, DBL_MAX), fChiSq(DBL_MAX), fChiSqSts(DBL_MAX), fChiSqKlmX(DBL_MAX), fChiSqKlmY(DBL_MAX) 
  {
   ;
  }
  //----------------------------------------------------------------------
  
  std::ostream& operator<< (std::ostream& o, const DwnstrTrackAutre& h)
  {
    auto aPrec = o.precision();
    auto aFlags = o.flags();
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Downstream Track --> x0, y0 " << h.XOffset() << " +- " << h.XOffsetErr() 
                           << " ," << h.YOffset() << " +- " << h.YOffsetErr() << "  slopes "  
                           << h.XSlope() << " +- " << h.XSlopeErr() << " " << h.YSlope()  
			   << " +- " << h.YSlopeErr()  << std::endl;
    o << " Downstream Track fitted momentum [GeV] " << h.Momentum() << " +- " << h.MomentumErr() << std::endl; 
    o.flags(aFlags); o.precision(aPrec);
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
