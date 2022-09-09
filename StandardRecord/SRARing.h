////////////////////////////////////////////////////////////////////////
// \file    SRARing.h
////////////////////////////////////////////////////////////////////////
#ifndef SRARING_H
#define SRARING_H

#include "StandardRecord/SRVector3D.h"

namespace caf
{
  /// An SRAring is a simple descriptor for an ARing.
  class SRAring
  {
    SRARing();
    virtual ~SRARing();

    unsigned short nhit;    ///< number of hits
    SRVector3D     center;  ///< center position of aring [cm?]
    float          radius;  ///< aring radius

    virtual void setDefault();
  };

} // end namespace

#endif // SRARING_H
