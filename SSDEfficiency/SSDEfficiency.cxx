////////////////////////////////////////////////////////////////////////
/// \brief SSDEfficiency class
/// \author vo17@illinois.edu
/// \date July 2026
////////////////////////////////////////////////////////////////////////

#include "SSDEfficiency/SSDEfficiency.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

namespace emph {
  SSDEfficiency::SSDEfficiency() {
    fSSDEfficiencies.clear();
    fSSDEfficiencies.resize(8, std::vector<std::vector<double>>(3, std::vector<double>(2, 1)));
    fIsDisabled = false;
  }

  //----------------------------------------------------------------------

  bool SSDEfficiency::LoadSSDEffConsts(std::string fname) {
    std::ifstream ifile;
    ifile.open(fname.c_str());
    if (!ifile.is_open())  {
      std::cerr << "Cannot open SSD Efficiency file: " << fname << std::endl;
      std::cerr << "Aborting!" << std::endl;
      std::abort();
    } 

    double eff;
    int station, plane, sensor;
    std::string line;

    while (getline(ifile, line)) {
      if (line[0] == '#') { continue; }

      std::stringstream lineStr(line);
      lineStr >> station >> plane >> sensor >> eff;
 
      if (station > 7 || plane > 2 || sensor > 1) {
        std::cout << "Invalid SSD Station/Plane/Sensor: " 
          << station << "/" << plane << "/" << sensor << std::endl;
        std::cout << "Check your SSD efficiency constants file" << std::endl;
      }

      if (fIsDisabled) {
        fSSDEfficiencies[station][plane][sensor] = 1;
      } else {
        fSSDEfficiencies[station][plane][sensor] = eff;
      }
    }

    ifile.close();
    return true;
  }

  //----------------------------------------------------------------------

  bool SSDEfficiency::WriteSSDEffConsts(std::string fname) {
    if (fSSDEfficiencies.empty()) { return false; }

    std::ofstream ofile;
    ofile.open(fname.c_str());
    if (!ofile.is_open()) { std::abort(); }

    for (int station = 0; station < 8; station++) {
      for (int plane = 0; plane < 3; plane++) {
        for (int sensor = 0; sensor < 2; sensor++) {
          double eff = fSSDEfficiencies[station][plane][sensor];
          ofile << station << " " << plane << " " << sensor << " " << eff << "\n";
        }
      } 
    } 

    ofile.close();
    return true;
  }

  //----------------------------------------------------------------------

  double SSDEfficiency::GetSSDEfficiency(int station, int plane, int sensor) {
    if (station > 7 || plane > 2 || sensor > 1) {
      std::cout << "GetSSDEfficiency: Out of range station/plane/sensor returning default efficiency of 1" << std::endl;
      return 1;
    }
    return fSSDEfficiencies[station][plane][sensor];
  }

  void SSDEfficiency::SetSSDEfficiency(int station, int plane, int sensor, double eff) {
    if (station > 7 || plane > 2 || sensor > 1) {
      std::cout << "SetSSDEfficiency: Out of range station/plane/sensor" << std::endl;
      return;
    }
    fSSDEfficiencies[station][plane][sensor] = eff;
  }
} // end namespace emph
