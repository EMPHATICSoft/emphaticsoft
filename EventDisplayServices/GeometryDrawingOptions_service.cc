////////////////////////////////////////////////////////////////////////
/// \file    GeometryDrawingOptions.cxx
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
/////////////////////////////////////////////////////////////////////////
#include "EventDisplayServices/GeometryDrawingOptions.h"


namespace evd
{
  // Unpack the configuration into the class data
  //---------------------------------------------------------------
  GeometryDrawingOptions::GeometryDrawingOptions(fhicl::ParameterSet const& pset, 
						 art::ActivityRegistry& ) //reg)
    : evdb::Reconfigurable{pset}
  {
    this->reconfigure(pset);
  }

  //---------------------------------------------------------------
  GeometryDrawingOptions::~GeometryDrawingOptions()
  {
  }

  //---------------------------------------------------------------
  void GeometryDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
  {
    fZoom          = pset.get<int>("Zoom.val");
    fZRange        = pset.get<int>("ZRange.val");
    fOutline       = pset.get<int>("Outline.val");
    fFiducialBounds= pset.get< std::vector<float> >("FiducialBounds.val");
    fSliceBoxSize  = pset.get< std::vector<float> >("SliceBoxSize.val");
    fFlip          = pset.get<int>("Flip.val");
    fLabel         = pset.get<int>("Label.val");
    fDimDisabled   = pset.get<int>("DimDisabled.val");
    fEnabledColor  = pset.get<int>("EnabledColor.val");
    fDisabledColor = pset.get<int>("DisabledColor.val");
    fHighlightPlane= pset.get<int>("HighlightPlane.val");
    fHighlightCell = pset.get<int>("HighlightCell.val");

    std::vector<double> v =
      pset.get<std::vector<double> >("ViewingAngles3D.val");

    fTheta = v[0];
    fPhi   = v[1];
    fPsi   = v[2];
  }

  DEFINE_ART_SERVICE(GeometryDrawingOptions)

} // end namespace evd
////////////////////////////////////////////////////////////////////////
