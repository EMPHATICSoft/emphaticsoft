///
/// \file    PlotDrawingOptions.cxx
/// \brief   Option for drawing plots on the "PlotView"
/// \author  messier@indiana.edu
///
#include "EventDisplayServices/PlotDrawingOptions.h"


namespace evd
{
  PlotDrawingOptions::PlotDrawingOptions(fhicl::ParameterSet const& pset, 
					 art::ActivityRegistry& ) // reg)
    : evdb::Reconfigurable{pset}
  {
    this->reconfigure(pset);
  }
  
  //......................................................................
  PlotDrawingOptions::~PlotDrawingOptions()
  {
  }

  //......................................................................
  void PlotDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
  {
    fPadDescription = pset.get< std::vector<std::string> >("PadDescription");
  }

  DEFINE_ART_SERVICE(PlotDrawingOptions)
} // end namespace evd
////////////////////////////////////////////////////////////////////////
