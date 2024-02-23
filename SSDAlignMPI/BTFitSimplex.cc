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
emph::rbal::BTAlignGeom1c* emph::rbal::BTAlignGeom1c::instancePtr=nullptr; 
emph::rbal::SSDAlignParams* emph::rbal::SSDAlignParams::instancePtr=nullptr; 
emph::rbal::BTMagneticField* emph::rbal::BTMagneticField::instancePtr=nullptr; 

//
// Main program for doing the complete ~ 64 parameters fit of all misalignment SSD parameters. 

int main(int argc, char **argv) {
//
   
   double dumVar = 0; // place holder.. 
   bool IsPhase1c = true; // December 4rth 2023. 
   
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
   int aRandomSeed= 234578;
   bool fixMomentum = true;
   double aRandomRollFactor = 1.0e-9;
   double limRolls = 0.09;
   size_t kSeTrShifted = INT_MAX;
   std::string viewShifted("none");
   double trShift = 0.;
   int selectedSpill = INT_MAX;
   std::string token("none");
   std::string tokenPrevBase("none");
   bool doCallFCNOnce = false; // for debugging purpose.. One cll to FCCN and that is it. 
   bool doLoadParamsFromPreviousRun = false;
   int iOptStartFromPrev = 0;
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
   int SoftLimitEpsilY = INT_MAX;
   double SoftLimitGeomSt4St5 = 0.; // No constraints. No addition to the average chi-Square if the distance is less than 2 mm 
   bool applyEmittanceConstraint = false;
   // To be corrected!! See BeamTrackSSDAlignFCN.cxx
   double betaFunctionY = 1357.; // Only valid for 120 GeV  See e-mail from Mike Olander, Jan 20 
   double betaFunctionX = 377.; // Only valid for 120 GeV 
   double alphaFunctionY = -25.1063; // same..  
   double alphaFunctionX = -8.62823; //
   double nominalMomentum = 29.7; // for the 30 GeV analysis
   double nominalMomentumDispersion = 0.; // for the 30 GeV analysis
   double uPLimitOnChiSqTrackFits = 200.; 
   double assumedDoubleGap = 2.5; // starting value for the gap 
   double MinimumGapSt4St5 = assumedDoubleGap - 0.1 ; // A bit arbitrary.. 
   int runNum = 1055; // For data... 
   double integrationStepSize = 0.;
   double EdmMaxTolerance = 0.5;
   double magnetShiftX = 0.; double magnetShiftY = 0.; double magnetShiftZ = 0.; double magnetRotXY = 0.;
   bool magnetIsRemoved = false;  
   bool doPencilBeam120 = false; 
   bool doAntiPencilBeam120 = false; 
   bool doAntiPencilBeam120OnlyV = false; 
   std::string G4EMPHDescrToken("none");
   int numMaxFCNCalls = 5000;
   bool doRejectVViews=false; bool doRejectSt6=false;
   bool doReject5XView=false; bool doReject5YView=false; bool doReject5VView=false; 
   bool doReject6XView=false; bool doReject6YView=false; bool doReject6VView=false; 
   bool arbInput = true; // no command line, done on Dec 29 2023.. Sequence 7r on run 2113 
   int RejectedStation = -1; 
   int FixedRollStation = -1; 
   double VertY8Offset = 0.;
   std::vector<int> rejectedStationsIndices;
   int FittedStationNum = -1;
   char FittedViewChar = 'Z';
   int iOptEulerVsRK4 = 0; // Euler is the default option. 
     
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
        } else if (parStr.find("phase1c") != std::string::npos) {
          int iS=0;
          valStrStr >> iS;
	  IsPhase1c = (iS == 1);
          if (myRank == 0) {
	    if (IsPhase1c) std::cerr << " Phase1c geometry is in effect "  << std::endl;
	    else std::cerr << " Phase1b geometry is in effect    "  << std::endl;
	  }  
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
	  doLoadParamsFromPreviousRun = (iTmpPrev != 0);
	  iOptStartFromPrev = iTmpPrev;
          if ((myRank == 0) && doLoadParamsFromPreviousRun)  std::cerr << "We will restart from a previous set of alignment Params  " << std::endl;
          if ((myRank == 0) && (iOptStartFromPrev == 2))  std::cerr << "We will restart from the base token, decent resids.   " << std::endl;
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
	    SoftLimitEpsilY = iS;
	    if (myRank == 0) std::cerr << " ... Not quite.. We will assume the Y emittance at Station 0 is about  " 
	                                 << 1.0e-3*SoftLimitEpsilY << " milimeter*miliradian " << std::endl;
	  }
        } else if (parStr.find("softLimitGeoSt4St5") != std::string::npos) {
          valStrStr >> SoftLimitGeomSt4St5;
          if ((myRank == 0) && (SoftLimitGeomSt4St5 > 1.0e-6))  
	    std::cerr << "We impose a soft contraints on the separation between the double sensors of " << SoftLimitGeomSt4St5 << std::endl;
        } else if (parStr.find("minimumGapSt4St5") != std::string::npos) {
          valStrStr >> MinimumGapSt4St5;
          if (myRank == 0)  
	    std::cerr << "The minimum separation between the double sensors is " << MinimumGapSt4St5 << " is the SoftLimitGeometry is imposed" << std::endl;
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
         } else if (parStr == std::string("rejectVViews")) {
          int iS;
          valStrStr >> iS;
	  doRejectVViews = (iS == 1);
          if ((myRank == 0) && doRejectVViews)  std::cerr << "We will not use the V views in Station 5  (or 6, if Phase1c)   " << std::endl;
          if ((myRank == 0) && (!doRejectVViews))  std::cerr << "We will use relevant V views (default)   " << std::endl;
         } else if (parStr == std::string("rejectedStation")) {
          valStrStr >> RejectedStation;
          if ((myRank == 0) && (RejectedStation > 1) && (RejectedStation < 7))  
	       std::cerr << "We will not use Station " << RejectedStation   << std::endl;
          if ((myRank == 0) && (RejectedStation == 56) ) { std::cerr << " We will reject stations (plural) 5 and 6  "  << std::endl; }
          if ((myRank == 0) && (RejectedStation < 2) ) { std::cerr << " Do we really want o reject Station 0 or 1 from the fit ? "  << std::endl; exit(2); }
          if ((myRank == 0) && ((RejectedStation > 6) && (RejectedStation != 56)) ) { std::cerr << " Illegal  rejected Station " << RejectedStation   << std::endl; exit(2); }
	  if ((RejectedStation == 6) || (RejectedStation == 56)) doRejectSt6 = true;
         } else if (parStr == std::string("rejectedStations") ) {
	  int listOfStation; 
          valStrStr >> listOfStation;
	  int ii1 = listOfStation/1000;
	  if (ii1 != 0) rejectedStationsIndices.push_back(ii1); 
	  int ii2 = (listOfStation - ii1*1000)/100;
	  if (ii2 != 0) rejectedStationsIndices.push_back(ii2); 
	  int ii3 = (listOfStation - ii1*1000 - ii2*100)/10;
	  if (ii3 != 0) rejectedStationsIndices.push_back(ii3); 
	  int ii4 =  (listOfStation - ii1*1000 - ii2*100 - ii3*10);
	  if (ii4 != 0) rejectedStationsIndices.push_back(ii4); 
	  for (size_t k=0; k!= rejectedStationsIndices.size(); k++)  { if (rejectedStationsIndices[k] == 6) doRejectSt6 = true; }
          if ((myRank == 0) && (rejectedStationsIndices.size() > 0)) { 
	    std::cerr << "We will not use Station ";
	    for (size_t k=0; k!= rejectedStationsIndices.size(); k++) std::cerr << rejectedStationsIndices[k] << ", ";
	    std::cerr << std::endl;
	   }
         } else if (parStr.find("fixedRollStation") != std::string::npos) {
          valStrStr >> FixedRollStation;
          if ((myRank == 0) && (FixedRollStation >= 0) && (FixedRollStation < 7))  
	       std::cerr << "We fix the roll angle and roll center to zero for station  " <<  FixedRollStation  << std::endl;
          if ((myRank == 0) && (FixedRollStation == -1) )  std::cerr << "Only Station 0 Y view roll angle is fixed to zero  " <<  FixedRollStation  << std::endl; 
        } else if (parStr.find("rejectSt6") != std::string::npos) { // Obsolete now... see just above..
          int iS;
          valStrStr >> iS;
	  doRejectSt6 = (iS == 1);
          if ((myRank == 0) && doRejectSt6)  std::cerr << "We will not use the station 6 (Applicable only to Phase1)    " << std::endl;
          if ((myRank == 0) && (!doRejectSt6))  std::cerr << "We will use relevantSt6 (default)   " << std::endl;
	  if (doRejectSt6) RejectedStation = 6;
        } else if (parStr == std::string("rejectSt5X")) { // simple but repeated.. 
          int iS;
          valStrStr >> iS;
	  doReject5XView = (iS == 1);
        } else if (parStr == std::string("rejectSt5Y")) { 
          int iS;
          valStrStr >> iS;
	  doReject5YView = (iS == 1);
        } else if (parStr == std::string("rejectSt5V")) {  
          int iS;
          valStrStr >> iS;
	  doReject5VView = (iS == 1);
         } else if (parStr == std::string("rejectSt6X")) { 
          int iS;
          valStrStr >> iS;
	  doReject6XView = (iS == 1);
        } else if (parStr == std::string("rejectSt6Y")) { 
          int iS;
          valStrStr >> iS;
	  doReject6YView = (iS == 1);
        } else if (parStr == std::string("rejectSt6V")) { 
          int iS;
          valStrStr >> iS;
	  doReject6VView = (iS == 1);
        } else if (parStr == std::string("fitStationView")) { 
	  char stChar = valStr[0]; FittedStationNum = stChar - '0';
	  FittedViewChar = valStr[1];
	  if (myRank == 0) std::cerr << " We will fit only the view " << FittedViewChar << " on Station " << FittedStationNum << std::endl;
       } else if (parStr == std::string("nomMom")) {
          valStrStr >> nominalMomentum;
          if (myRank == 0) std::cerr << " The nominal momentum is      "  << nominalMomentum << std::endl;	
         } else if (parStr == std::string("nomMomDisp")) {
          valStrStr >> nominalMomentumDispersion;
          if (myRank == 0) std::cerr << " The nominal momentum Dispersion is      "  << nominalMomentumDispersion << std::endl;	
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
        } else if (parStr.find("VertY8Offset") != std::string::npos) {
          valStrStr >> VertY8Offset;
          if (myRank == 0) std::cerr << " Shifting the reference frame definig TransShiftY8 to  "  << VertY8Offset << std::endl;	
        } else if (parStr.find("IntegrationStep") != std::string::npos) {
          valStrStr >> integrationStepSize;
          if (myRank == 0)  {
	     if (integrationStepSize > 1.0e-6)  
	       std::cerr << " The Magnetic Field map will be upload and 3D track fit will fit momentum with step size "  
	                             << integrationStepSize << std::endl;
	     else if (std::abs(integrationStepSize) < 1.0e-6)		     
	       std::cerr << " Simple kick approximation will be used.   "  << std::endl;
	  }
        } else if (parStr.find("IntegrationEulerVsRK4") != std::string::npos) {
          valStrStr >> iOptEulerVsRK4;
          if (myRank == 0)  std::cerr << " IntegrationEulerVsRK4  flag "  << iOptEulerVsRK4 << std::endl;
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
        } else if (parStr.find("MagRotXY") != std::string::npos) {
          valStrStr >> magnetRotXY;
          if (myRank == 0) std::cerr << " The Magnetic Field map will be rotate around the Z axis by   "  
	                             << magnetRotXY << std::endl;
        } else if (parStr == std::string("FixMomentum")) {
	  int iii = 0;
          valStrStr >> iii;
	  fixMomentum = (iii == 1);
          if ((myRank == 0) && (iii == 1)) std::cerr << " While doing individual track fits, the momentum will be fixed at its Nominal Value " << std::endl;
          if ((myRank == 0) && (iii == 0)) std::cerr << " While doing individual track fits, the momentum will float " << std::endl;
        } else if (parStr == std::string("randomSeed")) {
          valStrStr >> aRandomSeed;
          if (myRank == 0) std::cerr << " The random seed will be   "  << aRandomSeed << std::endl;
        } else if (parStr == std::string("randomRollFact")) {
          valStrStr >> aRandomRollFactor;
	  if ((aRandomRollFactor < 0.) || (aRandomRollFactor > 1.0)) {
	    if (myRank == 0) std::cerr << " The random roll factor " << aRandomRollFactor 
	                               << " must be between 0. and 1. Fatal...    " << std::endl;
	    exit(2);
	  } 
          if (myRank == 0) std::cerr << " The random roll factor    "  << aRandomRollFactor << std::endl;
        } else if (parStr == std::string("token")) {
          token = valStr;
          if (myRank == 0) std::cerr << " Token will be   "  << token << std::endl;
        } else if (parStr == std::string("PrevBaseToken")) {
          tokenPrevBase = valStr;
          if (myRank == 0) std::cerr << " The token for the seed MinValue Alignment will be    "  << tokenPrevBase << std::endl;
        } else if (parStr == std::string("limRolls")) {
          valStrStr >> limRolls;
          if (myRank == 0) std::cerr << " The maximum roll angle will be   "  << limRolls << std::endl;
        } else {
          if (myRank == 0) std::cerr << " Unrecognized argument   "  << parStr <<  " fatal, quit here and now " << std::endl;
          MPI_Finalize();
          exit(2);
        }
       }   
    }
    
    std::srand(aRandomSeed); 
    
    if ((fitType != std::string("2DX")) && (fitType != std::string("2DY")) && 
        (fitType != std::string("3D")) && (fitType != std::string("DumpInput"))) {
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
    if (myRank == 0) std::cerr << " HOSTNAME is " << myHostName << std::endl; 
    if (myHostName.find("spectrum") == std::string::npos) topDirAll = std::string("/work1/next/lebrun/EMPHATIC/Data/"); // On fnal Wilson
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
	 (token.find("Try3D_R1366_4f") != std::string::npos) ||
	 (token.find("Try3D_R1366_4g") != std::string::npos) ||
	 (token.find("Try3D_R1366_4h") != std::string::npos) ||
	 (token.find("Try3D_R1366_4i") != std::string::npos) ||
	 (token.find("Try3D_R1366_4j") != std::string::npos) ||
	 (token.find("Try3D_R1366_4e") != std::string::npos))) 
	     aFileDescr = std::string("_TgtGraphite120Gev_BrickV2_1o1g_V1g.dat");
    if ((runNum == 1043) && ((token.find("TightCl_1a") != std::string::npos) ||
                             (token.find("TightCl_1b") != std::string::npos)))  aFileDescr = std::string("_5St_try9_AlignUV_TightCluster_1a_V1g.dat");
    if ((runNum == 1043) && (token.find("TightCl_1c") != std::string::npos))  aFileDescr = std::string("_5St_try9_AlignUV_TightCluster_1b_V1g.dat");
    if ((runNum == 1293) && (token.find("TgtMag09") != std::string::npos))  aFileDescr = std::string("_SimProtonMisA1TgtMag09a_V1g.dat");
//    if ((runNum == 1293) && (token.find("TgtMag09") != std::string::npos))  aFileDescr = std::string("_SimProtonMisA0TgtMag09a_V1g.dat"); // A0 is not misaligned, review the 
    // U V orientation  (it has been flipped by 90 degrees, I suspect.. To be checked.. 
//    if ((runNum == 1293) && (token.find("NoTgtNoMag09d") != std::string::npos))  aFileDescr = std::string("_SimProtonNoMisNoTgtNoMagB0TgtMag09d_V1g.dat");
    if ((runNum == 1293) && (token.find("NoTgtNoMag09d") != std::string::npos))  aFileDescr = std::string("_SimMuonNoMisNoTgtNoMagB0TgtMag09d_V1g.dat"); // August 7 
    if ((runNum == 1293) && (token.find("NoTgtNoMag09e") != std::string::npos))  aFileDescr = std::string("_SimMuonMisSt2A1NoTgtNoMagB0TgtMag09d_V1g.dat"); // August 8 & 9 
    if ((runNum == 1293) && (token.find("NoTgtNoMag09f") != std::string::npos))  aFileDescr = std::string("_SimMuonMisSt2A1NoTgtNoMagB0TgtMag09d_V1g.dat"); // August 9, late 
    if ((runNum == 1293) && (token.find("MisMod5NoTgt") != std::string::npos))  aFileDescr = std::string("_SimMuonMisMod5NoTgtNoMagB0TgtMag09d_V1g.dat"); // August 11, mid-day 
    if ((runNum == 1293) && (token.find("MisMod6NoTgt") != std::string::npos))  aFileDescr = std::string("_SimMuonMisMod6NoTgtNoMagB0TgtMag09d_V1g.dat"); // August 12, late morning
    // ==> Re ran, August 14 
    if ((runNum == 1293) && (token.find("MisMod7NoTgt") != std::string::npos))  aFileDescr = std::string("_SimMuonMisMod7NoTgtNoMagB0TgtMag09d_V1g.dat"); // August 12, late morning
    if ((runNum == 1293) && (token.find("MisMod7bNoTgt") != std::string::npos))  aFileDescr = std::string("_SimMuonMisMod7bNoTgtNoMagB0TgtMag09d_V1g.dat"); // August 12, late morning
    if ((runNum == 1293) && (token.find("MisMod6bNoTgt") != std::string::npos))  aFileDescr = std::string("_SimMuonMisMod6NoTgtNoMagB0TgtMag09b_V1g.dat"); // August 12, late morning
    if ((runNum == 1274) && (token.find("Try3D_R1274_1") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A1e_1p1_a_V1g.dat"); // Sept 16, mid-morning
    if ((runNum == 1274) && (token.find("Try3D_R1274_2") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A1e_1p1_a_V1g.dat"); // Sept 16, mid-morning
    if ((runNum == 1274) && (token.find("Try3D_R1274_3") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A1e_1p1_a_V1g.dat"); // Sept 23, mid-morning
    if ((runNum == 1274) && (token.find("Try3D_R1274_4") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A1e_1p1_a_V1g.dat"); // Silly.... 
    // tweaking the multiple scattering, and implementing momentum dispersion. 
    if ((runNum == 1274) && (token.find("Try3D_R1274_5") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A1e_1p1_a_V1g.dat"); // Silly.... 
    // Asking 16 out of 16, with pre-aligned and tight chi-sq cuts on triplet. 
    if ((runNum == 1274) && (token.find("Try3D_R1274_6") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A2e_6a1_1_V1g.dat"); // Not so Silly.... 
    if ((runNum == 1274) && (token.find("Try3D_R1274_7") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A2e_7a1_1_V1g.dat"); // Not so Silly.... 
    if ((runNum == 2098) && (token.find("Try3D_R2098_7") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A2e_7a_7a1_9_V2a.dat"); 
    if ((runNum == 2113) && (token.find("Try3D_R2113_7") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A2e_7a_7a1_9_V2a.dat"); 
                            // Against my best whishes, Nov. Dec 2023.  
    if ((runNum == 2144) && (token.find("Try3D_R2144_8") != std::string::npos))  aFileDescr = std::string("_NoTgt120Gev_ClJan_A2i_7a_7s1104_5_MSt6_V2a.dat"); 
    if ((runNum == 2144) && (token.find("Try3D_R2144_9") != std::string::npos))  aFileDescr = std::string("_NoTgt120Gev_ClJan_A2i_7a_7s1104_5_MSt6_to9s0001_V2a.dat"); 
    if ((runNum == 2144) && (token.find("Try3D_R2144_10") != std::string::npos))  aFileDescr = std::string("_NoTgt120Gev_ClJan_A2i_7a_7s1104_5_MSt6_to9s0001_V2a.dat"); 
    if ((runNum == 2144) && (token.find("Try3D_R2144_11") != std::string::npos))  aFileDescr = std::string("_NoTgt120Gev_ClJan_A2i_7a_7s1104_5_MSt6_to9s0001_V2a.dat"); 
    if ((runNum == 2144) && (token.find("Try3D_R2144_11") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A2e_7a_7a1_9_V2a.dat"); 
    if ((runNum == 2113) && (token.find("Try3D_R2113_11") != std::string::npos))  aFileDescr = std::string("_NoTgt31Gev_ClSept_A2e_7a_7a1_9_V2a.dat"); 
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
     myBTIn.SetForPhase1c(IsPhase1c); 
     myBTIn.RejectMultTracks(arbInput); 
     
     int numExpected = 67272; // I know this number from running SSDAlign Stu1 Algo1 on run 1055. 
     if ((strictSt6Y) && (!strictSt6X))  { numExpected = 52842; myBTIn.SetKey(687401); }
     if ((strictSt6X)  && (!strictSt6Y)) { numExpected = 49651; myBTIn.SetKey(687402); }
     if ((strictSt6X)  && (strictSt6Y)) { 
        numExpected = 41321; myBTIn.SetKey(687403); // Only valid for run 1055
	if (G4EMPHDescrToken != std::string("none"))  numExpected = -1; // Variable, too much clerical work with this check. 
	if (runNum == 1366) myBTIn.SetKey(687503);
	if (runNum == 1274) myBTIn.SetKey(687503);
     }
     if (IsPhase1c) myBTIn.SetKey(687603); 
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
      
     if ((world_size == 1) && (fitType == "DumpInput")) {
       std::cerr << " .... this is short, dumping data from " << myBTIn.GetNumEvts() << std::endl;
       myBTIn.DumpCVSForR(0, 10000, 'X', token); 
       myBTIn.DumpCVSForR(0, 10000, 'Y', token); 
       myBTIn.DumpCVSForR(0, 10000, 'U', token); 
       myBTIn.DumpCVSForR(0, 10000, 'V', token); 
       std::cerr << " .... this analysis is done ! " << std::endl; MPI_Finalize(); exit(2);
     } 
     
     emph::rbal::distributeEvts(myBTIn); 
    //
    // Now deal with the parameters.. 
    //
    
//    if (myRank == 0) std::cerr << " ... Instantiating the Geometry...  " << std::endl;
    
    emph::rbal::BTAlignGeom *myGeo = emph::rbal::BTAlignGeom::getInstance();
    emph::rbal::BTAlignGeom1c *myGeo1c = emph::rbal::BTAlignGeom1c::getInstance();
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
      if (std::abs(magnetRotXY) > 1.0e-3) myMagField->SetReAlignRotXY(magnetRotXY);
      myGeo->SetIntegrationStepSize(integrationStepSize);
      myGeo1c->SetIntegrationStepSize(integrationStepSize);
    } 
    emph::rbal::SSDAlignParams *myParams = emph::rbal::SSDAlignParams::getInstance(); 
    myParams->SetForPhase1c(IsPhase1c); 
    myParams->SetMaximumRolls(limRolls);  

    myParams->SetStrictSt6(strictSt6X || strictSt6Y); 
    //
    myParams->SetMode(fitType);
    myParams->SetSoftLimits(doSoftLimits);
    
    if (std::abs(assumedDoubleGap) >  1.0e-6) myParams->SetDoubleGaps(0.5*assumedDoubleGap); 
    if (IsPhase1c &&(std::abs(VertY8Offset) >  1.0e-6)) myParams->SetVerticalY8(VertY8Offset); 

    if ((fitType == std::string("2DX")) || (fitType == std::string("3D"))) {
      for (size_t k=0; k!= 8; k++) myGeo->SetUnknownUncert('X', k, 0.000000025); // a fraction of the strip.. 
      for (size_t k=0; k!= 8; k++) myGeo1c->SetUnknownUncert('X', k, 0.000000025); // a fraction of the strip.. 
      // 
    }
   bool isSetFor120Run1055 = false;
   bool isSetFor30GeVBasedOnRun1055 = false;
   
   if(doPencilBeam120) myGeo->SetUncertErrorOutOfPencilBeam();
   // Only for the last station!!!  
   if(doRejectVViews) (IsPhase1c) ? myGeo1c->SetUnknownUncert('V', 2, 5000.) : myGeo->SetUnknownUncert('V', 2, 5000.);
   if(doRejectVViews) (IsPhase1c) ? myGeo1c->SetUnknownUncert('V', 3, 5000.) : myGeo->SetUnknownUncert('V', 3, 5000.);
   if(doReject5XView && IsPhase1c) {
          myGeo1c->SetUnknownUncert('X', 5, 5000.) ; myGeo1c->SetUnknownUncert('X', 6, 5000.);
   }	  
   if(doReject5YView && IsPhase1c) {
      myGeo1c->SetUnknownUncert('Y', 5, 5000.) ; myGeo1c->SetUnknownUncert('Y', 6, 5000.);
   }
   if(doReject5VView && IsPhase1c) {
      myGeo1c->SetUnknownUncert('V', 0, 5000.) ; myGeo1c->SetUnknownUncert('V', 1, 5000.);
   }
   if(doReject6XView && IsPhase1c) {
          myGeo1c->SetUnknownUncert('X', 7, 5000.) ; myGeo1c->SetUnknownUncert('X', 8, 5000.);
   }	  
   if(doReject6YView && IsPhase1c) {
      myGeo1c->SetUnknownUncert('Y', 7, 5000.) ; myGeo1c->SetUnknownUncert('X', 8, 5000.);
   }
   if(doReject6VView && IsPhase1c) {
      myGeo1c->SetUnknownUncert('V', 2, 5000.) ; myGeo1c->SetUnknownUncert('V', 3, 5000.);
   }
   if (IsPhase1c && doRejectSt6) {
      myGeo1c->SetUnknownUncert('V', 2, 5000.); myGeo1c->SetUnknownUncert('V', 3, 5000.);
      myGeo1c->SetUnknownUncert('X', 7, 5000.); myGeo1c->SetUnknownUncert('X', 8, 5000.);
      myGeo1c->SetUnknownUncert('Y', 7, 5000.); myGeo1c->SetUnknownUncert('Y', 8, 5000.);
   }
   if (IsPhase1c && (RejectedStation == 5)) {
      myGeo1c->SetUnknownUncert('V', 0, 5000.); myGeo1c->SetUnknownUncert('V', 1, 5000.);
      myGeo1c->SetUnknownUncert('X', 5, 5000.); myGeo1c->SetUnknownUncert('X', 6, 5000.);
      myGeo1c->SetUnknownUncert('Y', 5, 5000.); myGeo1c->SetUnknownUncert('Y', 6, 5000.);
   }
   if (IsPhase1c && (RejectedStation == 56)) {
      myGeo1c->SetUnknownUncert('V', 0, 5000.); myGeo1c->SetUnknownUncert('V', 1, 5000.);
      myGeo1c->SetUnknownUncert('X', 5, 5000.); myGeo1c->SetUnknownUncert('X', 6, 5000.);
      myGeo1c->SetUnknownUncert('Y', 5, 5000.); myGeo1c->SetUnknownUncert('Y', 6, 5000.);
      myGeo1c->SetUnknownUncert('V', 2, 5000.); myGeo1c->SetUnknownUncert('V', 3, 5000.);
      myGeo1c->SetUnknownUncert('X', 7, 5000.); myGeo1c->SetUnknownUncert('X', 8, 5000.);
      myGeo1c->SetUnknownUncert('Y', 7, 5000.); myGeo1c->SetUnknownUncert('Y', 8, 5000.);
   }
   if (IsPhase1c && (RejectedStation == 4)) {
        myGeo1c->SetUnknownUncert('X', static_cast<size_t>( RejectedStation), 5000.);
        myGeo1c->SetUnknownUncert('Y', static_cast<size_t>( RejectedStation), 5000.);
   }
   if (IsPhase1c && ((RejectedStation == 3) || ((RejectedStation == 2)))) {
        myGeo1c->SetUnknownUncert('X', static_cast<size_t>( RejectedStation), 5000.);
        myGeo1c->SetUnknownUncert('Y', static_cast<size_t>( RejectedStation), 5000.);
        myGeo1c->SetUnknownUncert('U', static_cast<size_t>( RejectedStation-2), 5000.);
   }
   // a more general way... Above needs clean up.. 
   //
   if (IsPhase1c && (rejectedStationsIndices.size() != 0)) 
       myGeo1c->SetUnknownUncertForStations(rejectedStationsIndices); 
    // New method, reload from a file... 
    //
    if (doLoadParamsFromPreviousRun) {
      if (iOptStartFromPrev == 2) {
        if (tokenPrevBase == std::string("none")) {
          if (myRank == 0) std::cerr << " Requesting an other Previous Alignment Params set, but no tokenPrevBase given... Fatal " << std::endl;
	  MPI_Finalize();
	  exit(2);
        }
	myParams->LoadValueFromPreviousRun(tokenPrevBase, true, true); 
      } else { 
        myParams->LoadValueFromPreviousRun(token, true, false); 
      }
    }
    if (doLoadParamsFromPreviousFCNHist) {
      myParams->LoadValueFromPreviousFCNHistory(token, requestFCNCallNumber); // in case the previous is stuck in a Minuit inf loop (ill-posed problem ).  
    }
    if (aRandomRollFactor > 1.0e-6) myParams->RandomizeRollsAndRollCenters(aRandomRollFactor, aRandomRollFactor);
    
    if (myRank == 0) myParams->DumpTable(token); 
//    if (myRank == 0) std::cerr << " Booby trap, stop now " << std::endl;
//    MPI_Finalize();
//    exit(2);
//
// Feb 2024 : optimization of the Magnetic field integrator.. 
//
    emph::rbal::BTMagneticField *myMagField = emph::rbal::BTMagneticField::getInstance();
    if (integrationStepSize > 0.5) myMagField->SetIntegratorSteps(integrationStepSize);
//    if (myRank == 0) std::cerr << " Booby trap, after setting the Integrator step size.. stop now " << std::endl;
//    MPI_Finalize();
//    exit(2);
//    myMagField->StuKick1(iOptEulerVsRK4);
//    if (myRank == 0) std::cerr << " Booby trap, after studying magnetic kick .. stop now " << std::endl;
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
      if ((myRank == 0) && (aName.find("Roll_") != std::string::npos)) std::cerr << " ... Uploading param " << aName << " value " << aValue << std::endl; 
      uPars.Add(aName, aValue, err);
//      if (!doSoftLimits) uPars.SetLimits(aName, itP->Limits().first, itP->Limits().second);
      uPars.SetLimits(aName, itP->Limits().first, itP->Limits().second);
//      if (myRank == 0) std::cerr << " Limits set for parameter " << aName << " are " << itP->Limits().first << ", " << itP->Limits().second << std::endl;
    }
    
    
    emph::rbal::BeamTrackSSDAlignFCN theFCN(fitType, &myBTIn);
    theFCN.SetForPhase1c(IsPhase1c);
    theFCN.SetEulervsRK4(iOptEulerVsRK4);
    theFCN.SetMCFlag(G4EMPHDescrToken != std::string("none"));
    if (magnetIsRemoved) theFCN.SetNoMagnet(true);
    theFCN.SetSoftLimits(doSoftLimits);
//    if ((SoftLimitEpsilY != 0) && (SoftLimitEpsilY != INT_MAX)) theFCN.SetAssumedSlopeSigma(SoftLimitEpsilY*1.0e-6); // actually, emittance in the Y plane 
    // Units are mm mrad. 
    theFCN.SetSoftLimitDoubleSensorCrack(SoftLimitGeomSt4St5);
    theFCN.SeMinimumDoubleSensorCrack(MinimumGapSt4St5);
    theFCN.SetBeamConstraint(applyEmittanceConstraint);
    theFCN.SetNominalMomentum(nominalMomentum);
    theFCN.SetNominalMomentumDisp(nominalMomentumDispersion);
    theFCN.SetAlignMode(fixMomentum); // In this context, kind of obvious. 
    theFCN.SetUpLimForChiSq(uPLimitOnChiSqTrackFits);
    if ((SoftLimitEpsilY != 0) && (SoftLimitEpsilY != INT_MAX)) theFCN.SetAssumedEpsilY(SoftLimitEpsilY*1.0e-3); // actually, emittance in the Y plane 
    if (fitSubType == std::string("TrShiftYOnly")) {
      theFCN.SetSelectedView('Y'); std::cerr << " 3D fit, but fit only the non-bend view Y " << std::endl;
    }
    if (fitSubType == std::string("TrShiftXOnly")) {
      theFCN.SetSelectedView('X'); std::cerr << " 3D fit, but fit only the X view data " << std::endl;
    }
    if ((fitSubType == std::string("TrShiftXYOnly")) || (fitSubType == std::string("TrRollCenterShiftXYOnly"))) {
      theFCN.SetSelectedView('Z'); std::cerr << " 3D fit, but fit only the X and Y view data " << std::endl;
    }
    if (fitSubType == std::string("TrShiftXYWOnly")) {
      theFCN.SetSelectedView('W'); std::cerr << " 3D fit, but fit only the X, Y and W view data (no station 4 & 5 U views " << std::endl;
    }
    if (fitSubType == std::string("TrShiftXYUOnly")) {
      theFCN.SetSelectedView('U'); std::cerr << " 3D fit, but fit only the X, Y and U view data (no station 2 & 3 W views " << std::endl;
    }
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
    if (doRejectVViews) myParams->FixParamsForView('V'); // we will also fix station 6 !!! 
    if (doReject5XView) {
      myParams->FixParamsForViewAtStation(5, 'X'); // we will also fix station 6 !!! 
//      myParams->FixParamsUpstreamMagnet(); // Remove, for series 7p61
    } 
    if (doReject5YView) {
      myParams->FixParamsForViewAtStation(5, 'Y'); // we will also fix station 6 !!!
//      myParams->FixParamsUpstreamMagnet(); 
    } 
    if (doReject5VView) {
      myParams->FixParamsForViewAtStation(5, 'V'); // we will also fix station 6 !!! 
//      myParams->FixParamsUpstreamMagnet(); 
    }
    if (doReject6YView) {
      myParams->FixParamsForViewAtStation(6, 'Y'); // we will also fix station 6 !!! 
//      myParams->FixParamsUpstreamMagnet(); 
    }
    if (doReject6XView) {
      myParams->FixParamsForViewAtStation(6, 'X'); // we will also fix station 6 !!! 
//      myParams->FixParamsUpstreamMagnet(); 
    }
    if (doReject6VView) {
      myParams->FixParamsForViewAtStation(6, 'V'); // we will also fix station 6 !!! 
//      myParams->FixParamsUpstreamMagnet(); 
    }
    if (token.find("7o61") != std::string::npos) { // Investigate Phase1c Station 5 on run 2113 ... Not a Z shift...angles are too small.. 
       theFCN.SetAllowLongShiftByStation(true);
       myParams->FixParamsForView('Y', false, std::string("LongShift_Y_5"));  // not a typo!!! we move by station... 
       myParams->FixParamsForView('Y', false, std::string("LongShift_Y_6"));
    }
    if (IsPhase1c && doRejectSt6) myParams->FixParamsForViewLastStation('A'); // A stand for all of them.. 
    if (IsPhase1c && (RejectedStation > 1) && (RejectedStation != 56)) myParams->FixParamsForAllViewsAtStation(RejectedStation); // All sensor for this station.. 
    if (IsPhase1c && (rejectedStationsIndices.size()  != 0)) {
      for (size_t k=0; k != rejectedStationsIndices.size(); k++) 
        myParams->FixParamsForAllViewsAtStation(rejectedStationsIndices[k]); // All sensor for this station.. 
    }
    if (IsPhase1c && (RejectedStation == 56)) {
       myParams->FixParamsForAllViewsAtStation(5); 
       myParams->FixParamsForAllViewsAtStation(6); 
    }
    if (IsPhase1c && ((FixedRollStation != -1) && (FixedRollStation < 7))) {
       myParams->FixRollAndRollCenterForStation(FixedRollStation); 
    }
    if (IsPhase1c && (FittedStationNum != -1)) {
       myParams->FixAllButStationView(static_cast<size_t>(FittedStationNum), FittedViewChar); 
    }    
   if (doAntiPencilBeam120OnlyV && doAntiPencilBeam120) {
       myParams->FixParamsForView('X');
       myParams->FixParamsForView('Y');
       myParams->FixParamsForView('W');
       if (fitSubType == "TrRollCenterShift") {
         myParams->FixParamsForView('U', false, std::string("TransShift"));
         myParams->FixParamsForView('U', false, std::string("DeltaRoll"));
         myParams->FixParamsForView('U', false, std::string("DeltaRollCenter"));
	 // Adding the V_3.. Poorly set in the 120 alignment.. 
	 myParams->FixMinuitParam(emph::rbal::TRSHIFT, 'U', 3, false);
	 myParams->FixMinuitParam(emph::rbal::ROLL, 'U', 3, false);
	 myParams->FixMinuitParam(emph::rbal::ROLLC, 'U', 3, false);
       }
    }
    for (std::vector<emph::rbal::SSDAlignParam>::const_iterator itP = myParams->ItCBegin(); itP != myParams->ItCEnd(); itP++) {
         if (itP->isFixedInMinuit()) {
	   minSimple.Fix(itP->Name().c_str());
	 } else { 
	   if (myRank == 0)  
	      std::cerr << "  ..... on rank 0 Varying parameter " << itP->Name() 
	                << " limits " << itP->DownLimit() << ", " << itP->UpLimit() <<  std::endl;
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
//	 if (itP1->Name().find("TransShift") == 0) limitsP1 = std::pair<double, double>(-1.0, 1.0); August 9, NoTgtNoMag09e_1b1
//	 if (itP2->Name().find("TransShift") == 0) limitsP2 = std::pair<double, double>(-1.0, 1.0);
         if (token.find("NoTgtNoMag09f_1b1") != std::string::npos) { 
	   if (itP1->Name().find("TransShift_Y_1") == 0) limitsP1 = std::pair<double, double>(0.25, 0.75); // August 9, NoTgtNoMag09f_1b1
	   if (itP2->Name().find("TransShift_Y_5") == 0) limitsP2 = std::pair<double, double>(-0.25, 0.25);
	 } else if (token.find("MisMod5NoTgt_1a1") != std::string::npos) {
	   if (itP1->Name().find("TransShift_Y_1") == 0) limitsP1 = std::pair<double, double>(-0.2 -0.75, -0.2 + 0.75); // August 9, NoTgtNoMag09f_1b1
	   if (itP2->Name().find("TransShift_Y_5") == 0) limitsP2 = std::pair<double, double>(0.2 - 4.5, 0.2 + 4.5);
	 }
	 
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
	  
	  if ((!IsPhase1c) && aName.find("Tilt_Y_6") != std::string::npos) fOutMinRes << " TransShift_Y_7" << " " << 0.5*assumedDoubleGap << " 1.0e-8 " << std::endl; 
 	  if ((IsPhase1c) && aName.find("Tilt_Y_7") != std::string::npos) fOutMinRes << " TransShift_Y_8" << " " << 0.5*assumedDoubleGap << " 1.0e-8 " << std::endl; 
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
}
