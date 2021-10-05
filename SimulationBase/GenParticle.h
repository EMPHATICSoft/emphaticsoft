////////////////////////////////////////////////////////////////////////
/// \brief   Definition of generated particle
/// \author  laliaga@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef GENPARTICLE_H
#define GENPARTICLE_H

#include <vector>
#include <string>
#include <stdint.h>
#include <iostream>
//#include "TVector3.h"
//#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace sb {
  
  class GenParticle {
  public:
    GenParticle(); // Default constructor
    virtual ~GenParticle() {}; //Destructor
    
    GenParticle( int pdgId,
		 int parentId,
		 int trackId,
                 CLHEP::Hep3Vector const&       startPosition,
		 CLHEP::Hep3Vector const&       stopPosition,
		 CLHEP::HepLorentzVector const& startMomentum,
		 CLHEP::HepLorentzVector const& stopMomentum,		 
		 std::string startProcess,
		 std::string stopProcess,
		 std::string startVolume,
		 std::string stopVolume, 
                 double time):
      _pdgId(pdgId),
      _parentId(parentId),
      _trackId(trackId),
      _startPosition(startPosition),
      _stopPosition(stopPosition),
      _startMomentum(startMomentum),
      _stopMomentum(stopMomentum),
      _startProcess(startProcess),
      _stopProcess(stopProcess),
      _startVolume(startVolume),
      _stopVolume(stopVolume),
      _time(time)
    {}

    // PDG particle ID code.
    int                            pdgId()         const { return _pdgId; }
    int                            parentId()      const { return _parentId; }
    int                            trackId()       const { return _trackId; }
    CLHEP::Hep3Vector       const& startPosition() const { return _startPosition;}
    CLHEP::Hep3Vector       const& stopPosition()  const { return _stopPosition;}    
    CLHEP::HepLorentzVector const& startMomentum() const { return _startMomentum;}
    CLHEP::HepLorentzVector const& stopMomentum()  const { return _stopMomentum;}    
    std::string                    startProcess()  const { return _startProcess;}
    std::string                    stopProcess()   const { return _stopProcess;}
    std::string                    startVolume()   const { return _startVolume;}
    std::string                    stopVolume()    const { return _stopVolume;}
    double                         time()          const { return _time;}

    friend std::ostream& operator << (std::ostream& o, const GenParticle& p);

  private:
    int                     _pdgId;
    int                     _parentId;
    int                     _trackId;
    CLHEP::Hep3Vector       _startPosition;
    CLHEP::Hep3Vector       _stopPosition;
    CLHEP::HepLorentzVector _startMomentum;
    CLHEP::HepLorentzVector _stopMomentum;
    std::string             _startProcess;
    std::string             _stopProcess;
    std::string             _startVolume;
    std::string             _stopVolume;
    double                  _time;
    
  };
  
}

#endif // GENPARTICLE_H
