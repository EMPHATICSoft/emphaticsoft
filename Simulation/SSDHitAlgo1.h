////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in a SSD plane
///          With a bit more information, such that the transformation to a rawData is easier. 
/// \author  lebrun@fnal.gov, jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_SSDHITALGO1_H
#define SIM_SSDHITALGO1_H

#include <vector>
#include <string>

namespace sim {
  /// A single unit of energy deposition in the liquid scintillator
  class SSDHitAlgo1 {
  public:
    SSDHitAlgo1();
    ~SSDHitAlgo1() {}
    
    double     GetX() const {return fX[0]; }  // Additional information, not to be used directly to convert to a strip number. 
    double     GetY() const {return fX[1]; }
    double     GetZ() const {return fX[2]; }

    double     GetYLocal0() const {return fYLocal0; } // entrance "measuring coordinate", Y by defintion if the View angle set to 0.  
    double     GetYLocal1() const {return fYLocal1; } // exit in the measuring coordinate  
    std::string GetG4VolName() const {return fG4VolName; }  
    
    
    double     GetPx() const {return fP[0]; } // additional ino. 
    double     GetPy() const {return fP[1]; }
    double     GetPz() const {return fP[2]; }

    int        GetPId() const {return fPId; } // pdg value

    int        GetPlane() const {return fPlane; }
    int        GetSensor() const {return fSensor; }
    int        GetStrip() const {return fStrip; }
    int        GetTrackID()      const {return fTrackID;}

    void       SetX(double x[3]) {fX.clear(); for (int i=0; i<3; ++i) fX.push_back(x[i]);}
    
    // Added, the only crucial info to convert the hit to a set of raw SSDstrip (more than one, rather frequently. ) 
     
    void       SetYLocal0(double y) {fYLocal0=y;}
    void       SetYLocal1(double y) {fYLocal1=y;}
    void       SetG4VolName(const std::string &v) {fG4VolName = v;}
    //
    void       SetP(double p[3]) {fP.clear(); for (int i=0; i<3; ++i) fP.push_back(p[i]);}
    void       SetPId(int id) { fPId = id; }
    void       SetPlane(int plane) { fPlane = plane; }
    void       SetSensor(int sensor) { fSensor = sensor; }
    void       SetStrip(int strip) { fStrip = strip; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }

  private:
    std::vector<double> fX;
    // Added/// 
    double fYLocal0;
    double fYLocal1;
    std::string fG4VolName;
    std::vector<double> fP;
    int    fPId;

    int    fPlane;
    int    fSensor; 
    int    fStrip;

    int    fTrackID;
  };

}
#endif
////////////////////////////////////////////////////////////////////////
