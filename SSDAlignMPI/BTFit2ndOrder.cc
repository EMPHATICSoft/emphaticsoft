#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <complex.h>
#include <mpi.h>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <float.h>
#include <sys/time.h>

#include "BTAlignGeom.h"
#include "BTAlignInput.h"
#include "SSDAlignParams.h"
#include "BeamTracks.h"
#include "myMPIUtils.h"
#include "BeamTrackSSDAlignFCN.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MinosError.h"
#include "Minuit2/MnContours.h"
#include "Minuit2/MnScan.h"
//
// Attempting to to the SSD alignment over a sample of track, including, Pitch, Yaw roll angles, deltaZ, transverse positions.
// In this main, just ditribute the event and check them.  
// 

emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr=NULL; 
emph::rbal::SSDAlignParams* emph::rbal::SSDAlignParams::instancePtr=NULL; 

//
// Main program for doing the complete ~ 64 parameters fit of all misalignment SSD parameters. 

int main(int argc, char **argv) {
//
   
   double dumVar = 0; // place holder.. 
   
   std::string fitType("2DY");
   std::string fitSubType("TrShift"); // Valid strings are "NoFixes" (nothing fixed), TrShift, PitchCorr, ZShift, Roll (for now..)
                                      // Adding Magnet Z position, MagnetZPos and MagnetKick.  Both can be added  
				      // indeed, making a new subfit type, allowing for TrShift + Magnet 
				      // Feb 2 2023 (a day after GroundHog day, who knows, may our Minos will improve. 
				      // Try subfit type "TrTiltShift"  "TrZShift" only tilts and transverse shifts, or only 
				      // Tr and long shifts. Only valid for 2D (test first in the vertical plane). 
   int SensorForFitSubType(INT_MAX); // Valid strings are "All" (nothing fixed), TRShift, PitchCorr, 
   char ViewForFitSubType('A'); // Applicable for one sensor at a time, for V plane and U plane, mostly. 
   int maxEvts = 1000000;
   bool strictSt6Y = true;
   bool strictSt6X = true; // only 2/3 of the event, but worth while, I guess.
   
   size_t kSeTrShifted = INT_MAX;
   std::string viewShifted("none");
   double trShift = 0.;
   int selectedSpill = INT_MAX;
   std::string token("none");
   bool doCallFCNOnce = false; // for debugging purpose.. One cll to FCCN and that is it. 
   bool doMinos = true;
   unsigned int  contourP1Index = INT_MAX;
   unsigned int  contourP2Index = INT_MAX;
   int nContourPts = 20;
   unsigned int  scanP1Index = INT_MAX;
   unsigned int  scanP2Index = INT_MAX;
   int nScanPts = 50;
   bool doSoftLimits = false;
   bool applyEmittanceConstraint = false;
   // To be corrected!! See BeamTrackSSDAlignFCN.cxx
   double betaFunctionY = 1357.; // Only valid for 120 GeV  See e-mail from Mike Olander, Jan 20 
   double betaFunctionX = 377.; // Only valid for 120 GeV 
   double alphaFunctionY = -25.1063; // same..  
   double alphaFunctionX = -8.62823; //
   int runNum = 1055; 
  
     
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int aRank = world_size - 1;

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    if (myRank == 0) std::cerr << " Job argument decoding, number of args  "  << argc << std::endl;
    if (argc > 1) { 
      int argcR = argc - 1;
      for (int kArg = 0; kArg != argcR/2; kArg++) {
        std::string parStr(argv[1+2*kArg]);
        std::string valStr(argv[2+2*kArg]);
        if (myRank == 0) std::cerr << " parStr " << parStr << " valStr " << valStr << std::endl;
        std::string valStrcpy(valStr);
        for (size_t ii=0; ii != valStrcpy.length(); ii++) if (valStrcpy[ii] == '.') valStrcpy[ii] ='p';
        std::istringstream valStrStr(valStr);
        if (parStr.find("fitType") != std::string::npos) {
           fitType = valStr;
          if (myRank == 0) std::cerr << " The request fit type will be   "  << fitType << std::endl;
        } else if (parStr.find("maxEvt") != std::string::npos) {
          valStrStr >> maxEvts;
          if (myRank == 0) std::cerr << " Requested number of events  "  << maxEvts << std::endl;
        } else if (parStr.find("runNum") != std::string::npos) {
          valStrStr >> runNum;
          if (myRank == 0) std::cerr << " Run number will be "  << runNum << std::endl;
        } else if (parStr.find("strictSt6Y") != std::string::npos) {
          int iS=0;
          valStrStr >> iS;
	  strictSt6Y = (iS == 1);
          if (myRank == 0) {
	    if (strictSt6Y) std::cerr << " We will require 6 Y view hits, no more, no less  "  << std::endl;
	    else std::cerr << " Allowing for one missing hits in the Y view, includes less illuminated station 4 & 5 sensors  "  << std::endl;
	  }  
        } else if (parStr.find("strictSt6X") != std::string::npos) {
          int iS=0;
          valStrStr >> iS;
	  strictSt6X = (iS == 1);
          if (myRank == 0) {
	    if (strictSt6X) std::cerr << " We will require 6 X view hits, no more, no less  "  << std::endl;
	    else std::cerr << " Allowing for one missing hits in the X view, includes less illuminated station 4 & 5 sensors  "  << std::endl;
	  }  
        } else if (parStr.find("spill") != std::string::npos) {
          valStrStr >> selectedSpill;
          if (myRank == 0) std::cerr << " Requested spill  "  << selectedSpill << std::endl;
        } else if (parStr.find("viewShifted") != std::string::npos) {
          valStrStr >> viewShifted;
          if (myRank == 0) std::cerr << " Requested Sensor view for transverse shift  "  << kSeTrShifted << std::endl;
         } else if (parStr.find("TrShiftSensor") != std::string::npos) {
          valStrStr >> kSeTrShifted;
          if (myRank == 0) std::cerr << " Requested Sensor index for motion shift  "  << kSeTrShifted << std::endl;
       } else if (parStr.find("trShift") != std::string::npos) {
          valStrStr >> trShift;
          if (myRank == 0) std::cerr << " Requested transverse shifted value  "  << trShift << std::endl;
       } else if (parStr.find("fitSubType") != std::string::npos) {
          fitSubType = valStr;
          if (myRank == 0) std::cerr << " Fit SubType is   "  << fitSubType << std::endl;	
        } else if (parStr.find("SensorForFitSubType") != std::string::npos) {
          valStrStr >> SensorForFitSubType;
          if (myRank == 0) std::cerr << " All sensor attribute fixed, except one, which is    "  << SensorForFitSubType << std::endl;	
        } else if (parStr.find("ViewForFitSubType") != std::string::npos) {
          ViewForFitSubType = valStr[0];
          if (myRank == 0) std::cerr << " The Selected view for a specific sensor is     "  << ViewForFitSubType << std::endl;	
        } else if (parStr.find("contourP1Index") != std::string::npos) {
          valStrStr >> contourP1Index;
          if (myRank == 0) std::cerr << " Contour Parameter Index  1   "  << contourP1Index << std::endl;	
        } else if (parStr.find("contourP2Index") != std::string::npos) {
          valStrStr >> contourP2Index;
          if (myRank == 0) std::cerr << " Contour Parameter Index  2   "  << contourP2Index << std::endl;	
        } else if (parStr.find("nContourPts") != std::string::npos) {
          valStrStr >> nContourPts;
          if (myRank == 0) std::cerr << " Number of Contour points    "  << nContourPts << std::endl;	
         } else if (parStr.find("scanP1Index") != std::string::npos) {
          valStrStr >> scanP1Index;
          if (myRank == 0) std::cerr << " Scan Parameter Index  1   "  << scanP1Index << std::endl;	
        } else if (parStr.find("scanP2Index") != std::string::npos) {
          valStrStr >> scanP2Index;
          if (myRank == 0) std::cerr << " Scan Parameter Index  2   "  << scanP2Index << std::endl;	
        } else if (parStr.find("nScanPts") != std::string::npos) {
          valStrStr >> nScanPts;
          if (myRank == 0) std::cerr << " Number of Scan points    "  << nScanPts << std::endl;	
       } else if (parStr.find("callFCNOnce") != std::string::npos) {
          int iCallFCNOnce;
          valStrStr >> iCallFCNOnce;
	  doCallFCNOnce = (iCallFCNOnce == 1);
          if ((myRank == 0) && doCallFCNOnce)  std::cerr << "We will do a single call to FCN, and quit  " << std::endl;
          if ((myRank == 0) && (!doCallFCNOnce))  std::cerr << "We will do the complete fit.  " << std::endl;
        } else if (parStr.find("softLimits") != std::string::npos) {
          int iS;
          valStrStr >> iS;
	  doSoftLimits = (iS == 1);
          if ((myRank == 0) && doSoftLimits)  std::cerr << "We will use soft limit, assume a Gaussian model for errors (now default)   " << std::endl;
          if ((myRank == 0) && (!doSoftLimits))  std::cerr << "We will use strict limit on survey params, i.e., define . Minuit Limits  " << std::endl;
         } else if (parStr.find("beamConstraint") != std::string::npos) {
          int iS;
          valStrStr >> iS;
	  applyEmittanceConstraint = (iS == 1);
          if ((myRank == 0) && applyEmittanceConstraint)  std::cerr << "We will use apply beam emittance constraint, assuming 120 GeV    " << std::endl;
          if ((myRank == 0) && (!applyEmittanceConstraint))  std::cerr << "We will NOT use beam emittance constrain (default)   " << std::endl;
         } else if (parStr.find("betaFuncY") != std::string::npos) {
          valStrStr >> betaFunctionY;
          if (myRank == 0) std::cerr << " Twiss beta Function, Vertical     "  << betaFunctionY << std::endl;	
         } else if (parStr.find("betaFuncX") != std::string::npos) {
          valStrStr >> betaFunctionX;
          if (myRank == 0) std::cerr << " Twiss beta Function, Horizontal     "  << betaFunctionX << std::endl;	
        } else if (parStr.find("doMinos") != std::string::npos) {
          int iDoMinos;
          valStrStr >> iDoMinos;
	  doMinos = (iDoMinos == 1);
          if ((myRank == 0) && doMinos)  std::cerr << "We will run a Minos analysis   " << std::endl;
          if ((myRank == 0) && (!doMinos))  std::cerr << "We will not run a Minos analysis.   " << std::endl;
      } else if (parStr.find("token") != std::string::npos) {
          token = valStr;
          if (myRank == 0) std::cerr << " Token will be   "  << token << std::endl;
       } else {
          if (myRank == 0) std::cerr << " Unrecognized argument   "  << parStr <<  " fatal, quit here and now " << std::endl;
          MPI_Finalize();
          exit(2);
        }
       }   
    }
    
    if ((fitType != std::string("2DX")) && (fitType != std::string("2DY")) && (fitType != std::string("3D")) ) {
      if (myRank == 0) std::cerr << " BTFit2ndOrder, unrecognized fit type " << fitType << " fatal, quit here and now " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    if ((SensorForFitSubType != INT_MAX) && (ViewForFitSubType == 'A')) {
      if (myRank == 0) std::cerr << " BTFit2ndOrder, Requesting optimization of a specific sensor " 
                                 << SensorForFitSubType << " but view is not specfified, fatal (for now) " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    if ((SensorForFitSubType == INT_MAX) && (ViewForFitSubType != 'A')) {
      if (myRank == 0) std::cerr << " BTFit2ndOrder, Requesting optimization of a specific sensor with view " 
                                 << ViewForFitSubType << " but sensor index is not specfified, fatal (for now) " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    if ((ViewForFitSubType != 'A') && (ViewForFitSubType != 'X') && (ViewForFitSubType != 'Y') 
                                   && (ViewForFitSubType != 'U') &&(ViewForFitSubType != 'V')) {
      if (myRank == 0) std::cerr << " BTFit2ndOrder, Requesting optimization of an inexistant sensor view " 
                                 << ViewForFitSubType << " fatal That is it " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    //
    // Get the data, a set of SSD Cluster. 
    // 
    
    std::string topDirAll("/home/lebrun/EMPHATIC/DataLaptop/");
    std::string myHostName(std::getenv("HOSTNAME"));
    if (myHostName.find("fnal") != std::string::npos) topDirAll = std::string("/work1/next/lebrun/EMPHATIC/Data/"); // On fnal Wilson
    std::string aFName(topDirAll); 
    std::ostringstream runNumStrStr;   runNumStrStr << runNum;
    std::string runNumStr(runNumStrStr.str());
    std::string aFileDescr; 
    if ((!strictSt6Y) && (!strictSt6X)) aFileDescr = std::string("_5St_try9_AlignUV_GenCompactA1_V1b.dat");
    else {
      if ((strictSt6Y) && (!strictSt6X))  aFileDescr = std::string("_5St_try9_AlignUV_GenCompactA3_V1c2.dat");
      if ((!strictSt6Y) && (strictSt6X))  aFileDescr = std::string("_5St_try9_AlignUV_GenCompactA4_V1d.dat");
      if ((strictSt6Y) && (strictSt6X))   aFileDescr = std::string("_5St_try9_AlignUV_GenCompactA5_V1e.dat");
    }
    aFName += std::string("CompactAlgo1Data_") + runNumStr + aFileDescr;
    struct timeval tvStart, tvStop, tvEnd;
    char tmbuf[64];
    gettimeofday(&tvStart,NULL);
    time_t nowTimeStart = tvStart.tv_sec;
    struct tm *nowtm = localtime(&nowTimeStart);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
    std::string dateNow(tmbuf);
    std::cerr << ".... from processor rank " << myRank << " out of " << world_size 
    	      << " option " << token <<  " starting at " << dateNow << std::endl;
	      
     emph::rbal::BTAlignInput myBTIn;
     
     int numExpected = 67272; // I know this number from running SSDAlign Stu1 Algo1 on run 1055. 
     if ((strictSt6Y) && (!strictSt6X))  { numExpected = 52842; myBTIn.SetKey(687401); }
     if ((strictSt6X)  && (!strictSt6Y)) { numExpected = 49651; myBTIn.SetKey(687402); }
     if ((strictSt6X)  && (strictSt6Y)) { numExpected = 41321; myBTIn.SetKey(687403); }  // Only valid for run 1055
     if (myRank == 0)  {
         std::cerr << " Doing the Fit2ndorder on file " << aFName << std::endl;
         myBTIn.FillItFromFile(numExpected, aFName.c_str(), selectedSpill);
	 std::cerr << " .... this analysis will be based on " << myBTIn.GetNumEvts() << std::endl;
//	 std::cerr << " And quit for now... " << std::endl; MPI_Finalize(); exit(2);
      }

     emph::rbal::distributeEvts(myBTIn); 
    //
    // Now deal with the parameters.. 
    //
    
    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    emph::rbal::SSDAlignParams *myParams = emph::rbal::SSDAlignParams::getInstance();
    myParams->SetStrictSt6(strictSt6X || strictSt6Y); 
    //
    myParams->SetMode(fitType);
    myParams->SetSoftLimits(doSoftLimits);
    
    if (fitType == std::string("2DX")); myParams->SetMoveLongByStation(false);
    if ((fitType == std::string("2DY")) || (fitType == std::string("3D"))) {
    /*
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1, -0.5442576);  // decent values to start with, I hope.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2, -1.3555225);  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3, -1.7229351);  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4, -0.6333689); 
      // After running this code, mode 2DY, TrShift... 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  0.00283388  );  // decent values to start with, I hope.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -0.0067159 );  // Too small, overall.  Not consistent with 1rst order calculation. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  -0.0110786 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4,  -1.33479   ); // large, but very few statistic for 120 GeV beam.  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 5,  0.0190995  ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 6,  -0.494184  ); // large, but very few statistic for 120 GeV beam. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 7,  0.0545789  ); ??? Was supposed to be fixed.. 
      // 3rd attemps .. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  -0.417856  );  //Starts to make sense.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -1.66744 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  -2.22324 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4,  -4.99995 ); // large, but very few statistic for 120 GeV beam.  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 5,  -2.05539 ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 6, 2.5 ); // at limit large, but very few statistic for 120 GeV beam. 
      */
//       
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('Y', k, 0.075); // Only ~ 1 strips, in average.. 
      myGeo->SetUnknwonUncert('Y', 4, 5.0);  myGeo->SetUnknwonUncert('Y', 6, 5.0); // again, suspicious track that are widely deflected.  
// 
//  Result from 3rd attemps 
//
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  -0.418312  );  //Starts to make sense.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -1.66995 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  -2.22679 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4, -5.  ); // large, but very few statistic for 120 GeV beam.  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 5, -2.05875  ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 6,   -5.0        ); // at limit large, but very few statistic for 120 GeV beam. 
      // Now that is stable.. 
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('Y', k, 0.04); // Only ~ 1 strips, in average.. 
      myGeo->SetUnknwonUncert('Y', 4, 5.0);  myGeo->SetUnknwonUncert('Y', 6, 5.0); // again, suspicious track that are widely deflected.  
//
// Result of the first Z minimization.. 
//     
      myParams->SetValue(emph::rbal::ZSHIFT, 'Y', 1, -1.39071  );  //Starts to make sense.. 
      myParams->SetValue(emph::rbal::ZSHIFT, 'Y', 2, -2.28253);  
      myParams->SetValue(emph::rbal::ZSHIFT, 'Y', 3, -2.41788);  
      myParams->SetValue(emph::rbal::ZSHIFT, 'Y', 4, -9.97131  ); // does not matter. Can't tell..  
      myParams->SetValue(emph::rbal::ZSHIFT, 'Y', 5, -0.239897 );
      myParams->SetValue(emph::rbal::ZSHIFT, 'Y', 6, -10.0  ); // at limit large, but very few statistic for 120 GeV beam. 
 // 
//  Result from 5th attemps, all 2DY parameters free.  
//
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  -0.43052  );  //Starts to make sense.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -1.64884 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  -2.22319 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4, -5.  ); // large, but very few statistic for 120 GeV beam.  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 5, -2.03585  ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 6,   -5.0        ); // at limit large, but very few statistic for 120 GeV beam. 
 // 
//  Result from xxth attemps, all LongShift fixed, null, Tr and tilt, with soft limits. Tilt factor now positive..  Or consistent with zero. 
//  StrictSt6 X Y See Feb_7 Scan series. 
// 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  -0.6051372735  );  //Starts to make sense.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -1.599688114 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  -2.130612278 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4, -5.  ); // large, but very few statistic for 120 GeV beam.  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 5, -2.0105059891  ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 6,   -5.0        ); // at limit large, but very few statistic for 120 GeV beam. 
      // Now that is stable.. 
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('Y', k, 0.0000004); // Turn it off! 
      myGeo->SetUnknwonUncert('Y', 4, 5.0);  myGeo->SetUnknwonUncert('Y', 6, 5.0); // again, suspicious track that are widely deflected.  
//
//   U coordinates; 
//
     myParams->SetValue(emph::rbal::TRSHIFT, 'U', 0,  0.137950); // First single Param min. 
     myParams->SetValue(emph::rbal::TRSHIFT, 'U', 1,  0.2920131); // First single Param min. 
     myGeo->SetUnknwonUncert('U', 0, 0.025);    myGeo->SetUnknwonUncert('U', 1, 0.025); // tentative.. 
//
// V coordinates.
//SensorForFitSubType
     for (size_t k=0; k!= 4; k++) myGeo->SetUnknwonUncert('V', k, 10.0); // Turn of ftheir weight, not rough aligned yet. 
     for (size_t k=0; k!= 4; k++) myParams->SetValue(emph::rbal::TRSHIFT, 'V', k,  0.);   // To make sure.. 
     myParams->SetValue(emph::rbal::TRSHIFT, 'V', 1,  2.219765422); // Success value, keep.. Little bias due to sift Limits. 
//     if (SensorForFitSubType != 1) myGeo->SetUnknwonUncert('V', 1, 100.); // Semi successfull, large error, poor statistics. 
//     else myGeo->SetUnknwonUncert('V', 1, 0.025); // attempting to aling the second sensor V, in Station 4. 
//     myGeo->SetUnknwonUncert('V', 1, 0.025); // attempting to aling the second sensor V, in Station 4. 
     myGeo->SetUnknwonUncert('V', 1, 5.0); // still FUBAR  
     myGeo->SetUnknwonUncert('V', 0, 5.0); // still FUBAR  
     // Success.. 
//     if (SensorForFitSubType != 2) myGeo->SetUnknwonUncert('V', 2, 100.); // Semi successfull, large error, poor statistics. 
//     else myGeo->SetUnknwonUncert('V', 2, 0.025); // attempting to aling the first sensor V, in Station 5. 
     myGeo->SetUnknwonUncert('V', 2, 0.025); // Success.. Leave it as that. 
     myParams->SetValue(emph::rbal::TRSHIFT, 'V', 2,  1.148411723); //  We get the X from a Projection from station 4& 5 on 1500 evets. for Run 1055 
     // But.. Residual looks O.K, with some noise.. Central peak is at resid 0., sigma 48 microns.  
     // Last plane.. V 3, station 5.  Uncertainty is 800 microns.  
     myGeo->SetUnknwonUncert('V', 3, 0.025); // attempting to aling the second sensor V, in Station 5. 
     myParams->SetValue(emph::rbal::TRSHIFT, 'V', 3,  -3.361921529); // Geeting there.. 
     myGeo->SetUnknwonUncert('V', 2, 5.0); // still FUBAR  
     myGeo->SetUnknwonUncert('V', 3, 5.0); // still FUBAR  
     
    } 
    if ((fitType == std::string("2DX")) || (fitType == std::string("3D"))) {
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1,  1.585286); 
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2,  1.697424);  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3,  2.983219);  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -1.495283); 
//
//   2n try , reducing the error .  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1,  1.68684 ); 
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2,  2.5173  );  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3,  4.18659 );  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -5.0 ); // Poor statistics.. 
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 5,  -0.279456 );
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 6,  -5.0 );
//      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('X', k, 0.030); // Only ~ 2 strips, in average.. 
//      myGeo->SetUnknwonUncert('X', 4, 5.0);  myGeo->SetUnknwonUncert('X', 6, 5.0); // again, suspicious track that are widely deflected.  
//   3rd (or 4rth)   try , reducing the error .  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.70846   ); 
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2, 2.57212  );  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3, 4.25464  );  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -5.0 ); // Poor statistics.. 
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 5, -0.320193  );
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 6,  -5.0 );
//      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('X', k, 0.025); // a fraction of the strip.. 
//
// Strict St6, Feb 7 
//     
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.68564   ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2, 2.49931  );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3, 4.1595  );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -5.0 ); // Poor statistics.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 5, -0.251312  );
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 6,  -5.0 );
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('X', k, 0.025); // a fraction of the strip.. 

    }
    if (fitType == std::string("3D")) {  // Result loosening up the alignment constraints, Feb 10 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 1,  -0.4232866132  );  // Back to previously found value, the 600 micron was due to soft limit constraints. . 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 2,  -1.688700314 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 3,  -2.250664639 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 4, -5.  ); // large, but very few statistic for 120 GeV beam.  
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 5, -2.050701327  ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'Y', 6,   -5.0        ); // at limit large, but very few statistic for 120 GeV beam. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.681291094 ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2, 2.487036371  );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3, 4.143409683 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -5.0 ); // Poor statistics.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 5,  -0.2362932867 );
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 6,  -5.0 );
      myParams->SetValue(emph::rbal::TRSHIFT, 'U', 0,  0.1310751719); //  
      myParams->SetValue(emph::rbal::TRSHIFT, 'U', 1,  0.2708798602 ); // 
      myParams->SetValue(emph::rbal::TRSHIFT, 'V', 1, 2.276203614 ); // Success value, keep.. Little bias due to sift Limits. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'V', 2, 1.071068301 ); // Success value, keep.. Little bias due to sift Limits. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'V', 3, -3.284696663 ); // Success value, keep.. Little bias due to sift Limits. 
   } 
    
    if (myRank == 0) myParams->DumpTable(token); 
//    if (myRank == 0) std::cerr << " Booby trap, stop now " << std::endl;
//    MPI_Finalize();
//    exit(2);

    ROOT::Minuit2::MnUserParameters uPars;
    for (size_t kPar=0; kPar != myParams->size();  kPar++) { 
      std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
      std::string aName(itP->Name());
      double aValue = itP->Value();
      double err = 0.1*std::abs(itP->Limits().second);
      if (err < 1.0e-10) err = 0.1;
      uPars.Add(aName, aValue, err);
      if (!doSoftLimits) uPars.SetLimits(aName, itP->Limits().first, itP->Limits().second);
    }
    
    
    emph::rbal::BeamTrackSSDAlignFCN theFCN(fitType, &myBTIn);
    theFCN.SetSoftLimits(doSoftLimits);
    theFCN.SetBeamConstraint(applyEmittanceConstraint);
    // Wrong value above... 
//    if (applyEmittanceConstraint) { 
//      theFCN.SetBeamAlphaBetaFunctionY(alphaFunctionY, betaFunctionY); 
//      theFCN.SetBeamAlphaBetaFunctionX(alphaFunctionX, betaFunctionX);
//    } 
    //
    
    if (doCallFCNOnce) {
      std::vector<double> parTmp(myParams->size(), 0.); 
      for (size_t k=0; k != myParams->size(); k++) {
 	 std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(k);
	 parTmp[k] = itP->Value();
      }
      theFCN.SetDebug(true);
      double chiSqTmp = theFCN(parTmp); 
      std::cerr << " Caling FCN once.. Result from rank " << myRank << " is " << chiSqTmp << std::endl;
      MPI_Finalize(); 
      exit(0);
   } 
    
    ROOT::Minuit2::MnMigrad  migrad(theFCN, uPars);
    //
    // Fix them, depending on subMode.. 
   //
    int nFixed= 0;
    std::vector<std::string> namesToBeFixed; namesToBeFixed.clear(); 
    std::vector<bool> fixed(myParams->size(), true);
    if (fitSubType != std::string("NoFixes")) {
      // by default, all are fixed, except a few, possible only one..  
      if (myRank == 0) std::cerr << " ... On Rank0, deciding which parameters are fixed.. numPars " 
                                 << myParams->size() << " for fitSubType " << fitSubType << std::endl;
      for (size_t kPar=0; kPar != myParams->size();  kPar++) {
         bool isFixed = true;
	 std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
	 std::string aName(itP->Name());
//         if (myRank == 0) std::cerr << " ... On Rank0, at Name .. " 
//                                 << aName << " Sensor " << itP->SensorI() << std::endl;
//  Re-write these clauses.. Keyin on whether or not we ask for a specific sensor. 
         if (SensorForFitSubType == INT_MAX) { 
	   if (((fitSubType == std::string("TrShift")) || (fitSubType == std::string("TrShiftMagnetKick")) ||
	        (fitSubType == std::string("TrZShift")) ||(fitSubType == std::string("TrTiltShift")))   && 
	        (aName.find("TransShift") != std::string::npos)) isFixed = false;
	   if (((fitSubType == std::string("ZShift"))||(fitSubType == std::string("TrZShift"))) && 
	        (aName.find("LongShift") != std::string::npos)) isFixed = false;
	   if (((fitSubType == std::string("PitchCorr")) || (fitSubType == std::string("TrTiltShift"))) && 
	        (aName.find("Tilt") != std::string::npos)) isFixed = false;
	   if ((fitSubType == std::string("DeltaRoll")) && 
	       (aName.find("DeltaRoll") != std::string::npos)) isFixed = false;
	 } else {
	   if ((ViewForFitSubType == itP->View()) && (SensorForFitSubType == itP->SensorI())) { 
	 // Specific sensors. 
	 // Find the sensor in the struct, Require  
	     if ((fitSubType == std::string("TrShift")) && (aName.find("TransShift") != std::string::npos)) isFixed = false;
	     if ((fitSubType == std::string("TrTiltShift")) && (aName.find("TransShift") != std::string::npos)) isFixed = false;
	     if ((fitSubType == std::string("ZShift")) && (aName.find("LongShift") != std::string::npos)) isFixed = false;
	     if ((fitSubType == std::string("PitchCorr")) && (aName.find("Tilt") != std::string::npos)) isFixed = false;
	     if ((fitSubType == std::string("TrTiltShift")) && (aName.find("Tilt") != std::string::npos)) isFixed = false;
	     if ((fitSubType == std::string("DeltaRoll")) && (aName.find("DeltaRoll") != std::string::npos)) isFixed = false;
	     if (fitSubType == std::string("TrShiftTiltRoll")) {
	        if (aName.find("DeltaRoll") != std::string::npos) isFixed = false;
	        if (aName.find("TransShift") != std::string::npos) isFixed = false;
	        if (aName.find("Tilt") != std::string::npos) isFixed = false;
	     }
	     if (fitSubType == std::string("TiltRoll")) {
	        if (aName.find("DeltaRoll") != std::string::npos) isFixed = false;
	        if (aName.find("Tilt") != std::string::npos) isFixed = false;
	     }
	   }
	 }
	 // Tuning the magnet.. Might not be needed, if we have the accurate field map. 
	 if (((fitSubType == std::string("MagnetZPos")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("LongMagC") == 0)) isFixed = false;   
	 if (((fitSubType == std::string("MagnetKick")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("KickMag") == 0)) isFixed = false;
	 if (((fitSubType == std::string("TrShiftMagnetKick")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("KickMag") == 0)) isFixed = false;
	 // try tilt and KickMag    
	 if (((fitSubType == std::string("TrTiltShiftMagnetKick"))) &&
	      (aName.find("KickMag") == 0)) isFixed = false;
	  // This sensor is not illuminated, or simply dead for run 1055 
//	  if (aName.find("_V_0") != std::string::npos) isFixed = true;
	  if (aName.find("_V_") != std::string::npos) { // gave up on V sensors for now, something still wrong.. 
	      isFixed = true;
	  }
	 //
	 // decided.. 
	 //
	 if (isFixed) namesToBeFixed.push_back(aName);
	 fixed[kPar] = isFixed;
	 if ((myRank == 0) && (!isFixed))  
	   std::cerr << "  ..... on rank " << myRank << "  Varying parameter " << aName << std::endl;
        }
        for (size_t kP=0; kP != namesToBeFixed.size(); kP++) {
          std::string aName(namesToBeFixed[kP]);
          migrad.Fix(aName.c_str());
        }
     }
//     std::cerr << " quit for now.... " << std::endl;
//     MPI_Finalize(); exit(2);
     if (myRank == 0) theFCN.OpenChiSqHistoryFile(token);
     //
     // Minimize
     //
     ROOT::Minuit2::FunctionMinimum min = migrad();
     bool myMinMigrad = min.IsValid();
     if (!myMinMigrad) {
       if (myRank == 0) {
         std::cerr << "  ... On rank 0, Minimum from Migrad is invalid... " << std::endl;
	 std::cerr << min << std::endl;
//	 theFCN.CloseChiSqHistoryFile();
       }
//       MPI_Finalize();
//       exit(2); 
// We want to be able to do the Scan.. 
     } else {
       
       if (myRank == 0) {
         std::cerr << "  ... On rank 0, Minimum from Migrad has been declared valid... " << std::endl;
         gettimeofday(&tvStop,NULL);
         time_t nowTimeStop = tvStop.tv_sec;
         struct tm *nowtm = localtime(&nowTimeStop);
         strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
         std::string dateNow(tmbuf);
         std::cerr << "  ... On rank 0, Minimum from Migrad has been declared valid...time  " << dateNow << std::endl;
         std::cerr << " The minimum is " << min << std::endl;
       }
       //
       // Save the set of track for this solution.. 
       //
       std::vector<double> parsSol(myParams->size(), 0.);
       for (size_t kP=0; kP != myParams->size(); kP++) parsSol[kP] = min.UserState().Value(kP);
       theFCN.SetDumpBeamTracksForR(true);
       std::string aNameSol("./BeamTracksFromMin_"); aNameSol += token; aNameSol += std::string("_V1.txt");
       theFCN.SetNameForBeamTracks(aNameSol);
       double chiSol = theFCN(parsSol);
       if (myRank == 0) {
         if (std::abs(chiSol - min.Fval()) > 1.0e-3) {
	     std::cerr << " .... On Rank 0, dump tracks for CVS, inconsistent chi-Sq problem, at min  " 
	               <<  min.Fval()  << " by FCN call " << chiSol << " Fatal.. " << std::endl; 
	     MPI_Finalize(); exit(2);
	 }
       }    	       
       theFCN.SetDumpBeamTracksForR(false);
       
     }
     // Requested Scans..  ? My own, based on Limits.. 
     if ((scanP1Index < static_cast<unsigned int>(myParams->size())) 
           && (scanP2Index < static_cast<unsigned int>(myParams->size()))) {
        std::vector<emph::rbal::SSDAlignParam>::const_iterator itP1 = myParams->It(static_cast<size_t>(scanP1Index));
	 // !!by Minuit Parameters numberr. See printed list.. 
        std::vector<emph::rbal::SSDAlignParam>::const_iterator itP2 = myParams->It(static_cast<size_t>(scanP2Index));
        if (myRank == 0) std::cerr << " Requesting Scans..For parameters " << itP1->Name()
	       << " and " << itP2->Name() << " Number of calls if FCN history file, so far " << theFCN.NCalls() <<std::endl;
	 std::vector<double> tmpPars(myParams->size(), 0.); size_t kp = 0;
	 std::pair<double, double> limitsP1 =  itP1->Limits(); std::pair<double, double> limitsP2 =  itP2->Limits();
	 double range1 = limitsP1.second - limitsP1.first; double range2 = limitsP2.second - limitsP2.first; 
	 double start1 = limitsP1.first; double start2 = limitsP2.first;	 
	 for (size_t kp=0; kp != myParams->size(); kp++) {
           std::vector<emph::rbal::SSDAlignParam>::const_iterator itPC = myParams->It(kp);
	   tmpPars[kp] = min.UserState().Value(itPC->Name());
	 }
	 //
	 // Using the Migrad errors, if the minimum is valid. 
	 //
	 if (myMinMigrad) {
	   range1 = 3.0*min.UserState().Error(itP1->Name()); range2 = 3.0*min.UserState().Error(itP2->Name());
	   start1 = min.UserState().Value(itP1->Name()) - 0.5*range1;
	   start2 = min.UserState().Value(itP2->Name()) - 0.5*range2;
	 }
	 double delta1 = range1/nScanPts; double delta2 = range2/nScanPts;
	 std::ofstream aFoutScan;
	 if (myRank == 0) {
	    std::string aNameScan("./Scan_"); aNameScan += token + std::string("_V1.txt");
	    aFoutScan.open(aNameScan.c_str());
	    aFoutScan << " k " << itP1->Name() << " " << itP2->Name() << " chiSq " << std::endl;
	 }
	 int ncc = 0;
	 for (int iSc1=0; iSc1 != nScanPts; iSc1++) {
	   tmpPars[scanP1Index] =  start1 + iSc1*delta1;
	   for (int iSc2=0; iSc2 != nScanPts; iSc2++) {
	     tmpPars[scanP2Index] =  start2 + iSc2*delta2;
	     double chiSqTmp = theFCN(tmpPars);
	     if (myRank == 0) 
	       aFoutScan  << " " << ncc << " " 
	                                << tmpPars[scanP1Index] << " " << tmpPars[scanP2Index] << " " << chiSqTmp << std::endl;
					// Result also available in FCN History file. 
	     ncc++;
	   }
	 }
         if (myRank == 0) { aFoutScan.close(); std::cerr << " Done with Scan" << itP1->Name()
	       << " and " << itP2->Name() << " Number of calls if FCN history file " << theFCN.NCalls() <<std::endl; }
    
     }
     if (myRank == 0) 
        std::cerr << " .. ChiSq " << min.Fval() << " Final Values and Minos Errors Num Params " <<  myParams->size() << std::endl;
     //
     // Call minos 
     //
     if (myMinMigrad && doMinos) { 
        ROOT::Minuit2::MnMinos minos(theFCN, min);
     //
     // Print the results. 
       if (myRank == 0) {
       std::cerr << " Name      Value     Errors -/+  " << std::endl;
       }
       for (size_t kPar=0; kPar != myParams->size();  kPar++) {
         if ((fitSubType != std::string("NoFixes")) && fixed[kPar]) continue; 
         std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
         std::string aName(itP->Name());
//         if (aName.find("TransShift") == std::string::npos) continue; // This is the uncertainty we reall care about.. 
         double theValue  = min.UserState().Value(aName);
         std::pair<double, double> err = minos(kPar, 1000);
         ROOT::Minuit2::MinosError mey = minos.Minos(kPar);
	 if (!mey.IsValid()) {
	   if (myRank == 0) { 
	     std::cerr << "  " << aName << "  !!!! Minos failed .. reverting to Migrad error " << std::endl;
             double theValue  = min.UserState().Value(aName);
	     double err = min.UserState().Error(aName);
	     std::cerr << " " << aName << " " << theValue << " " << err << std::endl; 
	   }
	 } else { 
           if (myRank == 0) 
	      std::cerr << " " << aName << " " << theValue << " " << err.first << "  " << err.second << std::endl;
	 } 
       }
     // Requested contours ?
       if (myMinMigrad && (contourP1Index < static_cast<unsigned int>(myParams->size())) 
           && (contourP2Index < static_cast<unsigned int>(myParams->size()))) { // !!Indices by Minuit Parameters numberr. See printed list..
       
         theFCN.SetUpError(2.0);
	 std::cerr << " On rank " << myRank << " Just before declaring ROOT::Minuit2::MnContours " << std::endl;
         ROOT::Minuit2::MnContours contour(theFCN, min);
	 std::cerr << " On rank " << myRank << " Just before running ROOT::Minuit2::MnContours " << std::endl;
         std::vector<std::pair<double, double> > resContour = contour(contourP1Index, contourP2Index, nContourPts); 
	 std::cerr << " On rank " << myRank << " Just after running ROOT::Minuit2::MnContours " << std::endl;
         std::string aNameC("./BTFit2ndOrder_"); aNameC += token;  aNameC += std::string("_Contour_");
         std::vector<emph::rbal::SSDAlignParam>::const_iterator itP1 = myParams->It(static_cast<size_t>(contourP1Index));
         std::vector<emph::rbal::SSDAlignParam>::const_iterator itP2 = myParams->It(static_cast<size_t>(contourP2Index));
         aNameC += itP1->Name() + std::string("__") + itP2->Name() + std::string(".txt");
         std::ofstream fOutC(aNameC.c_str());
         fOutC << " k  " << itP1->Name() << " " << itP2->Name() << " " << std::endl;
         int k = 0; 
         for (std::vector<std::pair<double, double> >::const_iterator it =  resContour.cbegin(); it != resContour.cend(); it++, k++) {
           fOutC << " " << k << " " << it->first << " " << it->second << std::endl;
         }
         fOutC.close();
         theFCN.SetUpError(1.0);
       }
     } else {
       if (myRank == 0) {
        std::cerr << " Name      Value     Errors  " << std::endl;
        for (size_t kPar=0; kPar != myParams->size();  kPar++) {
         if ((fitSubType != std::string("NoFixes")) && fixed[kPar]) continue; 
         std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
         std::string aName(itP->Name());
         double theValue  = min.UserState().Value(aName);
	 double err = min.UserState().Error(aName);
         if (myRank == 0) std::cerr << " " << aName << " " << theValue << " " << err << std::endl;
       }
      } // 
     } // Do minos or not.. 
     if (myRank == 0) theFCN.CloseChiSqHistoryFile();
     std::cerr << " Done, calling MPI_Finalze from " << myRank << std::endl;
     MPI_Finalize();
     
}