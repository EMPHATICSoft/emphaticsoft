/*==============================================================================
================================================================================
================================================================================

EMPHATIC Mag I Helmholtz Produce

Started 5 Jan 2024, Leo Bellantoni, FRA.  All available rights reserved.

This code uses Helmholtz decomposition to interpolate the 5mm meshed map of the
magnetic field of the 1st magnet used by the EMPHATIC collaboration, the so-called
China magnet owned by TRIUMF.

The class ToDoAndDoneLists maintains pointers to 2 files; one is a list of places
where the Helmholtz decomposition calculation remains to be done and the other
is the results of that decomposition.  To provide resilience over long periods of
time (the complete calculation for a full map at 1mm spacing is expected to take
weeks), these lists are external files which are opened, amended, revised and
re-closed relatively quickly.  The header file for this class contains a standalone
function which will reproduce the initial file of points to be evaluated.

The class DryThreadHandler creates up to a certain number of threads and feeds
them to the Helmholtz evaluation fullIntegration(...) which is in integrands.h
The name of this class reflects the fact that it does not maintain an active
pool of threads to be started and stopped using conditions from the pthread library
or elsewhere.  The overhead of creating and destroying threads is not that large
compared to the computational costs of the Helmholtz decomposition.

The classes ProbeCalib and FieldMap handle the data from the original mapping.
The method FieldMap::interpolate(...) uses quadratic interpolation to determine
an estimate of the magnetic field from nearby measured points.  It has been
broken out into its own cpp file.

The first version of this code is developed in the special and uniquely wonderful
"Leo" framework.  A key part of that framework is the template ra<T> which is a
lightweight wrapper around std::valarray, a fast-access C++ standard template.
The ra<T> carries knowledge of its own size and can be indexed from 1 (the
FORTRAN or mathemetician's default) or any other value.  However, its construction
requires dynamic memory allocation which is slow for small ra<T> in loops.
Accordingly, the header file RA3.h emulates some of the ra<T> functionality with
simple old style C arrays.


*/
// #include "LppMain.h"  Removed for emphaticgpvm version
#include "general/LppGlobalDefs.h"
#include "io/plusORminus.h"
#include "lies/stats.h"
#include "lies/meansig.h"
#include "histograms/Lhist1.h"
#include "histograms/Lhist2.h"
#include "histograms/Lbins.h"
#include "GUI/Lfit1.h"
#include "GUI/Lzcol.h"

#include <future>
using namespace std;

#include "Funcs.h"
#include "RA3.h"
#include "Inputs.h"
#include "ToDoAndDoneLists.h"
#include "ThreadHandler.h"
#include "ProbeCalib.h"
#include "FieldMap.h"
#include "dBCorrect.h"
#include "integrands.h"





int main (/*int argc, char* const argv[]*/) {
#ifndef raBOUND
    cout << "ra<T> has NO bounds checking" << endl << endl;
#endif
#ifdef raBOUND
    cout << "ra<T> DOES have bounds checking" << endl << endl;
#endif






//==============================================================================
//==============================================================================
//==============================================================================
cout << "Analyzing for radius " << bestRadius << " for z from "
    << bestLoZ << " to " << bestHiZ << "." << endl << endl;



// One usually does not have to create a new list, as there is one incorporated
// into the files for this project.  But if you do need that, here is where it
// happens!
if (today==makeFreshList) {
    ToDoAndDoneLists listBungler;
    if (listBungler.createFreshList(bestRadius, bestLoZ, bestHiZ, stepX, stepY, stepZ))
        cout<< "A fresh list of points to analyze has been created." <<endl;
    exit(0);
}





if (today==crunchDecomposition) {
    // Construct the list handler and thread handler.  Arguments is in
    // integrands.h as it is the argument list for fullHelmholtz.
    ToDoAndDoneLists listHandler(dirName+toDoFileName, dirName+doneFileName);
    ThreadHandler<ra<double>, Arguments> threadHandler(numberWorkThreads);
    ThreadHandler<ra<double>, Arguments>::ChoreInfo aChore;

    // Get your probe calibration and field map
    ProbeCalib calibration(dirName+calibFileName);

    FieldMap* fieldMap;
    ra<double> off(3);
    fieldMap = new FieldMap(dirName+mapFileName, calibration, true);
    double initOff[3] = {0.0,  0.0, 0.0};
    off.fillFromC(initOff,3);
    fieldMap->setOffset(off);



    // "Lady and gentlemen, start your engines!"
    time_t unixTstart, unixTstop;
    time(&unixTstart);



    // Stack up chores
    FieldMapPoint bite;         pair<int,int> count;
    while (listHandler.fetchNewPoint(bite)) {
        // Put together the arguments
        ra<double> rArg(3);
        rArg(x) = bite.x;   rArg(y) = bite.y;   rArg(z) = bite.z;
        Arguments blahBlahBlah = {rArg, fieldMap, bestRadius, bestLoZ, bestHiZ};
        aChore = {&fullHelmholtz, blahBlahBlah};

        // Put aChore in the deque
        int tmpOut = threadHandler.addChore(aChore);
        count = threadHandler.threadCount();
        cout<< "Added a new chore and started " << tmpOut << " of them; thread counters: " << count.first coml count.second <<endl;
    }
    listHandler.outputToDoList(dirName+toDoFileName);

    time(&unixTstop);
    printf("Wall time to enque chores: %9.0f sec.\n", (double)(unixTstop -unixTstart));
    unixTstart = unixTstop;

    if (count.first==0 && count.second==0) {
        cout<< "Well, there is nothing in the queue.  Bye!" <<endl;
        exit(1);
    }

    std::pair<bool,ra<double>> anAnswer;    // bool is true if there's more work to do
    anAnswer.first = true;
    while (anAnswer.first) {
    
        anAnswer = threadHandler.getResult();

        time(&unixTstop);
        printf("Wall time to evaluate integrals: %9.0f sec.      ", (double)(unixTstop -unixTstart));
        count = threadHandler.threadCount();
        cout<< "Thread counts: " << count.first coml count.second <<endl;
   

        FieldMapPoint happiness;
        if (anAnswer.second.ne1 != 0) {
            happiness.x   = anAnswer.second(1);
            happiness.y   = anAnswer.second(2);
            happiness.z   = anAnswer.second(3);
            happiness.BHx = anAnswer.second(4);
            happiness.BHy = anAnswer.second(5);
            happiness.BHz = anAnswer.second(6);
            happiness.BCx = anAnswer.second(7);
            happiness.BCy = anAnswer.second(8);
            happiness.BCz = anAnswer.second(9);
            if (!listHandler.addToDoneList(happiness)) {
                cout<< "Failure to add to Done list.  Result was " <<endl;
                cout<< "R(x)" coml "R(y)" coml "R(z)" coml "Bh(x)" coml "Bh(y)"
                    coml "Bh(z)" coml "Bc(x)" coml "Bc(y)" coml "Bc(z)" <<endl;
                cout<< happiness.x coml happiness.y coml happiness.z coml
                    happiness.BHx coml happiness.BHy coml happiness.BHz coml
                    happiness.BCx coml happiness.BCy coml happiness.BCz <<endl;
                cout<< "Mission aborted!" <<endl;
            }
        }
    }
}





if (today==checkResults) {
    // Create the stdForm map files from the csv file which holds decomposition
    // results - these can be used by the FieldMap constructor.  Put 'em on the Desktop
    string decompFileName  = outputMapOnDesktop[0];
    string correctFileName = outputMapOnDesktop[1];
    fstream decompFile(decompFileName.c_str(),ios_base::out);
    fstream correctFile(correctFileName.c_str(),ios_base::out);

    ifstream headerFile;            string line;
    headerFile.open(dirName +"DecompositionHeader.txt", ios::in);
    while (getline(headerFile, line)) decompFile << line << "\n";
    headerFile.close();
    headerFile.open(dirName +"CorrectedHeader.txt", ios::in);
    while (getline(headerFile, line)) correctFile << line << "\n";
    headerFile.close();

    inputtextfile readDecomp(dirName +checkFileName);
    double x,y,z, Bx,By,Bz;     char formattedLine[160];
    readDecomp.fetchline();     // Skip 1st line
    while (readDecomp.fetchline(",")!=-1) {
        x  = readDecomp.tokenasd(0);
        y  = readDecomp.tokenasd(1);
        z  = readDecomp.tokenasd(2);
        Bx = readDecomp.tokenasd(3);
        By = readDecomp.tokenasd(4);
        Bz = readDecomp.tokenasd(5);
        // mm & Tesla for these files
        sprintf(formattedLine, "%-25g %-24g %-24g %-24g %-24g %-24g",
                    1000*x,1000*y, 1000*z, Bx,By,Bz);
        decompFile << formattedLine << endl;
        
        Bx = readDecomp.tokenasd(6);
        By = readDecomp.tokenasd(7);
        Bz = readDecomp.tokenasd(8);
        sprintf(formattedLine, "%-25g %-24g %-24g %-24g %-24g %-24g",
                    1000*x,1000*y, 1000*z, Bx,By,Bz);
        correctFile << formattedLine << endl;
    }
    decompFile.close();
    correctFile.close();
    
    

    // Plot them and be gratified.
    ProbeCalib calibration(dirName +calibFileName);
    FieldMap decompField (decompFileName,       calibration,true);
    FieldMap correctField(correctFileName,      calibration,true);
    FieldMap originField (dirName +mapFileName, calibration,true);



    Lbins  bDivHisto(120, -8.0, +8.0);
    Lhist1 hDivDecomp (bDivHisto,"div(B) [T/m] - Helmholtz decomposed field map");
    Lhist1 hDivCorrect(bDivHisto,"div(B) [T/m] - Ad-hoc corrected field map");
    Lhist1 hDivOrigin (bDivHisto,"div(B) [T/m] - Original 5mm field map");
    
    Lbins  bDivAz(90,-pie, +pie);
    Lbins  bDivZ(100,-0.14,+0.31);
    Lbins  bDivR( 19,  0.0,+0.0185);
    Lbins  bDivT(19,-0.0190, +0.0190);
    Lbins  bDivO( 7,-0.0175, +0.0175);
    Lhist2 hDivOriginRvsZ  (bDivZ,bDivR, "|div(B)| [T/m] - Original 5mm field map, R vs Z");
    Lhist2 hDivOriginPhivsZ(bDivZ,bDivAz,"|div(B)| [T/m] - Original 5mm field map, phi vs Z");
    Lhist2 hDivOriginTrans (bDivO,bDivO, "|div(B)| [T/m] - Original 5mm field map, (x,y) plane");
    Lhist2 hDivDecompRvsZ  (bDivZ,bDivR, "|div(B)| [T/m] - Helmholtz decomposed field map, R vs Z");
    Lhist2 hDivDecompPhivsZ(bDivZ,bDivAz,"|div(B)| [T/m] - Helmholtz decomposed field map, phi vs Z");
    Lhist2 hDivDecompTrans (bDivT,bDivT, "|div(B)| [T/m] - Helmholtz decomposed field map, (x,y) plane");
    


    meansig divStats;       FieldMap::FieldMapInd ind;          double div,r,phi;
    for (FieldMap::FieldMapItr itr=decompField.pBegin; itr<decompField.pEnd; ++itr) {
        decompField.findInd(itr,ind);
        div = itr->valid ? decompField.findDiv(ind) : Dbig;
        if (div!=Dbig) {
            divStats.push(div);
            hDivDecomp.Fill(div);
            r   =  Qadd(itr->x,itr->y);
            phi = atan2(itr->y,itr->x);
            hDivDecompRvsZ.Fill  (itr->z,r,     fabs(div));
            hDivDecompPhivsZ.Fill(itr->z,phi,   fabs(div));
            hDivDecompTrans.Fill (itr->x,itr->y,fabs(div));
        }
    }
    cout << "Mean divergence in Helmholtz decomposed field is     " <<
        plusORminus::formatted(divStats.mean(),divStats.mean_err(),plusORminus::milli,"T/m") <<
        " & width is " << divStats.stdev() << " T/m" << endl;

    divStats.reset();
    for (FieldMap::FieldMapItr itr=correctField.pBegin; itr<correctField.pEnd; ++itr) {
        correctField.findInd(itr,ind);
        div = itr->valid ? correctField.findDiv(ind) : Dbig;
        if (div!=Dbig) {
            divStats.push(div);
            hDivCorrect.Fill(div);
        }
    }
    cout << "Mean divergence in ad-hoc corrected field is         " <<
        plusORminus::formatted(divStats.mean(),divStats.mean_err(),plusORminus::milli,"T/m") <<
        " & width is " << divStats.stdev() << " T/m" << endl;

    divStats.reset();
    for (FieldMap::FieldMapItr itr=originField.pBegin; itr<originField.pEnd; ++itr) {
        if (itr->z<bestLoZ || bestHiZ<itr->z) continue; // Apples to apples
        originField.findInd(itr,ind);
        div = itr->valid ? originField.findDiv(ind) : Dbig;
        if (div!=Dbig) {
            div *= scaleOrigBy;
            divStats.push(div);
            hDivOrigin.Fill(div);
            r   =  Qadd(itr->x,itr->y);
            phi = atan2(itr->y,itr->x);
            hDivOriginRvsZ.Fill  (itr->z,r,     fabs(div));
            hDivOriginPhivsZ.Fill(itr->z,phi,   fabs(div));
            hDivOriginTrans.Fill (itr->x,itr->y,fabs(div));
        }
    }
    cout << "Mean divergence in original 5mm field (z limited) is " <<
        plusORminus::formatted(divStats.mean(),divStats.mean_err(),plusORminus::milli,"T/m") <<
        " & width is " << divStats.stdev() << " T/m" << endl;



    Lfit1 pDivDecomp(hDivDecomp);
    pDivDecomp.logY  = true;
    pDivDecomp.show();
    hDivDecomp.CSVout("DecompPlot",true);

    Lfit1 pDivCorrect(hDivCorrect);
    pDivCorrect.logY = true;
    pDivCorrect.show();
    hDivCorrect.CSVout("CorrectPlot",true);

    Lfit1 pDivOrigin(hDivOrigin);
    pDivOrigin.logY = true;
    pDivOrigin.show();
    hDivOrigin.CSVout("OriginPlot",true);
 
    Lzcol pDivDecompRvsZ(hDivDecompRvsZ);
    pDivDecompRvsZ.logZ = true;
    pDivDecompRvsZ.show();

    Lzcol pDivDecompPhivsZ(hDivDecompPhivsZ);
    pDivDecompPhivsZ.logZ = true;
    pDivDecompPhivsZ.show();

    Lzcol pDivDecompTrans(hDivDecompTrans);
    pDivDecompTrans.ZlowAuto = pDivDecompTrans.ZhighAuto = false;
    pDivDecompTrans.ZhighLim = 1e4*(pDivDecompTrans.ZlowLim = 0.1);
    pDivDecompTrans.logZ = true;
    pDivDecompTrans.show();

    Lzcol pDivOriginRvsZ(hDivOriginRvsZ);
    pDivOriginRvsZ.logZ = true;
    pDivOriginRvsZ.show();

    Lzcol pDivOriginPhivsZ(hDivOriginPhivsZ);
    pDivOriginPhivsZ.logZ = true;
    pDivOriginPhivsZ.show();

    Lzcol pDivOriginTrans(hDivOriginTrans);
    pDivOriginTrans.ZlowAuto = pDivOriginTrans.ZhighAuto = false;
    pDivOriginTrans.ZhighLim = 1e4*(pDivOriginTrans.ZlowLim = 0.1);
    pDivOriginTrans.logZ = true;
    pDivOriginTrans.show();
}

}// end of main.
