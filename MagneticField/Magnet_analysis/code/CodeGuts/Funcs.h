//
//  Funcs.hpp
//  L++
//
//  Created by Leo Bellantoni on 3/30/23.
//
#pragma once



#include <cfloat>
#include <algorithm>



enum OverlapInd {upStart=1, upStop=2, downStart=3, downStop=4};



#define rEQ(x,y) ( abs(x-y) < 1.0e-7 )



enum Lv3ax {x=1, y=2, z=3};     // because ra is FORTRAN indexed.



enum WhichMap {central=0, upstream=1, downstream=2};
