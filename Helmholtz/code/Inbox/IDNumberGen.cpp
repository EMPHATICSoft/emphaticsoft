//  IDNumberGen.cxx
//  garsoft-mrb
//
//  Created by Leo Bellantoni on 2 Aug, 2019
//  With lots of help from Paul Russo :)



/* But it has a bug and does not compile!



#include "IDNumberGen.h"


IDNumberGen* IDNumberGen::create(IDNumber iniValue) { 

    // Repeated calls to new here do not create a memory leak.
    // Some kind of wierd C++11 magic.
    static IDNumberGen* tmp = new IDNumberGen();
    // If creating an instance with an initial value
    if (iniValue != std::numeric_limits<IDNumber>::max()) {
        // and if the iniValue isn't one of the previously used ones
        std::set<IDNumber>::iterator itr = previousInitializers.find(iniValue);
        if (itr==previousInitializers.end()) {
            previousInitializers.insert(iniValue);
            nextOneToMake.store(iniValue);
        }
    }
    // Even if nextOneToMake.store() not called, still fine that tmp is
    // uninitialized as getNewOne() only pulls info from a static field.
    return tmp;
}



IDNumber IDNumberGen::getNewOne() {
    IDNumber retval = nextOneToMake;
    nextOneToMake.store(nextOneToMake+1);
    return retval;
}



void IDNumberGen::newEventReset() {
    previousInitializers.clear();
    nextOneToMake.store(std::numeric_limits<IDNumber>::max());
    return;
}



std::atomic<IDNumber> IDNumberGen::nextOneToMake = std::numeric_limits<IDNumber>::max();
std::set<IDNumber>    IDNumberGen::previousInitializers = {};

IDNumberGen::IDNumberGen() {}
*/
