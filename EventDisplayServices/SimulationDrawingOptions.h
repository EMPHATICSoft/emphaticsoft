////////////////////////////////////////////////////////////////////////
/// \file    SimulationDrawingOptions.h
/// \brief   Global drawing options that apply to all displays
/// \author  messier@indiana.edu
///
#ifndef EVD_SIMULATIONDRAWINGOPTIONS_H
#define EVD_SIMULATIONDRAWINGOPTIONS_H
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
  class SimulationDrawingOptions : public evdb::Reconfigurable
  {
  public:
    
    explicit SimulationDrawingOptions(fhicl::ParameterSet const& pset, 
				      art::ActivityRegistry& reg);
    ~SimulationDrawingOptions();
    
    void reconfigure(fhicl::ParameterSet const& pset) override;

    /// Which MC Truth objects to draw
    int fDraw;
    static const int kDRAW_VERTEX   = 1<<0;
    static const int kDRAW_VECTORS  = 1<<1;
    static const int kDRAW_TRAJECT  = 1<<2;
    static const int kDRAW_HITS     = 1<<3;
    static const int kDRAW_NEUTRALS = 1<<4;
    static const int kDRAW_GAMMAS   = 1<<5;

    /// Threshold to apply to FLS hits (MeV)
    float fFLSHitThresh;

    /// How to render FLS hits
    int fFLSHitStyle;
    static const int kFLSHIT_AS_DOT  = 0;
    static const int kFLSHIT_AS_LINE = 1;
    
    /// What text to draw?
    int fText;
    static const int kTEXT_SHORT = 1<<0;
    static const int kTEXT_LONG  = 1<<1;

    int fTextDepthLimit;
    int fTextIncludeVertex;
    int fTextIncludeDirections;

    /// Modules to load the data from
    std::vector<std::string> fMCTruthModules;      ///< MCTruth here
    std::vector<std::string> fFLSHitListModules;   ///< FLSHitLists here
  };
}
#endif // __CINT__
DECLARE_ART_SERVICE(evd::SimulationDrawingOptions, LEGACY)
#endif
////////////////////////////////////////////////////////////////////////
