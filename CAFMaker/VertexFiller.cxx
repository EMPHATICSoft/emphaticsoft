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

    std::vector <rb::Vertex> vtxs;
    std::vector <rb::Track> trks;

    if ( !hv.failedToGet()) vtxs = *hv;
    if ( !ht.failedToGet()) trks = *ht;
    std::cout << "vtxs.size() = " << vtxs.size() << std::endl;
    std::cout << "trks.size() = " << trks.size() << std::endl;

    stdrec.vtxs.nvtx = vtxs.size();

    // loop over vertices
    for (int iv= 0; iv< (int)vtxs.size();iv++) {
      rb::Vertex v = vtxs[iv];
      caf::SRVertex srv = v;

      // loop over tracks in vertex
      for (size_t it=0; it < v.trkIdx.size(); ++it) {
	caf::SRTrack srt = trks[v.trkIdx[it]];
	srv.Add(srt);
      }
      stdrec.vtxs.vtx.push_back(srv);
    } // end of loop over vertexs
  }  
} // end namespace caf
