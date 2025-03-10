////////////////////////////////////////////////////////////////////////
// \file    TrackSegmentFiller.cxx
// \brief   Class that does the work to extract reco'd track segments from
//          the art event and add them to the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/TrackSegmentFiller.h"
#include "StandardRecord/SRTrackSegment.h"
#include "RecoBase/TrackSegment.h"

namespace caf
{
  void TrackSegmentFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto htsv = evt.getHandle<std::vector<rb::TrackSegment> >(fLabel);
      
    std::vector <rb::TrackSegment> segs;
    if(!htsv.failedToGet()) segs = *htsv;
    
    stdrec.sgmnts.nseg = segs.size();

    for (auto p : segs) {
      caf::SRTrackSegment sp;
      for (int i=0; i<3; ++i) 
	sp.vtx[i] = p.Vtx()[i];
      sp.mom.SetXYZ(p.P()[0],p.P()[1],p.P()[2]);
 
      sp.label = p.Label();
      sp.nspacepoints = p.NSpacePoints();
      stdrec.sgmnts.seg.push_back(sp);    
    } // end of loop over track segments
  }  
} // end namespace caf
