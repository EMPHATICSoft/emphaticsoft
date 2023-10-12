////////////////////////////////////////////////////////////////////////
/// \brief   Definition of SSD cluster [of hits].  Note, this class
///          is intended to only group SSD hits on the same sensor, so 
///          they should all have the same "angle".
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RecoBase/SSDCluster.h"

#include <iomanip>
#include <iostream>
#include <cassert>
#include <cmath>

namespace rb {
  
  //----------------------------------------------------------------------
  
  SSDCluster::SSDCluster() :
    fID(-1)
  {
  }

  //----------------------------------------------------------------------
  
  SSDCluster::SSDCluster(const art::PtrVector<emph::rawdata::SSDRawDigit>& hits, 
			 int id) :
    fID(id),
    fStation(-1),
    fSensor(-1),
    fPlane(-1)
  {
    assert(hits.size() > 1);
    
    for (size_t i=0; i<hits.size(); ++i) {
      Add(hits[i]);
    }    
  }
  
  //------------------------------------------------------------
  void SSDCluster::Add(const art::Ptr<emph::rawdata::SSDRawDigit>& hit)
  {
    fDigitVec.push_back(hit);
  }

  //------------------------------------------------------------
  void SSDCluster::Add(const art::PtrVector<emph::rawdata::SSDRawDigit>& hits)
  {

    for (size_t i=0; i<hits.size(); ++i) {
      fDigitVec.push_back(hits[i]);
    }

  }

  //------------------------------------------------------------
  art::Ptr<emph::rawdata::SSDRawDigit> SSDCluster::Digit(unsigned int idx) const
  {
    assert(idx < fDigitVec.size());

    return fDigitVec[idx];

  }

  //------------------------------------------------------------
  // return the average strip position of the cluster
  //------------------------------------------------------------
  double SSDCluster::AvgStrip() const
  {
    if (fDigitVec.empty()) return -9999.;

    double sum=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      sum += fDigitVec[i]->Row();
    }
    return sum/NDigits();

  }

  //------------------------------------------------------------
  double SSDCluster::WgtAvgStrip() const
  {
    if (fDigitVec.empty()) return -9999.;

    double sum=0.;
    double totalADC=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      sum += fDigitVec[i]->Row()*adcMap[fDigitVec[i]->ADC()];
      totalADC+=adcMap[fDigitVec[i]->ADC()];
    }
    return sum/totalADC;

  }

  //------------------------------------------------------------
  double SSDCluster::WgtRmsStrip() const
  {
    if (fDigitVec.empty()) return -9999.;
    // set to 1/sqrt(12) if single strip cluster
    if (NDigits()==1) return 1/sqrt(12);

    double rmssum=0.;
    double totalADC=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      rmssum += pow(fDigitVec[i]->Row()-WgtAvgStrip(),2)*adcMap[fDigitVec[i]->ADC()];
      totalADC+=adcMap[fDigitVec[i]->ADC()];
    }
    return sqrt(rmssum/totalADC);

  }

  //------------------------------------------------------------
  int SSDCluster::MinStrip() const
  {
    assert(NDigits() > 0);
    int min = 999;
    for(unsigned int i=0; i<NDigits(); ++i)
      if(Digit(i)->Row() < min) min = Digit(i)->Row();

    return min;
  }

  //------------------------------------------------------------
  int SSDCluster::MaxStrip() const
  {
    assert(NDigits() > 0);
    int max = -1;
    for(unsigned int i=0; i<NDigits(); ++i)
      if(Digit(i)->Row() > max) max = Digit(i)->Row();

    return max;
  }

  //------------------------------------------------------------
  double SSDCluster::AvgADC() const
  {
    if (fDigitVec.empty()) return -9999.;

    double sum=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      sum += adcMap[fDigitVec[i]->ADC()];
    }
    return sum/NDigits();

  }

  //------------------------------------------------------------
  // return the average time position of the cluster
  //------------------------------------------------------------
  double SSDCluster::AvgTime() const
  {
    if (fDigitVec.empty()) return -9999.;

    double sum=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      sum += fDigitVec[i]->Time();
    }
    return sum/NDigits();

  }

  //------------------------------------------------------------
  int SSDCluster::MinTime() const
  {
    assert(NDigits() > 0);
    int min = 999;
    for(unsigned int i=0; i<NDigits(); ++i)
      if(Digit(i)->Time() < min) min = Digit(i)->Time();

    return min;
  }

  //------------------------------------------------------------
  int SSDCluster::MaxTime() const
  {
    assert(NDigits() > 0);
    int max = -1;
    for(unsigned int i=0; i<NDigits(); ++i)
      if(Digit(i)->Time() > max) max = Digit(i)->Time();

    return max;
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const rb::SSDCluster& h)
  {
    o << "SSD Station Cluster for Station "<< h.Station()<<", Sensor "<<h.Sensor()<<std::endl;
    o << h.NDigits()<< " raw digits in cluster"<<std::endl;
    o << "Min Time: "<< h.MinTime()<<std::endl;
    o << "Max Time: "<< h.MaxTime()<<std::endl;
    o << "Time range: "<< h.TimeRange()<<std::endl;
    o << "Weighted Avg. strip: " << h.WgtAvgStrip()<<std::endl;
    o << "Weighted RMS strip: " << h.WgtRmsStrip()<<std::endl;
    o << "Avg. strip: "<< h.AvgStrip()<<std::endl;
    o << "Min Strip: "<< h.MinStrip()<<std::endl;
    o << "Max Strip: "<< h.MaxStrip()<<std::endl;
    o << "Width: "<< h.Width()<<std::endl;
    o << "Avg. ADC: "<< h.AvgADC()<<std::endl;


    return o;
  }
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
