////////////////////////////////////////////////////////////////////////
/// \brief   Unpack class convert LAPPD raw data
/// \author  fnicolas@fnal.gov
////////////////////////////////////////////////////////////////////////

#ifndef LAPPDRAWDATA_UNPACK
#define LAPPDRAWDATA_UNPACK

#include <vector>
#include <iostream>
#include <utility>
#include <fstream>
#include <bitset>
#include <time.h>

#include "LAPPD/LAPPDObj/LAPPDRawDigit.h"


namespace lappd {

  class LAPPDBinaryUnpacker {
  public:
    LAPPDBinaryUnpacker() = default; // Default constructor

    // Function to set a new file handle
    void setFileHandle(std::string filename);
    int getNEvents() const { return fNEvents; } // Get number of events
    lappd::LAPPDRawDigit readNextEvent();

  private:
    std::ifstream fFileHandle; // File handle for reading binary data
    int fNEvents; // Number of events in the file
    int fCurrentEventIndex = 0; // Current event index
  };
  
} // end namespace lappd


#endif // LAPPDRAWDATA_UNPACK
