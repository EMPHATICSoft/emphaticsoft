////////////////////////////////////////////////////////////////////////
/// \brief   The final stage of the reconstruction: The point of interaction. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/Vertex.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <climits>
#include <cfloat>

namespace rb {
  
  //----------------------------------------------------------------------
  
  Vertex::Vertex() : 
   fId(INT_MAX), fX(DBL_MAX), fY(DBL_MAX), fZ(DBL_MAX), 
   fXErr(DBL_MAX), fYErr(DBL_MAX), fZErr(DBL_MAX), 
   fChiSq(DBL_MAX)
  {
    for (int i=0; i<3; ++i)
      for (int j=0; j<3; ++j)
	fCovXYZ[i][j] = DBL_MAX;
  }
  //----------------------------------------------------------------------
  
  std::ostream& operator<< (std::ostream& o, const Vertex& h)
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
  
} // end namespace rbex
//////////////////////////////////////////////////////////////////////////////
