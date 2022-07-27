////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in a SSD plane
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_SSDHIT_H
#define SIM_SSDHIT_H

#include <vector>
#include <string>

namespace sim {
  /// A single unit of energy deposition in the liquid scintillator
  class SSDHit {
  public:
    SSDHit();
    ~SSDHit() {}
    
    double     GetX() const {return fX[0]; }
    double     GetY() const {return fX[1]; }
    double     GetZ() const {return fX[2]; }

    double     GetPx() const {return fP[0]; }
    double     GetPy() const {return fP[1]; }
    double     GetPz() const {return fP[2]; }

    int        GetPId() const {return fPId; } // pdg value

    int        GetPlane() const {return fPlane; }
    int        GetSensor() const {return fSensor; }
    int        GetStrip() const {return fStrip; }
    int        GetTrackID()      const {return fTrackID;}
    double     GetMass()         const  {return fMass;}
    std::string GetParticleType()   const   {return fParticleType;}

    void       SetX(double x[3]) {fX.clear(); for (int i=0; i<3; ++i) fX.push_back(x[i]);}
    void       SetP(double p[3]) {fP.clear(); for (int i=0; i<3; ++i) fP.push_back(p[i]);}
    void       SetPId(int id) { fPId = id; }
    void       SetPlane(int plane) { fPlane = plane; }
    void       SetSensor(int sensor) { fSensor = sensor; }
    void       SetStrip(int strip) { fStrip = strip; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }
    void       SetMass(double mass)  {fMass = mass;}
    void       SetType(std::string particletype)  {fParticleType = particletype;}

  private:
    std::vector<double> fX;
    std::vector<double> fP;
    int    fPId;

    int    fPlane;
    int    fSensor; 
    int    fStrip;

    int    fTrackID;

    double fMass;
    std::string fParticleType;
  };

}
#endif
////////////////////////////////////////////////////////////////////////
