/*  L++ inputtextfile
 *  Created by Leo Bellantoni on 8 Apr 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  Tokenizing text input is unnecessarily hard in C++.  This should simplify it.
 *
 *  Whitespace, i.e. ' ' (blanks) and'\t' (tabs) is the default tokenizing
 *  character; the default appears as "\0" in subWhite argument to the fetchline
 *  method.
 *
 *  The char EoL is the end-of-line character.  By default it is set to '\n'
 *  (newline) in the constructor, but it is public so you can change it yourself.
 *  Here's a usage example:
 *
 *  inputtextfile fromAcnet(<std::string filename>);
 *  string makeWhite = ":";
 *  while (fromAcnet.fetchline(makeWhite) != -1) {
 *      double Secs  = fromAcnet.tokenass(0);   // 1st token "as double"
 *      string Month = fromAcnet.tokenass(1);   // "token as string"
 *      int    Day   = fromAcnet.tokenasi(2);   // "token as integer"
 *      < do something with that data >
 *  }
 *
 */
#ifndef Lpp_inTxt
#define Lpp_inTxt



#include <fstream>
#include <string>
#include "MagneticField/field_map_tools/filehamna.h"



class inputtextfile {
public:
    inputtextfile(std::string filename);
    ~inputtextfile();

    // Fetch a line and get it ready for token translation; return number of
    // tokens in the line, or -1 if EOF.  Argument is a string of characters to
    // be substituted for whitespace; default value of "\0" means just use
    // ' ' (blanks) and'\t' (tabs) as whitespace
    int fetchline(std::string subWhite="\0");
    
    void rewind();
    
    // Sometimes the new OS X security question about how “L++” would like to
    // access files in your Desktop folder. does not get generated quickly
    // enough and then the new fstream step fails, producing a failbit.  This
    // option over-rides that failure
    bool overrideFailbit = true;

    // Pick out the ith token (counting from zero) and sscanf it with format code
    // As double, int or char*
    double      tokenasd(size_t i);
    int         tokenasi(size_t i);
    std::string tokenass(size_t i);
    
    // For debugging your input file, use this:
    std::string wtfIN();
    
    int  LinesRead;     // equivalently, number of successful calls to fetchline
    char EoL;
private:
    std::string   lineIn;
    std::string   initialLineIn;
    std::fstream* fileIN;
};


#endif
