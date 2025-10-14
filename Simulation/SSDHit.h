////////////////////////////////////////////////////////////////////////
/// \brief   A single unit of energy deposition in a SSD plane
///
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SIM_SSDHIT_H
#define SIM_SSDHIT_H

#include <vector>
#include <ostream>

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

    double     GetDE() const {return fDE; }

    int        GetPId() const {return fPId; } // pdg value

    int        GetStation() const { return fStation; }
    int        GetPlane() const {return fPlane; }
    int        GetSensor() const {return fSensor; }
    int        GetStrip() const {return fStrip; }
    int        GetTrackID()      const {return fTrackID;}

    void       SetX(double x[3]) {fX.clear(); for (int i=0; i<3; ++i) fX.push_back(x[i]);}
    void       SetP(double p[3]) {fP.clear(); for (int i=0; i<3; ++i) fP.push_back(p[i]);}
    void       SetDE(double de) {fDE = de; }

    void       SetPId(int id) { fPId = id; }
    void       SetStation(int station) { fStation = station; }
    void       SetPlane(int plane) { fPlane = plane; }
    void       SetSensor(int sensor) { fSensor = sensor; }
    void       SetStrip(int strip) { fStrip = strip; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }

    inline bool operator==(const SSDHit& ssdhit) const {
        return ((ssdhit.GetStation() == fStation)&&(ssdhit.GetPlane() == fPlane)&&(ssdhit.GetSensor() == fSensor)&&(ssdhit.GetStrip() == fStrip));
      }
	
    inline friend std::ostream& operator<<(std::ostream& os, const SSDHit& ssdhit){
	os << "Sim SSD hit (Station " <<  ssdhit.GetStation() << ", Plane "<<  ssdhit.GetPlane() << ", Sensor " <<  ssdhit.GetSensor() << ", Strip " << ssdhit.GetStrip() << std::endl;
        return os;
        }

	

  private:
    std::vector<double> fX;
    std::vector<double> fP;
    double fDE; // amount of energy deposited
    int    fPId;

    int    fStation;
    int    fPlane;
    int    fSensor; 
    int    fStrip;

    int    fTrackID;
  };

}
#endif
////////////////////////////////////////////////////////////////////////
