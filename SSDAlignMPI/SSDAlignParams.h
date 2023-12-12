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
#include "BTAlignGeom1c.h"
#include "SSDAlignParam.h"
#include <mpi.h>

namespace emph{ 
  namespace rbal {
    
    class SSDAlignParams {
    
      private: 
        SSDAlignParams();
	bool fIsPhase1c; 
	BTAlignGeom* myGeo;
	BTAlignGeom1c* myGeo1c;
        size_t fNumStations; // For Phase1b 
        size_t fNumSensorsXorY; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2 
        size_t fNumSensorsU; // Station 2 and 3, one sensor each 
        size_t fNumSensorsV; // Station 4 and 5, Two sensor each 
	std::string fMode; // Currently, 2DX, 2DY, 3D Default is 2DY (no magnetic deflection, to 1rst order, so, easiest.
	bool fMoveLongByStation; // We do (or do not move) all individual sensors within a station. ) 
	bool fUseSoftLimits;
	bool fStrictSt6;       
	char fSpecificView;
        int fSpecificSensor;
	double fLimRolls;
 
	//
	std::vector<std::string> fSubTypeDirectory;
	std::vector<SSDAlignParam> fDat;
	 
	static SSDAlignParams* instancePtr;
	
      public:
        inline static SSDAlignParams* getInstance() {
	 if ( instancePtr == NULL ) instancePtr = new SSDAlignParams();
	 return instancePtr;
	}
        
        SSDAlignParams(const SSDAlignParams&) = delete;   // no copy constructor. 
	
	// Setters 
	
        inline void SetForPhase1c(bool t=true) {
	  fIsPhase1c = t; 
	  if (fIsPhase1c) { 
            fNumStations = myGeo1c->NumStations(); 
            fNumSensorsXorY = myGeo1c->NumSensorsXorY(); 
            fNumSensorsU = myGeo1c->NumSensorsU(); 
            fNumSensorsV = myGeo1c->NumSensorsV();
	  } else {  
            fNumStations = myGeo->NumStations(); 
            fNumSensorsXorY = myGeo->NumSensorsXorY(); 
            fNumSensorsU = myGeo->NumSensorsU(); 
            fNumSensorsV = myGeo->NumSensorsV();
	  }
	  this->ReLoad(); 
	}
        inline void SetMaximumRolls(double r) {
	  fLimRolls = r;
	  this->ReLoad(); 
	}
	inline void SetMode(const std::string &m) { 
	  std::cerr << " SSDAlignParams::SetMode , new mode " << m << " previous mode " << fMode <<  std::endl;
	  const bool doReload = (fMode != m);  fMode = m; 
	  if (doReload) std::cerr << " ..... Reloading !... " << std::endl;
	  else std::cerr << " ..... No mode change, Not Reloading !... " << std::endl;
	  if (doReload) this->ReLoad(); 
	}
	inline void SetSpecificMinuitFixes(char specificStation, int SpecificSensor);  // must be called before SetMinuitParamFixes
	inline void SetMoveLongByStation(bool m) { 
	  const bool doReload = (fMoveLongByStation != m);  fMoveLongByStation = m; if (doReload) this->ReLoad(); 
	}
	inline void SetStrictSt6(bool m) {
	  const bool doReload = (fStrictSt6 != m);  fStrictSt6 = m; if (doReload) this->ReLoad(); 
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
	inline void SetValue(const std::string &aName, double val) {
	   for (std::vector<SSDAlignParam>::iterator it = fDat.begin(); it != fDat.end(); it++) {
	     if (it->Name() == aName) { it->SetValue(val); return;}
	   }
	}  
	inline void SetSoftLimits(bool u) { fUseSoftLimits = u; } 
	inline void SetDoubleGaps(double v) {
	  if (fIsPhase1c) return; // Not applicable, sensors are overlapping.. 
	  for (std::vector<SSDAlignParam>::iterator it = fDat.begin(); it != fDat.end(); it++) {
	   if (it->Type() != emph::rbal::TRSHIFT) continue;
	   switch (it->View()) {
	      case  'X' : {
	       if (it->SensorI() < 4) break;
	       if ((it->SensorI() % 2) == 0) it->SetValue(-v); else  it->SetValue(v);
	       break;
	     }
	      case  'Y' : {
	       if (it->SensorI() < 4) break;
	       if ((it->SensorI() % 2) == 0)  it->SetValue(-v); else  it->SetValue(v);
	       break;
	     }
	      case  'V' : {
	       if ((it->SensorI() % 2) == 1) it->SetValue(-v); else  it->SetValue(v); // not sure of these signs 
	       break;
	     }
	     default: // assume U is for Station 2 & 3 
	       break;
	    }
	  }
	  myGeo->SetDeltaTr('Y', 7, v);
	}
	inline void SetVerticalY8(double v) {
	  if (!fIsPhase1c) return; // Not applicable, Unsing double gap for Phase1b. 
	  myGeo1c->SetDeltaTr('Y', 8, v);
	}  
	  
	inline void SetSpecificSensor(int sensorIndex) { fSpecificSensor = sensorIndex; }
	inline void SetSpecificView(char aView) { fSpecificView = aView; }
	
	// Getter 
        inline bool isSubFitTypeValid(const std::string &aType) {
	  for (std::vector<std::string>::const_iterator it = fSubTypeDirectory.cbegin(); it != fSubTypeDirectory.cend(); it++) {
	    if (aType == (*it)) return true;
	  }
	  return false;
	}
	inline std::string Mode() const { return fMode; } 
	inline size_t NumParams() const { return fDat.size(); }
	inline std::vector<SSDAlignParam>::iterator ItEnd() { return fDat.end(); }  
	inline std::vector<SSDAlignParam>::iterator ItBegin()  { return fDat.begin(); }  
	inline std::vector<SSDAlignParam>::const_iterator ItCEnd() { return fDat.cend(); }  
	inline std::vector<SSDAlignParam>::const_iterator ItCBegin()  { return fDat.cbegin(); }  
	inline size_t size() const { return fDat.size(); }
	inline std::vector<SSDAlignParam>::const_iterator It(size_t kPar)  { 
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
	inline void  FixMinuitParam(paramType t, char view, size_t sensor, bool isFixed=true)  {
	  if ((t == emph::rbal::ZMAGC) || (t == emph::rbal::KICKMAGN)) {
	    for (std::vector<SSDAlignParam>::iterator it = fDat.begin(); it != fDat.end(); it++) 
	      if (t != it->Type()) { it->SetFixedInMinuit(isFixed); return;}
	    std::cerr << " SSDAlignParams::It , undefined paramtype related to the magnet " << t << " Fatal... " << std::endl;
	    MPI_Finalize(); exit(2);
	    return; 
	  }
	  for (std::vector<SSDAlignParam>::iterator it = fDat.begin(); it != fDat.end(); it++) {
	    if (t != it->Type()) continue;
	    if (view != it->View()) continue;
	    if (sensor != it->SensorS()) continue;
	    it->SetFixedInMinuit(isFixed);
	  }
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
        inline double Value(paramType t, char view, size_t sensor) const {
	  std::vector<SSDAlignParam>::const_iterator it=this->Itc(t, view, sensor);
	  if (it == fDat.cend()) return DBL_MAX;
	  return it->Value();
	}
        inline double Value(size_t k) const {
	  std::vector<SSDAlignParam>::const_iterator it = fDat.cbegin(); 
	  for (size_t kk=0; kk!= k; kk++) { 
	    it++;  if (it == fDat.cend()) return DBL_MAX;
	  }
	  return it->Value();
	}
	//
	// Checks
	//
	void SetMinuitParamFixes(const std::string &fitSubtype, int pencilBeamMode=0); 
	 // Last argument: Phase1b only, 
	 // if 1, 120 Gev beam, sigma x ~ sigmay = 1.5 mm spot size, x=-3.8, y=+4.5 (little or no W view).  
	 // if 0, assume broad beam, centered, all sensors can be moved, subject to fitSubTypeMode. 
	 // if -1, leave the pencil beam paramters fixes, move the others.
	//
	// IO 
	//
	void DumpTable(const std::string &token) const; // Asci, CSV format 
	//
	// To unload some of the messy and lengthy statements in the main program. 
	//
	void SetParamsForG4EMPHRun5c();
	void SetParamsForG4EMPHRun6g();
	void SetParamsForG4EMPHRun6h();
	
	
	void LoadValueFromPreviousRun(const std::string token, bool isSimplex=false); 
	void LoadValueFromPreviousFCNHistory(const std::string token, int requestedNCallNumber = INT_MAX); 
	void RandomizeRollsAndRollCenters(double rollW = 0.33333, double rollCW = 0.33333); 
	void FixParamsForView(const char aView, bool isFixed=true, const std::string &paramName=std::string ("")); 
	void FixParamsForViewLastStation(const char aView, bool isFixed=true); 
	void FixParamsForAllViewsAtStation(const int kSt, bool isFixed=true); 
	
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
	void CheatWithTruthValues(); // back doorr to upload true value, to debug Truth table 
    };
  } // rbal 
} // emph
#endif // 
