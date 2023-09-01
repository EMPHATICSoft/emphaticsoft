////////////////////////////////////////////////////////////////////////
/// \file  MCTrajectory.h
/// \version $Id: MCTrajectory.h,v 1.6 2012-11-01 19:18:11 brebel Exp $
/// \brief Trajectory class
///
/// \author  seligman@nevis.columbia.edu, changes by jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////

/// This class describes the trajectory of a particle created in the
/// Monte Carlo simulation.  /// There are no units defined in this class.  If it's used with
/// Geant4, the units will be (mm,ns,GeV), but this class does not
/// enforce this.

#ifndef SIMB_MCTRAJECTORY_H
#define SIMB_MCTRAJECTORY_H

#include <vector>
#include <iostream>

#include <TLorentzVector.h>
#include "StandardRecord/SRTrajectory.h"

namespace simb{
  class MCTrajectory: public caf::SRTrajectory {
  public:
    MCTrajectory();
    MCTrajectory(const TLorentzVector& position, const TLorentzVector& momentum); 
    ~MCTrajectory(){std::cerr << "Destructing simb::MCTrajectory\n";};		
  };
}

#endif // SIMB_MCTRAJECTORY_H
