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
#include "Geometry/Geometry.h"
#include "RecoBase/SSDCluster.h" 

namespace emph{ 
  namespace ssdr {
    
    class VolatileAlignmentParams {
      private: 
        VolatileAlignmentParams();
        const size_t fNumStations = 6; // For Phase1b 
        const size_t fNumSensorsXorY = 8; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2 
        const size_t fNumSensorsU = 8; // Station 2 and 3, one sensor each, or conversely (new convention) Station 4 and 5
        const size_t fNumSensorsV = 8; // Have not made my mind yet, so over dimension  
	const size_t fNumStrips = 639; // Inconsistent with the height of the Sensors, by one 60 micron pitch  
	//
	double fZCoordsMagnetCenter, fMagnetKick120GeV;
	double fPitch;
	double fWaferWidth;
	double fHalfWaferWidth;
	double fZPosErr; // assume that all the station (excluding station 0) Z position have 
	                 // an alignment uncertainty, longitudinal.
	// all dimension to the number of sensors (or SSD wafer), view by view. , in order of increasing Z 
	// We should not have nominal positions here, but they are conveninent to have here.. 
	std::vector<double> fZNomPosX, fZNomPosY; 
	std::vector<double> fZNomPosSt2and3, fZNomPosSt4and5; // Z position, nominal, as is in phase1b.gdml  
 	std::vector<double> fZDeltaPosX, fZDeltaPosY; 
	std::vector<double> fZDeltaPosSt2and3, fZDeltaPosSt4and5; // Z position tweaks, as determined by this package, from the multiBT fitter.  
	std::vector<double> fTrNomPosX, fTrNomPosY; 
	std::vector<double> fTrNomPosSt4and5, fTrNomPosSt2and3; // Transverse position, nominal 
 	std::vector<double> fTrDeltaPosX, fTrDeltaPosY;
	std::vector<double> fTrDeltaPosSt2and3, fTrDeltaPosSt4and5; // Transverse tweaks. 
 	std::vector<double> fTrDeltaPitchX, fTrDeltaPitchY, fTrDeltaPitchV, fTrDeltaPitchU; // Yaw or Pitch angle, leading to a reduced pitch. 
	// Pitch = nominal Pitch * ( 1 - fTrDeltaPitchX), as cos(Yaw) ~ (1.0 - Yaw*yaw)  DeltaPitch always a positive quantity. 
 	std::vector<double> fRollX, fRollY, fRollV, fRollU; // Roll angle,
	// For intance, for X view,  x -> x + y*fRollX, where cos(roll angle) ~ 1.0 and sin(roll angle) ~ roll angle
	// Although not strictly geometrical, the following uncertainties are part of this singleton. 
	// Added May 21 -23 2023 : The center of rotation is unlikely to be the center of wafer, or the beam axis.  Allow for one more parameter per Sensor. 
 	std::vector<double> fRollXC, fRollYC, fRollVC, fRollUC; // Roll angle Centers
	std::vector<double> fMultScatUncertXorY, fMultScatUncertV, fMultScatUncertU;
	std::vector<double> fUnknownUncertXorY, fUnknownUncertV, fUnknownUncertU;
	//
	// Internal variables, for quick access 
	std::vector<double> fZPosX, fZPosY, fZPosSt4and5, fZPosSt2and3; 
	std::vector<double> fTrPosX, fTrPosY, fTrPosSt4and5, fTrPosSt2and3; 
	 
	static VolatileAlignmentParams* instancePtr;
	
      public:
      
        inline static VolatileAlignmentParams* getInstance() {
	 if ( instancePtr == NULL ) instancePtr = new VolatileAlignmentParams();
	 return instancePtr;
	}
        
	void UpdateNominalFromStandardGeom(emph::geo::Geometry *theGeo); 
	
	
        VolatileAlignmentParams(const VolatileAlignmentParams&) = delete;   // no copy constructor. 
	
	
	
	// Setters 
	void SetZPosErr(double v) { fZPosErr = v; } 
	void SetDeltaZ(emph::geo::sensorView view, size_t sensor, double value);  // move individual sensors. For X, & Y, sensor ranges from 0 to fNumSensorsXoY 
	void SetDeltaZStation(emph::geo::sensorView view, size_t aStation, double value); // move individual stations.. 
	void SetDeltaTr(emph::geo::sensorView view, size_t kSe, double value); 
	void SetValueTrShiftLastPlane(emph::geo::sensorView view, double value);
	void SetRoll(emph::geo::sensorView view, size_t kSe, double value); 
	void SetRollCenter(emph::geo::sensorView view, size_t kSe, double value); 
	void SetDeltaPitchCorr(emph::geo::sensorView view, size_t kSe, double value); 
	void SetUnknwonUncert(emph::geo::sensorView view,  size_t kSe, double v);
	void SetMultScatUncert(emph::geo::sensorView view,  size_t kSe, double v);
	inline void SetZCoordsMagnetCenter(double v) { fZCoordsMagnetCenter = v; } 
	inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; } 
	//
	// Monte-Carlo studies. April-May-June 2023. 
	//
	void SetTransShiftFor4c5c6c(bool correctResid57=false, double factBad=1.0);
	void SetGeomFromSSDAlign(const std::string &aFileName);  
	
	// Getter 
	inline size_t NumStations() const { return fNumStations; } 
	inline size_t NumSensorsXorY() const { return fNumSensorsXorY; } 
	inline size_t NumSensorsU() const { return fNumSensorsU; } 
	inline size_t NumSensorsV() const { return fNumSensorsV; } 	
	inline size_t NumSensorsW() const { return fNumSensorsV; }
	inline size_t NumStrips() const { return fNumStrips; }
	inline double ZCoordsMagnetCenter() const { return fZCoordsMagnetCenter; } 
	inline double MagnetKick120GeV() const { return fMagnetKick120GeV; } 
	inline double ZPosErr() const { return fZPosErr; } 
	inline double ZPos(emph::geo::sensorView view, size_t kSt, size_t kSe=0) {  // Relevant ndex is the Station index for X and Y,  
	// Ugly.... Valid only for Phase1b Moreover, I possibility of swap U vs V.. Code in such a way we can 
	// figure this out.. Rely on the plane ordering, which does not depend on the stereo angle, for U vs W 
	  if (((kSt == 2) || (kSt == 3)) && (kSe == 0)) return fZPosSt2and3[kSt-2]; 
	  if (((kSt == 4) || (kSt == 5)) && (kSe > 3)) return fZPosSt4and5[(kSt-4)*2 + kSe % 2]; 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt; // kS is the index into the View array, ranging from 0 to 7, inclusive (Phase1b 
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fZPosX[kS]);  
	    } 
	    case emph::geo::Y_VIEW :  { 
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return (fZPosY[kS]); } 
	    case emph::geo::U_VIEW :  { std::cerr << " VolatileAlignmentParams::ZPos, real mess, unexpected case U_VIEW, fatal...  " << std::endl; exit(2); } 
	    case emph::geo::W_VIEW :  { std::cerr << " VolatileAlignmentParams::ZPos, real mess, unexpected case W_VIEW, fatal...  " << std::endl; exit(2); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::ZPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	  return 0.;  // Should never happen.. 
	}
	inline double TrPos(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if (((kSt == 2) || (kSt == 3)) && (kSe == 0)) return fTrPosSt2and3[kSt-2]; 
	  if (((kSt == 4) || (kSt == 5)) && (kSe >3)) return fTrPosSt4and5[(kSt-4)*2 + kSe % 2]; 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
//	     std::cerr << " VolatileAlignmentParams::TrPos, X,  kSt " << kSt << " kSe " << kSe << " kS " 
//	               << kS <<  " X nom " << fTrNomPosX[kS] << " delta " << fTrDeltaPosX[kS] <<  " pos " << fTrPosX[kS] << std::endl;
	     return fTrPosX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  { 
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
//	     std::cerr << " VolatileAlignmentParams::TrPos, Y,  kSt " << kSt << " kSe " << kSe << " kS " 
//	               << kS <<  " Y nom " << fTrNomPosY[kS] << " delta " << fTrDeltaPosY[kS] << " pos " << fTrPosY[kS] <<  std::endl;
	     return fTrPosY[kS]; } 
	     // Carefull.. if we swap the U vs W definition of Station 2&3 vs Station 4& 5 Sept 4 2023. 
//	    case emph::geo::U_VIEW :  { return (fTrPosSt2and3[kSt-2]); } 
//	    case emph::geo::W_VIEW :  { return (fTrPosSt4and5[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::TrPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double DeltaPitch(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Pitch Inconsistent notation from above!!!!!   
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
	      std::cerr << " VolatileAlignmentParams::Roll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	inline double RollCenter(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fRollXC[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fRollYC[kS]; 
	    } 
	    case emph::geo::U_VIEW :  { return (fRollUC[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fRollVC[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::RollCenter, unknown view " << view << " fatal, quit " << std::endl; 
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
