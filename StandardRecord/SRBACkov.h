////////////////////////////////////////////////////////////////////////
// \file    SRBACkov.h
////////////////////////////////////////////////////////////////////////
#ifndef SRBACKOV_H
#define SRBACKOV_H

namespace caf
{
  /// An SRBACkov is a simple descriptor for the BACkov.
  class SRBACkov
  {
  public:
    SRBACkov();
    virtual ~SRBACkov();

    float charge[6];    ///< charge recorded for each BACkov PMT
    bool  PID[5];       ///< probability (0 or 1) of being {e,mu,pi,K,p}

    virtual void setDefault();
  };

} // end namespace

#endif // SRBACKOV_H
