////////////////////////////////////////////////////////////////////////
// $Id: G4Alg.cxx,v 1.6 2012-12-03 23:52:01 rhatcher Exp $
//
// Geant4 Driver Module
//
// jpaley@fnal.gov, based on NOvA code (brebel@fnal.gov)
//
////////////////////////////////////////////////////////////////////////
#include <cassert>
#include <cstdlib>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <unistd.h>

// EMPHATIC includes
#include "Geometry/service/GeometryService.h"
#include "G4EMPH/G4Alg.h"
#include "G4Base/UserActionManager.h"
#include "G4Base/UserActionFactory.h"
#include "G4Base/G4Helper.h"
#include "SimulationBase/MCTruth.h"
#include "G4EMPH/SSDHitAction.h"
#include "G4EMPH/FastStopAction.h"
#include "G4EMPH/OpticalAction.h"
//#include "G4EMPH/ParticleListAction.h"
#include "G4EMPH/TrackListAction.h"
#include "G4EMPH/TOPAZLGHitAction.h"
#include "G4EMPH/ARICHHitAction.h"
#include "Simulation/SSDHit.h"
#include "Simulation/Particle.h"
#include "Simulation/Track.h"

// Framework includes
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"

// Geant4 includes
#include "Geant4/G4ParticleTable.hh"
#include "Geant4/G4MaterialPropertiesTable.hh"

namespace emph {

  //____________________________________________________________________________
  // Constructor
  G4Alg::G4Alg(fhicl::ParameterSet const& pset)
    : fG4Help(0)
    , fG4PhysListName     (pset.get< std::string     >("G4PhysListName", "QGSP_BERT"  ) )
    , fEnergyThresh       (pset.get< double          >("G4EnergyThreshold")  )
    , fManyParticles      (pset.get< bool            >("ManyParticles")  )
    , fSparseTrajectories (pset.get< bool            >("SparseTrajectories") )
    , fGenModuleLabel     (pset.get< std::string     >("GenModuleLabel")     )
    , fPlaIndex(0)
    , fShaIndex(0)
    , fSLGhaIndex(0)
    , fSARICHhaIndex(0)
    , fStopActionIndex(0) 
    , fOpticalActionIndex(0) 
      
  {

    /// dummy vector in case user didn't set "AddedUserActions" in fcl file
    std::vector<std::string>  nullstrvec;  // empty vector of strings
    const std::vector<std::string>& psetstrvec = 
      pset.get< std::vector<std::string> >("AddedUserActions",nullstrvec);

    std::cout << "%%%%% USER ACTIONS %%%%%" << std::endl;

    for (size_t j=0; j < psetstrvec.size(); ++j) {
      std::cout << j << " " << psetstrvec[j] << std::endl;
      fUserActions.push_back(psetstrvec[j]); // append user entries to list of actions
    }

    // Constructor decides if initialized value is a path or 
    // an environment variable
    cet::search_path sp("CETPKG_SOURCE");

    sp.find_file(pset.get< std::string >("G4MacroPath"), fG4MacroPath);
    struct stat sb;
    if ( fG4MacroPath.empty() || stat(fG4MacroPath.c_str(), &sb)!=0 ) {
      // failed to resolve the file name
      throw cet::exception("NoG4Macro")
        << "G4 macro file " << fG4MacroPath << " not found!\n"
        << __FILE__ << ":" << __LINE__ << "\n";
    }

    // need to instantiate the G4Helper in order to make the 
    // G4RunManager and pass it the physics list - G4 really dislikes
    // it if you try to make a G4UserRunAction (ie UserActionManager)
    // before passing a physics list to the run manager
    art::ServiceHandle<emph::geo::GeometryService> geo;
    fG4Help = new g4b::G4Helper(fG4MacroPath,
                                fG4PhysListName,
                                geo->Geo()->GDMLFile());

    // more control over GDML processing (before InitPhysics)
    const bool dfltOverlap = false;
    const bool dfltSchema  = true;
    bool overlapCheck       = pset.get< bool >("G4OverlapCheck",dfltOverlap);
    bool validateGDMLSchema = pset.get< bool >("G4ValidateGDMLSchema",dfltSchema);
    if ( dfltOverlap != overlapCheck || dfltSchema != validateGDMLSchema ) {
      mf::LogInfo("G4Alg") << "non-standard GDML processing: "
                           << std::boolalpha
                           << "overlapCheck " << overlapCheck
                           << " [" << dfltOverlap << "] "
                           << "validateGDMLSchema " << validateGDMLSchema
                           << " ]" << dfltSchema << "] "
                           << std::noboolalpha;
    }
    fG4Help->SetOverlapCheck(overlapCheck);
    fG4Help->SetValidateGDMLSchema(validateGDMLSchema);

    // Initialize the helper (physics stage)
    fG4Help->InitPhysics();

    G4Material * steel = G4Material::GetMaterial("Steel");

    if(steel != NULL){
      if(steel->GetMaterialPropertiesTable() == NULL)
        steel->SetMaterialPropertiesTable(new G4MaterialPropertiesTable);
      steel->GetMaterialPropertiesTable()->AddConstProperty("conductor", 1);
    }

    // Geant4 comes with "user hooks" that allows users to perform
    // special tasks at the beginning and end of runs, events, tracks,
    // steps.  By using the UserActionManager, we've separated each
    // set of user tasks into their own class; e.g., there can be one
    // class for processing ssd hits, one class for processing
    // particles, one class for histograms, etc.

    // All UserActions must be adopted after the InitMC call

    // Setup the user actions that we want to use.
    g4b::UserActionManager*  uam = g4b::UserActionManager::Instance();
    
// The ParticleNavigator goes into infinite loop, and we do not need it.. We use something simpler. The particle ancestry is also 
// availabe in the for of the G4Track ancestry.  Which, in our case, is much simpler than for the typical Neutrino experiment
// We do tracking, mostly..     
//    emph::ParticleListAction* pl  = new emph::ParticleListAction(fEnergyThresh,fManyParticles);
//    pl->SetName("emph::ParticleListAction");
//    pl->Config( pset );
    
    emph::TrackListAction* ptl  = new emph::TrackListAction();
    ptl->SetName("emph::TrackListAction");
    ptl->Config( pset );

    emph::SSDHitAction* sh = new emph::SSDHitAction();
    sh->SetName("emph::SSDHitAction");
    sh->Config( pset );
    
    emph::TOPAZLGHitAction* sh2 = new emph::TOPAZLGHitAction();
    sh2->SetName("emph::TOPAZLGHitAction");
    sh2->Config( pset );
    
	 emph::ARICHHitAction* sh3 = new emph::ARICHHitAction();
    sh3->SetName("emph::ARICHHitAction");
    sh3->Config( pset );

    emph::FastStopAction* sh4 = new emph::FastStopAction();
    sh4->SetName("emph::FastStopAction");
    sh4->Config( pset );
    
    emph::OpticalAction* sh5 = new emph::OpticalAction();
    sh5->SetName("emph::OpticalAction");
    sh5->Config( pset );

    // the ParticleListAction must be added to the UserActionManager 
    // first as it has to define the track ID in the case that the 
    // current particle to track is from an EM process that would cause
    // it not to be added to the list and the track ID has to be 
    // reassigned to be the same as its mother.
    //fPlaIndex = uam->GetSize() - 1;
    //if (false) std::cerr << "RWH: fPlaIndex " << fPlaIndex
    //          << " vs " << uam->GetIndex("g4n::ParticleListAction")
    //          << std::endl
    
    uam->AddAndAdoptAction(ptl);
    uam->AddAndAdoptAction(sh);   
    uam->AddAndAdoptAction(sh2);   
    uam->AddAndAdoptAction(sh3);
    uam->AddAndAdoptAction(sh4);
    uam->AddAndAdoptAction(sh5);
    // Should we bother with this.. ??? It seems that it is hardcoded in    
    fPlaIndex = 0; // Again, could change. 
    fShaIndex = 1; // SSD is the 2nd one..   Might change is we add others !  See above.. Very Sneaky.. 
    fSLGhaIndex = 2; // TOPAZLG is the 3rd one..   Yack,.. who knows..  
    fSARICHhaIndex = 3; // ARICH 
    fStopActionIndex = 4;
    fOpticalActionIndex = 5;
    
    ConfigUserActionManager(fUserActions,pset);

    std::cout << "%%%%%%%%%% ACTION LIST %%%%%%%%%%" << std::endl;
    uam->PrintActionList("config");

    // Complete the initialize the helper (actions+finalize stage)
    fG4Help->SetUserAction();

    return;
  }// end of constructor

  //____________________________________________________________________________
  // Destructor
  G4Alg::~G4Alg()  
  {  
    if (fG4Help) delete fG4Help;
  } // end of destructor

  //______________________________________________________________________________
  void G4Alg::ConfigUserActionManager(std::vector<std::string> const& actionList,
                                      fhicl::ParameterSet const& pset)
  {

    /// additional user supplied UserAction classes

    // UserActionManager is a singleton, so get a handle
    g4b::UserActionManager*  uam = g4b::UserActionManager::Instance();

    // The factory is also a singleton
    g4b::UserActionFactory& uafactory = g4b::UserActionFactory::Instance();


    // given a list of classes, ask the factory for a copy and configure
    // it before handing it to the manager

    for (size_t j = 0; j < actionList.size(); ++j ) {

      std::string uaname = actionList[j];  // will use name for finding pset too
      g4b::UserAction* ua = uafactory.GetUserAction(uaname);
    
      if( ua ){
        // factory constructed a UserAction, now configure it
      	// parameter set name can't have colons
        // (ie not namespace qualified class used to create our object)
        // take only the class name portion
        size_t icolon = uaname.find_last_of(":");
        if ( icolon == std::string::npos ) icolon = -1;
        std::string psetname = uaname.substr(icolon+1); // don't include colon
      
        // check if there is a sub-pset with the same name as the
        // (non-namespaced) class name we're trying to configure
        fhicl::ParameterSet uapset;  // for holding results of fetching sub-pset
        bool found = pset.get_if_present< fhicl::ParameterSet >(psetname,uapset);
	
        mf::LogInfo("G4Alg") << "UserAction \"" << uaname << "\" to be Config() with "
                             << ( ( found ) ? "\"" : "G4AlgPSet, couldn't find \"" )
                             << psetname << "\" pset";
        
        // if no named sub-pset available, pass the one G4Alg got
        fhicl::ParameterSet const& pset2pass = ( found ) ? uapset : pset;
      
        // configure using _some_ pset
        ua->Config(pset2pass);
        // add to the manager
        uam->AddAndAdoptAction(ua);
      
      } 
      else {
        // ooops, factory failed ... let someone know
        
        const std::vector<std::string>& availActions = uafactory.AvailableUserActions();
        MF_LOG_VERBATIM("G4Alg") << "emph::G4Alg requested UserAction \"" << uaname << "\" from "
                              << "g4b::UserActionFactory" << std::endl
                              << "but it only knows about: ";
        for(size_t k=0; k<availActions.size(); ++k)
          MF_LOG_VERBATIM("G4Alg") << "   " << availActions[k];
        MF_LOG_VERBATIM("G4Alg") << "proceeding without this action";
      }
    }
  }

  //______________________________________________________________________________
  void G4Alg::RunGeant(std::vector< art::Handle< std::vector<simb::MCTruth> > >& mclists,
                       std::vector<sim::SSDHit> & ssdhitlist,
                       std::vector< sim::Track> & tracklist,
                       std::vector< std::vector< std::pair<size_t, size_t> > >&  pListLimits)
  {  
//    g4b::UserActionManager*  uam = g4b::UserActionManager::Instance();
//    dynamic_cast<emph::ParticleListAction*>(uam->GetAction(fPlaIndex))->ResetAbortFlag();
//    dynamic_cast<ParticleListAction *>(uam->GetAction(fPlaIndex))->ResetTrackIDOffset();
    
    tracklist.clear();
    ssdhitlist.clear();

    // pListLimits keeps track of the index of first particle for a given MCTruth 
    // in the particlelist vector and the index beyond the last particle
    pListLimits .clear();
    pListLimits .resize(mclists.size());

    // Need to process Geant4 simulation for each interaction separately.
    for(size_t mcl = 0; mcl < mclists.size(); ++mcl){
      
      art::Handle< std::vector<simb::MCTruth> > mclistHandle = mclists[mcl];
      
      for(size_t i = 0; i < mclistHandle->size(); ++i){
        const simb::MCTruth* mct(&(*mclistHandle)[i]);
//        
//        size_t start = particlelist.size();
        this->RunGeant(mct, ssdhitlist, tracklist);
//        size_t end = particlelist.size();
// 
// We do not needt to keep incrementing the whole list of particle, we run event by event.. 
//        
//        pListLimits[mcl].push_back(std::pair<size_t, size_t>(start, end));
        
      }// end of loop over interactions in the current handle
    }// end loop over handles
 
    return;
  }// end of RunGeant

  //______________________________________________________________________________
  void G4Alg::RunGeant(std::vector< const simb::MCTruth* >& mctruths,
                       std::vector<sim::SSDHit> & ssdhitlist,
                       std::vector<sim::TOPAZLGHit> & lghitlist,
                       std::vector<sim::ARICHHit> & arichhitlist,
                       std::vector< sim::Track >& tracklist,
                       std::map<int, size_t>& trackIDToMCTruthIndex)
  {
    
    g4b::UserActionManager*  uam = g4b::UserActionManager::Instance();
    //dynamic_cast<emph::ParticleListAction*>(uam->GetAction(fPlaIndex))->ResetAbortFlag();
//    ParticleListAction* pla = dynamic_cast<ParticleListAction *>(uam->GetAction(fPlaIndex));
    TrackListAction* pla = dynamic_cast<TrackListAction *>(uam->GetAction(fPlaIndex));
//    pla->ResetTrackIDOffset();
    // getting instance of particle list action.
    SSDHitAction* sh = dynamic_cast<SSDHitAction *>(uam->GetAction(fShaIndex));
    // getting instance of ssd hit action.
    TOPAZLGHitAction* shLG = dynamic_cast<TOPAZLGHitAction *>(uam->GetAction(fSLGhaIndex));
    ARICHHitAction* shARICH = dynamic_cast<ARICHHitAction *>(uam->GetAction(fSARICHhaIndex));

//    particlelist.clear(); // Why?  We will to a deep copy after the event ran... See few lines below.. 
    tracklist.clear(); // Why?  We will to a deep copy after the event ran... See few lines below.. 
    ssdhitlist.clear();
    lghitlist.clear();
    
    // trackIDToMCTruthIndex keeps track of which trackIDs correspond to which MCTruth
    // in mctruths
    trackIDToMCTruthIndex.clear();
//
//   std::cerr << " G4Alg::RunGeant, before fG4Help->G4Run... " << std::endl;
//   std::cerr << " Size of MCTruth ptrs " << mctruths.size() << std::endl;
   for (std::vector<const simb::MCTruth* >::const_iterator itMcT = mctruths.cbegin();  itMcT != mctruths.cend(); itMcT++) {
       const simb::MCTruth* mctTmp = (*itMcT);
       if (mctTmp->NParticles() != 1) {
         std::cerr << " One incident particle per event, please, fatal..  " << std::endl; exit(2);
       }
       const simb::MCParticle aParticle = mctTmp->GetParticle(0);
//       std::cerr << " Code ... " << aParticle.PdgCode() << std::endl;
//       std::cerr << " Keep going 0921  " << std::endl; 
    }
    fG4Help->G4Run(mctruths);
    
    //trackIDToMCTruthIndex = pla->TrackIDToMCTruthIndexMap();

    tracklist = pla->GetAllTracks();
    // getting track list from particlelistaction.cxx
    ssdhitlist = sh->GetAllHits();
    // getting ssd hit list from ssdhitaction.cxx
    lghitlist = shLG->GetAllHits();
    // getting TOPAZ Lead Glass hit list from topazlghitaction.cxx
    arichhitlist = shARICH->GetAllHits();
    // getting ARICH
    return;
  }// end of RunGeant

  //______________________________________________________________________________

  void G4Alg::RunGeant(art::Ptr<simb::MCTruth>         mctruth,
                       std::vector<sim::SSDHit> & ssdhitlist,
                       std::vector< sim::Track >&   tracklist,
                       int                             trackIDOffset)
  {  
    this->RunGeant(mctruth.get(), ssdhitlist, tracklist, trackIDOffset);
  }

  //______________________________________________________________________________
  void G4Alg::RunGeant(const simb::MCTruth*             mctruth,
                       std::vector<sim::SSDHit>  & ,
                       std::vector< sim::Track >   & ,
                       int                              trackIDOffset)
  {
//    g4b::UserActionManager*  uam = g4b::UserActionManager::Instance();
//    dynamic_cast<emph::ParticleListAction*>(uam->GetAction(fPlaIndex))->ResetAbortFlag();

    MF_LOG_DEBUG("G4Alg") << *mctruth;

    if(trackIDOffset > 0){
//      dynamic_cast<ParticleListAction *>(uam->GetAction(fPlaIndex))->ResetTrackIDOffset(trackIDOffset);
       std::cerr << " G4Alg::RunGeant  trackIDOffset is not use when dealing only with sim::Track list " << std::endl; 
    }

    // The following tells Geant4 to track the particles in this interaction.
    fG4Help->G4Run(mctruth);
    
    return;
  }// end RunGeant for a single simb::MCTruth

  bool G4Alg::IsAborted()
  {
    return false;
//    g4b::UserActionManager*  uam = g4b::UserActionManager::Instance();
//    return dynamic_cast<emph::ParticleListAction*>(uam->GetAction(fPlaIndex))->IsAborted();
  } // Check whether Geant event was aborted
 
} // end namespace
