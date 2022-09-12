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
  
  SSDCluster::SSDCluster(const art::PtrVector<rb::SSDHit>& hits, 
			 std::vector<double> weights, int id) :
    fID(id)
  {
    assert(hits.size() > 1);
    
    for (size_t i=0; i<hits.size(); ++i) {
      if (i < weights.size())
	Add(hits[i],weights[i]);
      else
	Add(hits[i],1.);
    }
  }
  
  //------------------------------------------------------------
  void SSDCluster::Add(const art::Ptr<rb::SSDHit>& hit, double weight)
  {
    fHitVec.push_back(hit);
    fWeights.push_back(weight);
  }

  //------------------------------------------------------------
  void SSDCluster::Add(const art::PtrVector<rb::SSDHit>& hits,
		       const std::vector<double>& weights)
  {
    assert(hits.size() == weights.size());

    for (size_t i=0; i<hits.size(); ++i) {
      fHitVec.push_back(hits[i]);
      fWeights.push_back(weights[i]);
    }

  }

  //------------------------------------------------------------
  art::Ptr<rb::SSDHit> SSDCluster::Hit(unsigned int idx) const
  {
    assert(idx < fHitVec.size());

    return fHitVec[idx];

  }

  //------------------------------------------------------------
  void SSDCluster::SetWeight(unsigned int idx, double w)
  {
    if (idx >= fWeights.size()) return;
   
    fWeights[idx] = w;
  }

  //------------------------------------------------------------
  double SSDCluster::Weight(unsigned int idx)
  {
    if (idx >= fWeights.size()) return -9999.;
   
    return fWeights[idx];
  }

  //------------------------------------------------------------
  // return the weighted strip position of the cluster
  //------------------------------------------------------------
  double SSDCluster::WeightedStrip() const
  {
    if (fHitVec.empty()) return -9999.;

    double avg=0.;
    double sum=0.;
    for (size_t i=0; i<fHitVec.size(); ++i) {
      avg += fHitVec[i]->Strip()*fWeights[i];
      sum += fWeights[i];
    }
    return avg/sum;

  }

  //------------------------------------------------------------
  // return the weighted width, in units of strips, of the cluster
  //------------------------------------------------------------
  double SSDCluster::WeightedWidth() const
  {
    if (fHitVec.empty()) return -9999.;

    if (fHitVec.size() == 1.) return 1.;
    
    double avg2=0.;
    double avg=0.;
    double sum=0.;
    double sum2=0.;
    for (size_t i=0; i<fHitVec.size(); ++i) {
      avg  += fHitVec[i]->Strip();
      avg2 += fHitVec[i]->Strip()*fHitVec[i]->Strip();
      sum  += fWeights[i];
      sum2  += fWeights[i]*fWeights[i];
    }
    double sigma2 = avg2/double(fHitVec.size()) - avg*avg/double(fHitVec.size());
    return sqrt( sigma2*(1. - sum2/sum));
  }
  
  //------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, const rb::SSDCluster& h)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    o << " SSD Cluster weighted (position,width) = (" 
      << std::setw(5) << h.WeightedStrip()
      << ", " 
      << std::setw(5) << h.WeightedWidth();
    return o;
  }
  
} // end namespace rb
//////////////////////////////////////////////////////////////////////////////
