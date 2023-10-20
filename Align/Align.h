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

#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"

#include "TGeoMatrix.h"

namespace emph {

  class Align {
  public:
    Align(); // Default constructor
    virtual ~Align() {}; //Destructor

    //    void SetAbortIfFileNotFound(bool f) { fAbortIfFileNotFound = f;}

    bool LoadSSDConsts(std::string fname="");
    bool WriteSSDConsts(std::string fname="");

    TGeoCombiTrans* SSDMatrix(int station, int plane, int sensor) const;
    void SetSSDTranslation(int station, int plane, int sensor, TGeoTranslation*);
    void AddSSDTranslation(int station, int plane, int sensor, TGeoTranslation*);
    void SetSSDRotation(int station, int plane, int sensor, TGeoRotation*);
    void AddSSDRotation(int station, int plane, int sensor, TGeoRotation*);

    void Reset() { fSSDMatrix.clear(); }
    void CreateSSDMatrices(const emph::geo::Geometry*);

  private:

  //    bool fIsLoaded;
  //    bool fAbortIfFileNotFound;
    std::vector<std::vector<std::vector<TGeoCombiTrans*> > > fSSDMatrix;

  };
}

#endif // ALIGN_H

