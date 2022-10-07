////////////////////////////////////////////////////////////////////////
/// \file PrimaryParticleInformation.cxx
//
/// \version $Id: PrimaryParticleInformation.cxx,v 1.5 2012-09-24 15:19:29 brebel Exp $
/// \author  seligman@nevis.columbia.edu, brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#include "G4Base/PrimaryParticleInformation.h"
#include "SimulationBase/MCTruth.h"

namespace g4b{

  //-------------------------------------------------
  G4Allocator<PrimaryParticleInformation> PrimaryParticleInformationAllocator;

  //-------------------------------------------------
  simb::MCParticle const* PrimaryParticleInformation::GetMCParticle() const
  {
    if (!IsInMCTruth()) return nullptr;
    auto const* truth = GetMCTruth();
    if (!truth) return nullptr;
    auto const index = MCParticleIndex();
    if (index >= (std::size_t) truth->NParticles()) return nullptr; // or should we throw?
    return &(truth->GetParticle(index));
  } // PrimaryParticleInformation::GetMCParticle()
  
  
  //-------------------------------------------------
  void PrimaryParticleInformation::Print() const
  {
    if ( fMCTruth )
      std::cout << *fMCTruth;
  }

}// namespace
