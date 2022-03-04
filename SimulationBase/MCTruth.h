////////////////////////////////////////////////////////////////////////
/// \file  MCTruth.cxx
/// \brief Simple MC truth class, holds a vector of TParticles
///
/// \version $Id: MCTruth.h,v 1.5 2012-10-15 20:36:27 brebel Exp $
/// \author  jpaley@indiana.edu
////////////////////////////////////////////////////////////////////////
#ifndef SIMB_MCTRUTH_H
#define SIMB_MCTRUTH_H

#include <string>
#include <vector>
#include "SimulationBase/MCGeneratorInfo.h"
#include "SimulationBase/MCParticle.h"
#include "SimulationBase/MCBeamInfo.h"

namespace simb {

  /// event origin types
  typedef enum _ev_origin{
    kUnknown,           ///< ???
    kCosmicRay,         ///< Cosmic rays
    kSingleParticle     ///< single particles thrown at the detector
  } Origin_t;

  //......................................................................

  /// Event generator information
  class MCTruth {
  public:
    MCTruth();

  private:

    std::vector<simb::MCParticle> fPartList;    ///< list of particles in this event
    simb::MCBeamInfo              fMCBeamInfo;  ///< reference to beam particle
    simb::Origin_t                fOrigin;      ///< origin for this event
    simb::MCGeneratorInfo         fGenInfo;     ///< information about the generator that produced this event

  public:
    const simb::MCGeneratorInfo&  GeneratorInfo()     const;
    simb::Origin_t                Origin()            const;
    int                           NParticles()        const;
    const simb::MCParticle&       GetParticle(int i)  const;
    const simb::MCBeamInfo&       GetBeam()           const;

    void             Add(simb::MCParticle const& part);
    void             Add(simb::MCParticle&& part);
    void             SetGeneratorInfo(simb::Generator_t generator,
                                      const std::string & genVersion,
                                      const std::unordered_map<std::string, std::string>& genConfig);
    void             SetOrigin(simb::Origin_t origin);
    void             SetBeam(simb::MCBeamInfo& beam) {fMCBeamInfo = beam;}
 
    friend std::ostream&  operator<< (std::ostream& o, simb::MCTruth const& a);
  };
}

inline const simb::MCGeneratorInfo& simb::MCTruth::GeneratorInfo()     const { return fGenInfo;              }
inline simb::Origin_t               simb::MCTruth::Origin()            const { return fOrigin;               }
inline int                          simb::MCTruth::NParticles()        const { return (int)fPartList.size(); }
inline const simb::MCParticle&      simb::MCTruth::GetParticle(int i)  const { return fPartList[i];          }

inline void                         simb::MCTruth::Add(simb::MCParticle const& part) { fPartList.push_back(part); }
inline void                         simb::MCTruth::Add(simb::MCParticle&& part)      { fPartList.push_back(std::move(part)); }
inline void                         simb::MCTruth::SetOrigin(simb::Origin_t origin)  { fOrigin = origin;             }

inline void simb::MCTruth::SetGeneratorInfo(simb::Generator_t generator,
                                            const std::string &genVersion,
                                            const std::unordered_map<std::string, std::string>& genConfig)
{
  fGenInfo = simb::MCGeneratorInfo(generator, genVersion, genConfig);
}

#endif //SIMB_MCTRUTH_H
////////////////////////////////////////////////////////////////////////
