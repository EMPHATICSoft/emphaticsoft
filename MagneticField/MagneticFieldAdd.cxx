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
namespace emph {

  void EMPHATICMagneticField::studyZipTrackData2() { // March 28 2023:  Received from Leo, who received from Mike Tartaglia. 

//    std::string fNameZipIn("/home/lebrun/EMPHATIC/Documents/MagnetMarch2023/EMPHATIC001_SenisBodyScan_230320.txt");
    
    std::string fNameZipIn("/emph/data/users/lebrun/MagFieldMarch302023/EMPHATIC001_SenisBodyScan_230320.txt"); 
    
    this->uploadFromOneCSVSensisFile(fNameZipIn);
    double zFieldMax = 0.;
    double byMax = -1000.;
    for (std::vector<bFieldZipTrackPoint>::const_iterator itZ = ffieldZipTrack.cbegin();  itZ != ffieldZipTrack.cend(); itZ++) { 
      if ((std::abs(itZ->x) < 0.1) && (std::abs(itZ->y) < 0.1) && (std::abs(static_cast<double>(itZ->fby)) > byMax)) {
        byMax = static_cast<double>(std::abs(itZ->fby)); zFieldMax = static_cast<double>(itZ->z); 
      }
    }
    std::cerr << " EMPHATICMagneticField::studyZipTrackData2, number of ZipTrack Pts " << ffieldZipTrack.size() 
              << " By max " << byMax << " at Z = " << zFieldMax << std::endl;
    this->SetFieldOn();	      
    std::pair<double, double> byMaxCOMSOL = this->getMaxByAtCenter();	      
    std::cerr << " ... and from COMSOL, max at z = " << byMaxCOMSOL.first << " |by| " << byMaxCOMSOL.second << std::endl;
    // 
    // Assume we are the real probe is centered, both X and Y Look at the Z profile..
    //
    const double zShift = zFieldMax - byMaxCOMSOL.first; // At Center.. 
    std::ofstream fOutAll("./StuSensisComsolAll_V1.txt");
    fOutAll << " x y z BxC ByC BzC BxS ByS BzS " << std::endl;
    double ComSolPos[3]; double BComsol[3]; 
    for (std::vector<bFieldZipTrackPoint>::const_iterator itZ = ffieldZipTrack.cbegin();  itZ != ffieldZipTrack.cend(); itZ++) { 
      ComSolPos[0] = itZ->x; ComSolPos[1] = itZ->y; ComSolPos[2] = itZ->z - zShift;
      this->MagneticField( ComSolPos, BComsol); 
      fOutAll << " " << itZ->x << " " << itZ->y << " " << itZ->z << " " << BComsol[0] << " " << BComsol[1] << " " << BComsol[2];
      fOutAll << " " << itZ->fbx << " " << itZ->fby << " " << itZ->fbz << std::endl;
    }
    fOutAll.close();
    // Divergence test.. On Sensis data.. similar to 
    
    std::string fName1 = std::string("./EMPHATIC001_SenisBodyScan_230320_DivTest_2p5_1.txt"); 
    std ::ofstream fOutDiv(fName1.c_str());
    fOutDiv << " x y z BCx BCy BCz dBxDxC dByDyC dBzDzC divBC  BSx BSy BSz dBxDxS dByDyS dBzDzS divBS " << std::endl;
    double ComSolPosC[3]; double BComSolC[3]; 
    double ComSolPosN[3]; double BComSolN[3]; 
    
    for (int iX = -5; iX != 10; iX +=5) {
       const double xC = static_cast<double>(iX);
       const double xU = static_cast<double>(iX) + 5.0;
       ComSolPosC[0] = xC; ComSolPosN[0] = xU; 
       for (int iY = -5; iY != 10; iY +=5) {
         const double yC = static_cast<double>(iY);
         const double yU = static_cast<double>(iY) + 5;
         ComSolPosC[1] = yC; ComSolPosN[1] = yU; 
         for (std::vector<bFieldZipTrackPoint>::const_iterator itZ = ffieldZipTrack.cbegin();  itZ != ffieldZipTrack.cend(); itZ++) { 
 	   if (std::abs(xC - itZ->x) > 1.5) continue;
	   if (std::abs(yC - itZ->y) > 1.5) continue;
	   if ( (itZ->z < -25.) || (itZ->z > 150.)) continue;
	   // Rely on the fact that the data is ordered.. 
	   // search for zip taken at x= xL, y = yUp;
	   // Divergence X left.... 
	   std::vector<bFieldZipTrackPoint>::const_iterator itZN = ffieldZipTrack.cend(); // the selected point. 
           for (std::vector<bFieldZipTrackPoint>::const_iterator itZ2 = ffieldZipTrack.cbegin();  itZ2 != ffieldZipTrack.cend(); itZ2++) { 
	     if (itZ == itZ2) continue; // same point.. 
	     if (std::abs(itZ2->x - itZ->x - 5.0)  > 1.5) continue; // |delta| along  is 5 mm, left. 
	     if (std::abs(itZ2->y - itZ->y - 5.0)  > 1.5) continue; // |delta| along Y is 5 mm  2nd Point is up 
	     if (std::abs(itZ2->z - itZ->z - 5.0)  > 1.5) continue; //  Z 2n point is forward, 5 mm 
	     itZN = itZ2; 
	     break; 
	   }
	   if (itZN == ffieldZipTrack.cend()) {
	     std::cerr << " Div B test, X left, is failing, could not find matching point at X = " 
	               << itZ->x << " Y " << itZ->y << " Z " << itZ->z << " fatal.. for now.. " << std::endl; exit(2);  
	   }  
           const double zC = itZ->z; 
	   const double zUp = itZN->z;
	   // Refine based on the COMSOL map. 5 mm is too big... 
	   ComSolPosC[0] = xC; ComSolPosC[1] = yC; ComSolPosC[2] = zC - zShift;
	   ComSolPosN[0] = xC + 2.5; ComSolPosN[1] = yC + 2.5; ComSolPosN[2] = zC + 2.5 - zShift;
	   this->MagneticField(ComSolPosC, BComSolC);
	   this->MagneticField(ComSolPosN, BComSolN);  // Signs do matter a lot!!!... 
	   if ((iX == -5) && (iY == -5)) std::cerr << " Check... zC " << zC << " zUp " << zUp << " Bz s " << BComSolC[2] << " " << BComSolN[2] << std::endl;
	   double dBxdxComsol = (BComSolN[0] - BComSolC[0])/2.5;
	   double dBydyComsol = (BComSolN[1] - BComSolC[1])/2.5;
	   double dBzdzComsol = (BComSolN[2] - BComSolC[2])/2.5;
	   // Bz signal seems to be wrong ???? We would not notice... 
	   dBzdzComsol *= -1;
	   double divBC =  dBxdxComsol + dBydyComsol + dBzdzComsol;
	     
	   double dBxdxSensis = (itZN->fbx - itZ->fbx)/5.;  // x is Left
	   double dBzdzSensis = (itZN->fbz - itZ->fbz)/(zUp - zC);
	   double dBydySensis = (itZN->fby - itZ->fby)/5.;
	   double divBSensis =  dBxdxSensis + dBydySensis + dBzdzSensis;
	   fOutDiv << " " << xC << " " << yC << " " << zC << " " 
	            << BComSolC[0] << " " << BComSolC[1] << " " << BComSolC[2] << " " 
		    << dBxdxComsol << " " << dBydyComsol << " " << dBzdzComsol << " " << divBC << " "   
		    << itZ->fbx << " " << itZ->fby << " " << itZ->fbz << " " 
		    << dBxdxSensis << " " << dBydySensis << " " << dBzdzSensis << " " << divBSensis << std::endl;
        } // on Z 
      } // on iY     
    } // on iX 
    fOutDiv.close();
    std::cerr << " And quit for now... " << std::endl; exit(2);
    
  } 
  void EMPHATICMagneticField::uploadFromOneCSVSensisFile(const G4String &fName) {
  
    double numbers[8];
    ffieldZipTrack.clear(); // may be we want add all of them, to rethink.. for now, one at a time. 
    std::ifstream fileIn(fName.c_str());
    if (!fileIn.is_open()) {
      std::cerr << " EMPHATICMagneticField::uploadFromOneCSVSensisFile, file " << fName << " can not be open, fatal .." << std::endl; 
      exit(2);
    }
    std::string line;
    char aLinecStr[1024];
    int numLines = 0;
    const double oneInch = 25.4;
    // first pass, check the grid is uniform. 
    // Units are assumed to be in inches 
    while (fileIn.good()) {
      fileIn.getline(aLinecStr, 1024);
      std::string aLine(aLinecStr);
      if (aLine.length() < 2) continue; // end of file suspected, or blank in the file 
      if (numLines == 0) {
	  if (aLine.find("k X Y Zs Zmagnet Bx By Bz") == std::string::npos) { // Comment line from Mike T. dAQ.
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
	aPt.t = numbers[0];  aPt.x = oneInch*numbers[1]; aPt.y = oneInch*numbers[2]; aPt.z = oneInch*numbers[4];
	aPt.fbx = 10.0*numbers[5]; aPt.fby = 10.0*numbers[6]; aPt.fbz = 10.0*numbers[7]; // now in kGauss
        ffieldZipTrack.push_back(aPt);
      } 
      numLines++;
    }
    fileIn.close();
  }
  void EMPHATICMagneticField::writeBinary(const std::string &fName) const {
    std::ofstream fileOut(fName.c_str(), std::ios::out | std::ios::binary); 
    if ((!fileOut.is_open()) || (!fileOut.good())) {
         std::cerr << "EMPHATICMagneticField::writeBinary, file with name " << std::string(fName) << " can not be written, bail out " << std::endl;
	 exit(2);
    }
    int numCells[3]; numCells[0] = fNStepX; numCells[1] = fNStepY; numCells[2] = fNStepZ;
    fileOut.write(reinterpret_cast<char*> (&numCells[0]), 3*sizeof(int));
    double bounds[9]; bounds[0] = fXMin; bounds[1] = fYMin; bounds[2] =  fZMin; 
    bounds[3] =  fXMax; bounds[4] =  fYMax;  bounds[5] = fZMax;
    bounds[6] = fStepX; bounds[7] = fStepY; bounds[8] = fStepZ; 
    fileOut.write(reinterpret_cast<char*>(&bounds[0]), 9*sizeof(double));
    std::vector<double>  dd(3*static_cast<size_t>(fNStepZ), 0.); 
    std::cerr << " EMPHATICMagneticField::writeBinary... fNStepX " << fNStepX << " Y " << fNStepY << std::endl;
     for (size_t kx = 0; kx !=  static_cast<size_t>(fNStepX); kx++) { 
      for (size_t ky = 0; ky != static_cast<size_t>(fNStepY); ky++) {
       size_t kk=0; 
        for(size_t kz = 0; kz != static_cast<size_t>(fNStepZ); kz++, kk+=3) { 
	   size_t iv = indexForVector(kx, ky, kz);
	   dd[kk] = ffield[iv].fbx; dd[kk+1] = ffield[iv].fby; dd[kk+2] = ffield[iv].fbz;
	}
	fileOut.write(reinterpret_cast<char*>(&dd[0]), dd.size()*sizeof(double));
      }
    } 
    fileOut.close();
  }
  void EMPHATICMagneticField::readBinary(const std::string &fName) {
    std::ifstream fileIn(fName.c_str(), std::ios::in | std::ios::binary); 
    if ((!fileIn.is_open()) || (!fileIn.good())) {
         std::cerr << "EMPHATICMagneticField::readBinary, file with name " << std::string(fName) << " can not be read, bail out " << std::endl;
	 exit(2);
    }
    int numCells[3];
    fileIn.read(reinterpret_cast<char*> (&numCells[0]), 3*sizeof(int));
    fNStepX =  numCells[0];  fNStepY = numCells[1]; fNStepZ = numCells[2];
    double bounds[9]; 
    fileIn.read(reinterpret_cast<char*>(&bounds[0]), 9*sizeof(double));
    fXMin = bounds[0]; fYMin = bounds[1]; fZMin = bounds[2]; 
    fXMax = bounds[3]; fYMax = bounds[4]; fZMax = bounds[5];
    fStepX = bounds[6]; fStepY = bounds[7]; fStepZ = bounds[8]; 
    bFieldPoint aBV; aBV.fbx = nan("FCOMSOL"); aBV.fby = nan("FCOMSOL"); aBV.fbz = nan("FCOMSOL");
    size_t nTot = static_cast<size_t>(fNStepX) * static_cast<size_t>(fNStepY) * static_cast<size_t>(fNStepZ); 
    for (size_t i=0; i != nTot; i++) ffield.push_back(aBV);
    std::vector<double>  dd( 3*static_cast<size_t>(fNStepZ), 0.); 
    for (size_t kx = 0; kx !=  static_cast<size_t>(fNStepX); kx++) { 
      for (size_t ky = 0; ky != static_cast<size_t>(fNStepY); ky++) {
	fileIn.read(reinterpret_cast<char*>(&dd[0]), dd.size()*sizeof(double));
        size_t kk=0; 
        for(size_t kz = 0; kz !=  static_cast<size_t>(fNStepZ); kz++, kk+=3) { 
	   size_t iv = indexForVector(kx, ky, kz);
	   ffield[iv].fbx = dd[kk]; ffield[iv].fby = dd[kk+1];  ffield[iv].fbz = dd[kk+2];
	}
      }
    } 
    fileIn.close();
  }
  
  
      
} // emph namespace
