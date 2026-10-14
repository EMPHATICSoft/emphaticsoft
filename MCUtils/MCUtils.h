////////////////////////////////////////////////////////////////////////
/// \brief   Class for truth functions
/// \author  Jon Paley
/// \date    9/19/25
////////////////////////////////////////////////////////////////////////
#ifndef MCUTILS_H
#define MCUTILS_H

#include <vector>

#include "Simulation/SSDHit.h" 

namespace mcu {
  
  class MCUtils {
  public:
    MCUtils(); // Default constructor
    virtual ~MCUtils() {}; //Destructor
    
  public:
    double  findTruthAngle(std::vector<sim::SSDHit>& sim_i, std::vector<sim::SSDHit>& sim_f);
    void printSSDHits(const std::vector<sim::SSDHit>& sim_hits, bool ignoreLowEnergyElectronHits=true);
  };
  
}

#endif // MCUTILS_H
