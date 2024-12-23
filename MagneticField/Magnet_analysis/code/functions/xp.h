/*  L++ xp
 *  Created by Leo Bellantoni on 12/7/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  Raising to a power in a swift yet sound way.  Does not do complex numbers,
 *  as you probably want modulus not the square anyway.  The xp macro won't
 *  work for int, addr (size_t) or long types, as the macro expansion leaves
 *  a call to pow(int,int) at the preprocessor output that can not be used by
 *  the compiler.  But int will work for SQR, CUBE or QUAD.
 *
 *  Implementation in header file as everything is inlines and macros
 *
 */
#pragma once



#include "LppGlobals.h"



// In principle, could 'const' all argument lists... hardly seems worth it 
// though.  Could also try to overload ^ operator, but it is low on the 
// precedence chart and someday you would forget that and hurt yourself.
// Don't xp(this++,--that)!  Watch them side-effects & evaluation orders!

inline int         SQR(int b)         {return b*b;}
inline addr        SQR(addr b)        {return b*b;}
inline float       SQR(float b)       {return b*b;}
inline double      SQR(double b)      {return b*b;}
inline long double SQR(long double b) {return b*b;}

inline int         CUBE(int b)         {return b*b*b;}
inline addr        CUBE(addr b)        {return b*b*b;}
inline float       CUBE(float b)       {return b*b*b;}
inline double      CUBE(double b)      {return b*b*b;}
inline long double CUBE(long double b) {return b*b*b;}

inline int         QUAD(int b)         {int         p = b*b; return p*p;}
inline addr        QUAD(addr b)        {int         p = b*b; return p*p;}
inline float       QUAD(float b)       {float       p = b*b; return p*p;}
inline double      QUAD(double b)      {double      p = b*b; return p*p;}
inline long double QUAD(long double b) {long double p = b*b; return p*p;}

// Function template is tricker than macro on account of std::pow signatures.
#define xp(b,e) ((e)==2? SQR(b) : ((e)==3? CUBE(b) : ((e)==4? QUAD(b) : pow(b,e))))
