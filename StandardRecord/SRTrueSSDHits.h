////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHits.h
////////////////////////////////////////////////////////////////////////
#ifndef  SRTrueSSDHits_H
#define  SRTrueSSDHits_H

//#include "StandardRecord/SRVector3D.h" //don't think its needed for hits
#include "Math/Vector3D.h"

namespace caf
{
  ///SRTrueSSDHits is a simple descriptor for an SSDHit (not raw digit)
  class  SRTrueSSDHits
  {
  public:
    SRTrueSSDHits(); //Default constructor
    virtual ~ SRTrueSSDHits(); //Destructor
    
    ROOT::Math::XYZVector pos;
    ROOT::Math::XYZVector mom;

    double dE;
    int pid;
    
    int station;
    int plane;
    int sensor;
    int strip;
    int trackID;

    virtual void setDefault();
  };

} // end namespace

#endif // SRTrueSSDHits_H
