////////////////////////////////////////////////////////////////////////
/// \brief   Class for truth functions
/// \author  Jon Paley
/// \date    9/19/25
////////////////////////////////////////////////////////////////////////

#include "MCUtils/MCUtils.h"
#include "Simulation/SSDHit.h"

#include <vector>

#include "TVector3.h"
#include "TVectorD.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

namespace mcu {
  
  //----------------------------------------------------------------------
  
  MCUtils::MCUtils()
  {
  }
  
  //------------------------------------------------------------

  double MCUtils::findTruthAngle(std::vector<sim::SSDHit> sim_i, std::vector<sim::SSDHit> sim_f){
     double p_ix=0.; double p_iy=0.; double p_iz=0.;
     double p_fx=0.; double p_fy=0.; double p_fz=0.;

     if (sim_i.empty() || sim_f.empty()) {
       mf::LogError("MCUtils") << "findTruthAngle: Input hit vectors are empty.";
       return 0.0;
     }

     for (auto i : sim_i){
         p_ix += i.GetPx();
         p_iy += i.GetPy();
         p_iz += i.GetPz();
     }
     p_ix /= sim_i.size();
     p_iy /= sim_i.size();
     p_iz /= sim_i.size();

     for (auto f : sim_f){
         p_fx += f.GetPx();
         p_fy += f.GetPy();
         p_fz += f.GetPz();
     }
     p_fx /= sim_f.size();
     p_fy /= sim_f.size();
     p_fz /= sim_f.size();

     TVector3 p_i(p_ix,p_iy,p_iz);
     TVector3 p_f(p_fx,p_fy,p_fz);

     double theta_rad = p_i.Angle(p_f);

     return theta_rad;

  }

  //------------------------------------------------------------

 
} // end namespace mcu
//////////////////////////////////////////////////////////////////////////////
