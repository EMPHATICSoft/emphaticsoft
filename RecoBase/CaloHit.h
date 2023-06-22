////////////////////////////////////////////////////////////////////////
/// \brief   Definition of calorimeter hit
/// \author  robert chirco
/// \date    6/21/23
/// \song    rude by shygirl
////////////////////////////////////////////////////////////////////////
#ifndef CALOHIT_H
#define CALOHIT_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "RawData/RawDigit.h"
#include "Geometry/Geometry.h"

namespace rb {
  
  class CaloHit {
  public:
    CaloHit(); // Default constructor
    CaloHit(const std::vector<uint16_t> &wfm, int detchan);
    //CaloHit(const emph::rawdata::SSDRawDigit &ssdraw, const emph::geo::Detector &st); // Default constructor
    virtual ~CaloHit() {}; //Destructor
    
  private:

    int _channel; //block number (0-8, 4 is the center)

  public:
    // Getters
    double Channel() const {return _channel; }

    // Setters
    void SetChannel(int chan) { _channel = chan; }

    friend std::ostream& operator << (std::ostream& o, const CaloHit& h);
  };
  
}

#endif // CALOHIT_H
