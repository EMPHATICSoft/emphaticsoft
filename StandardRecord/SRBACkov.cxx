////////////////////////////////////////////////////////////////////////
// \file    SRBACkov.cxx
// \brief   TODO
////////////////////////////////////////////////////////////////////////
#include <limits>

#include "StandardRecord/SRBACkov.h"

namespace caf
{
  SRBACkov::SRBACkov() :
    charge{std::numeric_limits<float>::signaling_NaN(),
            std::numeric_limits<float>::signaling_NaN(),
            std::numeric_limits<float>::signaling_NaN(),
            std::numeric_limits<float>::signaling_NaN(),
            std::numeric_limits<float>::signaling_NaN(),
            std::numeric_limits<float>::signaling_NaN()},
    PID{std::numeric_limits<bool>::signaling_NaN(),
            std::numeric_limits<bool>::signaling_NaN(),
            std::numeric_limits<bool>::signaling_NaN(),
            std::numeric_limits<bool>::signaling_NaN(),
            std::numeric_limits<bool>::signaling_NaN()}
  {
  }

  //--------------------------------------------------------------------
  SRBACkov::~SRBACkov()
  {
  }

  //--------------------------------------------------------------------
  void SRBACkov::setDefault()
  {
    memset(charge, 0, sizeof(charge));
    memset(PID, 0, sizeof(PID));
  }

} // end namespace
