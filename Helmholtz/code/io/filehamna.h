/*  L++ filehamna
 *  Created by Leo Bellantoni on 12/27/12.
 *  Copyright 2012 FRA All rights reserved.
 *
 */
#pragma once



#include "LppGlobals.h"
#include <sys/stat.h>
#include <string>



inline bool filehamna(const std::string& filename) {
    struct stat buf;
    int retval = stat(filename.c_str(), &buf);
    if (retval==0 && !S_ISREG(buf.st_mode)) {
        LppUrk.LppIssue(19,"filehamna(string)");
    }
    // -1 is the error condition, meaning that the file isn't there
    return (retval == -1);
}
