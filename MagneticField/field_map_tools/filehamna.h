/*  L++ filehamna
 *  Created by Leo Bellantoni on 12/27/12.
 *  Copyright 2012 FRA All rights reserved.
 *
 */
#ifndef LppFilX_H
#define LppFilX_H



#include <sys/stat.h>
#include <string>
#include <iostream>
using std::cout;
using std::endl;



inline bool filehamna(const std::string& filename) {
    struct stat buf;
    int retval = stat(filename.c_str(), &buf);
    if (retval==0 && !S_ISREG(buf.st_mode)) {
        std::cout << "File " << filename.c_str() << " hamna!" << std::endl;
        abort();
    }
    // -1 is the error condition, meaning that the file isn't there
    return (retval == -1);
}



#endif
