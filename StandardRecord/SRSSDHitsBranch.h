////////////////////////////////////////////////////////////////////////
// \file    SRSSDHitsBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRSSDHitsBRANCH_H
#define SRSSDHitsBRANCH_H
#include "StandardRecord/SRSSDHits.h"

#include <vector>
#include <cstddef>

namespace caf
{
  /// SSD hits branch
    class SRSSDHitsBranch
    {
    public:
      SRSSDHitsBranch();// Default constructor
      ~SRSSDHitsBranch(); //Destructor

      std::vector<SRSSDHits>  hits;
      size_t                nhits;

      void fillSizes();
      
    };

} // end namespace

#endif // SRSSDHitsBRANCH_H
///////////////////////////////////////////////////////////////////////////
