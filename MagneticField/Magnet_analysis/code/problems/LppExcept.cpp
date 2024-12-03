/*  L++ LppExcept
 *  Created by Leo Bellantoni on 12/5/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  A simple exception class, thrown throughout L++.
 *
 */
#include "../problems/LppExcept.h"


// Constructors, destructor;
LppExcept::LppExcept() : exception(),
    code(-1), origin(""), level(Undefined), message(""), bothersome(nullptr) {};

LppExcept::LppExcept(int cd, char const* w, LppErrStatus s, char const* m) :
	exception(), code(cd), origin(w), level(s), message(m), bothersome(nullptr) {}

LppExcept::~LppExcept() {
    if (!bothersome) free(bothersome);
}

// Overload the std::exception function what(), sans const and throw modifiers.
char const* LppExcept::what() {
    std::string info = "L++ Exception at " +origin +" : " +message;
    size_t lenReply = info.size() +1;
    bothersome = (char*) malloc(lenReply * sizeof(char));
    strcpy(bothersome,info.c_str());
    return bothersome;
}
