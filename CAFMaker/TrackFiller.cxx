////////////////////////////////////////////////////////////////////////
// \file    TrackFiller.cxx
// \brief   Class that does the work to extract reco'd tracks from
//          the art event and add them to the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/TrackFiller.h"
#include "StandardRecord/SRTrack.h"
#include "RecoBase/Track.h"
#include "StandardRecord/SRTrackSegment.h"

namespace caf
{
  void TrackFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto htsv = evt.getHandle<std::vector<rb::Track> >(fLabel);
      
    std::vector <rb::Track> segs;
    if(!htsv.failedToGet()) segs = *htsv;
    
    stdrec.trks.ntrk = segs.size();

    for (auto p : segs) {
      caf::SRTrack sp;
      for (int i=0; i<3; ++i) 
	sp.vtx[i] = p.Vtx()[i];
      sp.mom.SetXYZ(p.P()[0],p.P()[1],p.P()[2]);

      for (size_t i=0; i<p.NTrackSegments(); i++){     
        auto pts = p.GetTrackSegment(i);
	caf::SRTrackSegment srts;
	for (int i=0; i<3; ++i)
          srts.vtx[i] = pts->Vtx()[i];
        srts.mom.SetXYZ(pts->P()[0],pts->P()[1],pts->P()[2]);
        srts.label = pts->Label();
	srts.nspacepoints = pts->NSpacePoints();
	
	sp.Add(srts);
	
      }
      stdrec.trks.trk.push_back(sp);
   } // end of loop over tracks
  }  
} // end namespace caf
