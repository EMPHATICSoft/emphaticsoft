////////////////////////////////////////////////////////////////////////
/// \brief   A module for propagating particles through Geant4 and
///          generating energy depositions
/// \author  messier@indiana.edu brebel@fnal.gov rhatcher@fnal.gov jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////


// EMPHATIC includes 
#include "G4EMPH/G4Alg.h"
#include "G4EMPH/ParticleListAction.h"
#include "Simulation/SSDHit.h"
#include "Simulation/SSDHitAlgo1.h"
#include "Simulation/TOPAZLGHit.h"
#include "Simulation/ARICHHit.h"
#include "Simulation/Particle.h"
#include "Simulation/Track.h"
#include "Simulation/Simulation.h"
#include "SimulationBase/MCTruth.h"
#include "ArtUtils/AssociationUtil.h"

#include <cassert>
#include <cstdlib>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <unistd.h>

// ROOT include
#include <TDatabasePDG.h>
#include <TSystem.h>
#include <TStopwatch.h>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "Geant4/Randomize.hh"

namespace emph {

  /// A module to pass interaction information to Geant4 and 
  /// create hits and particle lists
  class G4Gen : public art::EDProducer {
  public:
    explicit G4Gen(fhicl::ParameterSet const &pset);
    virtual ~G4Gen();                        

    void produce (art::Event& evt);
    void beginRun(art::Run& run);
    void endRun(art::Run& run);

  private:
    std::string         fGeneratorLabel;

    emph::G4Alg*         fG4Alg;     ///< G4Helper object
    fhicl::ParameterSet fG4AlgPSet; ///< parameter set to configure the G4Alg object
    TStopwatch          fStopwatch; ///< keep track of how long it takes to run the job

  };
}

namespace emph {

  //___________________________________________________________________________
  // Constructor
  G4Gen::G4Gen(fhicl::ParameterSet const& pset)
  : EDProducer(pset)
  , fG4Alg(0)
  , fG4AlgPSet(pset.get< fhicl::ParameterSet >("G4AlgPSet") )
  {
  
    fGeneratorLabel = fG4AlgPSet.get<std::string>("GenModuleLabel");

    // get the random number seed, use a random default if not specified
    // in the configuration file. 
    unsigned int seed = pset.get< unsigned int >("Seed", sim::GetRandomNumberSeed());
    // setup the random number service for Geant4, the "G4Engine" label is a 
    // special tag setting up a global engine for use by Geant4/CLHEP
    std::cerr << " G4Gen::G4Gen, the random number seed is " << seed << std::endl;

    createEngine(seed, "G4Engine");
    
     std::cerr << " A uniform throw... " << G4UniformRand() << std::endl;
   //
    std::cerr << " G4Gen::G4Gen Constructor.. after create engine " << std::endl;
    // Start counting the time
    fStopwatch.Start();

    produces< std::vector<sim::SSDHit> > ();
//    produces< std::vector<sim::SSDHitAlgo1> > ();
    produces< std::vector<sim::TOPAZLGHit> > ();
    produces< std::vector<sim::ARICHHit> > ();
    produces< std::vector<sim::Particle>     	         >();
    produces< std::vector<sim::Track>     	         >();
    //    produces< art::Assns<sim::Particle, simb::MCTruth>   >();

  }// end of constructor

  //___________________________________________________________________________
  // Destructor
  G4Gen::~G4Gen()  
  {  
    fStopwatch.Stop();
    mf::LogVerbatim("G4Gen") << "real time to run through Geant4: " 
                             << fStopwatch.RealTime();
    if(fG4Alg) delete fG4Alg;
  }// end of destructor

  //___________________________________________________________________________
  void G4Gen::beginRun(art::Run& ) //run) 
  {
    
    std::cerr << " G4Gen::begin run ..before instantiate g4alg " << std::endl;
    fG4Alg = new G4Alg(fG4AlgPSet);
    
    return;
  }// end of begin job

  //___________________________________________________________________________
  void G4Gen::endRun(art::Run& ) 
  {
    if (fG4Alg) delete fG4Alg;
    fG4Alg = 0;
  }// end of endRun

  //___________________________________________________________________________
  void G4Gen::produce(art::Event& evt)  
  {
    // need to instantiate the G4Helper in order to make the 
    // G4RunManager and pass it the physics list - G4 really dislikes
    // it if you try to make a G4UserRunAction (ie UserActionManager)
    // before passing a physics list to the run manager
  
    // Define the SSDHit and Particle vectors.
    std::unique_ptr<std::vector<sim::SSDHit> >  ssdhlcol(new std::vector<sim::SSDHit>  );
//    std::unique_ptr<std::vector<sim::SSDHitAlgo1> >  ssdhlcol(new std::vector<sim::SSDHitAlgo1>  );
    std::unique_ptr<std::vector<sim::TOPAZLGHit> >  lghlcol(new std::vector<sim::TOPAZLGHit>  );
    std::unique_ptr<std::vector<sim::ARICHHit> >  arichhlcol(new std::vector<sim::ARICHHit>  );
    std::unique_ptr<std::vector<sim::Particle>     >            pcal    (new std::vector<sim::Particle>    );
    std::unique_ptr<std::vector<sim::Track>     >            pcol    (new std::vector<sim::Track>    );
    //    std::unique_ptr< art::Assns<sim::Particle, simb::MCTruth> > tpassn  (new art::Assns<sim::Particle, simb::MCTruth>);
    
    // get beam particle
    art::Handle<std::vector<simb::MCParticle>> beam;
    evt.getByLabel(fGeneratorLabel, beam);
    
    // make sure there is only one beam particle
    assert(beam->size() == 1);
    simb::MCParticle b = beam->at(0);
    //
    // now create MCTruth
    simb::MCTruth mctru;
    mctru.SetBeam(b);

    /*
    auto beamPos = b.Position();
    std::cout << "%%%%% Beam Position = (" << beamPos[0] << "," 
	      << beamPos[1] << "," << beamPos[2] << ")" << std::endl;
    */

    // the next steps are a little clunky.  Make a vector of art::Ptr<MCTruth> 
    // objects to use when making the associations and another of just
    // const* MCTruth objects to pass to G4Alg
    //    std::vector< art::Ptr< simb::MCTruth > > mctp;
    std::vector< const simb::MCTruth* > mct;

    //    art::Ptr<simb::MCTruth> ptr(&mctru);

    //    for(size_t i = 0; i < mclist->size(); ++i){
    //      art::Ptr<simb::MCTruth> ptr(mclist, i);
    //    mctp.push_back(ptr);
    mct.push_back(&mctru); // ptr.get());
      //    }

    // make a map to keep track of which G4 track ID goes with which MCTruth object
    std::map<int, size_t> trackIDToMCTruthIndex;
//    std::cout << "******************** HERE 1 ********************" 
//	      << std::endl;
//    fG4Alg->RunGeant(mct, *ssdhlcol, *pcol, trackIDToMCTruthIndex);
    fG4Alg->RunGeant(mct, *ssdhlcol, *lghlcol,  *arichhlcol, *pcol, *pcal, trackIDToMCTruthIndex); // trackIDToMCTruthIndex will be left empty... Could be cleaned up.. 
//    std::cout << "******************** HERE 2 ********************" 
//	      << std::endl;
/*
    if (evt.id().event() < 10 ) { 
      std::cerr << "####################### The Particle List Action Size: " << pcal->size() << std::endl;
      std::cerr << "####################### The Track List Action Size: " << pcol->size() << std::endl;
      std::cerr << "####################### The SSD Hit List Size: " << ssdhlcol->size() << std::endl;
      std::cerr << "####################### The TOPAZLG Hit List Size: " << lghlcol->size() << std::endl;
      std::cerr << "####################### The ARICH Hit List Size: " << arichhlcol->size() << std::endl;
    }
*/

    // print the number of particles and ssd hits!

    // make associations for the particles and MCTruth objects
    //    int    trackID = INT_MAX;
    //    size_t mctidx  = 0;
    /*
    for(size_t p = 0; p < pcol->size(); ++p){

      sim::Particle &part = (*pcol)[p];
      trackID = part.TrackId();
      
      if( trackIDToMCTruthIndex.count(trackID) > 0){
        mctidx = trackIDToMCTruthIndex.find(trackID)->second;
	//        util::CreateAssn(evt, *pcol, mctp[mctidx], *tpassn, p);
      }
      else
	throw cet::exception("G4Gen") << "Cannot find MCTruth for Track Id: "
				      << trackID
				      << " to create association between Particle and MCTruth";
      
    }// end loop over handles
    */
    
    // Put the data products into the event
    evt.put(std::move(ssdhlcol));
    evt.put(std::move(lghlcol));
    evt.put(std::move(arichhlcol));
    evt.put(std::move(pcal)); 
    evt.put(std::move(pcol));
    //    evt.put(std::move(tpassn));
    
    return;
  }// end of produce
  
} // end namespace

namespace emph { DEFINE_ART_MODULE(G4Gen) }
