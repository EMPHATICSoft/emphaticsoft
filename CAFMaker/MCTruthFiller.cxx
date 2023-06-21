////////////////////////////////////////////////////////////////////////
// \file    MCTruthFiller.cxx
// \brief   Class that does the work to extract MCTruth ring info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/MCTruthFiller.h"
#include "SimulationBase/MCTruth.h"
#include "SimulationBase/MCParticle.h"
//#include "art/Framework/Principle/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void MCTruthFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    // get beam particle at this handle
    art::Handle<std::vector<simb::MCParticle>> beam;
    evt.getByLabel("generator", beam);

    // make sure there is only one beam particle
    assert(beam.size() == 1);
    simb::MCParticle b = beam->at(0);    
    /*
    float pbeam[3];
    int beampid;
    std::string trgt;
    */

   // now create MCTruth
   //    simb::MCTruth mctru;
   // mctru.SetBeam(b);

   // assign the truth member of rec with a default constructor  
    stdrec.truth = caf::SRTruth();

   // assign beam position, momentum
    auto beamPos = b.Position();
    auto beamMom = b.Momentum();
    std::cout << "%%%%% Beam Position = (" << beamPos[0] << "," <<
      beamPos[1] << "," << beamPos[2] << ")" << std::endl;
   
   // give those beam positions, momenta to the corresponding leafs 
    for (int i=0; i<3; ++i) {
      stdrec.truth.xbeam[i] = beamPos[i];
      stdrec.truth.pbeam[i] = beamMom[i];
    } // end for loop
  }

} // end namespace caf
