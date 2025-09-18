////////////////////////////////////////////////////////////////////////
// \file    VertexFiller.cxx
// \brief   Class that does the work to extract reco'd vertexs from
//          the art event and add them to the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/VertexFiller.h"
#include "StandardRecord/SRVertex.h"
#include "RecoBase/Vertex.h"
#include "RecoBase/Track.h"
//#include "StandardRecord/SRTrackSegment.h"
#include "RecoBase/RecoBaseDefs.h"
//#include "RecoBase/ArichID.h"

namespace caf
{
  void VertexFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto hv = evt.getHandle<std::vector<rb::Vertex> >(fLabelVertices);
    auto ht = evt.getHandle<std::vector<rb::Track> >(fLabelTracks);
//    auto htar = evt.getHandle<std::vector<rb::ArichID> >(fLabelArichID);  //random name 
    std::vector <rb::Vertex> vtxs;
    std::vector <rb::Track> trks;
//    std::vector <rb::ArichID> arichIDs;

//    if(!htsv.failedToGet()) {segs = *htsv; arichIDs = *htar;}
    if ( !hv.failedToGet()) vtxs = *hv;
    if ( !ht.failedToGet()) trks = *ht;
    stdrec.vtxs.nvtx = vtxs.size();

    // loop over vertices
    for (int iv= 0; iv< (int)vtxs.size();iv++) {
      rb::Vertex v = vtxs[iv];
      caf::SRVertex srv;
      srv.pos[0] = v.X();
      srv.pos[1] = v.Y();
      srv.pos[2] = v.Z();
      // loop over tracks in vertex
      for (size_t it=0; it < v.NumDwnstr(); ++it) {
	rb::Track t = trks[v.TrackUID(it)];
	caf::SRTrack srt;
	srt.mom.SetXYZ(t.P()[0],t.P()[1],t.P()[2]);
	for (int j=0; j<3; ++j)
	  srt.vtx[j] = t.Vtx()[j];
	for (size_t i=0; i<t.NTrackSegments(); i++){     
	  auto pts = t.GetTrackSegment(i);
	  caf::SRTrackSegment srts;
	  for (int i=0; i<3; ++i) srts.vtx[i] = pts->Vtx()[i];
	  srts.mom.SetXYZ(pts->P()[0],pts->P()[1],pts->P()[2]);
	  srts.region = pts->RegLabel();
	  srts.nspacepoints = pts->NSpacePoints();
	  srt.Add(srts);
	}
	srv.Add(srt);
/*
	if(arichIDs.size() != 0){
        sp.arich.vertexID = arichIDs[c].vertexID;
        sp.arich.scores = arichIDs[c].scores;
        sp.arich.nhit =  arichIDs[c].nhit;
	}
*/
      }
      stdrec.vtxs.vtx.push_back(srv);
    } // end of loop over vertexs
  }  
} // end namespace caf
