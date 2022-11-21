//
// Draw two views of the event, xy and rz.
//

#include "EventDisplay/EnsureTApplication.h"
#include "EventDisplay/PlotOrientation.h"
#include "EventDisplay/WaitFor.h"

#include "Geometry/Geometry.h"
#include "Geometry/GeometryService.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Rtypes.h"
#include "TBox.h"
#include "TArc.h"
#include "TArrow.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TLine.h"
#include "TPad.h"
#include "TString.h"

#include <cmath>
#include <iostream>

// Anonymous namespace for helpers.
  /*
namespace {

  // A helper class to manage the TGraph objects and the information needed to
  // create them.
  struct Helper {

    Helper(size_t n,
           int acolor,
           std::string const& opt,
           tex::GenParticle const* agen = 0)
      : color(acolor), drawingOption(opt), gen(agen), xyp(), rzp()
    {
      x.reserve(n);
      y.reserve(n);
      z.reserve(n);
      r.reserve(n);
    }

    void
    addPoint(CLHEP::Hep3Vector const& p)
    {
      x.push_back(p.x());
      y.push_back(p.y());
      z.push_back(p.z());
      r.push_back(p.perp());
    }

    bool
    empty() const
    {
      return x.empty();
    }

    size_t
    size() const
    {
      return x.size();
    }

    CLHEP::Hep3Vector
    back() const
    {
      if (x.empty()) {
        return CLHEP::Hep3Vector();
      }
      return CLHEP::Hep3Vector(x.back(), y.back(), z.back());
    }

    TGraph*
    drawXY()
    {

      if (empty())
        return xyp.get();

      xyp = std::unique_ptr<TGraph>(new TGraph(size(), &x.front(), &y.front()));
      xyp->SetMarkerColor(color);
      xyp->SetLineColor(color);
      xyp->SetMarkerStyle(kFullDotLarge);
      xyp->Draw(drawingOption.c_str());

      return xyp.get();
    }

    TGraph*
    drawRZ()
    {

      if (empty())
        return rzp.get();

      rzp = std::unique_ptr<TGraph>(new TGraph(size(), &z.front(), &r.front()));
      rzp->SetMarkerColor(color);
      rzp->SetLineColor(color);
      rzp->SetMarkerStyle(kFullDotLarge);
      rzp->Draw(drawingOption.c_str());

      return rzp.get();
    }

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    std::vector<double> r;

    int color;
    std::string drawingOption;
    tex::GenParticle const* gen;

    std::unique_ptr<TGraph> xyp;
    std::unique_ptr<TGraph> rzp;
  };

}
  */

namespace emph {
  
  class EventDisplay : public art::EDAnalyzer {
    
  public:
    explicit EventDisplay(fhicl::ParameterSet const& pset);
    
    void beginJob() override;
    void endJob() override;
    void analyze(const art::Event& event) override;
    
  private:
    // When this object is intialized, it will ensure that
    // interactive ROOT is properly initialized
    EnsureTApplication ensureTApp_;
    
    // Set by parameter set variables.
    /*
    art::InputTag gensTag_;
    bool drawGenTracks_;
    bool drawHits_;
    */

    bool prompt_;
    WaitFor waitFor_;
    bool multipage_;
    std::string plotFileName_;
    PlotOrientation orientation_;

    int nStepsTrack_;
    int maxDisplay_;
    int verbosity_;
    
    std::string plotFileBase_;
    std::string plotFileType_;
    
    art::ServiceHandle<emph::geo::GeometryService> geom_;
    //    art::ServiceHandle<PDT> pdt_;
    TCanvas* canvas_;

    TH1F* hFrame_;
    int displayCount_;

    // Various in-class helper methods.
    void DrawSSDs(std::string view);
    
    void parsePlotFileName();
    
    //    int selectColor(PDGCode::type id);
    
    /*
      void fillHits(
      std::vector<art::Handle<IntersectionCollection>> const& handles,
      Helper& h1,
      Helper& h2,
      Helper& h3);

    void fillTracks(GenParticleCollection const& gens, std::vector<Helper>& h);
    void fillTrack(GenParticle const& gen, Helper& h);

    */
    
    void print(TCanvas*, int);

    void waitForKeyboard();
    void waitForMouse();
    void checkParameters();
  };

}

//------------------------------------------------------------

emph::EventDisplay::EventDisplay(fhicl::ParameterSet const& pset)
  : art::EDAnalyzer(pset)
  , ensureTApp_()
    //  , gensTag_(pset.get<std::string>("genParticleTag"))
    //  , drawGenTracks_(pset.get<bool>("drawGenTracks", true))
    //  , drawHits_(pset.get<bool>("drawHits", true))
  , prompt_(pset.get<bool>("prompt", true))
  , waitFor_(pset.get<std::string>("waitFor", "mouse"))
  , multipage_(pset.get<bool>("multipage", false))
  , plotFileName_(pset.get<std::string>("plotFileName", ""))
  , orientation_(pset.get<std::string>("orientation", "portrait"))
  , nStepsTrack_(pset.get<int>("nStepsTrack", 100))
  , maxDisplay_(pset.get<int>("maxDisplay", 0))
  , verbosity_(pset.get<int>("verbosity", 0))
  , plotFileBase_()
  , plotFileType_()
  , geom_()
    //  , pdt_()
  , displayCount_(0)
{

  checkParameters();
  // Split the plot file name into a base and a type.
  parsePlotFileName();
}

//------------------------------------------------------------

void emph::EventDisplay::checkParameters()
{
  // Limit nStepsTrack
  if (nStepsTrack_ < 25) {
    nStepsTrack_ = 25;
  } else if (nStepsTrack_ > 1000) {
    nStepsTrack_ = 1000;
  }
}

//------------------------------------------------------------

void emph::EventDisplay::beginJob()
{
  
  int windowSizeShort(600);
  int windowSizeLong(1200);
  if (orientation_ == PlotOrientation::portrait) {
    canvas_ = new TCanvas(
			  "canvas_Test", "Event Display", windowSizeShort, windowSizeLong);
  } else {
    canvas_ = new TCanvas(
			  "canvas_Test", "Event Display", windowSizeLong, windowSizeShort);
  }
  canvas_->Modified();
  canvas_->Update();
  
  // Open a multipage plot file.
  if (multipage_) {
    if (verbosity_ > 1) {
      std::cout << "Opening multipage plot file: " << plotFileName_
                << std::endl;
    }
    canvas_->Print(std::string(plotFileName_ + "[").c_str());
  }
}

//------------------------------------------------------------

void emph::EventDisplay::endJob()
{
  
  // Close multipage plot file.
  if (multipage_) {
    if (verbosity_ > 1) {
      std::cout << "Closing multipage plot file: " << plotFileName_
                << std::endl;
    }
    canvas_->Print(std::string(plotFileName_ + "]").c_str());
  }
}

//------------------------------------------------------------

void emph::EventDisplay::DrawSSDs(std::string view)
{
  /*
  int nSSDs = _geom->NSSDs();

  std::vector<double> zbound(nSSDs);
  std::vector<double> wbound(nSSDs);
  */

  auto geo = geom_->Geo();
  int nSSDStations = geo->NSSDStations();

  TBox stBox;
  for (int i=0; i<nSSDStations; ++i) {
    auto station = geo->GetSSDStation(i);
    if (view == "xz") {
      stBox.DrawBox(-station.Dz(),-station.Width(),station.Dz(),station.Width());
    }
    if (view == "yz") {
      stBox.DrawBox(-station.Dz(),-station.Height(),station.Dz(),station.Height());
    }
  }
  
}

//------------------------------------------------------------

void emph::EventDisplay::analyze(const art::Event& event)
{
  
  if (displayCount_ > maxDisplay_)
    return;
  int pageCount = displayCount_++;
  
  // Guess at how much space to reserve inside the Helpers;
  // This computation will usually reserve a little too much but the wasted
  // space is small enough that this is OK.
  /*
  int nReserve = 0;
  for (auto const& handle : hitsHandles) {
    nReserve += handle->size();
  }
  
  // Populate the helper objects describing the generated trajectories.
  std::vector<Helper> chargedTracks;
  fillTracks(*gens, chargedTracks);
  
  // Populate the helper objects holding the hits.
  Helper h1(nReserve, kRed, "P"), h2(nReserve, kBlue, "P"),
    h3(nReserve, kGreen, "P");
  fillHits(hitsHandles, h1, h2, h3);
  
  Tracker const& tracker(geom_->tracker());
  */
      
  // Size of the frames is set by the geometry's world size.
  //  std::vector<double> const& world = geom_->worldHalfLengths();
  
  // ROOT shape drawing objects.
  TArc arc;
  arc.SetFillStyle(0);
  TLine line;
  
  // Clear page.
  canvas_->cd(0);
  canvas_->Clear();
  
  // Split the canvas into two pads
  if (orientation_ == PlotOrientation::portrait) {
    canvas_->Divide(1, 2);
  } else {
    canvas_->Divide(2, 1);
  }
  
  std::ostringstream os;
  os << event.id();
  std::string eventID = os.str();
  
  // Draw first plot.
  canvas_->cd(1);
  
  // Draw the geometry.
  auto geo = geom_->Geo();

  std::string titleXZ = "( " + eventID + ")  X vs Z;[mm];[mm]";
  double zsize = geo->WorldLength();
  double xsize = geo->WorldWidth();
  hFrame_ = gPad->DrawFrame(-zsize, -xsize, zsize, xsize, titleXZ.c_str());
  DrawSSDs("xz");
  /*
    for (auto const& shell : tracker.shells()) {
    arc.DrawArc(0., 0., shell.radius());
    }
  */
  
  // Draw second plot.
  canvas_->cd(2);
  
  // Draw the geometry.
  std::string titleYZ = "( " + eventID + ")  Y vs Z;[mm];[mm]";
  double ysize = geo->WorldHeight();
  hFrame_ = gPad->DrawFrame(-zsize, -ysize, zsize, ysize, titleYZ.c_str());
  DrawSSDs("yz");
  /*
    for (auto const& shell : tracker.shells()) {
    line.DrawLine(
    -shell.halfLength(), shell.radius(), shell.halfLength(), shell.radius());
    }
  */
  
  /*
    if (drawHits_) {
    h1.drawRZ();
    h2.drawRZ();
    h3.drawRZ();
    }
    
    if (drawGenTracks_) {
    for (auto& track : chargedTracks) {
    track.drawRZ();
    }
    }
  */
  
  // Flush contents to the screen.
  canvas_->Modified();
  canvas_->Update();
  
  // Print the canvas, if requested.
  print(canvas_, pageCount);
  
  // Wait for user input.
  if (prompt_) {
    if (waitFor_ == WaitFor::keyboard) {
      waitForKeyboard();
    } else if (waitFor_ == WaitFor::mouse) {
      waitForMouse();
    }
  }
  
} // end emph::EventDisplay::analyze

/*
int emph::EventDisplay::selectColor(PDGCode::type id)
    {
      if (id == PDGCode::K_plus)
	return kRed;
      if (id == PDGCode::K_minus)
	return kBlue;
      return kGreen;
    }
*/

/*
void
tex::EventDisplay::fillHits(
			    std::vector<art::Handle<IntersectionCollection>> const& handles,
			    Helper& h1,
			    Helper& h2,
			    Helper& h3)
    {

      for (auto const& handle : handles) {
	for (auto const& hit : *handle) {
	  if (hit.genTrack()->pdgId() == PDGCode::K_plus) {
	    h1.addPoint(hit.position());
	  } else if (hit.genTrack()->pdgId() == PDGCode::K_minus) {
	    h2.addPoint(hit.position());
	  } else {
	    h3.addPoint(hit.position());
	  }
	}
      }
    }

void
tex::EventDisplay::fillTracks(GenParticleCollection const& gens,
                              std::vector<Helper>& tracks)
    {

      tracks.reserve(gens.size());

      for (auto const& gen : gens) {

	// Skip particles that are already decayed.
	if (gen.hasChildren())
	  continue;

	int color = selectColor(gen.pdgId());
	tracks.push_back(Helper(100, color, "L", &gen));

	fillTrack(gen, tracks.back());
      }

    } // end tex::EventDisplay::fillTracks.

void
tex::EventDisplay::fillTrack(GenParticle const& gen, Helper& h)
    {

      double q = pdt_->getById(gen.pdgId()).charge();
      double bz(geom_->bz());

      Helix trk(gen.position(), gen.momentum().vect(), q, bz);

      // 3d path length to the end of the first outgoing arc.
      double smax = M_PI * trk.rho() / trk.sinTheta();

      // Add points along the trajectory.
      // N steps = N+1 points, to include the beginning of first step and
      //                       end of the last step.
      // Fixme: The first time that a point is outside the tracker,
      // add a point on the boundary of the tracker; then stop the loop.
      int nPoints = nStepsTrack_ + 1;
      double ds = smax / nStepsTrack_;
      for (int i = 0; i < nPoints; ++i) {
	double s = ds * i;
	CLHEP::Hep3Vector position = trk.position(s);
	if (geom_->tracker().insideTracker(position)) {
	  h.addPoint(position);
	}
      }
    }
*/

//------------------------------------------------------------
// Split name of the plot file into its base and type components.
// Supply a default type if requested.
// Force a file type of .pdf if multipage is selected.
void emph::EventDisplay::parsePlotFileName()
{
  
  if (plotFileName_.empty())
    return;
  
  size_t dotPosition = plotFileName_.rfind('.');
  if (dotPosition != std::string::npos) {
    plotFileBase_ = plotFileName_.substr(0, dotPosition);
    plotFileType_ = plotFileName_.substr(dotPosition);
  } else {
    plotFileBase_ = plotFileName_;
    plotFileType_ = (multipage_) ? ".pdf" : ".png";
  }
  
  if (multipage_) {
    if (plotFileType_ != ".pdf") {
      std::cout << "Warning: multipage has been chosen and file type for the "
	"plot file is not .pdf\n"
		<< "         Forcing file type to pdf." << std::endl;
      plotFileType_ = ".pdf";
      plotFileName_ = plotFileBase_ + plotFileType_;
    }
  }
}

//------------------------------------------------------------

void emph::EventDisplay::print(TCanvas*, int count)
{
  
  if (plotFileName_.empty())
    return;

  if (multipage_) {
    if (verbosity_ > 1) {
      std::cout << "Printing canvas to multipage plot file: " 
		<< plotFileName_ << std::endl;
    }
    canvas_->Print(plotFileName_.c_str());
  } 
  else {
    std::string version = std::to_string(count);
    std::string name = plotFileBase_;
    name += "_";
    name += version;
    name += plotFileType_;
    if (verbosity_ > 1) {
      std::cout << "Printing canvas to plot file: " << name << std::endl;
    }
    canvas_->Print(name.c_str());
  }
}

// Wait for input from the keyboard.
// This is a pain because the prompt window keeps overlaying the graphics
// canvas.
void emph::EventDisplay::waitForKeyboard()
{
  mf::LogPrint("Prompt")
    << "To quit: type the Q key, upper or lower case, plus the return key\n"
    << "To continue: type any key plus the return key:";
  char junk;
  std::cin >> junk;
  std::cerr << junk << std::endl;
  if (junk == 'q' || junk == 'Q') {
    throw cet::exception("USERSHUTDOWN")
      << "User requested shutdown in EventDisplay module.\n";
  }
}

// Wait for double click by mouse in the final TPad.
// This does not perform well over slow networks.
// Also, there is no way to ask the code to shutdown.
void
emph::EventDisplay::waitForMouse()
{
  mf::LogPrint("Prompt")
    << "To continue: double click inside the right hand graphics pad:";
  gPad->WaitPrimitive();
}

DEFINE_ART_MODULE(emph::EventDisplay)
