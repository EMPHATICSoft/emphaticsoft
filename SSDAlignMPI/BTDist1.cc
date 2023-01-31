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

#include "BTAlignInput.h"
#include "myMPIUtils.h" 
//
// Attempting to to the SSD alignment over a sample of track, including, Pitch, Yaw roll angles, deltaZ, transverse positions.
// In this main, just ditribute the event and check them.  
// 

int main(int argc, char **argv) {
//
   
   double dumVar = 0; // place holder.. 
   std::string token("none");
   
   bool strictSt6 = false;
   
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    if (argc < 3) {
      std::cerr << " ./BTDist MPI version. Need at least two arguments, such at token  and value argument. Fatal   " << std::endl;
      MPI_Finalize(); 
      exit(2);
    }
    if (myRank == 0) std::cerr << " Job argument decoding, number of args  "  << argc << std::endl;
    int argcR = argc - 1;
    bool pCenterOptUsed = false; 
    bool pRangeOptUsed = false; 
    bool numPhaseMinOptUsed = false;
    int selectedSpill = INT_MAX;
    double sleepFact = 0.000000022; // sleep that amount of time per event, to avoid clogging i/o.  
    // Not needed, it was the removal of BeamTrackCluster objects  that got slow. 
    int perfTrans = 0;
    
    for (int kArg = 0; kArg != argcR/2; kArg++) {
      std::string parStr(argv[1+2*kArg]);
      std::string valStr(argv[2+2*kArg]);
      if (myRank == 0) std::cerr << " parStr " << parStr << " valStr " << valStr << std::endl;
      std::string valStrcpy(valStr);
      for (size_t ii=0; ii != valStrcpy.length(); ii++) if (valStrcpy[ii] == '.') valStrcpy[ii] ='p';
      std::istringstream valStrStr(valStr);
      if (parStr.find("token") != std::string::npos) {
        valStrStr >> token;
        if (myRank == 0) std::cerr << " token is  "  << token << std::endl;
      } else if (parStr.find("strictSt6") != std::string::npos) {
        int iS=0;
        valStrStr >> iS;
	strictSt6 = (iS == 1);
        if (myRank == 0) {
	  if (strictSt6) std::cerr << " We will require 6 Y view hits, no more, no less  "  << std::endl;
	  else std::cerr << " Allowing for one missing hits in the Y view, includes less illuminated station 4 & 5 sensors  "  << std::endl;
	}  
      } else if (parStr.find("spill") != std::string::npos) {
          valStrStr >> selectedSpill;
          if (myRank == 0) std::cerr << " Requested spill  "  << selectedSpill << std::endl;
       } else if (parStr.find("sleepFact") != std::string::npos) {
          valStrStr >> sleepFact;
          if (myRank == 0) std::cerr << " Requested spill  "  << selectedSpill << std::endl;
       } else if (parStr.find("perfTrans") != std::string::npos) {
          valStrStr >> perfTrans;
          if (myRank == 0) std::cerr << " perfTrans flag   "  << perfTrans << std::endl;
     } else {
        if (myRank == 0) std::cerr << " Unrecognized argument   "  << parStr <<  " fata, quit here and now " << std::endl;
        MPI_Finalize();
        exit(2);
      }
    }
       
    std::string topDirAll("/home/lebrun/EMPHATIC/DataLaptop/");
    std::string myHostName(std::getenv("HOSTNAME"));
    if (myHostName.find("fnal") != std::string::npos) topDirAll = std::string("/work1/next/lebrun/EMPHATIC/Data/");

    std::string aFName(topDirAll);  
    if (!strictSt6) aFName += std::string("CompactAlgo1Data_1055_5St_try9_AlignUV_GenCompactA1_V1b.dat");
    else aFName += std::string("CompactAlgo1Data_1055_5St_try9_AlignUV_GenCompactA1_V1c.dat");
     
    struct timeval tvStart, tvStop, tvEnd;
    char tmbuf[64];
    gettimeofday(&tvStart,NULL);
    time_t nowTimeStart = tvStart.tv_sec;
    struct tm *nowtm = localtime(&nowTimeStart);
    strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
    std::string dateNow(tmbuf);
    std::cerr << ".... from processor " << std::string(processor_name) << " rank " << myRank << " out of " << world_size 
    	      << " token " << token <<  " starting at " << dateNow << std::endl;
	      
     emph::rbal::BTAlignInput myBT;
     
     int numExpected = 67272; // I know this number from running SSDAlign Stu1 Algo1 on run 1055. if strictSt6 = false
     if (strictSt6) { numExpected = 58586; myBT.SetKey(687401); }
     
     if (myRank == 0) myBT.FillItFromFile(numExpected, aFName.c_str(), selectedSpill);

    if (perfTrans == 1) { // Obsolete.. 
      int nRemoved = 0;
      for (std::vector<emph::rbal::BeamTrackCluster>::const_iterator it=myBT.cbegin(); it != myBT.cend(); it++) {
//    	    myBT.RemoveFirst();
            it->DoNotUse();
	    nRemoved++;
	    if (nRemoved == myBT.GetNumEvts()) break;
       }
       gettimeofday(&tvStop,NULL);
       time_t nowTimeStop = tvStop.tv_sec;
       struct tm *nowtm = localtime(&nowTimeStart);
       strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
       std::string dateNow(tmbuf);
       std::cerr << ".... done at " << dateNow << std::endl;
       MPI_Finalize(); 
       exit(2);
     }
	      
	            
     emph::rbal::distributeEvts(myBT, sleepFact); 
     //
     // test.. 
     //
     std::cerr << " Number of events for rank " << myRank << " is " << myBT.GetNumEvts() << std::endl;
     if (myRank < 5) {
        myBT.DumpCVSForR(myRank, 'Y', token);
     }
     gettimeofday(&tvEnd,NULL);
     time_t nowTimeEnd = tvEnd.tv_sec;      
     char tmbuf2[64];
     struct tm *nowtmEnd = localtime(&nowTimeEnd);
     strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
     std::string dateNowEnd(tmbuf2);
     std::cerr << " Done, before MPI_Finalize, for rank " << myRank  << " time is " <<  dateNowEnd << std::endl;
     MPI_Finalize();
     
}
