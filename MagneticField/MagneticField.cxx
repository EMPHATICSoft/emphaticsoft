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
// File: MagneticField.cxx
// Description: User Field class implementation.
///////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <cstdlib>

#include "MagneticField/MagneticField.h"

#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4FieldManager.hh"

#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "TFile.h"
#include "TTree.h"
#include "math.h"
//#define ddebug
//#define debug
//
// For testing purposes. 
#include "Geant4/Randomize.hh"

//Constructor and destructor:

namespace emph {

  EMPHATICMagneticField::EMPHATICMagneticField(const G4String &filename) :
    fStorageIsStlVector(true), fHasBeenAligned(false), fUseOnlyCentralPart(false), fInnerBoreRadius(23.5), // This is the Phase1b Japanese Magnet. 
    step(0), start{-16., -16., -20.},// for the root test file, units are cm Not sure about start values.
    fG4ZipTrackOffset{ 0., 0., 400.},  // Will be overwritten based on the G4/Art based geometry. 
    fNStepX(1), fNStepY(1), fNStepZ(1),
    fXMin(6.0e23),  fYMin(6.0e23), fZMin(6.0e23), fXMax(-6.0e23), fYMax(-6.0e23), fZMax(-6.0e23),
    fStepX(0.), fStepY(0.), fStepZ(0.),
    fInterpolateOption(0)
    {
#ifdef debug
      fVerbosity = 1;
#else
      fVerbosity = 0;
#endif
      //    this->NoteOnDoubleFromASCIIFromCOMSOL(); 
      //    std::cerr <<  " EMPHATICMagneticField::EMPHATICMagneticField...  And quit for now... " << std::endl; exit(2);
      if (filename.find(".root") != std::string::npos) this->uploadFromRootFile(filename);
      else this->uploadFromTextFile(filename);
      // These tests do something, comment out for sake of saving time for production use.     
          this->SetFieldOn();	// Set also in the G4EMPH, and the reconstruction.. Please check..       
// 
//          this->test1();
      //    this->test2();
      //    this->test3();
      //      this->studyZipTrackData1();
      //     this->studyZipTrackData2();
    }
    
  void EMPHATICMagneticField::G4GeomAlignIt(const emph::geo::Geometry *theEMPhGeometry) {
  
    fG4ZipTrackOffset[2] = -theEMPhGeometry->MagnetUSZPos() + 82.5; // rough guess! 
    std::cerr << " EMPHATICMagneticField::G4GeomAlignIt G4ZipTrack Z Offset set to " << fG4ZipTrackOffset[2] << std::endl;
    fHasBeenAligned = true; 
//
// Testing... at COMSOL coordinate of z = -82.5 mm, By ~ 7.5 Kg, 1/2  field 
//     
    double xTest[3], xTest2[3], BTest[3], BTest2[3]; 
    xTest[0] = 0.; xTest[1] = 0.; xTest[2] = -82.5;  xTest2[0] = 0.01; xTest2[1] = 0.004; xTest2[2] = -52.5; // in mm 
//    for (size_t k=0; k != 3; k++) xAligned[k] = x[k] + fG4ZipTrackOffset[k]; // The equation in the GetFieldValue. 
    this->MagneticField(xTest, BTest);
    std::cerr << " EMPHATICMagneticField::G4GeomAlignIt, BField at Upstream plate, internal Variables  " 
              << BTest[1] <<  " kG "  << std::endl;
    this->MagneticField(xTest2, BTest2);
    std::cerr << " .......... again, 20mm inward " << BTest2[1] << std::endl;
    for (size_t k=0; k != 3; k++) xTest[k] -= fG4ZipTrackOffset[k];
    BTest[1] = 0.;
    this->GetFieldValue(xTest, BTest);
    std::cerr << " EMPHATICMagneticField::G4GeomAlignIt, BField at Upstream plate, G4 Coordinates   " << BTest[1] <<  " kG " << std::endl;
   
  }
  
  void EMPHATICMagneticField::uploadFromRootFile(const G4String &fName) {
  
    fStorageIsStlVector = false; // Could up grade later.. 
    std::cerr << " EMPHATICMagneticField::uploadFromRootFile, currently disable does not provide stable answers with current compilers... " << std::endl;
    std::cerr << " Fatal error, quit here and now " << std::endl; exit(2);
  
    /*for(int i = 0; i < 250; i++){
      for(int j = 0; j < 250; j++){
      for(int k = 0; k < 250; k++){
      std::vector<double> temp(3, 0);
      field[i][j][k] = temp;
      }		
      }	
      }*/
    
    TFile mfFile(fName.c_str(), "READ");
    std::cout << " ==> Opening file " << fName << " to read magnetic field..."
	   << G4endl;
    
    if (mfFile.IsZombie()) {
      std::cerr << "Error opening file" << G4endl;
      exit(-1);
    }	
    TTree *tree = (TTree*) mfFile.Get("magField");
    
    double x;
    double y;
    double z;
    double Bx;
    double By;
    double Bz;
    
    std::ofstream fOutForR;
    if (fVerbosity)  {
      fOutForR.open("./EmphMagField_v1.txt");
      fOutForR << " x y z dx dy dz bx by bz " << std::endl;
    }
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("y", &y);
    tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("Bx", &Bx);
    tree->SetBranchAddress("By", &By);
    tree->SetBranchAddress("Bz", &Bz);
    
    int nEntries = tree->GetEntries();
    
    tree->GetEntry(0);
    double xPrev = x; 
    double yPrev = y; 
    double zPrev = z; 

    step = 0.25; // Only valid for emphatic magnet phase 1, January run. 
    tree->GetEntry(1);
//    if(abs(xVal - x) > step) step = abs(xVal - x);
//    else if(abs(yVal - y) > step) step = abs(yVal - y);
//    else step = abs(zVal - z);
    int maxIndX = 0;
    int maxIndY = 0;
    int maxIndZ = 0;
    for(int i = 0; i < nEntries; i++){
      tree->GetEntry(i);
      
      int indX = static_cast<int>((x-start[0])/step);
      int indY = static_cast<int>((y-start[1])/step);
      int indZ = static_cast<int>((z-start[2])/step);
      maxIndX = std::max(indX, maxIndX); 
      maxIndY = std::max(indY, maxIndY); 
      maxIndZ = std::max(indZ, maxIndZ); 
    }
    // Do We pre-allocate..
    // Note : This is not a regular grid!   Could cause some confusion.. 
    // 
    /* 
    std::vector<double> zero3(3, 0.);
    std::map<int, std::vector<double> > fieldAlongZ;
    for (int kz=0; kz !=  maxIndZ; kz++) fieldAlongZ.emplace(kz, zero3);
    std::cerr << " Size of map along Z " << fieldAlongZ.size() << std::endl;
    std::map<int, std::map<int, std::vector<double> > > fieldAlongYZ;
    for (int ky=0; ky !=  maxIndY; ky++) fieldAlongYZ.emplace(ky, fieldAlongZ);
    std::cerr << " Size of map along YZ " << fieldAlongYZ.size() << std::endl;
    for (int kx=0; kx !=  maxIndX; kx++) field.emplace(kx, fieldAlongYZ);
     std::cerr << " Size of map along XYZ " << field.size() << std::endl;
    */
    for(int i = 0; i < nEntries; i++){
      tree->GetEntry(i);
      int indX = static_cast<int>((x-start[0])/step);
      int indY = static_cast<int>((y-start[1])/step);
      int indZ = static_cast<int>((z-start[2])/step);
     
      if (fVerbosity || (i < 3)) {
	std::cout << "(x, y, z) = (" << x << ", " << y << ", " << z << ") cm,    (ix, iy, iz) = (" << indX 
	          << ", " << indY << ", " << indZ << "),    (Bx, By, Bz) = (" << Bx << ", " << By << ", " << Bz << ") kG" << G4endl;
       }
       if (fVerbosity)
        fOutForR << " " << x << " " << y << " " << z << " " << x- xPrev << " " << y - yPrev << " " << z - zPrev 
               << " " << Bx << " " << By << " " << Bz << std::endl; 
      
      std::vector<double> temp;
      temp.push_back(0.1*Bx); // from kG to tesla. 
      temp.push_back(0.1*By);
      temp.push_back(0.1*Bz);
      // 
      // This sound easy, but with it does not work.. 
      //  
      
      xPrev = x; 
      yPrev = y; 
      zPrev = z; 
     
    } // loop on entries 
    // We convert step and start to mm. 
    step *=10.; for (int k=0; k !=3; k++) start[k] *= 10.;
    // check direct access.. 
    std::cerr << " 3D map loaded up,  max Indices " << maxIndX << ", " << maxIndY << ", " << maxIndZ << G4endl;
    std::vector<double> testVal = field.at(maxIndX/2).at(maxIndY/2).at(maxIndZ/2);
    std::cerr << " Map size along X  " << field.size() << " Along Y, at loc X = 30  " << field[30].size() << std::endl;
    std::cerr << " Test Value at central point (Bx, By, Bz) " << testVal[0] << ", " << testVal[1] << ", " << testVal[2] << std::endl;
    
    ///////////////////////////////////////////////////////////////
    // Close the file
    std::cerr << " ==> Closing file " << fName  << G4endl;
    mfFile.Close();
    if (fVerbosity) fOutForR.close();
    //
  }
  void EMPHATICMagneticField::uploadFromTextFile(const G4String &fName) {
        
	std::cerr << " Entering EMPHATICMagneticField::uploadFromTextFile fName " << fName <<  std::endl;
	double numbers[6];
	std::ifstream fileIn(fName.c_str());
	if (!fileIn.is_open()) {
	  std::cerr << " EMPHATICMagneticField::uploadFromTextFile, file " << fName << " can not be open, fatal .." << std::endl; 
	  std::cerr << " Please copy the file  " << fName << " from emphaticgpvm02.fnal.gov:/emph/data/users/lebrun/" << std::endl; 
	  exit(2);
	}
        std::string line;
        char aLinecStr[1024];
        int numLines = 0;
	// first pass, check the grid is uniform. 
	double xC, yC, zC; double xP=0.; double yP=0.; double zP=0.; 
	double sumStepX = 0.;  double sumStepY = 0.; double sumStepZ = 0.;
	double sumStepSqX = 0.;  double  sumStepSqY = 0.; double  sumStepSqZ = 0.;
	bool backToFirstX = false;  bool backToFirstY = false; 
	std::vector<double> xVals;  std::vector<double> yVals;
	// Units are assumed to be in mm 
        while (fileIn.good()) {
          fileIn.getline(aLinecStr, 1024);
          std::string aLine(aLinecStr);
	  if (aLine.length() < 2) continue; // end of file suspected, or blank in the file 
	  if (aLine.find("%") != std::string::npos) continue; // Comment line from COMSOL. 
//	  if (numLines > 8254) std::cerr << " aLine " << aLine << std::endl;
          numLines++;
          std::istringstream aLStr(aLine);
	  aLStr >> xC; aLStr >> yC; aLStr >> zC;  // We skip the reading of the BField .. 
	  if (isnan(xC) || isnan(yC) || isnan(yC)) {
	      std::cerr << " EMPHATICMagneticField::uploadFromTextFile, line  " << numLines << " has NaN(s), fatal.. " << std::endl;
	      exit(2);
          }	  
	  fXMin = std::min(fXMin, xC); fYMin = std::min(fYMin, yC); fZMin = std::min(fZMin, zC);
	  fXMax = std::max(fXMax, xC); fYMax = std::max(fYMax, yC); fZMax = std::max(fZMax, zC);
	  if (numLines == 1) { xVals.push_back(xC); yVals.push_back(yC); }
	  // Assume stepping along the X axis is the fatest (line after line), then Y axis, last is Z axis. 
	  //  .. and that the first point is at fXMin, fYMin, fZMin 
	  if ((!backToFirstX) && (numLines > 2) && (std::abs(xC - fXMin) < 1.0e-3))  {
	     backToFirstX = true;
	     fStepX = sumStepX/(fNStepX-1); 
	     if (fNStepX < 2) {
	        std::cerr << " EMPHATICMagneticField::uploadFromTextFile, no information about the step size along the X axis, fatal  " << std::endl;
                exit(2);	
	     }
	     const double varStepX = (sumStepSqX - fNStepX*fStepX*fStepX)/((fNStepX-2)*(fNStepX-1)) ;
	     if (varStepX > 1. ) {
	        std::cerr << " EMPHATICMagneticField::uploadFromTextFile, variance of steps along X greate than 1 mm square, " << varStepX 
		             << " fNStepX " << fNStepX << ", fStepX " << fStepX << " fatal  " << std::endl;
                exit(2);	
	     }
	     std::cerr << " back to fXMin.. fNStepX " << fNStepX << " step size along X " << fStepX << std::endl;
	     
	  }
	  if ((!backToFirstX) && (numLines > 1))  {
	      fNStepX++; sumStepX += std::abs(xP - xC); sumStepSqX += (xP - xC)*(xP - xC); xVals.push_back(xC);
	  }
	  if (backToFirstX && (numLines > 1)) { // check subsequent x Scans.. 
	     bool foundXCoord = false;
	     for (size_t k=0; k != xVals.size(); k++) {
	       if (std::abs(xC - xVals[k]) < 1.0e-3) { foundXCoord=true; break; }
	     }
	     if (!foundXCoord) { 
	        std::cerr << " EMPHATICMagneticField::uploadFromTextFile, the scans along the X axis don't match.. " << std::endl; 
		std::cerr << "  ... numLines " << numLines << " size of xVals " << xVals.size() << std::endl;
		std::cerr << " first few xVals " << xVals[0] << ", " << xVals[1] << ", " << xVals[2] << std::endl;
		std::cerr << " Grid is irregular .... fatal  " << std::endl;
                exit(2);	
	     }
	     // we now deal with the grid info along the Y axis.
	     if ((!backToFirstY) && (numLines > 2) && (std::abs(xC - fXMin) < 1.0e-3) && (std::abs(yC - fYMin) > 1.0e-3))  { // skip the last one..
	      fNStepY++; sumStepY += std::abs(yP - yC); sumStepSqY += (yP - yC)*(yP - yC); yVals.push_back(yC);
	     }
	     if ((!backToFirstY) && (numLines > (fNStepX-3)) && (std::abs(yC - fYMin) < 1.0e-3))  {
	       backToFirstY = true; 
	       std::cerr << " Found the back to start along Y at line " << numLines << " step along Y = " << fNStepY << std::endl;
	       fStepY = sumStepY/(fNStepY-1); 
	       if (fNStepY < 2) {
	        std::cerr << " EMPHATICMagneticField::uploadFromTextFile, no information about the step size along the Y axis, fatal  " << std::endl;
                exit(2);	
	       }
	       const double varStepY = (sumStepSqY - fNStepY*fStepY*fStepY)/((fNStepY-2)*(fNStepY-1)) ;
	       if (varStepY > 1. ) {
	          std::cerr << " EMPHATICMagneticField::uploadFromTextFile, variance of steps along Y greate than 1 mm square, " << varStepY 
		             << " fNStepY " << fNStepY << ", fStepY " << fStepY << " fatal  " << std::endl;
                  exit(2);	
	       }
	       std::cerr << " back to fYMin.. fNStepY " << fNStepY << " step size along Y " << fStepY << std::endl;
	   } // back to the starting point along Y axis. 
	    //Now along the Z axis. 
	   if ((std::abs(xC - fXMin) < 1.0e-3) &&  (std::abs(yC - fYMin) < 1.0e-3)) {
	       fNStepZ++; sumStepZ += std::abs(zP - zC); sumStepSqZ += (zP - zC)*(zP - zC);
	       if (yVals.size() > 2) { // Check, only if we have filled vector (not the first increment in Z  
	          bool foundYCoord = false;
	          for (size_t k=0; k != yVals.size(); k++) {
	            if (std::abs(yC - yVals[k]) < 1.0e-3) { foundYCoord=true; break; }
	         }
	         if (!foundYCoord) { 
	           std::cerr << " EMPHATICMagneticField::uploadFromTextFile, the scans along the Y axis don't match.. " << std::endl; 
		   std::cerr << "  ... numLines " << numLines << " size of YVals " << yVals.size() << std::endl;
		   std::cerr << " first few yVals " << yVals[0] << ", " << yVals[1] << ", " << yVals[2] << std::endl;
		   std::cerr << " Grid is irregular .... fatal  " << std::endl;
                   exit(2);	
	         }
	       } // Check 
	    } // back to the starting point along Y axis. 
	  } // back to the starting point along X axis. 	  
	  xP = xC; yP = yC; zP = zC; 
	} // reading the file.. 
	fileIn.close();
	fStepZ = sumStepZ/(fNStepZ-1); 
	if (fNStepZ < 2) {
	   std::cerr << " EMPHATICMagneticField::uploadFromTextFile, no information about the step size along the Z axis, fatal  " << std::endl;
           exit(2);	
	}
	const double varStepZ = (sumStepSqZ - fNStepZ*fStepZ*fStepZ)/(fNStepZ*(fNStepZ-1)) ;
	if (varStepZ > 1. ) {
	          std::cerr << " EMPHATICMagneticField::uploadFromTextFile, variance of steps along Z greate than 1 mm square, " << varStepZ 
		             << " fNStepZ " << fNStepZ << ", fStepZ " << fStepZ << " fatal  " << std::endl;
                  exit(2);	
	}
	if (fVerbosity == 1) { 
	  std::cerr << " EMPHATICMagneticField::uploadFromTextFile, The grid is considered as regular, numPtx along X = " 
	            << fNStepX << ", Y = " << fNStepY << ", Z = " <<  fNStepZ << std::endl;
	  std::cerr << " .... grid step size along X = " 
	            << fStepX << ", Y = " << fStepY << ", Z = " <<  fStepZ << std::endl;
	  std::cerr << " .... Start locations  = " 
	            << fXMin << ", Y = " << fYMin << ", Z = " <<  fZMin << std::endl;
	}
	// allocate memory, if stl vector..
	if (fStorageIsStlVector) {  
	  bFieldPoint aBV; aBV.fbx = nan("FCOMSOL"); aBV.fby = nan("FCOMSOL"); aBV.fbz = nan("FCOMSOL");
	  size_t nTot = static_cast<size_t>(fNStepX) * static_cast<size_t>(fNStepY) * static_cast<size_t>(fNStepZ); 
	  for (size_t i=0; i != nTot; i++) ffield.push_back(aBV);
	} else {
	  start[0] = fXMin; start[1] = fYMin; start[2] = fZMin; step = fStepZ; // old notation.. keep for consistency. 
	}
	fileIn.open(fName.c_str());
	numLines = 0;
        while (fileIn.good()) {
          fileIn.getline(aLinecStr, 1024);
          std::string aLine(aLinecStr);
 	  if (aLine.find("%") != std::string::npos) continue; // Comment line from COMSOL. 
	  if (aLine.size() < 2) continue; // end of file suspected, or blank in the file 
          numLines++;
          std::istringstream aLStr(aLine);
  	  int count = 0; std::string num;
  	  while((count < 6) && (aLStr >> num)){
  	     if(num == "NaN") {
	         numbers[count] = 0;
		 std::cerr << " NaN found at line " << numLines << " line " << aLine << std::endl;
             }
  	     else  numbers[count] = std::stod(num); // probably a bad idea to treat NaN as field is really physically vanishing.. 
  				count++;
  	     }
	   if (fStorageIsStlVector) {  
	     size_t ii = this->indexForVector(numbers);
	     /*
	     if ((ii == 2302316) || (ii == 2302317)) {
	       double *ptr = &numbers[0];
               const size_t iX = static_cast<size_t>(floor(((*ptr) - fXMin)/fStepX)); ptr++;
               const size_t iY = static_cast<size_t>(floor(((*ptr) - fYMin)/fStepY)); ptr++;
               const size_t iZ = static_cast<size_t>(floor(((*ptr) - fZMin)/fStepZ));
	       const size_t iCheck = static_cast<size_t>(fNStepZ*fNStepY) * iX + static_cast<size_t>(fNStepZ) * iY + iZ;
	       std::cerr << " .. Bad point, Stl vector, numLines " << numLines << "  x y z   " 
	                                                       << numbers[0] << ", " << numbers[1] 
							       << ", " << numbers[2] << " by = " 
							       <<  numbers[4] << " iX " << iX << " iY " << iY 
							       << " iZ " << iZ <<  " iCheck " << iCheck << std::endl;
		std::cerr << " ..... fNStepX " << fNStepX << "  fNStepY " << fNStepY << " fNStepZ " << fNStepZ << std::endl;
		std::cerr << " ..... fStepX " << fStepX << "  fStepY " << fStepY 
		         << " fStepZ " << fStepZ << " fZMin " <<  fZMin << " Z coord " << numbers[2] << std::endl;
		std::cerr << " line " << aLine << std::endl;
		// This to many looks like a vicious behavior of casting or floor behaviour. !!! but patched.. 					       
	     }
	     */
	     if (ii < ffield.size()) { 
	       ffield[ii].fbx = 10.0*numbers[3]; ffield[ii].fby =10.0* numbers[4]; ffield[ii].fbz = 10.0*numbers[5]; // Tesla to KG
	     }
	   } else {
             int indX = static_cast<int>(floor(numbers[0]-start[0])/step);
             int indY = static_cast<int>(floor(numbers[1]-start[1])/step);
             int indZ = static_cast<int>(floor(numbers[2]-start[2])/step);
	    /*
 	     if ((indX == 63) && (indY == 66) && ((indZ == 83) || (indZ == 84)))
	        std::cerr << " .. Bad point, Stl map numLines " << numLines << " x,y,z " 
	                                                       << numbers[0] << ", " << numbers[1] 
							       << ", " << numbers[2] << " by = " <<  numbers[4] << std::endl;
	    */
             std::vector<double> temp;
             temp.push_back(numbers[3]*10.); // Tesla to kG. 
             temp.push_back(numbers[4]*10.);
             temp.push_back(numbers[5]*10.);
             field[indX][indY][indZ] = temp;
	   }
	}
	fileIn.close(); 
	int  numNanInTable = 0;
	for (size_t k=0; k != ffield.size(); k++) {
	  if (isnan(ffield[k].fbx) || isnan(ffield[k].fby) || isnan(ffield[k].fbz)) {
	    numNanInTable++;
	    if (numNanInTable < 50) {
	      std::cerr << " A field value was not filled at index " << k ;
	      size_t iX = k/ (fNStepZ * fNStepY); 
	      size_t kiY = k - iX*(fNStepZ * fNStepY);
	      size_t iY = kiY/fNStepZ;
	      size_t iZ = k - iX*(fNStepZ * fNStepY) - iY*fNStepZ; 
	      std::cerr << " index i " << iX << " Y " << iY << " Z " << iZ << " bad.. keep going " << std::endl;
	    }
	  }
	}
	if (numNanInTable > 0) { std::cerr << " .. Requiring Swiss precision, Fatal, quit now " << std::endl; exit(2); }   
//	std::cerr << " And quit after filling from COMSOL text  file " << std::endl; exit(2);
  }
  
  EMPHATICMagneticField::~EMPHATICMagneticField() {
    
  }
  
void EMPHATICMagneticField::uploadFromOneCSVZipFile(const G4String &fName) {
  
    double numbers[8];
    ffieldZipTrack.clear(); // may be we want add all of them, to rethink.. for now, one at a time. 
    std::ifstream fileIn(fName.c_str());
    if (!fileIn.is_open()) {
      std::cerr << " EMPHATICMagneticField::uploadFromOneCSVZipFile, file " << fName << " can not be open, fatal .." << std::endl; 
      exit(2);
    }
    std::string line;
    char aLinecStr[1024];
    int numLines = 0;
    // first pass, check the grid is uniform. 
    // Units are assumed to be in mm 
    while (fileIn.good()) {
      fileIn.getline(aLinecStr, 1024);
      std::string aLine(aLinecStr);
      if (aLine.length() < 2) continue; // end of file suspected, or blank in the file 
      if (numLines == 0) {
	  if (aLine.find("T X Y Z Bx By Bz Theta") == std::string::npos) { // Comment line from Mike T. dAQ.
	     std::cerr << " Unexpected header line " << aLine <<  " unknow file , quit here and now " << std::endl; exit(2); }   
      } else {
        bFieldZipTrackPoint aPt;
	std::istringstream  aLStr(aLine);
  	int count = 0; std::string num;
  	while((count < 8) && (aLStr >> num)){
  	     if(num == "NaN") {
	         numbers[count] = 0;
		 std::cerr << " NaN found at line " << numLines << " line " << aLine << std::endl;
             }
  	     else  numbers[count] = std::stod(num); // probably a bad idea to treat NaN as field is really physically vanishing.. 
  				count++;
  	}
	aPt.t = numbers[0];  aPt.x = numbers[1]; aPt.y = numbers[2]; aPt.z = numbers[3];
	aPt.fbx = numbers[4]; aPt.fby = numbers[5]; aPt.fbz = numbers[6]; aPt.theta = numbers[7];
        ffieldZipTrack.push_back(aPt);
      } 
      numLines++;
    }
    fileIn.close();
}    
  // Member functions
  
  void EMPHATICMagneticField::MagneticField(const double x[3], double B[3]) const 
  {
//    bool debugIsOn = ((std::abs(x[0] + 2.06) < 0.01) && (std::abs(x[1] - 6.42) < 0.01) && (std::abs(x[2] + 141.918) < 100.));
    bool debugIsOn = false;
    B[0] = 0.5e-4; // a bit of a waste of CPU, but it makes the code a bit cleaner 
    B[1] = 0.5e-4;
    B[2] = 0.5e-4;  // Who know what the residual field is at MT6.. Does not matter... 
    if (fFieldIsOff) return;
    if (debugIsOn) std::cerr << " EMPHATICMagneticField::MagneticField, at x,y,z " << x[0] << ", " << x[1] << ", " << x[2] << std::endl; 
    if (fStorageIsStlVector) {
      if ((x[0] < fXMin) || (x[0] > fXMax) || (x[1] < fYMin) || (x[1] > fYMax)|| (x[2] < fZMin) || (x[2] > fZMax)) return;
        double indX = (x[0] - fXMin)/fStepX; // Check units.. 
        double indY = (x[1] - fYMin)/fStepY;
        double indZ = (x[2] - fZMin)/fStepZ;
    
        size_t ix[2] = {static_cast<size_t>(floor(indX)), static_cast<size_t>(ceil(indX))};
        size_t iy[2] = {static_cast<size_t>(floor(indY)), static_cast<size_t>(ceil(indY))};
        size_t iz[2] = {static_cast<size_t>(floor(indZ)), static_cast<size_t>(ceil(indZ))};
	if (debugIsOn) std::cerr << " Indices .. x " << ix[0] << " " << ix[1] << " y " <<  iy[0] << " " << iy[1] <<
	                          " z " <<  iz[0] << " " << iz[1] << std::endl;       
	double sumx = 0.; double sumy = 0.; double sumz = 0.; double norm = 0.;
        if(fInterpolateOption == 0) { 
           for(int i = 0; i < 2; i++){
             for(int j = 0; j < 2; j++){
	       for(int k = 0; k < 2; k++){
	         size_t iV = indexForVector(ix[i], iy[j], iz[k]);
	         double dist = sqrt((indX-ix[i])*(indX-ix[i]) + (indY-iy[j])*(indY-iy[j]) + (indZ-iz[k])*(indZ-iz[k]));
	         sumx += ffield[iV].fbx * dist;
	         sumy += ffield[iV].fby * dist;
	         sumz += ffield[iV].fbz * dist;
	         norm += dist;
	        if (debugIsOn) std::cerr << " ........ i j k " << i << " " << j << " " << k << " iV " << iV << " dist " 
		                         << dist << " by " << ffield[iV].fby << " sumy " << sumy << " norm " << norm << std::endl;
	       }
	     }		
          }	
	  B[0] = (sumx/norm);
          B[1] = (sumy/norm);
          B[2] = (sumz/norm);
	  if (debugIsOn) std::cerr << " ... By .. " << B[1] << std::endl;
          return;
       } else {  // linear interpolation on the grid. 
         const double t = indX-ix[0];
         const double u = indY-iy[0];
         const double v = indZ-iz[0];
	 std::vector<size_t> iVXs(8, 0);
	 iVXs[0] = indexForVector(ix[0], iy[0], iz[0]);
	 iVXs[1] = indexForVector(ix[1], iy[0], iz[0]);
	 iVXs[2] = indexForVector(ix[0], iy[1], iz[0]);
	 iVXs[3] = indexForVector(ix[1], iy[1], iz[0]);
	 iVXs[4] = indexForVector(ix[0], iy[0], iz[1]);
	 iVXs[5] = indexForVector(ix[1], iy[0], iz[1]);
	 iVXs[6] = indexForVector(ix[0], iy[1], iz[1]);
	 iVXs[7] = indexForVector(ix[1], iy[1], iz[1]);
         for (size_t kc=0; kc != 3; kc++) {
	   switch(kc) {
	     case 0:
               B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*ffield[iVXs[0]].fbx;
	       B[kc] +=  t*(1.0-u)*(1.0-v)*ffield[iVXs[1]].fbx;
	       B[kc] +=  (1.0-t)*u*(1.0-v)*ffield[iVXs[2]].fbx;
	       B[kc] +=  t*u*(1.0-v)*ffield[iVXs[3]].fbx;
               B[kc] +=  (1.0-t)*(1.0-u)*v*ffield[iVXs[4]].fbx;
	       B[kc] +=  t*(1.0-u)*v*ffield[iVXs[5]].fbx;
	       B[kc] +=  (1.0-t)*u*v*ffield[iVXs[6]].fbx;
	       B[kc] +=  t*u*v*ffield[iVXs[7]].fbx;
	       break;
	    case 1:
               B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*ffield[iVXs[0]].fby;
	       B[kc] +=  t*(1.0-u)*(1.0-v)*ffield[iVXs[1]].fby;
	       B[kc] +=  (1.0-t)*u*(1.0-v)*ffield[iVXs[2]].fby;
	       B[kc] +=  t*u*(1.0-v)*ffield[iVXs[3]].fby;
               B[kc] +=  (1.0-t)*(1.0-u)*v*ffield[iVXs[4]].fby;
	       B[kc] +=  t*(1.0-u)*v*ffield[iVXs[5]].fby;
	       B[kc] +=  (1.0-t)*u*v*ffield[iVXs[6]].fby;
	       B[kc] +=  t*u*v*ffield[iVXs[7]].fby;
	       break;
	    case 2:
               B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*ffield[iVXs[0]].fbz;
	       B[kc] +=  t*(1.0-u)*(1.0-v)*ffield[iVXs[1]].fbz;
	       B[kc] +=  (1.0-t)*u*(1.0-v)*ffield[iVXs[2]].fbz;
	       B[kc] +=  t*u*(1.0-v)*ffield[iVXs[3]].fbz;
               B[kc] +=  (1.0-t)*(1.0-u)*v*ffield[iVXs[4]].fbz;
	       B[kc] +=  t*(1.0-u)*v*ffield[iVXs[5]].fbz;
	       B[kc] +=  (1.0-t)*u*v*ffield[iVXs[6]].fbz;
	       B[kc] +=  t*u*v*ffield[iVXs[7]].fbz;
	       break;
	    }
        } // three components of the field. 
        
	  if (debugIsOn) std::cerr << " ... By .. " << B[1] << std::endl;
       
       } // inpterpolation method. 
       
     } else { // map storage
        double indX = (x[0] - start[0])/step; 
        double indY = (x[1] - start[1])/step;
        double indZ = (x[2] - start[2])/step;
    
        int ix[2] = {int (floor(indX)), int (ceil(indX))};
        int iy[2] = {int (floor(indY)), int (ceil(indY))};
        int iz[2] = {int (floor(indZ)), int (ceil(indZ))};
	if (debugIsOn) {
	    std::cerr << " .... Indices .. x " << ix[0] << " " << ix[1] << " y " <<  iy[0] << " " << iy[1] <<
	                          " z " <<  iz[0] << " " << iz[1] <<  " .... step " << step << " start[0] " << start[0] << std::endl;		         
        }
        bool skip = false;
        if(field.find(ix[0]) == field.end()) { if (debugIsOn) std::cerr << " x[0] out " << std::endl; skip = true; }
        else if(field.find(ix[1]) == field.end()) { if (debugIsOn) std::cerr <<  " x[1] out " << std::endl; skip = true; }
        else{
        if(field.at(ix[0]).find(iy[0]) == field.at(ix[0]).end()) { if (debugIsOn) std::cerr <<  " x[0] y[0] out " << std::endl; skip = true; }
        else if(field.at(ix[0]).find(iy[1]) == field.at(ix[0]).end()) { if (debugIsOn) std::cerr <<  " x[0] y[1] out " << std::endl; skip = true; }
        else if(field.at(ix[1]).find(iy[0]) == field.at(ix[1]).end()) { if (debugIsOn) std::cerr <<  " x[1] y[0] out " << std::endl; skip = true; }
        else if(field.at(ix[1]).find(iy[1]) == field.at(ix[1]).end()) { if (debugIsOn) std::cerr <<  " x[1] y[1] out " << std::endl; skip = true; }
        else{
	  if(field.at(ix[0]).at(iy[0]).find(iz[0]) ==field.at(ix[0]).at(iy[0]).end()) { if (debugIsOn) std::cerr <<  " x[0] y[0] z[0] out " << std::endl; skip = true; }
	  else if(field.at(ix[0]).at(iy[0]).find(iz[1]) ==field.at(ix[0]).at(iy[0]).end()) { if (debugIsOn) std::cerr <<  " x[0] y[0] z[1] out " << std::endl; skip = true; }
	  else if(field.at(ix[0]).at(iy[1]).find(iz[0]) ==field.at(ix[0]).at(iy[1]).end()) { if (debugIsOn) std::cerr <<  " x[0] y[1] z[0] out " << std::endl; skip = true; }
	  else if(field.at(ix[0]).at(iy[1]).find(iz[1]) ==field.at(ix[0]).at(iy[1]).end()) { if (debugIsOn) std::cerr <<  " x[0] y[1] z[1] out " << std::endl; skip = true; }
	  else if(field.at(ix[1]).at(iy[0]).find(iz[0]) ==field.at(ix[1]).at(iy[0]).end()) { if (debugIsOn) std::cerr <<  " x[1] y[0] z[0] out " << std::endl; skip = true; }
	  else if(field.at(ix[1]).at(iy[0]).find(iz[1]) ==field.at(ix[1]).at(iy[0]).end()) { if (debugIsOn) std::cerr <<  " x[1] y[0] z[1] out " << std::endl; skip = true; }
	  else if(field.at(ix[1]).at(iy[1]).find(iz[0]) ==field.at(ix[1]).at(iy[1]).end()) { if (debugIsOn) std::cerr <<  " x[1] y[1] z[0] out " << std::endl; skip = true; }
	  else if(field.at(ix[1]).at(iy[1]).find(iz[1]) ==field.at(ix[1]).at(iy[1]).end()) { if (debugIsOn) std::cerr <<  " x[1] y[1] z[1] out " << std::endl; skip = true; }
        }
      }
    
    
      if(skip) return;
      if (debugIsOn) {
         std::cerr << " Indices along X " << ix[0] << ", " << ix[1] 
                      << " .. along Y " << iy[0] << ", " << iy[1] << " .. along Z " << iz[0] << ", " << iz[1] << std::endl;
         std::cerr << " ....  by at ix, iy iz low " << field.at(ix[0]).at(iy[0]).at(iz[0]).at(1) << " high " 
                                                  << field.at(ix[1]).at(iy[1]).at(iz[1]).at(1) << std::endl;
						  std::cerr << " And quit after the first succesful pick up " << std::endl; exit(2);
      }	    
   
      if(fInterpolateOption == 0) { 
        double sumx = 0;
        double sumy = 0;
        double sumz = 0;
        double norm = 0;
        for(int i = 0; i < 2; i++){
          for(int j = 0; j < 2; j++){
	    for(int k = 0; k < 2; k++){
	      double dist = sqrt((indX-ix[i])*(indX-ix[i]) + (indY-iy[j])*(indY-iy[j]) + (indZ-iz[k])*(indZ-iz[k]));
	      sumx += field.at(ix[i]).at(iy[j]).at(iz[k]).at(0)*dist;
	      sumy += field.at(ix[i]).at(iy[j]).at(iz[k]).at(1)*dist;
	      sumz += field.at(ix[i]).at(iy[j]).at(iz[k]).at(2)*dist;
	      norm += dist;
	        if (debugIsOn) std::cerr << " ........ i j k " << i << " " << j << " " << k << " dist " 
		                         << dist << " by " << field.at(ix[i]).at(iy[j]).at(iz[k]).at(1) 
					 << " sumy " << sumy << " norm " << norm << std::endl;
	     }
	   }		
          }	
/*
* Results are already in kilogauss, P.L. March 2022. 
* The (global) variable kiloguass is a member of the Geant4 System of Units class. 
*    
        B[0] = (sumx/norm)*kilogauss;
        B[1] = (sumy/norm)*kilogauss;
        B[2] = (sumz/norm)*kilogauss;
*/    
        B[0] = (sumx/norm);
        B[1] = (sumy/norm);
        B[2] = (sumz/norm);
	  if (debugIsOn) std::cerr << " ... By .. " << B[1] << std::endl;
      } else { // Linear interpolation on the 3D grid P.L. March 2022 
        const double t = indX-ix[0];
        const double u = indY-iy[0];
        const double v = indZ-iz[0];
        for (size_t kc=0; kc != 3; kc++) {
          B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*field.at(ix[0]).at(iy[0]).at(iz[0]).at(kc);
	  B[kc] +=  t*(1.0-u)*(1.0-v)*field.at(ix[1]).at(iy[0]).at(iz[0]).at(kc);
	  B[kc] +=  (1.0-t)*u*(1.0-v)*field.at(ix[0]).at(iy[1]).at(iz[0]).at(kc);
	  B[kc] +=  t*u*(1.0-v)*field.at(ix[1]).at(iy[1]).at(iz[0]).at(kc);
          B[kc] +=  (1.0-t)*(1.0-u)*v*field.at(ix[0]).at(iy[0]).at(iz[1]).at(kc);
	  B[kc] +=  t*(1.0-u)*v*field.at(ix[1]).at(iy[0]).at(iz[1]).at(kc);
	  B[kc] +=  (1.0-t)*u*v*field.at(ix[0]).at(iy[1]).at(iz[1]).at(kc);
	  B[kc] +=  t*u*v*field.at(ix[1]).at(iy[1]).at(iz[1]).at(kc);
        }
      }
    } // map storage. 
    if (debugIsOn) {
	
	std::cerr << "(x, y, z) = (" << x[0] << ", " << x[1] << ", " << x[2] 
	          << ") mm,    (Bx, By, Bz) = (" << B[0] << ", " << B[1] << ", " << B[2] << ") kG" << G4endl;
     }
    
  }
  
  
  CLHEP::Hep3Vector EMPHATICMagneticField::
  MagneticField(const CLHEP::Hep3Vector point) const {
    
    G4double x[3],B[3];
    CLHEP::Hep3Vector v;
    
    x[0] = point.x();
    x[1] = point.y();
    x[2] = point.z();
    EMPHATICMagneticField::MagneticField(x, B);
    v.setX(B[0]);   
    v.setY(B[1]);   
    v.setZ(B[2]);   
    return v;
  }
  
  
  void EMPHATICMagneticField::GetFieldValue(const double x[3], double* B) const
  {
    if (!fHasBeenAligned) {
      std::cerr << " EMPHATICMagneticField::GetFieldValue, " 
                <<  " Magnet has not been properly aligned, ZipTrack Coordinate system is not the G4 Coordinate system " << std::endl; 
    
      std::cerr << " Fatal error, for now... Quit here and now " << std::endl; exit(2);
    } 
    double xAligned[3];
    for (size_t k=0; k != 3; k++) xAligned[k] = x[k] + fG4ZipTrackOffset[k];
    double BInKg[3];
    const double rR = std::sqrt(x[0]*x[0] + x[1]*x[1]);
    if ((!fUseOnlyCentralPart) || (rR < fInnerBoreRadius)) {
      EMPHATICMagneticField::MagneticField(xAligned, BInKg);
    } else {
      EMPHATICMagneticField::MagneticFieldFromCentralBore(xAligned, BInKg);
    }
    for (size_t k=0; k != 3; k++) B[k] = BInKg[k]*CLHEP::kilogauss;
  }
    
  void EMPHATICMagneticField::Integrate(int iOpt, int charge, double stepAlongZ,
                            std::vector<double> &start, std::vector<double> &end) const {
    
    if ((start.size() != 6) || (end.size() != 6)) {
      std::cerr << " EMPHATICMagneticField::Integrate , wrong arguments, vectors must be dimensioned to 6. Fatal, quit here and now " << std::endl;
      exit(2);
    }
    const double QCst = charge * 1.0e3 / 0.03; // meter to mm, factor 10 to convert kG to Tesla. The factor 3 in denomintar is standard MKS units. 
    const bool doEuler = (iOpt == 0) || (iOpt == 10) ||  (iOpt == 100);
    const bool doOnlyBy = (iOpt/10 == 1);
    const double ZAccuracy = 1.0; // one mm, guess should be good enough. 
    double stepZ = stepAlongZ;
    std::vector<double> pos(start); // x,y ,z coordinate along the integration path. 
    bool debugIsOn = false;
    if (debugIsOn) {
       std::cerr << " EMPHATICMagneticField::Integrate, starting step along z " << stepAlongZ; 
       if (doEuler) std::cerr << " Simple Euler formula "; 
       else  std::cerr << " Runge-Kutta formula ";
       if (doOnlyBy) std::cerr << " Assume Bx = Bz = 0. "; 
       else std::cerr << " Assume Bz = 0. "; 
       std::cerr << std::endl;
    } 
    double xxStart[3], xxStop[3], xxMiddle[3], bAtZStart[3], bAtZStop[3], bAtZMiddle[3], bAtFirst[3], bAtSecond[3]; // temporary storage
    double slx = start[3]/start[5];  double sly = start[4]/start[5]; // approximate cosine director 
    double pSq = 0.; for (size_t k=0; k !=3; k++) pSq =+ start[k+3]*start[k+3]; 
    const double p = std::sqrt(pSq); // Momentum, a conserved quatity. 
    while (std::abs(pos[2] - end[2]) > ZAccuracy) {
       // check that we don't run past the end point 
       while (true) {
         if ((pos[2] + stepZ) < end[2]) break;
	 stepZ = stepZ/2.;
      }
      slx = pos[3]/p; // exact 
      sly = pos[4]/p;
      double slz = std::sqrt(1. - slx*slx - sly*sly);
      for (size_t k=0; k !=3; k++) { 
         xxStart[k] = pos[k]; 
         xxMiddle[k] = pos[k]; 
         xxStop[k] = pos[k]; 
      }
      xxMiddle[0] += slx*stepZ/2.; xxMiddle[1] += sly*stepZ/2.;
      xxStop[0] += slx*stepZ; xxStop[1] += sly*stepZ;
      if (debugIsOn) std::cerr << " At x,y,z " << pos[0] << " " << pos[1] << " " << pos[2] << " step size " << stepZ << std::endl;
      this->MagneticField(xxMiddle, bAtZMiddle);
      //
      // Change of slope along the X-axis (dominant component).
      //
      if (std::abs(bAtZMiddle[1]) > 0.5e-3) { 
	if (doEuler) { 
          const double radXMiddle = QCst *  p * slz / bAtZMiddle[1]; // in Tesla, radius in mm 
          const double dSlx = stepZ/radXMiddle; // first order, Euler method.
	  slx +=  dSlx;
	  if (debugIsOn) std::cerr << " ...............  delta Slx " << dSlx << " new slx " << slx << std::endl;
	} else { 
         this->MagneticField(xxStart, bAtZStart);
         const double radXStart = QCst *  p * slz / bAtZStart[1]; // in Tesla, radius in mm 
         const double dSlxK1 = stepZ/radXStart; // first order, Euler method.
	 double xxPos1[3]; for(size_t k=0; k !=3; k++) xxPos1[k] = xxMiddle[k];
	 xxPos1[0] += dSlxK1*stepZ/2.;
         this->MagneticField(xxPos1, bAtFirst);
         const double radXFirst = QCst *  p * slz /bAtFirst[1]; 
	 const double dSlxK2 = 0.5*stepZ/radXFirst;
	 double xxPos2[3]; for(size_t k=0; k !=3; k++) xxPos2[k] = xxMiddle[k];
	 xxPos2[0] += dSlxK2*stepZ/2.;
         this->MagneticField(xxPos2, bAtSecond);
         const double radXSecond = QCst *  p * slz /bAtSecond[1]; 
	 const double dSlxK3 = 0.5*stepZ/radXSecond;
	 xxStop[0] += dSlxK3*stepZ;
         this->MagneticField(xxStop, bAtZStop);
         const double radXLast = QCst *  p * slz /bAtZStop[1]; 
	 const double dSlxK4 = stepZ/radXLast;
	 const double dSlxRK = dSlxK1/6. + dSlxK2/3. + dSlxK3/3. + dSlxK4/6.;
	 slx += 1.5*dSlxRK; // if uniform field, all dSlx are equal, expect the same answer as Euler method. 
	 if (debugIsOn) std::cerr << " delta slx s " << dSlxK1 << ", " <<  dSlxK2 << ", " << dSlxK3 << ", " << dSlxK4 << ", "
	                          << " sum delta Slx " << 1.5*dSlxRK << " new slx " << slx << std::endl;
       } // Euler vs Runge-Kutta 4rth order. 
      } // finite component of By   
      if ((!doOnlyBy) &&  std::abs(bAtZMiddle[0]) > 0.5e-3) { 
        const double radYMiddle = -1.0 * QCst *  p * slz / bAtZMiddle[0]; // in Tesla, radius in mm 
        const double dSly = stepZ/radYMiddle; // first order, Euler method.
	if (doEuler) { 
	  sly +=  dSly;
	  if (debugIsOn) std::cerr << " ...............  delta Sly " << dSly << " new sly " << sly << std::endl;
	} else { 
          const double radYStart = -1.0 * QCst *  p * slz / bAtZStart[0]; // in Tesla, radius in mm 
          const double dSlyK1 = stepZ/radYStart; // first order, Euler method.
          const double radYFirst = -1.0 * QCst *  p * slz /bAtFirst[0]; 
	  const double dSlyK2 = 0.5*stepZ/radYFirst;
          const double radYSecond = QCst *  p * slz /bAtSecond[0]; 
	  const double dSlyK3 = 0.5*stepZ/radYSecond;
          const double radYLast = -1.0 * QCst *  p * slz /bAtZStop[0]; 
	  const double dSlyK4 = stepZ/radYLast;
	  const double dSlyRK = dSlyK1/6. + dSlyK2/3. + dSlyK3/3. + dSlyK4/6.;
	  sly += 1.5*dSlyRK;
	  if (debugIsOn) std::cerr << " delta sly s " << dSlyK1 << ", " <<  dSlyK2 << ", " << dSlyK3 << ", " << dSlyK4 << ", "
	                          << " sum delta sly " << 1.5*dSlyRK << " new sly " << sly << std::endl;
       } // Euler vs Runge-Kutta 4rth order. 
      } // finite component of Bx   
  
      pos[0] += slx*stepZ;
      pos[1] += sly*stepZ;
      pos[2] += stepZ;
      pos[3] = p * slx;
      pos[4] = p * sly;
      slz = std::sqrt(1. - slx*slx - sly*sly);
      pos[5] = p * slz;
       //
       // neglect Bz 
	 
     } // integration along the path.. 
      if (debugIsOn) std::cerr << " .. Final slopes, x, y, " << slx << ", " << sly <<  " and position " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
     for (size_t k=0; k != 6; k++) end[k] = pos[k]; 
  } 
  void EMPHATICMagneticField::NoteOnDoubleFromASCIIFromCOMSOL() const {
  
    std::cerr << " EMPHATICMagneticField::NoteOnDoubleFromASCIIFromCOMSOL, Consider the following line " << std::endl;
    std::string aL("-2.5                      5                        -142.50000000000003      9.418797023542423E-5     0.0737959778472815       0.012478406096875226");
    std::cerr << aL << std::endl;
    std::cerr << " ... and the the following conversion to double (see code) " << std::endl;
     double numbers[6];
     int count = 0; 
     std::string num;
     std::stringstream aLStr(aL);
     while((count < 6) && (aLStr >> num)){
   	     numbers[count] = std::stod(num); // probably a bad idea to treat NaN as field is really physically vanishing.. 
  				count++;
     }
     // we know convert to an index in the 3D array. 
     const double zMin = -350.; const double stepTmp = 2.5;
     size_t iZ = static_cast<size_t>(floor((numbers[2] - zMin)/stepTmp)); 
     std::cerr << " ...  iZ = " << iZ << " while expecting 83 = ((-142.5 +  350)/2.5 " << std::endl;
     // Indeed 
     const double zIncorrect = -142.50000000000003;
     const double zCorrect = -142.5;
     if (zIncorrect != zCorrect) { 
        std::cerr << " .. The trailing 0000000000003 does matter, in double precision.. see code...  " << std::endl;
	size_t iZZ = static_cast<size_t>(floor((zIncorrect - zMin)/stepTmp));
	std::cerr << " The floor function, part of stdlib is correct: applying to -142.50000000000003 gives an index of " << iZZ << std::endl;
     } else std::cerr << " .. The trailing 0000000000003 does not matter, the floor seems to be the problem. " << std::endl;
     //
     std::cerr << " The (ugly) fix is to add a negligible  quantity, postive,  to the double prior to the conversion to an index " << std::endl << std::endl;;
    
  } 

  void EMPHATICMagneticField::MagneticFieldFromCentralBore(const double Point[3], double BApprox[3]) const {
  // Crude extrapolation. 
    const double phi = std::atan2 (Point[1], Point[0]);
    double PointB[3];  PointB[0] = fInnerBoreRadius*std::cos(phi); PointB[1] = fInnerBoreRadius*std::sin(phi), PointB[2] = Point[2];
    double PointIn[3]; PointIn[0] = (Point[0] - PointB[0]); PointIn[1] = (Point[1] - PointB[1]); PointIn[2] = Point[2];
    double Bb[3]; this->MagneticField(PointB, Bb); double BIn[3];  this->MagneticField(PointIn, BIn);
    CLHEP::Hep3Vector deltaB;;
    for (int k=0; k!=3; k++) {
      deltaB[k] = (Bb[k] - BIn[k]);
      BApprox[k] = Bb[k] + deltaB[k];
      if (BApprox[k] > 12.) BApprox[k] = 12.0;
      if (BApprox[k] < -12.) BApprox[k] = -12.0;
    }
//    if (std::abs(Bb[1]) > 7.0) { 
//      std::cerr << " EMPHATICMagneticField::MagneticFieldFromCentralBore, at R = " << rR << " phi " << phi << std::endl;
//      std::cerr << " At bore, By " << Bb[1] << " Point In, x=" <<  PointIn[0] << " y=  " << PointIn[1] << ", z " << PointIn[2]
//                << " so, BIn y " << BIn[1] << " deltas B " << deltaB[1] << " final " << BApprox[1] << std::endl;
//    }
  }

  void EMPHATICMagneticField::test1() {
    
    
    // Look at the divergence of the field.. 
    //
    std::cerr << " EMPHATICMagneticField::test1, generating simple CSV test files.. " << std::endl;
    std::string fName1 = fStorageIsStlVector ? std::string("./EmphMagField_StlVector_v2a.txt") : std::string("./EmphMagField_StlMap_v2a.txt"); 
    std ::ofstream fOutForR(fName1.c_str());
    fOutForR << " x y z B0x B0y B0z B0 dB0xdx dB0ydy dB0zdz divB0 B1x B1y B1z B1 divB1" << std::endl;
    
    const double delta = 5.;
    double xN[3], xF[3], B0N[3], B0F[3], B1N[3], B1F[3];
    for (int iX = -10; iX != 10; iX++) { 
       const double x = 7.5 + iX*0.956; 
       xN[0] = x; // in mm, apparently... 
       xF[0] = x + delta;
       for (int iY = -10; iY != 20; iY++) { 
        const double y = 15.34 + iY*0.892; 
         xN[1] = y ;
         xF[1] = y + delta;
         for (int iZ = -80; iZ != 120; iZ++) { 
           const double z = iZ*4.578; 
           xN[2] = z;
           xF[2] = z + delta;
           this->setInterpolatingOption(0);
           this->MagneticField(xN, B0N); // xN is in mm.. 
           this->MagneticField(xF, B0F);
	   //
	   // Bug in the sign of Bz ???? 
	   //
	   B0N[2] *= -1.; B0F[2] *= -1.;
	   double divB0 = 0.; double b0Norm = 0.;
	   for (size_t kk=0; kk != 3; kk++) { 
	     divB0 += (B0F[kk] - B0N[kk])/delta; // kG/mm 
	     b0Norm += B0N[kk]*B0N[kk];
	   }
	   fOutForR << " " << x << " " << y << " " << z << " " 
	            << B0N[0] << " " << B0N[1] << " " << B0N[2] << " " << std::sqrt(b0Norm) << " " 
		    << (B0F[0] - B0N[0])/delta << " " <<  (B0F[1] - B0N[1])/delta << " " << (B0F[2] - B0N[2])/delta << " " << divB0;
           this->setInterpolatingOption(1);
           this->MagneticField(xN, B1N); // xN is in mm.. 
           this->MagneticField(xF, B1F);
	   //
	   // Bug in the sign of Bz ???? 
	   //
	   B0N[2] *= -1.; B0F[2] *= -1.;
	   double divB1 = 0.; double b1Norm = 0.;
	   for (size_t kk=0; kk != 3; kk++) { 
	     divB1 += (B1F[kk] - B1N[kk])/10.; // kG/mm 
	     b1Norm += B1N[kk]*B1N[kk];
	   }
	   fOutForR << " " << B1N[0] << " " << B1N[1] << " " << B1N[2] << " " 
		    << std::sqrt(b1Norm) << " " << divB1 << std::endl;
       }
      }
    }
    fOutForR.close();
    std::cerr << " Quit, debugging anomalous difference between stl vector and map " << std::endl; exit(2);
    std::string fName2 = fStorageIsStlVector ? std::string("./EmphMagField_StlVector_v2c.txt") : std::string("./EmphMagField_StlMap_v2c.txt"); 
    fOutForR.open(fName2.c_str());
    fOutForR << " x y z B0x B0y B0z B0 dB0xdx dB0ydy dB0zdz divB0 B1x B1y B1z B1 divB1" << std::endl;
    for (int iX = -150; iX != 150; iX++) { 
       const double x = 7.5 + iX*0.956; 
       xN[0] = x; // in mm, apparently... 
       xF[0] = x + delta;
       for (int iY = -10; iY != 20; iY++) { 
        const double y = 15.34 + iY*0.892; 
         xN[1] = y ;
         xF[1] = y + delta;
         for (int iZ = -5; iZ != 5; iZ++) { 
           const double z = 0. + iZ*4.578; 
           xN[2] = z;
           xF[2] = z + delta;
           this->setInterpolatingOption(0);
           this->MagneticField(xN, B0N); // xN is in mm.. 
           this->MagneticField(xF, B0F);
	   //
	   // Bug in the sign of Bz ???? 
	   //
	   B0N[2] *= -1.; B0F[2] *= -1.;
	   double divB0 = 0.; double b0Norm = 0.;
	   for (size_t kk=0; kk != 3; kk++) { 
	     divB0 += (B0F[kk] - B0N[kk])/delta; // kG/mm 
	     b0Norm += B0N[kk]*B0N[kk];
	   }
	   fOutForR << " " << x << " " << y << " " << z << " " 
	            << B0N[0] << " " << B0N[1] << " " << B0N[2] << " " 
		    << std::sqrt(b0Norm) << " " << 
		    (B0F[0] - B0N[0])/delta << " " <<  (B0F[1] - B0N[1])/delta << " " << (B0F[2] - B0N[2])/delta << " " << divB0;
           this->setInterpolatingOption(1);
           this->MagneticField(xN, B1N); // xN is in mm.. 
           this->MagneticField(xF, B1F);
	   //
	   // Bug in the sign of Bz ???? 
	   //
	   B0N[2] *= -1.; B0F[2] *= -1.;
	   double divB1 = 0.; double b1Norm = 0.;
	   for (size_t kk=0; kk != 3; kk++) { 
	     divB1 += (B1F[kk] - B1N[kk])/10.; // kG/mm 
	     b1Norm += B1N[kk]*B1N[kk];
	   }
	   fOutForR << " " << B1N[0] << " " << B1N[1] << " " << B1N[2] << " " 
		    << std::sqrt(b1Norm) << " " << divB1 << std::endl;
       }
      }
    }
    
    fOutForR.close();
   //
    // final test, random points, lots of them, to measure performance.. 
    // Since this is a local stress test, no need to keep track of seeds, random correctness, just use rand
    //
    /*
    std::cerr << " ....  Start random poking, stress test, and performance " << std::endl;
    this->setInterpolatingOption(1);
    for (int izzz =0; izzz < 10; izzz++) {
      std::cerr << " izzzzz.. " << izzz << std::endl; 
      for (int k=0; k != 100000000; k++) {
//                       123456789
         xN[0] = -180. + 360.0*static_cast<double>(rand())/RAND_MAX; 
         xN[1] = -180. + 360.0*static_cast<double>(rand())/RAND_MAX; 
         xN[2] = -400. + 400.0*static_cast<double>(rand())/RAND_MAX; 
         this->MagneticField(xN, B1N); // xN is in mm.. 
         if (( k < 5) || ((k % 5000000) == 1) ) 
            std::cerr << " k " << k << " x = " << xN[0] << " y " << xN[1] << " z " << xN[2] << " By " << B1N[1] << std::endl;
      }
    }
  */  
   
  }
  
  void EMPHATICMagneticField::test2() {
    
//      std::cerr << " EMPHATICMagneticField::test2, and quit now !... " << std::endl; exit(2);
      std::vector<double> start(6); 
      std::vector<double> stop(6);
      std::cerr << " EMPHATICMagneticField::test2 setting interpolation 3D radial " << std::endl;
      this->setInterpolatingOption(0); 
      double p = 10.0;
      start[0] = 4.0; start[1] = 11.0; start[2] = - 15.0; stop[2] = 450.; 
      double DeltaZ = stop[2] - start[2];
      double slx =0.1e-3; double sly = -0.2e-3; double slz = std::sqrt( 1.0 - slx*slx - sly*sly); 
      start[3] = p*slx; start[4] = p*sly;  start[5] = slz*p;
    
      this->Integrate(10, 1, 15.0, start, stop);
      double slxFinal = stop[3]/p;      double slyFinal = stop[4]/p;
      std::cerr << " .. Euler, Bx = Bz = 0., Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl; 
      stop[2] = 450.; 	  
      this->Integrate(0, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " ..Euler, Bz = 0.,  Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      stop[2] = 450.; 	  
      this->Integrate(1, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " ..RungeKutta, Bz = 0.,  Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   

      this->setInterpolatingOption(1); 
      std::cerr << " EMPHATICMagneticField::test2 setting interpolation 3D grid, linear " << std::endl;
      this->Integrate(10, 1, 15.0, start, stop);
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " .. Euler, Bx = Bz = 0., Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      stop[2] = 450.;  
      this->Integrate(0, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " .... Euler, Bz = 0. Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      stop[2] = 450.; 	  
      this->Integrate(1, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " ..RungeKutta, Bz = 0.,  Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      
      std ::ofstream fOutForR("./EmphMagField_p10_v3.txt");
      fOutForR << " p stepZ y dSlx3DR_E dslxLin_E dSlx3DR_RK dslxLin_RK " << std::endl;
      for (int kp=1; kp !=3; kp++) { 
        p = 1.0; 
	if (kp == 2) p = 10.;
        for (int kStep=1; kStep != 15; kStep++) {
          const double stepZ = 2.0 + 0.1*kStep*kStep;
          for (int kY= -100; kY != 100.; kY++) { 
            start[0] = -4.0; start[1] = 0.5*kY; start[2] = - 200.0; stop[2] = 200.; 
            DeltaZ = stop[2] - start[2];
            slx =0.1e-5; sly = -0.2e-5; slz = std::sqrt( 1.0 - slx*slx - sly*sly); 
            start[3] = p*slx; start[4] = p*sly;  start[5] = slz*p;
	    this->setInterpolatingOption(0); 
            this->Integrate(0, 1, stepZ, start, stop); 
	    const double dSlx3DR_E = stop[3]/p - slx;
	    stop[2] = 450.;
            this->Integrate(1, 1, stepZ, start, stop); 
	    const double dSlx3DR_RK = stop[3]/p - slx;
	    stop[2] = 450.;
	    this->setInterpolatingOption(1); 
            this->Integrate(0, 1, stepZ, start, stop); 
	    const double dSlxLin_E = stop[3]/p - slx;
	    stop[2] = 450.;
            this->Integrate(1, 1, stepZ, start, stop); 
	    const double dSlxLin_RK = stop[3]/p - slx;
	    stop[2] = 450.;
	    fOutForR << " " << p << " " << stepZ << " " << start[1] << " " 
	             << dSlx3DR_E << " " << dSlxLin_E << " " << dSlx3DR_RK << " " << dSlxLin_RK << std::endl;
	  }
	}
      }
      fOutForR.close();
    }  
  void EMPHATICMagneticField::test3() {
    //
    // Assuming a beam spot size of 1 cm, Gaussian, 
    //
    std::vector<double> start(6); 
    std::vector<double> stop(6);
    this->setInterpolatingOption(0); 
    const double p = 5.0; // arbitrarily 
    int numEvts = 1000000;
    // Test access to a Geant4 random number.. 
    //
    std ::ofstream fOutForR("./EmphMagField_test3_p5_Acceptance_v1.txt");
    fOutForR << " iEvt xS yS slxI slxI2 xF yF xFS2 yFS2 slxF slxFS2  xFMagP5 yFMagP5 slxFMagP5" << std::endl;
    start[2] = -200.;
    stop[2] = 450.;
    double pt = 0.1;
    for (int kEvt=0; kEvt != numEvts; kEvt++) {
      const double xNoOffset = 10.0*G4RandGauss::shoot();
      start[0] = xNoOffset;
      start[1] = 10.0*G4RandGauss::shoot();
      start[2] = -200.;
      stop[2] = 450.;
      double slxNoOff =( pt/p)*2.0*M_PI*G4RandGauss::shoot();
      double slx2Off = 2.0e-3 + slxNoOff;
      double sly =( pt/p)*2.0*M_PI*G4UniformRand();
      start[3] = p*slxNoOff; start[4] = p*sly; 
      double slz = std::sqrt( 1.0 - slxNoOff*slxNoOff - sly*sly);  
      start[5] = slz*p;
      stop[0] = -1.e10; stop[1] = -1.e10;
      this->Integrate(0, 1, 2., start, stop);
      const double xF = stop[0];
      const double slxFinal = stop[3]/p;
      const double yF = stop[1];
      stop[0] = -1.e10; stop[1] = -1.e10;      
      start[3] = p*slx2Off; start[4] = p*sly;
      start[2] = -200.;
      stop[2] = 450.;
      this->Integrate(0, 1, 2., start, stop);
      const double xFS2 = stop[0];
      const double yFS2 = stop[1];
      const double slxFinalS2 = stop[3]/p;
      stop[0] = -1.e10; stop[1] = -1.e10; 
      start[0] =  xNoOffset - 5.0;  
      start[3] = p*slxNoOff; start[4] = p*sly;
      start[2] = -200.;
      stop[2] = 450.;
      this->Integrate(0, 1, 2., start, stop);
      const double xFMagP5 = stop[0];
      const double yFMagP5 = stop[1];
      const double slxFinalMagP5 = stop[3]/p;
      fOutForR << " " << kEvt << " " << start[0] << " " << start[1] << " " <<  1000.0*slxNoOff << " " << 1000.0*slx2Off 
               << " " << xF << " " << yF << " " << xFS2 << " " << yFS2 << " " << 1000.0*slxFinal << " " << 1000.0*slxFinalS2
	       << " " << xFMagP5 << " " << yFMagP5 << " " << 1000.0*slxFinalMagP5 << std::endl;
    }
    fOutForR.close();
  }  
      
  std::pair<double, double> EMPHATICMagneticField::getMaxByAtCenter()  {
  
      double xN[3], B0N[3];
      xN[0] = 0.; xN[1] = 0.;
      double byMax=-1000.;
      double zAtMax=-1000.;
      this->setInterpolatingOption(0);
      for (int iZ = -300; iZ != 500; iZ++) {
        xN[2] = 1.0*iZ;
        this->MagneticField(xN, B0N); // xN is in mm.. 
	if (std::abs(B0N[1]) < byMax) continue;
	 byMax = std::abs(B0N[1]);
	 zAtMax = xN[2];
      }
      return std::pair<double, double>(zAtMax, byMax);
  }


  void EMPHATICMagneticField::studyZipTrackData1() {
    
//    std::string fNameZipIn("/home/lebrun/EMPHATIC/Documents/MagnetJune2022/DataMay22/Ycentered_X348_ByUp_220602-0953.csv");
    std::string fNameZipIn("/home/lebrun/EMPHATIC/Documents/MagnetJune2022/DataMay22/Ycentered_X358_ByUp_220601-1506_LevelShift.csv");
    this->uploadFromOneCSVZipFile(fNameZipIn);
    double zFieldMax = 0.;
    double byMax = -1000.;
    for (std::vector<bFieldZipTrackPoint>::const_iterator itZ = ffieldZipTrack.cbegin();  itZ != ffieldZipTrack.cend(); itZ++) { 
      if (std::abs(static_cast<double>(itZ->fby)) > byMax) { byMax = static_cast<double>(std::abs(itZ->fby)); zFieldMax = static_cast<double>(itZ->z); }
    }
    std::cerr << " EMPHATICMagneticField::studyZipTrackData1, number of ZipTrack Pts " << ffieldZipTrack.size() 
              << " By max " << byMax << " at Z = " << zFieldMax << std::endl;
    std::pair<double, double> byMaxCOMSOL = this->getMaxByAtCenter();	      
    std::cerr << " ... and from COMSOL, max at z = " << byMaxCOMSOL.first << " |by| " << byMaxCOMSOL.second << std::endl;
    // 
    // Assume we are the real probe is centered, bith X and Y Look at the Z profile.. 
    //
    const double zOffset = byMaxCOMSOL.first - zFieldMax;
    std::ofstream fOut("./studyZipTrackData1_CentralScan_Ycentered_X358_ByUp.txt");
    fOut << " z BxP ByP BzP Bp BxC ByC BzC BC " << std::endl;
    double xN[3], B0N[3];
    xN[0] = 0.; xN[1] = 0.;
    for (std::vector<bFieldZipTrackPoint>::const_iterator itZ = ffieldZipTrack.cbegin();  itZ != ffieldZipTrack.cend(); itZ++) { 
      xN[2] = itZ->z + zOffset; 
      this->MagneticField(xN, B0N);
      for (int k=0; k != 3; k++) B0N[k] /=10.; // to Tesla.  
      double bNormC = 0.; for (int k=0; k != 3; k++) bNormC += B0N[k]*B0N[k];
      bNormC = std::sqrt(bNormC);
      const double bNormP = std::sqrt(itZ->fbx*itZ->fbx + itZ->fby*itZ->fby + itZ->fbz*itZ->fbz);
      fOut << " " << itZ->z << " " << -1.0*itZ->fbx << " " <<  -1.0*itZ->fby << " " << -1.0*itZ->fbz << " "  
                  << bNormP << " " << B0N[0] << " " << B0N[1] << " " << B0N[2] << " "  << bNormC << std::endl;
    }
    fOut.close();
    // Test access to a Geant4 random number.. 
    //
   //    std ::ofstream fOutForR("./EmphMagField_test3_p5_Acceptance_v1.txt");
  }
} // end namespace emph

