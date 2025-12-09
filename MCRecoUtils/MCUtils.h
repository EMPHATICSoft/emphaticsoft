////////////////////////////////////////////////////////////////////////
///// \brief   Class for MC utils for truth matching 
///// \author  Manuel Dall'Olio
///// \date    10/13/25
//////////////////////////////////////////////////////////////////////////

#ifndef MCUTILS_H
#define MCUTILS_H

#include <vector>
#include <iostream>

#include "RecoBase/SSDCluster.h"
#include "Simulation/SSDHit.h"


namespace mcu{

class MCUtils {

    public:
    MCUtils();
    virtual ~MCUtils();
    std::vector<sim::SSDHit>::const_iterator GetTrueSSDHitIt(int station, int plane, int sensor, double strip,const std::vector<sim::SSDHit>& vec);

};


}	
#endif 
