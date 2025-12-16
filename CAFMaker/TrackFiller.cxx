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
#include "RecoBase/RecoBaseDefs.h"
#include "RecoBase/ArichID.h"

namespace caf
{
  void TrackFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto htsv = evt.getHandle<std::vector<rb::Track> >(fLabelTracks);
    auto htar = evt.getHandle<std::vector<rb::ArichID> >(fLabelArichID);  //random name 
    
    std::vector <rb::Track> segs;
    std::vector <rb::ArichID> arichIDs;

    if(!htsv.failedToGet() && !htar.failedToGet()) {segs = *htsv; arichIDs = *htar;}
    //if(!htsv.failedToGet()) segs = *htsv;
    stdrec.trks.ntrk = segs.size();

    for (int c= 0; c< (int)segs.size();c++) {
      rb::Track p = segs[c];
      caf::SRTrack sp;	
      for (int i=0; i<3; ++i) sp.vtx[i] = p.Vtx()[i];
      sp.mom.SetXYZ(p.P()[0],p.P()[1],p.P()[2]);
	for (size_t i=0; i<p.NTrackSegments(); i++){     
        auto pts = p.GetTrackSegment(i);
        caf::SRTrackSegment srts;
        for (int i=0; i<3; ++i) srts.vtx[i] = pts->Vtx()[i];
        srts.mom.SetXYZ(pts->P()[0],pts->P()[1],pts->P()[2]);
        srts.region = pts->RegLabel();
        srts.nspacepoints = pts->NSpacePoints();
        for (int i=0; i<3; ++i){
          srts.pointA[i] = pts->A()[i];
          srts.pointB[i] = pts->B()[i];
 	}
        srts.chi2 = pts->Chi2();
	srts.thetaX = pts->ThetaX();
        srts.thetaY = pts->ThetaY();

        sp.Add(srts);	
	}
	stdrec.trks.trk.push_back(sp);	 
      }

      if(arichIDs.size() != 0){
         int pos = arichIDs[0].trackID;
	 stdrec.trks.trk[pos].arich.trackID = arichIDs[0].trackID;
         stdrec.trks.trk[pos].arich.scoresLL = arichIDs[0].scoresLL;
 	 stdrec.trks.trk[pos].arich.scoresML = arichIDs[0].scoresML;
	 stdrec.trks.trk[pos].arich.nhit =  arichIDs[0].nhit;
	  }
	
	
  }  
} // end namespace caf
