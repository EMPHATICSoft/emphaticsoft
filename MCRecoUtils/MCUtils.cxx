#include "MCUtils.h"
#include "Simulation/SSDHit.h"
#include <iostream>

using namespace mcu;

namespace mcu{

  MCUtils::MCUtils() {}

  MCUtils::~MCUtils() {}

   std::vector<sim::SSDHit>::const_iterator MCUtils::GetTrueSSDHitIt(
    int station, int plane, int sensor, double strip,
    const std::vector<sim::SSDHit>& vec)
{
    return std::find_if(vec.begin(), vec.end(), [&](const auto& h) {
        return h.GetStation() == station &&
               h.GetPlane()   == plane &&
               h.GetSensor()  == sensor &&
               h.GetStrip()   == strip;
    });
}

}

