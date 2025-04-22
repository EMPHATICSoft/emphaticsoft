#ifndef UTILITY_H
#define UTILITY_H

#include <string>
namespace utl{
	void SplitFilename (const std::string& str, std::string &file, std::string &path);
	std::string ConvertToString(double x, int prec);
	bool CheckFile(std::string name);
}

#endif
