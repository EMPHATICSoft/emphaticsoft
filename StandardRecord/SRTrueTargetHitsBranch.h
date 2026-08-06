////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHitsBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRTrueTargetHitsBRANCH_H
#define SRTrueTargetHitsBRANCH_H
#include "StandardRecord/SRTrueTargetHit.h"

#include <cstddef>
#include <vector>
#include <cstddef>

namespace caf
{
    class SRTrueTargetHitsBranch
    {
    public:
      SRTrueTargetHitsBranch(); //Default constructor
      ~SRTrueTargetHitsBranch(); //Destructor

      std::vector<SRTrueTargetHit>  truehits;
      size_t                ntruehits; 

      void fillSizes();
      
    };

} // end namespace

#endif // SRTrueSSDHitsBRANCH_H
///////////////////////////////////////////////////////////////////////////
