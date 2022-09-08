////////////////////////////////////////////////////////////////////////
// \file    StandardRecord.h
// \brief   The StandardRecord is the primary top-level object in the
//          Common Analysis File trees.
// \version $Id: StandardRecord.h,v 1.7 2012-12-06 20:18:33 rocco Exp $
// \author  $Author: rocco $
// \date    $Date: 2012-12-06 20:18:33 $
////////////////////////////////////////////////////////////////////////
#ifndef STANDARDRECORD_H
#define STANDARDRECORD_H

#include <vector>

#include "StandardRecord/SRHeader.h"

/// Common Analysis Files
namespace caf
{

  /// \brief   The StandardRecord is the primary top-level object in the
  ///          Common Analysis File trees.

    class StandardRecord
    {

    public:
      StandardRecord();
      ~StandardRecord();

      SRHeader hdr;  ///< Header branch: run, subrun, etc.
      
    }; // end class

} // end caf namespace

#endif // STANDARDRECORD_H
