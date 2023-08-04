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
#include "BTMagneticField.h"
#include "BTAlignInput.h"
#include "SSDAlignParams.h"
#include "BeamTracks.h"
#include "myMPIUtils.h"
#include "BeamTrackSSDAlignFCN.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnSimplex.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MinosError.h"
#include "Minuit2/MnContours.h"
#include "Minuit2/MnScan.h"
//
// Attempting to to the SSD alignment over a sample of track, including, Pitch, Yaw roll angles, deltaZ, transverse positions.
// In this main, just ditribute the event and check them.  
// 

emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr=nullptr; 
emph::rbal::SSDAlignParams* emph::rbal::SSDAlignParams::instancePtr=nullptr; 
emph::rbal::BTMagneticField* emph::rbal::BTMagneticField::instancePtr=nullptr; 

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
				      // April 28, adding TrShiftX456, vary only the 3 downstream sensors. 
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
   bool doLoadParamsFromPreviousRun = false;
   bool doLoadParamsFromPreviousFCNHist = false;
   int requestFCNCallNumber = INT_MAX;
   unsigned int  contourP1Index = INT_MAX;
   unsigned int  contourP2Index = INT_MAX;
   int nContourPts = 20;
   unsigned int  scanP1Index = INT_MAX;
   unsigned int  scanP2Index = INT_MAX;
   std::string scanP1Name("none"); 
   std::string scanP2Name("none"); 
   int nScanPts = 50;
   bool doSoftLimits = false;
   int SoftLimitSlopeSigma = INT_MAX;
   bool applyEmittanceConstraint = false;
   // To be corrected!! See BeamTrackSSDAlignFCN.cxx
   double betaFunctionY = 1357.; // Only valid for 120 GeV  See e-mail from Mike Olander, Jan 20 
   double betaFunctionX = 377.; // Only valid for 120 GeV 
   double alphaFunctionY = -25.1063; // same..  
   double alphaFunctionX = -8.62823; //
   double nominalMomentum = 29.7; // for the 30 GeV analysis
   double uPLimitOnChiSqTrackFits = 200.; 
   double assumedDoubleGap = 3.25; // starting value for the gap 
   int runNum = 1055; // For data... 
   double integrationStepSize = 0.;
   double EdmMaxTolerance = 0.5;
   double magnetShiftX = 0.; double magnetShiftY = 0.; double magnetShiftZ = 0.;
   bool magnetIsRemoved = false;  
   bool doPencilBeam120 = false; 
   bool doAntiPencilBeam120 = false; 
   bool doAntiPencilBeam120OnlyV = false; 
   std::string G4EMPHDescrToken("none");
   int numMaxFCNCalls = 5000;
   bool doRejectVViews=false;
  
     
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int aRank = world_size - 1;

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    // 
    // Debugging crash in MPI .. This should run.. 
//    if (token.find("non") != std::string::npos) { 
//       std::cerr << " Check MPI sanity... " << std::endl;
//          emph::rbal::BeamTracks myBTrs; // empty.. 
//         double chi2 = emph::rbal::MeanChiSqFromBTracks(myBTrs, DBL_MAX/2, (0.111) ); // We include the ones that  
//    }
    
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
        } else if (parStr.find("MCInToken") != std::string::npos) {
          G4EMPHDescrToken = valStr;
          if (myRank == 0) std::cerr << " The Input data is G4EMP, token is "  << G4EMPHDescrToken << std::endl;
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
         } else if (parStr.find("scanP1Name") != std::string::npos) {
	  scanP1Name = valStr; 
          if (myRank == 0) std::cerr << " Scan Parameter Name  1   "  << valStr << std::endl;	
        } else if (parStr.find("scanP2Name") != std::string::npos) {
	  scanP2Name = valStr; 
          if (myRank == 0) std::cerr << " Scan Parameter Name  2   "  << valStr << std::endl;	
        } else if (parStr.find("nScanPts") != std::string::npos) {
          valStrStr >> nScanPts;
          if (myRank == 0) std::cerr << " Number of Scan points    "  << nScanPts << std::endl;	
       } else if (parStr.find("callFCNOnce") != std::string::npos) {
          int iCallFCNOnce;
          valStrStr >> iCallFCNOnce;
	  doCallFCNOnce = (iCallFCNOnce == 1);
          if ((myRank == 0) && doCallFCNOnce)  std::cerr << "We will do a single call to FCN, and quit  " << std::endl;
          if ((myRank == 0) && (!doCallFCNOnce))  std::cerr << "We will do the complete fit.  " << std::endl;
       } else if (parStr == std::string("startFromPrev")) {
          int iTmpPrev;
          valStrStr >> iTmpPrev;
	  doLoadParamsFromPreviousRun = (iTmpPrev == 1);
          if ((myRank == 0) && doLoadParamsFromPreviousRun)  std::cerr << "We will restart from previous, same seq major tag  " << std::endl;
        } else if (parStr == std::string("startFromPrevHist")) {
          int iTmpPrev;
          valStrStr >> iTmpPrev;
	  doLoadParamsFromPreviousFCNHist = (iTmpPrev == 1);
          if ((myRank == 0) && doLoadParamsFromPreviousFCNHist)  std::cerr << "We will restart from previous run, based on FCN History " << std::endl;
        } else if (parStr == std::string("PrevHistNCallReq")) {
          valStrStr >> requestFCNCallNumber ;
          if ((myRank == 0) )  std::cerr << " Requested NCall number on FCN History " << requestFCNCallNumber << std::endl;
        } else if (parStr.find("softLimits") != std::string::npos) {
          int iS;
          valStrStr >> iS;
	  doSoftLimits = (iS == 1);
          if ((myRank == 0) && doSoftLimits)  std::cerr << "We will use soft limit, assume a Gaussian model for errors (now default)   " << std::endl;
          if ((myRank == 0) && (!doSoftLimits))  std::cerr << "We will use strict limit on survey params, i.e., implemented as Minuit Limits  " << std::endl;
          if (iS > 1) {
	    SoftLimitSlopeSigma = iS;
	    if (myRank == 0) std::cerr << " ... Not quite.. We will assume the the slopes at Station 0 have a sigma of " 
	                                 << 1.0e-6*SoftLimitSlopeSigma << " micro-radian " << std::endl;
	  }
        } else if (parStr.find("pencilBeam") != std::string::npos) {
          int iS;
          valStrStr >> iS;
	  doPencilBeam120 = (iS == 1);
          if ((myRank == 0) && doPencilBeam120)  std::cerr << "We will assume a pencil beam, at x= -4.5, y= 3.8 mm    " << std::endl;
          if ((myRank == 0) && (!doPencilBeam120))  std::cerr << "We will assume a broad beam, all sensors active.  " << std::endl;
         } else if (parStr == std::string("antiPencilBeam")) {
          int iS;
          valStrStr >> iS;
	  doAntiPencilBeam120 = (iS == 1);
          if ((myRank == 0) && doAntiPencilBeam120)  std::cerr << "We will assume pencil beam sensor are fixed.     " << std::endl;
         } else if (parStr == std::string("antiPencilBeamOnlyV")) {
          int iS;
          valStrStr >> iS;
	  doAntiPencilBeam120OnlyV = (iS == 1);
          if ((myRank == 0) && doAntiPencilBeam120OnlyV)  std::cerr << "We will assume pencil beam sensor are fixed, varying only V views " << std::endl;
         } else if (parStr.find("beamConstraint") != std::string::npos) {
          int iS;
          valStrStr >> iS;
	  applyEmittanceConstraint = (iS == 1);
          if ((myRank == 0) && applyEmittanceConstraint)  std::cerr << "We will use apply beam emittance constraint, assuming 120 GeV    " << std::endl;
          if ((myRank == 0) && (!applyEmittanceConstraint))  std::cerr << "We will NOT use beam emittance constrain (default)   " << std::endl;
         } else if (parStr.find("rejectVViews") != std::string::npos) {
          int iS;
          valStrStr >> iS;
	  doRejectVViews = (iS == 1);
          if ((myRank == 0) && doRejectVViews)  std::cerr << "We will not use the V views in Station 5    " << std::endl;
          if ((myRank == 0) && (!doRejectVViews))  std::cerr << "We will use relevant V views (default)   " << std::endl;
         } else if (parStr.find("nomMom") != std::string::npos) {
          valStrStr >> nominalMomentum;
          if (myRank == 0) std::cerr << " The nominal momentum is      "  << nominalMomentum << std::endl;	
         } else if (parStr.find("betaFuncY") != std::string::npos) {
          valStrStr >> betaFunctionY;
          if (myRank == 0) std::cerr << " Twiss beta Function, Vertical     "  << betaFunctionY << std::endl;	
         } else if (parStr.find("betaFuncX") != std::string::npos) {
          valStrStr >> betaFunctionX;
          if (myRank == 0) std::cerr << " Twiss beta Function, Horizontal     "  << betaFunctionX << std::endl;	
        } else if (parStr.find("UpLimChisSqTr") != std::string::npos) {
          valStrStr >> uPLimitOnChiSqTrackFits;
          if (myRank == 0) std::cerr << " The upper limit on a the chiSq of a Beam Track is      "  << uPLimitOnChiSqTrackFits << std::endl;	
        } else if (parStr.find("EdmMaxTolerance") != std::string::npos) {
	  valStrStr >> EdmMaxTolerance;
          if (myRank == 0) std::cerr << " The Minuit2 Simplex Estimated distance to mnimim will be  "  << EdmMaxTolerance << std::endl;	
        } else if (parStr.find("maxFCNCalls") != std::string::npos) {
	  valStrStr >> numMaxFCNCalls;
          if (myRank == 0) std::cerr << " The Minuit2 Migrad maximum number of calls is  "  << numMaxFCNCalls << std::endl;	
        } else if (parStr.find("DoubleGap") != std::string::npos) {
          valStrStr >> assumedDoubleGap;
          if (myRank == 0) std::cerr << " The gap betweeen double sensors is assumed to be "  << assumedDoubleGap << std::endl;	
        } else if (parStr.find("IntegrationStep") != std::string::npos) {
          valStrStr >> integrationStepSize;
          if (myRank == 0)  {
	     if (integrationStepSize > 1.0e-6)  
	       std::cerr << " The Magnetic Field map will be upload and 3D track fit will fit momentum with step size "  
	                             << integrationStepSize << std::endl;
	     else if (std::abs(integrationStepSize) < 1.0e-6)		     
	       std::cerr << " Simple kick approximation will be used, but the momentum will be fitted.  "  << std::endl;
	     else 
	       std::cerr << " We will assume this is 120 GeV track, no momentum fits.  "  << std::endl;
				     
	     }
        } else if (parStr.find("RemoveMagnet") != std::string::npos) {
	  int iii = 0;
          valStrStr >> iii;
          if ((myRank == 0) && (iii == 1)) std::cerr << " The Magnet has been physically removed  " << std::endl; 
          if ((myRank == 0) && (iii != 1)) std::cerr << " The Magnet is in the spectrometer  "  << std::endl;
	  magnetIsRemoved =  (iii == 1);
        } else if (parStr.find("MagShiftX") != std::string::npos) {
          valStrStr >> magnetShiftX;
          if (myRank == 0) std::cerr << " The Magnetic Field map will be shifted along the X axis by  "  
	                             << magnetShiftX << std::endl;
        } else if (parStr.find("MagShiftY") != std::string::npos) {
          valStrStr >> magnetShiftY;
          if (myRank == 0) std::cerr << " The Magnetic Field map will be shifted along the Y axis by  "  
	                             << magnetShiftY << std::endl;
        } else if (parStr.find("MagShiftZ") != std::string::npos) {
          valStrStr >> magnetShiftZ;
          if (myRank == 0) std::cerr << " The Magnetic Field map will be shifted along the Z axis by  "  
	                             << magnetShiftZ << std::endl;
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
    if ((ViewForFitSubType != 'A') && (ViewForFitSubType != 'X') && (ViewForFitSubType != 'Y') 
                                   && (ViewForFitSubType != 'U') &&(ViewForFitSubType != 'V')) {
      if (myRank == 0) std::cerr << " BTFit2ndOrder, Requesting optimization of an inexistant sensor view " 
                                 << ViewForFitSubType << " fatal That is it " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    //
    // Get the data, a set of SSD Cluster. 
    
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
      if ((strictSt6Y) && (strictSt6X))   {
         aFileDescr = std::string("_5St_try9_AlignUV_GenCompactA5_V1e.dat");
	 if (G4EMPHDescrToken != std::string("none")) {
//	   aFileDescr = std::string("_5St_try9_AlignUV_") + G4EMPHDescrToken + std::string("_V1e.dat");  
	   aFileDescr = std::string("_") + G4EMPHDescrToken + std::string("_V1e.dat");  
	 }
      } 
    }
    if (runNum == 1366)  aFileDescr = std::string("_TgtGraphite120Gev_BrickV2_1o1d_V1f.dat");
    if ((runNum == 1366) && (token.find("Try3D_R1366_2a") != std::string::npos))  aFileDescr = std::string("_TgtGraphite120Gev_BrickV2_NA_V1f.dat");
    if ((runNum == 1366) && (token.find("Try3D_R1366_3a") != std::string::npos))  aFileDescr = std::string("_TgtGraphite120Gev_BrickV2_NA2C_V1f.dat");
    if ((runNum == 1366) && (token.find("Try3D_R1366_4a") != std::string::npos))  aFileDescr = std::string("_TgtGraphite120Gev_BrickV2_NotPreAligned_1o1g_V1g.dat");
    if ((runNum == 1366) && 
        ((token.find("Try3D_R1366_4b") != std::string::npos) || 
	 (token.find("Try3D_R1366_4c") != std::string::npos) || 
	 (token.find("Try3D_R1366_4d") != std::string::npos) ||
	 (token.find("Try3D_R1366_4e") != std::string::npos))) 
	     aFileDescr = std::string("_TgtGraphite120Gev_BrickV2_1o1g_V1g.dat");
    if ((runNum == 1043) && ((token.find("TightCl_1a") != std::string::npos) ||
                             (token.find("TightCl_1b") != std::string::npos)))  aFileDescr = std::string("_5St_try9_AlignUV_TightCluster_1a_V1g.dat");
    if ((runNum == 1043) && (token.find("TightCl_1c") != std::string::npos))  aFileDescr = std::string("_5St_try9_AlignUV_TightCluster_1b_V1g.dat");
    if ((runNum == 1293) && (token.find("TgtMag09") != std::string::npos))  aFileDescr = std::string("_SimProtonMisA1TgtMag09a_V1g.dat");
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
     if ((strictSt6X)  && (strictSt6Y)) { 
        numExpected = 41321; myBTIn.SetKey(687403); // Only valid for run 1055
	if (G4EMPHDescrToken != std::string("none"))  numExpected = -1; // Variable, too much clerical work with this check. 
	if (runNum == 1366) myBTIn.SetKey(687503);
     }
     if (maxEvts !=  1000000)  numExpected = maxEvts; // a bit of an abuse of variables names... Speed things up.. 
     if (myRank == 0)  {
         std::cerr << " Doing the Fit2ndorder, Simplex,  on file " << aFName << std::endl;
         myBTIn.FillItFromFile(numExpected, aFName.c_str(), selectedSpill);
	 if (aFName.find("SimProtonMisA1TgtMag09a") != std::string::npos) { 
	   std::cerr << " Adding data, different beam spots.. " << std::endl;
	   std::string aFName2(aFName); size_t iP2 = aFName2.find("Mag09a"); aFName2.replace(iP2, 6, std::string("Mag09b"));
	   myBTIn.FillItFromFile(1000000, aFName2.c_str(), selectedSpill);
	   std::string aFName3(aFName); size_t iP3 = aFName3.find("Mag09a"); aFName3.replace(iP3, 6, std::string("Mag09c"));
	   myBTIn.FillItFromFile(1000000, aFName3.c_str(), selectedSpill);
	 }
	 std::cerr << " .... this analysis will be based on " << myBTIn.GetNumEvts() << std::endl;
//	 std::cerr << " And quit for now... " << std::endl; MPI_Finalize(); exit(2);
      }

     emph::rbal::distributeEvts(myBTIn); 
    //
    // Now deal with the parameters.. 
    //
    
//    if (myRank == 0) std::cerr << " ... Instantiating the Geometry...  " << std::endl;
    
    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    if (G4EMPHDescrToken.find("SimProtonNoTgt") != std::string::npos) {
//       std::cerr << " Geometry, G4EMPH (should be unique, though.. " << std::endl;
       myGeo->SetMultScatUncert('X', 1,  0.003201263);  myGeo->SetMultScatUncert('Y', 1,  0.003201263); 
       myGeo->SetMultScatUncert('X', 2, 0.00512);  myGeo->SetMultScatUncert('Y', 2, 0.00512); 
       myGeo->SetMultScatUncert('X', 3, 0.0092);	myGeo->SetMultScatUncert('Y', 3, 0.0092);
       myGeo->SetMultScatUncert('X', 4, 0.0212);	myGeo->SetMultScatUncert('Y', 4, 0.0212);	
       myGeo->SetMultScatUncert('X', 5, 0.0264); myGeo->SetMultScatUncert('Y', 5, 0.0264);	
       myGeo->SetMultScatUncert('U', 0, 0.00512);  
       myGeo->SetMultScatUncert('U', 1, 0.0092);	
       myGeo->SetMultScatUncert('W', 0, 0.0212);  
       myGeo->SetMultScatUncert('W', 1, 0.0212);	
       myGeo->SetMultScatUncert('W', 2, 0.0264);  
       myGeo->SetMultScatUncert('W', 3, 0.0264);	 // clusmy.. typo error prone.. 
       if (G4EMPHDescrToken.find("3a") != std::string::npos)  myGeo->SetMagnetKick120GeV(1.0e-10);
       if (G4EMPHDescrToken.find("4a") != std::string::npos)  myGeo->SetMagnetKick120GeV(1.0e-10);
    }
    //
    if (integrationStepSize > 0.) { 
      if (myRank == 0) std::cerr << " ... Instantiating Magnetic field. " << std::endl;
      emph::rbal::BTMagneticField *myMagField = emph::rbal::BTMagneticField::getInstance();
      if (myRank == 0) {
        double xxx[3]; xxx[0] = 2.3; xxx[1] = 5.6; xxx[2] = 757.; double bField[3];
	myMagField->GetFieldValue(xxx, bField); 
	std::cerr << " Test on the Magnetic field, approximate center By " << bField[0] << "  By " << bField[1] << std::endl;
//	std::cerr << " ..... And quit for now.... " << std::endl; exit(2);
      }
      if (std::abs(magnetShiftX) > 1.0e-3) myMagField->SetReAlignShiftX(magnetShiftX);
      if (std::abs(magnetShiftY) > 1.0e-3) myMagField->SetReAlignShiftY(magnetShiftY);
      if (std::abs(magnetShiftZ) > 1.0e-3) myMagField->SetReAlignShiftZ(magnetShiftZ);
      myGeo->SetIntegrationStepSize(integrationStepSize);
    } 
    emph::rbal::SSDAlignParams *myParams = emph::rbal::SSDAlignParams::getInstance();
    myParams->SetStrictSt6(strictSt6X || strictSt6Y); 
    //
    myParams->SetMode(fitType);
    myParams->SetSoftLimits(doSoftLimits);
    
    bool withGap = (std::abs(assumedDoubleGap) >  1.0e-6);
    if (withGap) { 
      myGeo->SetValueTrShiftLastPlane('X', assumedDoubleGap );  
      myGeo->SetValueTrShiftLastPlane('Y', assumedDoubleGap );  
      myGeo->SetValueTrShiftLastPlane('U', assumedDoubleGap ); // This will be over written..   
      myGeo->SetValueTrShiftLastPlane('W', assumedDoubleGap );  
    }
    if ((fitType == std::string("2DX")) || (fitType == std::string("3D"))) {
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknownUncert('X', k, 0.000000025); // a fraction of the strip.. 
      // 
    }
    
   bool isSetFor120Run1055 = false;
   bool isSetFor30GeVBasedOnRun1055 = false;
   
   if(doPencilBeam120) myGeo->SetUncertErrorOutOfPencilBeam(); 
   if(doRejectVViews) myGeo->SetUnknownUncert('V', 500.);
   //
    // New method, reload from a file... 
    //
    if (doLoadParamsFromPreviousRun) {
      myParams->LoadValueFromPreviousRun(token, true);
    }
    if (doLoadParamsFromPreviousFCNHist) {
      myParams->LoadValueFromPreviousFCNHistory(token, requestFCNCallNumber); // in case the previous is stuck in a Minuit inf loop (ill-posed problem ).  
    }
    
    if (myRank == 0) myParams->DumpTable(token); 
//    if (myRank == 0) std::cerr << " Booby trap, stop now " << std::endl;
//    MPI_Finalize();
//    exit(2);

    ROOT::Minuit2::MnUserParameters uPars;
    for (size_t kPar=0; kPar != myParams->size();  kPar++) { 
      std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
      std::string aName(itP->Name());
      if (aName == scanP1Name) scanP1Index = kPar; if (aName == scanP2Name) scanP2Index = kPar;
      double aValue = itP->Value();
      double err = 0.1*std::abs(itP->Limits().second);
      if (err < 1.0e-10) err = 0.1;
//      if (aName.find("TransShift") != std::string::npos) err = 2.0;
      uPars.Add(aName, aValue, err);
      if (!doSoftLimits) uPars.SetLimits(aName, itP->Limits().first, itP->Limits().second);
    }
    
    
    emph::rbal::BeamTrackSSDAlignFCN theFCN(fitType, &myBTIn);
    theFCN.SetMCFlag(G4EMPHDescrToken != std::string("none"));
    if (magnetIsRemoved) theFCN.SetNoMagnet(true);
    theFCN.SetSoftLimits(doSoftLimits);
    if ((SoftLimitSlopeSigma != 0) && (SoftLimitSlopeSigma != INT_MAX)) theFCN.SetAssumedSlopeSigma(SoftLimitSlopeSigma*1.0e-6);
    theFCN.SetBeamConstraint(applyEmittanceConstraint);
    theFCN.SetNominalMomentum(nominalMomentum);
    theFCN.SetAlignMode(true); // In this context, kind of obvious. 
    theFCN.SetUpLimForChiSq(uPLimitOnChiSqTrackFits);
    //
    //
    // 
    // Dry run, to decide which track we keep.. 
    //
    std::vector<double> parTmp00(myParams->size(), 0.); 
      for (size_t k=0; k != myParams->size(); k++) {
 	 std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(k);
	 parTmp00[k] = itP->Value();
    }
    
    if (doCallFCNOnce) {
      std::vector<double> parTmp1(myParams->size(), 0.); 
      for (size_t k=0; k != myParams->size(); k++) {
 	 std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(k);
	 parTmp1[k] = itP->Value();
      }
      theFCN.SetDebug(true);
      theFCN.SetDumpBeamTracksForR(true);
      std::string aNameSol("./BeamTracksFromOneCall_"); aNameSol += token; aNameSol += std::string("_V1.txt");
      theFCN.SetNameForBeamTracks(aNameSol);
      double chiSqTmp1 = theFCN(parTmp1); 
      std::cerr << " Calling FCN once.. Result from rank " << myRank << " is " << chiSqTmp1 << std::endl;
      //
      // Test reproducibilty of FCN 
      //
      if (myRank == 0) std::cerr << " Furthermore... " << std::endl;
      for (int kTest=0; kTest != 2; kTest++) {
        std::vector<double> parTmpT(myParams->size(), 0.); 
        theFCN.SetDebug(false);
        theFCN.SetDumpBeamTracksForR(false);
        for (size_t k=0; k != myParams->size(); k++) {
 	   std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(k);
	   parTmpT[k] = itP->Value();
	   if (itP->Name().find("TransShift_X_5") != std::string::npos) parTmpT[k] = 0.1*(kTest + 1);
	   if (itP->Name().find("TransShift_X_6") != std::string::npos) {
	     parTmpT[k] = -0.2*(kTest + 1);
             if (myRank == 0) std::cerr << " .... At kTest " << kTest << " ... setting  " << itP->Name() << " to " << parTmpT[k] << std::endl;
	   }
 	}
        double chiSqTmpT = theFCN(parTmpT); 
        if (myRank == 0) std::cerr << " .... At kTest " << kTest << " ...  Result from rank 0 is " << chiSqTmpT << std::endl;
      }
      double chiSqTmp1Redo = theFCN(parTmp1); 
      std::cerr << " .... Back to where we were..on rank  " << myRank << " is " << chiSqTmp1Redo << std::endl;
      std::cerr << " .... And done for good on rank .... " << myRank << std::endl;
      MPI_Finalize(); 
      exit(0);
   } 
   
//    if (myRank == 1) theFCN.SetDebug(true);
    
    ROOT::Minuit2::MnSimplex minSimple(theFCN, uPars);
    
    if (myRank == 0) std::cerr << " ... On Rank0, deciding which parameters are fixed.. numPars " 
                                 << myParams->size() << " for fitSubType " << fitSubType << std::endl;
        
    if ((!doPencilBeam120) && (!doAntiPencilBeam120)) myParams->SetMinuitParamFixes(fitSubType, 0);
    if (doPencilBeam120) myParams->SetMinuitParamFixes(fitSubType, 1);
    if (doAntiPencilBeam120) myParams->SetMinuitParamFixes(fitSubType, -1);
    if (doRejectVViews) myParams->FixParamsForView('V');
    if (doAntiPencilBeam120OnlyV && doAntiPencilBeam120) {
       myParams->FixParamsForView('X');
       myParams->FixParamsForView('Y');
       myParams->FixParamsForView('U');
       if (fitSubType == "TrRollCenterShift") {
         myParams->FixParamsForView('V', false, std::string("TransShift"));
         myParams->FixParamsForView('V', false, std::string("DeltaRoll"));
         myParams->FixParamsForView('V', false, std::string("DeltaRollCenter"));
	 // Adding the V_3.. Poorly set in the 120 alignment.. 
	 myParams->FixMinuitParam(emph::rbal::TRSHIFT, 'V', 3, false);
	 myParams->FixMinuitParam(emph::rbal::ROLL, 'V', 3, false);
	 myParams->FixMinuitParam(emph::rbal::ROLLC, 'V', 3, false);
       }
    }
    for (std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->ItCBegin(); itP != myParams->ItCEnd(); itP++) {
         if (itP->isFixedInMinuit()) {
	   minSimple.Fix(itP->Name().c_str());
	 } else { 
	   if (myRank == 0)  
	      std::cerr << "  ..... on rank 0 Varying parameter " << itP->Name() << std::endl;
       }
     }
//     std::cerr << " quit for now.... " << std::endl;
//     MPI_Finalize(); exit(2);
     // Requested Scans..  ? My own, based on Limits.. 
     // One D scan.. Some cloned code... 
     if (myRank == 0) theFCN.OpenChiSqHistoryFile(token);
     if ((scanP1Index < static_cast<unsigned int>(myParams->size())) 
           && scanP2Index == INT_MAX) {
        std::vector<emph::rbal::SSDAlignParam>::const_iterator itP1 = myParams->It(static_cast<size_t>(scanP1Index));
	 // !!by Minuit Parameters numberr. See printed list.. 
         if (myRank == 0) std::cerr << " Requesting Scan..1D...  For parameter " << itP1->Name() 
	     << " Number of calls if FCN history file, so far " << theFCN.NCalls() <<std::endl;
	 std::vector<double> tmpPars(myParams->size(), 0.); size_t kp = 0;
	 std::pair<double, double> limitsP1 =  itP1->Limits();
	 double range1 = limitsP1.second - limitsP1.first;
	 double start1 = limitsP1.first;	 
	 for (size_t kp=0; kp != myParams->size(); kp++) {
           std::vector<emph::rbal::SSDAlignParam>::const_iterator itPC = myParams->It(kp);
	   tmpPars[kp] = itPC->Value();
	 }
	 double delta1 = range1/nScanPts;
	 std::ofstream aFoutScan;
	 if (myRank == 0) {
	    std::string aNameScan("./Scan1D_"); aNameScan += token + std::string("_V1.txt");
	    aFoutScan.open(aNameScan.c_str());
	    aFoutScan << " k " << itP1->Name() << " chiSq " << std::endl;
	 }
	 int ncc = 0;
	 for (int iSc1=0; iSc1 != nScanPts; iSc1++) {
	   tmpPars[scanP1Index] =  start1 + iSc1*delta1;
	   double chiSqTmp = theFCN(tmpPars);
	   if (myRank == 0) 
	       aFoutScan  << " " << ncc << " " 
	                                << tmpPars[scanP1Index] <<  " " << chiSqTmp << std::endl;
					// Result also available in FCN History file. 
	     ncc++;
	   }
         if (myRank == 0) { aFoutScan.close(); std::cerr << " Done with 1D Scan" << itP1->Name()
	        << " Number of calls if FCN history file " << theFCN.NCalls() <<  " and bail out.. " << std::endl; }
	 MPI_Finalize(); 
	 exit(2); 
     }
     
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
	   tmpPars[kp] = itPC->Value();
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
     //
     // Minimize
     //
      if (myRank == 0) std::cerr << " ... BTFitSimplex main, About to call minSimple operator to minimize, with nMax " 
                                 <<  numMaxFCNCalls << " and EdmMaxTolerance " << EdmMaxTolerance << std::endl;
      ROOT::Minuit2::FunctionMinimum min = minSimple(numMaxFCNCalls, EdmMaxTolerance); // more forgiving... 
      // We must synchronize here, as we will be collecting all the tracks.. 
      MPI_Barrier(MPI_COMM_WORLD);
      
      bool myMinSimpleOK = min.IsValid();
      if (myRank == 0) {
        std::cerr << min << std::endl;
        std::string fNameMinRes("./MinValues_Simplex");
	if (myMinSimpleOK) fNameMinRes += std::string("_OK_"); else fNameMinRes += std::string("_NotOK_");  
	fNameMinRes += token + std::string(".txt");	 
        std::ofstream fOutMinRes(fNameMinRes.c_str());
        for (size_t kPar=0; kPar != myParams->size();  kPar++) {
          std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
          std::string aName(itP->Name());
          double theValue  = min.UserState().Value(aName);
          double theError  = min.UserState().Error(aName);
          fOutMinRes << " " << aName << " " << theValue << " " << theError << std::endl;
	  // Add the extra gap for the last plane, Y view, which defines the reference frame.  Phase1b only. 
	  if (aName.find("Tilt_Y_6") != std::string::npos) fOutMinRes << " TransShift_Y_7" << " " << assumedDoubleGap << " 1.0e-8 " << std::endl;
        }
        fOutMinRes.close();
      }
       
       //
       // Save the set of track for this solution..Even if flaky..  
       //
      std::vector<double> parsSol(myParams->size(), 0.);
      for (size_t kP=0; kP != myParams->size(); kP++) parsSol[kP] = min.UserState().Value(kP);
      theFCN.SetDumpBeamTracksForR(true);
      std::string aNameSol("./BeamTracksFromMin_"); 
       if (myMinSimpleOK) aNameSol += std::string("_OK_"); else aNameSol += std::string("_NotOK_");  
      aNameSol += token; aNameSol += std::string("_V1.txt");
      theFCN.SetNameForBeamTracks(aNameSol);
      double chiSol = theFCN(parsSol);
      theFCN.SetDumpBeamTracksForR(false);
      if (myRank == 0) std::cerr << " Bailing out after simplex... " << std::endl;
      MPI_Finalize();
      exit(2); 
      std::cerr << " Done, calling MPI_Finalze from " << myRank << std::endl;
      MPI_Finalize();
}
