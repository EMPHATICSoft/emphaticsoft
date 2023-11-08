////////////////////////////////////////////////////////////////////////
/// \brief   Simple class to provide alignment matrices for detectors
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef ALIGN_H
#define ALIGN_H

#include <vector>
#include <map>
#include <stdint.h>
#include <iostream>

#include "TGeoMatrix.h"

namespace emph {

  class Align {
  public:
    Align(); // Default constructor
    virtual ~Align() {}; //Destructor

    //    void SetAbortIfFileNotFound(bool f) { fAbortIfFileNotFound = f;}

    bool LoadSSDConsts(std::string fname="");
    bool WriteSSDConsts(std::string fname="");

    TGeoCombiTrans* SSDMatrix(int station, int plane, int sensor);
    void SetSSDTranslation(int station, int plane, int sensor, TGeoTranslation*);
    void AddSSDTranslation(int station, int plane, int sensor, TGeoTranslation*);
    void SetSSDRotation(int station, int plane, int sensor, TGeoRotation*);
    void AddSSDRotation(int station, int plane, int sensor, TGeoRotation*);

    void Reset() { fSSDMatrix.clear(); }

  private:
    int Hash(int station, int plane, int sensor) const;
    void Unhash(int id, int& station, int& plane, int& sensor) const;
    std::unordered_map<int,TGeoCombiTrans*> fSSDMatrix;

  };
}

#endif // ALIGN_H

