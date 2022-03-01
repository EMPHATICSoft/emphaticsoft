////////////////////////////////////////////////////////////////////////
/// \file    SimulationDrawingOptions.cxx
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
//////////////////////////////////////////////////////////////////////////
#include "EventDisplayServices/SimulationDrawingOptions.h"


namespace evd
{
  // Unpack the configuration into the class data
  //---------------------------------------------------------------
  SimulationDrawingOptions::SimulationDrawingOptions(fhicl::ParameterSet const& pset, 
						     art::ActivityRegistry& ) // reg)
    : evdb::Reconfigurable{pset}
  {
    this->reconfigure(pset);
  }

  //---------------------------------------------------------------
  SimulationDrawingOptions::~SimulationDrawingOptions()
  {
  }

  //---------------------------------------------------------------
  void SimulationDrawingOptions::reconfigure(fhicl::ParameterSet const& pset)
  {
    fDraw         = pset.get<int>  ("Draw.val");
    fFLSHitThresh = pset.get<float>("FLSHitThresh.val");
    fFLSHitStyle  = pset.get<int>  ("FLSHitStyle.val");
    fText         = pset.get<int>  ("Text.val");
    
    fTextDepthLimit = pset.get<int>("TextDepthLimit.val");

    fTextIncludeVertex = pset.get<int>("TextIncludeVertex.val");

    fTextIncludeDirections = pset.get<int>("TextIncludeDirections.val");
    
    fMCTruthModules    = pset.get< std::vector<std::string> >("MCTruthModules.val");
    fFLSHitListModules = pset.get< std::vector<std::string> >("FLSHitListModules.val");
  }

  DEFINE_ART_SERVICE(SimulationDrawingOptions)

} // end namespace evd
////////////////////////////////////////////////////////////////////////
