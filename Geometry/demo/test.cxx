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
	for ( int i = 0; i < 3; i++){
	//for ( int i = 0; i < nstation; i++){
		SSDStation st = emgeo->GetSSDStation(i);
		int nsensor = st.NSSDs();
		for ( int j = 0; j < nsensor; j++){
			Detector sensor = st.GetSSD(j);
			std::cout << std::endl << std::endl << "The " << j <<"-th SSD sensor in the " << i <<"-th SSD station is located at " << sensor.Pos()[0] << " " << sensor.Pos()[1] << " " << sensor.Pos()[2]+st.Pos()[2] << " "<< unit << "." << std::endl;
			std::cout << "The rotation angle is " << sensor.Rot() << std::endl;
			std::cout << "The SSD is flipped? " << sensor.IsFlip() << std::endl;
			int nchan = sensor.NStrips();
			std::cout << "The SSD has " << nchan << " strips." << std::endl;
			for ( int k = 0; k< nchan; k++){
				emph::geo::Strip chan = sensor.GetStrip(k);

				double localcor=chan.Pos()[1];
				double rot=sensor.Rot();
				if(sensor.IsFlip()){
					localcor=-localcor;
					rot=TMath::Pi()*2-rot;
				}

				double globalcor[3];
				globalcor[0]=sensor.Pos()[0]+localcor*sin(rot);
				globalcor[1]=sensor.Pos()[1]+localcor*cos(rot);
				globalcor[2]=st.Pos()[2];
				std::cout << "The " << k <<"-th SSD channel " << chan.Name() << " in the "<< j <<"-th SSD sensor in the " << i <<"-th SSD station is located at " << globalcor[0] << " " << globalcor[1] << " " << globalcor[2] << " "<< unit << "." << std::endl;
			}
		}
	}

	std::cout << std::endl << "This unit test is finished." << std::endl << std::endl;

	return 1;

}
