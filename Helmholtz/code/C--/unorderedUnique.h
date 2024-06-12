/*  L++ unorderedUnique
 *  Created by Leo Bellantoni on 22 Jun 2020.
 *  Copyright 2020 FRA. All rights reserved.
 *
 *  C++ has the 'unique' algorithm which removes duplicate elements if they
 *  are consecutive.  You could sort your vector and apply that, or you could
 *  just invoke this method, which will leave the 1st appearances of each 
 *  element in their original order.
 *
 */
#pragma once



template <typename T> void unorderedUnique(std::vector<T> &v) {
    if ( v.size() <= 1 ) return;
    typename vector<T>::iterator it = v.begin();
    typename vector<T>::iterator nd = v.end();
    for (; it != nd; ++it) {
        nd = std::remove(it +1, nd, *it);
    }

    v.erase(nd, v.end());
    return;
}
