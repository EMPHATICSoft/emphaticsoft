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
    } FEBoard;
    
    std::map<FEBoard, std::string> BoardName = {
      {FEBoard::V1720, "V1720"},
      {FEBoard::TRB3, "TRB3"},
      {FEBoard::AliVATA, "AliVATA"},
      {FEBoard::SSD, "SSD"}
    };

    std::map<std::string, FEBoard> BoardId = {
      {"V1720", FEBoard::V1720},
      {"TRB3", FEBoard::TRB3},
      {"AliVATA", FEBoard::AliVATA},
      {"SSD", FEBoard::SSD}
    };

  }
}

#endif // CHANNELDEFS_H
