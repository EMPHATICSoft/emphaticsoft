////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD cluster [of hits].  Note, this class
///          is intended to only group SSD hits on the same sensor, so 
///          they should all have the same "angle".
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDCLUSTER_H
#define SSDCLUSTER_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"

#include "Geometry/DetectorDefs.h"
#include "RawData/SSDRawDigit.h"

namespace rb {

  // index is the ADC value from the DAQ (0-7), result is the converted ADC value
  const unsigned int adcMap[] = {41, 58, 73, 88, 103, 118, 133, 140};
  
  class SSDCluster {
  public:
    SSDCluster();
    explicit SSDCluster(const art::PtrVector<emph::rawdata::SSDRawDigit>& hits,
			int id=0);
    virtual ~SSDCluster() {}; //Destructor
    
  public:
    void Add(const art::Ptr<emph::rawdata::SSDRawDigit>& hit);
    void Add(const art::PtrVector<emph::rawdata::SSDRawDigit>& hits);
    
    void SetID(int id) { fID = id; }
    int ID() const {return fID; }

    unsigned int NDigits() const { return fDigitVec.size(); }

    art::Ptr<emph::rawdata::SSDRawDigit> Digit(unsigned int idx) const;
    const art::PtrVector<emph::rawdata::SSDRawDigit> Digits() const { return fDigitVec; }

    void SetStation(int station) {fStation = station;}
    void SetSensor(int sensor)     {fSensor = sensor;}

    int    Station()  const { return fStation; }
    int    Sensor()    const { return fSensor; } 
    double WgtAvgStrip() const;
    double WgtRmsStrip() const;
    double AvgStrip() const;
    int    MinStrip() const;
    int    MaxStrip() const;
    int    Width() const { return (MaxStrip()-MinStrip()+1);}
    double AvgADC() const;
    double AvgTime() const;
    int    MinTime() const;
    int    MaxTime() const;
    int    TimeRange() const { return (MaxTime()-MinTime()); }

    friend std::ostream& operator << (std::ostream& o, const SSDCluster& h);
    
  private:

    art::PtrVector<emph::rawdata::SSDRawDigit> fDigitVec;
    int fID;
    int fStation;
    int fSensor;
    
  };
  
}

#endif // SSDHIT_H
