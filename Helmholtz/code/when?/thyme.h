/*  L++ thyme.h
 *  Created by Leo Bellantoni on 12 May 15.
 *  Copyright 2015 FRA. All rights reserved.
 *
 *  thyme of day class written because <ctime> doesn't do what I want and I
 *  can write what I want faster than I can figure out some library.  Name 
 *  chosen to avoid confict with time.h & its contents or C++ form.
 *  Military time only!
 *
 */
#pragma once



#include "LppGlobals.h"



class thyme{
public:
    
    // Constructors.  Default destructor should be OK
    thyme();
    thyme(int inHour, int inMinute, double inSecond);
    
    // Copy and assignment operators
    thyme(thyme const& inThyme);
    thyme& operator=(thyme const& rhs);
    
    // Setters and getters
    void   setHour(int inHour);
    void   setMinute(int inMinute);
    void   setSecond(double inSecond);
    void   setThyme(int inHour, int inMinute, double inSecond);
    int    getHour() const;
    int    getMinute() const;
    double getSecond() const;
    std::string getPrintable();
    
    void   setThymeNumber(double inThyme);
    double getThymeNumber();
    
    // Members for which the class was created
    thyme  operator+ (thyme const& rhs);      thyme  operator+ (double const& rhs);
    thyme& operator+=(thyme const& rhs);      thyme& operator+=(double const& rhs);
    thyme  operator- (thyme const& rhs);      thyme  operator- (double const& rhs);
    thyme& operator-=(thyme const& rhs);      thyme& operator-=(double const& rhs);
    
    bool  operator< (thyme const& rhs) const;      bool  operator==(thyme const& rhs) const;
    bool  operator> (thyme const& rhs) const;      bool  operator!=(thyme const& rhs) const;
    bool  operator<=(thyme const& rhs) const;      bool  operator>=(thyme const& rhs) const;

    // Text output of the whole thyme.
    inline friend std::ostream& operator<<(std::ostream& out, const thyme& t) {
        int Harry = t.getHour();
        if (Harry < 10) {
            out << " " << Harry;
        } else {
            out        << Harry;
        }
        int Minne = t.getMinute();
        if (Minne < 10) {
            out << ":0" << Minne;
        } else {
            out << ":"  << Minne;
        }
        double Sally = t.getSecond();
        if (Sally < 10.0) {
            out << ":0" << Sally;
        } else {
            out << ":"  << Sally;
        }
        return out;
    }
    
private:
    int  Hour, Minute;
    double Second;
    double sec_since_midnight;

    void set_since_midnight();  // Not your grampa's setter & getter; but rather
    void get_since_midnight();  // to harmonize private data fields
};
