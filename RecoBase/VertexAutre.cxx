////////////////////////////////////////////////////////////////////////
/// \brief   The final stage of the reconstruction: The point of interaction. 
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/VertexAutre.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <climits>
#include <cfloat>

namespace rb {
  
  //----------------------------------------------------------------------
  
  VertexAutre::VertexAutre() : 
   fId(INT_MAX), fX(DBL_MAX), fY(DBL_MAX), fZ(DBL_MAX), 
   fXErr(DBL_MAX), fYErr(DBL_MAX), fZErr(DBL_MAX), 
   fCovXYZ(9, DBL_MAX), fChiSq(DBL_MAX)
  {
   ;
  }
  //----------------------------------------------------------------------
  
  std::ostream& operator<< (std::ostream& o, const VertexAutre& h)
  {
    auto aPrec = o.precision();
    auto aFlags = o.flags();
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " Vertex --> x0, y0, z0 " << h.X() << " +- " << h.XErr() 
                           << " ," << h.Y() << " +- " << h.YErr()
			   << " ," << h.Z() << " +- " << h.ZErr() << 
    o.flags(aFlags); o.precision(aPrec);
    return o;
  }
  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
