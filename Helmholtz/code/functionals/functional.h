/*  L++ functional class
 *  Created by Leo Bellantoni on 22 Dec 2015.
 *  Copyright 2015 FRA. All rights reserved.  Well, those that exist I guess.
 *  This might be from John J Barton's book.
 *
 */
#pragma once



#include "ra.h"



class functional {
/*  A pure virtual function, as declared here, implicitly makes the class it is
    defined for (in this case, functional) abstract.  Abstract classes cannot
    be instantiated.  Derived classes need to override/implement all inherited
    pure virtual functions. If they do not, they too will become abstract.
    A pure virtual function can be private, and this is done as part of the
    Template design pattern.
    
    This is the signature for a multidimensional function; for a function of a
    single variable, you might use an ra of 1 value, or you might just define
    the double operator()(double x) in your class and not derive from functional
    at all.
*/

public:
    virtual double operator()(ra<double>* X) = 0;
};
