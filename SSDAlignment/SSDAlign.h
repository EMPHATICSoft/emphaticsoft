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

    bool _isflip;
    double _angle; // rotation angle about the vertical y-axis
    double _strip;  // avg. strip position
    double _pitch;
    double _height;
    int _station;
    int _plane;
    int _sensor;
    int _axisindex;
    double _shift;
    emph::geo::sensorView _view;
	double _x, _y, _z, _u, _v;
    int _event;

  public:
    // Getters
    bool IsFlip() const { return _isflip; }
    double Angle() const { return _angle; }
    double Strip() const { return _strip; }
    double Pitch() const { return _pitch; }
    double Height() const { return _height; }
    int Station() const { return _station; }
    int Plane() const { return _plane; }
    int Sensor() const { return _sensor; }
    double Shift() const { return _shift;}
    int AxisIndex() const { return _axisindex; }
    emph::geo::sensorView View() const { return _view; }
    double X() const { return _x; }
    double Y() const { return _y; }
    double Z() const { return _z; }
    double U() const { return _u; }
    double V() const { return _v; }
    int Event() const { return _event; }

    // Setters
    void SetIsFlip(double isflip) { _isflip = isflip; }
    void SetAngle(double ang) { _angle = ang; }
    void SetStrip(double strip) { _strip = strip; }
    void SetPitch(double pitch) { _pitch = pitch; }
    void SetHeight(double height) { _height = height; }
    void SetStation(int station) { _station = station; }
    void SetPlane(int plane) { _plane = plane; }
    void SetSensor(int sensor) { _sensor = sensor; }
    void SetAxisIndex(int axisindex) { _axisindex = axisindex; }
    void SetShift(double shift) { _shift = shift; }
    void SetView(emph::geo::sensorView view) { _view = view; }
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
    void FillEvtHolder(const std::vector<emph::al::SSDAlign>& evt, std::vector<emph::al::SSDAlign>& xevt, std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& uevt,std::vector<emph::al::SSDAlign>& vevt);
    bool IsAlignmentEvent2(const std::vector<emph::al::SSDAlign>& evt,const int& nstations);
    bool IsAlignmentEvent2(const std::vector<emph::al::SSDAlign>& xevt,const std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& evt,const int& nstations);
    bool IsAlignmentEvent(std::vector<std::vector<double>> xpos,std::vector<std::vector<double>> ypos,std::vector<std::vector<double>>& pos);
    std::vector<double> PositionAtStations(std::vector<std::vector<double>> pos);
    std::vector<double> PositionAtStations(std::vector<std::vector<double>> xpos,std::vector<std::vector<double>> ypos, std::vector<std::vector<double>> pos);
    std::vector<int> IndexAtStations(std::vector<std::vector<double>> pos);
    std::vector<double> StationZpos();
    void LoadShifts(std::string fname, std::vector<double> &x, std::vector<double> &y, std::vector<double> &u, std::vector<double> &v);
    void CalibrateXYZ(double *cal); 
    
    friend std::ostream& operator << (std::ostream& o, const SSDAlign& h);
  };

  }
}

#endif // SSDALIGN_H
