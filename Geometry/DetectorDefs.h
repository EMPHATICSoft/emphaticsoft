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

      static int NChannel(DetectorType t) {
	switch (t) {
	case DetectorType::SSD:
	  return 28;
	case DetectorType::Trigger:
	  return 4;
	case DetectorType::GasCkov:
	  return 3;
	case DetectorType::BACkov:
	  return 6;
	case DetectorType::T0:
	  return 20;
	case DetectorType::RPC:
	  return 16;
	case DetectorType::ARICH:
	  return 576;
	case DetectorType::LGCalo:
	  return 9;
	default:
	  return -1;
	}
      }

    };
  }
}

#endif // DETECTORDEFS_H
