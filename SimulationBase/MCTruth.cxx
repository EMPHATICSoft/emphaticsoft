////////////////////////////////////////////////////////////////////////
/// \file  MCTruth.cxx
/// \brief Simple MC truth class, holds a vector of MCParticles
///
/// \version $Id: MCTruth.cxx,v 1.8 2012-10-15 20:36:27 brebel Exp $
/// \author  jpaley@indiana.edu
////////////////////////////////////////////////////////////////////////
#include "SimulationBase/MCTruth.h"

#include "canvas/Utilities/Exception.h"

#include "TDatabasePDG.h"

#include <iostream>
#include <cassert>

namespace simb{

  //......................................................................
  MCTruth::MCTruth() 
    : fPartList()
      //    , fBeam()
    , fOrigin(simb::kUnknown)
  { 
  }

  //......................................................................
  void MCTruth::SetBeam(simb::MCParticle& beam)
  {
    assert(fPartList.empty());
    fPartList.push_back(beam);
  }

  //......................................................................
  std::ostream& operator<< (std::ostream& o, simb::MCTruth const& a)
  {
    for (int i = 0; i < a.NParticles(); ++i)
      o << i << " " << a.GetParticle(i) << std::endl;

    return o;
  }
}
////////////////////////////////////////////////////////////////////////
