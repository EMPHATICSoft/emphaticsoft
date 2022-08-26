////////////////////////////////////////////////////////////////////////
/// \brief   SpillData class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/SpillData.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>

namespace emph {
namespace rawdata{
  
  //----------------------------------------------------------------------
  
  SpillData::SpillData() :   
    fMomentum(0.)
    , fPolarity(1)
    , fIntensity(0.)
  {
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const SpillData& r)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Momentum = "     << std::setw(5) << std::right << r.Polarity()*r.Momentum()
      << " Intensity = "     << std::setw(5) << std::right << (int)r.Intensity();
    return o;
  }
  
} // end namespace rawdata
} // end namespace emph
  
//////////////////////////////////////////////////////////////////////////////
