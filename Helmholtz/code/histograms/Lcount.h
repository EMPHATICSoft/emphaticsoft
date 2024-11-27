/*  L++ Lcounter
 *  Created by Leo Bellantoni on 17 Nov 2022.
 *  Copyright 2022 FRA. All rights reserved.
 *
 *  A counter class for analysis tasks.
 *
 */
#pragma once



#include <vector>
#include <string>
#include "LppGlobals.h"
#include "intsNabs.h"



class Lcount {
public:
    // Default constructor, destructor should be fine.
    // Constructor for the masses:
    Lcount(std::vector<std::string> inTags);
    
    // Default, copy and assignment constructors
    Lcount();
    Lcount(Lcount const& inLcount);
    Lcount& operator=(Lcount const& rhs);
    
    // Increment operation
    void click (std::string tag, double amount = 1.0);
    
    // Readout operations
    int total(std::string tag);
    double dTotal(std::string tag);
    
    
    
private:
    std::vector<std::string> tags;
    std::vector<double> count;
};
