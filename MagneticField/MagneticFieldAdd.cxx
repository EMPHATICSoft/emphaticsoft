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
#include "Geometry/service/GeometryService.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "TFile.h"
#include "TTree.h"
#include "math.h"
//#define ddebug
//#define debug
//
namespace emph {
//
/*
** Obsolete, but I'd like to keep the code, if need be 
  void MagneticField::studyZipTrackData2() { // March 28 2023:  Received from Leo, who received from Mike Tartaglia. 

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
    std::cerr << " MagneticField::studyZipTrackData2, number of ZipTrack Pts " << ffieldZipTrack.size() 
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
  void MagneticField::uploadFromOneCSVSensisFile(const G4String &fName) {
  
    double numbers[8];
    ffieldZipTrack.clear(); // may be we want add all of them, to rethink.. for now, one at a time. 
    std::ifstream fileIn(fName.c_str());
    if (!fileIn.is_open()) {
      std::cerr << " MagneticField::uploadFromOneCSVSensisFile, file " << fName << " can not be open, fatal .." << std::endl; 
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
  void MagneticField::writeBinary(const std::string &fName) const {
    std::ofstream fileOut(fName.c_str(), std::ios::out | std::ios::binary); 
    if ((!fileOut.is_open()) || (!fileOut.good())) {
         std::cerr << "MagneticField::writeBinary, file with name " << std::string(fName) << " can not be written, bail out " << std::endl;
	 exit(2);
    }
    int numCells[3]; numCells[0] = fNStepX; numCells[1] = fNStepY; numCells[2] = fNStepZ;
    fileOut.write(reinterpret_cast<char*> (&numCells[0]), 3*sizeof(int));
    double bounds[9]; bounds[0] = fXMin; bounds[1] = fYMin; bounds[2] =  fZMin; 
    bounds[3] =  fXMax; bounds[4] =  fYMax;  bounds[5] = fZMax;
    bounds[6] = fStepX; bounds[7] = fStepY; bounds[8] = fStepZ; 
    fileOut.write(reinterpret_cast<char*>(&bounds[0]), 9*sizeof(double));
    std::vector<double>  dd(3*static_cast<size_t>(fNStepZ), 0.); 
    std::cerr << " MagneticField::writeBinary... fNStepX " << fNStepX << " Y " << fNStepY << std::endl;
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
  void MagneticField::readBinary(const std::string &fName) {
    std::ifstream fileIn(fName.c_str(), std::ios::in | std::ios::binary); 
    if ((!fileIn.is_open()) || (!fileIn.good())) {
         std::cerr << "MagneticField::readBinary, file with name " << std::string(fName) << " can not be read, bail out " << std::endl;
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
  
*/  
   void emph::MagneticField::SetIntegratorSteps(double minStep) {
     bool debugIsOn = true;
  //
  // Nominal Geometry, Phase1c.. 
  //
     art::ServiceHandle<emph::geo::GeometryService> geomService;
     const emph::geo::Geometry* geo = geomService->Geo();
     for (int kSt= 3; kSt != 7; kSt++) { 
       const emph::geo::SSDStation *aSt = geo->GetSSDStation(kSt);
       const TVector3 aStPos = aSt->Pos();
       for (int kPlSe=0; kPlSe != aSt->NPlanes(); kPlSe++) {
	  const emph::geo::Plane *aPlane = aSt->GetPlane(kPlSe);
	  for (int kSe=0; kSe != aPlane->NSSDs(); kSe++) { 
	    const emph::geo::Detector *aSensor = aPlane->SSD(kSe); 
	    const TVector3 aSePos = aSensor->Pos();
            if (aSensor->View() == emph::geo::X_VIEW) { 
	       if (kSt == 3) fZXViewSt3 = aSePos.Z() + aStPos.Z();
	       if (kSt == 4) fZXViewSt4 = aSePos.Z() + aStPos.Z();
	       if (kSt == 5) fZXViewSt5 = aSePos.Z() + aStPos.Z();
	       if (kSt == 6) fZXViewSt6 = aSePos.Z() + aStPos.Z();
	    } 
          }
	}
      }
  //
  // Look at the first Z Coordinate where we have a non-zero field, at least 1 Gauss
  //
     fZStartField = fZXViewSt3;
     double xP[3], bf[3]; xP[0] = 0.; xP[1] = 0.; 
     while (fZStartField < fZXViewSt4) {
       xP[2] = fZStartField;
       this->GetFieldValue(xP, bf);
       if (std::abs(bf[1]) > 1.0e-6) break;
       fZStartField += 1.0; // assume ~ 2 mm grid.. 
     }
     fdZStartField = fZStartField - fZXViewSt3;
     std::vector<double> zzzBack;
     std::vector<double> zzz;
     std::ofstream fOutTmp;
     if (debugIsOn) {
       fOutTmp.open("./MagFieldIntegratorStepsV1.txt");
       std::cerr << "  BTMagneticField::SetIntegratorSteps, input steps size " << minStep << " start at St3, Z = " 
                              <<  fZXViewSt3 << " no field to " <<  fZStartField << std::endl;
			      
	fOutTmp << " z step " << std::endl;
     }		      
     fStepsIntSt3toSt4.clear(); std::vector<double> backfSteps; zzzBack.push_back(fZXViewSt4);
     backfSteps.push_back(2.0 * minStep); // a bit arbtrary.. This will be the minimum step in this region, St3 to St4. 
     double zz = fZXViewSt4 - backfSteps[0]; 
     double kFact = 1.05; // again arbitrary.. To be tuned.. Geometric progression of sort.. 
     size_t nn=1;
     size_t kkkOut=0;
     if (debugIsOn) std::cerr << " ... ... At first back step, step size " <<  backfSteps[0] << " zz " << zz << std::endl;
    while (zz > fZXViewSt3) {
        backfSteps.push_back(kFact*backfSteps[nn-1]);
        zzzBack.push_back(zz);        
	zz -= backfSteps[nn];
        if (debugIsOn) std::cerr << " ... ... At back step " << nn << " step size " <<  backfSteps[nn] << " zz " << zz << std::endl;
        nn++;
     }
     // last one 
     std::vector<double>::reverse_iterator ritL = backfSteps.rbegin(); *ritL += zz - fZXViewSt3; // first forwad step is a bit shorter.. Oh well.. 
     std::vector<double>::reverse_iterator ritzzz = zzzBack.rbegin();
     zzz.push_back(fZXViewSt3);
     for (std::vector<double>::reverse_iterator rit = ritL; rit != backfSteps.rend(); rit++, ritzzz++) {
        fStepsIntSt3toSt4.push_back(*rit); zzz.push_back(*ritzzz);
     }
     if (debugIsOn)  {
         std::cerr << "  ... Table of steps..Usptream of the magnet.  " << std::endl;
	 int kk = 0;
	 for (std::vector<double>::const_iterator it = fStepsIntSt3toSt4.cbegin(); it != fStepsIntSt3toSt4.cend(); it++, kk++ ) {
	    fOutTmp << " " << zzz[kkkOut] << " " <<  *it << std::endl; kkkOut++;
	    std::cerr << ", " << *it; 
	    if (((kk % 10) == 0) && (kk > 0)) std::cerr << std::endl;
	 }
	 std::cerr << std::endl;
	 // Check.. 
	 double sumSteps = 0; 
	 for (size_t k=0; k != fStepsIntSt3toSt4.size(); k++) {
	   sumSteps += fStepsIntSt3toSt4[k];
	 }
	 std::cerr << " Check, sum of step size upstream of Magnet " << sumSteps <<   " against Z3 to z4  dist " << (fZXViewSt4 - fZXViewSt3) << std::endl;
	 std::cerr << " End of setting step size upstream of Magnet, numSteps " << fStepsIntSt3toSt4.size() << std::endl;
     }
//
// Between Station 4 and 5, mostly inside the magnet.
//     
     fStepsIntSt4toSt5.clear(); backfSteps.clear(); zzzBack.clear();
// 
// first, close to the upstream face .
//
     if (debugIsOn) std::cerr << " ... ... Inside Magnet, from z  " << fZXViewSt4 << " to " << fZXViewSt5 
                              << " dist " << (fZXViewSt5 - fZXViewSt4) << std::endl;
     backfSteps.push_back(1.25 * minStep); // a bit arbtrary.. This will be the minimum step in this region, St3 to St4.
     zzzBack.push_back(903.);
     zz = 903. - backfSteps[0];
     double kFactIns = 1.05; // again arbitrary.. To be tuned.. Geometric progression of sort.. 
     nn=1;
     while (zz > fZXViewSt4) {
        const double stepCurrent = kFactIns*backfSteps[nn-1];
        backfSteps.push_back(stepCurrent);
	zzzBack.push_back(zz);
	zz -= backfSteps[nn];
        if (debugIsOn) std::cerr << " ... ... At back step " << nn << " step size " <<  backfSteps[nn] << " zz " 
	                           << zz << std::endl;
        nn++;
     }
     ritL = backfSteps.rbegin();
     ritzzz = zzzBack.rbegin();
     double sumStepZInside1 = 0.;
     for (std::vector<double>::reverse_iterator rit = ritL; rit != backfSteps.rend(); rit++, ritzzz++)  {
       fStepsIntSt4toSt5.push_back(*rit); zzz.push_back(*ritzzz); sumStepZInside1 += *rit; 
     }
     zz = fZXViewSt4 + sumStepZInside1;
     if (debugIsOn) std::cerr << "  .....  Check sumStepZInside1, final sumStep " << sumStepZInside1 <<  " zz = " << zz << std::endl;
     int numStepInsidePhase2 = static_cast<int>(1050. - zz)/minStep;
     for (int k2=0; k2 != numStepInsidePhase2; k2++) fStepsIntSt4toSt5.push_back(minStep); 
     zz += numStepInsidePhase2*minStep;
     if (debugIsOn) std::cerr << " end of flat step size region, zz " << zz << " sumStepZInside2 " << minStep*numStepInsidePhase2 << std::endl;
     nn = fStepsIntSt4toSt5.size();
     const double zzLastStep2 = zz;
//       
     double sumStepZInside3 = 0.;
     while (true) {  // Downstream fringe field, increase the step size.. 
       const double stepCurrent = kFactIns*fStepsIntSt4toSt5[nn-1]; 
       zz += stepCurrent;
       if (debugIsOn) std::cerr << " ... ... At forward step... " 
                                << " step size " << kFactIns*fStepsIntSt4toSt5[nn-1]  << " zz " << zz <<  " sumStepZInside3 " << sumStepZInside3 << std::endl;       
       if (zz > fZXViewSt5) break; //if overshooting, break; 
       fStepsIntSt4toSt5.push_back(stepCurrent);
       sumStepZInside3 += stepCurrent;
       if (debugIsOn) std::cerr << " ... ... keep going, at zz ... " 
                                 << zz <<  " sumStepZInside3 " << sumStepZInside3 << std::endl;       
       nn++;
     }
     // last one, correction for overshooting.. 
      if (debugIsOn) std::cerr << " ..... Near Station 5 .. sumStepZInside3 " << sumStepZInside3 << " zz " << zz << std::endl;
     const double lastStepToStation5 =  fZXViewSt5 - zzLastStep2 -  sumStepZInside3;
     fStepsIntSt4toSt5.push_back(lastStepToStation5); ritL = fStepsIntSt4toSt5.rbegin();
      if (debugIsOn) std::cerr << " ..... Last step up to Station 5 .. " << *ritL << std::endl;
     if (*ritL < 0.)  { 
        std::cerr << " .... BTMagneticField::SetIntegratorSteps, Inside magnet, macro step 3, last  step " << *ritL << 
	      " logic error quit now " << std::endl; exit(2);
     }
     if (debugIsOn)  {
         std::cerr << "  ... End inside Magnet.. Final zz " << zz << " ... sumStepZInside3 " 
	           << sumStepZInside3 << " table of steps..Inside the magnet  " << std::endl;
	 int kk = 0;
	 for (std::vector<double>::const_iterator it = fStepsIntSt4toSt5.cbegin(); it != fStepsIntSt4toSt5.cend(); it++, kk++ ) {
	    fOutTmp << " " << zzz[kkkOut] << " "  << *it << std::endl; kkkOut++;
	    std::cerr << ", " << *it; 
	    if (((kk % 10) == 0) && (kk > 0)) std::cerr << std::endl;
	 }
	 std::cerr << std::endl;
	 // Check.. dump to ASCIII
	 double sumSteps = 0; for (size_t k=0; k != fStepsIntSt4toSt5.size(); k++) sumSteps += fStepsIntSt4toSt5[k]; 
	 std::cerr << " Check, sum of step size in the Magnet " << sumSteps <<   " against Z4 to z5  dist " << (fZXViewSt5 - fZXViewSt4) << std::endl;
	 if (std::abs(sumSteps - (fZXViewSt5 - fZXViewSt4)) > 0.1 ) {
	    std::cerr << "  ...check on integrated stepping faled.. quit now for good " << std::endl; exit(2);
	 }
	 std::cerr << " End of setting step size inside the Magnet, numSteps " << fStepsIntSt4toSt5.size() << std::endl;
     }
     
     // Station 5 to 6... First, find the last meaning value of the field..      
     fZEndField = fZXViewSt5;
     zz = fZXViewSt5;
     while (fZEndField < fZXViewSt6) {
       xP[2] = fZEndField;
       this->GetFieldValue(xP, bf);
       if (std::abs(bf[1]) < 1.0e-6) break;
       fZEndField += 1.0; // assume ~ 2 mm grid.. 
     }
     fdZEndField = fZXViewSt6 - fZEndField;
     
     if (debugIsOn) std::cerr << "  Station 5 to 6 stepping..Starting from St5 at Z " << fZXViewSt5 
                              << " Field extend up to Z " << fZEndField << std::endl;
     fStepsIntSt5toSt6.clear(); fStepsIntSt5toSt6.push_back(fStepsIntSt4toSt5[fStepsIntSt4toSt5.size()-2]);
      // The very last step has been shorten, to match 
     nn = 1;
     zz = fZXViewSt5;
     double sumStep56 = fStepsIntSt5toSt6[0]; zz += fStepsIntSt5toSt6[0];
     while (zz < fZEndField) {  // Increase the step size.. 
       const double stepCurrent = kFact*fStepsIntSt5toSt6[nn-1];
       fStepsIntSt5toSt6.push_back(stepCurrent); sumStep56 += stepCurrent; 
       zzz.push_back(zz);
       zz += fStepsIntSt5toSt6[nn];
       if (debugIsOn) std::cerr << " ... ... At forward step... " 
                                << " step size " << stepCurrent  << " zz " << zz <<  " sumStep56 " << sumStep56 << std::endl;       
       nn++;
     }
     // last one 
     ritL = fStepsIntSt5toSt6.rbegin(); 
     const double prevSum56 = sumStep56 - (*ritL);		      
     if (debugIsOn) std::cerr << " ... ... after last step of " << (*ritL) <<  " zz " << zz << " fZEndField " 
                              << fZEndField << " sumStep56 " << sumStep56 <<  " prevSum56 " << prevSum56 << std::endl;
     *ritL = (fZEndField - fZXViewSt5) - prevSum56; // first forwad step is a bit shorter.. This should be fine..
     
     if (*ritL < 0.)  { 
        std::cerr << " .... BTMagneticField::SetIntegratorSteps, Downstream magnet, last  step " << *ritL << 
	      " logic error quit now " << std::endl; exit(2);
     }
     std::vector<double>::reverse_iterator ritZZZ = zzz.rbegin(); *ritZZZ = fZEndField;
     if (debugIsOn)  {
         // Correct the last Z coordinate.. 
         std::cerr << "  ... table of steps.Downstream of the magnet  " << std::endl;
	 int kk = 0;
	 double sumSteps = 0; 
	 for (std::vector<double>::const_iterator it = fStepsIntSt5toSt6.cbegin(); it != fStepsIntSt5toSt6.cend(); it++, kk++ ) {
	    sumSteps += *it; 
	    fOutTmp << " " << zzz[kkkOut] << " "  << *it << std::endl; kkkOut++;
	    std::cerr << ", " << *it; 
	    if (((kk % 10) == 0) && (kk > 0)) std::cerr << std::endl;
	 }
	 std::cerr << std::endl;
	 // Check.. 
	 std::cerr << " Check, sum of step size downstream of Magnet " << sumSteps <<   " against Z5 to EndField dist " << (fZEndField - fZXViewSt5) << std::endl;
	 std::cerr << " End of setting step size inside the Magnet, numSteps " << fStepsIntSt5toSt6.size() << std::endl;
	 fOutTmp.close();
     }
  }
  bool emph::MagneticField::IntegrateSt3toSt4(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn) {
    if ((start.size() != 6) || (end.size() != 6)) {
      std::cerr << " BTMagneticField::IntegrateSt3toSt4 , wrong arguments, vectors must be dimensioned to 6. Fatal, quit here and now " << std::endl;
      exit(2);
    }
    if (fInterpolateOption != 1) {
      std::cerr << " BTMagneticField::IntegrateSt3toSt4 , Poorly tested interpolatio option.. . Fatal, quit here and now " << std::endl;
      exit(2);
    }
    const bool doEuler = (iOpt == 0) || (iOpt == 10) ||  (iOpt == 100);  
    if (debugIsOn) {
       std::cerr << " BTMagneticField::IntegrateSt3toSt4, number of step along z " << fStepsIntSt3toSt4.size(); 
       if (doEuler) std::cerr << " Simple Euler formula "; 
       else  std::cerr << " Runge-Kutta formula ";
       std::cerr << std::endl;
    }   
    // 		    
    // going in straight line, up to the start of the map. 
    // 
    double slx = start[3]/start[5];  double sly = start[4]/start[5]; // approximate cosine director 
    double slz = std::sqrt(1. - slx*slx - sly*sly);
    double xxStart[3]; xxStart[0] = start[0] + slx *fdZStartField;  xxStart[1] = start[1] + sly *fdZStartField; 
    // xxStart is the starting point of the current RK4 step
    const double QCst = charge / 0.3; // Set for Reconstruction....  The factor 3 in denomintar is standard MKS units. 
    // Eventual option to assume delta y' =0 .. 
    double xxMiddle[3], bAtZStart[3], bAtZStop[3], bAtZMiddle[3]; 
    double bAtSecond[3], bAtThird[3], bAtFourth[3]; // temporary storage
    double pSq = 0.; for (size_t k=0; k !=3; k++) pSq =+ start[k+3]*start[k+3]; 
    const double p = std::sqrt(pSq); // Momentum, a conserved quatity. 
    const double QCstp  = QCst *  p; 
    std::vector<double> pos(start); // x,y ,z coordinate along the integration path. 
    if (doEuler) {  
      for (size_t kz=0; kz != fStepsIntSt3toSt4.size(); kz++) { // We will clone these prelimninaries for Rung-Kutta as well,
        // to avoid an if statement for every step.. 
        for (size_t k=0; k !=3; k++) xxMiddle[k] = pos[k];
	const double stepZ =  fStepsIntSt3toSt4[kz];
        xxMiddle[0] += slx*stepZ/2.; xxMiddle[1] += sly*stepZ/2.;
        this->GetFieldValue(xxMiddle, bAtZMiddle); // For reconstruction..Same coordinate system as in G4.  We think..
        if (!fStayedInMap) {
	  if (debugIsOn) std::cerr << " At step " << kz << " Z " << xxMiddle[2] << " In the magnet, but outside the map .. Wrong integral.. " << std::endl;
	  return false; 
        }      
	if (debugIsOn) std::cerr << " At x,y,z " << pos[0] << " " << pos[1] << " " << pos[2] 
                               << " step size " << stepZ << " By  " << bAtZMiddle[1] << std::endl;

        const double radXMiddle = QCstp * slz / bAtZMiddle[1]; // in Tesla, radius in mm 
        const double dSlx = stepZ/radXMiddle; // first order, Euler method.
	slx +=  dSlx;
        const double radYMiddle = -1.0 * QCstp * slz / bAtZMiddle[0]; // in Tesla, radius in mm 
        const double dSly = stepZ/radYMiddle; // first order, Euler method.
	sly +=  dSly;
	if (debugIsOn) std::cerr << " ...............  delta Slx " << 1.0e3*dSlx << " new slx " << 1.0e3*slx 
	                         <<  " dSly " << 1.0e3*dSly << " sly " << 1.0e3*sly << " mrad " << std::endl;
        pos[0] += slx*stepZ;
        pos[1] += sly*stepZ;
        pos[2] += stepZ;
        slz = std::sqrt(1. - slx*slx - sly*sly);
      } // on steps. 
      for (size_t k=0; k != 3; k++) end[k] = pos[k];
      end[3] = p * slx;
      end[4] = p * sly;
      slz = std::sqrt(1. - slx*slx - sly*sly);
      end[5] = p * slz;
    } else {  //  Runge-Kutta 4rth order.. 
      for (size_t k=0; k !=3; k++) xxStart[k] = pos[k]; 
      this->GetFieldValue(xxStart, bAtZStart);
      for (size_t kz=0; kz != fStepsIntSt3toSt4.size(); kz++) { 
	const double stepZ =  fStepsIntSt3toSt4[kz];  
	const double halfStepZ = 0.5*stepZ; 
	// At Position 1 
	if (debugIsOn) std::cerr << " .................. Pos 1 " << xxStart[0] << ", " 
	                         << xxStart[1] << ", " << xxStart[2] << " step size " << stepZ
	                          << " slopes, x, y  " << 1.0e3*slx << " , "  << 1.0e3*sly << " mrad " << std::endl;
        const double radXStart = QCstp * slz / bAtZStart[1]; // in Tesla, radius in mm 
        const double dSlxK1 = halfStepZ/radXStart; 
        const double radYStart = -QCst *  p * slz / bAtZStart[0]; 
        const double dSlyK1 = halfStepZ/radYStart;   // K1 step. 
	double xxPos2[3];
	const double dxK1 = dSlxK1*halfStepZ; const double dyK1 = dSlyK1*halfStepZ; 
	xxPos2[0] = xxStart[0] + dxK1;
	xxPos2[1] = xxStart[1] + dyK1;
	xxPos2[2] = xxStart[2] + halfStepZ;
 	// At position 2 
        this->GetFieldValue(xxPos2, bAtSecond); // first call to the magnetic field
        const double radX2 = QCstp * slz / bAtSecond[1]; // Step2
        const double dSlxK2 = halfStepZ/radX2; 
        const double radY2 = -QCst *  p * slz / bAtSecond[0]; 
        const double dSlyK2 = halfStepZ/radY2; 
	const double dxK2 = dSlxK2*halfStepZ; const double dyK2 = dSlyK2*halfStepZ; 
	if (debugIsOn) 
	  std::cerr << " .................. Pos 2 " << xxPos2[0] << ", " << xxPos2[1] << ", " << xxPos2[2] 
	            << " delta slopes, x, y  " << 1.0e3*dSlxK2 << " , "  << 1.0e3*dSlyK2 << std::endl;
	double xxPos3[3];
	xxPos3[0] = xxStart[0] + dxK2; // Yes, using the starting value for the current step.
	xxPos3[1] = xxStart[1] + dyK2;
	xxPos3[2] = xxStart[2] + halfStepZ;
        this->GetFieldValue(xxPos3, bAtThird); // 2nd call 
        if (!fStayedInMap) {
	  if (debugIsOn) std::cerr << " At step " << kz << " Z " << xxPos3[2] << " In the magnet, but outside the map .. Wrong integral.. " << std::endl;
	  return false; 
        }      
        const double radX3 = QCstp * slz / bAtThird[1]; //
        const double dSlxK3 = dSlxK2 + halfStepZ/radX3; // Full size step, this time 
        const double radY3 = -QCst *  p * slz / bAtThird[0]; 
        const double dSlyK3 = dSlyK2 + halfStepZ/radY3; 
	if (debugIsOn) 
	  std::cerr << " .................. Pos 3 " << xxPos3[0] << ", " << xxPos3[1] << ", " << xxPos3[2] 
	            << " delta slopes, x, y  " << 1.0e3*dSlxK3 <<  " , "  << 1.0e3*dSlyK3 << std::endl;

	double xxPos4[3];
	xxPos4[0] = xxStart[0] + stepZ*dSlxK3; // Yes, using the starting value for the current step.
	xxPos4[1] = xxStart[1] + stepZ*dSlyK3;
	xxPos4[2] = xxStart[2] + stepZ;
        this->GetFieldValue(xxPos4, bAtZStart); // 3rd call 
	const double dSlxKF = (dSlxK1 + dSlxK2 + 2.0*dSlxK3) / 3.;
	const double dSlyKF = (dSlyK1 + dSlyK2 + 2.0*dSlyK3) / 3.;
	if (debugIsOn) {  
	  std::cerr << " .................. Pos 4 " << xxPos4[0] << ", " << xxPos4[1] << ", " << xxPos4[2] << std::endl; 
	  std::cerr << " ..................  Average delta Slopes, X = " << dSlxKF << " Y = " << dSlyKF	<< std::endl;    
	}
	xxStart[0] += stepZ * dSlxKF; slx += dSlxKF; 
	xxStart[1] += stepZ * dSlyKF; sly += dSlyKF; 
	xxStart[2] += stepZ;
	if (debugIsOn) 
	  std::cerr << " .................. Pos 4 " << xxStart[0] << ", " << xxStart[1] << ", " << xxStart[2] << std::endl;
	  
         slz = std::sqrt(1. - slx*slx - sly*sly);
       } // RK4 steps
       end[0] = xxStart[0]; end[1] = xxStart[1]; end[2] = xxStart[2];
       end[3] = p * slx;
       end[4] = p * sly;
       slz = std::sqrt(1. - slx*slx - sly*sly);
       end[5] = p * slz;
    } // Euler vs RK4 
    return true;		    
  }		     
  bool emph::MagneticField::IntegrateSt4toSt5(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn) {
		    
    if ((start.size() != 6) || (end.size() != 6)) {
      std::cerr << " BTMagneticField::IntegrateSt4toSt5 , wrong arguments, vectors must be dimensioned to 6. Fatal, quit here and now " << std::endl;
      exit(2);
    }
    if (fInterpolateOption != 1) {
      std::cerr << " BTMagneticField::IntegrateSt4toSt5 , Poorly tested interpolatio option.. . Fatal, quit here and now " << std::endl;
      exit(2);
    }
    const bool doEuler = (iOpt == 0) || (iOpt == 10) ||  (iOpt == 100); 
    std::ofstream fOutDbg; 
    if (debugIsOn) {
       std::cerr << " BTMagneticField::IntegrateSt4toSt5, number of step along z " << fStepsIntSt4toSt5.size(); 
       if (doEuler) std::cerr << " Simple Euler formula "; 
       else  std::cerr << " Runge-Kutta formula ";
       std::cerr << std::endl;
       double stepZMin = DBL_MAX; 
       for (size_t k=1; k != fStepsIntSt4toSt5.size()-1;k++) {
         if (stepZMin > fStepsIntSt4toSt5[k] ) stepZMin = fStepsIntSt4toSt5[k];
       } 
       std::ostringstream fOutStrStr; 
       if (doEuler) {
         fOutStrStr << "./IntegrateSt4toSt5_Euler_MinStep_" << static_cast<int>(100.0*stepZMin) << "_V1.txt";
	 std::string fOutStr(fOutStrStr.str()); fOutDbg.open(fOutStr.c_str()); 
	 fOutDbg << " x y z sz by dSlx slx dSly sly " << std::endl;  
       } else {
         fOutStrStr << "./IntegrateSt4toSt5_RK4_MinStep_" << static_cast<int>(100.0*stepZMin) << "_V1.txt";
	 std::string fOutStr(fOutStrStr.str()); fOutDbg.open(fOutStr.c_str()); 
	 fOutDbg << " x y z sz by2 by3 by4 dSlx1 dSlx2 dSlx3 slx  dSly1 dSly2 dSly3 sly " << std::endl;  
       }
    }   
    // 		    
    // going in straight line, up to the start of the map. 
    // 
    double slx = start[3]/start[5];  double sly = start[4]/start[5]; // approximate cosine director 
    double slz = std::sqrt(1. - slx*slx - sly*sly);
    double xxStart[3]; xxStart[0] = start[0];  xxStart[1] = start[1]; 
    // xxStart is the starting point of the current RK4 step
    const double QCst = charge / 0.3; // Set for Reconstruction....  The factor 3 in denomintar is standard MKS units. 
    // Eventual option to assume delta y' =0 .. 
    double xxMiddle[3], bAtZStart[3], bAtZStop[3], bAtZMiddle[3]; 
    double bAtSecond[3], bAtThird[3], bAtFourth[3]; // temporary storage
    double pSq = 0.; for (size_t k=0; k !=3; k++) pSq =+ start[k+3]*start[k+3]; 
    const double p = std::sqrt(pSq); // Momentum, a conserved quatity. 
    const double QCstp  = QCst *  p; const double slxInit = slx;
    std::vector<double> pos(start); // x,y ,z coordinate along the integration path. 
    if (doEuler) {  
      for (size_t kz=0; kz != fStepsIntSt4toSt5.size(); kz++) { // We will clone these prelimninaries for Rung-Kutta as well,
        // to avoid an if statement for every step.. 
        for (size_t k=0; k !=3; k++) xxMiddle[k] = pos[k];
	const double stepZ =  fStepsIntSt4toSt5[kz];
        xxMiddle[0] += slx*stepZ/2.; xxMiddle[1] += sly*stepZ/2.;
        this->GetFieldValue(xxMiddle, bAtZMiddle); // For reconstruction..Same coordinate system as in G4.  We think..
        if (!fStayedInMap) {
	  if (debugIsOn) std::cerr << " At step " << kz << " Z " << xxMiddle[2] << " In the magnet, but outside the map .. Wrong integral.. " << std::endl;
	  return false; 
        }      
	if (debugIsOn) std::cerr << " At x,y,z " << pos[0] << " " << pos[1] << " " << pos[2] 
                               << " step size " << stepZ << " By  " << bAtZMiddle[1] << std::endl;

        const double radXMiddle = QCstp * slz / bAtZMiddle[1]; // in Tesla, radius in mm 
        const double dSlx = stepZ/radXMiddle; // first order, Euler method.
	slx +=  dSlx;
        const double radYMiddle = -1.0 * QCstp * slz / bAtZMiddle[0]; // in Tesla, radius in mm 
        const double dSly = stepZ/radYMiddle; // first order, Euler method.
	sly +=  dSly;
	if (debugIsOn) std::cerr << " ...............  delta Slx " << 1.0e3*dSlx << " new slx " << 1.0e3*slx 
	                         <<  " dSly " << 1.0e3*dSly << " sly " << 1.0e3*sly << " mrad " << std::endl;
        pos[0] += slx*stepZ;
        pos[1] += sly*stepZ;
        pos[2] += stepZ;
        slz = std::sqrt(1. - slx*slx - sly*sly);
//	 fOutDbg << " x y z sz by dSlx slx dSly sly " << std::endl;  
	if (debugIsOn) fOutDbg << " " <<  pos[0] << " " << pos[1] << " " << pos[2] << " " << stepZ << " " <<  
	                       1.0e4*bAtZMiddle[1] << " " <<  1.0e3*dSlx << " " << 1.0e3*slx << " " << 1.0e3*dSly << " " << 1.0e3*sly << std::endl;
      } // on steps. 
      for (size_t k=0; k != 3; k++) end[k] = pos[k];
      end[3] = p * slx;
      end[4] = p * sly;
      slz = std::sqrt(1. - slx*slx - sly*sly);
      end[5] = p * slz;
    } else {  //  Runge-Kutta 4rth order.. 
      for (size_t k=0; k !=3; k++) xxStart[k] = pos[k]; 
      this->GetFieldValue(xxStart, bAtZStart);
      for (size_t kz=0; kz != fStepsIntSt4toSt5.size(); kz++) { 
	const double stepZ =  fStepsIntSt4toSt5[kz];  
	const double halfStepZ = 0.5*stepZ; 
	// At Position 1 
	if (debugIsOn) std::cerr << " .................. Pos 1 " << xxStart[0] << ", " 
	                         << xxStart[1] << ", " << xxStart[2] << " step size " << stepZ
	                          << " slopes, x, y  " << 1.0e3*slx << " , "  << 1.0e3*sly << " mrad " << std::endl;
        const double radXStart = QCstp * slz / bAtZStart[1]; // in Tesla, radius in mm 
        const double dSlxK1 = halfStepZ/radXStart; 
        const double radYStart = -QCst *  p * slz / bAtZStart[0]; 
        const double dSlyK1 = halfStepZ/radYStart;   // K1 step. 
	double xxPos2[3];
	const double dxK1 = dSlxK1*halfStepZ; const double dyK1 = dSlyK1*halfStepZ; 
	xxPos2[0] = xxStart[0] + dxK1;
	xxPos2[1] = xxStart[1] + dyK1;
	xxPos2[2] = xxStart[2] + halfStepZ;
 	// At position 2 
        this->GetFieldValue(xxPos2, bAtSecond); // first call to the magnetic field
        const double radX2 = QCstp * slz / bAtSecond[1]; // Step2
        const double dSlxK2 = halfStepZ/radX2; 
        const double radY2 = -QCst *  p * slz / bAtSecond[0]; 
        const double dSlyK2 = halfStepZ/radY2; 
	const double dxK2 = dSlxK2*halfStepZ; const double dyK2 = dSlyK2*halfStepZ; 
	if (debugIsOn) { 
	  std::cerr << " .................. Pos 2 " << xxPos2[0] << ", " << xxPos2[1] << ", " << xxPos2[2] 
	            << " delta slopes, x, y  " << 1.0e3*dSlxK2 << " , "  << 1.0e3*dSlyK2 <<  " By " << 1.0e4*bAtSecond[1] << " kG " << std::endl;
	  std::cerr << " .....................Ckeck dxK1 " << dxK1 << " vs dxK2 " << dxK2 << " dyK1 " << dyK1 << " vs dyK2 " << dyK2 << std::endl; 
	} 
	double xxPos3[3];
	xxPos3[0] = xxStart[0] + dxK2; // Yes, using the starting value for the current step.
	xxPos3[1] = xxStart[1] + dyK2;
	xxPos3[2] = xxStart[2] + halfStepZ;
        this->GetFieldValue(xxPos3, bAtThird); // 2nd call 
        if (!fStayedInMap) {
	  if (debugIsOn) std::cerr << " At step " << kz << " Z " << xxPos3[2] << " In the magnet, but outside the map .. Wrong integral.. " << std::endl;
	  return false; 
        }      
        const double radX3 = QCstp * slz / bAtThird[1]; //
        const double dSlxK3 = dSlxK2 + halfStepZ/radX3; // Full size step, this time 
        const double radY3 = -QCst *  p * slz / bAtThird[0]; 
        const double dSlyK3 = dSlyK2 + halfStepZ/radY3; 
	if (debugIsOn) 
	  std::cerr << " .................. Pos 3 " << xxPos3[0] << ", " << xxPos3[1] << ", " << xxPos3[2] 
	            << " delta slopes, x, y  " << 1.0e3*dSlxK3 <<  " , "  << 1.0e3*dSlyK3 << " By " << 1.0e4*bAtThird[1] << std::endl;

	double xxPos4[3];
	xxPos4[0] = xxStart[0] + stepZ*dSlxK3; // Yes, using the starting value for the current step.
	xxPos4[1] = xxStart[1] + stepZ*dSlyK3;
	xxPos4[2] = xxStart[2] + stepZ;
        this->GetFieldValue(xxPos4, bAtZStart); // 3rd call... Store the result in BAtZStart, in preparation for the next step... 
	const double dSlxKF = (dSlxK1 + dSlxK2 + 2.0*dSlxK3) / 3.;
	const double dSlyKF = (dSlyK1 + dSlyK2 + 2.0*dSlyK3) / 3.;
	if (debugIsOn) {  
	  std::cerr << " .................. Pos 4 " << xxPos4[0] << ", " << xxPos4[1] << ", " << xxPos4[2] << std::endl; 
	  std::cerr << " ..................  Average delta Slopes, X = " << dSlxKF << " Y = " << dSlyKF	<< std::endl;    
	}
	xxStart[0] += stepZ * dSlxKF; slx += dSlxKF; 
	xxStart[1] += stepZ * dSlyKF; sly += dSlyKF; 
	xxStart[2] += stepZ;
	if (debugIsOn) {
	  std::cerr << " .................. Pos 4 " << xxStart[0] << ", " << xxStart[1] << ", " << xxStart[2] << std::endl;
//	  fOutDbg << " x y z sz by2 by3 by4 dSlx1 dSlx2 dSlx3 dSlx4 slx  dSly1 dSly2 dSly3 dSly4 sly " << std::endl;  
 	  fOutDbg << " " <<  xxStart[0] << " " << xxStart[1] << " " << xxStart[2] << " " << stepZ << " " 
	                 << 1.0e4*bAtSecond[1] << " " << 1.0e4*bAtThird[1] << " " << 1.0e4*bAtZStart[1] << " " 
			       <<  1.0e3*dSlxK1 << " " << 1.0e3*dSlxK2 << " " <<  1.0e3*dSlxK3   
			       << " " << 1.0e3*slx << " " <<  1.0e3*dSlyK1 << " " << 1.0e3*dSlyK2 << " " <<  1.0e3*dSlyK3  
			       << " "  << 1.0e3*sly << std::endl;
	 } 
         slz = std::sqrt(1. - slx*slx - sly*sly);
       } // RK4 steps
       end[0] = xxStart[0]; end[1] = xxStart[1]; end[2] = xxStart[2];
       end[3] = p * slx;
       end[4] = p * sly;
       slz = std::sqrt(1. - slx*slx - sly*sly);
       end[5] = p * slz;
    } // Euler vs RK4 
    if (debugIsOn) {
      std::cerr << std::endl << " ------> Integrated deflection in bend plane " << 1.0e3*(slx - slxInit) << " mrad "<< std::endl << std::endl;
      fOutDbg.close();
//      std::cerr << std::endl << " ------> And quit for now!!!  " << std::endl << std::endl; exit(2); 
    }
    return true;		    
  }		    
  bool emph::MagneticField::IntegrateSt5toSt6(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn) {
// Again, disgusting clone.. Except for the propagate at the end of the field region. 		    
    if ((start.size() != 6) || (end.size() != 6)) {
      std::cerr << " BTMagneticField::IntegrateSt5toSt6 , wrong arguments, vectors must be dimensioned to 6. Fatal, quit here and now " << std::endl;
      exit(2);
    }
    if (fInterpolateOption != 1) {
      std::cerr << " BTMagneticField::IntegrateSt5toSt6 , Poorly tested interpolatio option.. . Fatal, quit here and now " << std::endl;
      exit(2);
    }
    const bool doEuler = (iOpt == 0) || (iOpt == 10) ||  (iOpt == 100); 
    std::ofstream fOutDbg; 
    if (debugIsOn) {
       std::cerr << " BTMagneticField::IntegrateSt5toSt6, number of step along z " << fStepsIntSt5toSt6.size(); 
       if (doEuler) std::cerr << " Simple Euler formula "; 
       else  std::cerr << " Runge-Kutta formula ";
       std::cerr << std::endl;
       double stepZMin = DBL_MAX; 
       for (size_t k=1; k != fStepsIntSt5toSt6.size()-1;k++) {
         if (stepZMin > fStepsIntSt5toSt6[k] ) stepZMin = fStepsIntSt5toSt6[k];
       } 
       std::ostringstream fOutStrStr; 
       if (doEuler) {
         fOutStrStr << "./IntegrateSt5toSt6_Euler_MinStep_" << static_cast<int>(100.0*stepZMin) << "_V1.txt";
	 std::string fOutStr(fOutStrStr.str()); fOutDbg.open(fOutStr.c_str()); 
	 fOutDbg << " x y z sz by dSlx slx dSly sly " << std::endl;  
       } else {
         fOutStrStr << "./IntegrateSt5toSt6_RK4_MinStep_" << static_cast<int>(100.0*stepZMin) << "_V1.txt";
	 std::string fOutStr(fOutStrStr.str()); fOutDbg.open(fOutStr.c_str()); 
	 fOutDbg << " x y z sz by2 by3 by4 dSlx1 dSlx2 dSlx3 slx  dSly1 dSly2 dSly3 sly " << std::endl;  
       }
    }   
    // 		    
    // going in straight line, up to the start of the map. 
    // 
    double slx = start[3]/start[5];  double sly = start[4]/start[5]; // approximate cosine director 
    double slz = std::sqrt(1. - slx*slx - sly*sly);
    double xxStart[3]; xxStart[0] = start[0];  xxStart[1] = start[1]; 
    // xxStart is the starting point of the current RK4 step
    const double QCst = charge / 0.3; // Set for Reconstruction....  The factor 3 in denomintar is standard MKS units. 
    // Eventual option to assume delta y' =0 .. 
    double xxMiddle[3], bAtZStart[3], bAtZStop[3], bAtZMiddle[3]; 
    double bAtSecond[3], bAtThird[3], bAtFourth[3]; // temporary storage
    double pSq = 0.; for (size_t k=0; k !=3; k++) pSq =+ start[k+3]*start[k+3]; 
    const double p = std::sqrt(pSq); // Momentum, a conserved quatity. 
    const double QCstp  = QCst *  p; const double slxInit = slx;
    std::vector<double> pos(start); // x,y ,z coordinate along the integration path. 
    if (doEuler) {  
      for (size_t kz=0; kz != fStepsIntSt5toSt6.size(); kz++) { // We will clone these prelimninaries for Rung-Kutta as well,
        // to avoid an if statement for every step.. 
        for (size_t k=0; k !=3; k++) xxMiddle[k] = pos[k];
	const double stepZ =  fStepsIntSt5toSt6[kz];
        xxMiddle[0] += slx*stepZ/2.; xxMiddle[1] += sly*stepZ/2.;
        this->GetFieldValue(xxMiddle, bAtZMiddle); // For reconstruction..Same coordinate system as in G4.  We think..
        if (!fStayedInMap) {
	  if (debugIsOn) std::cerr << " At step " << kz << " Z " << xxMiddle[2] << " In the magnet, but outside the map .. Wrong integral.. " << std::endl;
	  return false; 
        }      
	if (debugIsOn) std::cerr << " At x,y,z " << pos[0] << " " << pos[1] << " " << pos[2] 
                               << " step size " << stepZ << " By  " << bAtZMiddle[1] << std::endl;

        const double radXMiddle = QCstp * slz / bAtZMiddle[1]; // in Tesla, radius in mm 
        const double dSlx = stepZ/radXMiddle; // first order, Euler method.
	slx +=  dSlx;
        const double radYMiddle = -1.0 * QCstp * slz / bAtZMiddle[0]; // in Tesla, radius in mm 
        const double dSly = stepZ/radYMiddle; // first order, Euler method.
	sly +=  dSly;
	if (debugIsOn) std::cerr << " ...............  delta Slx " << 1.0e3*dSlx << " new slx " << 1.0e3*slx 
	                         <<  " dSly " << 1.0e3*dSly << " sly " << 1.0e3*sly << " mrad " << std::endl;
        pos[0] += slx*stepZ;
        pos[1] += sly*stepZ;
        pos[2] += stepZ;
        slz = std::sqrt(1. - slx*slx - sly*sly);
//	 fOutDbg << " x y z sz by dSlx slx dSly sly " << std::endl;  
	if (debugIsOn) fOutDbg << " " <<  pos[0] << " " << pos[1] << " " << pos[2] << " " << stepZ << " " <<  
	                       1.0e4*bAtZMiddle[1] << " " <<  1.0e3*dSlx << " " << 1.0e3*slx << " " << 1.0e3*dSly << " " << 1.0e3*sly << std::endl;
      } // on steps. 
      for (size_t k=0; k != 3; k++) end[k] = pos[k];
      end[3] = p * slx;
      end[4] = p * sly;
      slz = std::sqrt(1. - slx*slx - sly*sly);
      end[5] = p * slz;
    } else {  //  Runge-Kutta 4rth order.. 
      for (size_t k=0; k !=3; k++) xxStart[k] = pos[k]; 
      this->GetFieldValue(xxStart, bAtZStart);
      for (size_t kz=0; kz != fStepsIntSt5toSt6.size(); kz++) { 
	const double stepZ =  fStepsIntSt5toSt6[kz];  
	const double halfStepZ = 0.5*stepZ; 
	// At Position 1 
	if (debugIsOn) std::cerr << " .................. Pos 1 " << xxStart[0] << ", " 
	                         << xxStart[1] << ", " << xxStart[2] << " step size " << stepZ
	                          << " slopes, x, y  " << 1.0e3*slx << " , "  << 1.0e3*sly << " mrad " << std::endl;
        const double radXStart = QCstp * slz / bAtZStart[1]; // in Tesla, radius in mm 
        const double dSlxK1 = halfStepZ/radXStart; 
        const double radYStart = -QCst *  p * slz / bAtZStart[0]; 
        const double dSlyK1 = halfStepZ/radYStart;   // K1 step. 
	double xxPos2[3];
	const double dxK1 = dSlxK1*halfStepZ; const double dyK1 = dSlyK1*halfStepZ; 
	xxPos2[0] = xxStart[0] + dxK1;
	xxPos2[1] = xxStart[1] + dyK1;
	xxPos2[2] = xxStart[2] + halfStepZ;
 	// At position 2 
        this->GetFieldValue(xxPos2, bAtSecond); // first call to the magnetic field
        const double radX2 = QCstp * slz / bAtSecond[1]; // Step2
        const double dSlxK2 = halfStepZ/radX2; 
        const double radY2 = -QCst *  p * slz / bAtSecond[0]; 
        const double dSlyK2 = halfStepZ/radY2; 
	const double dxK2 = dSlxK2*halfStepZ; const double dyK2 = dSlyK2*halfStepZ; 
	if (debugIsOn) { 
	  std::cerr << " .................. Pos 2 " << xxPos2[0] << ", " << xxPos2[1] << ", " << xxPos2[2] 
	            << " delta slopes, x, y  " << 1.0e3*dSlxK2 << " , "  << 1.0e3*dSlyK2 <<  " By " << 1.0e4*bAtSecond[1] << " kG " << std::endl;
	  std::cerr << " .....................Ckeck dxK1 " << dxK1 << " vs dxK2 " << dxK2 << " dyK1 " << dyK1 << " vs dyK2 " << dyK2 << std::endl; 
	} 
	double xxPos3[3];
	xxPos3[0] = xxStart[0] + dxK2; // Yes, using the starting value for the current step.
	xxPos3[1] = xxStart[1] + dyK2;
	xxPos3[2] = xxStart[2] + halfStepZ;
        this->GetFieldValue(xxPos3, bAtThird); // 2nd call 
        if (!fStayedInMap) {
	  if (debugIsOn) std::cerr << " At step " << kz << " Z " << xxPos3[2] << " In the magnet, but outside the map .. Wrong integral.. " << std::endl;
	  return false; 
        }      
        const double radX3 = QCstp * slz / bAtThird[1]; //
        const double dSlxK3 = dSlxK2 + halfStepZ/radX3; // Full size step, this time 
        const double radY3 = -QCst *  p * slz / bAtThird[0]; 
        const double dSlyK3 = dSlyK2 + halfStepZ/radY3; 
	if (debugIsOn) 
	  std::cerr << " .................. Pos 3 " << xxPos3[0] << ", " << xxPos3[1] << ", " << xxPos3[2] 
	            << " delta slopes, x, y  " << 1.0e3*dSlxK3 <<  " , "  << 1.0e3*dSlyK3 << " By " << 1.0e4*bAtThird[1] << std::endl;

	double xxPos4[3];
	xxPos4[0] = xxStart[0] + stepZ*dSlxK3; // Yes, using the starting value for the current step.
	xxPos4[1] = xxStart[1] + stepZ*dSlyK3;
	xxPos4[2] = xxStart[2] + stepZ;
        this->GetFieldValue(xxPos4, bAtZStart); // 3rd call... Store the result in BAtZStart, in preparation for the next step... 
	const double dSlxKF = (dSlxK1 + dSlxK2 + 2.0*dSlxK3) / 3.;
	const double dSlyKF = (dSlyK1 + dSlyK2 + 2.0*dSlyK3) / 3.;
	if (debugIsOn) {  
	  std::cerr << " .................. Pos 4 " << xxPos4[0] << ", " << xxPos4[1] << ", " << xxPos4[2] << std::endl; 
	  std::cerr << " ..................  Average delta Slopes, X = " << dSlxKF << " Y = " << dSlyKF	<< std::endl;    
	}
	xxStart[0] += stepZ * dSlxKF; slx += dSlxKF; 
	xxStart[1] += stepZ * dSlyKF; sly += dSlyKF; 
	xxStart[2] += stepZ;
	if (debugIsOn) {
	  std::cerr << " .................. Pos 4 " << xxStart[0] << ", " << xxStart[1] << ", " << xxStart[2] << std::endl;
//	  fOutDbg << " x y z sz by2 by3 by4 dSlx1 dSlx2 dSlx3 dSlx4 slx  dSly1 dSly2 dSly3 dSly4 sly " << std::endl;  
 	  fOutDbg << " " <<  xxStart[0] << " " << xxStart[1] << " " << xxStart[2] << " " << stepZ << " " 
	                 << 1.0e4*bAtSecond[1] << " " << 1.0e4*bAtThird[1] << " " << 1.0e4*bAtZStart[1] << " " 
			       <<  1.0e3*dSlxK1 << " " << 1.0e3*dSlxK2 << " " <<  1.0e3*dSlxK3   
			       << " " << 1.0e3*slx << " " <<  1.0e3*dSlyK1 << " " << 1.0e3*dSlyK2 << " " <<  1.0e3*dSlyK3  
			       << " "  << 1.0e3*sly << std::endl;
	 } 
         slz = std::sqrt(1. - slx*slx - sly*sly);
       } // RK4 steps
       end[0] = xxStart[0]; end[1] = xxStart[1]; end[2] = xxStart[2];
       end[3] = p * slx;
       end[4] = p * sly;
       slz = std::sqrt(1. - slx*slx - sly*sly);
       end[5] = p * slz;
    } // Euler vs RK4 
    // Not quit done, we need to extrapolate to Station 6.. 
    end[0] += slx *fdZEndField; end[1] += sly *fdZEndField;
    if (debugIsOn) {
      std::cerr << std::endl << " ------> Integrated deflection in bend plane " << 1.0e3*(slx - slxInit) << " mrad "<< std::endl << std::endl;
      fOutDbg.close();
    }
    return true;	     
  } // IntegrateSt5toSt6
  
    
} // emph namespace
