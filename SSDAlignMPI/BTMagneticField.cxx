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

#include "BTMagneticField.h"

#include <cmath>
//#define ddebug
//#define debug
//

//Constructor and destructor:

namespace emph {

  emph::rbal::BTMagneticField::BTMagneticField() :
    fStorageIsStlVector(true), fHasBeenAligned(false), fUseOnlyCentralPart(false), fInnerBoreRadius(23.5), // This is the Phase1b Japanese Magnet. 
    step(0), start{-16., -16., -20.},// for the root test file, units are cm Not sure about start values.
    fG4ZipTrackOffset{ 0., 0., 400.},  // Will be overwritten based on the G4/Art based geometry. 
    fNStepX(1), fNStepY(1), fNStepZ(1),
    fXMin(6.0e23),  fYMin(6.0e23), fZMin(6.0e23), fXMax(-6.0e23), fYMax(-6.0e23), fZMax(-6.0e23),
    fStepX(0.), fStepY(0.), fStepZ(0.),
    fInterpolateOption(0), fVerbosity(0), fReAlignShift(3, 0.)
    {
#ifdef debug
      fVerbosity = 1;
#else
      fVerbosity = 0;
#endif
    
     std::string fileName("mfMapBinary.dat");
     std::string myHostName(std::getenv("HOSTNAME"));
      
     std::string topDirAll =(myHostName.find("fnal") != std::string::npos) ? 
            std::string("/work1/next/lebrun/EMPHATIC/Data/") : std::string("/home/lebrun/EMPHATIC/DataLaptop/"); 
	     // On fnal Wilson or on my desk1 
     std::string fullName = topDirAll + fileName;
      //    this->NoteOnDoubleFromASCIIFromCOMSOL(); 
      //    std::cerr <<  " BTMagneticField::BTMagneticField...  And quit for now... " << std::endl; exit(2);
     this->readBinaryAndBroadcast(fullName);
     this->G4GeomAlignIt();

      // These tests do something, comment out for sake of saving time for production use.     
     this->SetFieldOn();	// Set also in the G4EMPH, and the reconstruction.. Please check..       
// 
    }
    
  void emph::rbal::BTMagneticField::G4GeomAlignIt() {
  
//    fG4ZipTrackOffset[2] = -theEMPhGeometry->MagnetUSZPos() + 82.5; // rough guess! 
    fG4ZipTrackOffset[2] =  -595.2; // Extracted from running complete g4gen_job*   
    std::cerr << " BTMagneticField::G4GeomAlignIt G4ZipTrack Z Offset set to " << fG4ZipTrackOffset[2] << std::endl;
    for (size_t k=0; k !=3; k++) fG4ZipTrackOffset[k] += fReAlignShift[k]; 
    fHasBeenAligned = true; 
//
// Testing... at COMSOL coordinate of z = -82.5 mm, By ~ 7.5 Kg, 1/2  field 
//
    if (fVerbosity == 0) return; 
    std::vector<double> xTest(3, 0.); std::vector<double>  xTest2(3,0.);
    std::vector<double> BTest(3, 0.); std::vector<double> BTest2(3, 0.); 
    xTest[0] = 0.; xTest[1] = 0.; xTest[2] = -82.5;  xTest2[0] = 0.01; xTest2[1] = 0.004; xTest2[2] = -52.5; // in mm 
//    for (size_t k=0; k != 3; k++) xAligned[k] = x[k] + fG4ZipTrackOffset[k]; // The equation in the GetFieldValue. 
    this->MagneticField(xTest, BTest);
    std::cerr << " BTMagneticField::G4GeomAlignIt, BField at Upstream plate, internal Variables  " 
              << BTest[1] <<  " kG "  << std::endl;
    this->MagneticField(xTest2, BTest2);
    std::cerr << " .......... again, 20mm inward " << BTest2[1] << std::endl;
    for (size_t k=0; k != 3; k++) xTest[k] -= fG4ZipTrackOffset[k];
    BTest[1] = 0.;
    this->GetFieldValue(&xTest[0], &BTest[0]);
    std::cerr << " BTMagneticField::G4GeomAlignIt, BField at Upstream plate, G4 Coordinates   " << BTest[1] <<  " kG " << std::endl;
   
  }
  
  // Member functions
  
  void emph::rbal::BTMagneticField::MagneticField(const std::vector<double> &x, std::vector<double> &B) const 
  {
//    bool debugIsOn = ((std::abs(x[0] + 2.06) < 0.01) && (std::abs(x[1] - 6.42) < 0.01) && (std::abs(x[2] + 141.918) < 100.));
    bool debugIsOn = false;
    B[0] = 0.5e-4; // a bit of a waste of CPU, but it makes the code a bit cleaner 
    B[1] = 0.5e-4;
    B[2] = 0.5e-4;  // Who know what the residual field is at MT6.. Does not matter... 
    if (fFieldIsOff) return;
    if (debugIsOn) std::cerr << " BTMagneticField::MagneticField, at x,y,z " << x[0] << ", " << x[1] << ", " << x[2] << std::endl; 
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
	          << ") mm,    (Bx, By, Bz) = (" << B[0] << ", " << B[1] << ", " << B[2] << ") kG" << std::endl;
     }
    
  }
    
  void emph::rbal::BTMagneticField::GetFieldValue(const double x[3], double* B) const
  {
    if (!fHasBeenAligned) {
      std::cerr << " BTMagneticField::GetFieldValue, " 
                <<  " Magnet has not been properly aligned, ZipTrack Coordinate system is not the G4 Coordinate system " << std::endl; 
    
      std::cerr << " Fatal error, for now... Quit here and now " << std::endl; exit(2);
    } 
    std::vector<double> xAligned(3, 0.);
    for (size_t k=0; k != 3; k++) xAligned[k] = x[k] + fG4ZipTrackOffset[k];
    std::vector<double> BInKg(3, 0.);
    const double rR = std::sqrt(x[0]*x[0] + x[1]*x[1]);
//    if ((!fUseOnlyCentralPart) || (rR < fInnerBoreRadius)) {
      BTMagneticField::MagneticField(xAligned, BInKg);
//    } else {
//      BTMagneticField::MagneticFieldFromCentralBore(xAligned, BInKg);
//    }
    for (size_t k=0; k != 3; k++) B[k] = BInKg[k]*1.0e-4; // from Tesla to CLHEP::kilogauss;
  }
    
  void emph::rbal::BTMagneticField::Integrate(int iOpt, int charge, double stepAlongZ,
                            std::vector<double> &start, std::vector<double> &end) const {
    
    if ((start.size() != 6) || (end.size() != 6)) {
      std::cerr << " BTMagneticField::Integrate , wrong arguments, vectors must be dimensioned to 6. Fatal, quit here and now " << std::endl;
      exit(2);
    }
//    const double QCst = charge * 1.0e3 / 0.03; // meter to mm, factor 10 to convert kG to Tesla. The factor 3 in denomintar is standard MKS units. 
    const double QCst = charge / 0.3; // Set for Reconstruction....  The factor 3 in denomintar is standard MKS units. 
    const bool doEuler = (iOpt == 0) || (iOpt == 10) ||  (iOpt == 100);
    const bool doOnlyBy = (iOpt/10 == 1);
    const double ZAccuracy = 1.0; // one mm, guess should be good enough. 
    double stepZ = stepAlongZ;
    std::vector<double> pos(start); // x,y ,z coordinate along the integration path. 
    bool debugIsOn = false;
    if (debugIsOn) {
       std::cerr << " BTMagneticField::Integrate, starting step along z " << stepAlongZ; 
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
//      this->MagneticField(xxMiddle, bAtZMiddle);
      this->GetFieldValue(xxMiddle, bAtZMiddle); // For reconstruction..Same coordinate system as in G4.  We think..  
      if (debugIsOn) std::cerr << " At x,y,z " << pos[0] << " " << pos[1] << " " << pos[2] 
                               << " step size " << stepZ << " By  " << bAtZMiddle[1] << std::endl;
      //
      // Change of slope along the X-axis (dominant component).
      //
      if (std::abs(bAtZMiddle[1]) > 0.5e-6) { 
	if (doEuler) { 
          const double radXMiddle = QCst *  p * slz / bAtZMiddle[1]; // in Tesla, radius in mm 
          const double dSlx = stepZ/radXMiddle; // first order, Euler method.
	  slx +=  dSlx;
	  if (debugIsOn) std::cerr << " ...............  delta Slx " << dSlx << " new slx " << slx << std::endl;
	} else { 
         this->GetFieldValue(xxStart, bAtZStart);
         const double radXStart = QCst *  p * slz / bAtZStart[1]; // in Tesla, radius in mm 
         const double dSlxK1 = stepZ/radXStart; // first order, Euler method.
	 double xxPos1[3]; for(size_t k=0; k !=3; k++) xxPos1[k] = xxMiddle[k];
	 xxPos1[0] += dSlxK1*stepZ/2.;
         this->GetFieldValue(xxPos1, bAtFirst);
         const double radXFirst = QCst *  p * slz /bAtFirst[1]; 
	 const double dSlxK2 = 0.5*stepZ/radXFirst;
	 double xxPos2[3]; for(size_t k=0; k !=3; k++) xxPos2[k] = xxMiddle[k];
	 xxPos2[0] += dSlxK2*stepZ/2.;
         this->GetFieldValue(xxPos2, bAtSecond);
         const double radXSecond = QCst *  p * slz /bAtSecond[1]; 
	 const double dSlxK3 = 0.5*stepZ/radXSecond;
	 xxStop[0] += dSlxK3*stepZ;
         this->GetFieldValue(xxStop, bAtZStop);
         const double radXLast = QCst *  p * slz /bAtZStop[1]; 
	 const double dSlxK4 = stepZ/radXLast;
	 const double dSlxRK = dSlxK1/6. + dSlxK2/3. + dSlxK3/3. + dSlxK4/6.;
	 slx += 1.5*dSlxRK; // if uniform field, all dSlx are equal, expect the same answer as Euler method. 
	 if (debugIsOn) std::cerr << " delta slx s " << dSlxK1 << ", " <<  dSlxK2 << ", " << dSlxK3 << ", " << dSlxK4 << ", "
	                          << " sum delta Slx " << 1.5*dSlxRK << " new slx " << slx << std::endl;
       } // Euler vs Runge-Kutta 4rth order. 
      } // finite component of By   
      if ((!doOnlyBy) &&  std::abs(bAtZMiddle[0]) > 0.5e-6) { 
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
      if (debugIsOn) std::cerr << " .. Final slopes, x, y, " << slx << ", " << sly 
                               <<  " and position " << pos[0] << ", " << pos[1] << ", " << pos[2] << " and momentum " << pos[5] << std::endl;
     for (size_t k=0; k != 6; k++) end[k] = pos[k]; 
  } 
      
  std::pair<double, double> emph::rbal::BTMagneticField::getMaxByAtCenter()  {
  
      std::vector<double> xN(3, 0.);  std::vector<double> B0N(3, 0.);
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
  
  void emph::rbal::BTMagneticField::readBinaryAndBroadcast(const std::string &fName) { 
  
     std::ifstream fileIn;
     int world_size;
     MPI_Comm_size(MPI_COMM_WORLD, &world_size);
     int myRank;
     MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     int numCells[3];
     int aTag = 78230;
     if (myRank == 0) {       
       fileIn.open(fName.c_str(), std::ios::in | std::ios::binary); 
       if ((!fileIn.is_open()) || (!fileIn.good())) {
         std::cerr << "BTMagneticField::readBinaryAndBroadcast, file with name " << std::string(fName) << " can not be read, bail out " << std::endl;
	 exit(2);
      }
      fileIn.read(reinterpret_cast<char*> (&numCells[0]), 3*sizeof(int));
      std::cerr << " Read the number of cells, for Z " << numCells[2] << std::endl;
      if (world_size > 1) { 
        for (int kSlave=1; kSlave != world_size; kSlave++) {
             MPI_Send((void*) &numCells[0], 3, MPI_INT, kSlave, aTag, MPI_COMM_WORLD );
	}
      }
    } else {
        MPI_Status aStatus;
        MPI_Recv((void*) &numCells[0], 3, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &aStatus);
    }
    aTag = 78231;
    fNStepX =  numCells[0];  fNStepY = numCells[1]; fNStepZ = numCells[2];
    if (myRank == 1)  std::cerr << " Rank 1, Got the number of cells, for X " << fNStepX <<  " Y " << fNStepY << " Z " << fNStepZ << std::endl;
    double bounds[9];
    if (myRank == 0) {       
      fileIn.read(reinterpret_cast<char*>(&bounds[0]), 9*sizeof(double));
      if (world_size > 1) { 
        for (int kSlave=1; kSlave != world_size; kSlave++) {
             MPI_Send((void*) &bounds[0], 9, MPI_DOUBLE, kSlave, aTag, MPI_COMM_WORLD );
	}
      }
      std::cerr << " Read the number of boundaries, fZMax " << bounds[5] << std::endl;
      
    } else {
        MPI_Status aStatus;
        MPI_Recv((void*) &bounds[0], 9, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &aStatus);
    }
    aTag = 78232;
    fXMin = bounds[0]; fYMin = bounds[1]; fZMin = bounds[2]; 
    fXMax = bounds[3]; fYMax = bounds[4]; fZMax = bounds[5];
    fStepX = bounds[6]; fStepY = bounds[7]; fStepZ = bounds[8]; 
    bFieldPoint aBV; aBV.fbx = nan("FCOMSOL"); aBV.fby = nan("FCOMSOL"); aBV.fbz = nan("FCOMSOL");
    size_t nTot = static_cast<size_t>(fNStepX) * static_cast<size_t>(fNStepY) * static_cast<size_t>(fNStepZ); 
    for (size_t i=0; i != nTot; i++) ffield.push_back(aBV);
    std::vector<double>  dd( 3*static_cast<size_t>(fNStepZ), 0.); 
    for (size_t kx = 0; kx !=  static_cast<size_t>(fNStepX); kx++) { 
      for (size_t ky = 0; ky != static_cast<size_t>(fNStepY); ky++) {
        if (myRank == 0) {       
	  fileIn.read(reinterpret_cast<char*>(&dd[0]), dd.size()*sizeof(double));
          if (world_size > 1) { 
            for (int kSlave=1; kSlave != world_size; kSlave++) {
               MPI_Send((void*) &dd[0], 3*fNStepZ, MPI_DOUBLE, kSlave, aTag, MPI_COMM_WORLD );
	    }
	  }
//          std::cerr << " Read the number field data, kx " << kx << " ky " << ky << " " << dd.size() << " doubles " << std::endl;
        } else { 
          MPI_Status aStatus;
          MPI_Recv((void*) &dd[0], 3*fNStepZ, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &aStatus);
	}
        size_t kk=0; 
        for(size_t kz = 0; kz !=  static_cast<size_t>(fNStepZ); kz++, kk+=3) { 
	   size_t iv = indexForVector(kx, ky, kz);
	   ffield[iv].fbx = dd[kk]; ffield[iv].fby = dd[kk+1];  ffield[iv].fbz = dd[kk+2];
	}
      }
    } 
    if (myRank == 0) fileIn.close();
 
  
  
  }
} // end namespace emph
