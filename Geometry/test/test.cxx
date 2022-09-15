////////////////////////////////////////////////////////////////////////
/// \brief   unit test for the geometry package
/// \author  wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"
#include "RunHistory/RunHistory.h"

using namespace emph::geo;

int main(int argc, char* argv[]){

	if(argc !=2){
		std::cerr << "Usage: UnitTest [Run-number]" <<std::endl;
		return -1;
	}
	
	int runNum = std::stoi(argv[1]);
	std::cout << "This is a unit test for Run" << runNum << "." << std::endl << std::endl;

	runhist::RunHistory *fRunHistory = new runhist::RunHistory(runNum);
	Geometry *emgeo = new Geometry(fRunHistory->GeoFile());

	std::cout << "The magnet position is " << emgeo->MagnetUSZPos() << " - " << emgeo->MagnetDSZPos() << " cm." << std::endl;

	for ( int i = Trigger ; i < NDetectors ; i++ ){
		if ( !emgeo->DetectorLoad(i) )continue;
		std::cout << "The " << DetInfo::Name(DetectorType(i)) << " position is " << emgeo->DetectorUSZPos(i) << " - " << emgeo->DetectorDSZPos(i) << " cm." << std::endl;
	}

	int nstation = emgeo->NSSDStations();
	for ( int i = 0; i < nstation; i++){
		SSDStation st = emgeo->GetSSDStation(i);
		int nsensor = st.NSSDs();
		for ( int j = 0; j < nsensor; j++){
			Detector sensor = st.GetSSD(j);
			std::cout << "The " << j <<"-th SSD sensor in the " << i <<"-th SSD station is located at " << sensor.Pos()[2]+st.Pos()[2] << " cm." << std::endl;
			std::cout << "The cosine of rotation angle is " << sensor.Rot() << std::endl;
		}
	}

	std::cout << std::endl << "This unit test is finished." << std::endl << std::endl;

	return 1;

}
