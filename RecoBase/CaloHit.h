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
    //const std::vector<uint16_t> _wfm; //waveform vector (108 bins)
    int _time;
    float _intchg;

  public:
    // Getters
    int Channel() const {return _channel; }
    //const std::vector<uint16_t> WaveForm() const {return _wfm; }
    int Time() const {return _time; }
    float IntCharge() const {return _intchg; }
   
    // Setters
    void SetChannel(int chan) { _channel = chan; }
    //void SetWaveForm(const std::vector<uint16_t> &wfm) { _wfm = wfm; }
    void SetTime(const std::vector<uint16_t> &wfm);
    //void SetTime(const std::vector<uint16_t> &wfm){ _time = std::min_element(wfm.begin(),wfm.end()) - wfm.begin(); }   
    void SetIntCharge(float intchg) { _intchg = intchg; }

    friend std::ostream& operator << (std::ostream& o, const CaloHit& h);
  };
  
}

#endif // CALOHIT_H
