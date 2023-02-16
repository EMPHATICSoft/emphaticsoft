////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD align class 
/// \author  jmirabit@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDALIGN_H
#define SSDALIGN_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "RawData/RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "Geometry/Geometry.h"
#include "RecoBase/SSDCluster.h"

namespace emph{
  namespace al{


  class SSDAlign {
  public:
    SSDAlign(); // Default constructor
    SSDAlign(const rb::SSDCluster &clust, const emph::geo::Detector &sd, const emph::geo::SSDStation &st, int evt); // Default constructor
    virtual ~SSDAlign() {}; //Destructor
    
  private:

    double _angle; // rotation angle about the vertical y-axis
    double _strip;  // avg. strip position
    double _pitch;
    double _height;
    int _station;
    int _sensor;
	 double _x, _y, _z, _u, _v;
	 double _xcal, _ycal, _zcal;
    int _event;

  public:
    // Getters
    double Angle() const { return _angle; }
    double Strip() const { return _strip; }
    double Pitch() const { return _pitch; }
    double Height() const { return _height; }
    int Station() const { return _station; }
    int Sensor() const { return _sensor; }
    double X() const { return _x; }
    double Y() const { return _y; }
    double Z() const { return _z; }
    double U() const { return _u; }
    double V() const { return _v; }
    int Event() const { return _event; }

    // Setters
    void SetAngle(double ang) { _angle = ang; }
    void SetStrip(double strip) { _strip = strip; }
    void SetPitch(double pitch) { _pitch = pitch; }
    void SetHeight(double height) { _height = height; }
    void SetStation(int station) { _station = station; }
    void SetSensor(int sensor) { _sensor = sensor; }
    void SetX(double x) { _x = x; }
    void SetY(double y) { _y = y; }
    void SetZ(double z) { _z = z; }
    void SetU(double u) { _u = u; }
    void SetV(double v) { _v = v; }
    void SetEvent(int event) { _event = event; }

    
    std::vector<int> SSDInfo();
    void SSDHitPosition(std::vector<std::vector<double>>& xpos, std::vector<std::vector<double>>& ypos,std::vector<std::vector<double>>& upos, std::vector<std::vector<double>>& vpos);
    bool IsSingleHit(std::vector<double>& hits);
    bool IsAlignmentEvent(std::vector<std::vector<double>> pos);
    bool IsAlignmentEvent(std::vector<std::vector<double>> xpos,std::vector<std::vector<double>> ypos,std::vector<std::vector<double>>& pos);
    std::vector<double> PositionAtStations(std::vector<std::vector<double>> pos);
    std::vector<double> PositionAtStations(std::vector<std::vector<double>> xpos,std::vector<std::vector<double>> ypos, std::vector<std::vector<double>> pos);
    std::vector<int> IndexAtStations(std::vector<std::vector<double>> pos);
    std::vector<double> StationZpos();
    void CalibrateXYZ(double *cal); 
    
    friend std::ostream& operator << (std::ostream& o, const SSDAlign& h);
  };

  }
}

#endif // SSDALIGN_H
