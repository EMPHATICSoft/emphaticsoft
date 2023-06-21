////////////////////////////////////////////////////////////////////////
// \file    MCTruthFiller.h
// \note    Class to fill CAF MCTruth info
////////////////////////////////////////////////////////////////////////
#ifndef MCTRUTHFILLER_H
#define MCTRUTHFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class MCTruthFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);


  };

} // end namespace

#endif // MCTruthFILLER_H
