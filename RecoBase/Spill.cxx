////////////////////////////////////////////////////////////////////////
/// \brief   Spill class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/Spill.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  Spill::Spill() :   
    fTimestamp(0), fIntensity(0.), fMomentum(0.)
  {

  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const Spill& sp)
  {
    o << "Spill Unix Timestamp = " << sp.Timestamp()
      << ", Momentum = " << sp.Momentum()
      << ", Intensity = " << sp.Intensity();
    return o;
  }
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
