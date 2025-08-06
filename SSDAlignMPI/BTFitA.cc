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
#include "BeamTracks.h"
#include "myMPIUtils.h"
//
// Attempting to to the SSD alignment over a sample of track, including, Pitch, Yaw roll angles, deltaZ, transverse positions.
// In this main, just ditribute the event and check them.  
// 

emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr=NULL; 

int main(int argc, char **argv) {
//
   
   double dumVar = 0; // place holder.. 
   
   std::string fitType("2DY");
   int maxEvts = 1000000;
   size_t kSeTrShifted = INT_MAX;
   std::string viewShifted("none");
   double trShift = 0.;
   int selectedSpill = INT_MAX;
   bool doMigrad = false;
   std::string token("none");
   
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int aRank = world_size - 1;

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    if (myRank == 0) std::cout << " Job argument decoding, number of args  "  << argc << std::endl;
    if (argc > 1) { 
      int argcR = argc - 1;
      for (int kArg = 0; kArg != argcR/2; kArg++) {
        std::string parStr(argv[1+2*kArg]);
        std::string valStr(argv[2+2*kArg]);
        if (myRank == 0) std::cout << " parStr " << parStr << " valStr " << valStr << std::endl;
        std::string valStrcpy(valStr);
        for (size_t ii=0; ii != valStrcpy.length(); ii++) if (valStrcpy[ii] == '.') valStrcpy[ii] ='p';
        std::istringstream valStrStr(valStr);
        if (parStr.find("fitType") != std::string::npos) {
           fitType = valStr;
          if (myRank == 0) std::cout << " The request fit type will be   "  << fitType << std::endl;
        } else if (parStr.find("maxEvt") != std::string::npos) {
          valStrStr >> maxEvts;
          if (myRank == 0) std::cout << " Requested number of events  "  << maxEvts << std::endl;
        } else if (parStr.find("spill") != std::string::npos) {
          valStrStr >> selectedSpill;
          if (myRank == 0) std::cout << " Requested spill  "  << selectedSpill << std::endl;
        } else if (parStr.find("viewShifted") != std::string::npos) {
          valStrStr >> viewShifted;
          if (myRank == 0) std::cout << " Requested Sensor view for transverse shift  "  << kSeTrShifted << std::endl;
         } else if (parStr.find("TrShiftSensor") != std::string::npos) {
          valStrStr >> kSeTrShifted;
          if (myRank == 0) std::cout << " Requested Sensor index for motion shift  "  << kSeTrShifted << std::endl;
       } else if (parStr.find("trShift") != std::string::npos) {
          valStrStr >> trShift;
          if (myRank == 0) std::cout << " Requested transverse shifted value  "  << trShift << std::endl;
       } else if (parStr.find("doMigrad") != std::string::npos) {
          int aVal;
          valStrStr >> aVal;
	  doMigrad = (aVal == 1);
          if (doMigrad && (myRank == 0))  std::cout << " We will do Migrad  for the event fits " << std::endl;
          if ((!doMigrad) && (myRank == 0))  std::cout << " We will NOT do Migrad  for the event fits " << std::endl;
       } else if (parStr.find("token") != std::string::npos) {
          token = valStr;
          if (myRank == 0) std::cout << " Token will be   "  << token << std::endl;
       } else {
          if (myRank == 0) std::cout << " Unrecognized argument   "  << parStr <<  " fatal, quit here and now " << std::endl;
          MPI_Finalize();
          exit(2);
        }
       }   
    }
    
    if ((fitType != std::string("2DX")) && (fitType != std::string("2DY")) && (fitType != std::string("3D")) ) {
      std::cerr << " BTFitA, unrecognized fit type " << fitType << " fatal, quit here and now " << std::endl;
      MPI_Finalize();
      exit(2);
    }
    
    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    //
    // Setting of the transverse off set previously found.. 
    //
    myGeo->SetDeltaTr('X', 1, 1.585286);   
    myGeo->SetDeltaTr('X', 2, 1.697424);
    myGeo->SetDeltaTr('X', 3,  2.983219);
    myGeo->SetDeltaTr('X', 4,  -1.495283); 
//    myGeo->SetDeltaTr('Y', 1, -0.5442576);   
//    myGeo->SetDeltaTr('Y', 2, -1.3555225);
//    myGeo->SetDeltaTr('Y', 3, -1.7229351);
//    myGeo->SetDeltaTr('Y', 4, -0.6333689);
    myGeo->SetDeltaTr('Y', 1, -0.403086);   
    myGeo->SetDeltaTr('Y', 2, -1.62367);
    myGeo->SetDeltaTr('Y', 3, -2.168  );
    myGeo->SetDeltaTr('Y', 4, -5.0); // At limit in the 2DY Trshifts Minos fit.  
    myGeo->SetDeltaTr('Y', 5, -2.00445);
    myGeo->SetDeltaTr('Y', 6, -5.0);
    //
    // New values, from 2nd order fit, all parameters varying (except Rolls). 
     myGeo->SetDeltaTr(  'Y', 1,  -0.43052  );  //Starts to make sense.. 
     myGeo->SetDeltaTr(  'Y', 2,  -1.64884 );  
     myGeo->SetDeltaTr(  'Y', 3,  -2.22319 );  
     myGeo->SetDeltaTr(  'Y', 4, -5.  ); // large, but very few statistic for 120 GeV beam.  
     myGeo->SetDeltaTr(  'Y', 5, -2.03585  ); 
     myGeo->SetDeltaTr(  'Y', 6,   -5.0        ); // at limit large, but very few statistic for 120 GeV beam. 
      // Now that is stable.. 
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknownUncert('Y', k, 0.0000004); // Turn it off! 
      myGeo->SetUnknownUncert('Y', 4, 5.0);  myGeo->SetUnknownUncert('Y', 6, 5.0); // again, suspicious track that are widely deflected.  

    
    if (kSeTrShifted != INT_MAX) myGeo->SetDeltaTr(viewShifted[0], kSeTrShifted, trShift);
    
   //
   // 
    std::string topDirAll("/home/lebrun/EMPHATIC/DataLaptop/");
    std::string myHostName(std::getenv("HOSTNAME"));
    if (myHostName.find("fnal") != std::string::npos) topDirAll = std::string("/work1/next/lebrun/EMPHATIC/Data/");
    std::string aFName(topDirAll);  aFName += std::string("CompactAutreData_1055_5St_try9_AlignUV_GenCompactA1_V1b.dat");
     
    struct timeval tvStart, tvStop, tvEnd;
    char tmbuf[64];
    gettimeofday(&tvStart,NULL);
    time_t nowTimeStart = tvStart.tv_sec;
    struct tm *nowtm = localtime(&nowTimeStart);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
    std::string dateNow(tmbuf);
    std::cout << ".... from processor rank " << myRank << " out of " << world_size 
    	      << " option " << token <<  " starting at " << dateNow << std::endl;
	      
     emph::rbal::BTAlignInput myBTIn;
     int numExpected = 67272; // I know this number from running SSDAlign Stu1 Autre on run 1055. 
     if (myRank == 0)  {
         myBTIn.FillItFromFile(numExpected, aFName.c_str(), selectedSpill);
	 std::cerr << " .... this analysis will be based on " << myBTIn.GetNumEvts() << std::endl;
      }

     emph::rbal::distributeEvts(myBTIn); 
    
     emph::rbal::BeamTracks myBTrs; 
     size_t iEvt = 0; 
     for (std::vector<emph::rbal::BeamTrackCluster>::const_iterator it = myBTIn.cbegin(); it != myBTIn.cend(); it++) {
       if (!it->Keep()) continue; 
       emph::rbal::BeamTrack aTr;
       if (doMigrad) aTr.SetDoMigrad(doMigrad);
       aTr.SetDebug((iEvt < 5));
       if (fitType == std::string("2DY")) { 
         aTr.doFit2D('Y', it); 
       } else if (fitType == std::string("2DX")) aTr.doFit2D('X', it); 
       else if (fitType == std::string("3D")) aTr.doFit3D(it);
       myBTrs.AddBT(aTr);
       iEvt++;
       if (iEvt > maxEvts) break;
     }
     /* 
      This was to check to collectBeamTrack algorithm.. 
     if (myRank == 1) {  // temporary, may something badly messed up in the collect.. 
     
         myBTrs.DumpForCVS("./BFitA_TmpDebug_Rank1.txt");
     }
     */
     emph::rbal::collectBeamTracks(myBTrs);

     if (myRank == 0) {
       std::string fNStr("./BFitA_"); fNStr += token + std::string("_") + fitType + std::string("_V1.txt");
       myBTrs.DumpForCVS(fNStr.c_str());
       std::cerr << " Analysis of Beam Tracks, number of them " << myBTrs.size() << std::endl;
       std::cerr << " Summary of Residuals for fitType " << fitType << " NumValid Chi " << myBTrs.NumValidChiSq() << "  SumChiSq " 
                 << myBTrs.SumChiSq() << " mean, up lim = 100. " << myBTrs.MeanChiSq(100.) << std::endl;  // chiSq now better, TrShift global fit 
       if (fitType == std::string("3D")) std::cerr << " --------------- For X view " << std::endl;
       if ((fitType == std::string("2DX")) || (fitType == std::string("3D")))  {
         for (size_t kSe =1; kSe != 8; kSe++) {
	   std::cerr << " Sensor " << kSe << " mean " << myBTrs.MeanResid(fitType, 'X', kSe) 
	             << " RMS " << myBTrs.RmsResid(fitType, 'X', kSe) << std::endl;
	 }
       }
       if (fitType == std::string("3D")) std::cerr << " --------------- For Y view " << std::endl;
       if ((fitType == std::string("2DY")) || (fitType == std::string("3D"))) {
          for (size_t kSe =1; kSe !=8; kSe++) {
	   std::cerr << " Sensor " << kSe << " mean " << myBTrs.MeanResid(fitType, 'Y', kSe) 
	             << " RMS " << myBTrs.RmsResid(fitType, 'Y', kSe) << std::endl;
	 }
       }
       if (fitType == std::string("3D")) {
         std::cerr << " --------------- For U view " << std::endl; 
         for (size_t kSe =1; kSe !=2; kSe++) {
	   std::cerr << " Sensor " << kSe << " mean " << myBTrs.MeanResid(fitType, 'U', kSe) 
	             << " RMS " << myBTrs.RmsResid(fitType, 'U', kSe) << std::endl;
	 }
         std::cerr << " --------------- For V view " << std::endl; 
         for (size_t kSe = 1; kSe !=4; kSe++) {
	   std::cerr << " Sensor " << kSe << " mean " << myBTrs.MeanResid(fitType, 'V', kSe) 
	             << " RMS " << myBTrs.RmsResid(fitType, 'V', kSe) << std::endl;
	 }
       }
     } // rank 0 
   //
    
     std::cerr << " Done, calling MPI_Finalze from " << myRank << std::endl;
     MPI_Finalize();
     
}
