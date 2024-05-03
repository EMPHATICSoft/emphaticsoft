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
#include "RecoBase/LineSegment.h"

namespace emph{
  namespace al{


  class SSDAlign {
  public:
    SSDAlign(); // Default constructor
    SSDAlign(const emph::geo::Detector &sd, const emph::geo::SSDStation &st); // Default constructor
    SSDAlign(const rb::SSDCluster &clust, int evt); // Default constructor
    virtual ~SSDAlign() {}; //Destructor
    
  private:

    double _pitch;
    int _station;
    int _plane;
    int _sensor;
    int _axisindex;
    double _shift;
    emph::geo::sensorView _view;
    int _width;
	double _x, _y, _z, _u, _w;
    int _event;

  public:
    // Getters
    double Pitch() const { return _pitch; }
    int Station() const { return _station; }
    int Plane() const { return _plane; }
    int Sensor() const { return _sensor; }
    double Shift() const { return _shift;}
    int Width() const { return _width; }
    int AxisIndex() const { return _axisindex; }
    emph::geo::sensorView View() const { return _view; }
    double X() const { return _x; }
    double Y() const { return _y; }
    double Z() const { return _z; }
    double U() const { return _u; }
    double W() const { return _w; }
    int Event() const { return _event; }

    // Setters
    void SetPitch(double pitch) { _pitch = pitch; }
    void SetStation(int station) { _station = station; }
    void SetPlane(int plane) { _plane = plane; }
    void SetSensor(int sensor) { _sensor = sensor; }
    void SetAxisIndex(int axisindex) { _axisindex = axisindex; }
    void SetWidth(int width) { _width = width; }
    void SetShift(double shift) { _shift = shift; }
    void SetView(emph::geo::sensorView view) { _view = view; }
    void SetPos(rb::LineSegment ls);
    void SetX(double x) { _x = x; }
    void SetY(double y) { _y = y; }
    void SetZ(double z) { _z = z; }
    void SetU(double u) { _u = u; }
    void SetW(double w) { _w = w; }
    void SetEvent(int event) { _event = event; }

    
    std::vector<int> SSDInfo();
    void SSDHitPosition(std::vector<std::vector<double>>& xpos, std::vector<std::vector<double>>& ypos,std::vector<std::vector<double>>& upos, std::vector<std::vector<double>>& vpos);
    void FillEvtHolder(std::vector<emph::al::SSDAlign>& evt, std::vector<emph::al::SSDAlign>& xevt, std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& uevt,std::vector<emph::al::SSDAlign>& vevt);
    bool IsAlignmentEvent(const std::vector<emph::al::SSDAlign>& evt,const int& nstations);
    bool IsAlignmentEvent(const std::vector<emph::al::SSDAlign>& xevt,const std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& evt,const int& nstations);
    
    friend std::ostream& operator << (std::ostream& o, const SSDAlign& h);
  };

  }
}

#endif // SSDALIGN_H
