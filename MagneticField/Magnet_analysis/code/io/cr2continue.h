/*  L++ cr2continue
 *  Created by Leo Bellantoni on 2/26/14.
 *  Copyright 2014 FRA All rights reserved.
 *
 */
#pragma once



#include <sstream>



inline bool cr2continue() {
    std::string input = "";
    std::cout << "<cr> to continue, other & <cr> to stop ";
    std::getline(std::cin, input);
    return (input.size()==0);
}
