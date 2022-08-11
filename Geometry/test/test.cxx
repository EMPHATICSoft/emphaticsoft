////////////////////////////////////////////////////////////////////////
/// \brief   unit test for the geometry package
/// \author  wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"

using namespace emph::geo;

int main(){

	std::cout << "This is a unit test." << std::endl << std::endl;

	//	std::string package_path;
	//	package_path = getenv ("CETPKG_SOURCE");
	Geometry *emgeo = new Geometry("gdml/phase1a.gdml"); //package_path+"/Geometry/gdml/"+phase_name+".gdml");

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
		}
	}

	std::cout << std::endl << "This unit test is finished." << std::endl << std::endl;

	return 1;

}
