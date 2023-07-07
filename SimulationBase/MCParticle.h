////////////////////////////////////////////////////////////////////////
/// \file  MCParticle.h
/// \brief Particle class
/// \version $Id: MCParticle.h,v 1.16 2012-11-20 17:39:38 brebel Exp $
/// \author  brebel@fnal.gov, changes by jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class describes a particle created in the detector Monte
/// Carlo simulation.

#ifndef SIMB_MCPARTICLE_H
#define SIMB_MCPARTICLE_H

#include "StandardRecord/SRParticle.h"
#include <set>
#include <string>
#include <iostream>
#include "TVector3.h"
#include "TLorentzVector.h"

namespace simb {

  class MCParticle: public caf::SRParticle  {
  public:
	static const int s_uninitialized;
	
	MCParticle();

	MCParticle(const int trackId, const int pdg,
	     const std::string process,
	     const int mother = -1,
	     const double mass = s_uninitialized,
	     const int status = 1);
    
    	~MCParticle();
	
		
  };
}
#endif // SIMB_MCPARTICLE_H
