#include <fstream>
#include <sstream>
#include <iomanip>

void MakeMisalignConsts(int nFiles = 100)
{

  TRandom myRand(542372104);

  double xoff = 3; // mm for x-position offset width
  double yoff = 3; // mm for y-position offset width
  double zoff = 0.; // mm for z-position offset width
  double phioff = 2.; // degrees for rotation about the z-axis
  int nStations = 8;
  int nPlanes = 3;
  int nSensors = 2;
  std::streamsize originalPrecision = std::cout.precision();

  for (int iFile = 0; iFile<=nFiles; ++iFile) {

    std::stringstream fname;
    fname << "MisalignConst_" << iFile << ".txt";
    std::ofstream fout;
    fout.open(fname.str().c_str());
    fout.precision(4);
    for (int iStation = 0; iStation<nStations; ++iStation) {
      for (int iPlane = 0; iPlane<nPlanes; ++iPlane) {
	for (int iSensor = 0; iSensor<nSensors; ++iSensor) {
	  if (iFile>0) 
	    fout << iStation << "  " << iPlane << "  " << iSensor << "  " 
		 << std::setprecision(4)
		 << myRand.Gaus(0,xoff) << "  "
		 << myRand.Gaus(0,yoff) << "  "
		 << myRand.Gaus(0,zoff) << "  "
		 << myRand.Gaus(0,phioff) << "  "
		 << myRand.Gaus(0,phioff) << "  "
		 << myRand.Gaus(0,phioff) << std::endl;
	  else
	    fout << iStation << "  " << iPlane << "  " << iSensor << "  " 
		 << std::setprecision(4)
		 << 0 << "  "
		 << 0 << "  "
		 << 0 << "  "
		 << 0 << "  "
		 << 0 << "  "
		 << 0 << std::endl;
	}
      }
    }
  }
}
