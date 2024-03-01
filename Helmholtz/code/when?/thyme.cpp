/*  L++ thyme.h
 *  Created by Leo Bellantoni on 12 May 15.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  Time of day class written because <ctime> doesn't do what I want and I
 *  can write what I want faster than I can figure out some library.  Name
 *  chosen to avoid confict with time.h & its contents or C++ form
 *
 */
#include "thyme.h"
using namespace std;


// Constructors
thyme::thyme() :
Hour(0), Minute(0), Second(0), sec_since_midnight(0) {}

thyme::thyme(int inHour, int inMinute, double inSecond) {
    bool valid = true;
    if (inHour<0   || 23<inHour)   valid = false;
    if (inMinute<0 || 59<inMinute) valid = false;
    if (inSecond<0 || 60<inSecond) valid = false;
    if (!valid) {
        LppUrk.LppIssue(10,"thyme::thyme(int,int,double)");
    }
    Hour = inHour;     Minute = inMinute;    Second = inSecond;
    set_since_midnight();
    return;
}



// Copy and assignment constructors
thyme::thyme(thyme const& inThyme) {
    *this = inThyme;
}
thyme& thyme::operator=(thyme const& rhs) {
    if (this != &rhs) {
        Hour                = rhs.Hour;
        Minute              = rhs.Minute;
        Second              = rhs.Second;
        set_since_midnight();
    }
    return *this;
}



// Setters and getters.  Every setter/getter must update internal state
void thyme::setHour (int inHour) {
    if (inHour<0   || 23<inHour) {
        LppUrk.LppIssue(10,"thyme::setHour(int)");
    } else {
        Hour = inHour;
        set_since_midnight();
    }
    return;
}
void thyme::setMinute(int inMinute) {
    if (inMinute<0 || 59<inMinute) {
        LppUrk.LppIssue(10,"thyme::setMinute(int)");
    } else {
        Minute = inMinute;
        set_since_midnight();
    }
}
void thyme::setSecond(double inSecond) {
    if (inSecond<0 || 60<=inSecond) {
        LppUrk.LppIssue(10,"thyme::setDay(int)");
    } else {
        Second   = inSecond;
        set_since_midnight();
    }
    return;
}
void thyme::setThyme(int inHour, int inMinute, double inSecond) {
    setHour(inHour);
    setMinute(inMinute);
    setSecond(inSecond);
}

int    thyme::getHour()   const {return Hour;}
int    thyme::getMinute() const {return Minute;}
double thyme::getSecond() const {return Second;}
string thyme::getPrintable() {
    string retval = "";
    char tmp[8];
    sprintf(tmp,"%2.2d:%2.2d:",Hour,Minute);
    retval  = tmp;
    sprintf(tmp,"%06.3f",Second);
    retval += tmp;
    // Observe: local retval is copied (and cast, actually) before return.
    return retval;
}

void thyme::setThymeNumber(double inThyme) {
    if (inThyme<0 || inThyme>86400) {
        LppUrk.LppIssue(10,"thyme::setThymeNumber(double)");
    }
    sec_since_midnight = inThyme;
    get_since_midnight();
}
double thyme::getThymeNumber() {
    // Don't want sec_since_midnight made public
    return sec_since_midnight;
}


// Time arithmetic operators
thyme  thyme::operator+(thyme const& rhs) {
    thyme retval(*this);     return (retval += rhs);
}
thyme& thyme::operator+=(thyme const& rhs){
    sec_since_midnight = this->sec_since_midnight +rhs.sec_since_midnight;
    get_since_midnight();
    if (sec_since_midnight >86400) {
        LppUrk.LppIssue(10,"thyme::operator+ or +=(thyme)");
    }
    return *this;
}

thyme  thyme::operator-(thyme const& rhs) {
    thyme retval(*this);     return (retval -= rhs);
}
thyme& thyme::operator-=(thyme const& rhs){
    sec_since_midnight = this->sec_since_midnight -rhs.sec_since_midnight;
    if (sec_since_midnight < 0) {
        LppUrk.LppIssue(10,"thyme::operator- or -=(thyme)");
    }
    get_since_midnight();
    return *this;
}

thyme  thyme::operator+ (double const& rhs) {
    thyme retval(*this);     return (retval += rhs);
}
thyme& thyme::operator+=(double const& rhs){
    sec_since_midnight = this->sec_since_midnight +rhs;
    get_since_midnight();
    if (sec_since_midnight >86400) {
        LppUrk.LppIssue(10,"thyme::operator+ or +=(thyme)");
    }
    return *this;
}

thyme  thyme::operator- (double const& rhs) {
    thyme retval(*this);     return (retval -= rhs);
}
thyme& thyme::operator-=(double const& rhs){
    sec_since_midnight = this->sec_since_midnight -rhs;
    if (sec_since_midnight < 0){
        LppUrk.LppIssue(10,"thyme::operator- or -=(int)");
    }
    get_since_midnight();
    return *this;
}


// Time comparison operators
bool thyme::operator< (thyme const& rhs) const {
    return sec_since_midnight <  rhs.sec_since_midnight;
}
bool thyme::operator==(thyme const& rhs) const {
    return sec_since_midnight == rhs.sec_since_midnight;
}
bool thyme::operator> (thyme const& rhs) const {
    return sec_since_midnight >  rhs.sec_since_midnight;
}
bool thyme::operator!=(thyme const& rhs) const {
    return sec_since_midnight != rhs.sec_since_midnight;
}
bool thyme::operator<=(thyme const& rhs) const {
    return sec_since_midnight <= rhs.sec_since_midnight;
}
bool thyme::operator>=(thyme const& rhs) const {
    return sec_since_midnight >= rhs.sec_since_midnight;
}


// Keep Hour, Minute, Second synchronized
void thyme::set_since_midnight() {
    sec_since_midnight  = 3600*Hour +60*Minute +Second;
    return;
}
void thyme::get_since_midnight() {
    Hour   = sec_since_midnight/3600;
    Minute = (sec_since_midnight -3600*Hour)/60;
    Second = sec_since_midnight -3600*Hour -60*Minute;
    return;
}
