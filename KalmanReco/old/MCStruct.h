#ifndef MCSTRUCT
#define MCSTRUCT

#include <vector>
#include <string>

struct Geant4Struct{
	std::vector<double> *pxStart;
	std::vector<double> *pyStart;
	std::vector<double> *pzStart;
	std::vector<double> *pxStop;
	std::vector<double> *pyStop;
	std::vector<double> *pzStop;
	std::vector<double> *xStart;
	std::vector<double> *yStart;
	std::vector<double> *zStart;
	std::vector<double> *xStop;
	std::vector<double> *yStop;
	std::vector<double> *zStop;
	std::vector<int> *pid;
	std::vector<int> *parentID;
	std::vector<int> *trackID;
	std::vector<int> *startProc;
	std::vector<int> *stopProc;
	std::vector<std::string> *startMat;
	std::vector<std::string> *stopMat;
    std::vector<std::vector<double> > *siHitsx;
    std::vector<std::vector<double> > *siHitsy;
    std::vector<std::vector<double> > *siHitsz;
    std::vector<std::vector<double> > *siHitspX;
    std::vector<std::vector<double> > *siHitspY;
    std::vector<std::vector<double> > *siHitspZ;
};

#endif
