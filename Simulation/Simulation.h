////////////////////////////////////////////////////////////////////////
/// \brief  Simulation header file
/// \author brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef SIM_H
#define SIM_H

/// Data classes required for detector simulation
namespace sim
{ 
  unsigned int GetRandomNumberSeed();
  
  static const int kNoiseId          = 1234567890; ///< flag for noise id
  static const int kNoParticleId     = -999;       ///< if any track id method returns     
  ///< sim::kNoParticleId, it means the    
  ///< associated particle was too low-energy    
  ///< to be written by the detector Monte Carlo.
  static const int kPrimaryParticle  = 0;       ///< enumeration to indicate the mother     
  ///< of a particle is a primary particle       

}

#endif
/////////////////////////////////////////////////////////////////////////
