////////////////////////////////////////////////////////////////////////
// \file    SREventQual.h
////////////////////////////////////////////////////////////////////////
#ifndef SREVENTQUAL_H
#define SREVENTQUAL_H

#include <stdint.h>

namespace caf
{
  /// Event Quality metrics
  class SREventQual
  {
  public:
    SREventQual();
    ~SREventQual();

    bool hasssdhits; ///< Does the event have any SSD Raw Digits?
    int  trigcoinc;  ///< Number of trigger PMTs above threshold.

    void setDefault();
  };

} // end namespace

#endif // SREVENTQUAL_H
