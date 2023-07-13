////////////////////////////////////////////////////////////////////////
// \file    HeaderFiller.cxx
// \brief   Class that does the work to extract header info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/HeaderFiller.h"
#include "RecoBase/Spill.h"

namespace caf
{
  void HeaderFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    // Get metadata information for header
    unsigned int run = evt.run();
    unsigned int subrun = evt.subRun();
    unsigned int spillNum = evt.id().event();
    
    stdrec.hdr = SRHeader();

    stdrec.hdr.run    = run;
    stdrec.hdr.subrun = subrun;
    stdrec.hdr.evt    = spillNum;

    art::Handle<rb::Spill> spillHandle;
    try {
      evt.getByLabel("spillinfo",spillHandle);
      stdrec.hdr.timestamp = spillHandle->Timestamp();
      stdrec.hdr.intensity = spillHandle->Intensity();
      stdrec.hdr.momentum  = spillHandle->Momentum();
      stdrec.hdr.gcp5      = spillHandle->MT5CPR();
      stdrec.hdr.gcp6      = spillHandle->MT6CPR();
      stdrec.hdr.gcp5iFix  = spillHandle->MT5CP2();
      stdrec.hdr.gcp5iFix  = spillHandle->MT6CP2();
      stdrec.hdr.tbccdi    = spillHandle->TBCCDI();
      stdrec.hdr.tbccdo    = spillHandle->TBCCDO();
      stdrec.hdr.mt6ca1    = spillHandle->MT6CA1();
      stdrec.hdr.mt6ca2    = spillHandle->MT6CA2();
      stdrec.hdr.mt6ca3    = spillHandle->MT6CA3();
    }
    catch(...) {
      std::cout << "No spill info object found!  That's ok if this is MC" 
		<< std::endl;
    }

  }

} // end namespace caf
