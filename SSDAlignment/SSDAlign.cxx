////////////////////////////////////////////////////////////////////////
/// \brief   SSD Alignment class
/// \author jmirabit@bu.edu 
/// \date
////////////////////////////////////////////////////////////////////////
#include "SSDAlignment/SSDAlign.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cassert>
  
namespace emph {
  namespace al{

  //----------------------------------------------------------------------
  SSDAlign::SSDAlign() :   
     _pitch(0.06), _station(0.), _plane(0.), _sensor(0.), _axisindex(0.), _shift(0.), _view(emph::geo::X_VIEW), _x(0.), _y(0.), _z(0.), _u(0.), _w(0.), _event(0.)
  {
  }
  //------------------------------------------------------------
  SSDAlign::SSDAlign(const emph::geo::Detector &sd, const emph::geo::SSDStation &st)
  {
      _view = sd.View();
	  _z = sd.Pos()[2] + st.Pos()[2];
  }
  
  //------------------------------------------------------------
  SSDAlign::SSDAlign(const rb::SSDCluster &clust, int evt)
  {
	  _pitch = 0.06;
	  _station = clust.Station();
      _plane = clust.Plane();
	  _sensor = clust.Sensor();
      _axisindex = -999; //needs to be set in program since it is geometry dependent
      _shift = 0;
      _view = clust.View();
      _width = clust.Width();
      _x = 0;
      _y = 0;
      _z = 0;
	  _u = (sqrt(2)/2)*(this->X()-this->Y());	
	  _w = (sqrt(2)/2)*(this->X()+this->Y());	
	  _event = evt;
  }

  //------------------------------------------------------------
  void SSDAlign::SetPos(rb::LineSegment ls){
      double x0[3];
      double x1[3];
      double xavg[3];

      for (int i=0; i<3; ++i){
        x0[i] = ls.X0()[i];
        x1[i] = ls.X1()[i];
        xavg[i] = (x0[i]+x1[i])/2.;
      }

      SetX(xavg[0]);
      SetY(xavg[1]);
      SetZ(xavg[2]);
  
      SetU((sqrt(2)/2)*(xavg[0]-xavg[1]));
      SetW((sqrt(2)/2)*(xavg[0]+xavg[1]));
  }

  //------------------------------------------------------------
  void SSDAlign::FillEvtHolder(std::vector<emph::al::SSDAlign>& evt, std::vector<emph::al::SSDAlign>& xevt, std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& uevt,std::vector<emph::al::SSDAlign>& vevt)
  {
      //First loop through events to check if any have Width>2 (keep track of sensors where this is the case using ssp)
      std::vector<std::array<int,3>> wide_hits;
      for (size_t i=0; i<evt.size(); ++i){
          if (evt[i].Width()>2) wide_hits.push_back({evt[i].Station(),evt[i].Sensor(),evt[i].Plane()});
      }
      //Loop through again to remove all events from these sensors
      for (size_t i=0; i<evt.size(); ++i){
          int station = evt[i].Station();
          int sensor = evt[i].Sensor();
          int plane = evt[i].Plane();
          for (size_t j=0; j<wide_hits.size(); ++j){
              if (station == wide_hits[j][0] && sensor == wide_hits[j][1] && plane == wide_hits[j][2]){
                  //evt.erase(evt.begin()+int(i));
              }
          }
      }
      for (size_t i=0; i<evt.size(); ++i){
        //skip event if the width of the cluster is greater than 2 strips
        if(evt[i].View()==emph::geo::X_VIEW) xevt.push_back(evt[i]);
        if(evt[i].View()==emph::geo::Y_VIEW) yevt.push_back(evt[i]);
        if(evt[i].View()==emph::geo::U_VIEW) uevt.push_back(evt[i]);
        if(evt[i].View()==emph::geo::W_VIEW) vevt.push_back(evt[i]);
    }
  }

  //------------------------------------------------------------
  bool SSDAlign::IsAlignmentEvent(const std::vector<emph::al::SSDAlign>& evt,const int& nstations)
  {
    std::vector<int> stations_hit;
    //Not an alignment event if there are more hits than stations or fewer than 3 hits
    if (evt.size()>static_cast<size_t>(nstations) || evt.size()<3){
        return false;
    }
    //Throw away events with multiple hits per station
    else{
        for (size_t i=0; i<evt.size(); ++i){
            stations_hit.push_back(evt[i].Station());
        }
        std::sort(stations_hit.begin(),stations_hit.end());
        //after sorting stations hit, if there exist hits in the same station then it is not good for alignment
        bool s = std::adjacent_find(stations_hit.begin(), stations_hit.end()) != stations_hit.end();

        if(s) return false;
        else return true;
    }
  }

  //------------------------------------------------------------
  bool SSDAlign::IsAlignmentEvent(const std::vector<emph::al::SSDAlign>& xevt,const std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& evt,const int& nstations)
  {
    std::vector<int> stations_hit;
    std::vector<int> xstations_hit;
    std::vector<int> ystations_hit;

    for (size_t i=0; i<evt.size(); ++i) stations_hit.push_back(evt[i].Station()); 
    for (size_t i=0; i<xevt.size(); ++i) xstations_hit.push_back(yevt[i].Station()); 
    for (size_t i=0; i<yevt.size(); ++i) ystations_hit.push_back(yevt[i].Station()); 

    std::sort(stations_hit.begin(),stations_hit.end());
    std::sort(xstations_hit.begin(),xstations_hit.end());
    std::sort(ystations_hit.begin(),ystations_hit.end());

    const bool s = std::adjacent_find(stations_hit.begin(), stations_hit.end()) != stations_hit.end();
    const bool sx = std::adjacent_find(xstations_hit.begin(), xstations_hit.end()) != xstations_hit.end();
    const bool sy = std::adjacent_find(ystations_hit.begin(), ystations_hit.end()) != ystations_hit.end();
    
    //reject any event with multiple hits in tilted axis
    //if (s) return false;
    std::vector<emph::al::SSDAlign> new_holder;

    //reject any event that contains multiple hits per axis and station
    if (s || sx || sy){
        return false;
    }
    else {
        int rotevt_index=0;
        int xevt_index=0;
        int yevt_index=0;
        std::vector<bool> rot_stations (nstations,0);
        for (size_t i=0; i<stations_hit.size(); ++i){
            rot_stations[stations_hit[i]]=1;
        }
        for (int i=0; i<nstations; ++i){
            if(rot_stations[i]){
                new_holder.push_back(evt[rotevt_index]);
                rotevt_index+=1;
            }
            else if (std::find(std::begin(xstations_hit), std::end(xstations_hit), i) != std::end(xstations_hit) && std::find(std::begin(ystations_hit), std::end(ystations_hit), i) != std::end(ystations_hit)){
                new_holder.push_back(xevt[xevt_index]);
                new_holder.push_back(yevt[yevt_index]);
                xevt_index+=1;
                yevt_index+=1;
            }
            else return false;
        }
    }
    //Reformatting event to hold relevent x/y SSDAlign objects
    evt.clear();
    evt = new_holder;

    if (stations_hit.size()==0) return false;
    else return true;
  }

  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const SSDAlign& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " Pitch = "        << std::setw(5) << std::right << h.Pitch()     
      << " Event Number  = "        << std::setw(5) << std::right << h.Event();
    return o;
  }


  }
}
//////////////////////////////////////////////////////////////////////////////
