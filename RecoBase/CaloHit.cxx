////////////////////////////////////////////////////////////////////////
/// \brief   Calo hit class
/// \author  robert chirco
/// \date    6/21/23
/// \song    angels in tibet by amaarae
////////////////////////////////////////////////////////////////////////
#include "RecoBase/CaloHit.h"

#include <iomanip>
#include <iostream>
#include <cassert>

namespace rb {
  
  //----------------------------------------------------------------------
  
  CaloHit::CaloHit() :
    _channel(0)   
    //_angle(0.), _strip(0.), _pitch(60.)
  {

  }
  
  //------------------------------------------------------------

  CaloHit::CaloHit(const std::vector<uint16_t> &wfm, int detchan)
  {
  	_channel = detchan;
  }
  
  //------------------------------------------------------------

  std::ostream& operator<< (std::ostream& o, const CaloHit& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Channel = "     << std::setw(5) << std::right << h.Channel();
    return o;
  }

  //------------------------------------------------------------

 
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
