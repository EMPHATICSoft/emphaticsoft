////////////////////////////////////////////////////////////////////////
/// \brief   Definitions of channel types and constants
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELDEFS_H
#define CHANNELDEFS_H

namespace emph {

  namespace cmap {
    
    enum class FEBoard {
      V1720    = 0,
	TRB3     = 1,
	AliVATA  = 2,
	SSD      = 3,
	NBoards  = 4
	};

    const std::map<FEBoard, std::string> BoardName = {
      {FEBoard::V1720, "V1720"},
      {FEBoard::TRB3, "TRB3"},
      {FEBoard::AliVATA, "AliVATA"},
      {FEBoard::SSD, "SSD"}
    };

    const std::map<std::string, FEBoard> BoardId = {
      {"V1720", FEBoard::V1720},
      {"TRB3", FEBoard::TRB3},
      {"AliVATA", FEBoard::AliVATA},
      {"SSD", FEBoard::SSD}
    };

  
}

#endif // CHANNELDEFS_H
