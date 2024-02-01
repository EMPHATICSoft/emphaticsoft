////////////////////////////////////////////////////////////////////////
// \file    HeaderFiller.h
// \note    Class to fill CAF Header info.  Since the header gets 
//          information from the subrun object and not the event, this
//          class does not inherit from the Filler base class. 
////////////////////////////////////////////////////////////////////////
#ifndef HEADERFILLER_H
#define HEADERFILLER_H

#include "art/Framework/Principal/SubRun.h"
#include "StandardRecord/StandardRecord.h"

namespace caf
{
  /// Class to fill header information
  class HeaderFiller 
  {
  public:

    void Fill(art::SubRun&, caf::SRHeader&);

    std::string fDQLabel;

  };

} // end namespace

#endif // HEADERFILLER_H
