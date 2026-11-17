////////////////////////////////////////////////////////////////////////
/// \brief   Align class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "Align/Align.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <tuple>
#include <vector>

namespace emph {

  Align::Align() 
  {
    fSSDMatrix.clear();
    fIsDisabled = false;
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

    // Sensors without constants get an identity transform so callers never see NULL
    auto itr = fSSDMatrix.find(id);
    if (itr == fSSDMatrix.end())
      itr = fSSDMatrix.emplace(id, new TGeoCombiTrans()).first;
    return itr->second;

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

    if (fSSDMatrix.find(id) == fSSDMatrix.end()) {
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

    if (fSSDMatrix.find(id) == fSSDMatrix.end()) {
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

    std::vector<int> ids;
    ids.reserve(fSSDMatrix.size());
    for (const auto& entry : fSSDMatrix) ids.push_back(entry.first);
    std::sort(ids.begin(), ids.end(), [this](int lhs, int rhs) {
      int lhsStation, lhsPlane, lhsSensor;
      int rhsStation, rhsPlane, rhsSensor;
      this->Unhash(lhs, lhsStation, lhsPlane, lhsSensor);
      this->Unhash(rhs, rhsStation, rhsPlane, rhsSensor);
      return std::tie(lhsStation, lhsPlane, lhsSensor) <
             std::tie(rhsStation, rhsPlane, rhsSensor);
    });

    for (const int id : ids) {
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
      if (fIsDisabled)
	fSSDMatrix[id]->SetTranslation(0.,0.,0.);
      else
	fSSDMatrix[id]->SetTranslation(dx,dy,dz);
      TGeoRotation rot;
      if (fIsDisabled)
	rot.SetAngles(0.,0.,0.);
      else
	rot.SetAngles(phi,theta,psi);
      fSSDMatrix[id]->SetRotation(rot);

      //      std::cout<<"Align constants: "<<lineStr.str()<<std::endl;     
    }
    
    ifile.close();
    return true;
  }

  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
