/*  L++ LppError
 *  Created by Leo Bellantoni on 12/30/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  An error handling class, used throughout L++.
 *
 */
#include "../problems/LppError.h"
#include "../problems/LppExcept.h"
using namespace std;


LppError::LppError() {
    #include "ErrorCodeList.h"
    for (int i=0; i<UsrErrNbr; ++i) {
		UsrIssueCount[0] = 0;
	}
	return;
}


// No constructors or destructors; here is the functional part
void LppError::LppIssue(int cd, char const* where) {
	if (LppErrTbl.find(cd)==LppErrTbl.end()) {
        cout << "What is error code " << cd << "?" << endl;
		throw LppExcept(0,where,Fatal,"L++ Invalid error generated");
	}
    if (LppErrTbl[cd].level == Fatal) {
		throw LppExcept(cd,where,LppErrTbl[cd].level,LppErrTbl[cd].message);
	}
	
	// I'm giving you THREE chances...
	if (LppErrTbl[cd].count<3) {
        string info;
        if (LppErrTbl[cd].level == Warning) {
			info = "L++ Warning at ";
		}
		if (LppErrTbl[cd].level == Inform) {
			info = "L++ Inform at ";
		}
        info = info +where +": " +LppErrTbl[cd].message;
 		cout << info << endl;
		++LppErrTbl[cd].count;
	}
	return;
}	// Observe:  a function definition is a block and hence doesn't need ;


void LppError::UsrIssue(int cd, LppErrStatus level, char const* where,
                        char const* message) {
    cd = -cd; // User error codes must be negative & nonzero
	if ((cd<1) || (cd>=UsrErrNbr)) {
		throw LppExcept(0,where,Fatal,"User: Invalid error generated");
	}
	if (level == Fatal) {
        cout << endl << "User exception at " << where << ": " << endl;
        string mess(message);
		throw runtime_error(mess);
	}
	
	// I'm giving you THREE chances...
	if (UsrIssueCount[cd]<3) {
		char info[256];
		if (level == Warning) {
			strcpy(info,"\nUser warning at ");
		}
		if (level == Inform) {
			strcpy(info,"\nUser inform at ");
		}
		strcat(info,where);
		strcat(info,": ");
		strcat(info,message);
        
		cout << info << endl;
		++UsrIssueCount[cd];
	}
	return;
}
