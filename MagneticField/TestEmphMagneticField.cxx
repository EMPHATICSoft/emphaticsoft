///////////////////////////////////////////////////////////////////////////////
// File: TestEmphMagneticField.cxx
// Description: A set of test of the magnetic field. 
/// Note it compiles, but may not be running as one would like. No formal Boost-like Test units 
// Could be invoked from the art service  
///////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <cstdlib>
#include "MagneticField/TestEmphMagneticField.h"
#include "Geant4/Randomize.hh"

namespace emph { 
  TestEmphMagneticField::TestEmphMagneticField(EMPHATICMagneticField* theField) : fBField(theField) {
     ;
  }
  //
  void TestEmphMagneticField::test1() {
    
    // Look at the divergence of the field.. 
    //
    std::cerr << " TestEmphMagneticField::test1, generating simple CSV test files.. " << std::endl;
    std::string fName1("./EmphMagField_Test1_v3.txt"); 
    std ::ofstream fOutForR(fName1.c_str());
    fOutForR << " x y z B0x B0y B0z B0 divB0 B1x B1y B1z B1 divB1" << std::endl;
    
    double xN[3], xF[3], B0N[3], B0F[3], B1N[3], B1F[3];
    
    for (int iX = -10; iX != 10; iX++) { 
       const double x = 7.5 + iX*0.956; 
       xN[0] = x; // in mm, apparently... 
       xF[0] = x + 10.;
       for (int iY = -10; iY != 20; iY++) { 
        const double y = 15.34 + iY*0.892; 
         xN[1] = y ;
         xF[1] = y + 10.;
         for (int iZ = -80; iZ != 120; iZ++) { 
           const double z = iZ*4.578; 
           xN[2] = z;
           xF[2] = z + 10.;
           fBField->setInterpolatingOption(0);
           fBField->MagneticField(xN, B0N); // xN is in mm.. 
           fBField->MagneticField(xF, B0F);
	   double divB0 = 0.; double b0Norm = 0.;
	   for (size_t kk=0; kk != 2; kk++) { 
	     divB0 += (B0F[kk] - B0N[kk])/10.; // kG/mm 
	     b0Norm += B0N[kk]*B0N[kk];
	   }
	   fOutForR << " " << x << " " << y << " " << z << " " 
	            << B0N[0] << " " << B0N[1] << " " << B0N[2] << " " 
		    << std::sqrt(b0Norm) << " " << divB0;
           fBField->setInterpolatingOption(1);
           fBField->MagneticField(xN, B1N); // xN is in mm.. 
           fBField->MagneticField(xF, B1F);
	   double divB1 = 0.; double b1Norm = 0.;
	   for (size_t kk=0; kk != 2; kk++) { 
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
    std::string fName2("./EmphMagField_Test1_v3b.txt");
    fOutForR.open(fName2.c_str());
    fOutForR << " x y z B0x B0y B0z B0 divB0 B1x B1y B1z B1 divB1" << std::endl;
    
    for (int iX = -150; iX != 150; iX++) { 
       const double x = 7.5 + iX*0.956; 
       xN[0] = x; // in mm, apparently... 
       xF[0] = x + 10.;
       for (int iY = -10; iY != 20; iY++) { 
        const double y = 15.34 + iY*0.892; 
         xN[1] = y ;
         xF[1] = y + 10.;
         for (int iZ = -5; iZ != 5; iZ++) { 
           const double z = 0. + iZ*4.578; 
           xN[2] = z;
           xF[2] = z + 10.;
           fBField->setInterpolatingOption(0);
           fBField->MagneticField(xN, B0N); // xN is in mm.. 
           fBField->MagneticField(xF, B0F);
	   double divB0 = 0.; double b0Norm = 0.;
	   for (size_t kk=0; kk != 2; kk++) { 
	     divB0 += (B0F[kk] - B0N[kk])/10.; // kG/mm 
	     b0Norm += B0N[kk]*B0N[kk];
	   }
	   fOutForR << " " << x << " " << y << " " << z << " " 
	            << B0N[0] << " " << B0N[1] << " " << B0N[2] << " " 
		    << std::sqrt(b0Norm) << " " << divB0;
           fBField->setInterpolatingOption(1);
           fBField->MagneticField(xN, B1N); // xN is in mm.. 
           fBField->MagneticField(xF, B1F);
	   double divB1 = 0.; double b1Norm = 0.;
	   for (size_t kk=0; kk != 2; kk++) { 
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
    fBField->setInterpolatingOption(1);
    for (int izzz =0; izzz < 10; izzz++) {
      std::cerr << " izzzzz.. " << izzz << std::endl; 
      for (int k=0; k != 100000000; k++) {
//                       123456789
         xN[0] = -180. + 360.0*static_cast<double>(rand())/RAND_MAX; 
         xN[1] = -180. + 360.0*static_cast<double>(rand())/RAND_MAX; 
         xN[2] = -400. + 400.0*static_cast<double>(rand())/RAND_MAX; 
         fBField->MagneticField(xN, B1N); // xN is in mm.. 
         if (( k < 5) || ((k % 5000000) == 1) ) 
            std::cerr << " k " << k << " x = " << xN[0] << " y " << xN[1] << " z " << xN[2] << " By " << B1N[1] << std::endl;
      }
    }
  */  
   
  }
  
  void TestEmphMagneticField::test2() {
    
//      std::cerr << " TestEmphMagneticField::test2, and quit now !... " << std::endl; exit(2);
      std::vector<double> start(6); 
      std::vector<double> stop(6);
      std::cerr << " TestEmphMagneticField::test2 setting interpolation 3D radial " << std::endl;
      fBField->setInterpolatingOption(0); 
      double p = 10.0;
      start[0] = 4.0; start[1] = 11.0; start[2] = - 15.0; stop[2] = 450.; 
      double DeltaZ = stop[2] - start[2];
      double slx =0.1e-3; double sly = -0.2e-3; double slz = std::sqrt( 1.0 - slx*slx - sly*sly); 
      start[3] = p*slx; start[4] = p*sly;  start[5] = slz*p;
    
      fBField->Integrate(10, 1, 15.0, start, stop);
      double slxFinal = stop[3]/p;      double slyFinal = stop[4]/p;
      std::cerr << " .. Euler, Bx = Bz = 0., Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl; 
      stop[2] = 450.; 	  
      fBField->Integrate(0, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " ..Euler, Bz = 0.,  Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      stop[2] = 450.; 	  
      fBField->Integrate(1, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " ..RungeKutta, Bz = 0.,  Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   

      fBField->setInterpolatingOption(1); 
      std::cerr << " TestEmphMagneticField::test2 setting interpolation 3D grid, linear " << std::endl;
      fBField->Integrate(10, 1, 15.0, start, stop);
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " .. Euler, Bx = Bz = 0., Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      stop[2] = 450.;  
      fBField->Integrate(0, 1, 15.0, start, stop); 
      slxFinal = stop[3]/p;   slyFinal = stop[4]/p;
      std::cerr << " .... Euler, Bz = 0. Position x,y, " << stop[0] << ",  " 
                << stop[1] << " Deflection in X " << (slxFinal - slx)*DeltaZ << " mm in Y "  << (slyFinal - sly)*DeltaZ << std::endl;   
      stop[2] = 450.; 	  
      fBField->Integrate(1, 1, 15.0, start, stop); 
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
	    fBField->setInterpolatingOption(0); 
            fBField->Integrate(0, 1, stepZ, start, stop); 
	    const double dSlx3DR_E = stop[3]/p - slx;
	    stop[2] = 450.;
            fBField->Integrate(1, 1, stepZ, start, stop); 
	    const double dSlx3DR_RK = stop[3]/p - slx;
	    stop[2] = 450.;
	    fBField->setInterpolatingOption(1); 
            fBField->Integrate(0, 1, stepZ, start, stop); 
	    const double dSlxLin_E = stop[3]/p - slx;
	    stop[2] = 450.;
            fBField->Integrate(1, 1, stepZ, start, stop); 
	    const double dSlxLin_RK = stop[3]/p - slx;
	    stop[2] = 450.;
	    fOutForR << " " << p << " " << stepZ << " " << start[1] << " " 
	             << dSlx3DR_E << " " << dSlxLin_E << " " << dSlx3DR_RK << " " << dSlxLin_RK << std::endl;
	  }
	}
      }
      fOutForR.close();
    }  
  void TestEmphMagneticField::test3() {
    //
    // Assuming a beam spot size of 1 cm, Gaussian, 
    //
    std::vector<double> start(6); 
    std::vector<double> stop(6);
    fBField->setInterpolatingOption(0); 
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
      fBField->Integrate(0, 1, 2., start, stop);
      const double xF = stop[0];
      const double slxFinal = stop[3]/p;
      const double yF = stop[1];
      stop[0] = -1.e10; stop[1] = -1.e10;      
      start[3] = p*slx2Off; start[4] = p*sly;
      start[2] = -200.;
      stop[2] = 450.;
      fBField->Integrate(0, 1, 2., start, stop);
      const double xFS2 = stop[0];
      const double yFS2 = stop[1];
      const double slxFinalS2 = stop[3]/p;
      stop[0] = -1.e10; stop[1] = -1.e10; 
      start[0] =  xNoOffset - 5.0;  
      start[3] = p*slxNoOff; start[4] = p*sly;
      start[2] = -200.;
      stop[2] = 450.;
      fBField->Integrate(0, 1, 2., start, stop);
      const double xFMagP5 = stop[0];
      const double yFMagP5 = stop[1];
      const double slxFinalMagP5 = stop[3]/p;
      fOutForR << " " << kEvt << " " << start[0] << " " << start[1] << " " <<  1000.0*slxNoOff << " " << 1000.0*slx2Off 
               << " " << xF << " " << yF << " " << xFS2 << " " << yFS2 << " " << 1000.0*slxFinal << " " << 1000.0*slxFinalS2
	       << " " << xFMagP5 << " " << yFMagP5 << " " << 1000.0*slxFinalMagP5 << std::endl;
    }
    fOutForR.close();
  }  
  
  
} // TestEmphMagnetic Field  
