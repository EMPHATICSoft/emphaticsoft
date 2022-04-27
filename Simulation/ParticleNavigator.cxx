////////////////////////////////////////////////////////////////////////
/// \brief   Particle list in DetSim contains Monte Carlo particle information.
///
/// \author  seligman@nevis.columbia.edu
/// \date
////////////////////////////////////////////////////////////////////////
///
/// Although there's nothing in the following class that assumes
/// units, the standard for NOvASoft is that distances are in cm, and
/// energies are in GeV.
////////////////////////////////////////////////////////////////////////
#include "Simulation/ParticleNavigator.h"

#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>

#include "TLorentzVector.h"

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"

#include "Simulation/EveIdCalculator.h"

namespace sim 
{

  // Static variable for eve ID calculator. We're using an unique_ptr,
  // so when this object is eventually deleted (at the end of the job)
  // it will delete the underlying pointer.
  static std::unique_ptr<EveIdCalculator> eveIdCalculator;

  //-------------------------------------------------------------
  // Constructor.
  ParticleNavigator::ParticleNavigator() 
  {
  }  

  //-------------------------------------------------------------
  ParticleNavigator::ParticleNavigator(const std::vector<sim::Particle>& parts)
  {
    for(size_t p = 0; p < parts.size(); ++p) Add(new sim::Particle(parts[p]));
  }

  //-------------------------------------------------------------
  // Destructor 
  ParticleNavigator::~ParticleNavigator()
  {
    this->clear();
  }
  
  //-------------------------------------------------------------
  // Copy constructor.  Note that since this class inherits from
  // TObject, we have to copy its information explicitly.
  ParticleNavigator::ParticleNavigator( const ParticleNavigator& rhs ) 
  {
    // Clear any contents that we already possess.
    this->clear();
    
    // Copy each entry in the other ParticleNavigator.
    for ( const_iterator entry = rhs.fParticleList.begin();
	  entry != rhs.fParticleList.end(); ++entry ){
      const sim::Particle* original = (*entry).second;
      sim::Particle* copy = new sim::Particle( *original );
      this->insert( copy );
    }
  }
  
  //-------------------------------------------------------------
  // Assignment constructor.
  ParticleNavigator& ParticleNavigator::operator=( const ParticleNavigator& rhs )
  {
    // Usual test for self-assignment.
    if ( this == &rhs ) return *this;
    
    // Clear any contents that we already possess.
    this->clear();
    
    // Copy each entry in the other ParticleNavigator.
    for ( const_iterator entry = rhs.fParticleList.begin();
	  entry != rhs.fParticleList.end(); ++entry ){
      const sim::Particle* original = (*entry).second;
      sim::Particle* copy = new sim::Particle( *original );
      this->insert( copy );
    }
    
    return *this;
  }
  
  //-------------------------------------------------------------
  // Apply an energy cut to the particles.
  void ParticleNavigator::Cut( const double& cut )
  {
    // The safest way to do this is to create a list of track IDs that
    // fail the cut, then delete those IDs.
    
    // Define a list of IDs.
    typedef std::set< key_type > keyList_type;
    keyList_type keyList;
    
    // Add each ID that fails the cut to the list.
    for ( const_iterator i = fParticleList.begin(); i != fParticleList.end(); ++i ){
      const sim::Particle* particle = (*i).second;
      Double_t totalInitialEnergy = particle->E();
      if ( totalInitialEnergy < cut ) { 
	keyList.insert( (*i).first ); 
      }
    }
    
    // Go through the list, deleting the particles that are on the list.
    for ( keyList_type::const_iterator i = keyList.begin(); i != keyList.end(); ++i ){
      this->erase( *i );
    }
  }
  
  //-------------------------------------------------
  const ParticleNavigator::key_type& ParticleNavigator::TrackId( const size_type index ) const
  {
    const_iterator i = fParticleList.begin();
    std::advance(i,index);
    return (*i).first;
  }
  
  //-------------------------------------------------
  ParticleNavigator::mapped_type ParticleNavigator::Particle( const size_type index ) const
  {
    const_iterator i = fParticleList.begin();
    std::advance(i,index);
    return (*i).second;
  }
  
  //-------------------------------------------------
  ParticleNavigator::mapped_type ParticleNavigator::Particle( const size_type index )
  {
    iterator i = fParticleList.begin();
    std::advance(i,index);
    return (*i).second;
  }
  
  //-------------------------------------------------
  bool ParticleNavigator::IsPrimary( int trackID ) const
  {
    return fPrimaries.find( trackID )  !=  fPrimaries.end();
  }
  
  //-------------------------------------------------
  int ParticleNavigator::NumberOfPrimaries() const
  {
    return fPrimaries.size();
  }
  
  //-------------------------------------------------
  const sim::Particle* ParticleNavigator::Primary( const int index ) const
  {
    // Advance "index" entries from the beginning of the primary list.
    primaries_const_iterator primary = fPrimaries.begin();
    std::advance( primary, index );
    
    // Get the track ID from that entry in the list.
    int trackID = *primary;
    
    // Find the entry in the particle list with that track ID.
    const_iterator entry = fParticleList.find(trackID);

    // Return the Particle object in that entry.
    return (*entry).second;
  }

  //-------------------------------------------------
  sim::Particle* ParticleNavigator::Primary( const int index )
  {
    // Advance "index" entries from the beginning of the primary list.
    primaries_const_iterator primary = fPrimaries.begin();
    std::advance( primary, index );

    // Get the track ID from that entry in the list.
    int trackID = *primary;

    // Find the entry in the particle list with that track ID.
    iterator entry = fParticleList.find(trackID);

    // Return the Particle object in that entry.
    return (*entry).second;
  }

  //-------------------------------------------------
  ParticleNavigator ParticleNavigator::Add( const int& offset ) const
  {
    // Start with a fresh ParticleNavigator, the destination of the
    // particles with adjusted track numbers.
    ParticleNavigator result;

    // For each particle in our list:
    for ( const_iterator i = fParticleList.begin(); i != fParticleList.end(); ++i ){
      const sim::Particle* particle = (*i).second;
      
      // Create a new particle with an adjusted track ID.
      sim::Particle* adjusted = new sim::Particle( particle->TrackId() + offset,
						   particle->PdgCode(),
						   particle->Process(),
						   particle->Mother(),
						   particle->Mass() );
      
      adjusted->SetPolarization( particle->Polarization() );
      
      // Copy all the daughters, adjusting the track ID.
      for ( int d = 0; d < particle->NumberDaughters(); ++d ){
	int daughterID = particle->Daughter(d);
	adjusted->AddDaughter( daughterID + offset );
      }
      
      // Copy the trajectory points.
      for ( size_t t = 0; t < particle->NumberTrajectoryPoints(); ++t ){
	adjusted->AddTrajectoryPoint( particle->Position(t), particle->Momentum(t) );
      }
      
      // Add the adjusted particle to the destination particle list.
      // This will also adjust the destination's list of primary
      // particles, if needed.
      result.insert( adjusted );
    }
    
    return result;
  }
  
  //-------------------------------------------------------------
  // Just in case: define the result of "scalar * ParticleNavigator" to be
  // the same as "ParticleNavigator * scalar".
  ParticleNavigator operator+(const int& value, const ParticleNavigator& list) 
  {
    return list + value;
  }


  // This is the main "insertion" method for the ParticleNavigator
  // pseudo-array pseudo-map.  It does the following:
  //  - Add the Particle to the list; if the track ID is already in the
  //    list, throw an exception.
  //  - If it's a primary particle, add it to the list of primaries.
  void ParticleNavigator::insert( sim::Particle* particle ) 
  { 
    int trackID = particle->TrackId();
    iterator insertion = fParticleList.lower_bound( trackID );
    if ( insertion == fParticleList.end() ){
      // The best "hint" we can give is that the particle will go at
      // the end of the list.
      fParticleList.insert( insertion, value_type( trackID, particle ) );
    }
    else if ( ( (*insertion).first == trackID )) { //  && trackID != 1) 
      throw cet::exception("ParticleNavigator") << "sim::ParticleNavigator::insert - ERROR - "
						<< "track ID=" << trackID 
						<< " is already in the list";
    }
    else{
      //      if (trackID > 1)
	// It turns out that the best hint we can give is one more
	// than the result of lower_bound.
	fParticleList.insert( ++insertion, value_type( trackID, particle ) );
    }

    // If this is a primary particle, add it to the list. Look to see
    // if the process name contains the string rimary - leave the 
    // "p" off cause it might be capitalized, but then again maybe not
    if ( particle->Process().find("rimary") != std::string::npos )
      fPrimaries.insert( trackID );
  }

  //-------------------------------------------------------------
  void ParticleNavigator::clear()
  {
    for ( iterator i = fParticleList.begin(); i != fParticleList.end(); ++i ){
      delete (*i).second;
    }

    fParticleList.clear();
    fPrimaries.clear();

    eveIdCalculator.reset();

    return;
  }

  //-------------------------------------------------------------
  // An erase that includes the deletion of the associated Particle*.
  ParticleNavigator::size_type ParticleNavigator::erase( const key_type& key )
  {
    iterator entry = fParticleList.find( key );
    delete (*entry).second;
    return fParticleList.erase( key );
  }

  //-------------------------------------------------------------
  std::ostream& operator<< ( std::ostream& output, const ParticleNavigator& list )
  {
    // Determine a field width for the particle number.
    ParticleNavigator::size_type numberOfParticles = list.size();
    int numberOfDigits = (int) std::log10( (double) numberOfParticles ) + 1;

    // A simple header.
    output.width( numberOfDigits );
    output << "#" << ": < ID, particle >" << std::endl; 

    // Write each particle on a separate line.
    ParticleNavigator::size_type nParticle = 0;
    for ( ParticleNavigator::const_iterator particle = list.begin(); 
	  particle != list.end(); ++particle, ++nParticle ){
      output.width( numberOfDigits );
      output << nParticle << ": " 
	     << "<" << (*particle).first 
	     << "," << *((*particle).second) 
	     << ">" << std::endl;
    }

    return output;
  }

  //-------------------------------------------------------------
  // operator[] in a non-const context.
  ParticleNavigator::mapped_type ParticleNavigator::operator[]( const key_type& key)
  {
    const_iterator i = fParticleList.find(key);
    if(i == fParticleList.end())
      throw cet::exception("ParticleNavigator") << "track id is not in map";
    return (*i).second;
  }

  //-------------------------------------------------------------
  // operator[] in a const context; not usual for maps, but I find it useful.
  ParticleNavigator::mapped_type ParticleNavigator::operator[]( const key_type& key) const
  {
    const_iterator i = fParticleList.find(key);
    if(i == fParticleList.end())
      throw cet::exception("ParticleNavigator") << "track id is not in map";
    return (*i).second;
  }


  //-------------------------------------------------------------
  // The eve ID calculation.
  int ParticleNavigator::EveId( const int trackID ) const
  {
    // If the eve ID calculator has never been initialized, use the
    // default method.
    if ( eveIdCalculator.get() == 0 ){
      AdoptEveIdCalculator( new EveIdCalculator );
    }

    // If the eve ID calculator has changed, or we're looking at a
    // different ParticleNavigator, initialize the calculator.
    static EveIdCalculator* saveEveIdCalculator = 0;
    if ( saveEveIdCalculator != eveIdCalculator.get() ) {
      saveEveIdCalculator = eveIdCalculator.get();
      eveIdCalculator->Init( this );
    }
    if ( eveIdCalculator->ParticleNavigator() != this ){
      eveIdCalculator->Init( this );
    }
    
    // After the "bookkeeping" tests, here's where we actually do the
    // calculation.
    return eveIdCalculator->CalculateEveId( trackID );
  }

  //-------------------------------------------------------------
  // Save a new eve ID calculation method.
  void ParticleNavigator::AdoptEveIdCalculator( EveIdCalculator* calc )
  {
    eveIdCalculator.reset(calc);
  }

} // end namespace sim
////////////////////////////////////////////////////////////////////////
