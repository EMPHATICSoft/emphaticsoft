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
    ~SSDHit() {};
    
    double     X() const {return fX[0]; }
    double     Y() const {return fX[1]; }
    double     Z() const {return fX[2]; }

    double     Px() const {return fP[0]; }
    double     Py() const {return fP[1]; }
    double     Pz() const {return fP[2]; }

    double     DE() const {return fDE; }

    int        PId() const {return fPId; } // pdg value

    int        Station() const { return fStation; }
    int        Plane() const {return fPlane; }
    int        Sensor() const {return fSensor; }
    int        Strip() const {return fStrip; }
    int        TrackID()      const {return fTrackID;}
    std::string Process() const {return fProcess; }
    void       SetX(double x[3]) {fX.clear(); for (int i=0; i<3; ++i) fX.push_back(x[i]);}
    void       SetP(double p[3]) {fP.clear(); for (int i=0; i<3; ++i) fP.push_back(p[i]);}
    void       SetDE(double de) {fDE = de; }
    void       SetProcess(std::string str) {fProcess = str; }
    void       SetPId(int id) { fPId = id; }
    void       SetStation(int station) { fStation = station; }
    void       SetPlane(int plane) { fPlane = plane; }
    void       SetSensor(int sensor) { fSensor = sensor; }
    void       SetStrip(int strip) { fStrip = strip; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }

  private:
    std::vector<double> fX;
    std::vector<double> fP;
    std::string fProcess;
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
