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
#include "SSDAlignParams.h"
//
// Attempting to to the SSD alignment over a sample of track, including, Pitch, Yaw roll angles, deltaZ, transverse positions.
// In this main, just ditribute the event and check them.  
// 


emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr=NULL; 
emph::rbal::SSDAlignParams* emph::rbal::SSDAlignParams::instancePtr=NULL; 

int main(int argc, char **argv) {
//
   
   double dumVar = 0; // place holder.. 
   
   char aView = 'Y';
   size_t aSensor = 1;
   size_t aStation = 0;
   
   std::string optMode("2DY");
   
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    if (world_size != 1) {
      std::cerr << " No need to test with more than one process.. Quit here " << std::endl; MPI_Finalize(); exit(2); 
    }
    
    std::cerr << " Job argument decoding, number of args  "  << argc << std::endl;
    if (argc > 1) { 
      int argcR = argc - 1;
      for (int kArg = 0; kArg != argcR/2; kArg++) {
        std::string parStr(argv[1+2*kArg]);
        std::string valStr(argv[2+2*kArg]);
         std::cout << " parStr " << parStr << " valStr " << valStr << std::endl;
        std::string valStrcpy(valStr);
        for (size_t ii=0; ii != valStrcpy.length(); ii++) if (valStrcpy[ii] == '.') valStrcpy[ii] ='p';
        std::istringstream valStrStr(valStr);
        if (parStr.find("view") != std::string::npos) {
           aView = valStr[0];
           std::cout << " The request view will be   "  << aView << std::endl;
        } else if (parStr.find("Sensor") != std::string::npos) {
          valStrStr >> aSensor;
           std::cout << " Requested Sensor is  "  << aSensor << std::endl;
        } else if (parStr.find("Station") != std::string::npos) {
          valStrStr >> aStation;
           std::cout << " Requested Station is "  << aStation << std::endl;
        } else if (parStr.find("Mode") != std::string::npos) {
          optMode = valStr;
           std::cout << " Requested Mode  is  "  << optMode << std::endl;
       } else {
           std::cout << " Unrecognized argument   "  << parStr <<  " fata, quit here and now " << std::endl;
          MPI_Finalize();
          exit(2);
        }
       }   
    }
    
//    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    emph::rbal::SSDAlignParams* myPs =  emph::rbal::SSDAlignParams::getInstance();
    
    std::cerr << " Got List of parameters.. By default  " << myPs->NumParams() << std::endl;
    
    myPs->SetMode(optMode); 
    
   std::cerr << " After changing the mode.. number of Parameters  " << myPs->NumParams() << std::endl;
    
    const double aval = 0.234; 
    
    myPs->SetValue(emph::rbal::TRSHIFT, aView, aSensor, aval);  
    
    const double valTrShift = myPs->Value(emph::rbal::TRSHIFT, aView, aSensor); 
    if (aval == valTrShift) std::cerr << " Setting and retrieving value semms to work.. value " << valTrShift << std::endl;
    else std::cerr << " Setting and retrieving value semms fails, set " << aval << " retrieved   " << valTrShift << std::endl;
    
    std::cerr << " Produce stable table .. " << std::endl;
    std::string token("test");
     myPs->DumpTable(token);
    
     std::cerr << " Done, calling MPI_Finalze " << std::endl;
     MPI_Finalize();
     
}
