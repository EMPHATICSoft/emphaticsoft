////////////////////////////////////////////////////////////////////////
// \file    SRBACkovBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRBACKOVBRANCH_H
#define SRBACKOVBRANCH_H
#include "StandardRecord/SRBACkov.h"

#include <cstddef>
#include <limits>
#include <vector>
#include <cstddef>

namespace caf
{
  /// Reconstructed BACkov hits from each of the 6 PMTs
    class SRBACkovBranch
    {
    public:
      SRBACkovBranch();
      ~SRBACkovBranch();

      std::vector<SRBACkov>  backovhits;  ///< BACkovhits produced by BACkovHitReco
      std::size_t                 nbackovhits; ///< number of backovhits 
      bool                   PID[5];      ///< probability (0 or 1) of being {e,mu,pi,K,p}

      void fillSizes();
      
    };

} // end namespace

#endif // SRBACKOVBRANCH_H
///////////////////////////////////////////////////////////////////////////
