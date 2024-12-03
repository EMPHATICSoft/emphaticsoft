/*  L++ date.h
 *  Created by Leo Bellantoni on 11/19/12.
 *  Copyright 2012 FRA. All rights reserved.
 *
 *  Date class written because <ctime> doesn't do what I want and I
 *  can write what I want faster than I can figure out some library.
 *
 */
#pragma once



#include "LppGlobals.h"



class date{
public:
    // Conversion functions for day & months.
    static std::string convertDay(int inDay){
        switch (inDay) {
            case  0: return "Sun";
            case  1: return "Mon";
            case  2: return "Tue";
            case  3: return "Wen";
            case  4: return "Thu";
            case  5: return "Fri";
            case  6: return "Sat";
            LppUrk.LppIssue(10,"convertDay(int)");
        }
        return "";      // Shaddup, compiler
    }
    static int convertDay(std::string inDay){
        for (std::string::size_type i=0; i<inDay.length(); ++i) {
            inDay[i] = toupper(inDay[i]);
        }
        if (inDay == "SUN" || inDay == "SUNDAY")    return  0;
        if (inDay == "MON" || inDay == "MONDAY")    return  1;
        if (inDay == "TUE" || inDay == "TUEDAY")    return  2;
        if (inDay == "WEN" || inDay == "WEDNESDAY") return  3;
        if (inDay == "THU" || inDay == "THURSDAY")  return  4;
        if (inDay == "FRI" || inDay == "FRIDAY")    return  5;
        if (inDay == "SAT" || inDay == "SATUDAY")   return  6;
        LppUrk.LppIssue(10,"convertDAY(string)");
        return 0;     // Shaddup, compiler.
    }
    static int convertMonth(std::string inMonth) {
        for (std::string::size_type i=0; i<inMonth.length(); ++i) {
            inMonth[i] = toupper(inMonth[i]);
        }
        
        if (inMonth == "JAN" || inMonth == "JANUARY")   return  1;
        if (inMonth == "FEB" || inMonth == "FEBUARY")   return  2;
        if (inMonth == "MAR" || inMonth == "MARCH")     return  3;
        if (inMonth == "APR" || inMonth == "APRIL")     return  4;
        if (inMonth == "MAY")                           return  5;
        if (inMonth == "JUN" || inMonth == "JUNE")      return  6;
        if (inMonth == "JUL" || inMonth == "JULY")      return  7;
        if (inMonth == "AUG" || inMonth == "AUGUST")    return  8;
        if (inMonth == "SEP" || inMonth == "SEPTEMBER") return  9;
        if (inMonth == "OCT" || inMonth == "OCTOBER")   return 10;
        if (inMonth == "NOV" || inMonth == "NOVEMBER")  return 11;
        if (inMonth == "DEC" || inMonth == "DECEMBER")  return 12;
        LppUrk.LppIssue(10,"convertMonth(string)");
        return 0;     // Shaddup, compiler.
    }
    static std::string convertMonth(int inMonth){
        switch (inMonth) {
            case  1: return "Jan";
            case  2: return "Feb";
            case  3: return "Mar";
            case  4: return "Apr";
            case  5: return "May";
            case  6: return "Jun";
            case  7: return "Jul";
            case  8: return "Aug";
            case  9: return "Sep";
            case 10: return "Oct";
            case 11: return "Nov";
            case 12: return "Dec";
            LppUrk.LppIssue(10,"convertMonth(int)");
        }
        return "";      // Shaddup, compiler
    }


    // Constructors.  Default destructor should be OK
    date();
    date(int inYear, int inMonth, int inDay);

    // Copy and assignment operators
    date(date const& inDate);
    date& operator=(date const& rhs);

    // Setters and getters
    void setYear(int inYear);
    void setMonth(int inMonth);
    void setDay(int inDay, int inYear, int inMonth);
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    
    void setDayNumber(int inDayNumber);
    int  getDayNumber() const;
    int  getDayOfWeek() const;
    
    // Members for which the class was created
    date  operator+ (date const& rhs);      date  operator+ (int const& rhs);
    date& operator+=(date const& rhs);      date& operator+=(int const& rhs);
    date  operator- (date const& rhs);      date  operator- (int const& rhs);
    date& operator-=(date const& rhs);      date& operator-=(int const& rhs);

    bool  operator< (date const& rhs) const;      bool  operator==(date const& rhs) const;
    bool  operator> (date const& rhs) const;      bool  operator!=(date const& rhs) const;
    bool  operator<=(date const& rhs) const;      bool  operator>=(date const& rhs) const;

    // Text output of the whole date.
    inline friend std::ostream& operator<<(std::ostream& out, const date& d) {
        int Daisy = d.getDay();
        if (Daisy < 10) {
            out << " " << Daisy << " " << convertMonth(d.getMonth()) << " " << d.getYear();
        } else {
            out <<        Daisy << " " << convertMonth(d.getMonth()) << " " << d.getYear();
        }
        return out;
    }

    
private:
    int  Year, Month, Day;
    int  days_since_0Jan1800;

    static int DaysInMonth[13];
    bool is_leap(int inYear);
    bool validYear(int inYear);    bool validMonth(int inMonth);
    bool validDay(int inDay, int inYear, int inMonth);
    void set_since_0Jan1800();  // Not your grampa's setter & getter; but rather
    void get_since_0Jan1800();  // to harmonize private data fields
};
