//
//  mergeChecker.hpp
//  L++
//
//  Created by Leo Bellantoni on 13 Sep 2023.
//



#ifndef mergeChecker_hpp
#define mergeChecker_hpp



#include "quadd.h"
#include "Lbins.h"
#include "Lhist1.h"
#include "Lfit1.h"
#include "Lhist2.h"
#include "Lzcol.h"
#include "Funcs.h"
#include "FieldMap.h"





class mapChecker {
public:
    mapChecker(FieldMap* aMap_in);

    void checkMap();



private:
    FieldMap* aMap;
};





#endif
