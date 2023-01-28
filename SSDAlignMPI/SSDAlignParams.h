////////////////////////////////////////////////////////////////////////
/// \brief  The Geometry, SSD only, data for the SSD aligner 
///          Used by main SSD Aligner Algo1 package.   
///          This is a singleton class. 
///          Units are mm, as default G4 units.  Angle in radians. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDALIGNPARAMS_H
#define SSDALIGNPARAMS_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include "BTAlignGeom.h"
#include "SSDAlignParam.h"
#include <mpi.h>

namespace emph{ 
  namespace rbal {
    
    class SSDAlignParams {
    
      private: 
        SSDAlignParams();
	BTAlignGeom* myGeo;
        const size_t fNumStations; // For Phase1b 
        const size_t fNumSensorsXorY; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2 
        const size_t fNumSensorsU; // Station 2 and 3, one sensor each 
        const size_t fNumSensorsV; // Station 2 and 3, one sensor each 
	std::string fMode; // Currently, 2DX, 2DY, 3D Default is 2DY (no magnetic deflection, to 1rst order, so, easiest.
	bool fMoveLongByStation; // We do (or do not move) all individual sensors within a station. )  
	//
	std::vector<SSDAlignParam> fDat;
	 
	static SSDAlignParams* instancePtr;
	
      public:
      
        inline static SSDAlignParams* getInstance() {
	 if ( instancePtr == NULL ) instancePtr = new SSDAlignParams();
	 return instancePtr;
	}
      
        SSDAlignParams(const SSDAlignParams&) = delete;   // no copy constructor. 
	
	// Setters 
	
	inline void SetMode(const std::string &m) { 
	  std::cerr << " SSDAlignParams::SetMode , new mode " << m << " previous mode " << fMode <<  std::endl;
	  const bool doReload = (fMode != m);  fMode = m; 
	  if (doReload) std::cerr << " ..... Reloading !... " << std::endl;
	  else std::cerr << " ..... No mode change, Not Reloading !... " << std::endl;
	  if (doReload) this->ReLoad(); 
	}
	inline void SetMoveLongByStation(bool m) { 
	  const bool doReload = (fMoveLongByStation != m);  fMoveLongByStation = m; if (doReload) this->ReLoad(); 
	}
	inline void SetLimits(paramType t, char view, size_t sensor, const std::pair<double,double> lim) {
	  std::vector<SSDAlignParam>::iterator it = this->It(t, view, sensor);
	  if (it == fDat.end()) return;
	  it->SetLimits(lim);
	}  
	inline void SetValue(paramType t, char view, size_t sensor, double val) {
	  std::vector<SSDAlignParam>::iterator it = this->It(t, view, sensor);
	  if (it == fDat.end()) return;
	  it->SetValue(val);
	}  
	
	// Getter 
	inline std::string Mode() const { return fMode; } 
	inline size_t NumParams() const { return fDat.size(); }
	inline std::vector<SSDAlignParam>::iterator ItEnd() { return fDat.end(); }  
	inline std::vector<SSDAlignParam>::iterator ItBegin()  { return fDat.begin(); }  
	inline size_t size() const { return fDat.size(); }
	inline std::vector<SSDAlignParam>::const_iterator It(size_t kPar)  const { 
	   for (std::vector<SSDAlignParam>::const_iterator it = fDat.cbegin(); it != fDat.cend(); it++) {
	     if (it->MinuitNumber() == kPar) return it;
	   }
	   std::cerr << " SSDAlignParams::It , undefined parameter number " << kPar << " Fatal... " << std::endl;
	   MPI_Finalize(); exit(2);
	   return fDat.cend(); 
        } 
	inline std::vector<SSDAlignParam>::const_iterator Itc(paramType t, char view, size_t sensor)  const {
	  if ((t == emph::rbal::ZMAGC) || (t == emph::rbal::KICKMAGN)) {
	    for (std::vector<SSDAlignParam>::const_iterator it = fDat.cbegin(); it != fDat.cend(); it++) 
	      if (t != it->Type()) return it;
	    std::cerr << " SSDAlignParams::It , undefined paramtype related to the magnet " << t << " Fatal... " << std::endl;
	    MPI_Finalize(); exit(2);
	    return fDat.cend(); 
	  }
	  for (std::vector<SSDAlignParam>::const_iterator it = fDat.cbegin(); it != fDat.cend(); it++) {
	    if (t != it->Type()) continue;
	    if (view != it->View()) continue;
	    if (sensor == it->SensorS()) return it;
	  }
	  std::cerr << " SSDAlignParams::It , undefined paramtype " << t << " or view " 
	            << view << " or sensor number " << sensor << " Fatal... " << std::endl;
	  MPI_Finalize(); exit(2);
	  return fDat.cend(); 
	}
	inline size_t NumSensorsXorY() const { return fNumSensorsXorY; } 
	inline size_t NumSensorsU() const { return fNumSensorsU; } 
	inline size_t NumSensorsV() const { return fNumSensorsV; } 	
	inline size_t NumSensorsW() const { return fNumSensorsV; }
        inline std::pair<double, double> Limits(paramType t, char view, size_t sensor) {
	  std::vector<SSDAlignParam>::const_iterator it=this->Itc(t, view, sensor);
	  if (it == fDat.cend()) return std::pair<double, double>(DBL_MAX, DBL_MAX);
	  return it->Limits();
	}
        inline double Value(paramType t, char view, size_t sensor) {
	  std::vector<SSDAlignParam>::const_iterator it=this->Itc(t, view, sensor);
	  if (it == fDat.cend()) return DBL_MAX;
	  return it->Value();
	}
        inline double Value(size_t k) {
	  std::vector<SSDAlignParam>::const_iterator it = fDat.cbegin(); 
	  for (size_t kk=0; kk!= k; kk++) { 
	    it++;  if (it == fDat.cend()) return DBL_MAX;
	  }
	  return it->Value();
	}
	//
	// IO 
	//
	void DumpTable(const std::string &token) const; // Asci, CSV format 
	
	private:
	  
	inline std::vector<SSDAlignParam>::iterator It(paramType t, char view, size_t sensor) {
	  if ((t == emph::rbal::ZMAGC) || (t == emph::rbal::KICKMAGN)) {
	    for (std::vector<SSDAlignParam>::iterator it = fDat.begin(); it != fDat.end(); it++) 
	      if (t != it->Type()) return it;
	    return fDat.end(); 
	  }
	  for (std::vector<SSDAlignParam>::iterator it = fDat.begin(); it != fDat.end(); it++) {
	    if (t != it->Type()) continue;
	    if (view != it->View()) continue;
	    if (sensor == it->SensorS()) return it;
	  }
	  return fDat.end();
	}
        void ReLoad(); // after a change of mode, or at construction time. 
    };
  } // rbal 
} // emph
#endif // 
