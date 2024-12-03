/*  L++ LppExcept
 *  Created by Leo Bellantoni on 12/5/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  A simple exception class, thrown throughout L++.  Most of these
 *  conditions are conceptually some form of std::runtime_error, but
 *  I've just derived from std::exception
 *
 */
#pragma once





#include "LppError.h"


class LppExcept : std::exception {
public:
// Constructors, destructor;
    LppExcept();
	LppExcept(int c, char const* w, LppErrStatus s, char const* m);
    ~LppExcept();

// Public data members - no setters/getters
	int code;
	std::string origin;
	LppErrStatus level;
	std::string message;

// Overload the std::exception function.
	char const* what();
 
// Bothersome pointer to text on heap because of what what() returns.
private:
    char* bothersome;
};
