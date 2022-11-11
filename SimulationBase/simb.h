/**
 * @file   SimulationBase/simb.h
 * @brief  Common type definitions for data products (and a bit beyond).
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   April 6, 2018
 */

#ifndef NUSIMDATA_SIMULATIONBASE_SIMB_H
#define NUSIMDATA_SIMULATIONBASE_SIMB_H

// C++ standard library
#include <limits> // std::numeric_limits<>


/// Base utilities and modules for event generation and detector simulation.
namespace simb {

  //--- BEGIN Generator index --------------------------------------------------
  /// @{
  /**
   * @name Generated particle index
   * 
   * This type represents the position of a specific generated particle in the
   * generator truth record, `simb::MCTruth`.
   * The particle itself can be accessed from that record via
   * `simb::MCTruth::GetParticle()`.
   */
  
  /// Type of particle index in the generator truth record (`simb::MCTruth`).
  using GeneratedParticleIndex_t = std::size_t;
  
  /// Constant representing the absence of generator truth information.
  constexpr GeneratedParticleIndex_t NoGeneratedParticleIndex
    = std::numeric_limits<GeneratedParticleIndex_t>::max();
  
  /// Returns whether the specified one is an acceptable generator index.
  inline bool isGeneratedParticleIndex(GeneratedParticleIndex_t index)
    { return index != NoGeneratedParticleIndex; }
  
  /// @}
  //--- END Generator index ----------------------------------------------------
  
  
} // namespace simb



#endif // NUSIMDATA_SIMULATIONBASE_SIMB_H
