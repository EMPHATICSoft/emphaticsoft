#!/bin/bash
export MPICXX=/usr/lib64/openmpi/bin/mpicxx
export MPIRUN=/usr/lib64/openmpi/bin/mpirun
#
# Note : to build out-of-source , execute cmake -B . -S ../src 
#
# Example of what to run.  See BTFitSimplex code for th path of the input file. 
#
aToken="Try3D_R2113_7s1001_5" 
# p series: freeze all Upstream of Magnet params.. oriented only St5X (3 params, that is it..) 
aSubFitMode="TrRollCenterShift"
aRunNum="2113"
aVersionList="1"
aDay="03"
aTokenList="PMom120WideBeam"
time $MPIRUN --np 1 /home/lebrun/EMPHATIC/SSDAlign/build/BTFitSimplex \
 token $aToken fitType 3D fitSubType $aSubFitMode runNum $aRunNum  phase1c 1 RemoveMagnet 1 limRolls 0.05 spill 10 \
 softLimits 1000000 UpLimChisSqTr 1.0e7 DoubleGap 0 maxEvts 1000000 nomMom 120. IntegrationStep 3.0 FixMomentum 1 \
 softLimitGeoSt4St5 0. EdmMaxTolerance 50. maxFCNCalls 20000 startFromPrev 0 callFCNOnce 0 VertY8Offset -1.0 \
>& tt_BTFitSimplex_${aSubFitMode}_${aRunNum}_${aToken}_${aTokenList}_Jan_${aDay}_${aVersionList}.lis 
date
ls -ltr tt_BTFitSimplex_${aSubFitMode}_${aRunNum}_${aToken}_${aTokenList}_Jan_${aDay}_${aVersionList}.lis
