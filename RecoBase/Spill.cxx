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
    fTimestamp(0), fIntensity(0.), fMomentum(0.),
    fMT5CPR(0.), fMT6CPR(0.), fMT5CP2(0.), fMT6CP2(0.),
    fTBCCDI(0.), fTBCCDO(0.), fMT6CA1(0.), fMT6CA2(0.), fMT6CA3(0.)
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
