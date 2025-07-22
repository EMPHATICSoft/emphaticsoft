/*  L++ talk2me
 *  Created by Leo Bellantoni on 8 Apr 2015.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  Keyboard input is unnecessarily hard.  The following should simplify it.
 *  You also have to #include <sstream> when porting to Linux machines.
 *
 *  Default constructor has no bounds checks.  These can be introduced by
 *  setting appropriate values in checkLo, checkHi, cutLo and cutHi.
 *
 */
#pragma once



#include <sstream>
#include <iostream>
#include <string>



template <class T> class talk2me {
public:
    bool checkLo,checkHi;
    T    cutLo,  cutHi;
    std::string prompt;
    
    talk2me() : checkLo(false),checkHi(false),prompt("Yo. ") {}
    
    T now() {
        std::string input = "";
        T retval;           bool OKinput;
        
        do {std::cout << prompt << " ";
            getline(std::cin, input);
            std::stringstream tempStream(input);
            tempStream >> retval;
            
            OKinput = true;
            if (checkLo && retval<cutLo) OKinput =false;
            if (checkHi && retval>cutHi) OKinput =false;
        } while ( !OKinput );
        
        return retval;
    }
};
