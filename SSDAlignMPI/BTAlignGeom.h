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
        const size_t fNumSensorsW = 4; // a.k.a V view.. Following notation in SSDReco/SSDRecStationDwnstrAlgo1 Station 4 and 5, one sensor each 
        const size_t fNumSensorsU = 2; // Station 2 and 3, one sensor each   
	const size_t fNumStrips = 639; // to be verified at some point.. Consitent with the gdml, I think.. 
	//
	double fZCoordsMagnetCenter; 
	double fMagnetKick120GeV;
	double fPitch;
	double fWaferWidth;
	double fHalfWaferWidth;
	double fIntegrationStepSize;
	// all dimension to the number of sensors (or SSD wafer), view by view. , in order of increasing Z 
	std::vector<double> fZNomPosX, fZNomPosY, fZNomPosSt2and3, fZNomPosSt4and5; // Z position, nominal, as is in phase1b.gdml  
 	std::vector<double> fZDeltaPosX, fZDeltaPosY, fZDeltaPosSt2and3, fZDeltaPosSt4and5; // Z position tweaks, as determined by this package, from the multiBT fitter.  
	std::vector<double> fTrNomPosX, fTrNomPosY, fTrNomPosSt2and3, fTrNomPosSt4and5; // Transverse position, nominal 
 	std::vector<double> fTrDeltaPosX, fTrDeltaPosY, fTrDeltaPosSt2and3, fTrDeltaPosSt4and5; // Transverse tweaks. 
 	std::vector<double> fTrDeltaPitchX, fTrDeltaPitchY, fTrDeltaPitchSt2and3, fTrDeltaPitchSt4and5; // Yaw or Pitch angle, leading to a reduced pitch. 
	// Pitch = nominal Pitch * ( 1 - fTrDeltaPitchX), as cos(Yaw) ~ (1.0 - Yaw*yaw)  DeltaPitch always a positive quantity. 
 	std::vector<double> fRollX, fRollY, fRollSt2and3, fRollSt4and5; // Roll angle,
 	std::vector<double> fRollXC, fRollYC, fRollSt2and3C, fRollSt4and5C; // Center of rotation with respect to the center of the coordinate system. 
	// For intance, for X view,  x -> x + y*fRollX, where cos(roll angle) ~ 1.0 and sin(roll angle) ~ roll angle
	// Although not strictly geometrical, the following uncertainties are part of this singleton. 
	// May 21 -22 : add a correction to the uncertainty on where the rotation center is.. 
	// So x -> x + ( y - fRollXC)*fRollX, where, it is likely that fRollXC is of the order of tens of mm.  
	std::vector<double> fMultScatUncertXorY, fMultScatUncertW, fMultScatUncertU;
	std::vector<double> fUnknownUncertX, fUnknownUncertY, fUnknownUncertW, fUnknownUncertU;
	//
	// Internal variables, for quick access 
	std::vector<double> fZPosX, fZPosY, fZPosSt2and3, fZPosSt4and5; 
	std::vector<double> fTrPosX, fTrPosY, fTrPosSt2and3, fTrPosSt4and5; 
	 
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
	void SetValueTrShiftLastPlane(char view, double value);
	void SetRoll(char view, size_t kSe, double value); 
	void SetRollCenter(char view, size_t kSe, double value); 
	void SetDeltaPitchCorr(char view, size_t kSe, double value); 
	void SetUnknownUncert(char view,  size_t kSe, double v);
	void SetUnknownUncert(char view,  double v); // all sensors within a view 
	void SetMultScatUncert(char view,  size_t kSe, double v);
	inline void SetZCoordsMagnetCenter(double v) { fZCoordsMagnetCenter = v; } 
	inline void SetMagnetKick120GeV(double v) { fMagnetKick120GeV = v; }
	inline void SetIntegrationStepSize(double s) { fIntegrationStepSize = s; } 
	
	// Getter 
	inline size_t NumStations() const { return fNumStations; } 
	inline size_t NumSensorsXorY() const { return fNumSensorsXorY; } 
	inline size_t NumSensorsU() const { return fNumSensorsU; } 
	inline size_t NumSensorsV() const { return fNumSensorsW; } 	
	inline size_t NumSensorsW() const { return fNumSensorsW; }
	inline size_t NumStrips() const { return fNumStrips; }
	inline double ZCoordsMagnetCenter() const { return fZCoordsMagnetCenter; } 
	inline double MagnetKick120GeV() const { return fMagnetKick120GeV; } 
	inline double IntegrationStepSize() const { return fIntegrationStepSize; } 
	
	inline double ZPos(char view, size_t kSe) {  // longitudinal 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fZPosX[kSe]);  
	    } 
	    case 'Y' :  { return (fZPosY[kSe]); } 
	    case 'U' :  { return (fZPosSt2and3[kSe]); } 
	    case 'V' :  { return (fZPosSt4and5[kSe]); }
	    case 'W' :  { return (fZPosSt4and5[kSe]); }
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
	    case 'V' :  { return (fTrPosSt4and5[kSe]); } 
	    case 'W' :  { return (fTrPosSt4and5[kSe]); }
	    case 'U' :  { return (fTrPosSt2and3[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::TrPos, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen.. 
	}
	inline double TrNominalPos(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fTrNomPosX[kSe]);  
	    } 
	    case 'Y' :  { return (fTrNomPosY[kSe]); } 
	    case 'V' :  { return (fTrNomPosSt4and5[kSe]); } 
	    case 'W' :  { return (fTrNomPosSt4and5[kSe]); }
	    case 'U' :  { return (fTrNomPosSt2and3[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::TrNomPos, unknown view " << view << " fatal, quit " << std::endl; 
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
	    case 'W' :  { return (fTrDeltaPitchSt4and5[kSe]); } 
	    case 'V' :  { return (fTrDeltaPitchSt4and5[kSe]); }
	    case 'U' :  { return (fTrDeltaPitchSt2and3[kSe]); }
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
	    case 'W' :  { return (fRollSt4and5[kSe]); } 
	    case 'V' :  { return (fRollSt4and5[kSe]); }
	    case 'U' :  { return (fRollSt2and3[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::Roll, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	inline double RollCenter(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fRollXC[kSe]);  
	    } 
	    case 'Y' :  { return (fRollYC[kSe]); } 
	    case 'W' :  { return (fRollSt4and5C[kSe]); } 
	    case 'V' :  { return (fRollSt4and5C[kSe]); }
	    case 'U' :  { return (fRollSt2and3C[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::RollCenter, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
	
	inline double UnknownUncert(char view, size_t kSe) { // Transverse 
          switch (view) {
	    case 'X' : {
//	     if (sensor >= fZNomPosX.size()) { std::cerr .... No checks!. 
	     return (fUnknownUncertX[kSe]);  
	    } 
	    case 'Y' :  { return (fUnknownUncertY[kSe]); } 
	    case 'U' :  { return (fUnknownUncertU[kSe]); } 
	    case 'V' : case 'W' : { return (fUnknownUncertW[kSe]); }
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
	    case 'V' :  case 'W' :{ return (fMultScatUncertW[kSe]); }
	    default : { 
	      std::cerr << " BTAlignGeom::MultScatUncert, unknown view " << view << " fatal, quit " << std::endl; 
	      exit(2);  } 
	   }
	  return 0.;  // Should never happen..
	}
        inline double Pitch(char view, size_t kSe) { return (fPitch * (1.0 - this->DeltaPitch(view, kSe))); }
	//
	// to handle the 120 GeV pencil beam 
	void SetUncertErrorOutOfPencilBeam() ; 
	//
	// Moving the stations.... Study the longitudinal shifts. 
	//
	void MoveZPosOfXUVByY();
    };
  } // rbal 
} // emph
#endif // 
