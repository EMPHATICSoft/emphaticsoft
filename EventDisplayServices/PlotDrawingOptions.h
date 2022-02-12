///
/// \file    PlotDrawingOptions.h
/// \brief   Options for which plots to draw on the PlotsView
/// \author  messier@indiana.edu
///
#ifndef EVD_PLOTDRAWINGOPTIONS_H
#define EVD_PLOTDRAWINGOPTIONS_H
#ifndef __CINT__

#include <vector>
#include <string>

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "EventDisplayBase/Reconfigurable.h"

namespace evd {
  /// \brief Option for drawing plots on the "PlotView"
  class PlotDrawingOptions : public evdb::Reconfigurable
  {
  public:
    
    explicit PlotDrawingOptions(fhicl::ParameterSet const& pset,
				art::ActivityRegistry& reg);
    ~PlotDrawingOptions();
    
    void reconfigure(fhicl::ParameterSet const& pset) override;
    
    ///
    /// The pad descriptions should follow this pattern:
    ///
    /// [module]/[object]/[option1,option2,option3] + [module]/[object] + ...
    ///
    /// All plots listed with "+" will be superimposed on a single pad.
    ///
    /// module  - Which module produced the plot?
    /// object  - Name of the object (TH1F, TGraph, ...) to be plotted
    /// options - Inlucdes pad and drawing options:
    ///           logx - make pad have log x axis
    ///           logy - make pad have log y axis
    ///           logz - make pad have log z axis
    ///           gridx - show x grid lines
    ///           gridy - show y grid lines
    ///           lcolor=[n] - Set line color of plot (2=red,3=green,..)
    ///           lstyle=[n] - Set line style of plot
    ///           lwidth=[n] - Set line width of plot
    ///           mcolor=[n] - Set marker color of plot (2=red,3=green,..)
    ///           mstyle=[n] - Set marker style of plot
    ///           msize=[f]  - Set marker size of plot
    ///           all others are passed to Draw(...) directly.
    ///
    std::vector<std::string> fPadDescription; ///< What's in a pad?
  };
}
#endif // __CINT__
DECLARE_ART_SERVICE(evd::PlotDrawingOptions, LEGACY)
#endif
////////////////////////////////////////////////////////////////////////
