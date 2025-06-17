////////////////////////////////////////////////////////////////////////
// \file    SRARingBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRARINGBRANCH_H
#define SRARINGBRANCH_H
#include "StandardRecord/SRARing.h"

#include <cstddef>
#include <vector>

namespace caf
{
  /// Reconstructed rings found by various algorithms
    class SRARingBranch
    {
    public:
      SRARingBranch();
      ~SRARingBranch();

      std::vector<SRARing>  arich;  ///< ARings produced by ARichReco
      size_t                narich; ///< number of arings 

      void fillSizes();
      
    };

} // end namespace

#endif // SRARINGBRANCH_H
///////////////////////////////////////////////////////////////////////////
