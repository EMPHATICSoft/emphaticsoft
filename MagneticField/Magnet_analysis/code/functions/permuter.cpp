/*  L++   permuter.cpp
 *  Created by Leo Bellantoni on 24 Sep 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 */


#include "permuter.h"


// Standard boilerplate
permuter::permuter()
    : size(0), from1(false), data(nullptr) {}

permuter::permuter(permuter const& inPerm)
    : data(nullptr){
    *this = inPerm;
    return;
}
permuter& permuter::operator=(permuter const& rhs) {
    if (this != &rhs) {
        size  = rhs.size;
        from1 = rhs.from1;
        delete data;
        if (rhs.data) data = new ra<int>(size);
        if (!from1) data->setbase(0);
        
        int i = from1 ?      1 :    0;
        int N = from1 ? size+1 : size;
        for (; i<N; ++i) (*data)(i) = (*rhs.data)(i);
    }
    return *this;
}

permuter::~permuter() {
    delete data;
    return;
}





// Users enter here.
permuter::permuter(int n, bool inFrom1) {
    if (n<2) {
        LppUrk.LppIssue(114, "permuter(int)");
    }

    size  = n;
    from1 = inFrom1;
    data = new ra<int>(size);
    if (!from1) data->setbase(0);
    init();
    return;
}

void permuter::init() {
    // Set the initial permutation 0,1,...n-1 or perhaps 1,2,...n
    int i = from1 ?      1 :    0;
    int N = from1 ? size+1 : size;
    for (; i<N; ++i) (*data)(i) = i;
    return;
}

void permuter::next() {
    // Increment to the next permutation - the guts is here!
    int i, j, k, N, tmp;
    N = from1 ? size+1 : size;

    i = N - 2;
    while ( (*data)(i) > (*data)(i+1) &&
            ((from1 && i!=1) || (!from1 && i!=0)) ) i--;
    
    // Are all the elements in decreasing order?  Then you're done
    if (!from1) {
        if ((i==0) && (*data)(0) > (*data)(1)) {
            init();
            LppUrk.LppIssue(115, "permuter::next() [1]");
            return;
        }
    } else {
        if ((i==1) && (*data)(1) > (*data)(2)) {
            init();
            LppUrk.LppIssue(115, "permuter::next() [2]");
            return;
        }
    }
    
    k = i + 1;
    for (j = i + 2; j < N; j++ ) {
        if ( (*data)(j) > (*data)(i) &&
             (*data)(j) < (*data)(k) ) {
            k = j;
        }
    }
    
    // Swap i and k
    tmp = (*data)(i);
    (*data)(i) = (*data)(k);
    (*data)(k) = tmp;
    
    for (j = i + 1; j <= ((N + i) / 2); j++) {
        tmp = (*data)(j);
        (*data)(j) = (*data)(N +i -j);
        (*data)(N +i -j) = tmp;
    }
    return;
}

int permuter::get(const int i) {
    return (*data)(i);
}
