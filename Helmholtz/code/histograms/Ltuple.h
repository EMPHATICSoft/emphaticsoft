/*  L++ Ltuple
 *  Created by Leo Bellantoni on 12/1/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  Light, fast, machine dependent ntuple.
 *  Constructors can throw runtime_error
 *  N.B. Linux has a filesize limit of 2^31 - 1 = 2Gbyte (?)
 *
 */
#pragma once



#include <iostream>
#include "LppGlobals.h"





class Ltuple {public:
    #include "Ltuple.def"
    #define Ltu (Lt->ple)



    Ltuple();
    // Careful! operator= and copy constructor will leave pointers
    // to the same file in two places.  Why?  I don't remember!
    Ltuple(Ltuple const& rhs);
    Ltuple& operator= (Ltuple const& rhs);
    ~Ltuple();



    // The argument exists corresponds to reading it vs. writing it
    Ltuple(char const* filename, bool exists);

    void clear_ple();                   // set entire tuple to zero
    void write_ple() const;
    bool read_ple();                    // true if successful, false if EOF
                                        // can hurl a std::runtime_error


    
private:
    FILE* fp_;
    char* filename_;
    bool  exists_;
    void  comcopy(Ltuple const& rhs, bool constructing);
};
