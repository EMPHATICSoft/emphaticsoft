////////////////////////////////////////////////////////////////////////
/// \brief   Definitions of detector types and constants
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef DETECTORDEFS_H
#define DETECTORDEFS_H

#include <map>
#include <string>

namespace emph {

  namespace geo {
    
    typedef enum tDetectorType {
      Trigger = 0,
      GasCkov = 1,
      BACkov = 2,
      T0 = 3,
      RPC = 4,
      SSD = 5,
      ARICH = 6,
      LGCalo = 7,
      NDetectors = 8	
    } DetectorType;
    
    std::map<DetectorType, std::string> DetectorName = {
      {DetectorType::Trigger, "Trigger"},
      {DetectorType::GasCkov, "GasCkov"},
      {DetectorType::BACkov, "BACkov"},
      {DetectorType::T0, "T0"},
      {DetectorType::RPC, "RPC"},
      {DetectorType::SSD, "SSD"},
      {DetectorType::ARICH, "ARICH"},
      {DetectorType::LGCalo, "LGCalo"}
    };

    std::map<std::string, DetectorType> DetectorId = {
      {"Trigger", DetectorType::Trigger},
      {"GasCkov", DetectorType::GasCkov},
      {"BACkov", DetectorType::BACkov},
      {"T0", DetectorType::T0},
      {"RPC", DetectorType::RPC},
      {"SSD", DetectorType::SSD},
      {"ARICH", DetectorType::ARICH},
      {"LGCalo", DetectorType::LGCalo}
    };

  }
}

#endif // DETECTORDEFS_H
