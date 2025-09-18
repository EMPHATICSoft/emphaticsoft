////////////////////////////////////////////////////////////////////////
// \file     VertexFiller.h
// \note    Class to fill CAF  Vertex info
////////////////////////////////////////////////////////////////////////
#ifndef  VERTEXFILLER_H
#define  VERTEXFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class VertexFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);
    //void Fill(art::Event&, caf::StandardRecord& sr1,  caf::StandardRecord& sr2);
    std::string fLabelVertices;
    std::string fLabelTracks;
    std::string fLabelArichID;
  };

} // end namespace

#endif //  VERTEXFILLER_H
