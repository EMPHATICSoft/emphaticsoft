////////////////////////////////////////////////////////////////////////
/// \brief   Align class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "Align/Align.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

namespace emph {

  Align::Align() 
  {
    fSSDMatrix.clear();
  }
  
  //----------------------------------------------------------------------    

  TGeoCombiTrans* Align::SSDMatrix(int station, int plane, int sensor) const
  {
    if (station > 0 && station < int(fSSDMatrix.size()))
      if (plane > 0 && plane < int(fSSDMatrix[station].size()))
	if (sensor > 0 && sensor < int(fSSDMatrix[station][plane].size()))
	  return fSSDMatrix[station][plane][sensor];

    return NULL;
  }

  //----------------------------------------------------------------------

  void Align::CreateSSDMatrices(const emph::geo::Geometry* geo)
  {
    fSSDMatrix.clear();

    int nstations = geo->NSSDStations();    
    for (int i=0; i<nstations; ++i) {
      int nplanes = geo->GetSSDStation(i)->NPlanes();
      std::vector<std::vector<TGeoCombiTrans*> > mp;
      for (int j=0; j<nplanes; ++j) {
	int nsensors = geo->GetSSDStation(i)->GetPlane(j)->NSSDs();
	std::vector<TGeoCombiTrans*> ms;
	ms.resize(nsensors,0);
	for (int k=0; k<nsensors; ++k) {
	  ms[k] = new TGeoCombiTrans();
	}
	mp.push_back(ms);
      }
      fSSDMatrix.push_back(mp);
    }
    
  }

  //----------------------------------------------------------------------

  void Align::SetSSDTranslation(int station, int plane, int sensor,
				TGeoTranslation* t)
  {
    if (station > 0 && station < int(fSSDMatrix.size()))
      if (plane > 0 && plane < int(fSSDMatrix[station].size()))
	if (sensor > 0 && sensor < int(fSSDMatrix[station][plane].size())) {
	  if (!fSSDMatrix[station][plane][sensor])
	    fSSDMatrix[station][plane][sensor] = new TGeoCombiTrans();
	  fSSDMatrix[station][plane][sensor]->SetTranslation(*t);
	}
  }

  //----------------------------------------------------------------------

  void Align::AddSSDTranslation(int station, int plane, int sensor,
				TGeoTranslation* t)
  {
    if (station > 0 && station < int(fSSDMatrix.size()))
      if (plane > 0 && plane < int(fSSDMatrix[station].size()))
	if (sensor > 0 && sensor < int(fSSDMatrix[station][plane].size())) {
	  if (!fSSDMatrix[station][plane][sensor]) {
	    fSSDMatrix[station][plane][sensor] = new TGeoCombiTrans();
	    fSSDMatrix[station][plane][sensor]->SetTranslation(*t);
	  }
	  else {
	    (*fSSDMatrix[station][plane][sensor]) *= (*t);
	  }
	}
  }

  //----------------------------------------------------------------------

  void Align::SetSSDRotation(int station, int plane, int sensor,
			     TGeoRotation* t)
  {
    if (station > 0 && station < int(fSSDMatrix.size()))
      if (plane > 0 && plane < int(fSSDMatrix[station].size()))
	if (sensor > 0 && sensor < int(fSSDMatrix[station][plane].size())) {
	  if (!fSSDMatrix[station][plane][sensor])
	    fSSDMatrix[station][plane][sensor] = new TGeoCombiTrans();
	  fSSDMatrix[station][plane][sensor]->SetRotation(t);
	}
  }
  
  //----------------------------------------------------------------------

  void Align::AddSSDRotation(int station, int plane, int sensor,
			     TGeoRotation* t)
  {
    if (station > 0 && station < int(fSSDMatrix.size()))
      if (plane > 0 && plane < int(fSSDMatrix[station].size()))
	if (sensor > 0 && sensor < int(fSSDMatrix[station][plane].size())) {
	  if (!fSSDMatrix[station][plane][sensor]) {
	    fSSDMatrix[station][plane][sensor] = new TGeoCombiTrans();
	    fSSDMatrix[station][plane][sensor]->SetRotation(t);
	  }
	  else {
	    (*fSSDMatrix[station][plane][sensor]) *= (*t);
	  }
	}
  }

  //----------------------------------------------------------------------
  
  bool Align::WriteSSDConsts(std::string fname)
  {
    if (fSSDMatrix.empty()) return false;

    std::ofstream ofile;
    ofile.open(fname.c_str());
    if (!ofile.is_open())      std::abort();

    for (size_t i=0; i<fSSDMatrix.size(); ++i)
      for (size_t j=0; j<fSSDMatrix[i].size(); ++j)
	for (size_t k=0; k<fSSDMatrix[i][j].size(); ++k) {
	  auto t = fSSDMatrix[i][j][k]->GetTranslation();
	  TGeoRotation rot;
	  rot.SetMatrix(fSSDMatrix[i][j][k]->GetRotationMatrix());
	  double phi, theta, psi;
	  rot.GetAngles(phi, theta, psi);
	  ofile << i << " " << j << " " << k << " " 
		<< t[0] << " " << t[1] << " " << t[2] << " " 
		<< phi << " " << theta << " " << psi << "\n";
	}
    ofile.close();
    return true;
  }
  
  //----------------------------------------------------------------------    

  bool Align::LoadSSDConsts(std::string fname)
  {
    if (fSSDMatrix.empty()) {
      std::cerr << "Cannot load alignment constants into an uninitialized "
		<< "Align object.  Please be sure to call " 
		<< "Align::CreateSSDMatrices() first!" << std::endl;
      abort();
    }

    std::ifstream ifile;
    ifile.open(fname.c_str());
    if (!ifile.is_open())  {
      std::cerr << "Cannot open alignment file: " << fname << std::endl;
      std::cerr << "Aborting!" << std::endl;
      std::abort();
    }

    double dx, dy, dz;
    double phi, theta, psi;
    int station, plane, sensor;
    std::string line;

    while (getline(ifile,line)) {
      if (line[0] == '#') continue;
      std::stringstream lineStr(line);
      lineStr >> station >> plane >> sensor 
	      >> dx >> dy >> dz >> phi >> theta >> psi;
      
      if (station > 0 && station < int(fSSDMatrix.size()))
	if (plane > 0 && plane < int(fSSDMatrix[station].size()))
	  if (sensor > 0 && sensor < int(fSSDMatrix[station][plane].size())) {
	    if (!fSSDMatrix[station][plane][sensor]) 
	      fSSDMatrix[station][plane][sensor] = new TGeoCombiTrans();
	    fSSDMatrix[station][plane][sensor]->SetTranslation(dx,dy,dz);
	    TGeoRotation rot;
	    rot.SetAngles(phi,theta,psi);
	    fSSDMatrix[station][plane][sensor]->SetRotation(rot);
	  }
    }

    ifile.close();
    return true;
  }

  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
