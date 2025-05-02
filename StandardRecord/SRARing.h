////////////////////////////////////////////////////////////////////////
// \file    SRARing.h
////////////////////////////////////////////////////////////////////////
#ifndef SRARING_H
#define SRARING_H

#include "StandardRecord/SRVector3D.h"
#include "StandardRecord/SRArichID.h"
namespace caf
{
  /// An SRAring is a simple descriptor for an ARing.
  class SRARing: public SRArichID{
  public:
    SRARing();
    virtual  ~SRARing(); 
    float     center[2];  ///< center position of aring [cm?]
    float     radius;  ///< aring radius
	
    virtual void setDefault();
  };

} // end namespace

#endif // SRARING_H
