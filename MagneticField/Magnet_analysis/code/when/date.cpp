/*  L++ date.cpp
 *  Created by Leo Bellantoni on 11/19/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  Date class written because <ctime> doesn't do what I want and I
 *  can write what I want faster than I can figure out some library.
 *
 */
#include "date.h"
using namespace std;


// Constructors
date::date() :
    Year(0), Month(0), Day(0) {
    set_since_0Jan1800();
}
date::date(int inYear, int inMonth, int inDay) {
    bool valid = true;
    if (!validYear (inYear))  valid = false;
    if (!validMonth(inMonth)) valid = false;
    // Need to know Year and Month before setting Day.
    if (!validDay  (inDay, inYear,inMonth))   valid = false;
    if (!valid) {
        LppUrk.LppIssue(10,"date::date(int,int,int)");
    }
    Year = inYear;     Month = inMonth;    Day = inDay;
    set_since_0Jan1800();
    return;
}


// Copy and assignment constructors
date::date(date const& indate) {
	*this = indate;
}
date& date::operator=(date const& rhs) {
	if (this != &rhs) {
        Year                = rhs.Year;
        Month               = rhs.Month;
        Day                 = rhs.Day;
        days_since_0Jan1800 = rhs.days_since_0Jan1800;
    }
	return *this;
}


// Setters and getters.  Every setter/getter must update internal state
void date::setYear (int inYear) {
    if (validYear(inYear)) {
        Year = inYear;
        set_since_0Jan1800();
    } else {
        LppUrk.LppIssue(10,"date::setYear(int)");
    }
    return;
}
void date::setMonth(int inMonth) {
    if (validMonth(inMonth)) {
        Month = inMonth;
        set_since_0Jan1800();
    } else {
        LppUrk.LppIssue(10,"date::setMonth(int)");
    }
}
void date::setDay(int inDay, int inYear, int inMonth) {
    if (validDay(inDay, inYear, inMonth)) {
        Day   = inDay;
        set_since_0Jan1800();
    } else {
        LppUrk.LppIssue(10,"date::setDay(int)");
    }
    return;
}

int date::getYear()  const {return Year;}
int date::getMonth() const {return Month;}
int date::getDay()   const {return Day;}

void date::setDayNumber(int inDayNumber) {
    days_since_0Jan1800 = inDayNumber;
    get_since_0Jan1800();
}
int date::getDayNumber() const {
    // Don't want days_since_0Jan1800 made public
    return days_since_0Jan1800;
}
int date::getDayOfWeek() const {
    return (days_since_0Jan1800-5)%7;
}

// Date arithmetic operators
date  date::operator+(date const& rhs) {
    date retval(*this);     return (retval += rhs);
}
date& date::operator+=(date const& rhs){
    days_since_0Jan1800 = this->days_since_0Jan1800 +rhs.days_since_0Jan1800;
    get_since_0Jan1800();
    return *this;
}

date  date::operator-(date const& rhs) {
    date retval(*this);     return (retval -= rhs);
}
date& date::operator-=(date const& rhs){
    days_since_0Jan1800 = this->days_since_0Jan1800 -rhs.days_since_0Jan1800;
    if (days_since_0Jan1800 < 0) {
        LppUrk.LppIssue(10,"date::operator- or -=(date)");
    }
    get_since_0Jan1800();
    return *this;
}

date  date::operator+ (int const& rhs) {
    date retval(*this);     return (retval += rhs);
}
date& date::operator+=(int const& rhs){
    days_since_0Jan1800 = this->days_since_0Jan1800 +rhs;
    get_since_0Jan1800();
    return *this;
}

date  date::operator- (int const& rhs) {
    date retval(*this);     return (retval -= rhs);
}
date& date::operator-=(int const& rhs){
    days_since_0Jan1800 = this->days_since_0Jan1800 -rhs;
    if (days_since_0Jan1800 < 0){
        LppUrk.LppIssue(10,"date::operator- or -=(int)");
    }
    get_since_0Jan1800();
    return *this;
}


// Date comparison operators
bool date::operator< (date const& rhs) const {
    return days_since_0Jan1800 <  rhs.days_since_0Jan1800;
}
bool date::operator==(date const& rhs) const {
    return days_since_0Jan1800 == rhs.days_since_0Jan1800;
}
bool date::operator> (date const& rhs) const {
    return days_since_0Jan1800 >  rhs.days_since_0Jan1800;
}
bool date::operator!=(date const& rhs) const {
    return days_since_0Jan1800 != rhs.days_since_0Jan1800;
}
bool date::operator<=(date const& rhs) const {
    return days_since_0Jan1800 <= rhs.days_since_0Jan1800;
}
bool date::operator>=(date const& rhs) const {
    return days_since_0Jan1800 >= rhs.days_since_0Jan1800;
}


// Initialize static arrays in cpp file
int date::DaysInMonth[13] = {0, 31,28,31,30,31,30,31,31,30,31,30,31};


// A few handy functions
bool date::is_leap(int inYear) {
    // 1600, 2000, 2400 are leap years other century years are not.  Excel has
    // intentional bug where 1900 is a leap year, because Lotus 1-2-3 had that
    if ( (inYear-1600)%400 == 0) {
        return true;
    } else if ( inYear%100 == 0) {
        return false;
    } else if ( inYear%4   == 0) {
        return true;
    } else {
        return false;
    }
}
bool date::validYear(int inYear) {
    return (inYear >= 1800);
}
bool date::validMonth(int inMonth) {
    return ( 0 < inMonth && inMonth < 13);
}
bool date::validDay(int inDay, int inYear, int inMonth) {
    bool retval = validYear(inYear) && validMonth(inMonth);
    if ( inDay < 1  || 31 < inDay ) retval = false;
    if ( inMonth==4 || inMonth==6 || inMonth==9 || inMonth==11){
        if (inDay > 30) retval = false;
    }
    if ( inMonth==2 ){
        if (is_leap(inYear)) {
            if (inDay > 29) retval = false;
        } else {
            if (inDay > 28) retval = false;
        }
    }
    return retval;
}

        
// The hard part: counting of days
void date::set_since_0Jan1800() {
    // Convert Year,Month,Day to internal field days_since_0Jan1800
    days_since_0Jan1800 = (Year -1800)*365;

    int leaps_since_0Jan1800 = (Year -1801)/4;
    if (Year>1900) --leaps_since_0Jan1800; // 1900 is not a leap year
    days_since_0Jan1800 += leaps_since_0Jan1800;

    if (Month >  1) days_since_0Jan1800 +=  DaysInMonth[1];
    if (Month >  2) days_since_0Jan1800 +=  (is_leap(Year)? 29: 28);
    for (int i=3; i<12; ++i) {
        if (Month > i) days_since_0Jan1800 +=  DaysInMonth[i];
    }

    days_since_0Jan1800 += Day; // Use close-of-business.
    return;
}
void date::get_since_0Jan1800() {
    // Convert the private field days_since_0Jan1800 to Year,Month,Day.
    int year = 1800;
    int days_pulled = is_leap(year) ? 366 : 365;
    int count_of_days = days_since_0Jan1800;
    while (count_of_days > days_pulled) {
        count_of_days -= days_pulled;
        ++year;
        days_pulled = is_leap(year) ? 366 : 365;
    }
    Year = year;
    
    int month =01;
    days_pulled = DaysInMonth[month];
    while (count_of_days > days_pulled) {
        count_of_days -= days_pulled;
        ++month;
        days_pulled = DaysInMonth[month];
        if (month == 2 && is_leap(Year)) ++days_pulled;
    }
    Month = month;
    Day   = count_of_days;
    return;
}

