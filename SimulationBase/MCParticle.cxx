////////////////////////////////////////////////////////////////////////
/// \file  MCParticle.cxx
/// \brief Description of a particle passed to Geant4
///
/// \version $Id: MCParticle.cxx,v 1.12 2012-11-20 17:39:38 brebel Exp $
/// \author  seligman@nevis.columbia.edu
////////////////////////////////////////////////////////////////////////
#include "SimulationBase/MCParticle.h"

#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TLorentzVector.h>
#include <TVector3.h>

#include <iterator>
#include <iostream>
#include <climits>

namespace simb {


  const int MCParticle::s_uninitialized = std::numeric_limits<int>::min();

  //...................................................................
  MCParticle::MCParticle()
  {
  }
  
  //...................................................................
  /// Standard constructor.
  MCParticle::MCParticle(const int trackId, 
			 const int pdg,
			 const std::string process,
			 const int mother,
			 const double mass,
			 const int status)
    : caf::SRParticle(trackId, pdg, process, mother, mass, status)
  {
  }

  //...................................................................
  MCParticle::~MCParticle() 
  {
  }

}

