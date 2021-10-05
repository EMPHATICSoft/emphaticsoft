////////////////////////////////////////////////////////////////////////
/// \brief   Definition of the simulated beam
/// \author  laliaga@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef MCBEAMINFO_H
#define MCBEAMINFO_H

#include <vector>
#include <string>
#include <stdint.h>
#include <iostream>
//#include "TVector3.h"
//#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace sb {
  
  class MCBeamInfo {
  public:
    MCBeamInfo(); // Default constructor
    virtual ~MCBeamInfo() {}; //Destructor
    
    MCBeamInfo( int pdgId,
		CLHEP::Hep3Vector const&       startPosition,
		CLHEP::HepLorentzVector const& startMomentum):
      _pdgId(pdgId),
      _startPosition(startPosition),
      _startMomentum(startMomentum)
    {}
    
    int                            pdgId()         const { return _pdgId; }
    CLHEP::Hep3Vector       const& startPosition() const { return _startPosition;}
    CLHEP::HepLorentzVector const& startMomentum() const { return _startMomentum;}
    
    friend std::ostream& operator << (std::ostream& o, const MCBeamInfo& b);

  private:
    int                     _pdgId;
    CLHEP::Hep3Vector       _startPosition;
    CLHEP::HepLorentzVector _startMomentum;
    
  };
  
}

#endif // MCBEAMINFO_H
