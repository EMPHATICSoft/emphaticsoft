////////////////////////////////////////////////////////////////////////
// \file    SpacePointFiller.cxx
// \brief   Class that does the work to extract ssd raw digit info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/SpacePointFiller.h"
#include "StandardRecord/SRSpacePoint.h"
#include "RecoBase/SpacePoint.h"

namespace caf
{
  void SpacePointFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto hspv = evt.getHandle<std::vector<rb::SpacePoint> >(fLabel);
      
    std::vector <rb::SpacePoint> spcpts;
    if(!hspv.failedToGet()) spcpts = *hspv;
    
    stdrec.spcpts.nsp = spcpts.size();

    for (auto p : spcpts) {
      caf::SRSpacePoint sp;
      for (int i=0; i<3; ++i)
	sp.x[i] = p.Pos()[i];
      sp.station = p.Station();
      
      stdrec.spcpts.sp.push_back(sp);
   } // end for hitId
  }  
} // end namespace caf
