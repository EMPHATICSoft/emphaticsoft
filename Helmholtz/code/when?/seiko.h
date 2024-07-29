/*  L++ seiko.h
 *  Created by Leo Bellantoni on 8/10/17.
 *  Copyright 2017 FRA. All rights reserved.
 *
 *  Combines an L++ date with an L++ thyme, and has setters/getters to deal
 *  with Unix times.  Really all this class does is convert from L++ to Unix
 *  time_t values and back.
 *
 */
#pragma once



#include <ctime>
#include "date.h"
#include "thyme.h"



class seiko{
public:
    // Constructors.  I think the default constructors and destructors are OK
    seiko(date MakeDate, thyme MakeTime);
    seiko(time_t MakeUnix);

    // Use UTC Unix time (default, true value) or local Unix time?  This only
    // pertains to time_t values; thyme does not have a time zone in it.
    void SetUTC(bool UseUTC);
    
    // Getters
    date   getDate() const;
    thyme  getTime() const;
    time_t getUnix() const;
    
private:
    date   _d;
    thyme  _t;
    time_t _u;
    bool   _UTC;
};
