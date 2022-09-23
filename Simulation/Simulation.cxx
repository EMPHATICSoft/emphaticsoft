////////////////////////////////////////////////////////////////////////
/// \brief   Useful tools for simulation
/// \author  brebel@fnal.gov
/// \date
///////////////////////////////////////////////////////////////////////
#include "Simulation/Simulation.h"

#include "TRandom3.h"

namespace sim
{
  //................................................................
  unsigned int GetRandomNumberSeed()
  {
    // the maximum allowed seed for the art::RandomNumberGenerator
    // is 900000000. Use TRandom3 to get the seed value in that range.
    // Instantiating TRandom3 with a 0 means that its seed is set based
    // on the TUUID and should always be random, even for jobs running on the
    // same machine
    TRandom3 *rand = new TRandom3(0);
    
    // set gRandom to be our new TRandom3 for cases where we need to pull
    // random numbers out of ROOT histograms, etc
    gRandom = rand;
    return rand->Integer(900000000);
  }
  
} // end namespace sim
////////////////////////////////////////////////////////////////////////
