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
      std::cout<<"HERE   1"<<std::endl;
    art::Handle< std::vector <rb::BACkovHit> > bhitv;

    evt.getByLabel(fLabel, bhitv);

    if(!fLabel.empty() && bhitv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*bhitv).name(), 0, 0, 0)
		<< "' found under label '" << fLabel << "'. " << std::endl;
    }
      
    std::vector<rb::BACkovHit> backovhits;
    if(!bhitv.failedToGet()) backovhits = *bhitv;
    std::cout<<backovhits.size()<<std::endl;

    for (unsigned int bhitId = 0; bhitId < backovhits.size(); ++bhitId) {
      stdrec.backov.backovhits.push_back(SRBACkov());

      //Fill all 6 PMT channels with charge values
      for (int i=0; i<6; ++i){
          SRBACkov& srBACkov = stdrec.backov.backovhits[i];
          srBACkov.charge = backovhits[bhitId].ChargeChan(i);
          srBACkov.time = backovhits[bhitId].TimeChan(i);
      }
      //Fill 5 PID possibilities
     stdrec.backov.PID[0] = backovhits[bhitId].IsElectron(); 
     stdrec.backov.PID[1] = backovhits[bhitId].IsMuon(); 
     stdrec.backov.PID[2] = backovhits[bhitId].IsPion(); 
     stdrec.backov.PID[3] = backovhits[bhitId].IsKaon(); 
     stdrec.backov.PID[4] = backovhits[bhitId].IsProton(); 
    } 

  }

} // end namespace caf
