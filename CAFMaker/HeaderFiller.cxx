////////////////////////////////////////////////////////////////////////
// \file    HeaderFiller.cxx
// \brief   Class that does the work to extract header info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/HeaderFiller.h"
#include "RecoBase/Spill.h"

namespace caf
{
  void HeaderFiller::Fill(art::SubRun& sr, caf::SRHeader& hdr)
  {
    // Get metadata information for header
    unsigned int run = sr.run();
    unsigned int subrun = sr.subRun();
    
    hdr.run    = -1;
    hdr.subrun = -1;

    art::Handle<rb::Spill> spillHandle;
    try {
      sr.getByLabel("spillinfo",spillHandle);
      hdr.timestamp = spillHandle->Timestamp();
      hdr.intensity = spillHandle->Intensity();
      hdr.momentum  = spillHandle->Momentum();
      hdr.gcp5      = spillHandle->MT5CPR();
      hdr.gcp6      = spillHandle->MT6CPR();
      hdr.gcp5iFix  = spillHandle->MT5CP2();
      hdr.gcp5iFix  = spillHandle->MT6CP2();
      hdr.tbccdi    = spillHandle->TBCCDI();
      hdr.tbccdo    = spillHandle->TBCCDO();
      hdr.mt6ca1    = spillHandle->MT6CA1();
      hdr.mt6ca2    = spillHandle->MT6CA2();
      hdr.mt6ca3    = spillHandle->MT6CA3();

      hdr.run    = run;
      hdr.subrun = subrun;
      
    }
    catch(...) {
      std::cout << "No spill info object found!  That's ok if this is MC" 
		<< std::endl;
    }

  }

} // end namespace caf
