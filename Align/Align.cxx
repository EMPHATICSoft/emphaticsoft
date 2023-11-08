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
    std::cout << "Created new Align object!" << std::endl;
  }

  //----------------------------------------------------------------------    
  
  int Align::Hash(int station, int plane, int sensor) const 
  {
    return station*100+plane*10+sensor;
  }

  //----------------------------------------------------------------------    
  
  void Align::Unhash(int id, int& station, int& plane, int& sensor) const 
  {
    station = id/100;
    plane = (id-station*100)/10;
    sensor = id%10;
  }

  //----------------------------------------------------------------------    
  
  TGeoCombiTrans* Align::SSDMatrix(int station, int plane, int sensor)
  {
    int id = this->Hash(station,plane,sensor);

    if (fSSDMatrix.find(id) == fSSDMatrix.end()) return NULL;
    return fSSDMatrix[id];

  }

  //----------------------------------------------------------------------

  void Align::SetSSDTranslation(int station, int plane, int sensor,
				TGeoTranslation* t)
  {

    int id = this->Hash(station,plane,sensor);

    if (fSSDMatrix.find(id) != fSSDMatrix.end())
      delete fSSDMatrix[id];

    fSSDMatrix[id] = new TGeoCombiTrans();
    fSSDMatrix[id]->SetTranslation(*t);

  }

  //----------------------------------------------------------------------

  void Align::AddSSDTranslation(int station, int plane, int sensor,
				TGeoTranslation* t)
  {
    int id = this->Hash(station,plane,sensor);

    if (fSSDMatrix.find(id) != fSSDMatrix.end()) {
      fSSDMatrix[id] = new TGeoCombiTrans();
      fSSDMatrix[id]->SetTranslation(*t);      
    }
    else {
      (*fSSDMatrix[id]) *= (*t);
    }

  }

  //----------------------------------------------------------------------

  void Align::SetSSDRotation(int station, int plane, int sensor,
			     TGeoRotation* t)
  {
    int id = this->Hash(station,plane,sensor);

    if (fSSDMatrix.find(id) != fSSDMatrix.end()) 
      delete fSSDMatrix[id];
    
    fSSDMatrix[id] = new TGeoCombiTrans();
    fSSDMatrix[id]->SetRotation(*t);      

  }
  
  //----------------------------------------------------------------------

  void Align::AddSSDRotation(int station, int plane, int sensor,
			     TGeoRotation* t)
  {
    int id = this->Hash(station,plane,sensor);

    if (fSSDMatrix.find(id) != fSSDMatrix.end()) {
      fSSDMatrix[id] = new TGeoCombiTrans();
      fSSDMatrix[id]->SetRotation(*t);      
    }
    else {
      (*fSSDMatrix[id]) *= (*t);
    }
    
  }

  //----------------------------------------------------------------------
  
  bool Align::WriteSSDConsts(std::string fname)
  {
    if (fSSDMatrix.empty()) return false;
    
    int station, plane, sensor;

    std::ofstream ofile;
    ofile.open(fname.c_str());
    if (!ofile.is_open())      std::abort();

    for (auto itr=fSSDMatrix.begin(); itr!=fSSDMatrix.end(); ++itr) {
      int id = itr->first;
      this->Unhash(id,station,plane,sensor);

      auto t = fSSDMatrix[id]->GetTranslation();
      TGeoRotation rot;
      rot.SetMatrix(fSSDMatrix[id]->GetRotationMatrix());
      double phi, theta, psi;
      rot.GetAngles(phi, theta, psi);
      ofile << station << " " << plane << " " << sensor << " " 
	    << t[0] << " " << t[1] << " " << t[2] << " " 
	    << phi << " " << theta << " " << psi << "\n";      
    }
    
    ofile.close();
    return true;
  }
  
  //----------------------------------------------------------------------    

  bool Align::LoadSSDConsts(std::string fname)
  {
    /*
  //----------------------------------------------------------------------

  void Align::CreateSSDMatrices(const emph::geo::Geometry* geo)
  {
    std::cout << "Creating SSD alignment matrices" << std::endl;
    fSSDMatrix.clear();

    int nstations = geo->NSSDStations();    
    std::cout << "nstations = " << nstations << std::endl;
    for (int i=0; i<nstations; ++i) {
      int nplanes = geo->GetSSDStation(i)->NPlanes();
      std::vector<std::vector<TGeoCombiTrans*> > mp;
      for (int j=0; j<nplanes; ++j) {
	int nsensors = geo->GetSSDStation(i)->GetPlane(j)->NSSDs();
	ms.resize(nsensors,0);
	for (int k=0; k<nsensors; ++k) {
	  ms[k] = new TGeoCombiTrans();
	}
	mp.push_back(ms);
      }
    }

    std::cout << "Done creating SSD alignment matrices." << std::endl;
  }


     */
    
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
      
      int id = this->Hash(station,plane,sensor);

      if (fSSDMatrix.find(id) != fSSDMatrix.end()) delete fSSDMatrix[id];

      fSSDMatrix[id] = new TGeoCombiTrans();
      fSSDMatrix[id]->SetTranslation(dx,dy,dz);
      TGeoRotation rot;
      rot.SetAngles(phi,theta,psi);
      fSSDMatrix[id]->SetRotation(rot);
      
    }
    
    ifile.close();
    return true;
  }

  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
