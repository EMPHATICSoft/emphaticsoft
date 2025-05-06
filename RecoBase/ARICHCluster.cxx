////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD cluster [of hits].  Note, this class
///          is intended to only group SSD hits on the same sensor, so 
///          they should all have the same "angle".
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/ARICHCluster.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>

namespace rb {
  
  //----------------------------------------------------------------------
  
  ARICHCluster::ARICHCluster() :
    fID(-1)
  {
  }

  //----------------------------------------------------------------------
  
  void ARICHCluster::Add(std::pair<int,int> hit)
  {
     fHits.push_back(hit);
 }
  //------------------------------------------------------------
 std::pair<int,int> ARICHCluster::Digit(unsigned int idx) 
  {
    assert(idx < fHits.size());

    return fHits[idx];

  }
  //------------------------------------------------------------
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
