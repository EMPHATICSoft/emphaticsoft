#include "Utility.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void utl::SplitFilename (const std::string& str, std::string &file, std::string &path)
{
	std::cout << "Splitting: " << str << '\n';
	std::size_t found = str.find_last_of("/\\");
	path = str.substr(0,found);
	file = str.substr(found+1);
	std::cout << " path: " << path << '\n';
	std::cout << " file: " << file << '\n';
}

std::string utl::ConvertToString(double x, int prec){
	std::stringstream ss;
	std::string val;

	ss << std::fixed << std::setprecision(prec);
	ss << x;
	ss >> val;

	return val;
}


bool utl::CheckFile(std::string name){
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } 
    else {
        return false;
    }   
}
