////////////////////////////////////////////////////////////////////////
// \file    ARICHFiller.cxx
// \brief   Class that does the work to extract arich ring info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/ARICHFiller.h"
#include "RecoBase/ARing.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void ARICHFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    art::Handle< std::vector <rb::ARing> > arv;

    evt.getByLabel(fLabel, arv);

    if(!fLabel.empty() && arv.failedToGet()) {
      return;
      //      std::cout << "CAFMaker: No product of type '"
      //		<< abi::__cxa_demangle(typeid(*arv).name(), 0, 0, 0)
      //		<< "' found under label '" << fLabel << "'. " << std::endl;
    }
      
    std::vector<rb::ARing> arings;
    if(!arv.failedToGet()) arings = *arv;

    for (unsigned int ringId = 0; ringId < arings.size(); ++ ringId) {
      stdrec.ring.arich.push_back(SRARing());
      SRARing& srARing = stdrec.ring.arich.back();

      srARing.nhit = arings[ringId].NHits();
    } // end for ringId

  }

} // end namespace caf
