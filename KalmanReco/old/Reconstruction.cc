#include "Reconstruction.h"
#include "Utility.h"
#include "TMath.h"
#include <iomanip> 
#include <algorithm>
#include <sstream>

#include "TFile.h"
//#include "omp.h" 

using namespace utl; 
using namespace std;

Reconstruction::Reconstruction (string mapFile, int instance){
	
	fRandom = new TRandom3;
	fRandom->SetSeed(0);

	fExtrap = new TrackExtrapolation(true, 0.05, mapFile);
	
	std::ostringstream name;
	name << "hdp" << instance;
	hdp =  new TH1D(name.str().c_str(), "", 100, -0.2, 0.2);
	name.str(""); name.clear();
	name << "hdtx" << instance;
	hdtx = new TH1D(name.str().c_str(), "", 100, -0.004, 0.004);
	name.str(""); name.clear();
	name << "hdty" << instance;
	hdty = new TH1D(name.str().c_str(), "", 100, -0.004, 0.004);
	name.str(""); name.clear();
	name << "hdx" << instance;
	hdx =  new TH1D(name.str().c_str(), "", 100, -0.030, 0.015);
	name.str(""); name.clear();
	name << "hdy" << instance;
	hdy =  new TH1D(name.str().c_str(), "", 100, -0.015, 0.015);
	
	name.str(""); name.clear();
	name << "hdpRel" << instance;
	hdpRel =  new TH1D(name.str().c_str(), "", 100, -4, 4);
	name.str(""); name.clear();
	name << "hdtxRel" << instance;
	hdtxRel = new TH1D(name.str().c_str(), "", 100, -4, 4);
	name.str(""); name.clear();
	name << "hdtyRel" << instance;
	hdtyRel = new TH1D(name.str().c_str(), "", 100, -4, 4);
	name.str(""); name.clear();
	name << "hdxRel" << instance;
	hdxRel =  new TH1D(name.str().c_str(), "", 100, -4, 4);
	name.str(""); name.clear();
	name << "hdyRel" << instance;
	hdyRel =  new TH1D(name.str().c_str(), "", 100, -4, 4);

}







