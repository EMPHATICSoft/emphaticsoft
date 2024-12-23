/*  L++ Ldata
 *  Created by Leo Bellantoni on 1/9/14.
 *  Copyright 2014 FRA. All rights reserved.
 *
 *  A set of data points for scatter plots.
 *
 */
#pragma once



#include <fstream>

#include "LppGlobals.h"
#include "ra.h"
#include "xp.h"
#include "quadd.h"
#include "filehamna.h"





#define  LdataPointItr vector<Ldata::Point>::iterator





class Ldata{
public:
    // We also need ============================================================
    class Point {
    public:
        Point() : dx(0.0), dy(0.0), inFit(true) {}
        Point(double xIn, double dxIn, double yIn, double dyIn) :
            x(xIn),dx(dxIn),y(yIn),dy(dyIn), inFit(true) {}
        // Copy and assignment constructors
        Point(Point const& inPoint) {
            *this = inPoint;
        }
        Point& operator=(Point const& rhs) {
            if (this != &rhs) {
                 x = rhs.x;      y = rhs.y;
                dx = rhs.dx;    dy = rhs.dy;
                inFit = rhs.inFit;
            }
            return *this;
        }
        
        // with some simple binary ops
        Point  operator+ (Point const& rhs) const {
            Point retval(*this);    return (retval += rhs);
        }
        Point& operator+=(Point const& rhs) {
            x += rhs.x;             y += rhs.y;
            dx = Qadd(dx,rhs.dx);   dy = Qadd(dy,rhs.dy);
            return *this;
        }
        Point  operator- (Point const& rhs) const {
            Point retval(*this);    return (retval -= rhs);
        }
        Point& operator-=(Point const& rhs) {
            x -= rhs.x;             y -= rhs.y;
            dx = Qadd(dx,rhs.dx);   dy = Qadd(dy,rhs.dy);
            return *this;
        }
        // Including one to permit sorting
        bool operator<(Point const& rhs) const {
            return x < rhs.x;
        }
        
        // These 5 data
        double x, dx, y, dy;
        bool   inFit;
    };  // Point class definition ends here ====================================

 
    
    // No dynamic mempry, no default constructor or destructor
    Ldata();
    Ldata(Ldata const& inData);
    Ldata& operator=(Ldata const& rhs);   // deep copy! could sink a lot of CPU!



    // Constructor for the masses
    Ldata(std::string inInfo);
    
    // Deque operations for manipulating the Points of an Ldata
    void push(Point datum);     // invokes vector::push_back, which invokes
                                // the copy constructor of the pushed class, in
                                // this case, Point(Point const&) which does a
                                // deep copy rather than a shallow move
    void erase(LdataPointItr ptr);      // Remove the element pointed to
    void clear();                       // Reset the whole vector
    Point& operator[] (addr i);
    addr size() const;
    LdataPointItr begin();
    LdataPointItr end();

    // Finding and scaling data
    LdataPointItr find(double x);     // returns this.end() on fail
    LdataPointItr find(double x, double y);
    void Scale(double const& scale);

    // Sort the data by x value
    void Sort();
    
    // Statistics analysis using weighting from 1/dy-squared.
    enum stat {SumW=1, MeanX=2,MeanY=3, RmsX=4,RmsY=5, Rho=6, B=7,M=8,dB=9,dM=10 };
    double Wstats(stat s);

    // CSV output: watch out for text printing precision problems!
    void CSVout(std::string filename = "");
    
    
    std::string info;               // Data set title, whatever
    std::vector<Point> Points;      // public, yea, sure.
};
