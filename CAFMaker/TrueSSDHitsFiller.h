////////////////////////////////////////////////////////////////////////
// \file    TrueSSDHitsFiller.h
// \note    Class to fill CAF SSDHit info
////////////////////////////////////////////////////////////////////////
#ifndef TrueSSDHitsFILLER_H
#define TrueSSDHitsFILLER_H

#include "CAFMaker/FillerBase.h"

namespace caf
{
  /// Class to fill header information
  class TrueSSDHitsFiller : public FillerBase
  {
  public:

    void Fill(art::Event&, caf::StandardRecord&);

    std::string fLabel;

  };

} // end namespace

#endif // TrueSSDHitsFILLER_H
