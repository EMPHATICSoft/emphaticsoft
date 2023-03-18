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

    double     GetTLocal0() const {return fTLocal0; } // entrance "transverse measuring coordinate", Y by arbitrary setting in G4EMPH,
                                                      //  to be explicit , when the View angle, or rotation angle of the waver, is set to 0.  
    double     GetTLocal1() const {return fTLocal1; } // exit in the same measuring coordinate system  
    double     GetEDep()  const { return fEDep; } // Used to determine the ADC amplitude for the corresponding set of rawdata strips. 
//    std::string GetG4SensorVolName() const {return fG4SensorVolName; }  
    
    
    double     GetPx() const {return fP[0]; } // additional ino. 
    double     GetPy() const {return fP[1]; }
    double     GetPz() const {return fP[2]; }

    int        GetPId() const {return fPId; } // pdg value

    int        GetStation() const {return fStation; }
    int        GetPlane() const {return fPlane; }
    int        GetSensor() const {return fSensor; }
    int        GetStrip() const {return fStrip; }
    int        GetTrackID()      const {return fTrackID;}

    void       SetX(double x[3]) {fX.clear(); for (int i=0; i<3; ++i) fX.push_back(x[i]);}
    
    // Added, the only crucial info to convert the hit to a set of raw SSDstrip (more than one, rather frequently. ) 
     
    void       SetTLocal0(double y) {fTLocal0=y;}
    void       SetTLocal1(double y) {fTLocal1=y;}
//    void       SetG4SensorVolName(const std::string &v) {fG4SensorVolName = v;}
    void       SetEDep(const double &e) {fEDep = e;}
    //
    void       SetP(double p[3]) {fP.clear(); for (int i=0; i<3; ++i) fP.push_back(p[i]);}
    void       SetPId(int id) { fPId = id; }
    void       SetStation(int s) { fStation = s; } //valid range are 0 to 5 
    void       SetPlane(int plane) { fPlane = plane; }
    void       SetSensor(int sensor) { fSensor = sensor; }
    void       SetStrip(int strip) { fStrip = strip; }
    void       SetTrackID(int trkId) { fTrackID = trkId; }

  private:
    std::vector<double> fX;
    // Added/// 
    double fTLocal0; // entrance Y coordinate 
    double fTLocal1;
    double fEDep; // energy deposition 
//    std::string fG4SensorVolName; // No longer needed, we have now the station number (0 through 5) and the plane number (also 0 through 5, 0 to 1, or 0 to 2)
    std::vector<double> fP;
    int    fPId;

    short int    fStation;
    short int    fPlane;
    short int    fSensor; 
    short int    fStrip;

    int    fTrackID;
  };

}
#endif
////////////////////////////////////////////////////////////////////////
