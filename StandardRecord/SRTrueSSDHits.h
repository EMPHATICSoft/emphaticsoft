////////////////////////////////////////////////////////////////////////
// \file    SRTrueSSDHits.h
////////////////////////////////////////////////////////////////////////
#ifndef  SRTrueSSDHits_H
#define  SRTrueSSDHits_H

//#include "StandardRecord/SRVector3D.h" //don't think its needed for hits

namespace caf
{
  ///SRTrueSSDHits is a simple descriptor for an SSDHit (not raw digit)
  class  SRTrueSSDHits
  {
  public:
    SRTrueSSDHits(); //Default constructor
    virtual ~ SRTrueSSDHits(); //Destructor
    
    double GetX;
    double GetY;
    double GetZ;
    
    double GetPx;
    double GetPy;
    double GetPz;

    double GetDE;
    int GetPId;
    
    int GetStation;
    int GetPlane;
    int GetSensor;
    int GetStrip;
    int GetTrackID;

    virtual void setDefault();
  };

} // end namespace

#endif // SRTrueSSDHits_H
