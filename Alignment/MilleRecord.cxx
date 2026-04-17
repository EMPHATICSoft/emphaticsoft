////////////////////////////////////////////////////////////////////////
/// \file    MilleRecord.cxx
/// \brief   Class and container to save a Mille record..  
/// \version 
/// \author  lebrun@fnal.gov, to create interface c++ ->c -> f90.. 
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <array>
#include <iomanip>
#include <cmath>
#include <cfloat>
#include "Alignment/MilleRecord.h"
#include "millepede_ii/Mille.h"

namespace emph {

  namespace align {
    
    //----------------------------------------------------------------------
    
    MilleRecord::MilleRecord(const int evtNum, const short spill, const short station, const short plane, const short sensor, 
                         const std::array<float,3> dervis, const float co, const float pull, const float err) :
       evtNum_(evtNum), spill_(spill), station_(station), plane_(plane), sensor_(sensor), 
       coordOrtho_(co), pull_(pull), err_(err), derivs_(dervis) 
      {
         ; 
      }
      std::ostream& operator<< (std::ostream& o, const emph::align::MilleRecord& h)
     {
        o << "Mille Record, spill " <<  h.spill_ << " evt " << h.evtNum_ << " station  "
	  << h.station_<<  ", Plane " << h.plane_ << " Sensor "<<  h.sensor_ <<std::endl;
	o << "   ... deriv " << h.derivs_[0] << ", " << h.derivs_[1] << ", " << h.derivs_[2] 
	  << " OrthoCoord " << h.coordOrtho_ << " pull  " << h.pull_ << " err " << h.err_ << std::endl << std::endl;
	return o;
      }
      
      
      /// 
      MilleRecords::MilleRecords() {
         ;
      }
      
      double MilleRecords::CenterPulls(short station, short plane, short sensor) {
      
        double a=0; int n=0;  
        for (auto it=fData.cbegin(); it != fData.cend(); it++ ) {
	  if ((station != it->station_) || (plane != it->plane_) || sensor != it->sensor_) continue;
	  a += it->pull_; n++;
	}
	if (n < 1) return DBL_MIN;
	a /= n;
        for (auto it=fData.begin(); it != fData.end(); it++ ) {
	  if ((station != it->station_) || (plane != it->plane_) || sensor != it->sensor_) continue;
	  it->pull_ -= a;
	}
	return a;
      }
      // 
      void MilleRecords::CenterPulls(short station, short plane, short sensor, double val) {
      
        for (auto it=fData.begin(); it != fData.end(); it++ ) {
	  if ((station != it->station_) || (plane != it->plane_) || sensor != it->sensor_) continue;
	  it->pull_ -= val;
	}
      }
      
      void MilleRecords::CenterPulls(const std::map<short, double> &vals) {
      
        for (auto it=fData.begin(); it != fData.end(); it++ ) {
	  short index = 1000*it->station_ + 100*it->plane_ + 10*it->sensor_;
	  double val = vals.at(index);
	  it->pull_ -= val;
	}
      }
      
       void MilleRecords::ShiftCenterPulls(short station, double x, double y) { // valid only for phase1c 
        short maxPlane = (station == 0 || station == 7 || station == 4) ? 2 : 3;
	for (short kPl=0; kPl != maxPlane; kPl++) { 
          for (auto it=fData.begin(); it != fData.end(); it++ ) {
	    if (it->station_ != station) continue;
	    if (it->plane_ != kPl) continue;
	      if (station == 0 || station == 1) {
	        if (kPl == 0) it->pull_ -= y; 
		else it->pull_ -= x;
	      } else if ((station == 2) || (station == 3)) {
	        if (kPl == 1) it->pull_ -= y; 
		else if (kPl == 2) it->pull_ -= x;
		else if (kPl == 0) it->pull_ -= (1.0/sqrt(2.)) * ( y - x); // assume perfect 45 degrees.. 	      
	      } else {
	        if (kPl == 0) it->pull_ -= x;
		else if (kPl == 1) it->pull_ -= y; 
		else if (kPl == 2) it->pull_ -= (1.0/sqrt(2.)) * ( y - x); // assume perfect 45 degrees.. 	      
	      }
	   } 
	}
      }
     
      void MilleRecords::SaveIt(bool byStations, const std::string &newFileName) const {
      
        Mille  *m = new Mille(newFileName.c_str(),true,true);
 
        int labels[3] = {999, 999, 999};
	const float lcd=1.; // local derivatives, fixed, no strictly local parameters. 
	int nRec = 0; 
        for (auto it=fData.cbegin(); it != fData.cend(); it++, nRec++ ) {
	  if (nRec < 50) std::cerr << (*it);
          for (size_t kParam = 1; kParam !=4; kParam++) 
	   { 
	     labels[kParam-1] = (byStations) ? (it->station_ * 10 + kParam) : 
	                                       (it->station_ * 1000 + it->plane_*100 + it->sensor_*10 + kParam);
	  }
	  if ((nRec < 50) && (it->station_ == 2))
	     std::cerr << " MilleRecords::SaveIt, station 2, Plane " << it->plane_ 
	               << " sensor " << it->sensor_ << " labels " 
		       <<  labels[0] << ", " << labels[1] << ", " << labels[2] << std::endl << std::endl;  
	  m->mille(1, &lcd, 3, &(it->derivs_[0]), labels, it->pull_, it->err_); 
//	  m->mille(3, &(it->derivs_[0]), 3, &(it->derivs_[0]), &labels[0], it->pull_, it->err_); 
	  auto itNext = it; itNext++;
	  if (itNext == fData.cend()) { m->end(); break; } // done.. 
	  if (itNext->station_ < it->station_) m->end();  // next event..
        }
	 
	 delete m;
      }
 } // name space align 
} // name space emph 
