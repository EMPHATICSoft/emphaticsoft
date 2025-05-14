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
#include <cmath>
#include <vector>
#include <string>
#include "TH3F.h"

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
    bool fStayedInMap; // Stayed in Map while integrating.. 
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
    void SetUsingRootHistos(bool flag=false) { fUsingRootHistos=flag; }
    void SetUseStlVector(bool useVec) { fStorageIsStlVector = useVec; }
    inline void setInterpolatingOption(int iOpt) { fInterpolateOption = iOpt; } // iOpt = 0 => 3D radial average , 1 linearized along axes of the 3D grid. 
    
    void Integrate(int iOpt, int charge, double stepAlongZ,  
		   std::vector<double> &start, std::vector<double> &end); 
    //    inline void setUseOnlyTheCentralPart(bool  t=true) {  fUseOnlyCentralPart = t; }    
   inline bool didStayedInMap() { return fStayedInMap; }  // While integrating.. 
   //
   // Feb 2024:  Improve the performance of the Euler & RK4 integrator, by pre-ordaining the steps
   void SetIntegratorSteps(double minStep);
   //
   // assuming the above has been called.. Return false if we fall outside the 15 X 15 map inside the magnet. 
   //
   bool IntegrateSt3toSt4(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn = false); 
   bool IntegrateSt4toSt5(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn = false); 
   bool IntegrateSt5toSt6(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn = false); 
   
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
//
//  Feb. 2024, Optimizatiopn of the Euler or RK4 stepsize
//
    double fZXViewSt3, fZXViewSt4, fZXViewSt5, fZXViewSt6, fZStartField, fZEndField;
    double fdZStartField,  fdZEndField; // to go in straight line, field is zero.    
    std::vector<double> fStepsIntSt3toSt4, fStepsIntSt4toSt5, fStepsIntSt5toSt6;
//

  };
  
} // end namespace emph

#endif
