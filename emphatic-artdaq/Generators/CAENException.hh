//
//  CAENException.h (W.Badgett)
//
#ifndef _CAEN_EXCEPTION_H
#define _CAEN_EXCEPTION_H

#include "CAENDigitizerType.h"
#include "CAENDecoder.hh"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace emphaticdaq
{
class CAENException: public std::exception
{
  public: 

  CAEN_DGTZ_ErrorCode error; 
  std::string label;
  int boardId;

  CAENException(CAEN_DGTZ_ErrorCode error_, 
		std::string label_,
		int boardId_);

  void print(std::ostream & os = std::cout);
  using std::exception::what;
  std::string what();// override;
};
}

std::ostream& operator<<(std::ostream& s, const emphaticdaq::CAENException& e);
 
#endif
