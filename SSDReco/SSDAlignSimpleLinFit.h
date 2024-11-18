////////////////////////////////////////////////////////////////////////
/// \brief   2D simple Linear regression, to avoid dragging ROOT into this, as this is not polynomial  fit 
///          No Minuit is necessary. 
///  Note we break the convention of pre-fixing claaa member.. Also, everything is public. 
//   This is pretty much c-like code, std::vector are fixed size, of double.. 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDALIGNSIMPLELINFIT_H
#define SSDALIGNSIMPLELINFIT_H

#include <vector>
#include <stdint.h>
#include <iostream>


namespace emph { 
  namespace ssdr {
 
    class SSDAlignSimpleLinFit {
    
      public:
      
	  SSDAlignSimpleLinFit();
	  int ndgf; // number of degrees of freedom Other data member are self explicit.
	  double offset;
	  double slope;
	  double sigmaOffset;
	  double sigmaSlope;
	  double covOffsetSlope; 
	  double chiSq;
	  std::vector<double> resids; // The residuals
	  
      private:
	  //
	  // The fixex Z coordinate for the fit. 
	  //
	  std::vector<double> fZCoords;
	  
      public:
      	  
	  inline void SetZCoords(std::vector<double> v) { fZCoords = v; } 
	  
          void  fitLin(bool align0to4, const std::vector<double> &t, const std::vector<double> &sigT) ; 
	  double GetTrOffInit(const std::vector<double> &t);
	  double GetTrSlInit(const std::vector<double> &t);
    
      };
   }
}      
#endif
