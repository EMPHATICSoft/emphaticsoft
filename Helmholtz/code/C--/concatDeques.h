/*  L++ concatDeques
 *  Created by Leo Bellantoni on 3 Jan 2019.
 *  Copyright 2019 FRA. All rights reserved.
 *
 *  Template function to concatenate two deques.
 *  Also a predicate class for sorting  char* deques & other containers,
 *  and a sample (untested) predicate function to do the same thing
 *
 */
#pragma once



#include <deque>



template <typename T> std::deque<T> concatDeques(std::deque<T>& dequeA, std::deque<T>& dequeB) {
    std::deque<T> retval;        
    typename std::deque<T>::iterator iDeque = dequeA.begin();
    for (;iDeque != dequeA.end(); ++iDeque) {
        retval.push_back(*iDeque);
    }
    iDeque = dequeB.begin();
    for (;iDeque != dequeB.end(); ++iDeque) {
        retval.push_back(*iDeque);
    }
    return retval;
}



class StrCmp{public:
    bool operator() (char const* a, char const* b) {return (strcmp(a,b)<0 ? true : false);}
};

bool StrungCmp(char const* a, char const* b) {return (strcmp(a,b)<0 ? true : false);}
