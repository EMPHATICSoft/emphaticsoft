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
#include "LightParamLogLike.hh"
#include "LightParamSim.h"
#include <sys/time.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

//
// Attempting simulate the nasty anti-correlation between long (or negative ) electron lifetime in the drift region  
// Using LightParamSim, which is a small destim package. May - June 2022
// 
bool getTheKryptonSampleRun8686(const std::string &token, double xCenter, double yCenter, double radius, LPRecSample &myS); 

void execOneMinimizationScan(const std::string &optStr, int aTag, double centralPar, double rangePar, LightParamLogLike &myLogLike,
                                          std::vector<double> &parVals, std::vector<double> &results, std::vector<int> &numInf); 
					  
void execOneMinimizationMultScan(const std::string &optStr, int numPhaseSim, int numSteps, double centralPar, double rangePar, LightParamLogLike &myLogLike);

void synchronizeLLs(int aTag); 
void broadcastLLs(int aTag, std::vector<double> &dbData);
void broadcastIntsFromRank0(int aTag, std::vector<double> &dbData);
void getEventsCountOnRank0(LightParamLogLike &myLL, std::vector<int> &evtCnts); 
void loadBalanceEvts(const std::vector<int> &evtCnts, LightParamLogLike &myLL); 
//  void moveEventFromRanktoRank(int rank1, int rank2); done in loadBalance

std::string topDirAll("/work1/next/lebrun/DataTrig1/8686_P08a2/");

int main(int argc, char **argv) {
//
   double xCenter = -10.; double yCenter = -10.; double radius=5.0; // dimension in mm 
   
   size_t numSteps = 10;
   int numForSim = 100; // go fast, to debug synchronization.. 
   double pCenterOpt = 0.;
   double pRangeOpt = 1.0;
   int numPhaseMin = 2;
   // the hole in the EL gap field. (or immediate TPB layer. )
   double xHole = 0.; double yHole = 0.; double radHole= 2.0;
   double depthHole = 1.0e-8;
   int selectedSiPM = INT_MAX;
   double siPMDiffuseLightRelYield = 1.0e-9; // Best value so far is.. none of it! That is for effZHeigt = 10 mm, and Trns 0.1 -> 0.5
//   double effZHeightSiPM = 10.;
   double effZHeightSiPM = 7.5; // July 12 setting.. 
   double transverseDiffCoeff = 0.15;
   double elLife = 3100.;
   double elLightYieldAv = 486.;
   double elLightYieldSigma = std::sqrt(elLightYieldAv);
   double geomGlobalLightEff = 5.0e-4;
   double S2Skewness = 0.;
   double LongDiff = 0.;
   int fakeDataSwitch = 0;
   double elLightYieldMechSigma = 0.01;
   
   std::string optStr("none");
   
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
     std::cerr << " ./GenLightParamSim MPI version. Need at least two argument, optStr  and value argument. Fatal   " << std::endl;
     MPI_Finalize(); 
     exit(2);
   }
   if (myRank == 0) std::cout << " Job argument decoding, number of args  "  << argc << std::endl;
   int argcR = argc - 1;
   bool pCenterOptUsed = false; 
   bool pRangeOptUsed = false; 
   bool numPhaseMinOptUsed = false;
   for (int kArg = 0; kArg != argcR/2; kArg++) {
     std::string parStr(argv[1+2*kArg]);
     std::string valStr(argv[2+2*kArg]);
     if (myRank == 0) std::cout << " parStr " << parStr << " valStr " << valStr << std::endl;
     std::string valStrcpy(valStr);
     for (size_t ii=0; ii != valStrcpy.length(); ii++) if (valStrcpy[ii] == '.') valStrcpy[ii] ='p';
     std::istringstream valStrStr(valStr);
     if (parStr.find("opt") != std::string::npos) {
      optStr = valStr;
      if (myRank == 0) std::cout << " Option is  "  << optStr << std::endl;
    } else if (parStr.find("xCenter") != std::string::npos) {
       valStrStr >> xCenter;
      if (myRank == 0) std::cout << " xCenter is  "  << xCenter << std::endl;
    } else if (parStr.find("yCenter") != std::string::npos) {
       valStrStr >> yCenter;
      if (myRank == 0) std::cout << " yCenter is  "  << yCenter << std::endl;
    } else if (parStr.find("xHole") != std::string::npos) {
       valStrStr >> xHole;
      if (myRank == 0) std::cout << " xHole is  "  << xHole << std::endl;
    } else if (parStr.find("yHole") != std::string::npos) {
       valStrStr >> yHole;
      if (myRank == 0) std::cout << " yHole is  "  << yHole << std::endl;
    } else if (parStr.find("radHole") != std::string::npos) {
       valStrStr >> radHole;
      if (myRank == 0) std::cout << " hole radius is  "  << radHole << std::endl;
    } else if (parStr.find("depthHole") != std::string::npos) {
       valStrStr >> depthHole;
      if (myRank == 0) std::cout << " depthHole is  "  << depthHole << std::endl;
    } else if (parStr.find("radius") != std::string::npos) {
        valStrStr >> radius;
      if (myRank == 0) std::cout << " radius  "  << radius << std::endl;
    } else if (parStr.find("SiPMSel") != std::string::npos) {
        valStrStr >> selectedSiPM;
      if (myRank == 0) std::cout << "  selectedSiPM "  << selectedSiPM << std::endl;
    } else if (parStr.find("SiPMDiffuseLight") != std::string::npos) {
        valStrStr >> siPMDiffuseLightRelYield;
      if (myRank == 0) std::cout << "  SiPMDiffuseLight Relative yield "  << siPMDiffuseLightRelYield << std::endl;
    } else if (parStr.find("effZHeightSiPM") != std::string::npos) {
        valStrStr >> effZHeightSiPM ;
      if (myRank == 0) std::cout << " effZHeightSiPM  "  << effZHeightSiPM << std::endl;
    } else if (parStr.find("TransDiffCoeff") != std::string::npos) {
        valStrStr >> transverseDiffCoeff ;
      if (myRank == 0) std::cout << " transverseDiffCoeff  "  << transverseDiffCoeff << std::endl;
    } else if (parStr.find("ElLife") != std::string::npos) {
        valStrStr >> elLife ;
      if (myRank == 0) std::cout << " elLife  "  << elLife << std::endl;
    } else if (parStr.find("ELLightYieldAv") != std::string::npos) {
        valStrStr >>  elLightYieldAv;
      if (myRank == 0) std::cout << " ELLightYieldAv  "  << elLightYieldAv << std::endl;
    } else if (parStr.find("ELLightYieldSigma") != std::string::npos) {
        valStrStr >>  elLightYieldSigma;
      if (myRank == 0) std::cout << " ELLightYield Sigma  "  << elLightYieldSigma << std::endl;
     } else if (parStr.find("ELLightYieldMechSigma") != std::string::npos) {
        valStrStr >>  elLightYieldMechSigma;
      if (myRank == 0) std::cout << " ELLightYield Mechanical Sigma  "  << elLightYieldMechSigma << std::endl;
   } else if (parStr.find("GeomGlobalLightEff") != std::string::npos) {
        valStrStr >>  geomGlobalLightEff;
      if (myRank == 0) std::cout << " Geometric global light effciency..  "  << geomGlobalLightEff << std::endl;
    } else if (parStr.find("S2Skewness") != std::string::npos) {
        valStrStr >> S2Skewness ;
      if (myRank == 0) std::cout << " S2 Skewness  "  <<  S2Skewness << std::endl;
    } else if (parStr.find("LongDiff") != std::string::npos) {
        valStrStr >> LongDiff ;
      if (myRank == 0) std::cout << " Longitudinal Coefficient  "  <<  LongDiff << std::endl;
   } else if (parStr.find("numSim") != std::string::npos) {
      valStrStr >> numForSim;
      if (myRank == 0) std::cout << " numForSim  "  << numForSim << std::endl;
    } else if (parStr.find("numSteps") != std::string::npos) {
      valStrStr >> numSteps;
      if (myRank == 0) std::cout << " numSteps  "  << numSteps << std::endl;
    } else if (parStr.find("fakeData") != std::string::npos) {
      valStrStr >> fakeDataSwitch;
      if (myRank == 0) {
         if (fakeDataSwitch == 1) std::cout << " We will cheat and use fake data for the real data! "  << std::endl;
	 else std::cout << " Usual data mode! "  << std::endl;
      }
    } else if (parStr.find("pCenter") != std::string::npos) {
      valStrStr >> pCenterOpt;
      pCenterOptUsed = true;
      if (myRank == 0) std::cout << " Option pCenter invoke pcenter will be   "  << pCenterOpt << std::endl;
    } else if (parStr.find("pRange") != std::string::npos) {
      valStrStr >> pRangeOpt;
      pRangeOptUsed = true;
      if (myRank == 0) std::cout << " Option pRange invoke,  pRange will be  "  << pRangeOpt << std::endl;
    } else if (parStr.find("numPhaseMin") != std::string::npos) {
      valStrStr >> numPhaseMin;
      numPhaseMinOptUsed = true;
      if (myRank == 0) std::cout << " Option numPhaseMin invoke pcenter will be  "  <<  numPhaseMin << std::endl;
   } else {
      if (myRank == 0) std::cout << " Unrecognized argument   "  << parStr <<  " fata, quit here and now " << std::endl;
      MPI_Finalize();
      exit(2);
   }
  }   
//    MPI_Comm master_comm;
//    MPI_Comm_split( MPI_COMM_WORLD, (myRank == 0), 0, &master_comm );

     std::string lightParamFile(topDirAll);  lightParamFile += std::string("map_8686.h5");
     
     struct timeval tvStart, tvStop, tvEnd;
     char tmbuf[64];
     gettimeofday(&tvStart,NULL);
     time_t nowTimeStart = tvStart.tv_sec;
     struct tm *nowtm = localtime(&nowTimeStart);
     strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtm);
     std::string dateNow(tmbuf);
     std::cout << ".... from processor " << std::string(processor_name) << " rank " << myRank << " out of " << world_size 
               << " option " << optStr <<  " starting at " << dateNow << std::endl;
      
     LightParamLogLike myLogLike(myRank);
     myLogLike.setNumEvtsForSim(numForSim); 
     const double rrCenter = std::sqrt(xCenter * xCenter + yCenter * yCenter);
     if (rrCenter < 1.) {
       myLogLike.setPhiSectorForELParam(0);
     } else { 
       const double aPhi = M_PI + std::atan2(yCenter, xCenter);
       const int phiSect = static_cast<int>(std::floor(myLogLike.getNumELPhiSectors() * aPhi/(2.0*M_PI)));
       myLogLike.setPhiSectorForELParam(phiSect);
     } 
     //
     // install a hole... Could use one more argument here. 
     //
     myLogLike.installHoleCenter1(xHole, yHole, radHole, depthHole);
     //
     // set the selected SiPM for bary center bias studies. 
     //
     myLogLike.setSelectedSiPM(selectedSiPM);
     LPRecSample myS(lightParamFile);
     std::string  token("P08a2"); 
     getTheKryptonSampleRun8686(token,  xCenter, yCenter, radius, myS); 
     if (myS.evts_.size() == 0)   
	   std::cerr << "  From rank " << myRank << " No calculation of LogLikelihood, as no events selected " << std::endl;
     myLogLike.setMyData(myS); // extra deep copy, but small amount of data.. 
   //
      synchronizeLLs(0); 
      
      std::vector<int> evtsCnts(world_size, 0);
      getEventsCountOnRank0(myLogLike, evtsCnts); // and broadcast this ino. 
      //
      if (myRank < 2) {
        std::cout << " from rank " << myRank << "  After getEventsCountOnRank0, evtsCnts size " << evtsCnts.size() << std::endl << std::flush;
	for (size_t k=0; k != evtsCnts.size(); k++)  std::cout << " from rank " << myRank << " evtsCnts [" << k << "]= " << evtsCnts[k] << std::endl << std::flush;
	std::cout << std::flush;
      }
//       synchronizeLLs(101); Assume it isynchronized.. 

      loadBalanceEvts(evtsCnts, myLogLike);
      
      
       // one more for event tally 
     std::vector<double> pVals(numSteps+1, 0.); //  Last is the # of events. 
     std::vector<double> allResults(numSteps+1, 0.);
     std::vector<int> allNumInf(numSteps+1, 0.);
     std::vector<double> dbData(2, 0.);
     if (optStr.find("Scan1D_") == 0) {
       std::string optStrVar1D = optStr.substr(7);
       if ((!pCenterOptUsed) || (!pRangeOptUsed)) {
         std::cerr << " Main, GenLightParamSim, insufficient argument set, expect starting value for pCent and pRange .. Fatal " << std::endl;
	 MPI_Finalize();
	 exit(2);
       }
       // Dirty stuff, I better keep track of this, .. overwriting some parameter, 
       myLogLike.setASimParameter(ELIFE, elLife); // tentative, to check.. 
       myLogLike.setASimParameter(SIPMDIFFUSELIGHT, siPMDiffuseLightRelYield); // tentative, to check..Done on July 10, no extra light is best! 
       myLogLike.setASimParameter(EFFZHEIGHTSIPM, effZHeightSiPM); // tentative, to check.. 
       myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, elLightYieldAv);  /// June 28-29 data, valid for Q4 Central, no holes. 
       myLogLike.setASimParameter(NOMS2LIGHTYIELDSIGMA, elLightYieldSigma);  /// June 28-29 data, valid for Q4 Central, no holes. 
       myLogLike.setASimParameter(MECHS2LIGHTYIELDSIGMA, elLightYieldMechSigma);  /// June 28-29 data, valid for Q4 Central, no holes. 
       myLogLike.setASimParameter(GEOMGLOBALSIPMEFF, geomGlobalLightEff );     
       myLogLike.setASimParameter(S2SKEWNESS, S2Skewness );     
       myLogLike.setASimParameter(LONGDIFF, LongDiff );     
       myLogLike.setASimParameter(TRANSDIFF, transverseDiffCoeff);
       if ((std::abs(xCenter - 10.) < 5.) && (std::abs(yCenter + 10.) < 5.)) {
//          myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, 572.);  /// June 28-29 data, valid for Q4 Central, no holes. 
//          myLogLike.setASimParameter(GEOMGLOBALSIPMEFF, 2.5e-4); // Best value from runs June29, for the 4rth Quadrant.   
//          myLogLike.setASimParameter(TRANSDIFF, 0.082);  
       } else if ((std::abs(xCenter - 10.) < 5.) && (std::abs(yCenter - 10.) < 5.)) {
//          myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, 585.);  /// Mid June data, valid for Q1 Central, no holes.
//          myLogLike.setASimParameter(GEOMGLOBALSIPMEFF, 3.5e-4); // Best value To be checked..  
       } else if ((std::abs(xCenter - 40.) < 10.) && (std::abs(yCenter + 160.) < 10.)) {
       
//          myLogLike.setASimParameter(ELIFE, 3100.);  /// Late June..  
//           myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, 486.);  /// June 28-29 data, valid for Q4 Central, no holes. 
//          myLogLike.setASimParameter(GEOMGLOBALSIPMEFF, 5.0e-4); // Tentative, to check.    
//          myLogLike.setASimParameter(TRANSDIFF, 0.12); 
	  // July 4 2022  
//          myLogLike.setASimParameter(TRANSDIFF, 0.24);  // a bit more precise.. Jul 11, with effZHeightSiPM = 7.5 
//             July 21 : 
	  
      }  else if ((std::abs(xCenter - 80.) < 5.) && (std::abs(yCenter + 160.) < 5.)) {
//          myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, 440.);  /// July 4 data.. 
//          myLogLike.setASimParameter(GEOMGLOBALSIPMEFF, 3.5e-4); // Best value To be checked..  
      }
//       myLogLike.setASimParameter(GEOMGLOBALSIPMEFF, 5.0e-4); // Best value from runs June24 
       double pCenter = pCenterOpt;
       double pRange = pRangeOpt;
      if (fakeDataSwitch == 1) myLogLike.switchToFakeData();
       execOneMinimizationMultScan(optStrVar1D, numPhaseMin, numSteps,  pCenter, pRange, myLogLike); // multiple passes.. 
     }
      else if (optStr.find("LongDiffStudy1") != std::string::npos) {  // old code...  
        execOneMinimizationScan(optStr, 1, 0.056, 0.01, myLogLike, pVals, allResults, allNumInf); // one pass only 
     } else if (optStr.find("ELLightYieldAv") != std::string::npos) { // old code...  
	double pCenter = pCenterOptUsed ? pCenterOpt : 650.;
	double pRange = pRangeOptUsed ? pRangeOpt : 400.;
        execOneMinimizationScan(optStr, 1, pCenter, pRange, myLogLike, pVals, allResults, allNumInf ); // first pass only 
        double minLL = DBL_MAX;
	if (numPhaseMin > 1) { 
	  if (myRank == 0) {
	    pCenter = myLogLike.fitParabolaLL(pVals, allResults);
	    dbData[0] = pCenter; dbData[1] = minLL;
	  } 
	  broadcastLLs(2, dbData); 
	  pCenter = dbData[0]; // updated all ranks with the correct pCenter. 
          execOneMinimizationScan(optStr, 2, pCenter, pRange/2., myLogLike, pVals, allResults, allNumInf); // 2nd pass 
	  if (numPhaseMin > 2) { 
	    if (myRank == 0) {
	      pCenter = myLogLike.fitParabolaLL(pVals, allResults);
	      dbData[0] = pCenter; dbData[1] = minLL;
	    } 
	    broadcastLLs(3, dbData); 
	    pCenter = dbData[0]; // updated all ranks with the correct pCenter. 
            execOneMinimizationScan(optStr, 3, pCenter, pRange/4., myLogLike, pVals, allResults, allNumInf); // 3rd pass  
	  }
       } 
     } else if (optStr.find("ELLightYieldTest") != std::string::npos) {
        numSteps = 1; // for brevity. 
	pVals.resize(numSteps+2);
	allResults.resize(numSteps+2);
        execOneMinimizationScan(optStr, 1, 580., 1., myLogLike, pVals, allResults, allNumInf); // first pass only 
	if (myRank == 0) std::cout << " ELLightYieldTest, first pass done, pVals[0] " << pVals[0] <<  " allResults " << allResults[0] << ", " << std::endl;
        execOneMinimizationScan(optStr, 2, 680., 1., myLogLike, pVals, allResults, allNumInf); // 2nd pass, shifted by a step  
 	if (myRank == 0) std::cout << " ELLightYieldTest, 2nd pass done, pVals[0] " << pVals[0] <<  " allResults " << allResults[0] << ", " << std::endl;
       execOneMinimizationScan(optStr, 3, 580., 1., myLogLike, pVals, allResults, allNumInf); // 3rd pass 
	if (myRank == 0) std::cout << " ELLightYieldTest, 3rd pass done, pVals[0] " << pVals[0] <<  " allResults " << allResults[0] << ", "  << std::endl;
       execOneMinimizationScan(optStr, 4, 680., 1., myLogLike, pVals, allResults, allNumInf); // 3rd pass 
	if (myRank == 0) std::cout << " ELLightYieldTest, 4rth pass done, pVals[0] " << pVals[0] <<  " allResults " << allResults[0] << ", "  << std::endl;
    } else if (optStr.find("ELLightYieldLongDiff") != std::string::npos) {
        myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, 650.);
        std::string optLongDiff("LongDiffPhase1");
	std::string optELLightYield("ELLightYieldAv");
        execOneMinimizationScan(optLongDiff, 1, 0.056, 0.01, myLogLike, pVals, allResults, allNumInf); // first pass only 
        double minLL = DBL_MAX;
	double pCenterLongDiff = 0.;
	if (myRank == 0) {
	    pCenterLongDiff = 0.; minLL = DBL_MAX;
	    for (size_t kP = 0; kP != numSteps; kP++) {  // could do a parabolic fit here... 
	      if (allResults[kP] < minLL) { minLL = allResults[kP]; pCenterLongDiff = pVals[kP]; } 
	    }
	    dbData[0] = pCenterLongDiff; dbData[1] = minLL;
	  } 
	broadcastLLs(2, dbData);
	if (myRank != 0 ) pCenterLongDiff = dbData[0]; 
        myLogLike.setASimParameter(LONGDIFF, pCenterLongDiff);
        execOneMinimizationScan(optELLightYield, 2, 586.705, 150., myLogLike, pVals, allResults, allNumInf); // 2nd pass
	double  pCenterELLightYield = 0.;
	if (myRank == 0) {
	  pCenterELLightYield = 0.; minLL = DBL_MAX;
	  for (size_t kP = 0; kP != numSteps; kP++) {  // could do a parabolic fit here... 
	      if (allResults[kP] < minLL) { minLL = allResults[kP]; pCenterELLightYield = pVals[kP]; } 
	  }
	  dbData[0] = pCenterELLightYield; dbData[1] = minLL;
	} 
	broadcastLLs(3, dbData);
	if (myRank != 0 ) pCenterELLightYield = dbData[0]; 
        myLogLike.setASimParameter(NOMS2LIGHTYIELDMEAN, pCenterELLightYield);
        std::string optLongDiff2("LongDiffPhase2");
        execOneMinimizationScan(optLongDiff2, 3, pCenterLongDiff, 0.01, myLogLike, pVals, allResults, allNumInf); // 2nd pass 
    }

  // Finalize the MPI environment. 
//   sleep(2);
   
   if (myRank == 0) { 
      gettimeofday(&tvEnd,NULL);
      time_t nowTimeEnd = tvEnd.tv_sec;
      struct tm *nowtmEnd = localtime(&nowTimeEnd);
      strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
      std::string dateNowEnd(tmbuf);
      std::cout << " From Rank 0, invoking MPI_Finalize, really done now, at t = " << dateNowEnd << std::endl;
   }
   MPI_Finalize();

}
void execOneMinimizationScan(const std::string &optStr, int aTag, double centralPar, double rangePar, LightParamLogLike &myLogLike,
                                          std::vector<double> &parVals, std::vector<double> &results, std::vector<int> &numInf) { 

    struct timeval tvStart, tvStop, tvEnd;
    char tmbuf2[64];
    
    gettimeofday(&tvStart,NULL);
    
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    if (myRank < 2) {
      time_t nowTimeRec = tvStart.tv_sec;
      struct tm *nowtmRec = localtime(&nowTimeRec);
      strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmRec);
      std::string dateStart(tmbuf2);
      std::cout << " ... from Rank " << myRank << " start a scan optStr " << optStr << " t = " << dateStart << " centralPar " << centralPar << std::endl;
    }
    const int numSteps = static_cast<int>(parVals.size()) - 1; // last element for the total event yield, not touched here... next too last is the # of inf.  
    
    std::vector<double> rankResults(2*results.size(), 0.); // one for the result, second for the number of infinities.. 
//    std::vector<double> rankNumInf(results.size(), 0.);  
    rankResults[numSteps] = myLogLike.numEvtsData();  
    parVals[numSteps] = DBL_MAX; 
    
    const double stepPar = rangePar/numSteps;
    const double startPar = centralPar - 0.5*stepPar*numSteps;
    for (int kP = 0; kP != numSteps;  kP++) {
         const double val = startPar + kP*stepPar;
         parVals[kP] = val; 
         if (myRank == 0) { results[kP] = 0.; numInf[kP] = 0; }
//         if ((myRank < 3) && (kP == 0)) 
//	   std::cout << " ... from Rank " << myRank << " setting parVals[0] " << parVals[0] << std::endl;
    }				  
    enum LIGHTPARAMS myAnalysis;
    if (optStr.find("LongDiff") != std::string::npos) {   
        myAnalysis = LONGDIFF;
    } else if (optStr.find("ELLightYieldAv") != std::string::npos) {
         myAnalysis = NOMS2LIGHTYIELDMEAN;
    } else if (optStr.find("ELLightYieldTest") != std::string::npos) {
         myAnalysis = NOMS2LIGHTYIELDMEAN;
    } else if (optStr.find("ElLife") != std::string::npos) {
         myAnalysis = ELIFE;
    } else if (optStr.find("TransDiff") != std::string::npos) {
         myAnalysis = TRANSDIFF;
    } else if (optStr.find("ELLightYieldSigma") != std::string::npos) {
         myAnalysis = NOMS2LIGHTYIELDSIGMA;
    } else if (optStr.find("ELLightYieldMechSigma") != std::string::npos) {
         myAnalysis = MECHS2LIGHTYIELDSIGMA;
    } else if (optStr.find("GeomGlobalSiPMEff") != std::string::npos) {
         myAnalysis = GEOMGLOBALSIPMEFF ;
    } else if (optStr.find("SelectedSiPMEff") != std::string::npos) {
         myAnalysis = EFFSIPMEFF0 ;
    } else if (optStr.find("SiPMDiffuseLight") != std::string::npos) {
         myAnalysis = SIPMDIFFUSELIGHT ;
    } else if (optStr.find("S2TrigThresh") != std::string::npos) {
         myAnalysis = S2TRIGTHRESH;
    } else if (optStr.find("QuadRadialC") != std::string::npos) {  // Implemented, I think.. 
         myAnalysis = QUADRADIALELCOEFFS0;
    } else if (optStr.find("S2Skewness") != std::string::npos) { 
         myAnalysis = S2SKEWNESS;
    } else {
      std::cerr << " GenLightParamMPINis, error, undrecognized Scan1d varaiable " << myAnalysis << " quit here " << std::endl; 
      MPI_Finalize(); exit(2);
    }
    // type of scan or study. 
    myLogLike.studyFewEventsMPI(myRank, myAnalysis, parVals, rankResults);
    // getting the results out.. 
    gettimeofday(&tvStop,NULL);
    long unsigned diffT = tvStop.tv_sec - tvStart.tv_sec;
    if (myRank != 0) {
//      MPI_Send((void*) &parVals[0], static_cast<int>(parVals.size()), MPI_DOUBLE, 0, 0, master_comm);
      MPI_Send((void*) &rankResults[0], static_cast<int>(rankResults.size()), MPI_DOUBLE, 0, aTag, MPI_COMM_WORLD );
      std::cout << " ... Did send the data from rank " << myRank << " after " << diffT << " seconds of calculation " << std::endl;
    } else {
       std::cout << " ... from rank " << myRank << " finished calculation after " << diffT << " seconds " << std::endl;
       for (size_t kLL=0; kLL != numSteps; kLL++) {
         results[kLL] += rankResults[kLL]; // The master is also a worker. 
	  const size_t kLLRank = kLL + parVals.size();
	  if (kLLRank >= rankResults.size()) {
	      std::cerr << " ... from Rank 0, Wrong indices into rankResults " << kLLRank << " for kLL " << kLL << "  break here ! " << std::endl;
	      MPI_Finalize(); exit(2);
	    }
	    numInf[kLL] += static_cast<int>(rankResults[kLLRank]);
       } 
       for (int kSlave=1; kSlave != world_size; kSlave++) { 
         MPI_Status aStatus;
         MPI_Recv((void*) &rankResults[0], static_cast<int>(rankResults.size()), MPI_DOUBLE, kSlave, aTag, MPI_COMM_WORLD, &aStatus); 
         gettimeofday(&tvStop,NULL);
         time_t nowTimeRec = tvStop.tv_sec;
         struct tm *nowtmRec = localtime(&nowTimeRec);
         strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmRec);
         std::string dateRec(tmbuf2);
         std::cout << " ... from Rank " << kSlave << " Received the LLsm date/time " << dateRec << std::endl;
         for (size_t kLL=0; kLL != parVals.size(); kLL++) {
	    results[kLL] += rankResults[kLL]; // The worker contributes (hopefully) 
	    const size_t kLLRank = kLL + parVals.size();
	    if (kLLRank >= rankResults.size()) {
	      std::cerr << " ... from Rank " << kSlave << " Wrong indices into rankResults " << kLLRank << " for kLL " << kLL << "  break here ! " << std::endl;
	      MPI_Finalize(); exit(2);
	    }
	    numInf[kLL] += static_cast<int>(rankResults[kLLRank]); 
	 }
       } // loop over slave to get results. 
       gettimeofday(&tvStop,NULL);
       time_t nowTimeEnd = tvStop.tv_sec;
       struct tm *nowtmEnd = localtime(&nowTimeEnd);
       strftime(tmbuf2, sizeof tmbuf2, "%Y-%m-%d_%H_%M_%S", nowtmEnd);
       std::string dateNowEnd(tmbuf2);
       std::ostringstream aTagStrStr; aTagStrStr << aTag;
       std::string fNameOut(topDirAll); 
       fNameOut += std::string("LogLikeOut_") + optStr + std::string("_Phase_") + 
                   aTagStrStr.str() + std::string("_t_") + dateNowEnd + std::string("_V2.txt");
       std::cout << " ... from rank " << myRank << " Finished all calculations, saving result on file  " << fNameOut << std::endl;
       std::ofstream fOut(fNameOut.c_str());
       fOut << " pVal LL nInf " << std::endl;
       for (size_t kPar = 0; kPar != results.size(); kPar++) {
         fOut << " " << std::setprecision(6) << parVals[kPar] << " " << std::setprecision(9) 
	             << results[kPar] << std::setprecision(6) << " " << numInf[kPar] << std::endl;
       }
       fOut.close();
   } // Master node, rank 0 
   // Synchronization. 
   synchronizeLLs(aTag);
}
void execOneMinimizationMultScan(const std::string &optStr, int numPhaseMin, int numSteps, double pCenter, double pRange, LightParamLogLike &myLogLike) {

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    std::vector<double> pVals(numSteps+1, 0.);
    std::vector<double> allResults(numSteps+1, 0.);
    std::vector<int> allNumInf(numSteps+1, 0.);
    std::vector<double> dbData(2, 0.);
    execOneMinimizationScan(optStr, 1, pCenter, pRange, myLogLike, pVals, allResults, allNumInf); // first pass only 
    myLogLike.printAllParams(topDirAll, optStr);
    if (numPhaseMin == 1) return;
    double minLL = DBL_MAX;
    int iPhaseMin = 2;
    double pRangeNextPass = pRange;
    while (iPhaseMin < numPhaseMin+1) {
      if (myRank == 0) {
        pCenter = myLogLike.fitParabolaLL(pVals, allResults); 
	if (((optStr.find("GeomGlobalSiPMEff") != std::string::npos) || 
	     (optStr.find("GeomGlobalSiPMEff") != std::string::npos) || (optStr.find("ElLife") != std::string::npos)) && (pCenter < 0.)) {
	  std::cout << " execOneMinimizationMultScan on " << optStr << " next pCenter = " << pCenter << " readjust... " << std::endl;
	  pCenter = pVals[0] - 1.0e-8; 
	
	}
        dbData[0] = pCenter; dbData[1] = minLL;
      } 
      broadcastLLs(iPhaseMin, dbData);
      pCenter = dbData[0]; // updated all ranks with the correct pCenter.	 
      pRangeNextPass = ( (pCenter < pVals[0]) || (pCenter > pVals[numSteps-1])) ? pRangeNextPass : pRangeNextPass/2.; 
      const double pCenterMin = pCenter - 0.5*pRange;
      if (((optStr.find("GeomGlobalSiPMEff") != std::string::npos) || 
           (optStr.find("GeomGlobalSiPMEff") != std::string::npos) || (optStr.find("ElLife") != std::string::npos)) && (pCenterMin < 0.)) 
            pRangeNextPass = (2.0 * pCenter) - 1.0e-6;
      if (myRank == 0) {
        struct timeval tvStart;
        char tmbuf[64];
        gettimeofday(&tvStart,NULL);
        time_t nowTimeStart = tvStart.tv_sec;
        struct tm *nowtmStart = localtime(&nowTimeStart);
        strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d_%H_%M_%S", nowtmStart);
        std::string dateNowStart(tmbuf);
        std::cout << " From Rank 0, execOneMinimizationMultScan , at t = " << dateNowStart <<  " scan " 
         	 << iPhaseMin << " pCenter " << pCenter << " range " << pRangeNextPass << std::endl;
       }
       execOneMinimizationScan(optStr, iPhaseMin, pCenter, pRangeNextPass, myLogLike, pVals, allResults, allNumInf); // 2nd pass 
       iPhaseMin++;
     } // on minimization phase, each one scan 
}

