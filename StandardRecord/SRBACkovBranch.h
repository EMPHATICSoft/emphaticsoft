////////////////////////////////////////////////////////////////////////
// \file    SRBACkovBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRBACKOVBRANCH_H
#define SRBACKOVBRANCH_H
#include "StandardRecord/SRBACkov.h"

#include <vector>

namespace caf
{
  /// Reconstructed rings found by various algorithms
    class SRBACkovBranch
    {
    public:
      SRBACkovBranch();
      ~SRBACkovBranch();

      std::vector<SRBACkov>  backovhits;  ///< BACkovhits produced by BACkovHitReco
      size_t                nbackovhits; ///< number of backovhits 

      void fillSizes();
      
    };

} // end namespace

#endif // SRARINGBRANCH_H
///////////////////////////////////////////////////////////////////////////
