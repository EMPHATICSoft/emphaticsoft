////////////////////////////////////////////////////////////////////////
// \file    GasCkovFiller.cxx
// \brief   Class that does the work to extract Gas Cherenkov info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/GasCkovFiller.h"
#include "RecoBase/GasCkovHit.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void GasCkovFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    art::Handle< std::vector <rb::GasCkovHit> > gchitv;

    evt.getByLabel(fLabel, gchitv);

    if(!fLabel.empty() && gchitv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*gchitv).name(), 0, 0, 0)
		<< "' found under label '" << fLabel << "'. " << std::endl;
    }
      
    std::vector<rb::GasCkovHit> gasckovhits;
    if(!gchitv.failedToGet()) gasckovhits = *gchitv;

    for (unsigned int gchitId = 0; gchitId < gasckovhits.size(); ++gchitId) {
        //Fill all 6 PMT channels with charge values
        for (int i=0; i<3; ++i){
            stdrec.gasckov.gasckovhits.push_back(SRGasCkov());
            SRGasCkov& srGasCkov = stdrec.gasckov.gasckovhits[i];
            srGasCkov.charge = gasckovhits[gchitId].ChargeChan(i);
            srGasCkov.time = gasckovhits[gchitId].TimeChan(i);
        }
        //Fill 5 PID possibilities
        stdrec.gasckov.PID[0] = gasckovhits[gchitId].IsElectron(); 
        stdrec.gasckov.PID[1] = gasckovhits[gchitId].IsMuon(); 
        stdrec.gasckov.PID[2] = gasckovhits[gchitId].IsPion(); 
        stdrec.gasckov.PID[3] = gasckovhits[gchitId].IsKaon(); 
        stdrec.gasckov.PID[4] = gasckovhits[gchitId].IsProton(); 
    } 

  }

} // end namespace caf
