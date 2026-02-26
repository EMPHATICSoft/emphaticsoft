////////////////////////////////////////////////////////////////////////
// \file    SRVertexBranch.cxx
// \brief   An SRVertexBranch contains vectors of SRVertexs.
//          It is intended for use in the Common Analysis ROOT trees.
////////////////////////////////////////////////////////////////////////

#include "StandardRecord/SRVertexBranch.h"

namespace caf
{
  SRVertexBranch::SRVertexBranch():
    nvtx(-1)
  {
    vtx.clear();
  }

  //--------------------------------------------------------------------
  SRVertexBranch::~SRVertexBranch()
  {
  }

  //--------------------------------------------------------------------
  void SRVertexBranch::fillSizes()
  {
    nvtx = vtx.size();
  }

} // end namespace caf
////////////////////////////////////////////////////////////////////////
