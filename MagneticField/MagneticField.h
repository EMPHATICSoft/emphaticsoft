///////////////////////////////////////////////////////////////////////////////
// File: MagneticField.h
// Description: A class for control of the Magnetic Field of the detector.
///////////////////////////////////////////////////////////////////////////////
#ifndef EMPHMagneticField_H
#define EMPHMagneticField_H

#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
//#include "MagneticField/FieldMap.h"
#include <map>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <string>

#include "TH3F.h"

class G4FieldManager;

namespace emph {

  struct bFieldPoint {
    float fbx, fby, fbz; // in kG, internally. 
  };

  class MagneticField { // : public G4MagneticField {
  public:
    MagneticField(); 
    ~MagneticField();  
    
    // Access functions
    void Field(const double Point[3], double Bfield[3]);
    
    void GetFieldValue(const double Point[3], double* Bfield); // units are mm, return values in kilogauss
    void SetFieldFileName(std::string fileName) { fFieldFileName = fileName; }
    void SetVerbosity(int v) { fVerbosity = v; }
    void SetUsingRootHistos(bool flag=false) { fUsingRootHistos=flag; }

  private:
    std::string fFieldFileName;
    bool  fFieldLoaded;
    bool fStorageIsStlVector; // We fill ffield, the stl vector<bFieldPoint>  if true.  else, the stl map of stl map... 
    std::vector<bFieldPoint> fBfield;
    std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, std::vector<double> > > > fFieldMap;
    double step;
    double start[3]; // old boundaries.. 
    double fG4ZipTrackOffset[3];
    int fNStepX, fNStepY, fNStepZ;
    double fXMin, fYMin, fZMin, fXMax, fYMax, fZMax; // New ones, used 
    double fStepX, fStepY, fStepZ; 
    int fInterpolateOption;
    int fVerbosity;
    bool  fUsingRootHistos;
    TH3F* fBx3DHisto;
    TH3F* fBy3DHisto;
    TH3F* fBz3DHisto;

  public: 
    void SetUseStlVector(bool useVec) { fStorageIsStlVector = useVec; }
    inline void setInterpolatingOption(int iOpt) { fInterpolateOption = iOpt; } // iOpt = 0 => 3D radial average , 1 linearized along axes of the 3D grid. 
    
    void Integrate(int iOpt, int charge, double stepAlongZ,  
		   std::vector<double> &start, std::vector<double> &end); 
    //    inline void setUseOnlyTheCentralPart(bool  t=true) {  fUseOnlyCentralPart = t; }    
  private:
    void uploadFromTextFile();
    void LoadRootHistos();
    void AlignWithGeom();
    void CalcFieldFromVector(const double Point[3], double* Bfield);
    void CalcFieldFromMap(const double Point[3], double* Bfield);
    void CalcFieldFromRootHistos(const double Point[3], double* Bfield);

    inline size_t indexForVector(double *xyz) const {
      double *ptr = xyz; 
      // floor seems to fail if close to real boundary, so add a tiny offset
      // see NoteOnDoubleFromASCII
      size_t iX = static_cast<size_t>(floor(((*ptr) + 1.0e-10  - fXMin)/fStepX)); ptr++; 
      size_t iY = static_cast<size_t>(floor(((*ptr) + 1.0e-10 - fYMin)/fStepY)); ptr++;
      size_t iZ = static_cast<size_t>(floor(((*ptr) + 1.0e-10 - fZMin)/fStepZ));  
      return (static_cast<size_t>(fNStepZ*fNStepY) * iX + static_cast<size_t>(fNStepZ) * iY + iZ);
    } 

    inline size_t indexForVector(size_t iX, size_t iY, size_t iZ) const {
      return (static_cast<size_t>(fNStepZ*fNStepY) * iX + static_cast<size_t>(fNStepZ) * iY + iZ);
    } 

  };
  
} // end namespace emph

#endif
