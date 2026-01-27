////////////////////////////////////////////////////////////////////////
// \file    SRVertexBranch.h
////////////////////////////////////////////////////////////////////////
#ifndef SRVERTEXBRANCH_H
#define SRVERTEXBRANCH_H
#include "StandardRecord/SRVertex.h"

#include <vector>

namespace caf
{
  /// Reconstructed rings found by various algorithms
    class SRVertexBranch
    {
    public:
      SRVertexBranch();
      ~SRVertexBranch();

      std::vector<SRVertex> vtx; 
      size_t               nvtx;     

      void fillSizes();
      
    };

} // end namespace

#endif // SRVERTEXBRANCH_H
///////////////////////////////////////////////////////////////////////////
