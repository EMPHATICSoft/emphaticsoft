#include <iostream>
#include <stdlib.h>
#include <string>
#include "JSONError.h"

std::string jsmnNames[5] = {"undefined", "object", "array", "string", "primitive"};
void CheckJSMNType(jsmntok_t token, jsmntype_t type, const int line, const std::string func, const std::string file ){
	if(token.type != type){
		std::cerr << "***********************************************************" << std::endl;
		std::cerr << "Error in " << func << ", line " << line << " in " << file << std::endl;
		std::cerr << "Expected json " << jsmnNames[token.type] << ", got json " << jsmnNames[type] << std::endl;
		std::cerr << "Failed to parse JSON file. Exiting..." << std::endl;
		std::cerr << "***********************************************************" << std::endl;
		exit(EXIT_FAILURE);	
	}
}
