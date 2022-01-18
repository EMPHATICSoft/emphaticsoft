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
      ToFT0 = 3,
      ToFRPC = 4,
      SSD = 5,
      ARICH = 6,
      LGCalo = 7,
      NDetectors = 8	
    } DetectorType;
    
    std::map<DetectorType, std::string> DetectorName = {
      {DetectorType::Trigger, "Trigger"},
      {DetectorType::GasCkov, "GasCkov"},
      {DetectorType::BACkov, "BACkov"},
      {DetectorType::ToFT0, "ToFT0"},
      {DetectorType::ToFRPC, "ToFRPC"},
      {DetectorType::SSD, "SSD"},
      {DetectorType::ARICH, "ARICH"},
      {DetectorType::LGCalo, "LGCalo"}
    };

    std::map<std::string, DetectorType> DetectorId = {
      {"Trigger", DetectorType::Trigger},
      {"GasCkov", DetectorType::GasCkov},
      {"BACkov", DetectorType::BACkov},
      {"ToFT0", DetectorType::ToFT0},
      {"ToFRPC", DetectorType::ToFRPC},
      {"SSD", DetectorType::SSD},
      {"ARICH", DetectorType::ARICH},
      {"LGCalo", DetectorType::LGCalo}
    };

  }
}

#endif // DETECTORDEFS_H
