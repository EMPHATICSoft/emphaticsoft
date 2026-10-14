////////////////////////////////////////////////////////////////////////
/// \brief   Definition of ARICH ring
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef ARING_H
#define ARING_H

#include <vector>
#include <stdint.h>
#include <iostream>

//#include "StandardRecord/SRARing.h"


namespace rb {
  
  class ARing {
    public:
    ARing(); // Default constructor
    virtual ~ARing() {}; //Destructor
    
  private:

    float _center[3]; // rotation angle about the vertical y-axis
    float _radius;
    int _nhits;
    float theta_C;
    float P_pion;
    float P_kaon;
    float P_proton;
 

   
  public:
      float  XCenter() const { return _center[0]; }
    float  YCenter() const { return _center[1]; }
    float  ZCenter() const { return _center[2]; }
    const float* Center() const { return _center; }
    float  Radius() const { return _radius; }
    int    NHits() const { return _nhits; }
    float GetP_Pion() const {return P_pion;}
    float GetP_P_Kaon() const {return P_kaon;}
    float GetP_Proton() const {return P_proton;}
    float GetTheta() const {return theta_C;}

    void SetCenter(float x[3]) { for (int i=0; i<3; ++i) _center[i] = x[i]; }
    void SetRadius(float r) { _radius = r; }
    void SetNHits(int nh) { _nhits = nh; }
    void SetP_Pion (float proxy_p_pion){P_pion = proxy_p_pion;}
    void SetP_Kaon (float proxy_p_kaon){P_kaon = proxy_p_kaon;}
    void SetP_Proton (float proxy_p_proton){P_proton = proxy_p_proton;}
    void SetTheta (float theta_proxy){theta_C = theta_proxy;}
};

}
#endif // ARING_H
