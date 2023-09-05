////////////////////////////////////////////////////////////////////////
// \file    TrueSSDHitsFiller.cxx
// \brief   Class that does the work to extract SSDHit info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/TrueSSDHitsFiller.h"
#include "RecoBase/ARing.h"
#include "RawData/SSDRawDigit.h"
#include "Simulation/SSDHit.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void TrueSSDHitsFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    std::string fSSDHitLabel = "ssdhits";
    auto truehitv = evt.getHandle<std::vector<sim::SSDHit> >(fSSDHitLabel);

    //    art::Handle< std::vector <sim::SSDHit> > truehitv;
    // evt.getByLabel(fLabel, truessdhitv);

    // if(!fLabel.empty() && truehitv.failedToGet()) {
    if(!fSSDHitLabel.empty() && truehitv.failedToGet()) {      
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*truehitv).name(), 0, 0, 0)
		<< "' found under label '" << fSSDHitLabel << "'. " << std::endl;
    }
      
    std::vector<sim::SSDHit> truessdhits;
    if(!truehitv.failedToGet()) truessdhits = *truehitv;

    for (unsigned int truehitId = 0; truehitId < truessdhits.size(); ++ truehitId) {
      stdrec.truehits.truehits.push_back(SRTrueSSDHits());
      SRTrueSSDHits& srTrueSSDHits = stdrec.truehits.truehits.back();

      srTrueSSDHits.GetX = truessdhits[truehitId].GetX();
      srTrueSSDHits.GetY = truessdhits[truehitId].GetY();
      srTrueSSDHits.GetZ = truessdhits[truehitId].GetZ();

      srTrueSSDHits.GetPx = truessdhits[truehitId].GetPx();
      srTrueSSDHits.GetPy = truessdhits[truehitId].GetPy();
      srTrueSSDHits.GetPz = truessdhits[truehitId].GetPz();

      srTrueSSDHits.GetDE = truessdhits[truehitId].GetDE();
      srTrueSSDHits.GetPId = truessdhits[truehitId].GetPId();

      srTrueSSDHits.GetStation = truessdhits[truehitId].GetStation();
      srTrueSSDHits.GetPlane = truessdhits[truehitId].GetPlane();
      srTrueSSDHits.GetSensor = truessdhits[truehitId].GetSensor();
      srTrueSSDHits.GetStrip = truessdhits[truehitId].GetStrip();
      srTrueSSDHits.GetTrackID = truessdhits[truehitId].GetTrackID();

    } // end for truehitId

  }

} // end namespace caf
