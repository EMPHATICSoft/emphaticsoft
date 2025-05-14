//
//  Lcounter.cpp
//  L++
//
//  Created by Leo Bellantoni on 11/17/22.
//
#include "Lcount.h"



Lcount::Lcount(vector<string> inTags) : tags(inTags) {
    count.resize(tags.size());
    for (addr i=0; i<count.size(); ++i) count[i] = 0;
}

// Default, copy and assignment constructors
Lcount::Lcount() {};    // tags and count are stdlib and initialize themselves
Lcount::Lcount(Lcount const& inLcount) {
    *this = inLcount;
}
Lcount& Lcount::operator=(Lcount const& rhs) {
    if (this != &rhs) {
        tags = rhs.tags;
        count = rhs.count;
    }
    return *this;
}

// Increment operation
void Lcount::click (string tag, double amount) {
    for (addr i=0; i<tags.size(); ++i) {
        if (tag.compare(tags[i])==0) {
            count[i] += amount;
            return;
        }
    }
    LppUrk.LppIssue(309,"Lcount::click");
}

// Readout operations
int Lcount::total(string tag) {
    return nint(dTotal(tag));
    
}
double Lcount::dTotal(string tag) {
    for (addr i=0; i<tags.size(); ++i) {
        if (tag.compare(tags[i])==0) {
            return count[i];
        }
    }
    LppUrk.LppIssue(309,"Lcount::total/dTotal");
    return 0.0;   // Shaddup, compiler
}
