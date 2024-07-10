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
	  _u = (sqrt(2)/2)*(this->X()+this->Y());	
	  _w = (sqrt(2)/2)*((-1.)*this->X()+this->Y());	
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
  
      SetU((sqrt(2)/2)*(xavg[0]+xavg[1]));
      SetW((sqrt(2)/2)*((-1.)*xavg[0]+xavg[1]));
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
    //Loop through all stations, if station has exactly one rotated sensor hit, add it
    //If station has no rotated sensor hit, add x/y hits if there are exactly one of each
    //Replace "evt"  with these events stored in "new_holder"
    std::vector<emph::al::SSDAlign> new_holder;
    bool rotated_events=0;
    for (int i=0; i<nstations; ++i){
        int n=0; int nx=0; int ny=0;
        int nind=-1; int nxind=-1; int nyind=-1;
        for (size_t j=0; j<evt.size(); ++j) {
            if (evt[j].Station()==i){
                n+=1;
                nind=j;
            }
        }
        for (size_t j=0; j<xevt.size(); ++j) {
            if (xevt[j].Station()==i){
                nx+=1;
                nxind=j;
            }
        }
        for (size_t j=0; j<yevt.size(); ++j) {
            if (yevt[j].Station()==i){
                ny+=1;
                nyind=j;
            }
        }

        if (n==1){
            new_holder.push_back(evt[nind]);
            rotated_events=1;
        }
        else if (nx==1 && ny==1) {
            new_holder.push_back(xevt[nxind]);
            new_holder.push_back(yevt[nyind]);
        }
    }

    //Reformatting event to hold relevent x/y SSDAlign objects
    evt.clear();
    evt = new_holder;

    //if there are no rotated events do not do alignment
    if (!rotated_events) return false;
    //if there are fewer than 5 events do not align (1 rotated 2x/y or 2 rotated 1 x/y)
    else if (evt.size()<5) return false;

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
