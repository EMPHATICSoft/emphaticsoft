////////////////////////////////////////////////////////////////////////
// \file     VertexFiller.h
// \note    Class to fill CAF  Vertex info
////////////////////////////////////////////////////////////////////////
#ifndef  VERTEXFILLER_H
#define  VERTEXFILLER_H

//#include "CAFMaker/FillerBase.h"
#include "art/Framework/Principal/Event.h"
#include "StandardRecord/StandardRecord.h"
#include "StandardRecord/SRVertex.h"
#include "Simulation/SSDHit.h"
#include "RecoBase/Track.h"

namespace caf
{
  /// Class to fill header information
  class VertexFiller //: public FillerBase
  {
  public:

    void FillSimple(art::Event&, std::vector<caf::SRVertex>&);
    void FillKalman(art::Event&, std::vector<caf::SRVertex>&);
    //void Fill(art::Event&, caf::StandardRecord& sr1,  caf::StandardRecord& sr2);
    void GetBeamTrackTruth(caf::SRBeamTrack&, rb::Track&, const std::vector<sim::SSDHit> &);
    void GetSecondaryTrackTruth(caf::SRSecondaryTrack&, rb::Track&, const std::vector<sim::SSDHit> &);
    std::string fVertexLabel;
    std::string fTrackLabel;
    std::string fKVertexLabel;
    std::string fKTrackLabel;
    std::string fBeamTrackLabel;
    std::string fArichIDLabel;
    std::string fSSDHitLabel;
  };

} // end namespace

#endif //  VERTEXFILLER_H
