///////////////////////////////////////////////////////////////////////
/// \brief   Event display ART module
/// \author  brebel@fnal.gov, messier@indiana.edu
//////////////////////////////////////////////////////////////////////

#ifndef __CINT__

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}
#include <string>

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art_root_io/TFileDirectory.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Principal/Handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// emphaticsoft includes
#include "EventDisplayBase/DisplayWindow.h"
#include "EventDisplayServices/RawDrawingOptions.h"
//#include "EventDisplay/XZYZProjectionsView.h"
//#include "EventDisplay/HardwareView.h"
#include "EventDisplay/Display3DView.h"
//#include "EventDisplay/MCTrueView.h"
//#include "EventDisplay/PlotView.h"


namespace evd{

  class EVD : public art::EDAnalyzer 
  {
   public:
     explicit EVD(fhicl::ParameterSet const &pset);
     virtual ~EVD();

     void analyze(art::Event const& evt);
     void beginJob();
   };
}
#endif // __CINT__

////////////////////////////////////////////////////////////////////////

// Builder for the XZYXProjections view
/*static evdb::Canvas* mk_xzyzproj_canvas(TGMainFrame* mf)
{
  return new evd::XZYZProjectionsView(mf);
}
*/

// Builder for the Hardware view
/*static evdb::Canvas* mk_hardware_canvas(TGMainFrame* mf)
{
  return new evd::HardwareView(mf);
}
*/

// Builder for the Display3D view
static evdb::Canvas* mk_display3d_canvas(TGMainFrame* mf)
{
  return new evd::Display3DView(mf);
}

// Builder for the MCTruth view
//static evdb::ObjListCanvas* mk_mctrue_canvas(TGMainFrame* mf)
//{
//  return new evd::MCTrueView(mf);
//}

// Builder for the plots view
/*
static evdb::Canvas* mk_plot_canvas(TGMainFrame* mf)
{
  return new evd::PlotView(mf);
}
*/

namespace evd{

  //----------------------------------------------------
  EVD::EVD(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
  {
    
  }

  //----------------------------------------------------
  EVD::~EVD()
  {
  }

  //----------------------------------------------------
  void EVD::beginJob()
  {

    // Register the list of windows used by the event display
    /*    evdb::DisplayWindow::Register("XZ/YZ Projections",
				  "XZ/YZ Projections",
				  840,
				  1260,
				  mk_xzyzproj_canvas);
    */

    /*    evdb::DisplayWindow::Register("Hardware View",
				  "Hardware View",
				  840,
				  1260,
				  mk_hardware_canvas);
    */
    evdb::DisplayWindow::Register("Display3D",
				  "Display3D",
 				  840,
 				  1260,
 				  mk_display3d_canvas);
    
//     evdb::ListWindow::Register("MC Particle List",
// 			       "MC Particle List",
// 			       400,
// 			       800,
// 			       mk_mctrue_canvas);

/*
    evdb::DisplayWindow::Register("Plots View",
				  "Plots View",
				  840,
				  1260,
				  mk_plot_canvas);
*/

    // Open up the main display window and run
    evdb::DisplayWindow::OpenWindow(0);
    std::cout << "******************** HERE ********************" << std::endl;

  }

  //----------------------------------------------------
  void EVD::analyze(const art::Event& )//evt)
  {
  }
  
} // end namespace evd

////////////////////////////////////////////////////////////////////////
namespace evd
{
  DEFINE_ART_MODULE(EVD)
} // namespace evd
////////////////////////////////////////////////////////////////////////
