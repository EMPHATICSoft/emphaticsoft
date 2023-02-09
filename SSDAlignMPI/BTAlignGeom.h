////////////////////////////////////////////////////////////////////////
/// \brief  The Geometry, SSD only, data for the SSD aligner 
///          Used by main SSD Aligner Algo1 package.   
///          This is a singleton class. 
///          Units are mm, as default G4 units.  Angle in radians. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef BTALIGNGEOM_H
#define BTALIGNGEOM_H

#include <vector>
#include <stdint.h>
#include <iostream>

//  #include "BeamTrackCluster.h" not needed

namespace emph{ 
  namespace rbal {
    
    class BTAlignGeom {
      private: 
        BTAlignGeom();
        const size_t fNumStations = 6; // For Phase1b 
        const size_t fNumSensorsXorY = 8; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2 
        const size_t fNumSensorsU = 2; // Station 2 and 3, one sensor each 
        const size_t fNumSensorsV = 4; // Station 4 and 5, two sensors each 
	const size_t fNumStrips = 639; // to be verified at some point.. Consitent with the gdml, I think.. 
	//
	double fZCoordsMagnetCenter; 
	double fMagnetKick120GeV;
	double fPitch;
	double fWaferWidth;
	double fHalfWaferWidth;
	// all dimension to the number of sensors (or SSD wafer), view by view. , in order of increasing Z 
	std::vector<double> fZNomPosX, fZNomPosY, fZNomPosU, fZNomPosV; // Z position, nominal, as is in phase1b.gdml  
 	std::vector<double> fZDeltaPosX, fZDeltaPosY, fZDeltaPosU, fZDeltaPosV; // Z position tweaks, as determined by this package, from the multiBT fitter.  
	std::vector<double> fTrNomPosX, fTrNomPosY, fTrNomPosU, fTrNomPosV; // Transverse position, nominal 
 	std::vector<double> fTrDeltaPosX, fTrDeltaPosY, fTrDeltaPosU, fTrDeltaPosV; // Transverse tweaks. 
 	std::vector<double> fTrDeltaPitchX, fTrDeltaPitchY, fTrDeltaPitchU, fTrDeltaPitchV; // Yaw or Pitch angle, leading to a reduced pitch. 
	// Pitch = nominal Pitch * ( 1 - fTrDeltaPitchX), as cos(Yaw) ~ (1.0 - Yaw*yaw)  DeltaPitch always a positive quantity. 
 	std::vector<double> fRollX, fRollY, fRollU, fRollV; // Roll angle,
	// For intance, for X view,  x -> x + y*fRollX, where cos(roll angle) ~ 1.0 and sin(roll angle) ~ roll angle
	// Although not strictly geometrical, the following uncertainties are part of this singleton. 
	//
	std::vector<double> fMultScatUncertXorY, fMultScatUncertU, fMultScatUncertV;
	std::vector<double> fUnknownUncertXorY, fUnknownUncertU, fUnknownUncertV;
	//
	// Internal variables, for quick access 
	std::vector<double> fZPosX, fZPosY, fZPosU, fZPosV; 
	std::vector<double> fTrPosX, fTrPosY, fTrPosU, fTrPosV; 
	 
	static BTAlignGeom* instancePtr;
	
      public:
      
        inline static BTAlignGeom* getInstance() {
	 if ( instancePtr == NULL ) instancePtr = new BTAlignGeom();
	 return instancePtr;
	}
      
        BTAlignGeom(const BTAlignGeom&) = delete;   // no copy constructor. 
	
	// Setters 
	
	void SetDeltaZ(char view, size_t sensor, double value);  // move individual sensors. For X, & Y, sensor ranges from 0 to fNumSensorsXoY 
	void SetDeltaZStation(char view, size_t aStation, double value); // move individual stations.. 
	void SetDeltaTr(char view, size_t kSe, double value); 
	void SetRoll(char view, size_t kSe, double value); 
	void SetDeltaPitchCorr(char view, size_t kSe, double value); 
	void SetUnknwonUncert(char view,  size_t kSe, double v);
	inline void SetZCoordsMagnetCenter(double v) { fZCoordsMagnetCenter = v; } 
	inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; } 
	
	// Getter 
	inline size_t NumStations() const { return fNumStations; } 
	inline size_t NumSensorsXorY() const { return fNumSensorsXorY; } 
	inline size_t NumSensorsU() const { return fNumSensorsU; } 
	inline size_t NumSensorsV() const { return fNumSensorsV; } 	
	inline size_t NumSensorsW() const { return fNumSensorsV; }
	inline size_t NumStrips() const { return fNumStrips; }
	inline double ZCoordsMagnetCenter() const { return fZCoordsMagnetCenter; } 
	inline double MagnetKick120GeV() const { return fMagnetKick120GeV; } 
	
	inline double ZPos(char view, size_t kSe) {  // longitudinal 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fZPosX[kSe]);  
	    } 
	    case 'Y' :  { return (fZPosY[kSe]); } 
	    case 'U' :  { return (fZPosU[kSe]); } 
	    case 'V' :  { return (fZPosV[kSe]); }
	    case 'W' :  { return (fZPosV[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::ZPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	  return 0.;  // Should never happen.. 
	}
	inline double TrPos(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fTrPosX[kSe]);  
	    } 
	    case 'Y' :  { return (fTrPosY[kSe]); } 
	    case 'U' :  { return (fTrPosU[kSe]); } 
	    case 'V' :  { return (fTrPosV[kSe]); }
	    case 'W' :  { return (fTrPosV[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::TrPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double DeltaPitch(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fTrDeltaPitchX[kSe]);  
	    } 
	    case 'Y' :  { return (fTrDeltaPitchY[kSe]); } 
	    case 'U' :  { return (fTrDeltaPitchU[kSe]); } 
	    case 'V' :  { return (fTrDeltaPitchV[kSe]); }
	    case 'W' :  { return (fTrDeltaPitchV[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double Roll(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fRollX[kSe]);  
	    } 
	    case 'Y' :  { return (fRollY[kSe]); } 
	    case 'U' :  { return (fRollU[kSe]); } 
	    case 'V' :  { return (fRollV[kSe]); }
	    case 'W' :  { return (fRollV[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	
	inline double UnknownUncert(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fUnknownUncertXorY[kSe]);  
	    } 
	    case 'Y' :  { return (fUnknownUncertXorY[kSe]); } 
	    case 'U' :  { return (fUnknownUncertU[kSe]); } 
	    case 'V' : case 'W' : { return (fUnknownUncertV[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::UnknownUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	   
	inline double MultScatUncert(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : case 'Y' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fMultScatUncertXorY[kSe]);  
	    } 
	    case 'U' :  { return (fMultScatUncertU[kSe]); } 
	    case 'V' :  case 'W' :{ return (fMultScatUncertV[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::MultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
        inline double Pitch(char view, size_t kSe) { return (fPitch * (1.0 - this->DeltaPitch(view, kSe))); }
	    
    };
  } // rbal 
} // emph
#endif // 
