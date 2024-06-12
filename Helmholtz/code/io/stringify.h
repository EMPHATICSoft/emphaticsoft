/*  L++ stringify
 *  Created by Leo Bellantoni on 1 Apr 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Strongly typed languages are wonderful for writing bug free code, but
 *  writing a numeric value into a string is unnecessarily hard in C++.
 *  So is the inverse operation, converting a number in a string to a
 *  numeric type.  The following should simplify it.
 *
 */
#pragma once



#include <string>



template<typename T> std::string stringify(T const& x) {
    std::ostringstream o;
    if (!(o << x)) LppUrk.LppIssue(0,"stringify");
    return o.str();
}



template<typename T> T yfignirts(std::string const& s,
                                 bool failIfLeftoverChars =false) {
    std::istringstream i(s);
    char c;
    T x;
    if (!(i >> x) || (failIfLeftoverChars && i.get(c))) LppUrk.LppIssue(0,"yfignirts");
    return x;
}
