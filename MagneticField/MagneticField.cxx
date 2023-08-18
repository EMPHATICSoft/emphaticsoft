///////////////////////////////////////////////////////////////////////////////
// File: MagneticField.cxx
// Description: User Field class implementation.
///////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "MagneticField/MagneticField.h"

#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
#include "TFile.h"
#include "TTree.h"
#include "math.h"

// For testing purposes. 
//#include "Geant4/Randomize.hh"

namespace emph {
  
  MagneticField::MagneticField() :
    fFieldFileName(""), fFieldLoaded(false), 
    fStorageIsStlVector(true), 
    step(0), start{-16., -16., -20.},
    fG4ZipTrackOffset{ 0., 0., 0.},
    fNStepX(1), fNStepY(1), fNStepZ(1),
    fXMin(6.0e23),  fYMin(6.0e23), fZMin(6.0e23), 
    fXMax(-6.0e23), fYMax(-6.0e23), fZMax(-6.0e23),
    fStepX(0.), fStepY(0.), fStepZ(0.),
    fInterpolateOption(0), fVerbosity(0)
    {

    }
    
  //----------------------------------------------------------------------
  
  MagneticField::~MagneticField() {
  }

  //----------------------------------------------------------------------

  void MagneticField::AlignWithGeom() {
    // by convention, the field map's local coordinate system has it's orgin at the center of the upstream face of the magnet
    art::ServiceHandle<emph::geo::GeometryService> geomService;
    const emph::geo::Geometry* geo = geomService->Geo();

    fG4ZipTrackOffset[2] = -geo->MagnetUSZPos();

    /*
      Need to add some functionality to the Geometry class to provide these
      fG4ZipTrackOffset[0] = -geo->MagnetUSXPos();
      fG4ZipTrackOffset[1] = -geo->MagnetUSYPos();
    */

    if (fVerbosity)
      std::cerr << " MagneticField::AlignWithGeom G4ZipTrack Z Offset set to " << fG4ZipTrackOffset[2] << std::endl;

    if (fVerbosity > 1) {
      //
      // Testing... at COMSOL coordinate of z = -82.5 mm, By ~ 7.5 Kg, 1/2  field 
      //     
      double xTest[3], xTest2[3], BTest[3], BTest2[3]; 
      xTest[0] = 0.; xTest[1] = 0.; xTest[2] = -geo->MagnetUSZPos();  
      xTest2[0] = 0.01; xTest2[1] = 0.004; xTest2[2] = 30.; // in mm 
      this->Field(xTest, BTest);
      std::cerr << " MagneticField::AlignWithGeom, BField at Upstream plate, internal Variables  " 
		<< BTest[1] <<  " kG "  << std::endl;
      this->Field(xTest2, BTest2);
      std::cerr << " .......... again, 30mm inside " << BTest2[1] << std::endl;
      for (size_t k=0; k != 3; k++) xTest[k] -= fG4ZipTrackOffset[k];
      BTest[1] = 0.;
      this->GetFieldValue(xTest, BTest);
      std::cerr << " MagneticField::AlignWithGeom, BField at Upstream plate, G4 Coordinates   " << BTest[1] <<  " kG " << std::endl;      
    }
  }
  
  //----------------------------------------------------------------------

  void MagneticField::uploadFromTextFile() 
  {
    if (fFieldFileName == "") {
      std::cerr << "Magnetic Field file name is not set, aborting..." << std::endl;
      abort();
    }

    double numbers[6];
    std::ifstream fileIn(fFieldFileName.c_str());
    if (!fileIn.is_open()) {
      std::cerr << " MagneticField::uploadFromTextFile, file " << fFieldFileName 
		<< " can not be open, fatal .." << std::endl; 
      //      std::cerr << " Please copy the file  " << fName << " from emphaticgpvm02.fnal.gov:/emph/data/users/lebrun/" << std::endl; 
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
      if (isnan(xC) || isnan(yC) || isnan(zC)) {
	std::cerr << " MagneticField::uploadFromTextFile, line  " << numLines << " has NaN(s), fatal.. " << std::endl;
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
	  std::cerr << " MagneticField::uploadFromTextFile, no information about the step size along the X axis, fatal  " << std::endl;
	  exit(2);	
	}
	const double varStepX = (sumStepSqX - fNStepX*fStepX*fStepX)/((fNStepX-2)*(fNStepX-1)) ;
	if (varStepX > 1. ) {
	  std::cerr << " MagneticField::uploadFromTextFile, variance of steps along X greate than 1 mm square, " << varStepX 
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
	  std::cerr << " MagneticField::uploadFromTextFile, the scans along the X axis don't match.. " << std::endl; 
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
	    std::cerr << " MagneticField::uploadFromTextFile, no information about the step size along the Y axis, fatal  " << std::endl;
	    exit(2);	
	  }
	  const double varStepY = (sumStepSqY - fNStepY*fStepY*fStepY)/((fNStepY-2)*(fNStepY-1)) ;
	  if (varStepY > 1. ) {
	    std::cerr << " MagneticField::uploadFromTextFile, variance of steps along Y greate than 1 mm square, " << varStepY 
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
	      std::cerr << " MagneticField::uploadFromTextFile, the scans along the Y axis don't match.. " << std::endl; 
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
      std::cerr << " MagneticField::uploadFromTextFile, no information about the step size along the Z axis, fatal  " << std::endl;
      exit(2);	
    }
    const double varStepZ = (sumStepSqZ - fNStepZ*fStepZ*fStepZ)/(fNStepZ*(fNStepZ-1)) ;
    if (varStepZ > 1. ) {
      std::cerr << " MagneticField::uploadFromTextFile, variance of steps along Z greate than 1 mm square, " << varStepZ 
		<< " fNStepZ " << fNStepZ << ", fStepZ " << fStepZ << " fatal  " << std::endl;
      exit(2);	
    }
    if (fVerbosity == 1) { 
      std::cerr << " MagneticField::uploadFromTextFile, The grid is considered as regular, numPtx along X = " 
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
      for (size_t i=0; i != nTot; i++) fBfield.push_back(aBV);
    } else {
      start[0] = fXMin; start[1] = fYMin; start[2] = fZMin; step = fStepZ; // old notation.. keep for consistency. 
    }
    fileIn.open(fFieldFileName.c_str());
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
	if (ii < fBfield.size()) { 
	  fBfield[ii].fbx = 10.0*numbers[3]; fBfield[ii].fby =10.0* numbers[4]; fBfield[ii].fbz = 10.0*numbers[5]; // Tesla to KG
	}
      } else {
	int indX = static_cast<int>(floor(numbers[0]-start[0])/step);
	int indY = static_cast<int>(floor(numbers[1]-start[1])/step);
	int indZ = static_cast<int>(floor(numbers[2]-start[2])/step);
	std::vector<double> temp;
	temp.push_back(numbers[3]*10.); // Tesla to kG. 
	temp.push_back(numbers[4]*10.);
	temp.push_back(numbers[5]*10.);
	fFieldMap[indX][indY][indZ] = temp;
      }
    }
    fileIn.close(); 
    int  numNanInTable = 0;
    for (size_t k=0; k != fBfield.size(); k++) {
      if (isnan(fBfield[k].fbx) || isnan(fBfield[k].fby) || isnan(fBfield[k].fbz)) {
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
    
    fFieldLoaded = true;

    this->AlignWithGeom();
  }
  
  //----------------------------------------------------------------------

  // Member functions
  
  void MagneticField::Field(const double x[3], double B[3]) 
  {
    B[0] = 0.; // a bit of a waste of CPU, but it makes the code a bit cleaner 
    B[1] = 0.;
    B[2] = 0.; 
    if (fVerbosity) std::cerr << " MagneticField::MagneticField, at x,y,z " << x[0] << ", " << x[1] << ", " << x[2] << std::endl; 
    if (fStorageIsStlVector) 
      CalcFieldFromVector(x,B);
    else
      CalcFieldFromMap(x,B);
    
    if (fVerbosity) {
      
      std::cerr << "(x, y, z) = (" << x[0] << ", " << x[1] << ", " << x[2] 
		<< ") mm,    (Bx, By, Bz) = (" << B[0] << ", " << B[1] << ", " << B[2] << ") kG" << std::endl;
    }

    return;
  }
  
  //----------------------------------------------------------------------

  void MagneticField::CalcFieldFromVector(const double x[3], double B[3]) 
  {
    if (!fFieldLoaded)
      this->uploadFromTextFile();

    if ((x[0] < fXMin) || (x[0] > fXMax) || (x[1] < fYMin) || (x[1] > fYMax)|| (x[2] < fZMin) || (x[2] > fZMax)) return;
    double indX = (x[0] - fXMin)/fStepX; // Check units.. 
    double indY = (x[1] - fYMin)/fStepY;
    double indZ = (x[2] - fZMin)/fStepZ;
    
    size_t ix[2] = {static_cast<size_t>(floor(indX)), static_cast<size_t>(ceil(indX))};
    size_t iy[2] = {static_cast<size_t>(floor(indY)), static_cast<size_t>(ceil(indY))};
    size_t iz[2] = {static_cast<size_t>(floor(indZ)), static_cast<size_t>(ceil(indZ))};
    if (fVerbosity) 
      std::cerr << " Indices .. x " << ix[0] << " " << ix[1] << " y " <<  iy[0] 
		<< " " << iy[1] << " z " <<  iz[0] << " " << iz[1] << std::endl;
    double sumx = 0.; double sumy = 0.; double sumz = 0.; double norm = 0.;
    if(fInterpolateOption == 0) { 
      for(int i = 0; i < 2; i++){
	for(int j = 0; j < 2; j++){
	  for(int k = 0; k < 2; k++){
	    size_t iV = indexForVector(ix[i], iy[j], iz[k]);
	    double dist = sqrt((indX-ix[i])*(indX-ix[i]) + (indY-iy[j])*(indY-iy[j]) + (indZ-iz[k])*(indZ-iz[k]));
	    sumx += fBfield[iV].fbx * dist;
	    sumy += fBfield[iV].fby * dist;
	    sumz += fBfield[iV].fbz * dist;
	    norm += dist;
	    if (fVerbosity) 
	      std::cerr << " ........ i j k " << i << " " << j << " " << k << " iV " << iV << " dist " << dist << " by " << fBfield[iV].fby << " sumy " << sumy << " norm " << norm << std::endl;
	  }
	}		
      }	
      B[0] = (sumx/norm);
      B[1] = (sumy/norm);
      B[2] = (sumz/norm);
      if (fVerbosity) std::cerr << " ... By .. " << B[1] << std::endl;
      return;      
    } 
    else {  // linear interpolation on the grid. 
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
	  B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*fBfield[iVXs[0]].fbx;
	  B[kc] +=  t*(1.0-u)*(1.0-v)*fBfield[iVXs[1]].fbx;
	  B[kc] +=  (1.0-t)*u*(1.0-v)*fBfield[iVXs[2]].fbx;
	  B[kc] +=  t*u*(1.0-v)*fBfield[iVXs[3]].fbx;
	  B[kc] +=  (1.0-t)*(1.0-u)*v*fBfield[iVXs[4]].fbx;
	  B[kc] +=  t*(1.0-u)*v*fBfield[iVXs[5]].fbx;
	  B[kc] +=  (1.0-t)*u*v*fBfield[iVXs[6]].fbx;
	  B[kc] +=  t*u*v*fBfield[iVXs[7]].fbx;
	  break;
	case 1:
	  B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*fBfield[iVXs[0]].fby;
	  B[kc] +=  t*(1.0-u)*(1.0-v)*fBfield[iVXs[1]].fby;
	  B[kc] +=  (1.0-t)*u*(1.0-v)*fBfield[iVXs[2]].fby;
	  B[kc] +=  t*u*(1.0-v)*fBfield[iVXs[3]].fby;
	  B[kc] +=  (1.0-t)*(1.0-u)*v*fBfield[iVXs[4]].fby;
	  B[kc] +=  t*(1.0-u)*v*fBfield[iVXs[5]].fby;
	  B[kc] +=  (1.0-t)*u*v*fBfield[iVXs[6]].fby;
	  B[kc] +=  t*u*v*fBfield[iVXs[7]].fby;
	  break;
	case 2:
	  B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*fBfield[iVXs[0]].fbz;
	  B[kc] +=  t*(1.0-u)*(1.0-v)*fBfield[iVXs[1]].fbz;
	  B[kc] +=  (1.0-t)*u*(1.0-v)*fBfield[iVXs[2]].fbz;
	  B[kc] +=  t*u*(1.0-v)*fBfield[iVXs[3]].fbz;
	  B[kc] +=  (1.0-t)*(1.0-u)*v*fBfield[iVXs[4]].fbz;
	  B[kc] +=  t*(1.0-u)*v*fBfield[iVXs[5]].fbz;
	  B[kc] +=  (1.0-t)*u*v*fBfield[iVXs[6]].fbz;
	  B[kc] +=  t*u*v*fBfield[iVXs[7]].fbz;
	  break;
	}
      } // three components of the field. 
      
      if (fVerbosity) std::cerr << " ... By .. " << B[1] << std::endl;
    }
  }
  
  //----------------------------------------------------------------------
  
  void MagneticField::CalcFieldFromMap(const double x[3], double B[3]) 
  {
    if (!fFieldLoaded)
      this->uploadFromTextFile();

    double indX = (x[0] - start[0])/step; 
    double indY = (x[1] - start[1])/step;
    double indZ = (x[2] - start[2])/step;
    
    int ix[2] = {int (floor(indX)), int (ceil(indX))};
    int iy[2] = {int (floor(indY)), int (ceil(indY))};
    int iz[2] = {int (floor(indZ)), int (ceil(indZ))}; 
    if (fVerbosity) {
      std::cerr << " .... Indices .. x " << ix[0] << " " << ix[1] << " y " 
		<<  iy[0] << " " << iy[1] << " z " <<  iz[0] << " " << iz[1] 
		<<  " .... step " << step << " start[0] " << start[0] << std::endl;
    }
    bool skip = false;
    if(fFieldMap.find(ix[0]) == fFieldMap.end()) { 
      if (fVerbosity) std::cerr << " x[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.find(ix[1]) == fFieldMap.end()) { 
      if (fVerbosity) 
	std::cerr <<  " x[1] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[0]).find(iy[0]) == fFieldMap.at(ix[0]).end()) { 
      if (fVerbosity) std::cerr <<  " x[0] y[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[0]).find(iy[1]) == fFieldMap.at(ix[0]).end()) { 
      if (fVerbosity) std::cerr <<  " x[0] y[1] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[1]).find(iy[0]) == fFieldMap.at(ix[1]).end()) { 
      if (fVerbosity) std::cerr <<  " x[1] y[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[1]).find(iy[1]) == fFieldMap.at(ix[1]).end()) { 
      if (fVerbosity) std::cerr <<  " x[1] y[1] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[0]).at(iy[0]).find(iz[0]) ==
	    fFieldMap.at(ix[0]).at(iy[0]).end()) { 
      if (fVerbosity) std::cerr <<  " x[0] y[0] z[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[0]).at(iy[0]).find(iz[1]) ==
	    fFieldMap.at(ix[0]).at(iy[0]).end()) { 
      if (fVerbosity) std::cerr <<  " x[0] y[0] z[1] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[0]).at(iy[1]).find(iz[0]) ==
	    fFieldMap.at(ix[0]).at(iy[1]).end()) { 
      if (fVerbosity) std::cerr <<  " x[0] y[1] z[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[0]).at(iy[1]).find(iz[1]) ==
	    fFieldMap.at(ix[0]).at(iy[1]).end()) { 
      if (fVerbosity) std::cerr <<  " x[0] y[1] z[1] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[1]).at(iy[0]).find(iz[0]) ==
	    fFieldMap.at(ix[1]).at(iy[0]).end()) { 
      if (fVerbosity) std::cerr <<  " x[1] y[0] z[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[1]).at(iy[0]).find(iz[1]) ==
	    fFieldMap.at(ix[1]).at(iy[0]).end()) { 
      if (fVerbosity) std::cerr <<  " x[1] y[0] z[1] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[1]).at(iy[1]).find(iz[0]) ==
	    fFieldMap.at(ix[1]).at(iy[1]).end()) { 
      if (fVerbosity) std::cerr <<  " x[1] y[1] z[0] out " << std::endl; 
      skip = true; 
    }
    else if(fFieldMap.at(ix[1]).at(iy[1]).find(iz[1]) ==
	    fFieldMap.at(ix[1]).at(iy[1]).end()) { 
      if (fVerbosity) std::cerr <<  " x[1] y[1] z[1] out " << std::endl; 
      skip = true; 
    }
    
    if(skip) return;
    /*
    if (fVerbosity) {
      std::cerr << " Indices along X " << ix[0] << ", " << ix[1] 
		<< " .. along Y " << iy[0] << ", " << iy[1] << " .. along Z " 
		<< iz[0] << ", " << iz[1] << std::endl;
      std::cerr << " ....  by at ix, iy iz low " 
		<< fFieldMap.at(ix[0]).at(iy[0]).at(iz[0]).at(1) << " high " 
		<< fFieldMap.at(ix[1]).at(iy[1]).at(iz[1]).at(1) << std::endl;
      std::cerr << " And quit after the first succesful pick up " << std::endl; 
      exit(2);
    }	    
    */

    if(fInterpolateOption == 0) { 
      double sumx = 0;
      double sumy = 0;
      double sumz = 0;
      double norm = 0;
      for(int i = 0; i < 2; i++){
	for(int j = 0; j < 2; j++){
	  for(int k = 0; k < 2; k++){
	    double dist = sqrt((indX-ix[i])*(indX-ix[i]) + (indY-iy[j])*(indY-iy[j]) + (indZ-iz[k])*(indZ-iz[k]));
	    sumx += fFieldMap.at(ix[i]).at(iy[j]).at(iz[k]).at(0)*dist;
	    sumy += fFieldMap.at(ix[i]).at(iy[j]).at(iz[k]).at(1)*dist;
	    sumz += fFieldMap.at(ix[i]).at(iy[j]).at(iz[k]).at(2)*dist;
	    norm += dist;
	    if (fVerbosity) 
	      std::cerr << " ........ i j k " << i << " " << j << " " << k 
			<< " dist " << dist << " by " 
			<< fFieldMap.at(ix[i]).at(iy[j]).at(iz[k]).at(1) 
			<< " sumy " << sumy << " norm " << norm << std::endl;
	  }
	}		
      }	
      B[0] = (sumx/norm);
      B[1] = (sumy/norm);
      B[2] = (sumz/norm);
      if (fVerbosity) std::cerr << " ... By .. " << B[1] << std::endl;
    } else { // Linear interpolation on the 3D grid P.L. March 2022 
      const double t = indX-ix[0];
      const double u = indY-iy[0];
      const double v = indZ-iz[0];
      for (size_t kc=0; kc != 3; kc++) {
	B[kc] =   (1.0-t)*(1.0-u)*(1.0-v)*fFieldMap.at(ix[0]).at(iy[0]).at(iz[0]).at(kc);
	B[kc] +=  t*(1.0-u)*(1.0-v)*fFieldMap.at(ix[1]).at(iy[0]).at(iz[0]).at(kc);
	B[kc] +=  (1.0-t)*u*(1.0-v)*fFieldMap.at(ix[0]).at(iy[1]).at(iz[0]).at(kc);
	B[kc] +=  t*u*(1.0-v)*fFieldMap.at(ix[1]).at(iy[1]).at(iz[0]).at(kc);
	B[kc] +=  (1.0-t)*(1.0-u)*v*fFieldMap.at(ix[0]).at(iy[0]).at(iz[1]).at(kc);
	B[kc] +=  t*(1.0-u)*v*fFieldMap.at(ix[1]).at(iy[0]).at(iz[1]).at(kc);
	B[kc] +=  (1.0-t)*u*v*fFieldMap.at(ix[0]).at(iy[1]).at(iz[1]).at(kc);
	B[kc] +=  t*u*v*fFieldMap.at(ix[1]).at(iy[1]).at(iz[1]).at(kc);
      }
    }
  } // map storage. 
      
  //----------------------------------------------------------------------
  // Assume the magnetic field map is in units of kilogauss
  void MagneticField::GetFieldValue(const double x[3], double* B) 
  {
    double xAligned[3];
    for (size_t k=0; k != 3; k++) xAligned[k] = x[k] + fG4ZipTrackOffset[k];
    double BInKg[3];
    const double rR = std::sqrt(x[0]*x[0] + x[1]*x[1]);
    this->Field(xAligned, B);
  }

  //----------------------------------------------------------------------
   
  /* 
     iOpt = 0 => simple Euler formula, iOpt = 1 => 4rth order Runge Kutta
     Start and end must be dimension to 6, usual phase space,  x,y,z, px, py, pz 
     The integration ends at a fixed Z, i.e. end[2] .  The stepAlongZ is the initial step size, anlong the z axis. 
     Algorithm: simple Runge-Kutta, 4rth order.  Suggest step size: ~ 20 mm for the February 2022 version of the field map. 
     distance units are mm (as in Geant4, by default.) and momentum are in GeV/c  (as in Geant4, by default.) 
     Curling around is not supported, 
  */
  
  void MagneticField::Integrate(int iOpt, int charge, double stepAlongZ,
					std::vector<double> &start, std::vector<double> &end) {
    
    if ((start.size() != 6) || (end.size() != 6)) {
      std::cerr << " MagneticField::Integrate , wrong arguments, vectors must be dimensioned to 6. Fatal, quit here and now " << std::endl;
      exit(2);
    }
    const double QCst = charge * 1.0e3 / 0.03; // meter to mm, factor 10 to convert kG to Tesla. The factor 3 in denomintar is standard MKS units. 
    const bool doEuler = (iOpt == 0) || (iOpt == 10) ||  (iOpt == 100);
    const bool doOnlyBy = (iOpt/10 == 1);
    const double ZAccuracy = 1.0; // one mm, guess should be good enough. 
    double stepZ = stepAlongZ;
    std::vector<double> pos(start); // x,y ,z coordinate along the integration path. 
    if (fVerbosity) {
      std::cerr << " MagneticField::Integrate, starting step along z " << stepAlongZ; 
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
      if (fVerbosity) std::cerr << " At x,y,z " << pos[0] << " " << pos[1] << " " << pos[2] << " step size " << stepZ << std::endl;
      this->Field(xxMiddle, bAtZMiddle);
      //
      // Change of slope along the X-axis (dominant component).
      //
      if (std::abs(bAtZMiddle[1]) > 0.5e-3) { 
	if (doEuler) { 
	  const double radXMiddle = QCst *  p * slz / bAtZMiddle[1]; // in Tesla, radius in mm 
	  const double dSlx = stepZ/radXMiddle; // first order, Euler method.
	  slx +=  dSlx;
	  if (fVerbosity) std::cerr << " ...............  delta Slx " << dSlx << " new slx " << slx << std::endl;
	} else { 
	  this->Field(xxStart, bAtZStart);
	  const double radXStart = QCst *  p * slz / bAtZStart[1]; // in Tesla, radius in mm 
	  const double dSlxK1 = stepZ/radXStart; // first order, Euler method.
	  double xxPos1[3]; for(size_t k=0; k !=3; k++) xxPos1[k] = xxMiddle[k];
	  xxPos1[0] += dSlxK1*stepZ/2.;
	  this->Field(xxPos1, bAtFirst);
	  const double radXFirst = QCst *  p * slz /bAtFirst[1]; 
	  const double dSlxK2 = 0.5*stepZ/radXFirst;
	  double xxPos2[3]; for(size_t k=0; k !=3; k++) xxPos2[k] = xxMiddle[k];
	  xxPos2[0] += dSlxK2*stepZ/2.;
	  this->Field(xxPos2, bAtSecond);
	  const double radXSecond = QCst *  p * slz /bAtSecond[1]; 
	  const double dSlxK3 = 0.5*stepZ/radXSecond;
	  xxStop[0] += dSlxK3*stepZ;
	  this->Field(xxStop, bAtZStop);
	  const double radXLast = QCst *  p * slz /bAtZStop[1]; 
	  const double dSlxK4 = stepZ/radXLast;
	  const double dSlxRK = dSlxK1/6. + dSlxK2/3. + dSlxK3/3. + dSlxK4/6.;
	  slx += 1.5*dSlxRK; // if uniform field, all dSlx are equal, expect the same answer as Euler method. 
	  if (fVerbosity) std::cerr << " delta slx s " << dSlxK1 << ", " <<  dSlxK2 << ", " << dSlxK3 << ", " << dSlxK4 << ", "
				   << " sum delta Slx " << 1.5*dSlxRK << " new slx " << slx << std::endl;
	} // Euler vs Runge-Kutta 4rth order. 
      } // finite component of By   
      if ((!doOnlyBy) &&  std::abs(bAtZMiddle[0]) > 0.5e-3) { 
	const double radYMiddle = -1.0 * QCst *  p * slz / bAtZMiddle[0]; // in Tesla, radius in mm 
	const double dSly = stepZ/radYMiddle; // first order, Euler method.
	if (doEuler) { 
	  sly +=  dSly;
	  if (fVerbosity) std::cerr << " ...............  delta Sly " << dSly << " new sly " << sly << std::endl;
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
	  if (fVerbosity) std::cerr << " delta sly s " << dSlyK1 << ", " <<  dSlyK2 << ", " << dSlyK3 << ", " << dSlyK4 << ", "
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
    if (fVerbosity) std::cerr << " .. Final slopes, x, y, " << slx << ", " << sly <<  " and position " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
    for (size_t k=0; k != 6; k++) end[k] = pos[k]; 
  } 

  /*
  void MagneticField::NoteOnDoubleFromASCIIFromCOMSOL() const {
  
    std::cerr << " MagneticField::NoteOnDoubleFromASCIIFromCOMSOL, Consider the following line " << std::endl;
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
  */

} // end namespace emph

