/*  L++ singleton
 *  Created by Leo Bellantoni on 15 Aug 2018.
 *  Copyright 2018 FRA. All rights reserved.
 *
 *  Example of canonical singleton coding pattern.  This one just encapsulates
 *  a little bit of data.
 *
 */
#include "singleton.h"



singleton::singleton():
    data(nullptr) {}

singleton* singleton::fInstance = nullptr;

singleton* singleton::Instance() {
    if (fInstance == nullptr) fInstance = new singleton();
    return fInstance;
} 
