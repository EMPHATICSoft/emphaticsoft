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
#include "StandardRecord/SRARingBranch.h"
#include "StandardRecord/SRBACkovBranch.h"
#include "StandardRecord/SREventQual.h"
#include "StandardRecord/SRGasCkovBranch.h"
#include "StandardRecord/SRLineSegmentBranch.h"
#include "StandardRecord/SRSpacePointBranch.h"
#include "StandardRecord/SRSSDClustBranch.h"
#include "StandardRecord/SRSSDHitsBranch.h"
//#include "StandardRecord/SRTrackBranch.h"
#include "StandardRecord/SRTrackSegmentBranch.h"
#include "StandardRecord/SRVertexBranch.h"
#include "StandardRecord/SRTruth.h"

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

      SRHeader             hdr;     ///< Header branch: run, subrun, etc.
      SRARingBranch        ring;    ///< ARICH ring branch: nhit, radius, etc.
      SRBACkovBranch       backov;  ///< BACkov PMT charge and PID
      SREventQual          evtqual; ///< Event quality metrics
      SRGasCkovBranch      gasckov; ///< GasCkov PMT charge and PID
      SRLineSegmentBranch  lineseg; ///< Line Segment branch: x0, x1
      SRSpacePointBranch   spcpts;  ///< Reco space points branch.
      SRSSDClustBranch     cluster; ///< SSD Cluster Branch: avgadc, avgstrip, etc
      SRSSDHitsBranch      hits;    ///< SSD Hits branch:
      //      SRTrackBranch        trks;    ///< Reco space points branch.
      SRTrackSegmentBranch sgmnts;  ///< Reco track segments branch
      SRVertexBranch       vtxs;    ///< Reco vertices
      SRTruth	           truth;   ///< Simulation truth momenta, position, ID, etc.

    }; // end class

} // end caf namespace

#endif // STANDARDRECORD_H
