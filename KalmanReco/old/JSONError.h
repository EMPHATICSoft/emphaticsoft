#ifndef JSONERROR_H
#define JSONERROR_H

#include "jsmn.h"

void CheckJSMNType(jsmntok_t token, jsmntype_t type, const int line, const std::string func, const std::string file );
	
#endif
