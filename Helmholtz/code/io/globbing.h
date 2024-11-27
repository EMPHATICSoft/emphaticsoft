/*  L++ globbing
 *  Created by Leo Bellantoni on 7 Sep 2017.
 *  Copyright 2017 FRA All rights reserved.
 *  Input:  a globbable string
 *  Output: a vector of strings of globbed names.
 *
 *  Example:
 *      string search_dir = "/minerva/data/users/bellanto/TwoParticles/RunIe/";
 *      string lookfor    = "TwoParts_1???_??_AnaTuple.root";
 *      int Nfiles;
 *      vector<string> files = globbing(search_dir +lookfor);
 *      cout << ( Nfiles = (int)files.size() ) << " files found.\n";
 *
 *
 */
#pragma once



#include "LppGlobals.h"
#include <glob.h>
#include <vector>
using namespace std;
using std::vector;



inline vector<string> globbing(const string& pat){
    glob_t glob_result;
    vector<string> retval;
    
    glob(pat.c_str(),GLOB_TILDE,nullptr,&glob_result);
    
    for (addr i=0; i<glob_result.gl_pathc; ++i){
        retval.push_back(string(glob_result.gl_pathv[i]));
    }
    
    globfree(&glob_result);
    // Here is why you can do this: it is OK to return a local variable, 
    // although not a pointer or reference to one.  A function returns basically
    // a value, you see; so what happens is that the expression to the right of
    // the return keyword is evaluated and sent back to the calling routine.  If
    // that value is a pointer or reference to a local variable, the value will 
    // still exist after exit but what it points to will not.  That's not what
    // happens here.  Here you just return a value.  The value happens to be a
    // vector<string> but that's OK, because as a general rule, a standard container
    // will act like a locally defined variable on the stack.  But, you might think,
    // this isn't good because retval has to be deep-copied and that's expensive.
    // Nah.  Turns out C++ has "Return Value Optimization" and avoids the deep copy.
    return retval;
}
