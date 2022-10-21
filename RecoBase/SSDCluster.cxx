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
    fID(id)
  {
    assert(hits.size() > 1);
    
    for (size_t i=0; i<hits.size(); ++i) {
      Add(hits[i]);
    }
    
    SetStation(hits[0]->FER());
    SetModule(hits[0]->Module());
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

    double avg=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      avg += fDigitVec[i]->Row();
    }
    return avg/NDigits();

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
  // return the average time position of the cluster
  //------------------------------------------------------------
  double SSDCluster::AvgTime() const
  {
    if (fDigitVec.empty()) return -9999.;

    double avg=0.;
    for (size_t i=0; i<NDigits(); ++i) {
      avg += fDigitVec[i]->Time();
    }
    return avg/NDigits();

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
    // o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    // o << " SSD Cluster (position,width) = (" 
    //   << std::setw(2) << h.AvgStrip()
    //   << ", " 
    //   << h.Width()
    //   << " " << h.NDigits() <<" digits."
    o << "SSD Station Cluster for FER "<< h.Station()<<", Module "<<h.Module()<<std::endl;
    o << h.NDigits()<< " raw digits in cluster"<<std::endl;
    o << "Min Time: "<< h.MinTime()<<std::endl;
    o << "Max Time: "<< h.MaxTime()<<std::endl;
    o << "Time range: "<< h.TimeRange()<<std::endl;
    o << "Avg. strip: "<< h.AvgStrip()<<std::endl;
    o << "Min Strip: "<< h.MinStrip()<<std::endl;
    o << "Max Strip: "<< h.MaxStrip()<<std::endl;
    o << "Width: "<< h.Width()<<std::endl;



    return o;
  }
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
