//
//  CAENDecoder class converts CAENDigitizer enumerated 
//  codes into human readable text   (W.Badgett)
//

#define TRACE_NAME "CAENDecoder"
#include "artdaq/DAQdata/Globals.hh"

#include "CAENDecoder.hh"
#include "CAENException.hh"

#include <iostream>
#include <sstream>
#include <ctime>

// Following function is not thread safe

const char * emphaticdaq::CAENDecoder::zeit(char delimiter)
{
  time_t t;
  struct tm * hora;
  static char heure[20];
  char format[20];

  sprintf(format,"%%Y.%%m.%%d-%%H%c%%M%c%%S", delimiter, delimiter);
  std::time(&t);
  hora = std::localtime(&t);
  std::strftime(heure,sizeof(heure),format,hora);
  return((char *)heure);
}


void emphaticdaq::CAENDecoder::commError(CAENComm_ErrorCode retcod,
			    const std::string label)
{
  if ( retcod != CAENComm_Success)
  {
    char buffer[256];
    CAENComm_DecodeError(retcod,buffer);
    TLOG_ERROR("CAENDecoder") << zeit() << " " << label 
			      << " error: " << buffer << TLOG_ENDL;
  }
}

void emphaticdaq::CAENDecoder::vmeError(CVErrorCodes      retcod,
			   const std::string label)
{
  if ( retcod != cvSuccess )
  {
    char buffer[256];
    CAENComm_DecodeError(retcod,buffer);
    TLOG_ERROR("CAENDecoder") << zeit() << " " << label 
			      << " error: " << buffer << TLOG_ENDL;
  }
}

void emphaticdaq::CAENDecoder::checkError(CAEN_DGTZ_ErrorCode err, 
			     const std::string label,
			     int boardId)
{
  if ( err != CAEN_DGTZ_Success )
  {
    std::stringstream text;
    emphaticdaq::CAENException e(err, label,boardId);
    TLOG_ERROR("CAENDecoder") << zeit() << " " << e.what() << " ["<< err << "]" << TLOG_ENDL;
    throw(e);
  }
}

