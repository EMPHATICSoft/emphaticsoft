////////////////////////////////////////////////////////////////////////
/// \file    GeometryDrawingOptions.h
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
/// \version $Id: GeometryDrawingOptions.h,v 1.8 2012-12-05 02:24:46 bckhouse Exp $
///
#ifndef EVD_GEOMETRYDRAWINGOPTIONS_H
#define EVD_GEOMETRYDRAWINGOPTIONS_H
#ifndef __CINT__

#include <vector>
#include <string>

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "EventDisplayBase/Reconfigurable.h"

namespace evd {
  /// \brief   Global drawing options that apply to all displays
  class GeometryDrawingOptions : public evdb::Reconfigurable
  {
  public:
    
    explicit GeometryDrawingOptions(fhicl::ParameterSet const& pset,
				    art::ActivityRegistry& reg);
    ~GeometryDrawingOptions();

    void reconfigure(fhicl::ParameterSet const& pset) override;
    
    // Geometry Drawing Options
    
    /// How to zoom the display
    int    fZoom;
    static const int kZOOM_NONE     = 0;
    static const int kZOOM_HITS     = 1;
    static const int kZOOM_TRUTH    = 2;
    static const int kZOOM_SLICE    = 3;
    static const int kZOOM_SLICEBOX = 4;
    static const int kZOOM_PARTIAL  = 5;
    // if zoom partial is chosen, you can set the z upper bound with fZRange
    float fZRange;
    /// Which outlines to draw
    int fOutline;
    static const int kOUTLINE_DETECTOR      = 1<<0;
    static const int kOUTLINE_FIDUCIAL_USER = 1<<1;
    static const int kOUTLINE_GRID          = 1<<2;
    static const int kOUTLINE_DCMS          = 1<<3;
    static const int kOUTLINE_CELLS         = 1<<4;
    static const int kOUTLINE_BADBOX        = 1<<5;
    // If fiducial-users is chosen, fFiducialBounds defines the bounds of 
    // the box. The first three numbers are low x,y,z and the next three
    // are upper x,y,z.
    std::vector<float> fFiducialBounds;
    std::vector<float> fSliceBoxSize;

    /// Reverse the sense of any of the coordinate axes?
    int fFlip;
    static const int kFLIP_X = 1<<0;
    static const int kFLIP_Y = 1<<1;
    static const int kFLIP_Z = 1<<2;
    
    /// Which labels to draw?
    int fLabel;
    static const int kLABEL_PLANECELL = 1<<0;
    static const int kLABEL_COMPASS   = 1<<1;

    /// Dim disabled channels? What colors?
    int fDimDisabled;
    int fEnabledColor;
    int fDisabledColor;

    int fHighlightPlane;
    int fHighlightCell;

    /// View angles for 3D
    double fTheta, fPhi, fPsi;
  };
}
#endif // __CINT__
DECLARE_ART_SERVICE(evd::GeometryDrawingOptions, LEGACY)
#endif
////////////////////////////////////////////////////////////////////////
