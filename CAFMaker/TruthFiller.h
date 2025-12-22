////////////////////////////////////////////////////////////////////////
// \file    SRTruthFiller.h
// \note    Class to fill CAF MC particle info
////////////////////////////////////////////////////////////////////////
#ifndef SRTRUTHFILLER_H
#define SRTRUTHFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class TruthFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);
    bool GetG4Hits;
    std::string fLabel;

  };

} // end namespace

#endif // SRTRUTHFILLER_H
