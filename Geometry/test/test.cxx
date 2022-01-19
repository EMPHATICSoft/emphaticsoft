#include <iostream>
#include "Geometry/Geometry.h"
int main(){
	std::cout<<"This is a unit test."<<std::endl;
	emph::geo::Geometry *emgeo=new emph::geo::Geometry("/emph/app/users/linyan/emphaticsoft/Geometry/gdml/phase1-test.gdml");
	std::cout<<emgeo->MagnetDSZPos()<<std::endl;
	return 1;
}
