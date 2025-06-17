////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHitsBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTrueSSDHitsBRANCH_H
#define SRTrueSSDHitsBRANCH_H
#include "StandardRecord/SRTrueSSDHits.h"

#include <cstddef>
#include <vector>

namespace caf
{
    class SRTrueSSDHitsBranch
    {
    public:
      SRTrueSSDHitsBranch(); //Default constructor
      ~SRTrueSSDHitsBranch(); //Destructor

      std::vector<SRTrueSSDHits>  truehits;
      size_t                ntruehits; 

      void fillSizes();
      
    };

} // end namespace

#endif // SRTrueSSDHitsBRANCH_H
///////////////////////////////////////////////////////////////////////////
