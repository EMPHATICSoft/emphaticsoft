/*  L++ concatVectors
 *  Created by Leo Bellantoni on 22 Jun 2020.
 *  Copyright 2020 FRA. All rights reserved.
 *
 *  Template function to concatenate two vectors.
 *
 */
#pragma once



#include <vector>



template <typename T> std::vector<T> concat(std::vector<T>& vectorA, std::vector<T>& vectorB) {
    std::vector<T> retval;
    retval.reserve( vectorA.size() +vectorB.size() );
    retval.insert(retval.end(), vectorA.begin(),vectorA.end() );
    retval.insert(retval.end(), vectorB.begin(),vectorB.end() );
    return retval;
}
