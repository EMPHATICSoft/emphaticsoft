/*****************************************************************
* Author:		iain
* Data:			02/16/2018
* Copied from:	https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
* Copied by:	M. Pavin
******************************************************************/
#ifndef INPUTPARSER
#define INPUTPARSER

#include <vector>
#include <string>

class InputParser{
    public:
        InputParser (int &argc, char **argv);
        
        const std::string& getCmdOption(const std::string &option) const;      
        bool cmdOptionExists(const std::string &option) const;
    private:
        std::vector <std::string> tokens;
};

#endif
