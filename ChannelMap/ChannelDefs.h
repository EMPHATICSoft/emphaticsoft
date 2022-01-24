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

  namespace cmap {
    
    typedef enum tFEBoard {
      V1720    = 0,
      TRB3     = 1,
      AliVATA  = 2,
      SSD      = 3,
      NBoards  = 4
    } FEBoardType;
    
    std::map<FEBoardType, std::string> BoardName = {
      {FEBoardType::V1720, "V1720"},
      {FEBoardType::TRB3, "TRB3"},
      {FEBoardType::AliVATA, "AliVATA"},
      {FEBoardType::SSD, "SSD"}
    };

    std::map<std::string, FEBoardType> BoardId = {
      {"V1720", FEBoardType::V1720},
      {"TRB3", FEBoardType::TRB3},
      {"AliVATA", FEBoardType::AliVATA},
      {"SSD", FEBoardType::SSD}
    };

  }
}

#endif // CHANNELDEFS_H
