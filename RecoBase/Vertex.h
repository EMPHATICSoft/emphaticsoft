////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Vertex
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "StandardRecord/SRVertex.h"

namespace rb {

  class Vertex : public caf::SRVertex {
  public:
    Vertex(); // Default constructor
    virtual ~Vertex() {}; //Destructor
    
    std::vector<size_t> trkIdx;
    
  };
  
}

#endif // VERTEX_H
