/*  L++ seiko.cpp
 *  Created by Leo Bellantoni on 8/10/17.
 *  Copyright 2017 FRA. All rights reserved.
 *
 *  Combines an L++ date with an L++ thyme, and has setters/getters to deal
 *  with Unix times.  Really all this class does is convert from L++ to Unix
 *  time_t values and back.
 *
 */
#include "seiko.h"
using namespace std;



// Constructors
seiko::seiko(date MakeDate, thyme MakeTime) :
    _d(MakeDate), _t(MakeTime), _UTC(true) {
    tm temp;
    temp.tm_isdst = 0;
    temp.tm_sec   = _t.getSecond();
    temp.tm_min   = _t.getMinute();
    temp.tm_hour  = _t.getHour();
    temp.tm_mday  = _d.getDay();
    temp.tm_mon   = _d.getMonth() -1;
    temp.tm_year  = _d.getYear()  -1900;
    _u = timegm(&temp);
    return;
}
seiko::seiko(time_t MakeUnix) :
    _u(MakeUnix), _UTC(true){
    tm* temp;
    temp = gmtime(&_u);
    _d = date ( temp->tm_year+1900, temp->tm_mon+1, temp->tm_mday );
    _t = thyme( temp->tm_hour,      temp->tm_min,   temp->tm_sec  );
    return;
}



// Setters
void seiko::SetUTC(bool UseUTC) {
    if ( _UTC == UseUTC ) return;
    tm* temp;
    if (_UTC) {
        // We are going to local time
        temp = gmtime(&_u);
        _u = timelocal(temp);
    } else {
        // We are going to Greenwich time
        temp = localtime(&_u);
        _u = timegm(temp);
    }
    _UTC = UseUTC;
    return;
}


// Getters
date   seiko::getDate() const {return _d;}
thyme  seiko::getTime() const {return _t;}
time_t seiko::getUnix() const {return _u;}

