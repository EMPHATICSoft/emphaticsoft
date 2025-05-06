////////////////////////////////////////////////////////////////////////
/// \brief   Definition of ARICH cluster [of hits].  Note, this class
///          is intended to only group ARICH hits in time, wrt refence time.
/// \author  mdallolio
/// \date 	
////////////////////////////////////////////////////////////////////////
#ifndef ARICHCLUSTER_H
#define ARICHCLUSTER_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "canvas/Persistency/Common/PtrVector.h"
#include "Geometry/DetectorDefs.h"
#include "Geometry/Geometry.h"
#include "RawData/TRB3RawDigit.h"
#include "ChannelMap/service/ChannelMapService.h"


namespace rb {
  
  class ARICHCluster {
  public:
    ARICHCluster();
   explicit ARICHCluster(const std::vector<std::pair<int,int>> echans);
   virtual ~ARICHCluster() {}; //Destructor
    
  public:

    void Add(const std::pair<int,int> echan);
    void Add(const float time);    


    void SetID(int id) { fID = id; }
    int ID() const {return fID; }

    std::pair<int,int> Digit(unsigned int idx);
    float  Time(unsigned int idx);
    unsigned int NDigits() const { return fHits.size(); }
    bool empty() const {return this->NDigits() == 0;}
   
    std::vector<std::pair<int,int>> Digits() const {return fHits;}
    std::vector<float> Times() const {return fTimes;}

    
  private:

    std::vector<std::pair<int,int>> fHits;   
    std::vector<float> fTimes;
    int fID;
  };
  
}

#endif
