////////////////////////////////////////////////////////////////////////
// \file    SRGasCkovBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRGASCKOVBRANCH_H
#define SRGASCKOVBRANCH_H
#include "StandardRecord/SRGasCkov.h"

#include <limits>
#include <vector>
#include <cstddef>

namespace caf
{
  /// Reconstructed Gas Cherenkov hits from each of the 3 PMTs
    class SRGasCkovBranch
    {
    public:
      SRGasCkovBranch();
      ~SRGasCkovBranch();

      std::vector<SRGasCkov>  gasckovhits;  ///< GasCkovhits produced by GasCkovHitReco
      std::size_t                 ngasckovhits; ///< number of gasckovhits
      bool                   PID[5];      ///< probability (0 or 1) of being {e,mu,pi,K,p}

      void fillSizes();
      
    };

} // end namespace

#endif // SRGASCKOVBRANCH_H
///////////////////////////////////////////////////////////////////////////
