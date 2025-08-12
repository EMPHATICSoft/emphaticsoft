////////////////////////////////////////////////////////////////////////
/// \brief  a DELTA Geometry, SSD only, data for the SSD aligner 
///          Used temporarely Autre package.   
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
//        const size_t fNumStations = 6; // For Phase1b 
        const size_t fNumStations = 8; // For Phase1b, and Phase1c 
        const size_t fNumSensorsXorY = 11; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2= 8 Phase1c : add 2 stations, one double sensor.  
        const size_t fNumSensorsU = 8; // Station 2 and 3, one sensor each, or conversely (new convention) Station 4 and 5
	                               // Phace 1c
        const size_t fNumSensorsV = 8; // Have not made my mind yet, so over dimension for phase1c and phase1b 
        const size_t fNumSensorsW = 8; // Same as above, V earlier notation, W is the one used by Teresa and Linyan.  
	// October 2023: For phase1b, I made up my mind, but could not present the result, and Jonathan has no time/patience 
	// to sort this out..  Phase1c : 3*2 + 2 dterao views. 
	const size_t fNumStrips = 640; // Inconsistent with the height of the Sensors, by one 60 micron pitch  
	//
	bool fIsPhase1c; 
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
	std::vector<double> fZNomPos1cSt2and3W, fZNomPos1cSt5and6W; // Z position, nominal, Phase1c. for Stereo angle views.
	//  fixing the fixing:  The geometry has been fixd on the main branch...   
 	std::vector<double> fZDeltaPosX, fZDeltaPosY; 
	std::vector<double> fZDeltaPosSt2and3, fZDeltaPosSt4and5; // Z position tweaks, as determined by this package, from the multiBT fitter. 
	std::vector<double> fZDeltaPos1cSt2and3W, fZDeltaPos1cSt5and6W; // Z position, Deltas, Phase1c. for Stereo angle views.  
	std::vector<double> fTrNomPosX, fTrNomPosY; 
	std::vector<double> fTrNomPosSt4and5, fTrNomPosSt2and3; // Transverse position, nominal 
	std::vector<double> fTrNomPos1cSt2and3W, fTrNomPos1cSt5and6W; // Transverse position, nominal, stereo...  
 	std::vector<double> fTrDeltaPosX, fTrDeltaPosY;
	std::vector<double> fTrDeltaPosSt2and3, fTrDeltaPosSt4and5; // Transverse tweaks. 
	std::vector<double> fTrDeltaPos1cSt2and3W, fTrDeltaPos1cSt5and6W; // Transverse position, tweaks, stereo...  
 	std::vector<double> fTrDeltaPitchX, fTrDeltaPitchY;
	std::vector<double> fTrDeltaPitchSt2and3, fTrDeltaPitchSt4and5; // Yaw or Pitch angle, leading to a reduced pitch. 
	// Pitch = nominal Pitch * ( 1 - fTrDeltaPitchX), as cos(Yaw) ~ (1.0 - Yaw*yaw)  DeltaPitch always a positive quantity. 
 	std::vector<double> fRollX, fRollY;
	std::vector<double> fRollSt2and3, fRollSt4and5; // Roll angle,
	std::vector<double> fRoll1cSt2and3W, fRoll1cSt5and6W; // Z position, Deltas, Phase1c. for Stereo angle views.  
	// For intance, for X view,  x -> x + y*fRollX, where cos(roll angle) ~ 1.0 and sin(roll angle) ~ roll angle
	// Although not strictly geometrical, the following uncertainties are part of this singleton. 
	// Added May 21 -23 2023 : The center of rotation is unlikely to be the center of wafer, or the beam axis.  Allow for one more parameter per Sensor. 
 	std::vector<double> fRollXC, fRollYC;
	std::vector<double> fRollSt2and3C, fRollSt4and5C; // Roll angle Centers
	std::vector<double> fRoll1cSt2and3WC, fRoll1cSt5and6WC; // Roll Centers, Phase1c. for Stereo angle views.  
	std::vector<double> fMultScatUncertXorY, fMultScatUncertSt2and3, fMultScatUncertSt4and5;
	std::vector<double> fMultScatUncertSt2and31c, fMultScatUncertSt5and61c;
	std::vector<double> fUnknownUncertXorY, fUnknownUncertSt2and3, fUnknownUncertSt4and5;  
	  // Not implemented for 1C, assume nearly identical to 1b. Also, plan to use the Kalman filter.. 
	//
	// Internal variables, for quick access .. Includes the deltas 
	std::vector<double> fZPosX, fZPosY, fZPosSt4and5, fZPosSt2and3, fZPos1cSt5and6W, fZPos1cSt2and3W; 
	std::vector<double> fTrPosX, fTrPosY, fTrPosSt4and5, fTrPosSt2and3, fTrPos1cSt5and6W, fTrPos1cSt2and3W; 
	 
	static VolatileAlignmentParams* instancePtr;
	
      public:
        
	inline void SetPhase1X(int aRunNum) {
	   fIsPhase1c = (aRunNum > 1999); 
	   if (!fIsPhase1c) return;
	   std::cerr << " VolatileAlignmentParams::SetPhase1X, using Phase1c setting " << std::endl;
	} 
	
        inline static VolatileAlignmentParams* getInstance() {
	 if ( instancePtr == NULL ) instancePtr = new VolatileAlignmentParams();
	 return instancePtr;
	}
        
	void UpdateNominalFromStandardGeom(emph::geo::Geometry *theGeo); 
	
	
        VolatileAlignmentParams(const VolatileAlignmentParams&) = delete;   // no copy constructor. 
	
	
	
	// Setters 
	void SetZPosErr(double v) { fZPosErr = v; } 
	void SetDeltaZ(emph::geo::sensorView view, size_t sensor, double value);  
	// move individual sensors. For X, & Y, sensor ranges from 0 to fNumSensorsXoY 
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
	  if (fIsPhase1c) return ZPos1c(view, kSt, kSe); // Also ugly... 
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
	    case emph::geo::U_VIEW :  { 
	       //  std::cerr << " VolatileAlignmentParams::ZPos, real mess, unexpected case U_VIEW, fatal...  " << std::endl; exit(2); } 
	      return fZPosSt2and3[kSt-2];
	    }  
	    case emph::geo::W_VIEW :  { 
	    // std::cerr << " VolatileAlignmentParams::ZPos, real mess, unexpected case W_VIEW, fatal...  " << std::endl; exit(2); }
	      return fZPosSt4and5[2*(kSt-4)];
	    }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::ZPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	  return 0.;  // Should never happen.. 
	}
	inline double ZPos1c(emph::geo::sensorView view, size_t kSt, size_t kSe=0) {  // Relevant ndex is the Station index for X and Y, 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt; // kS is the index into the View array, ranging from 0 to 7, inclusive (Phase1b 
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fZPosX[kS]);  
	    } 
	    case emph::geo::Y_VIEW :  { 
	      size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt; // kS is the index into the View array, ranging from 0 to 7, inclusive (Phase1b 
	      return (fZPosY[kS]); } 
	    case emph::geo::U_VIEW :  { 
	      std::cerr << " VolatileAlignmentParams::ZPos1c  No U view in Phase1c !! Fatal... " << std::endl; exit(2); 
	    }  
	    case emph::geo::W_VIEW :  {
	      if (kSt < 5) {
	        return fZPos1cSt2and3W[2*(kSt-2)];
	      } else {   
	        return fZPos1cSt5and6W[2*(kSt-5)];
	      }
	    }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::ZPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	  }
	  return 0.;  // Should never happen.. 
	
	
	
	} 
	inline double TrPos(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if (fIsPhase1c) return TrPos1c(view, kSt, kSe);
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
	    case emph::geo::U_VIEW :  { return (fTrPosSt2and3[kSt-2]); } 
	    case emph::geo::W_VIEW :  { return (fTrPosSt4and5[(kSt-4)*2 + kSe % 2]); }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::TrPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double TrPos1c(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
//	     std::cerr << " VolatileAlignmentParams::TrPos1c, X,  kSt " << kSt << " kSe " << kSe << " kS " 
//	               << kS <<  " X nom " << fTrNomPosX[kS] << " delta " << fTrDeltaPosX[kS] <<  " pos " << fTrPosX[kS] << std::endl;
	     return fTrPosX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  { 
	     size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	     return fTrPosY[kS]; } 
	     // Carefull.. if we swap the U vs W definition of Station 2&3 vs Station 4& 5 Sept 4 2023. 
	    case emph::geo::U_VIEW :  { std::cerr << " VolatileAlignmentParams::TrPos1c, No such U view!! Fatal " << std::endl; exit(2); } 
	    case emph::geo::W_VIEW :  {
	      if (kSt < 4) {
//	              if (kSt == 3) {
//		        std::cerr << " TrPos1c, debugging, kSt " << kSt << " kSe " << kSe << " fTrPos1cSt2and3W " 
//			          << fTrPos1cSt2and3W[kSt-2] << std::endl;
//			std::cerr << " And quit now..... " << std::endl;
//			exit(2);
//	              }
	              return (fTrPos1cSt2and3W[kSt-2]);
		 } else { return (fTrPos1cSt5and6W[(kSt-5)*2 + kSe % 2]); } 
	    }
	    default : { 
	      std::cerr << " VolatileAlignmentParams::TrPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double DeltaPitch(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Pitch    
	  if (fIsPhase1c) return 0.; // Don't bother... for now.. 
	  if (((kSt == 2) || (kSt == 3)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW))) 
	    return  fTrDeltaPitchSt2and3[kSt-2];
	  if (((kSt == 4) || (kSt == 5)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fTrDeltaPitchSt4and5[(kSt-4)*2 + kSe % 2];
	     
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fTrDeltaPitchX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  { 
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fTrDeltaPitchY[kS]; } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double Roll(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if (fIsPhase1c) return Roll1c (view, kSt, kSe) ; 
	  if (((kSt == 2) || (kSt == 3)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fRollSt2and3[kSt-2];
	  if (((kSt == 4) || (kSt == 5)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fRollSt4and5[(kSt-4)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fRollX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fRollY[kS]; 
	    } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::Roll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	inline double Roll1c(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if ((kSt == 2) || (kSt == 3)) {
	      if (view == emph::geo::U_VIEW)  { 
                   std::cerr << " VolatileAlignmentParams::Roll1c, no such U view in Phase1c, fatal " << std::endl; 
		   exit(2); 
	      }
	      if (view == emph::geo::W_VIEW) return  fRoll1cSt2and3W[kSt-2];
	  }
	  if (((kSt > 4) && (view == emph::geo::W_VIEW)))
	    return  fRoll1cSt5and6W[(kSt-5)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	     return fRollX[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	      return fRollY[kS]; 
	    } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::Roll1c, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	inline double RollCenter(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if (fIsPhase1c) return Roll1cCenter (view, kSt, kSe) ; 
	  if (((kSt == 2) || (kSt == 3)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fRollSt2and3C[kSt -2];
	  if (((kSt == 4) || (kSt == 5)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fRollSt4and5C[(kSt-4)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fRollXC[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fRollYC[kS]; 
	    } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::RollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	
	inline double Roll1cCenter(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	   
	  if ((kSt == 2) || (kSt == 3)) {
	      if (view == emph::geo::U_VIEW) {
	        std::cerr << " VolatileAlignmentParams::Roll1cCenter, no U view, fatal, " << std::endl; exit(2);
	      }
	      if (view == emph::geo::W_VIEW) return  fRoll1cSt2and3WC[kSt-2];
	  }
	  if (((kSt > 4) && (view == emph::geo::W_VIEW)))
	    return  fRoll1cSt5and6WC[(kSt-5)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	     return fRollXC[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	      return fRollYC[kS]; 
	    } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::Roll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	inline double UnknownUncert(emph::geo::sensorView view, size_t kSt,  size_t kSe) { // Transverse 
	  if (fIsPhase1c) return 0.; // don't bother, it was just to make sure 
	  if (((kSt == 2) || (kSt == 3)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fUnknownUncertSt2and3[kSt-2];
	  if (((kSt == 4) || (kSt == 5)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fUnknownUncertSt4and5[(kSt-4)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	     size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	     return fUnknownUncertXorY[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fUnknownUncertXorY[kS]; 
	    } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	   
	inline double MultScatUncert(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if  (fIsPhase1c) return MultScatUncert1c(view, kSt, kSe); 
	  if (((kSt == 2) || (kSt == 3)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fMultScatUncertSt2and3[kSt-2];
	  if (((kSt == 4) || (kSt == 5)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fMultScatUncertSt4and5[(kSt-4)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fMultScatUncertXorY[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 3) ? (4 + (kSt-4)*2 + kSe % 2) : kSt;
	      return fMultScatUncertXorY[kS]; 
	    } 
	    default : { 
	      std::cerr << " VolatileAlignmentParams::DeltaPitch, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	
 	inline double MultScatUncert1c(emph::geo::sensorView view, size_t kSt, size_t kSe) { // Transverse 
	  if (((kSt == 2) || (kSt == 3)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fMultScatUncertSt2and31c[kSt-2];
	  if (((kSt == 5) || (kSt == 6)) && ((view == emph::geo::U_VIEW) || (view == emph::geo::W_VIEW)))
	    return  fMultScatUncertSt5and61c[(kSt-5)*2 + kSe % 2];
          switch (view) {
	    case emph::geo::X_VIEW : {
	      size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	      return fMultScatUncertXorY[kS];  
	    } 
	    case emph::geo::Y_VIEW :  {
	      size_t kS =  (kSt > 4) ? (5 + (kSt-5)*2 + kSe % 2) : kSt;
	      return fMultScatUncertXorY[kS]; 
	    } 
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
