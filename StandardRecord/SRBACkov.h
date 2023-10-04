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

    float charge;    ///< charge recorded for BACkov PMT
    float time;     ///< time of hit recorded for BACkov PMT

    virtual void setDefault();
  };

} // end namespace

#endif // SRBACKOV_H
