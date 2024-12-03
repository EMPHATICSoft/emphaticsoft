/*  L++ minmax
 *  Created by Leo Bellantoni on 12/21/06.
 *  Copyright 2006 URA. All rights reserved.
 *
 *  Minimum - maximum functions that are slightly more useful than fmaxf, fmax 
 *  and fmaxl, which allow only two arguments.  Don't use stdargs, as the calling
 *  syntax is ugly; it does not allow these called functions to figure out for 
 *  themselves how many arguments there are.  So just polymorph for up to a 8 
 *  arguments of the same type; argument promotion handles the case of mixed
 *  arguments.  Use compile time recursion to the std::min and std::max in
 *  stl_algobase.h.
 *
 *  Include macros for sign and aint (FORTRAN-style floor) here, and a 
 *  function to map arbitrary angles into the -pi to +pi range of atan2.
 *
 *  Implementation in header file as everything is templates and macros
 */
#pragma once


#include <algorithm>
using std::min, std::max;



template <typename T> T min(T a1, T a2, T a3) {
	T a = std::min(a1,a2);
	return ( a<a3 ? a : a3 );
}
template <typename T> T min(T a1, T a2, T a3, T a4) {
	T a = min(a1,a2,a3);
	return ( a<a4 ? a : a4 );
}
template <typename T> T min(T a1, T a2, T a3, T a4, T a5) {
	T a = min(a1,a2,a3,a4);
	return ( a<a5 ? a : a5 );
}
template <typename T> T min(T a1, T a2, T a3, T a4, T a5, T a6) {
	T a = min(a1,a2,a3,a4,a5);
	return ( a<a6 ? a : a6 );
}
template <typename T> T min(T a1, T a2, T a3, T a4, T a5, T a6, T a7) {
	T a = min(a1,a2,a3,a4,a5,a6);
	return ( a<a7 ? a : a7 );
}
template <typename T> T min(T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8) {
	T a = min(a1,a2,a3,a4,a5,a6,a7);
	return ( a<a8 ? a : a8 );
}

template <typename T> T max(T a1, T a2, T a3) {
	T a = max(a1,a2);
	return ( a>a3 ? a : a3 );
}
template <typename T> T max(T a1, T a2, T a3, T a4) {
	T a = max(a1,a2,a3);
	return ( a>a4 ? a : a4 );
}
template <typename T> T max(T a1, T a2, T a3, T a4, T a5) {
	T a = max(a1,a2,a3,a4);
	return ( a>a5 ? a : a5 );
}
template <typename T> T max(T a1, T a2, T a3, T a4, T a5, T a6) {
	T a = max(a1,a2,a3,a4,a5);
	return ( a>a6 ? a : a6 );
}
template <typename T> T max(T a1, T a2, T a3, T a4, T a5, T a6, T a7) {
	T a = max(a1,a2,a3,a4,a5,a6);
	return ( a>a7 ? a : a7 );
}
template <typename T> T max(T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8) {
	T a = max(a1,a2,a3,a4,a5,a6,a7);
	return ( a>a8 ? a : a8 );
}
