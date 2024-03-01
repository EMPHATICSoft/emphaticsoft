/*  L++ permutations
 *  Created by Leo Bellantoni on 24 Sep 2015.
 *  Copyright 2015 FRA.  Well, those that exist I guess.
 *
 *  Permutation of indices.  Written with one eye on gsl_permute, by Brian Gough
 *  I believe, who wrote it with one eye on Knuth, Vols. 1 & 3.
 *
 */
#pragma once



#include "LppGlobals.h"
#include "ra.h"



class permuter{
public:
    // Follow the rule-of-four
    permuter();
    permuter(permuter const& inPerm);
    permuter& operator=(permuter const& rhs);
    ~permuter();



    // User's constructor
    explicit permuter(int n,bool from1=false);

    void init();                // Set the initial permutation 0,1,...n-1
    void next();                // Increment to the next permutation
    int get(const int i);       // Get the ith index

private:
    addr size;
    ra<int>* data;
    bool from1;                 // Set true permuations 1,2,...n; default is off
};
