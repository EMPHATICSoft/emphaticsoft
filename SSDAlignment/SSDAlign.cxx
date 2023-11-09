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
    _isflip(0), _angle(0.), _strip(0.), _pitch(0.06), _height(0.), _station(0.), _plane(0.), _sensor(0.), _axisindex(0.), _shift(0.), _view(emph::geo::X_VIEW), _x(0.), _y(0.), _z(0.), _u(0.), _v(0.), _event(0.)
  {

  }
  
  //------------------------------------------------------------
  SSDAlign::SSDAlign(const rb::SSDCluster &clust, const emph::geo::Detector &sd, const emph::geo::SSDStation &st, int evt)
  {
      _isflip = sd.IsFlip();
	  _angle = sd.Rot();
	  _strip = clust.AvgStrip();
      if (_isflip) _strip = 639 - _strip; //flip strip number if sensor is flipped
	  _pitch = 0.06;
	  _height = sd.Pos()[0];
	  _station = clust.Station();
      _plane = clust.Plane();
	  _sensor = clust.Sensor();
      _axisindex = -999; //needs to be set in program since it is geometry dependent
      _shift = 0;
      _view = sd.View();
	  _x = (this->Strip()*this->Pitch()-sd.Height()/2)*sin(sd.Rot())+sd.Pos()[0]; 
	  _y = (this->Strip()*this->Pitch()-sd.Height()/2)*cos(sd.Rot())+sd.Pos()[1]; 
	  _z = sd.Pos()[2] + st.Pos()[2];
	  _u = (sqrt(2)/2)*(this->X()-this->Y());	
	  _v = (sqrt(2)/2)*(this->X()+this->Y());	
	  _event = evt;
  }

  //------------------------------------------------------------
  std::vector<int> SSDAlign::SSDInfo()
  {
	  //This function returns a vector in the form (0/1/2,index)
	  //The first entry denotes whether the ssd measures x(0), y(1), u(2), or v(3)
	  //Second is the index of the ssd used for alignment later (order they appear in beamline
	  std::vector<int> ssd_info;

	  int station = this->Station();
	  int sensor = this->Sensor();

	  if (station == 0 && sensor == 0) ssd_info = {1,0};
	  else if (station == 0 && sensor == 1) ssd_info = {0,0};
	  else if (station == 1 && sensor == 0) ssd_info = {1,1};
	  else if (station == 1 && sensor == 1) ssd_info = {0,1};
	  else if (station == 2 && sensor == 1) ssd_info = {1,2};
	  else if (station == 2 && sensor == 2) ssd_info = {0,2};
	  else if (station == 3 && sensor == 1) ssd_info = {1,3};
	  else if (station == 3 && sensor == 2) ssd_info = {0,3};
	  else if (station == 2 && sensor == 0) ssd_info = {2,2};
	  else if (station == 3 && sensor == 0) ssd_info = {2,3};
	  else if (station == 4 && sensor == 0) ssd_info = {0,4};
	  else if (station == 4 && sensor == 1) ssd_info = {0,5};
	  else if (station == 4 && sensor == 2) ssd_info = {1,4};
	  else if (station == 4 && sensor == 3) ssd_info = {1,5};
	  else if (station == 4 && sensor == 4) ssd_info = {3,4};
	  else if (station == 4 && sensor == 5) ssd_info = {3,5};
	  else if (station == 5 && sensor == 0) ssd_info = {0,6};
	  else if (station == 5 && sensor == 1) ssd_info = {0,7};
	  else if (station == 5 && sensor == 2) ssd_info = {1,6};
	  else if (station == 5 && sensor == 3) ssd_info = {1,7};
	  else if (station == 5 && sensor == 4) ssd_info = {3,6};
	  else if (station == 5 && sensor == 5) ssd_info = {3,7};

	  else{ssd_info = {-1,-1}; std::cout<<"station and mod "<< station<< " "<<sensor<<std::endl;}

	  return ssd_info;
  }

  //------------------------------------------------------------
  void SSDAlign::SSDHitPosition(std::vector<std::vector<double>>& xpos, std::vector<std::vector<double>>& ypos,std::vector<std::vector<double>>& upos, std::vector<std::vector<double>>& vpos)
  {
  	  std::vector<int> ssd_info = this->SSDInfo();

	  if(ssd_info[0]==0){
		double x = this->X();
		xpos[ssd_info[1]].push_back(x);	
	  }
	  if(ssd_info[0]==1){
		double y = this->Y();
		ypos[ssd_info[1]].push_back(y);	
	  }
	  if(ssd_info[0]==2){
	  	double u = this->U();
	        upos[ssd_info[1]].push_back(u);	
	  }
	  if(ssd_info[0]==3){
	  	double v = this->V();
	        vpos[ssd_info[1]].push_back(v);	
	  }
	  
  }

  //------------------------------------------------------------
  bool SSDAlign::IsSingleHit(std::vector<double>& hits)
  {
    //Combine 2 nearby hits into 1
    if (hits.size()==2 && abs(hits[0]-hits[1])<=this->Pitch()) {
	hits = {(hits[0]+hits[1])/2};
	//std::cout<<"hist[0] "<<hits[0]<<"   hits[1] "<<hits[1]<<std::endl;
    }
    //Combine 3 nearby hits into 1
    if (hits.size()==3){
	std::sort(hits.begin(),hits.end());
	if(abs(hits[0]-hits[1])<=this->Pitch() && abs(hits[1]-hits[2])<=this->Pitch()){
		hits = {hits[1]};
	}
	
    }

    //Possible operation to filter noisy channels

    //Returns true if there is only 1 hit recorded
    if (hits.size()==1) return true;
    else return false;
  }

  //------------------------------------------------------------
  void SSDAlign::FillEvtHolder(const std::vector<emph::al::SSDAlign>& evt, std::vector<emph::al::SSDAlign>& xevt, std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& uevt,std::vector<emph::al::SSDAlign>& vevt)
  {
    for (size_t i=0; i<evt.size(); ++i){
        if(evt[i].View()==emph::geo::X_VIEW) xevt.push_back(evt[i]);
        if(evt[i].View()==emph::geo::Y_VIEW) yevt.push_back(evt[i]);
        if(evt[i].View()==emph::geo::U_VIEW) uevt.push_back(evt[i]);
        if(evt[i].View()==emph::geo::W_VIEW) vevt.push_back(evt[i]);
    }
  }

  //------------------------------------------------------------
  bool SSDAlign::IsAlignmentEvent(std::vector<std::vector<double>> pos)
  {
    //Require 1 and only 1 hit in each SSD station for given input vector of positions
    //Define bool variables denoting if there is only 1 hit in each station
    bool s1 = IsSingleHit(pos[0]);
    bool s2 = IsSingleHit(pos[1]);
    bool s3 = IsSingleHit(pos[2]);
    bool s4 = IsSingleHit(pos[3]);
    bool s5 = IsSingleHit(pos[4])^IsSingleHit(pos[5]); //xor to require only 1 hit total
    bool s6 = IsSingleHit(pos[6])^IsSingleHit(pos[7]); //xor to require only 1 hit total

    if(s1 && s2 && s3 && s4 && s5 && s6) return true;
    else return false;
  } 

  //------------------------------------------------------------
  bool SSDAlign::IsAlignmentEvent2(const std::vector<emph::al::SSDAlign>& evt,const int& nstations)
  {
    int flag=0;
    std::vector<int> stations_hit;
    //std::cout<<"Number of stations: "<< nstations<< "     hits: "<<evt.size()<<std::endl;
    //Require equal number of hits to stations
    if (evt.size()!=static_cast<size_t>(nstations)){
        return false;
    }
    else{
        for (size_t i=0; i<evt.size(); ++i){
            stations_hit.push_back(evt[i].Station());
        }
        std::sort(stations_hit.begin(),stations_hit.end());
        for (int i=0; i<nstations; ++i){
            if (stations_hit[i]!=i) flag+=1;
        }
    }

    if(flag==0) return true;
    else return false;
  }

  //------------------------------------------------------------
  bool SSDAlign::IsAlignmentEvent2(const std::vector<emph::al::SSDAlign>& xevt,const std::vector<emph::al::SSDAlign>& yevt,std::vector<emph::al::SSDAlign>& evt,const int& nstations)
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

    //std::cout<<"views : "<<std::endl;
    //for (size_t i =0; i<evt.size(); ++i){
    //    std::cout<<evt[i].View()<<"  ";
    //}
    //std::cout<<"      stations hitsize: "<<stations_hit.size()<<"   first station hit: "<<stations_hit[0];
    //std::cout<<std::endl;

    if (stations_hit.size()==0) return false;
    else return true;
  }

  //------------------------------------------------------------
  bool SSDAlign::IsAlignmentEvent(std::vector<std::vector<double>> xpos,std::vector<std::vector<double>> ypos,std::vector<std::vector<double>>& pos)
  {
    //bools relevent to if there are x and y hits to calculate a u/v position at each SSD station
    bool s1 = IsSingleHit(xpos[0]) && IsSingleHit(ypos[0]);
    bool s2 = IsSingleHit(xpos[1]) && IsSingleHit(ypos[1]);
    bool s3 = IsSingleHit(xpos[2]) && IsSingleHit(ypos[2]);
    bool s4 = IsSingleHit(xpos[3]) && IsSingleHit(ypos[3]);
    bool s5 = (IsSingleHit(xpos[4])^IsSingleHit(xpos[5])) && (IsSingleHit(ypos[4])^IsSingleHit(ypos[5]));
    bool s6 = (IsSingleHit(xpos[6])^IsSingleHit(xpos[7])) && (IsSingleHit(ypos[6])^IsSingleHit(ypos[7]));

    //bools relevent to u/v SSDs
    bool b3 = IsSingleHit(pos[2]);
    bool b4 = IsSingleHit(pos[3]);
    bool b5 = IsSingleHit(pos[4])^IsSingleHit(pos[5]); //xor to require only 1 hit total
    bool b6 = IsSingleHit(pos[6])^IsSingleHit(pos[7]); //xor to require only 1 hit total

    //First determine if the calibration is for u or v SSDs
    //if b3 and b4 then there are hits in each u SSD
    if(b3 && b4){
    	if(s1 && s2 && s5 && s6) return true;
	else return false;
    }
    //if b5 and b6 then there are hits in each v SSD plane
    else if(b5 && b6){
    	if(s1 && s2 && s3 && s4) return true;
	else return false;
    }
    else return false;
  } 

  //------------------------------------------------------------
  std::vector<double> SSDAlign::PositionAtStations(std::vector<std::vector<double>> pos)
  {
    std::vector<double> station_pos = {0,0,0,0,0,0};
    
    //For first 4 stations there is only one measurement
    for(int i=0; i<4; ++i) station_pos[i] = pos[i][0];

    //Final two stations we take the nonzero element of the double layered ssd
    if (pos[4].empty()) station_pos[4] = pos[5][0];
    else station_pos[4] = pos[4][0];
    if (pos[6].empty()) station_pos[5] = pos[7][0];
    else station_pos[5] = pos[6][0];


    return station_pos;
  }

  //------------------------------------------------------------
  std::vector<double> SSDAlign::PositionAtStations(std::vector<std::vector<double>> xpos, std::vector<std::vector<double>> ypos, std::vector<std::vector<double>> pos)
  {
    std::vector<double> station_pos = {0,0,0,0,0,0};

    //First determine if this is for u or v SSDs
    bool b3 = IsSingleHit(pos[2]);
    bool b4 = IsSingleHit(pos[3]);
    bool b5 = IsSingleHit(pos[4])^IsSingleHit(pos[5]); //xor to require only 1 hit total
    bool b6 = IsSingleHit(pos[6])^IsSingleHit(pos[7]); //xor to require only 1 hit total
    
    //For u SSDs
    if(b3 && b4){
	station_pos[0] = (sqrt(2)/2)*(xpos[0][0]-ypos[0][0]);	
    	station_pos[1] = (sqrt(2)/2)*(xpos[1][0]-ypos[1][0]);	
    	station_pos[2] = pos[2][0];	
    	station_pos[3] = pos[3][0];
	//Final two stations we take the nonzero element of the double layered ssd
	double x5, x6, y5, y6;
	if(xpos[4].empty()) x5 = xpos[5][0];
	else x5 = xpos[4][0];
	if(ypos[4].empty()) y5 = ypos[5][0];
	else y5 = ypos[4][0];
	if(xpos[6].empty()) x6 = xpos[7][0];
	else x6 = xpos[6][0];
	if(ypos[6].empty()) y6 = ypos[7][0];
	else y6 = ypos[6][0];

	station_pos[4] = (sqrt(2)/2)*(x5-y5);
	station_pos[5] = (sqrt(2)/2)*(x6-y6);
    }
    //For v SSDs
    else if(b5 && b6){
	station_pos[0] = (sqrt(2)/2)*(xpos[0][0]+ypos[0][0]);	
    	station_pos[1] = (sqrt(2)/2)*(xpos[1][0]+ypos[1][0]);	
    	station_pos[2] = (sqrt(2)/2)*(xpos[3][0]+ypos[2][0]);	
    	station_pos[3] = (sqrt(2)/2)*(xpos[2][0]+ypos[3][0]);	
	//Final two stations we take the nonzero element of the double layered ssd
    	if (pos[4].empty()) station_pos[4] = pos[5][0];
  	else station_pos[4] = pos[4][0];
   	if (pos[6].empty()) station_pos[5] = pos[7][0];
    	else station_pos[5] = pos[6][0];

    }
    else std::cout<<"ERROR"<<std::endl;

    return station_pos;
  }

  //------------------------------------------------------------
  std::vector<int> SSDAlign::IndexAtStations(std::vector<std::vector<double>> pos)
  {
    std::vector<int> station_index = {0,0,0,0,0,0};
    
    //For first 4 stations there is only one measurement
    for(int i=0; i<4; ++i) station_index[i] = i;

    //Final two stations we take the nonzero element of the double layered ssd
    if (pos[4].empty()) station_index[4] = 5;
    else station_index[4] = 4; 
    if (pos[6].empty()) station_index[5] = 7;
    else station_index[5] = 6;


    return station_index;
  }

  //------------------------------------------------------------
  std::vector<double> SSDAlign::StationZpos()
  {
    std::vector<double> z_pos = {0,0,0,0,0,0};
     
    return z_pos;

  }

  //------------------------------------------------------------
  void SSDAlign::LoadShifts(std::string fname, std::vector<double> &x, std::vector<double> &y, std::vector<double> &u, std::vector<double> &v)
  {
    std::ifstream AlignmentFile;
    AlignmentFile.open(fname.c_str());
    if (!AlignmentFile.is_open()) {
        std::cout<<"Error: SSD alignment file not found"<<std::endl;
        std::abort();
    }
    //int colIdx = 0;
    std::string line;
    std::string holder;
    double val;
    std::string axis;

    while (getline(AlignmentFile, line)){
        std::stringstream lineStr(line);
        std::cout<<lineStr.str().c_str()<<std::endl;
        lineStr >> holder >> axis; 
        if (holder[0]=='#') continue; 
        while (lineStr >> val){
            if (axis=='X') x.push_back(val);
            if (axis=='Y') y.push_back(val);
            if (axis=='U') u.push_back(val);
            if (axis=='V') v.push_back(val);
        }
    }
    AlignmentFile.close();
  }

  //------------------------------------------------------------
   void SSDAlign::CalibrateXYZ(double *cal)
  {
	  _x=_x+cal[0];
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const SSDAlign& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " SSD Strip = "     << std::setw(5) << std::right << h.Strip()
      << " Angle = "     << std::setw(5) << std::right << h.Angle()
      << " Pitch = "        << std::setw(5) << std::right << h.Pitch()     
      << " Event Number  = "        << std::setw(5) << std::right << h.Event();
    return o;
  }


  }
}
//////////////////////////////////////////////////////////////////////////////
