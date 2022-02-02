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

    class DetInfo {
    public:
      static std::string Name(DetectorType t) {
	switch (t) {
	case DetectorType::SSD:
	  return std::string("SSD");
	case DetectorType::Trigger:
	  return std::string("Trigger");
	case DetectorType::GasCkov:
	  return std::string("GasCkov");
	case DetectorType::BACkov:
	  return std::string("BACkov");
	case DetectorType::T0:
	  return std::string("T0");
	case DetectorType::RPC:
	  return std::string("RPC");
	case DetectorType::ARICH:
	  return std::string("ARICH");
	case DetectorType::LGCalo:
	  return std::string("LGCalo");
	case DetectorType::NDetectors:
	default:
	  return std::string("Unknown");
	}
      }
      
      static DetectorType Id(std::string d) {
	if (d == "SSD") return DetectorType::SSD;
	if (d == "Trigger") return DetectorType::Trigger;
	if (d == "GasCkov") return DetectorType::GasCkov;
	if (d == "BACkov") return DetectorType::BACkov;
	if (d == "T0") return DetectorType::T0;
	if (d == "RPC") return DetectorType::RPC;
	if (d == "SSD") return DetectorType::SSD;
	if (d == "ARICH") return DetectorType::ARICH;
	if (d == "LGCalo") return DetectorType::LGCalo;
	return DetectorType::NDetectors;
      }
      
    };
  }
}

#endif // DETECTORDEFS_H
