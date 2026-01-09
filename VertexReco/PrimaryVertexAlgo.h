////////////////////////////////////////////////////////////////////////
/// \brief   Class for primary vertex finding algorithm
/// \author  Jon Paley
/// \date    9/22/25
////////////////////////////////////////////////////////////////////////
#ifndef PRIMARYVERTEXALGO_H
#define PRIMARYVERTEXALGO_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "RecoBase/Vertex.h"
#include "RecoBase/Track.h"

namespace emph {

  class PVAlgo {
  public:
    PVAlgo(); // Default constructor
    virtual ~PVAlgo() {}; //Destructor

  public:
    // Define functions here
    bool FindVertexDOCA(std::vector<rb::Track> &trks, rb::Vertex& vtx);

  };
  
}

#endif // PRIMARYVERTEXALGO_H
