//
// - ROOT-based 3D event display for EMPHATIC. Uses the TEve display
//   classes.  Requires EvtDisplayUtils, NavState, and EvtDisplayService.
//

// EMPHATIC includes
#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"

// art includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// ROOT includes
// ... libCore
#include <TApplication.h>
#include <TString.h>
#include <TSystem.h>
// ... libRIO
#include <TFile.h>
// ... libGui
#include <TGString.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGTextView.h>
#include <TGLayout.h>
#include <TGTab.h>
#include <TG3DLine.h>
// ... libGeom
#include <TGeoManager.h>
#include <TGeoTube.h>
#include <TGeoBBox.h>
#include <TGeoCompositeShape.h>
#include <TGeoBoolNode.h>
// ... libEG
#include <TParticle.h>
// ... libRGL
#include <TGLViewer.h>
// ... libEve
#include <TEveGeoShape.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveBrowser.h>
#include <TEveGeoNode.h>
#include <TEveViewer.h>
#include <TEveScene.h>
#include <TEveProjectionManager.h>
#include <TEveProjectionAxes.h>
#include <TEvePointSet.h>
#include <TEveTrack.h>
#include <TEveTrackPropagator.h>
#include <TEveLine.h>

//#include <sstream>
#include <map>

#include "EventDisplay/EvtDisplayUtils.h"
#include "RecoBase/SSDCluster.h"
#include "RecoBase/LineSegment.h"
#include "DetGeoMap/DetGeoMap.h"
#include "Simulation/Particle.h"

// ... Anonymous namespace for helpers.
namespace {

  // ... Helper for setting color and transparency of detector objects
  void setRecursiveColorTransp(TGeoVolume *vol, Int_t color, Int_t transp)
  {
     if(color>=0)vol->SetLineColor(color);
     if(transp>=0)vol->SetTransparency(transp);
     Int_t nd = vol->GetNdaughters();
     for (Int_t i=0; i<nd; i++) {
        setRecursiveColorTransp(vol->GetNode(i)->GetVolume(), color, transp);
     }
  }

}

namespace emph {

  class EventDisplay3D : public art::EDAnalyzer {

  public:

    explicit EventDisplay3D(fhicl::ParameterSet const& pset);

    void beginJob() override;
    void endJob()   override;
    void beginRun( const art::Run& run ) override;
    void analyze(const art::Event& event) override;

  private:

    // Set by parameter set variables.
    Double_t        camRotateCenterH_;
    Double_t        camRotateCenterV_;
    Double_t        camDollyDelta_;
    bool            fDrawMCTruth;
    std::string     fMCTruthLabel;
    bool            fDrawSSDClusters;
    std::string     fSSDClustLabel;
    bool            fDrawTracks;
    std::string     fTracksLabel;
    bool            fDrawVertices;
    std::string     fVerticesLabel;
    int             fVisLevel;

    std::unordered_map<int,int> partColor;

    art::ServiceHandle<emph::geo::GeometryService> geom_;

    std::unique_ptr<emph::EvtDisplayUtils>visutil_;
    TEveGeoShape* fSimpleGeom;

    TEveViewer *fXZView;
    TEveViewer *fYZView;
    TEveProjectionManager *fXZMgr;
    TEveProjectionManager *fYZMgr;
    TEveScene *fDetXZScene;
    TEveScene *fDetYZScene;
    TEveScene *fEvtXZScene;
    TEveScene *fEvtYZScene;

    TGTextEntry      *fTeRun,*fTeSubRun,*fTeEvt;
    TGLabel          *fTlRun,*fTlSubRun,*fTlEvt;

    TEveTrackList *fTrackList;
    TEveElementList *fSSDClustsList;
    TEveElementList* fMCTrueParticleList;

    dgmap::DetGeoMap* fDetGeoMap;

    void makeNavPanel();
    void drawSSDClust(Int_t mColor, Int_t mSize, const rb::SSDCluster& clust);
    void drawMCParticle(Int_t mColor, Int_t mSize, const sim::Particle& part);
    void DrawSSDClusters(const art::Event& event);
    void DrawMCTruth(const art::Event& event);
    void DrawTracks(const art::Event& event);
    void DrawVertices(const art::Event& event);

  };

}

emph::EventDisplay3D::EventDisplay3D(fhicl::ParameterSet const& pset):
  art::EDAnalyzer(pset),
  camRotateCenterH_ ( pset.get<Double_t>   ("camRotateCenterH", 0.26) ),
  camRotateCenterV_ ( pset.get<Double_t>   ("camRotateCenterV",-2.  ) ),
  camDollyDelta_    ( pset.get<Double_t>   ("camDollyDelta",500.) ),
  fDrawMCTruth      ( pset.get<bool>       ("DrawMCTruth",true) ),
  fMCTruthLabel     ( pset.get<std::string>("MCTruthLabel","geantgen") ),
  fDrawSSDClusters  ( pset.get<bool>       ("DrawSSDClusters",true) ),
  fSSDClustLabel    ( pset.get<std::string>("SSDClustLabel","clust") ),
  fDrawTracks       ( pset.get<bool>       ("DrawTracks",true) ),
  fTracksLabel      ( pset.get<std::string>("TracksLabel","ssdreco") ),
  fDrawVertices     ( pset.get<bool>       ("DrawVertices",true) ),
  fVerticesLabel    ( pset.get<std::string>("VerticesLabel","vtxreco") ),
  fVisLevel         ( pset.get<int>        ("VisLevel",4) ),
  geom_(art::ServiceHandle<emph::geo::GeometryService>()),
  visutil_(new emph::EvtDisplayUtils()),
  fSimpleGeom(0),
  fXZView(0),fYZView(0),fXZMgr(0),fYZMgr(0),
  fDetXZScene(0),fDetYZScene(0),fEvtXZScene(0),fEvtYZScene(0),
  fTeRun(0),fTeSubRun(0),fTeEvt(0),
  fTlRun(0),fTlSubRun(0),fTlEvt(0),
  fTrackList(0),fSSDClustsList(0),fDetGeoMap(NULL)
{

  partColor[211] = kGreen-2;
  partColor[2212] = kRed;
  partColor[-211] = kGreen+2;
  partColor[-321] = kYellow+2;
  partColor[321] = kYellow-2;
  partColor[-11] = kWhite+2;
  partColor[11] = kWhite-2;
  //  if ( trkMaxStepSize_ < 0.1 )trkMaxStepSize_ = 0.1;

}

void emph::EventDisplay3D::makeNavPanel()
{
  std::cout << "%%%%% EventDisplay3d::makeNavPanel() %%%%%" << std::endl;

  // Create control panel for event navigation
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft); // insert nav frame as new tab in left pane

  TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  frmMain->SetWindowName("EVT NAV");
  frmMain->SetCleanup(kDeepCleanup);

  TGHorizontalFrame* navFrame = new TGHorizontalFrame(frmMain);
  TGVerticalFrame* evtidFrame = new TGVerticalFrame(frmMain);
  {
    TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")) );
    TGPictureButton* b = 0;

    // ... Create back button and connect to "PrevEvent" rcvr in visutils
    b = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "GoBack.gif"));
    navFrame->AddFrame(b);
    b->Connect("Clicked()", "emph::EvtDisplayUtils", visutil_.get(), "PrevEvent()");

    // ... Create forward button and connect to "NextEvent" rcvr in visutils
    b = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "GoForward.gif"));
    navFrame->AddFrame(b);
    b->Connect("Clicked()", "emph::EvtDisplayUtils", visutil_.get(), "NextEvent()");

    // ... Create reload button and connect to "ReloadFCL" rcvr in visutils
    b = new TGPictureButton(navFrame, gClient->GetPicture(icondir + "ReloadPage.gif"));
    navFrame->AddFrame(b);
    b->Connect("Clicked()", "emph::EvtDisplayUtils", visutil_.get(), "ReloadFHICL()");

    // ... Create run num text entry widget and connect to "GotoEvent" rcvr in visutils


    TGHorizontalFrame* runoFrame = new TGHorizontalFrame(evtidFrame);
    fTlRun = new TGLabel(runoFrame,"Run Number");
    fTlRun->SetTextJustify(kTextLeft);
    fTlRun->SetMargins(5,5,5,0);
    runoFrame->AddFrame(fTlRun);
    
    fTeRun = new TGTextEntry(runoFrame, visutil_->fTbRun = new TGTextBuffer(5), 1);
    visutil_->fTbRun->AddText(0, "1");
    fTeRun->Connect("ReturnPressed()","emph::EvtDisplayUtils", visutil_.get(),"GotoEvent()");
    runoFrame->AddFrame(fTeRun,new TGLayoutHints(kLHintsExpandX));

    // ... Create subrun num text entry widget
    TGHorizontalFrame* subrunoFrame = new TGHorizontalFrame(evtidFrame);
    fTlSubRun = new TGLabel(runoFrame,"SubRun Number");
    fTlSubRun->SetTextJustify(kTextLeft);
    fTlSubRun->SetMargins(5,5,5,0);
    runoFrame->AddFrame(fTlSubRun);
    
    fTeSubRun = new TGTextEntry(subrunoFrame, visutil_->fTbSubrun = new TGTextBuffer(5), 1);
    visutil_->fTbSubrun->AddText(0, "1");
    //    fTeSubRun->SetEnabled(kFalse);
    //    fTeSubRun->Connect("ReturnPressed()","emph::EvtDisplayUtils", visutil_.get(),"GotoEvent()");
    subrunoFrame->AddFrame(fTeSubRun,new TGLayoutHints(kLHintsExpandX));
    
    // ... Create evt num text entry widget and connect to "GotoEvent" rcvr in visutils    
    TGHorizontalFrame* evnoFrame = new TGHorizontalFrame(evtidFrame);
    fTlEvt = new TGLabel(evnoFrame,"Evt Number");
    fTlEvt->SetTextJustify(kTextLeft);
    fTlEvt->SetMargins(5,5,5,0);
    evnoFrame->AddFrame(fTlEvt);

    fTeEvt = new TGTextEntry(evnoFrame, visutil_->fTbEvt = new TGTextBuffer(5), 1);
    visutil_->fTbEvt->AddText(0, "1");
    fTeEvt->Connect("ReturnPressed()","emph::EvtDisplayUtils", visutil_.get(),"GotoEvent()");
    evnoFrame->AddFrame(fTeEvt,new TGLayoutHints(kLHintsExpandX));

    // ... Add horizontal run & event number subframes to vertical evtidFrame
    evtidFrame->AddFrame(runoFrame,new TGLayoutHints(kLHintsExpandX));
    evtidFrame->AddFrame(subrunoFrame,new TGLayoutHints(kLHintsExpandX));
    evtidFrame->AddFrame(evnoFrame,new TGLayoutHints(kLHintsExpandX));

    // ... Add navFrame and evtidFrame to MainFrame
    frmMain->AddFrame(navFrame);
    TGHorizontal3DLine *separator = new TGHorizontal3DLine(frmMain);
    frmMain->AddFrame(separator, new TGLayoutHints(kLHintsExpandX));
    frmMain->AddFrame(evtidFrame);

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Nav", 0);
  }
}

void emph::EventDisplay3D::beginJob()
{
  std::cout << "%%%%% Start of EventDisplay3d::beginJob() %%%%%" << std::endl;

  // Initialize global Eve application manager (return gEve)
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TEveManager::Create();

  // Create detector and event scenes for ortho views
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fDetXZScene = gEve->SpawnNewScene("Det XZ Scene", "");
  fDetYZScene = gEve->SpawnNewScene("Det YZ Scene", "");
  fEvtXZScene = gEve->SpawnNewScene("Evt XZ Scene", "");
  fEvtYZScene = gEve->SpawnNewScene("Evt YZ Scene", "");

  // Create XZ/YZ projection mgrs, draw projected axes, & add them to scenes
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  fXZMgr = new TEveProjectionManager(TEveProjection::kPT_XZ);
  TEveProjectionAxes* axes_xz = new TEveProjectionAxes(fXZMgr);
  fDetXZScene->AddElement(axes_xz);

  fYZMgr = new TEveProjectionManager(TEveProjection::kPT_YZ);
  TEveProjectionAxes* axes_yz = new TEveProjectionAxes(fYZMgr);
  fDetYZScene->AddElement(axes_yz);

  // Create side-by-side ortho XZ & YZ views in new tab & add det/evt scenes
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TEveWindowSlot *slot = 0;
  TEveWindowPack *pack = 0;

  slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  pack = slot->MakePack();
  pack->SetElementName("2D Views");
  pack->SetVertical();
  pack->SetShowTitleBar(kFALSE);

  pack->NewSlot()->MakeCurrent();
  fXZView = gEve->SpawnNewViewer("XZ View", "");
  fXZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOX);
  fXZView->AddScene(fDetXZScene);
  fXZView->AddScene(fEvtXZScene);

  pack->NewSlot()->MakeCurrent();
  fYZView = gEve->SpawnNewViewer("YZ View", "");
  fYZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
  fYZView->AddScene(fDetYZScene);
  fYZView->AddScene(fEvtYZScene);

  gEve->GetBrowser()->GetTabRight()->SetTab(0);

  // Create navigation panel
  // ~~~~~~~~~~~~~~~~~~~~~~~~
  makeNavPanel();

  // Add new Eve event into the "Event" scene and make it the current event
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // (Subsequent elements added using "AddElements" will be added to this event)
  gEve->AddEvent(new TEveEventManager("Event", "EMPHATIC Event"));

  // ... Set up initial camera orientation in main 3d view
  //   - rotate camera by "camRotateCenterH_" radians about horizontal going through
  //     center, followed by "camRotateCenterV_" radians about vertical going through
  //     center
  //   - move camera by "camDollyDelta_" units towards(+'ve) or away from(-'ve) center,
  //     combination of two boolean args controls sensitivity.
  TGLViewer *glv = gEve->GetDefaultGLViewer();
  //  glv->SetName("3D View");
  glv->SetGuideState(TGLUtil::kAxesEdge, kTRUE, kFALSE, 0);
  glv->CurrentCamera().RotateRad(camRotateCenterH_,camRotateCenterV_);
  //  glv->CurrentCamera().Dolly(camDollyDelta_,kFALSE,kFALSE);

  std::cout << "%%%%% End of EventDisplay3d::beginJob() %%%%%" << std::endl;

}

void emph::EventDisplay3D::beginRun( const art::Run& )
{
  std::cout << "%%%%% Start of EventDisplay3d::beginRun() %%%%%" << std::endl;

  gEve->GetGlobalScene()->DestroyElements();
  fDetXZScene->DestroyElements();
  fDetYZScene->DestroyElements();

  std::cout << "%%%%% EventDisplay3d cleaned up from previous event %%%%%" << std::endl;
  
  auto geo    = geom_->Geo();
  auto geoMgr = geo->ROOTGeoManager();
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // make 2D views
  TEveElementList* det2D = new TEveElementList("det2D");

  // add SSDStations

  std::cout << "%%%%% EventDisplay3d adding SSDStations %%%%%" << std::endl;

  int nSSDStations = geo->NSSDStations();
  std::string stationId="SSD Station %d";
  for (int i=0; i<nSSDStations; ++i) {
    std::cout << "%%%%% Adding Station " << i << std::endl;
    auto station = geo->GetSSDStation(i);
    TEveGeoShape* egs = new TEveGeoShape(Form(stationId.c_str(),i));
    double stPos[3];
    station.Pos().GetXYZ(stPos);
    std::cout << "stPos = (" << stPos[0] << "," << stPos[1] << "," << stPos[2] << ")" << std::endl;
    egs->SetShape(new TGeoBBox(station.Width(),station.Height(),station.Dz(),stPos));
    egs->SetMainColor(kGreen+1);
    det2D->AddElement(egs);
  }
  
  std::cout << "%%%%% EventDisplay3d done adding SSDStations %%%%%" << std::endl;

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  auto world_n = geoMgr->GetTopNode();
  auto etopnode = new TEveGeoTopNode(geoMgr, world_n);

  etopnode->SetVisLevel(fVisLevel); 
  //  etopnode->GetNode()->GetVolume()->SetVisibility(kFALSE);

  // ... Use helper to recursively make detector elements
  //     transparent & set custom colors
  setRecursiveColorTransp(etopnode->GetNode()->GetVolume(), kCyan-10, 70);

  // ... Add static detector geometry to global scene
  gEve->AddGlobalElement(etopnode);

  // Draw the 2D projections using the EVE element list created above 
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // .... Add the EVE element list to the global scene first
  gEve->AddGlobalElement(det2D);

  // ... Import elements of the list into the projected views
  fXZMgr->ImportElements(det2D, fDetXZScene);
  fYZMgr->ImportElements(det2D, fDetYZScene);

  // ... Turn OFF rendering of duplicate detector in main 3D view
  gEve->GetGlobalScene()->FindChild("det2D")->SetRnrState(kFALSE);

  // ... Turn ON rendering of detector in 2D views
  fDetXZScene->FindChild("det2D [P]")->SetRnrState(kTRUE);
  fDetYZScene->FindChild("det2D [P]")->SetRnrState(kTRUE);

}

//------------------------------------------------------------
// ... Helper for drawing hit as a TEvePointSet with specified color, markersize, and hit index
void emph::EventDisplay3D::drawSSDClust(Int_t mColor, Int_t mSize, 
				      const rb::SSDCluster& cl)
{

  rb::LineSegment ls;
  if (! fDetGeoMap)
    fDetGeoMap = new dgmap::DetGeoMap();

  if (fDetGeoMap->SSDClusterToLineSegment(cl, ls)) {
    //    std::cout << ls << std::endl;
    
    TEveLine* l = new TEveLine();
    
    l->SetNextPoint(ls.X0()[0], ls.X0()[1], ls.X0()[2]);
    l->SetNextPoint(ls.X1()[0], ls.X1()[1], ls.X1()[2]);
    l->SetLineColor(mColor);
    l->SetMarkerSize(mSize);
    fSSDClustsList->AddElement(l);
  }

}

//------------------------------------------------------------

void emph::EventDisplay3D::DrawSSDClusters(const art::Event& event)
{
  if (fSSDClustsList == 0) {
    fSSDClustsList = new TEveElementList("SSD Clusters"); 
    fSSDClustsList->IncDenyDestroy();              // protect element against destruction
  }
  else {
    fSSDClustsList->DestroyElements();             // destroy children of the element
  }
  
  try {
    
    //    art::Handle<std::vector<rb::SSDCluster> > ssdClusters;
    auto ssdClusters = event.getHandle<std::vector<rb::SSDCluster>>(fSSDClustLabel);
    
    if  (!ssdClusters->empty()) {
      for (size_t idx=0; idx<ssdClusters->size(); ++idx) {
	const rb::SSDCluster& cl = (*ssdClusters)[idx];
	drawSSDClust(kRed,4,cl);
      }
    }
  }
  catch(...) {
    
  }
  //  fSSDClustsList->AddElement(SSDHitsList);  
  gEve->AddElement(fSSDClustsList);    

}

//------------------------------------------------------------

void emph::EventDisplay3D::DrawMCTruth(const art::Event& event)
{
  if (fMCTrueParticleList == 0) {
    fMCTrueParticleList = new TEveElementList("MC Particles"); 
    fMCTrueParticleList->IncDenyDestroy();              // protect element against destruction
  }
  else {
    fMCTrueParticleList->DestroyElements();             // destroy children of the element
  }

  try {    
    //    art::Handle<std::vector<rb::SSDCluster> > ssdClusters;
    auto particles = event.getHandle<std::vector<sim::Particle>>(fMCTruthLabel);
    
    if  (!particles->empty()) {
      for (size_t idx=0; idx<particles->size(); ++idx) {
	const sim::Particle& part = (*particles)[idx];
	drawMCParticle(partColor[part.fpdgCode],4,part);
      }
    }
  }
  catch(...) {
    std::cerr << "No true particles found under label: " << fMCTruthLabel 
	      << std::endl;
  }

  gEve->AddElement(fMCTrueParticleList);
}

//------------------------------------------------------------

void emph::EventDisplay3D::drawMCParticle(Int_t mColor, Int_t mSize, 
					  const sim::Particle& part)
{ 
  
  double x0[3];
  double x1[3];
  TEveLine* l = new TEveLine();    
  std::cout << "drawMCParticle: " << part.fpdgCode << " (" 
	    << part.ftrajectory.X(0) << "," << part.ftrajectory.Y(0) << "," 
	    << part.ftrajectory.Z(0) << ")";
  l->SetNextPoint(part.ftrajectory.X(0), part.ftrajectory.Y(0), part.ftrajectory.Z(0));  
  for (size_t i=1; i<part.ftrajectory.NPoints(); ++i) {
    std::cout << "->(" << part.ftrajectory.X(i) << ","
	      << part.ftrajectory.Y(i) <<"," << part.ftrajectory.Z(i) <<")";
    l->SetNextPoint(part.ftrajectory.X(i), part.ftrajectory.Y(i), part.ftrajectory.Z(i));  
  }
  std::cout << std::endl;

  l->SetMarkerSize(mSize);
  l->SetLineColor(mColor);
  fMCTrueParticleList->AddElement(l);
  
}

//------------------------------------------------------------

void emph::EventDisplay3D::DrawTracks(const art::Event& )
{

}

//------------------------------------------------------------

void emph::EventDisplay3D::DrawVertices(const art::Event& )
{

}

//------------------------------------------------------------

void emph::EventDisplay3D::analyze(const art::Event& event )
{
  //  std::cout << "%%%%% Entering EventDisplay3D::analyze() %%%%% " << std::endl;

  // ... Update the run and event numbers in the TGTextEntry widgets in the Navigation panel
  std::ostringstream sstr;
  sstr << event.id().run();
  visutil_->fTbRun->Clear();
  visutil_->fTbRun->AddText(0,sstr.str().c_str());
  gClient->NeedRedraw(fTeRun);

  //  std::cout << "%%%%% Updating run %%%%% " << std::endl;

  sstr.str("");
  sstr << event.id().event();
  visutil_->fTbEvt->Clear();
  visutil_->fTbEvt->AddText(0,sstr.str().c_str());
  gClient->NeedRedraw(fTeEvt);

  //  std::cout << "%%%%% Updating event %%%%% " << std::endl;

  // ... Delete visualization structures associated with previous event
  //  std::cout << "%%%%% Deleting visualization structures... %%%%% " << std::endl;
  gEve->GetViewers()->DeleteAnnotations();
  gEve->GetCurrentEvent()->DestroyElements();
  //  std::cout << "%%%%% Deleted visualization structures. %%%%% " << std::endl;

  // Draw MC truth
  if (fDrawMCTruth) DrawMCTruth(event); // currently does nothing

  // Draw reco objects
  // ~~~~~~~~~~~~~~~~~~~~~~~  
  if (fDrawSSDClusters) DrawSSDClusters(event);
  if (fDrawTracks)      DrawTracks(event); // currently does nothing
  if (fDrawVertices)    DrawVertices(event); // currently does nothing


  // Draw the generated tracks as helices in a uniform axial field
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  

//  if (drawGenTracks_) {
//    auto gens = event.getValidHandle<GenParticleCollection>(gensTag_);
/*
  if (fTrackList == 0) {
    fTrackList = new TEveTrackList("Tracks"); 
    fTrackList->SetLineWidth(4);
    fTrackList->IncDenyDestroy();                 // protect element against destruction
  }
  else {
    fTrackList->DestroyElements();                // destroy children of the element
  }

  TEveTrackPropagator* trkProp = fTrackList->GetPropagator();
  trkProp->SetMagField(0.,0.,1.2);
  trkProp->SetMaxR(1000.);
  trkProp->SetMaxZ(2000.);
  trkProp->SetMaxStep(3.);
  
  //    int mcindex=-1;
  //    for ( auto const& gen: *gens){
  //      mcindex++;
  // ... Skip tracks decayed in the generator.
  //      if ( gen.hasChildren() ) continue;
  TParticle mcpart;
  mcpart.SetMomentum(0.,0.,4.,4.109);
  mcpart.SetProductionVertex(0.,0.,-100.,0.);
  mcpart.SetPdgCode(2212);
  TEveTrack* track = new TEveTrack(&mcpart,0,trkProp);
  track->SetIndex(0);
  track->SetStdTitle();
  track->SetAttLineAttMarker(fTrackList);
  track->SetMainColor(kViolet+1);
  fTrackList->AddElement(track);
  fTrackList->MakeTracks();
  gEve->AddElement(fTrackList);
*/
  
  // Import event into ortho views and apply projections
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  TEveElement* currevt = gEve->GetCurrentEvent();

  fEvtXZScene->DestroyElements();
  fXZMgr->ImportElements(currevt, fEvtXZScene);

  fEvtYZScene->DestroyElements();
  fYZMgr->ImportElements(currevt, fEvtYZScene);

} // end emph::EventDisplay3D::analyze

void emph::EventDisplay3D::endJob(){

}

DEFINE_ART_MODULE(emph::EventDisplay3D)
