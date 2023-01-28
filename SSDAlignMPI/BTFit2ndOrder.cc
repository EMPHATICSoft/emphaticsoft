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
   int SensorForFitSubType(INT_MAX); // Valid strings are "All" (nothing fixed), TRShift, PitchCorr, 
   int maxEvts = 1000000;
   size_t kSeTrShifted = INT_MAX;
   std::string viewShifted("none");
   double trShift = 0.;
   int selectedSpill = INT_MAX;
   std::string token("none");
   bool doCallFCNOnce = false; // for debugging purpose.. One cll to FCCN and that is it. 
   bool doMinos = true;
   unsigned int  contourP1Index = INT_MAX;
   unsigned int  contourP2Index = INT_MAX;
   int nContourPts = 100;
   unsigned int  scanP1Index = INT_MAX;
   unsigned int  scanP2Index = INT_MAX;
   int nScanPts = 50;
     
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
      std::cerr << " BTFit2ndOrder, unrecognized fit type " << fitType << " fatal, quit here and now " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    //
    // Get the data, a set of SSD Cluster. 
    // 
    
    std::string topDirAll("/home/lebrun/EMPHATIC/DataLaptop/");
    std::string myHostName(std::getenv("HOSTNAME"));
    if (myHostName.find("fnal") != std::string::npos) topDirAll = std::string("/work1/next/lebrun/EMPHATIC/Data/"); // On fnal Wilson
    std::string aFName(topDirAll);  aFName += std::string("CompactAlgo1Data_1055_5St_try9_AlignUV_GenCompactA1_V1b.dat");
     
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
     if (myRank == 0)  {
         myBTIn.FillItFromFile(numExpected, aFName.c_str(), selectedSpill);
	 std::cerr << " .... this analysis will be based on " << myBTIn.GetNumEvts() << std::endl;
      }

     emph::rbal::distributeEvts(myBTIn); 
    //
    // Now deal with the parameters.. 
    //
    
    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    emph::rbal::SSDAlignParams *myParams = emph::rbal::SSDAlignParams::getInstance();
    //
    myParams->SetMode(fitType);
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
      // Now that is stable.. 
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('Y', k, 0.0000004); // Turn it off! 
      myGeo->SetUnknwonUncert('Y', 4, 5.0);  myGeo->SetUnknwonUncert('Y', 6, 5.0); // again, suspicious track that are widely deflected.  
     
    
    } 
    if ((fitType == std::string("2DX")) || (fitType == std::string("3D"))) {
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1,  1.585286); 
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2,  1.697424);  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3,  2.983219);  
//      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -1.495283); 
//
//   2n try , reducing the error .  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1,  1.68684 ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2,  2.5173  );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3,  4.18659 );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -5.0 ); // Poor statistics.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 5,  -0.279456 );
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 6,  -5.0 );
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('X', k, 0.030); // Only ~ 2 strips, in average.. 
      myGeo->SetUnknwonUncert('X', 4, 5.0);  myGeo->SetUnknwonUncert('X', 6, 5.0); // again, suspicious track that are widely deflected.  
//   3rd (or 4rth)   try , reducing the error .  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 1, 1.70846   ); 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 2, 2.57212  );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 3, 4.25464  );  
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 4,  -5.0 ); // Poor statistics.. 
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 5, -0.320193  );
      myParams->SetValue(emph::rbal::TRSHIFT, 'X', 6,  -5.0 );
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknwonUncert('X', k, 0.025); // Only ~ 2 strips, in average.. 

    } 
    
    if (myRank == 0) myParams->DumpTable(token); 
//    if (myRank == 0) std::cerr << " Booby trap, stop now " << std::endl;
//    MPI_Finalize();
//    exit(2);

    ROOT::Minuit2::MnUserParameters uPars;
    size_t kPar = 0;
    for (size_t kPar=0; kPar != myParams->size();  kPar++) { 
      std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
      std::string aName(itP->Name());
      double aValue = itP->Value();
      double err = 0.1*std::abs(itP->Limits().second);
      if (err < 1.0e-10) err = 0.1;
      uPars.Add(aName, aValue, err);
      uPars.SetLimits(aName, itP->Limits().first, itP->Limits().second);
    }
    
    
    emph::rbal::BeamTrackSSDAlignFCN theFCN(fitType, &myBTIn);
    if (doCallFCNOnce) {
      std::vector<double> parTmp(myParams->size(), 0.); 
      for (size_t k=0; k != myParams->size(); k++) {
 	 std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->It(kPar);
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
	 if ((fitSubType == std::string("TrShift")) && 
	     (SensorForFitSubType == INT_MAX) && (aName.find("TransShift") == 0)) isFixed = false;
	 if ((fitSubType == std::string("ZShift")) && 
	     (SensorForFitSubType == INT_MAX) && (aName.find("LongShift") == 0)) isFixed = false;
	 if ((fitSubType == std::string("PitchCorr")) && 
	     (SensorForFitSubType == INT_MAX) && (aName.find("Tilt") == 0)) isFixed = false;
	 if ((fitSubType == std::string("DeltaRoll")) && 
	     (SensorForFitSubType == INT_MAX) && (aName.find("Roll") == 0)) isFixed = false;
	 // Specific sensors. 
	 // Find the sensor in the struct,  
	 if ((fitSubType == std::string("TrShift")) && 
	     (SensorForFitSubType == itP->SensorI()) && (aName.find("TransShift") == 0)) isFixed = false;
	 if ((fitSubType == std::string("ZShift")) && 
	     (SensorForFitSubType == itP->SensorI()) && (aName.find("LongShift") == 0)) isFixed = false;
	 if ((fitSubType == std::string("PitchCorr")) && 
	     (SensorForFitSubType == itP->SensorI()) && (aName.find("Tilt") == 0)) isFixed = false;
	 if ((fitSubType == std::string("DeltaRoll")) && 
	     (SensorForFitSubType == itP->SensorI()) && (aName.find("Roll") == 0)) isFixed = false;
	 if (((fitSubType == std::string("MagnetZPos")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("LongMagC") == 0)) isFixed = false;   
	 if (((fitSubType == std::string("MagnetKick")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("KickMag") == 0)) isFixed = false;
	 if (((fitSubType == std::string("TrShiftMagnetKick")) || (fitSubType == std::string("MagnetZPosKick"))) &&
	      (aName.find("KickMag") == 0)) isFixed = false;
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
     if (!min.IsValid()) {
       if (myRank == 0) {
         std::cerr << "  ... On rank 0, Minimum from Migrad is invalid... " << std::endl;
	 std::cerr << min << std::endl;
//	 theFCN.CloseChiSqHistoryFile();
       }
       MPI_Finalize();
       exit(2);
     } else {
       
       if (myRank == 0) {
         std::cerr << "  ... On rank 0, Minimum from Migrad has been delcared valid... " << std::endl;
         gettimeofday(&tvStop,NULL);
         time_t nowTimeStop = tvStop.tv_sec;
         struct tm *nowtm = localtime(&nowTimeStop);
         strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
         std::string dateNow(tmbuf);
         std::cerr << "  ... On rank 0, Minimum from Migrad has been declared valid...time  " << dateNow << std::endl;
         std::cerr << " The minimum is " << min << std::endl;
       }
     }
     // Requested contours ?
     if (min.IsValid() && (contourP1Index < static_cast<unsigned int>(myParams->size())) 
           && (contourP2Index < static_cast<unsigned int>(myParams->size()))) {
       
       ROOT::Minuit2::MnContours contour(theFCN, min);
       std::vector<std::pair<double, double> > resContour = contour(contourP1Index, contourP2Index, nContourPts); 
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
     }
     // Requested Scans..  ? My own, based on Limits.. 
     if ((scanP1Index < static_cast<unsigned int>(myParams->size())) 
           && (scanP2Index < static_cast<unsigned int>(myParams->size()))) {
        std::vector<emph::rbal::SSDAlignParam>::const_iterator itP1 = myParams->It(static_cast<size_t>(scanP1Index));
        std::vector<emph::rbal::SSDAlignParam>::const_iterator itP2 = myParams->It(static_cast<size_t>(scanP2Index));
        if (myRank == 0) std::cerr << " Requesting Scans..For parameters " << itP1->Name()
	       << " and " << itP2->Name() << " Number of calls if FCN history file, so far " << theFCN.NCalls() <<std::endl;
	 std::vector<double> tmpPars(myParams->size(), 0.); size_t kp = 0;
	 std::pair<double, double> limitsP1 =  itP1->Limits(); std::pair<double, double> limitsP2 =  itP2->Limits();
	 double range1 = limitsP1.second - limitsP1.first; double range2 = limitsP2.second - limitsP2.first; 
	 double delta1 = range1/nScanPts; double delta2 = range2/nScanPts;
	 for (size_t kp=0; kp != myParams->size(); kp++) {
           std::vector<emph::rbal::SSDAlignParam>::const_iterator itPC = myParams->It(kp);
	   tmpPars[kp] = min.UserState().Value(itPC->Name());
	 }
	 if (myRank == 0) std::cerr << " k " << itP1->Name() << " " << itP2->Name() << " chiSq " << std::endl;
	 int ncc = 0;
	 for (int iSc1=0; iSc1 != nScanPts; iSc1++) {
	   tmpPars[scanP1Index] =  limitsP1.first + iSc1*delta1;
	   for (int iSc2=0; iSc2 != nScanPts; iSc2++) {
	     tmpPars[scanP2Index] =  limitsP2.first + iSc2*delta2;
	     double chiSqTmp = theFCN(tmpPars);
	     if (myRank == 0) std::cerr << " " << ncc << " " 
	                                << tmpPars[scanP1Index] << " " << tmpPars[scanP2Index] << chiSqTmp << std::endl;
					// Result also available in FCN History file. 
	     ncc++;
	   }
	 }
         if (myRank == 0) std::cerr << " Done with Scan" << itP1->Name()
	       << " and " << itP2->Name() << " Number of calls if FCN history file " << theFCN.NCalls() <<std::endl;
    
     }
     std::cerr << " .. ChiSq " << min.Fval() << " Final Values and Minos Errors Num Params " <<  myParams->size() << std::endl;
     //
     // Call minos 
     //
     if (doMinos) { 
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
         if (aName.find("TransShift") == std::string::npos) continue; // This is the uncertainty we reall care about.. 
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
