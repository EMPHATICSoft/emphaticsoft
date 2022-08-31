////////////////////////////////////////////////////////////////////////
/// \brief   Definition of raw beamline data (extracted from IF beam DB)
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef RAWSPILLDATA_H
#define RAWSPILLDATA_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace emph { 
  namespace rawdata {
  
    class SpillData {
    public:
      SpillData(); // Default constructor
      ~SpillData() {}; //Destructor
    
    private:
      
      float fMomentum;
      short fPolarity; // 1 or -1
      
      float fIntensity; // incident rate 

    public:

      float Momentum()       const { return fMomentum; }
      short Polarity()       const { return fPolarity;  }
      float Intensity()      const { return fIntensity; }

    
      // Setters
      void  SetMomentum (float mom) { fMomentum = mom; }
      void  SetPolarity (short pol) { fPolarity = pol; }
      void  SetIntensity(float ppp) { fIntensity = ppp; }

      friend std::ostream& operator << (std::ostream& o, const SpillData& r);
  };
  
  }
}

#endif // RAWSPILLDATA_H
