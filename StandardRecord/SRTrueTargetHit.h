////////////////////////////////////////////////////////////////////////
// \file    SRTrueTargetHits.h
////////////////////////////////////////////////////////////////////////
#ifndef  SRTrueTargetHits_H
#define  SRTrueTargetHits_H

//#include "StandardRecord/SRVector3D.h" //don't think its needed for hits
#include "Math/Vector3D.h"

namespace caf
{
  ///SRTrueTargetHits is a simple descriptor for a target hit (not raw digit)
  class  SRTrueTargetHit
  {
  public:
    SRTrueTargetHit(); //Default constructor
    virtual ~ SRTrueTargetHit(); //Destructor
    
    ROOT::Math::XYZVector pos_pre;
    ROOT::Math::XYZVector pos_post; 
    ROOT::Math::XYZVector mom_pre;
    ROOT::Math::XYZVector mom_post;

    double dE;
    int pid;
    int trackID;
    std::string process;
    
    virtual void setDefault();
  };

} // end namespace

#endif // SRTrueTargetHits_H
