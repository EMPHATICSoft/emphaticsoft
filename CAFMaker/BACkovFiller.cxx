////////////////////////////////////////////////////////////////////////
// \file    BACkovFiller.cxx
// \brief   Class that does the work to extract BAckov info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/BACkovFiller.h"
#include "RecoBase/BACkovHit.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void BACkovFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    art::Handle< std::vector <rb::BACkovHit> > bhitv;

    evt.getByLabel(fLabel, bhitv);

    if(!fLabel.empty() && bhitv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*bhitv).name(), 0, 0, 0)
		<< "' found under label '" << fLabel << "'. " << std::endl;
    }
      
    std::vector<rb::BACkovHit> backovhits;
    if(!bhitv.failedToGet()) backovhits = *bhitv;

    for (unsigned int bhitId = 0; bhitId < backovhits.size(); ++ bhitId) {
      stdrec.backov.backovhits.push_back(SRBACkov());
      SRBACkov& srBACkov = stdrec.backov.backovhits.back();

      //Fill all 6 PMT channels with charge values
      for (int i=0; i<6; ++i){
          srBACkov.charge[i] = backovhits[bhitId].ChargeChan(i);
      }
      //Fill 5 PID possibilities
     srBACkov.PID[0] = backovhits[bhitId].IsElectron(); 
     srBACkov.PID[1] = backovhits[bhitId].IsMuon(); 
     srBACkov.PID[2] = backovhits[bhitId].IsPion(); 
     srBACkov.PID[3] = backovhits[bhitId].IsKaon(); 
     srBACkov.PID[4] = backovhits[bhitId].IsProton(); 

    } // end for ringId

  }

} // end namespace caf
