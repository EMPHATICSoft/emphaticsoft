////////////////////////////////////////////////////////////////////////
/// \brief   2D simple Linear regression, to avoid dragging ROOT into this, as this is not polynomial  fit 
///          No Minuit is necessary. 
///  Note we break the convention of pre-fixing claaa member.. Also, everything is public. 
//   This is pretty much c-like code, std::vector are fixed size, of double.. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>

#include "SSDReco/SSDAlignSimpleLinFit.h"

 using namespace emph;

namespace emph {
  namespace ssdr {
  
     SSDAlignSimpleLinFit::SSDAlignSimpleLinFit() : 
       ndgf(-1), offset(DBL_MAX), slope(DBL_MAX), sigmaOffset(0.), sigmaSlope(0.), covOffsetSlope(0.), chiSq(DBL_MAX), resids(0), 
       fZCoords(6, 0.) { ; } 
     
       void SSDAlignSimpleLinFit::fitLin(bool align0to4, const std::vector<double> &ts, const std::vector<double> &sigTs) { 
       //
       // See good old Numerical Recipes, chapter 15.2, subroutine fit. (yeah, a subroutine!) 
       //
       if (ts.size() != sigTs.size()) {
         std::cerr << " SSDAlignSimpleLinFit::fitLin mismatch of length between measurement values ( " 
                  << ts.size() << " )  and uncertainties ( " << sigTs.size() << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
       }
       if ((!align0to4) && (ts.size() != fZCoords.size())) {
         std::cerr << " SSDAlignSimpleLinFit::fitLin  mismatch of length between measurement values ( " 
                  << ts.size() << " )  Z positions of planes ( " << fZCoords.size() << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
       }
       if ((align0to4) && (ts.size() != fZCoords.size() -1)) {
         std::cerr << " SSDAlignSimpleLinFit::fitLin mismatch of length between measurement values ( " 
                  << ts.size() << " )  Z positions of planes ( " << fZCoords.size() -1 << " ) .. Fatal, stop here and now " << std::endl;
		  exit(2);  
       }
       this->ndgf = static_cast<int> (ts.size() - 2);
       std::vector<double> ws(ts);
       double sx = 0.; double sy = 0.; double ss = 0.;
       for (size_t k=0; k != ts.size(); k++) {
         ws[k] = 1.0/(sigTs[k]*sigTs[k]);
         sx += fZCoords[k]*ws[k]; sy += ts[k]*ws[k]; ss += ws[k];
       }
//       std::cerr << " SSDAlign2DXYAlgo1::fitLin...  Sum of the weights " << ss << " sx " << sx << " sy " << sy << std::endl; 
       const double sxoss = sx/ss;
       double b = 0.; double st2 = 0.;
       for (size_t k=0; k != ts.size(); k++) {
         const double tmpT = (fZCoords[k] - sxoss)/sigTs[k];
         st2 += tmpT*tmpT;
         b += tmpT*ts[k];
       }
//       std::cerr << " SSDAlign2DXYAlgo1::fitLin...  Sum of the weights " << ss << " st2 " << st2 << " b " << b << std::endl; 
       this->slope = b/st2;
       this->offset = (sy - sx*this->slope)/ss;
       this->sigmaOffset = std::sqrt((1. + (sx*sx)/(ss*st2))/ss);   
       this->sigmaSlope = std::sqrt(1./st2);
       this->covOffsetSlope = -1.0 * sx/(ss*st2);
       this->chiSq = 0.;
       for(size_t k=0; k != ts.size(); k++) {
         this->resids[k] = ts[k] - (this->offset + this->slope*fZCoords[k]);
         this->chiSq += (this->resids[k]*this->resids[k])*ws[k];
       }
     
//       std::cerr << " SSDAlign2DXYAlgo1::fitLin, done, offset " << this->offset << " +- " 
//                 <<  this->sigmaOffset << " chiSq " <<  this->chiSq << std::endl; 
     } // end of fitLin.. 
   } // namespace ssdr
 } // name space emph
