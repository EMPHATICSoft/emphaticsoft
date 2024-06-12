# GNU Makefile for the L++ project to do a Helmholtz decomposition of the magnetic field
# for the first "Phase I" or "China" magnet in the EMPHATIC experiment.
#
# Leo Bellantoni, Jan 2024
#
#
CXX       = g++ -std=gnu++17
HEADFLAGS = -I. -IC--/ -ICodeGuts/ -Ifunctionals/ -Ifunctions/ -Igeneral/ -IGUI/ -Ihistograms/ -Iio/ -Ilies/ -Iparticles/ -Ira/ -Irandoms/ -Iwhen?/ -IXML/
LIBFLAGS  = -lpthread
DEBFLAGS  = -g3 -Wall -Wno-reorder -Wno-unused-variable -Wno-nonnull-compare
OPTFLAGS  = -O3
PROFLAGS  = -pg -O3
OBJFILES = ToDoAndDoneLists.o ProbeCalib.o FieldMap.o interpolate.o poly3int.o
LPPFILES = LppExcept.o LppError.o inputtextfile.o Lvec.o Lmat.o quadd.o fitter.o VEGAS.o ranunl.o stats.o meansig.o Lbins.o Lhistbin.o Lprofbin.o Lhist1.o Lhist2.o Lprof1.o Lprof2.o LppGNUpipe.o plotbase.o plotfit.o Lfit1.o Lzcol.o




all:
	(make MAIN)





MAIN: Funcs Inputs ToDoAndDoneLists ProbeCalib FieldMap dBCorrect integrands Lpp
	$(CXX) -c main.cpp						$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -o main.exe main.o $(OBJFILES) $(LPPFILES) $(LIBFLAGS)	$(OPTFLAGS) $(HEADFLAGS)
	rm *.o
DEBUG: Funcs Inputs ToDoAndDoneListsD ProbeCalibD FieldMapD dBCorrect integrands LppD
	$(CXX) -c main.cpp						$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -o main.exe main.o $(OBJFILES) $(LPPFILES) $(LIBFLAGS)	$(DEBFLAGS) $(HEADFLAGS)
	rm *.o



Funcs:
#	Just the Func.h file

Inputs:
#	Just the Inputs.h file

ToDoAndDoneLists:
	$(CXX) -c CodeGuts/ToDoAndDoneLists.cpp			$(OPTFLAGS) $(HEADFLAGS)
ToDoAndDoneListsD:
	$(CXX) -c CodeGuts/ToDoAndDoneLists.cpp			$(DEBFLAGS) $(HEADFLAGS)

ProbeCalib:
	$(CXX) -c CodeGuts/ProbeCalib.cpp			$(OPTFLAGS) $(HEADFLAGS)
ProbeCalibD:
	$(CXX) -c CodeGuts/ProbeCalib.cpp			$(DEBFLAGS) $(HEADFLAGS)

FieldMap:poly3int Funcs
	$(CXX) -c CodeGuts/FieldMap.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c CodeGuts/interpolate.cpp			$(OPTFLAGS) $(HEADFLAGS)
FieldMapD:poly3intD Funcs
	$(CXX) -c CodeGuts/FieldMap.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c CodeGuts/interpolate.cpp			$(DEBFLAGS) $(HEADFLAGS)

dBCorrect:

integrands: Funcs FieldMap RA3

RA3:

poly3int: RA3
	$(CXX) -c CodeGuts/poly3int.cpp				$(OPTFLAGS) $(HEADFLAGS)
poly3intD: RA3
	$(CXX) -c CodeGuts/poly3int.cpp				$(DEBFLAGS) $(HEADFLAGS)





# No dependencies are listed here for the L++ package, as it is expected to remain unchanged.
# However, certain specific parts of it will need to be compiled.
Lpp:
	$(CXX) -c problems/LppExcept.cpp 			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c problems/LppError.cpp 			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c io/inputtextfile.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c ra/Lvec.cpp					$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c ra/Lmat.cpp					$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c functions/quadd.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c functionals/fitter.cpp			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c functionals/VEGAS.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c randoms/ranunl.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c lies/stats.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c lies/meansig.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lbins.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lhistbin.cpp			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lprofbin.cpp			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lhist1.cpp 			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lhist2.cpp 			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lprof1.cpp 			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lprof2.cpp 			$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/LppGNUpipe.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/plotbase.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/plotfit.cpp				$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/Lfit1.cpp					$(OPTFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/Lzcol.cpp					$(OPTFLAGS) $(HEADFLAGS)

LppD:
	$(CXX) -c problems/LppExcept.cpp 			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c problems/LppError.cpp 			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c io/inputtextfile.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c ra/Lvec.cpp					$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c ra/Lmat.cpp					$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c functions/quadd.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c functionals/fitter.cpp			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c functionals/VEGAS.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c randoms/ranunl.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c lies/stats.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c lies/meansig.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lbins.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lhistbin.cpp			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lprofbin.cpp			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lhist1.cpp 			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lhist2.cpp 			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lprof1.cpp 			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c histograms/Lprof2.cpp 			$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/LppGNUpipe.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/plotbase.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/plotfit.cpp				$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/Lfit1.cpp					$(DEBFLAGS) $(HEADFLAGS)
	$(CXX) -c GUI/Lzcol.cpp					$(DEBFLAGS) $(HEADFLAGS)
