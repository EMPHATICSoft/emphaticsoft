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

#include "RecoBase/SSDHit.h"

namespace rb {
  
  class SSDCluster {
  public:
    SSDCluster();
#ifndef __GCCXML__
    explicit SSDCluster(const art::PtrVector<rb::SSDHit>& hits, 
			std::vector<double> weights = std::vector<double>(), 
			int id=0);
#endif // __GCCXML__
    virtual ~SSDCluster() {}; //Destructor
    
  public:
    void Add(const art::Ptr<rb::SSDHit>& hit, double weight=1);
    void Add(const art::PtrVector<rb::SSDHit>& hits,
	     const std::vector<double>& weights = std::vector<double>());
    
    void SetID(int id) { fID = id; }
    int ID() const {return fID; }

    unsigned int NHits() const { return fHitVec.size(); }

    art::Ptr<rb::SSDHit> Hit(unsigned int idx) const;
    const art::PtrVector<rb::SSDHit> Hits() const { return fHitVec; }

    void   SetWeights(std::vector<double>& w8s) { fWeights = w8s; }
    void   SetWeight(unsigned int idx, double w);
    double Weight(unsigned int idx);

    double WeightedStrip() const;
    double WeightedWidth() const;

    friend std::ostream& operator << (std::ostream& o, const SSDCluster& h);
    
#ifndef __GCCXML__
    
  protected:
    art::PtrVector<rb::SSDHit> fHitVec;
    std::vector<double>        fWeights;
    int                        fID;
#endif // __GCCXML__
    
  };
  
}

#endif // SSDHIT_H
