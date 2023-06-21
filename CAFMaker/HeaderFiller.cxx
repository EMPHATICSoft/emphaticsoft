////////////////////////////////////////////////////////////////////////
// \file    HeaderFiller.cxx
// \brief   Class that does the work to extract header info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/HeaderFiller.h"

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

  }

} // end namespace caf
