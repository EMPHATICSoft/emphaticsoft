#include <iostream>

#include "Geometry/Geometry.h"
#include "Geometry/DetectorDefs.h"

int main(){

	std::cout << "This is a unit test." << std::endl << std::endl;

	emph::geo::Geometry *emgeo = new emph::geo::Geometry("/emph/app/users/linyan/emphaticsoft/Geometry/gdml/phase1-test.gdml");

	std::cout << "The magnet position is " << emgeo->MagnetUSZPos() << " - " << emgeo->MagnetDSZPos() << " cm." << std::endl;

	for ( int i = emph::geo::Trigger ; i < emph::geo::NDetectors ; i++ ){
		if ( !emgeo->DetectorLoad(i) )continue;
		std::cout << "The " << emph::geo::DetectorName[emph::geo::DetectorType(i)] << " position is " << emgeo->DetectorUSZPos(i) << " - " << emgeo->DetectorDSZPos(i) << " cm." << std::endl;
	}
	
	std::cout << std::endl << "This unit test is finished." << std::endl << std::endl;

	return 1;

}
