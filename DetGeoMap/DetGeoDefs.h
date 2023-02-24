////////////////////////////////////////////////////////////////////////
/// \brief   Definitions of channel types and constants
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELDEFS_H
#define CHANNELDEFS_H

#include <map>
#include <string>

namespace emph {

  namespace dgmap {
    
    typedef enum tFEBoard {
      V1720    = 0,
      TRB3     = 1,
      AliVATA  = 2,
      SSD      = 3,
      NBoards  = 4
    } FEBoardType;
    
    class Board {
    public:
      static std::string Name(FEBoardType t) {
	switch (t) {
	case FEBoardType::V1720:
	  return std::string("V1720");
	case FEBoardType::TRB3:
	  return std::string("TRB3");
	case FEBoardType::AliVATA:
	  return std::string("AliVATA");
	case FEBoardType::SSD:
	  return std::string("SSD");	  
	case FEBoardType::NBoards:
	default:
	  return std::string("Unknown");
	}
      }
      
      static FEBoardType Id(std::string b) {
	if (b == "V1720") return FEBoardType::V1720;
	if (b == "TRB3") return FEBoardType::TRB3;
	if (b == "AliVATA") return FEBoardType::AliVATA;
	if (b == "SSD") return FEBoardType::SSD;
	return FEBoardType::NBoards;
      }
    };
    
  }
}

#endif // CHANNELDEFS_H
