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
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <sstream>

#include "BTMagneticField.h"
#include "BTAlignGeom1c.h"

#include <cmath>
namespace emph {

void  emph::rbal::BTMagneticField::testField1() const {
    
    const double y1 = 1.0;
    double xTest[3], BTest[3]; 
    xTest[0] = 0.; xTest[1] = 0.; xTest[2] = -150.; //-82.5;  
    for (int ix=0; ix != 1; ix++) {
      std::ostringstream fNameStrStr; fNameStrStr << "./fieldTest1_iX_" << ix << "_Y1.txt";
      std::string fName(fNameStrStr.str());
      std::ofstream fOut(fName.c_str()); 
      fOut << " z Bx By Bz " <<std::endl;
      xTest[0] = 1.*ix; xTest[1] = 1.0;
      xTest[0] = 2.00*ix + 0.35; xTest[1] = 0.66;
      for (int iz=-50; iz != 150; iz++) {
        xTest[2] = 987 + 0.5*iz;
        this->GetFieldValue(xTest, BTest);
	fOut << " " << xTest[2] << " " << BTest[0] <<  " " << BTest[1] <<  " " << BTest[2] << std::endl;
      }
      fOut.close();
    }
  }
  
  void emph::rbal::BTMagneticField::StuKick1(int iOpt) const {
  
     std::vector<double> startMag(6, 0.); std::vector<double> endMag(6, 0.); std::vector<double> tmpMag(6, 0.);
     const double pMom = 120.; const double Q = 1.;
     const double slx0 = -5.3e-3;  const double sly0 = 10.2e-3; 
     startMag[0] = 1.95 + slx0*fZXViewSt3; // assume station 0 is the origin. 
     startMag[1] = -5.030668 + sly0*fZXViewSt3; ; // Probably approximate..only 300 microns difference between X and Y views.. if one believes the geometry.   
     startMag[2] = fZXViewSt3; 
     startMag[3] = slx0*pMom; // 
     startMag[4] = sly0*pMom; // assume small slope, sin(theta) = theta.. 
     startMag[5] = pMom * std::sqrt(1.0 - slx0*slx0 - sly0*sly0);
     std::ofstream fOut("./BTMagneticField_StuKick1_V1.txt"); 
     fOut << " x y kickX kickY " << std::endl;
     //
     const double xStCenter = startMag[0]; const double yStCenter = startMag[1];
     for (int iX = -16; iX != 16; iX++) { 
       startMag[0] = 1.0*iX + xStCenter; 
       for (int iY = -16; iY != 16; iY++) {
         startMag[1] = 1.0*iY + yStCenter; 
         if (!IntegrateSt3toSt4(iOpt, Q, startMag, endMag)) { 
           std::cerr << " BTMagneticField::StuKick1, fail to integrate, St3 to St4, iX " << iX << " iY " 
	               << iY << " Should not happenn, exit now " << std::endl; exit(2);
         }
         for (size_t k=0; k != 6; k++) tmpMag[k] = endMag[k]; tmpMag[2] = fZXViewSt4;
         if (!IntegrateSt4toSt5(iOpt, Q, tmpMag, endMag)) { 
            std::cerr << " BTMagneticField::StuKick1, fail to integrate, St4 to St5, iX " << iX << " iY " << iY << std::endl;
	    fOut << " " << startMag[0] << " " << startMag[1] << " 1.0e3 1.0e3 " << std::endl; 
         } else { 
           for (size_t k=0; k != 6; k++) tmpMag[k] = endMag[k]; tmpMag[2] = fZXViewSt5;
           if (!IntegrateSt5toSt6(iOpt, Q, tmpMag, endMag)) { 
             std::cerr << " BTMagneticField::StuKick1, fail to integrate, St5 to St6, exit now " << std::endl; exit(2); 
           }
           const double deltaSlx = (endMag[3] - startMag[3])/pMom; 
           const double deltaSly = (endMag[4] - startMag[4])/pMom; 
           fOut << " " << startMag[0] << " " << startMag[1] << " " << 1.0e3*deltaSlx << " " << 1.0e3*deltaSly << std::endl;
	} // O.K. Pro
       } // on IY
     } // on IX
     fOut.close();
//     std::cerr << " BTMagneticField::StuKick1, deltaSlx = " << 1.0e3*deltaSlx << " y " << 1.0e3*deltaSly << std::endl;
  }
  void emph::rbal::BTMagneticField::SetIntegratorSteps(double minStep) {
      bool debugIsOn = true;
  //
  // Nominal Geometry, Phase1c.. 
  //
      BTAlignGeom1c *myGeo1c = emph::rbal::BTAlignGeom1c::getInstance();
      fZXViewSt3 = myGeo1c->ZPos('X', 3); fZXViewSt4 = myGeo1c->ZPos('X', 4);
      fZXViewSt5 = myGeo1c->ZPos('X', 5); fZXViewSt6 = myGeo1c->ZPos('X', 7); // no typo, Station5  is a double sensor.     
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
     double sumStepZInside1 = 0.;
     while (zz > fZXViewSt4) {
        const double stepCurrent = kFactIns*backfSteps[nn-1];
        backfSteps.push_back(stepCurrent); sumStepZInside1 += stepCurrent;
	zzzBack.push_back(zz);
	zz -= backfSteps[nn];
        if (debugIsOn) std::cerr << " ... ... At back step " << nn << " step size " <<  backfSteps[nn] << " zz " 
	                           << zz << " sumStepZInside1 " << sumStepZInside1 << std::endl;
        nn++;
     }
     ritL = backfSteps.rbegin();
     ritzzz = zzzBack.rbegin(); sumStepZInside1 = 0.;
     for (std::vector<double>::reverse_iterator rit = ritL; rit != backfSteps.rend(); rit++, ritzzz++)  {
       fStepsIntSt4toSt5.push_back(*rit); zzz.push_back(*ritzzz); sumStepZInside1 += *rit; 
     }
     zz = zzz[zzz.size()-1];
     if (debugIsOn) std::cerr << "  .....  Ccheck sumStepZInside1, final " << sumStepZInside1 <<  " zz = " << zz << std::endl;
     double sumStepZInside2 = 0.;
     while (zz < 1050) {  // nearly flat field, constant step size.. 
        fStepsIntSt4toSt5.push_back(minStep); sumStepZInside2 += minStep; 
	zzz.push_back(zz);
	zz += minStep;
        nn++;
     }
     if (debugIsOn) std::cerr << " end of flat step size region, zz " << zz << " sumStepZInside2 " << sumStepZInside2 << std::endl;
//       
     double sumStepZInside3 = 0.;
     while (zz < fZXViewSt5) {  // Downstream fringe field, increase the step size.. 
       const double stepCurrent = kFactIns*fStepsIntSt4toSt5[nn-1]; 
       fStepsIntSt4toSt5.push_back(stepCurrent); sumStepZInside3 += stepCurrent;
       zzz.push_back(zz);
       if (debugIsOn) std::cerr << " ... ... At forward step... " 
                                << " step size " << kFactIns*fStepsIntSt4toSt5[nn-1]  << " zz " << zz <<  " sumStepZInside3 " << sumStepZInside3 << std::endl;       
       zz += fStepsIntSt4toSt5[nn];
       if (zz > fZXViewSt5) break;
       nn++;
     }
     // last one, correction for overshooting..  
     ritL = fStepsIntSt4toSt5.rbegin(); 
     *ritL += fZXViewSt5 - fZXViewSt4 - (sumStepZInside1 + sumStepZInside2 + sumStepZInside3); // first forwad step is a bit shorter.. Oh well.. 
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
  bool emph::rbal::BTMagneticField::IntegrateSt3toSt4(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn) const {
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
  bool emph::rbal::BTMagneticField::IntegrateSt4toSt5(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn) const {
		    
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
  bool emph::rbal::BTMagneticField::IntegrateSt5toSt6(int iOpt, int charge,  
                    std::vector<double> &start, std::vector<double> &end, bool debugIsOn) const {
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
  
  
  
} // namespace 
