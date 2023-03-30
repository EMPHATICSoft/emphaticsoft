//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
///////////////////////////////////////////////////////////////////////////////
// File: MagneticField.h
// Description: A class for control of the Magnetic Field of the detector.
///////////////////////////////////////////////////////////////////////////////
#ifndef EMPHATICMagneticField_H
#define EMPHATICMagneticField_H

#include "Geant4/G4UniformMagField.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include <map>
#include <cmath>
#include <vector>

class G4FieldManager;

namespace emph {

  struct bFieldPoint {
    float fbx, fby, fbz; // in kG, internally. 
  };

  struct bFieldZipTrackPoint {
    float t, x, y, z, fbx, fby, fbz, theta; // in kG, internally. // For Sensis file, in Tesla. 
  };
  
  class EMPHATICMagneticField: public G4MagneticField {
  public:
    EMPHATICMagneticField(const G4String &name);
    ~EMPHATICMagneticField();  
      
    // Access functions
    void MagneticField(const double Point[3], double Bfield[3]) const;
    void G4GeomAlignIt(const emph::geo::Geometry *theEMPhGeometry);
    CLHEP::Hep3Vector MagneticField(const CLHEP::Hep3Vector Point) const;
    virtual void GetFieldValue(const double Point[3], double* Bfield) const; // units are mm, return values in kilogauss
    void test1(); // Check that divB ~ 0.;  
    void test2(); // test integration, study expected deflections.  
    void test3(); // test calculation of preliminary acceptance sensitivity of beam axis and/or SSD Yaw uncertainty.  
    void studyZipTrackData1(); // June 4 2022:  Received from Leo, who received from Mike Tartaglia. 
    void studyZipTrackData2(); // March 28 2023:  Received from Leo, who received from Mike Tartaglia. 
    std::pair<double, double> getMaxByAtCenter();

  protected:
    // Find the global Field Manager
    G4FieldManager* GetGlobalFieldManager(); 
    
  private:
    bool fFieldIsOff;
    bool fStorageIsStlVector; // We fill ffield, the stl vector<bFieldPoint>  if true.  else, the stl map of stl map... 
    bool fHasBeenAligned; 
    bool fUseOnlyCentralPart;
    double fInnerBoreRadius;
    std::vector<bFieldPoint> ffield;
    std::vector<bFieldZipTrackPoint> ffieldZipTrack; // from actual data.. 
    double xZipOne, yZipOne; // if studying one Zip track at a time.. ZipTrack data from Mike T. 
    std::map<int, std::map<int, std::map<int, std::vector<double> > > > field;
    double step;
    double start[3]; // old boundaries.. 
    double fG4ZipTrackOffset[3];
    int fNStepX, fNStepY, fNStepZ;
    double fXMin, fYMin, fZMin, fXMax, fYMax, fZMax; // New ones, used 
    double fStepX, fStepY, fStepZ; 
    int fInterpolateOption;
    G4int fVerbosity;
    
  public: 
   inline void setInterpolatingOption(int iOpt) { fInterpolateOption = iOpt; } // iOpt = 0 => 3D radial average , 1 linearized along axes of the 3D grid. 
   void Integrate(int iOpt, int charge, double stepAlongZ,  
                    std::vector<double> &start, std::vector<double> &end) const; 
    // iOpt = 0 => simple Euler formula, iOpt = 1 => 4rth order Runge Kutta
    // Start and end must be dimension to 6, usual phase space,  x,y,z, px, py, pz 
   // The integration ends at a fixed Z, i.e. end[2] .  The stepAlongZ is the initial step size, anlong the z axis. 
   // Algorithm: simple Runge-Kutta, 4rth order.  Suggest step size: ~ 20 mm for the February 2022 version of the field map. 
   // distance units are mm (as in Geant4, by default.) and momentum are in GeV/c  (as in Geant4, by default.) 
   // Curling around is not supported, 
   //
   // June 2022 : start analysis of ZipTrack data from Mike T. 
   //
   inline void setUseOnlyTheCentralPart(bool  t=true) {  fUseOnlyCentralPart = t; } 
   inline void setXZipOne(double x) { xZipOne = x; } 
   inline void setYZipOne(double y) { yZipOne = y; } 
   inline void SetFieldOn() { fFieldIsOff = false; }
   inline void SetFieldOff() { fFieldIsOff = true; }
   private:
    void uploadFromRootFile(const G4String &fName);
    void uploadFromTextFile(const G4String &fName);
    void uploadFromOneCSVZipFile(const G4String &fName);
    void uploadFromOneCSVSensisFile(const G4String &fName);
    inline size_t indexForVector(double *xyz) const {
      double *ptr = xyz; 
//      size_t iX = static_cast<size_t>(floor(((*ptr) - fXMin)/fStepX)); ptr++; // floor seems to fail if close to real boundary.. 
//      size_t iY = static_cast<size_t>(floor(((*ptr) - fYMin)/fStepY)); ptr++;
//      size_t iZ = static_cast<size_t>(floor(((*ptr) - fZMin)/fStepZ));
      // A better version... 
      size_t iX = static_cast<size_t>(floor(((*ptr) + 1.0e-10  - fXMin)/fStepX)); ptr++; // floor seems to fail if close to real boundary.. 
      size_t iY = static_cast<size_t>(floor(((*ptr) + 1.0e-10 - fYMin)/fStepY)); ptr++;
      size_t iZ = static_cast<size_t>(floor(((*ptr) + 1.0e-10 - fZMin)/fStepZ));  // see NoteOnDoubleFromASCII
     return (static_cast<size_t>(fNStepZ*fNStepY) * iX + static_cast<size_t>(fNStepZ) * iY + iZ);
    } 
    inline size_t indexForVector(size_t iX, size_t iY, size_t iZ) const {
      return (static_cast<size_t>(fNStepZ*fNStepY) * iX + static_cast<size_t>(fNStepZ) * iY + iZ);
    } 
    
    void MagneticFieldFromCentralBore(const double Point[3], double BApprox[3]) const;

    
    void NoteOnDoubleFromASCIIFromCOMSOL() const ; // documenting why we add a small quantity to get the correct output from (size_t) floor
  };
  
} // end namespace emph

#endif
