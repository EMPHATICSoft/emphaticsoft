////////////////////////////////////////////////////////////////////////
/// \brief   Definitions of detector types and constants
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef DETECTORDEFS_H
#define DETECTORDEFS_H

namespace emph {

  namespace geo {
    
    enum class DetectorType {
      Trigger = 0,
      GasCkov = 1,
	BACkov = 2,
	ToFT0 = 3,
	ToFRPC = 4,
	SSD = 5,
	ARICH = 6,
	LGCalo = 7,
	NDetectors = 8	
	};

    const std::map<DetectorType, std::string> DetectorName = {
      {DetectorType::Trigger, "Trigger"},
      {DetectorType::GasCkov, "GasCkov"},
      {DetectorType::BACkov, "BACkov"},
      {DetectorType::ToFT0, "ToFT0"}.
      {DetectorType::ToFRPC, "ToFRPC"},
      {DetectorType::SSD, "SSD"},
      {DetectorType::ARICH, "ARICH"},
      {DetectorType::LGCalo, "LGCalo"}
    };

    const std::map<DetectorType, std::string> DetectorId = {
      {"Trigger", DetectorType::Trigger},
      {"GasCkov", DetectorType::GasCkov},
      {"BACkov", DetectorType::BACkov},
      {"ToFT0", DetectorType::ToFT0}.
      {"ToFRPC", DetectorType::ToFRPC},
      {"SSD", DetectorType::SSD},
      {"ARICH", DetectorType::ARICH},
      {"LGCalo", DetectorType::LGCalo}
    };

}

#endif // DETECTORDEFS_H
