#include <string>
using namespace std;





string dirName = "./";

string mapFileName   = "mfMapMeasured_5mm.txt";
string calibFileName = "Probe_calibration_Mar2023.csv";
string toDoFileName  = "ToDoFile.csv";
string doneFileName  = "Results.csv";
string checkFileName = "Results.csv";
string headerFileName[2] = {"DecompositionHeader.txt",
                            "CorrectedHeader.txt"};

string outputMapOnDesktop[2] = {"mfMapHelmholtz.txt",
                                "mfMapCorrected.txt"};

double bestRadius =  0.017464;
double bestLoZ    = -0.1405;    // out an extra half-millimeter to avoid
double bestHiZ    =  0.3105;    // poles in the integrand.





enum WhatToDo {makeFreshList, crunchDecomposition, checkResults};
WhatToDo today = makeFreshList;





// If making a fresh list, select the steps here.  Output will appear on
// desktop in a file named FreshList.csv.  Move that into toDoFileName.
double stepX = 0.002;      double stepY = 0.002;      double stepZ = 0.002;




int numberWorkThreads =  4;     // The main thread here will be mostly suspended and won't
            // soak up any core's clock cycles much.  On an Apple M1, when the thermal load
			// gets high, the system slows down its clocks.  Try powermetrics -s thermal or
            // powermetrics --poweravg 60 | grep CPU | grep "active residency"
            // 5 threads seems to be a level that prevents throttling; 8 is a level where
            // other use of this laptop is hindered.
			// On emphaticgpvm01 - 3, there are 4 cores and a somewhat different architecture.
			// Values of 4 will result in this process taking up as much of the machine as
			// it can; values above 4 do not seem to result in faster execution and values
			// below 4 cap the fraction of the machine's CPU that will be used by this process.


int VEGAS_chatty      = -1;
int VEGAS_nIterations = 10;

int VEGAS_callsCylinder = 1000000;
int VEGAS_callsDisk     = 0.0189 *VEGAS_callsCylinder;      // Approximate area ratio





double scaleOrigBy = 21.03;
