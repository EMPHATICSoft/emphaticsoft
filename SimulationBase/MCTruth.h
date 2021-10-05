////////////////////////////////////////////////////////////////////////
/// \brief MC truth class, holds the information about the event generator information and a vector of GenParticle 
/// \author  laliaga@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef MCTRUTH_H
#define MCTRUTH_H

#include <string>
#include <vector>
#include "SimulationBase/GenParticle.h"
#include "SimulationBase/MCBeamInfo.h"

namespace sb {

  class GenParticle;
  class MCBeamInfo;

  class MCTruth {
  public:
    MCTruth(); // Default constructor
    virtual ~MCTruth() {}; // Destructor

  private:

    std::vector<sb::GenParticle> fPartList;   
    sb::MCBeamInfo               fMCBeamInfo;     

  public:
    const sb::MCBeamInfo&   MCBeamInfo()         const;
    int                     NParticles()       const;
    const sb::GenParticle&  GetParticle(int i) const;
    void                    Add(sb::GenParticle const& part);
    void                    Add(sb::GenParticle&& part);
    void                    SetBeamInfo(int pdgId,
					CLHEP::Hep3Vector const&       startPosition,
					CLHEP::HepLorentzVector const& startMomentum);
    
    friend std::ostream&  operator<< (std::ostream& o, sb::MCTruth const& a);
  };
}

inline const sb::MCBeamInfo&  sb::MCTruth::MCBeamInfo()                     const { return fMCBeamInfo;           }
inline       int              sb::MCTruth::NParticles()                     const { return (int)fPartList.size(); }
inline const sb::GenParticle& sb::MCTruth::GetParticle(int i)               const { return fPartList[i];          }
inline       void             sb::MCTruth::Add(sb::GenParticle const& part)       { fPartList.push_back(part);    }
inline       void             sb::MCTruth::Add(sb::GenParticle&& part)            { fPartList.push_back(std::move(part)); }

inline       void             sb::MCTruth::SetBeamInfo(int pdgId,
						       CLHEP::Hep3Vector const&       startPosition,
						       CLHEP::HepLorentzVector const& startMomentum)
{
  fMCBeamInfo = sb::MCBeamInfo(pdgId, startPosition, startMomentum);
}

#endif //MCTRUTH_H
////////////////////////////////////////////////////////////////////////
