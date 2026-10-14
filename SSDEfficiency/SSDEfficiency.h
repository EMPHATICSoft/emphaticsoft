////////////////////////////////////////////////////////////////////////
/// \brief Simple class to provide efficiency matrix for SSDs
/// \author vo17@illinois.edu
/// \date July 2026
////////////////////////////////////////////////////////////////////////


#ifndef SSDEFFICIENCY_H
#define SSDEFFICIENCY_H

#include <vector>
#include <iostream>

namespace emph {

  class SSDEfficiency {
    public:
      SSDEfficiency(); // Default constructor
      virtual ~SSDEfficiency() {}; // Destructor

      bool LoadSSDEffConsts(std::string fname="");
      bool WriteSSDEffConsts(std::string fname="");

      double GetSSDEfficiency(int station, int plane, int sensor);
      void SetSSDEfficiency(int station, int plane, int sensor, double eff);

      void Reset() { fSSDEfficiencies.clear(); }
      void Disable() { fIsDisabled = true; }
      void Enable() { fIsDisabled = false; }
      bool IsDisabled() const { return fIsDisabled; }

    private:
      std::vector<std::vector<std::vector<double>>> fSSDEfficiencies;
      bool fIsDisabled;
  };
}

#endif // SSDEFFICIENCY_H
