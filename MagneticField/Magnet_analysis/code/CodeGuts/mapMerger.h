//  Created by Leo Bellantoni on 13 Sep 2023.



#ifndef mergeMaps_hpp
#define mergeMaps_hpp





#include <iostream>
using std::ofstream, std::ifstream;
using std::ios;
#include "intsNabs.h"
#include "minmax.h"
#include "Funcs.h"
#include "FieldMap.h"





class mapMerger {
public:
    mapMerger(FieldMap* upMap_, FieldMap* cnMap_, FieldMap* dnMap_,
              ra<double>* overlapZ_,
              string headerFileName_, string outputMapName_);

    void mergeMaps(bool copySpacing, double halfSpanX,double halfSpanY,
                          double lowestZout,double highestZout,
                          double stepX,double stepY,double stepZ);



private:
    FieldMap* upMap; FieldMap* cnMap; FieldMap* dnMap;
    ra<double> overlapZ;
    string headerFileName, outputMapName;
};




#endif
