////////////////////////////////////////////////////////////////////////
// \file     VertexFiller.h
// \note    Class to fill CAF  Vertex info
////////////////////////////////////////////////////////////////////////
#ifndef  VERTEXFILLER_H
#define  VERTEXFILLER_H

#include "CAFMaker/FillerBase.h"
#include "Simulation/SSDHit.h"
#include "StandardRecord/SRVertex.h"
#include "RecoBase/Track.h"

namespace caf
{
  /// Class to fill header information
  class VertexFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);
    //void Fill(art::Event&, caf::StandardRecord& sr1,  caf::StandardRecord& sr2);
    void GetBeamTrackTruth(caf::SRBeamTrack&, const std::vector<sim::SSDHit> &);
    caf::SRSecondaryTrack GetSecondaryTrack(rb::Track&, const std::vector<sim::SSDHit> &);
    std::string fVertexLabel;
    std::string fTrackLabel;
    std::string fArichIDLabel;
    std::string fSSDHitLabel;
  };

} // end namespace

#endif //  VERTEXFILLER_H
