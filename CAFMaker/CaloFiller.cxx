////////////////////////////////////////////////////////////////////////
// \file    CaloFiller.cxx
// \brief   Class that does the work to extract calorimeter info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/CaloFiller.h"
#include "RecoBase/CaloHit.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void CaloFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    art::Handle< std::vector <rb::CaloHit> > chv;

    evt.getByLabel(fLabel, chv);

    if(!fLabel.empty() && chv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*chv).name(), 0, 0, 0)
		<< "' found under label '" << fLabel << "'. " << std::endl;
    }
      
    std::vector<rb::CaloHit> calohits;
    if(!chv.failedToGet()) calohits = *chv;

    for (unsigned int caloId = 0; caloId < calohits.size(); ++ caloId) {
      stdrec.calo.calohit.push_back(SRCaloHit());
      SRCaloHit& srCaloHit = stdrec.calo.calohit.back();

      srCaloHit.channel = calohits[caloId].Channel();
      srCaloHit.time = calohits[caloId].Time();
      srCaloHit.intchg = calohits[caloId].IntCharge();
    } // end for caloId

  }

} // end namespace caf
