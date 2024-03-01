/*  L++ concatVectors
 *  Created by Leo Bellantoni on 22 Jun 2020.
 *  Copyright 2020 FRA. All rights reserved.
 *
 *  Template function to concatenate two vectors.
 *
 */
#pragma once



template <typename T> vector<T> concat(vector<T>& vectorA, vector<T>& vectorB) {
    vector<T> retval;
    retval.reserve( vectorA.size() +vectorB.size() );
    retval.insert(retval.end(), vectorA.begin(),vectorA.end() );
    retval.insert(retval.end(), vectorB.begin(),vectorB.end() );
    return retval;
}
