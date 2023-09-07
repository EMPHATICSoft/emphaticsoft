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
#include <iterator>
#include <iostream>

namespace caf
{
  void TrueSSDHitsFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
  
    //  auto truehitv = evt.getHandle<std::vector <sim::SSDHit> >(fLabel);
    
    art::Handle< std::vector<sim::SSDHit> > truehitv;
    try {
      evt.getByLabel(fLabel, truehitv);
    }
    catch(...) {
      std::cout << "WARNING: No SSDHits found!" << std::endl;
    }

    //     if(!fLabel.empty() && truehitv.failedToGet()) {
    //if(!fSSDHitLabel.empty() && truehitv.failedToGet()) {      
    //std::cout << "CAFMaker: No product of type '"
    //		<< abi::__cxa_demangle(typeid(*truehitv).name(), 0, 0, 0)
    //		<< "' found under label '" << fLabel << "'. " << std::endl;
    //}
      
    //std::vector<sim::SSDHit> ssdhits;

    //if (ssdhits.empty()) {
    //std::cout << "ssdhits is empty." << std::endl;
    //} else {
    //std::cout << "ssdhits is not empty." << std::endl;
    //}

    // if(!truehitv.failedToGet()) ssdhits = *truehitv;

    for (unsigned int truehitId = 0; truehitId < truehitv->size(); ++truehitId) {

      const sim::SSDHit& ssdhits = (*truehitv)[truehitId];

      stdrec.truehits.truehits.push_back(SRTrueSSDHits());
      SRTrueSSDHits& srTrueSSDHits = stdrec.truehits.truehits.back();

      srTrueSSDHits.GetX = ssdhits.GetX();
      srTrueSSDHits.GetY = ssdhits.GetY();
      srTrueSSDHits.GetZ = ssdhits.GetZ();

      srTrueSSDHits.GetPx = ssdhits.GetPx();
      srTrueSSDHits.GetPy = ssdhits.GetPy();
      srTrueSSDHits.GetPz = ssdhits.GetPz();

      srTrueSSDHits.GetDE = ssdhits.GetDE();
      srTrueSSDHits.GetPId = ssdhits.GetPId();

      srTrueSSDHits.GetStation = ssdhits.GetStation();
      srTrueSSDHits.GetPlane = ssdhits.GetPlane();
      srTrueSSDHits.GetSensor = ssdhits.GetSensor();
      srTrueSSDHits.GetStrip = ssdhits.GetStrip();
      srTrueSSDHits.GetTrackID = ssdhits.GetTrackID();

    } // end for truehitId

  }

} // end namespace caf
