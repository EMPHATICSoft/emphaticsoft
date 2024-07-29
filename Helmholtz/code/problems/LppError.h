/*  L++ LppError
 *  Created by Leo Bellantoni on 12/30/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  An error handling class, used throughout L++.  In most cases only a single instance
 *  is created but there is no great need to code up the classical singleton pattern.
 *  However, don't build copy or assignment constructors
 *
 */
#pragma once



#include <iostream>
#include <cstring>			// Evidently not in <iostream> on this version of Linux.
#include <stdexcept>
#include <map>





enum LppErrStatus {Warning, Fatal, Inform, Undefined};



class LppError{
public:
	// Synthesized destructor is fine; just specify the default constructor.
	LppError();

	// Entry point for all levels of errors in Lpp code; will throw exception
    // if Fatal
	void LppIssue(int c, char const* where);

    // Entry point for all levels of errors in user's code; will throw
    // exception if Fatal.  Error codes independent of LppIssue and negative
	void UsrIssue(int c, LppErrStatus level, char const* where,
                  char const* message);

private:
	struct table_entry {int count; LppErrStatus level; char const* message;};
    std::map<int, table_entry> LppErrTbl;

    static int const UsrErrNbr = 100;
    int UsrIssueCount[UsrErrNbr];
    
    // Forbid copy and assignment constructors
    LppError& operator=(LppError const& rhs);
    LppError(LppError const& inErr);
    
};
