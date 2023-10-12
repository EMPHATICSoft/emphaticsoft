////////////////////////////////////////////////////////////////////////
/// \brief   unit test for the geometry package
/// \author  wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////


#include <iostream>

#include "TGeoManager.h"
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
  std::cout << "This is a unit test for Run " << runNum << "." << std::endl << std::endl;

  runhist::RunHistory *fRunHistory = new runhist::RunHistory(runNum);
  Geometry *emgeo = new Geometry(fRunHistory->GeoFile());

  std::string unit="";
  if(TGeoManager::GetDefaultUnits()==TGeoManager::kG4Units) unit = "mm";
  else unit = "cm";
	
  std::cout << "The magnet position is " << emgeo->MagnetUSZPos() << " - " << emgeo->MagnetDSZPos() << " "<< unit << "." << std::endl;

  for ( int i = Trigger ; i < NDetectors ; i++ ){
    if ( !emgeo->DetectorLoad(i) )continue;
    std::cout << "The " << DetInfo::Name(DetectorType(i)) << " position is " << emgeo->DetectorUSZPos(i) << " - " << emgeo->DetectorDSZPos(i) << " "<< unit << "." << std::endl;
  }

  int nstation = emgeo->NSSDStations();
  for ( int i = 0; i < nstation; i++){
    const SSDStation* st = emgeo->GetSSDStation(i);
    int nplanes = st->NPlanes();
    for (int k=0; k < nplanes; ++k) {
      const Plane* pln = st->GetPlane(k);
      int nsensor = pln->NSSDs();
      for ( int j = 0; j < nsensor; j++){
	const Detector* sensor = pln->SSD(j);
	std::cout << "The " << j <<"-th SSD sensor in the " << k << "-th plane of the " << i <<"-th SSD station is located at " << sensor->Pos()[0] << " " << sensor->Pos()[1] << " " << sensor->Pos()[2]+st->Pos()[2] << " "<< unit << "." << std::endl;
	std::cout << "The rotation angle is " << sensor->Rot() << std::endl;
	std::cout << "The SSD is flipped? " << sensor->IsFlip() << std::endl;
	int nchan = sensor->NStrips();
	std::cout << "The SSD has " << nchan << " strips." << std::endl;
      }
    }
  }
  
  std::cout << std::endl << "This unit test is finished." << std::endl << std::endl;

  return 1;

}
