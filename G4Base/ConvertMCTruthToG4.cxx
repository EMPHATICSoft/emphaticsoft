////////////////////////////////////////////////////////////////////////
/// \file  ConvertMCTruthToG4.cxx
/// \brief Convert MCTruth to G4Event; Geant4 event generator
///
/// \version $Id: ConvertMCTruthToG4.cxx,v 1.7 2012-09-24 15:19:29 brebel Exp $
/// \author  seligman@nevis.columbia.edu, brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "G4Base/ConvertMCTruthToG4.h"
#include "G4Base/PrimaryParticleInformation.h"
#include "SimulationBase/MCTruth.h"
#include "SimulationBase/MCParticle.h"

#include "Geant4/G4Event.hh"
#include "Geant4/G4IonTable.hh"
#include "Geant4/G4PrimaryVertex.hh"
#include "Geant4/G4ParticleDefinition.hh"
#include "Geant4/G4ParticleTable.hh"

#include <TParticle.h>

#include <CLHEP/Vector/LorentzVector.h>

#include <iostream>
#include <vector>
#include <map>

#include "messagefacility/MessageLogger/MessageLogger.h"

namespace g4b{

  // Static variables used by class.

  // Geant4's particle table.
  G4ParticleTable* ConvertMCTruthToG4::fParticleTable = 0;

  //-----------------------------------------------------
  // Constructor and destructor.
  ConvertMCTruthToG4::ConvertMCTruthToG4():
  fTheEventSequentialNumber(0) 
  {
  }

  //-----------------------------------------------------
  ConvertMCTruthToG4::~ConvertMCTruthToG4() 
  {
    // Print out a list of "unknown" PDG codes we saw in the input.
    if ( ! fUnknownPDG.empty() ){
      std::ostringstream badtxt;
      std::map<G4int, G4int>::iterator i = fUnknownPDG.begin();
      for ( ; i != fUnknownPDG.end(); ++i ){
        int pdg = (*i).first;
        badtxt << "\n   Unknown PDG code = " << pdg
                << ", seen " << (*i).second << " times.";

        const int genieLo = 2000000001;
        const int genieHi = 2000000202;
        if ( pdg >= genieLo && pdg <= genieHi ) {
          badtxt << "  (GENIE specific)";
        }
      }

      mf::LogWarning("ConvertPrimaryToGeant4")
	<< "The following unknown PDG codes were present in the "
        << "simb::MCTruth input.\n"
	<< "They were not processed by Geant4."
        << badtxt.str();
    }

  }

  //-----------------------------------------------------
  void ConvertMCTruthToG4::Reset()
  {
    fConvertList.clear();
  }

  //-----------------------------------------------------
  void ConvertMCTruthToG4::Append( art::Ptr<simb::MCTruth>& mct )
  {
    this->Append( mct.get() );
  }

  //-----------------------------------------------------
  void ConvertMCTruthToG4::Append( const simb::MCTruth* mct )
  {
    fConvertList.push_back( mct );
  }

  //-----------------------------------------------------
  void ConvertMCTruthToG4::GeneratePrimaries( G4Event* event )
  {
    fTheEventSequentialNumber++;
    event->SetEventID(fTheEventSequentialNumber);
    // Take the contents of MCTruth objects and use them to
    // initialize the G4Event.
  
    // A G4Event organizes its particles in terms of "vertices" and
    // "particles", like HepMC.  Unfortunately, ROOT doesn't use
    // HepMC, so the MCTruth objects aren't organized that way.
    // For most of the work that we'll ever do, there'll be only one
    // vertex in the event.  However, just in case there are multiple
    // vertices (e.g., overlays, double vertex studies) I want the
    // code to function properly.
  
    // So create a map of particle positions and associated
    // G4PrimaryVertex*.  Note that the map must use CLHEP's
    // LorentzVector, and not ROOT's, since ROOT does not define an
    // operator< for its physics vectors.
    std::map< CLHEP::HepLorentzVector, G4PrimaryVertex* >                  vertexMap;
    std::map< CLHEP::HepLorentzVector, G4PrimaryVertex* >::const_iterator  vi; 

    // For each MCTruth (probably only one, but you never know):
    // index keeps track of which MCTruth object you are using
    size_t index = 0;
    for( auto const* mct : fConvertList){
    
      // For each simb::MCParticle in the MCTruth:
      for ( int p = 0; p != mct->NParticles(); ++p ){
	// Implementation note: the following statement copies the
	// MCParticle from the MCTruth, instead of getting a
	// const reference.    
	simb::MCParticle const& particle = mct->GetParticle(p);

	// status code == 1 means "track this particle."  Any
	// other status code should be ignored by the Monte Carlo.
	if ( particle.StatusCode() != 1 ) continue;
      
	// Get the Particle Data Group code for the particle.
	G4int pdgCode = particle.PdgCode();
      
	// Get the vertex.  Note that LArSoft/ROOT uses cm, but
	// Geant4/CLHEP uses mm.
	G4double x = particle.Vx() * CLHEP::cm;
	G4double y = particle.Vy() * CLHEP::cm;
	G4double z = particle.Vz() * CLHEP::cm;
	G4double t = particle.T()  * CLHEP::ns;
      
	// Create a CLHEP four-vector from the particle's vertex.
	CLHEP::HepLorentzVector fourpos(x,y,z,t);
      
	// Is this vertex already in our map?
	G4PrimaryVertex* vertex = 0;
	std::map< CLHEP::HepLorentzVector, G4PrimaryVertex* >::const_iterator result = vertexMap.find( fourpos );
	if ( result == vertexMap.end() ){
	  // No, it's not, so create a new vertex and add it to the
	  // map.
	  vertex = new G4PrimaryVertex(x, y, z, t);
	  vertexMap[ fourpos ] = vertex;

	  // Add the vertex to the G4Event.
	  event->AddPrimaryVertex( vertex );
	}
	else{
	  // Yes, it is, so use the existing vertex.
	  vertex = (*result).second;
	}
      
	// Get additional particle information.
	TLorentzVector momentum = particle.Momentum(); // (px,py,pz,E)
	TVector3 polarization = particle.Polarization();
      
	// Get the particle table if necessary.  (Note: we're
	// doing this "late" because I'm not sure at what point
	// the G4 particle table is initialized in the loading process.
	if ( fParticleTable == 0 ){
	  fParticleTable = G4ParticleTable::GetParticleTable();
	}

	// Get Geant4's definition of the particle.
	G4ParticleDefinition* particleDefinition;
      
	if(pdgCode==0){
	  particleDefinition = fParticleTable->FindParticle("geantino");
//	  particleDefinition = fParticleTable->FindParticle("opticalphoton");  // no longer v4.7 .. 
	}
	else
	  particleDefinition = fParticleTable->FindParticle(pdgCode);

	if ( pdgCode > 1000000000) { // If the particle is a nucleus
	  mf::LogDebug("ConvertPrimaryToGeant4") << ": %%% Nuclear PDG code = " << pdgCode
					      << " (x,y,z,t)=(" << x
					      << "," << y
					      << "," << z
					      << "," << t << ")"
					      << " P=" << momentum.P()
					      << ", E=" << momentum.E();
	  // If the particle table doesn't have a definition yet, ask the ion
	  // table for one. This will create a new ion definition as needed.
	  if (!particleDefinition) {
            int Z = (pdgCode % 10000000) / 10000; // atomic number
            int A = (pdgCode % 10000) / 10; // mass number
            particleDefinition = fParticleTable->GetIonTable()->GetIon(Z, A, 0.);
          }
        }

	// What if the PDG code is unknown?  This has been a known
	// issue with GENIE.
	if ( particleDefinition == 0 ){
	  mf::LogDebug("ConvertPrimaryToGeant4") << ": %%% Code not found = " << pdgCode;
	  fUnknownPDG[ pdgCode ] += 1;
	  continue;
	}
      
	// Create a Geant4 particle to add to the vertex.
	G4PrimaryParticle* g4particle = new G4PrimaryParticle( particleDefinition,
							       momentum.Px() * CLHEP::GeV,
							       momentum.Py() * CLHEP::GeV,
							       momentum.Pz() * CLHEP::GeV);

	// Add more particle information the Geant4 particle.
	G4double charge = particleDefinition->GetPDGCharge();
	g4particle->SetCharge( charge );
	g4particle->SetPolarization( polarization.x(),
				     polarization.y(),
				     polarization.z() );
      
	// Add the particle to the vertex.
	vertex->SetPrimary( g4particle );

	// Create a PrimaryParticleInformation object, and save
	// the MCTruth pointer in it.  This will allow the
	// ParticleActionList class to access MCTruth
	// information during Geant4's tracking.
	PrimaryParticleInformation* primaryParticleInfo = new PrimaryParticleInformation;
	primaryParticleInfo->SetMCTruth( mct, index, p );
	  
	// Save the PrimaryParticleInformation in the
	// G4PrimaryParticle for access during tracking.
	g4particle->SetUserInformation( primaryParticleInfo );

	mf::LogDebug("ConvertPrimaryToGeant4") << ": %%% primary PDG=" << pdgCode
					    << ", (x,y,z,t)=(" << x
					    << "," << y
					    << "," << z
					    << "," << t << ")"
					    << " P=" << momentum.P()
					    << ", E=" << momentum.E();

      } // for each particle in MCTruth
      ++index;
    } // for each MCTruth
  }// GeneratePrimaries
}// namespace
