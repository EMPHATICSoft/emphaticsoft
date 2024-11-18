/*  L++ inputtextfile
 *  Created by Leo Bellantoni on 8 Apr 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  Tokenizing text input is unnecessarily hard in C++.  This should simplify it.
 *
 */


#include "inputtextfile.h"
#include <cerrno>
#include <unistd.h>
using namespace std;




// Constructor for the masses
inputtextfile::inputtextfile(string filename) : LinesRead(0) {
    if (filehamna(filename)) {
        cout << filename << endl;
        LppUrk.LppIssue(17,"inputtextfile(string)");
        fileIN->close();
        delete fileIN;
    }
    int nErr = 0;
    wtf:
    fileIN = new fstream(filename.c_str(),ios_base::in);
    if (fileIN->fail()) {
        cout << filename << endl;
        if (overrideFailbit && nErr==0 && (fileIN->rdstate() & std::ifstream::failbit)!=0) {
            LppUrk.LppIssue(20,"inputtextfile(string)");
            cout << std::strerror(errno) << endl;
            fileIN->close();
            delete fileIN;
            sleep(1);       // Give the file system time to think.
            ++nErr;
            goto wtf;
        }
        LppUrk.LppIssue(15,"inputtextfile(string)");
        fileIN->close();
        delete fileIN;
    }
    EoL = '\n';   // Default end of line is \n
}

// Destructor closes file so as to free the fstream* fileIN.  Were it not
// for this level of indirection, could rely on RAII to close *fileIN.  Not that
// the 4 bytes that the pointer takes up is a huge memory leak in most cases.
inputtextfile::~inputtextfile() {
    fileIN->close();
    delete fileIN;
}

void inputtextfile::rewind() {
    fileIN->clear();
    fileIN->seekg(0);
    LinesRead = 0;
    initialLineIn = lineIn = "";
}



// Fetch a line and get it ready for token translation; return number of tokens
// in the line, or -1 if EOF.
int inputtextfile::fetchline(string subWhite) {
    int retval = 0;
    getline(*fileIN, lineIn, EoL);
    initialLineIn = lineIn;
    if (fileIN->bad()) {
        // IO error
        LppUrk.LppIssue(21,"inputtextfile::getline");
    } else if (fileIN->eof()) {
        return -1;
    } else {
        // substitute whitespace in for subWhite
        if (subWhite != "\0") {
            for (addr i=0; i<lineIn.length(); ++i) {
                for (addr j=0; j<subWhite.length(); ++j) {
                    if (lineIn[i] == subWhite[j]) lineIn[i] = ' ';
                }
            }
        }
        // Count the whitespace separated tokens.  Start by skipping whitespace
        // at the start of the line
        addr i = 0;
        for (; i<lineIn.length(); ++i) {
            if (lineIn[i] !=' '  &&  lineIn[i]   !='\t') break;
        }
        for (; i<lineIn.length(); ++i) {
            if ( (lineIn[i]   !=' ' && lineIn[i]   !='\t') &&
                 (lineIn[i+1] ==' ' || lineIn[i+1] =='\t') ) ++retval;
        }
    }
    // If the last token goes to end of line, count it too.
    char last = lineIn[lineIn.length()-1];
    if (last !=' ' && last !='\t') ++retval;

    ++LinesRead;
    return retval;
}


// Get the ith token (count i from 0) using scanf functionality
double inputtextfile::tokenasd(addr i) {
    string format = "";
    for (addr j=0; j<i; ++j) format += "%*s";
    format += "%lf";
    double retval;
    int scanned = sscanf(lineIn.c_str(),format.c_str(), &retval);
    if (scanned != 1) {
        LppUrk.LppIssue(18,"inputtextfile::tokenasd(int)");
    }
    return retval;
}
int    inputtextfile::tokenasi(addr i) {
    string format = "";
    for (addr j=0; j<i; ++j) format += "%*s";
    format += "%d";
    int retval;
    int scanned = sscanf(lineIn.c_str(),format.c_str(),&retval);
    if (scanned != 1) {
        LppUrk.LppIssue(18,"inputtextfile::tokenasi(int)");
    }
    return retval;
}
string inputtextfile::tokenass(addr i) {
    string format = "";
    for (addr j=0; j<i; ++j) format += "%*s";
    format += "%1023s";
    char retval[1024];
    int scanned = sscanf(lineIn.c_str(),format.c_str(),&retval);
    if (scanned != 1) {
        LppUrk.LppIssue(18,"inputtextfile::tokenass(int)");
    }
    return string(retval);
}


// Because who knows what darkness lies in the hearts of input files?
string inputtextfile::wtfIN() {
    return initialLineIn;
}
