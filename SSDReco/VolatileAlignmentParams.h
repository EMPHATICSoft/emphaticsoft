////////////////////////////////////////////////////////////////////////
/// \brief  a DELTA Geometry, SSD only, data for the SSD aligner 
///          Used temporarely Algo1 package.   
///          This is a singleton class, could be, if need be part of the art services.  
///          Units are mm, as default G4 units.  Angle in radians. 
///          This only work for Phase1b, for now.. Number of Stations and Sensors are hard-coded. 
///          Same sign conventions and organization as the SSDAlignMPI/BTAlignGeom class. 
///          In fact, a clone... rarely a good thing, but, remember, can't have an MPI application in the art framework. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef VOLATILEALIGNMENTPARAMS_H
#define VOLATILEALIGNMENTPARAMS_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include "RecoBase/SSDCluster.h" 

namespace emph{ 
  namespace ssdr {
    
    class VolatileAlignmentParams {
      private: 
        VolatileAlignmentParams();
        const size_t fNumStations = 6; // For Phase1b 
        const size_t fNumSensorsXorY = 8; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2 
        const size_t fNumSensorsU = 2; // Station 2 and 3, one sensor each 
        const size_t fNumSensorsV = 4; // Station 4 and 5, two sensors each 
	const size_t fNumStrips = 640; // Inconsistent with the height of the Sensors, by one 60 micron pitch  
	//
	double fZCoordsMagnetCenter, fMagnetKick120GeV;
	double fPitch;
	double fWaferWidth;
	double fHalfWaferWidth;
	// all dimension to the number of sensors (or SSD wafer), view by view. , in order of increasing Z 
	// We should not have nominal positions here, but they are conveninent to have here.. 
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
	 
	static VolatileAlignmentParams* instancePtr;
	
      public:
      
        inline static VolatileAlignmentParams* getInstance() {
	 if ( instancePtr == NULL ) instancePtr = new VolatileAlignmentParams();
	 return instancePtr;
	}
      
        VolatileAlignmentParams(const VolatileAlignmentParams&) = delete;   // no copy constructor. 
	
	// Setters 
	
	void SetDeltaZ(emph::geo::sensorView view, size_t sensor, double value);  // move individual sensors. For X, & Y, sensor ranges from 0 to fNumSensorsXoY 
	void SetDeltaZStation(emph::geo::sensorView view, size_t aStation, double value); // move individual stations.. 
	void SetDeltaTr(emph::geo::sensorView view, size_t kSe, double value); 
	void SetValueTrShiftLastPlane(emph::geo::sensorView view, double value);
	void SetRoll(emph::geo::sensorView view, size_t kSe, double value); 
	void SetDeltaPitchCorr(emph::geo::sensorView view, size_t kSe, double value); 
	void SetUnknwonUncert(emph::geo::sensorView view,  size_t kSe, double v);
	void SetMultScatUncert(emph::geo::sensorView view,  size_t kSe, double v);
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
	
	inline double ZPos(emph::geo::sensorView view, size_t kSt, size_t kSe) {  // Relevant ndex is the Station index for X and Y,  
	// Ugly.... Valid only for Phase1b  
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fZPosX[kS]);  
	    } 
	    case emph::geo::Y_VIEW :  { 
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return (fZPosY[kS]); } 
	    case emph::geo::U_VIEW :  { return (fZPosU[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fZPosV[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::ZPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	  return 0.;  // Should never happen.. 
	}
	inline double TrPos(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fTrPosX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  { 
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fTrPosY[kS]; } 
	    case emph::geo::U_VIEW :  { return (fTrPosU[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fTrPosV[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::TrPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double DeltaPitch(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fTrDeltaPitchX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  { 
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fTrDeltaPitchY[kS]; } 
	    case emph::geo::U_VIEW :  { return (fTrDeltaPitchU[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fTrDeltaPitchV[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double Roll(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fRollX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fRollY[kS]; 
	    } 
	    case emph::geo::U_VIEW :  { return (fRollU[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fRollV[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	
	inline double UnknownUncert(emph::geo::sensorView view, size_t kSt,  size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fUnknownUncertXorY[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fUnknownUncertXorY[kS]; 
	    } 
	    case emph::geo::U_VIEW :  { return (fUnknownUncertU[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fUnknownUncertV[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	   
	inline double MultScatUncert(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fMultScatUncertXorY[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fMultScatUncertXorY[kS]; 
	    } 
	    case emph::geo::U_VIEW :  { return (fMultScatUncertU[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fMultScatUncertV[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
        inline double Pitch(emph::geo::sensorView view, size_t kSt, size_t kSe) { return (fPitch * (1.0 - this->DeltaPitch(view, kSt, kSe))); }
	    
    };
  } // geo 
} // emph
#endif // 
