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
#include "BTAlignGeom1c.h"
//
// Attempting to to the SSD alignment over a sample of track, including, Pitch, Yaw roll angles, deltaZ, transverse positions.
// In this main, just ditribute the event and check them.  
// 


emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr=NULL; 
emph::rbal::BTAlignGeom1c* emph::rbal::BTAlignGeom1c::instancePtr=NULL; 

int main(int argc, char **argv) {
//
   
   double dumVar = 0; // place holder.. 
   
   char aView = '?';
   size_t aSensor = 0;
   size_t aStation = 0;
   
   std::string optStr("none");
   
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int aRank = world_size - 1;

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
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
        if (parStr.find("view") != std::string::npos) {
           aView = valStr[0];
          if (myRank == 0) std::cout << " The request view will be   "  << aView << std::endl;
        } else if (parStr.find("Sensor") != std::string::npos) {
          valStrStr >> aSensor;
          if (myRank == 0) std::cout << " Requested Sensor is  "  << aSensor << std::endl;
        } else if (parStr.find("Station") != std::string::npos) {
          valStrStr >> aStation;
          if (myRank == 0) std::cout << " Requested Station is "  << aStation << std::endl;
         } else if (parStr.find("Rank") != std::string::npos) {
          valStrStr >> aRank;
          if (myRank == 0) std::cout << " Probed rank is  "  << aRank << std::endl;
       } else {
          if (myRank == 0) std::cout << " Unrecognized argument   "  << parStr <<  " fata, quit here and now " << std::endl;
          MPI_Finalize();
          exit(2);
        }
       }   
    }
    
    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    if (myRank == aRank) { 
      std::cerr << " Z Position for sensor " << aSensor << " is " << myGeo->ZPos(aView, aSensor) << std::endl;
      myGeo->SetDeltaZ(aView, aSensor, 0.5);
      std::cerr << " New Z position, shift by +0.5, for sensor  " << aSensor << " is " << myGeo->ZPos(aView, aSensor) << std::endl;
      myGeo->SetDeltaZStation(aView, aStation, 0.25);
      std::cerr << " New Z position, shift by +0.25, for sensor  " << aSensor << " is " <<  myGeo->ZPos(aView, aSensor) << std::endl;
      std::cerr << " Transverse Position for sensor " << aSensor << " is " << myGeo->TrPos(aView, aSensor) << std::endl;
      myGeo->SetDeltaTr(aView, aStation, 0.25);
      std::cerr << " New transverse position, shift by +0.5, for sensor  " << aSensor << " is " <<  myGeo->TrPos(aView, aSensor) << std::endl;
    }
     std::cerr << " Done, calling MPI_Finalze for " << myRank << std::endl;
     MPI_Finalize();
     
}
