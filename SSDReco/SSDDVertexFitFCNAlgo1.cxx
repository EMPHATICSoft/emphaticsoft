////////////////////////////////////////////////////////////////////////
/// \brief   Vertex Minuit2 fit FCN function Based on the signle upstream track and 
///          The downstream tracks.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>
#include <cassert>

#include "SSDReco/SSDVertexFitFCNAlgo1.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

// emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr; defined elsewhere 

namespace emph {
  namespace ssdr {
    
    SSDVertexFitFCNAlgo1::SSDVertexFitFCNAlgo1() :
    FCNBase(),
    fDebugIsOn(false), fErrorDef(1.0) {
     
//
    }
    SSDVertexFitFCNAlgo1::~SSDVertexFitFCNAlgo1() {
//      if (fFOutResids.is_open()) fFOutResids.close();
    } 
    double SSDVertexFitFCNAlgo1::operator()(const std::vector<double> &pars) const {
    
      assert(pars.size() == 3);
      if (fDebugIsOn) this->printInputData(); 
      assert(fZ2 != DBL_MAX);
      if (fDebugIsOn) std::cerr << "SSDVertexFitFCNAlgo1::operator, Start, X = " << pars[0] << " Y " 
                                << pars[1] << " Z " <<  pars[2] << " Number of Dwn tracks " << fDataDwn.size() << std::endl;  
      if (fDataDwn.size() ==  0) return 2.0e10; // require at least 3 SSD Space Points 
      double chi2 = 0.;
      const double xv = pars[0]; const double yv = pars[1]; const double zv = pars[2];
      const double dz2 = zv - fZ2;
      const double xUpV =  fDataUpstr->XOffset() + zv*fDataUpstr->XSlope();
      const double yUpV =  fDataUpstr->YOffset() + zv*fDataUpstr->YSlope();
      const double xUpVErrSq =  fDataUpstr->XOffsetErr()*fDataUpstr->XOffsetErr() + 
                                   zv*zv*fDataUpstr->XSlopeErr()*zv*zv*fDataUpstr->XSlopeErr();
      const double yUpVErrSq =  fDataUpstr->YOffsetErr()*fDataUpstr->YOffsetErr() + 
                                   zv*zv*fDataUpstr->YSlopeErr()*zv*zv*fDataUpstr->YSlopeErr(); // over estimate, no covariance 
      const double numUp = (xv-xUpV)*(xv-xUpV) + (yv-yUpV)*(yv-yUpV);
      const double denomUp = xUpVErrSq + yUpVErrSq;
      chi2 += numUp/denomUp;
      // Downstream of the interaction 
      for(size_t k=0; k != fDataDwn.size(); k++) {
        const double xDwnV =  fDataDwn[k]->XOffset() + dz2*fDataDwn[k]->XSlope();
        const double yDwnV =  fDataDwn[k]->YOffset() + dz2*fDataDwn[k]->YSlope();
        const double numDwn = (xv-xDwnV)*(xv-xDwnV) + (yv-yDwnV)*(yv-yDwnV);
        double denomDwn = DBL_MAX;
        if (fDebugIsOn) std::cerr << "  ... At Dwnstream Track " << fDataDwn[k]->ID() << " x at V "  << xDwnV << " y " << yDwnV << std::endl;
        if (fDataDwn[k]->CovMatrix(0, 0) == DBL_MAX) {
          const double xDwnVErrSq =  fDataDwn[k]->XOffsetErr()*fDataDwn[k]->XOffsetErr() + 
                                   dz2*dz2*fDataDwn[k]->XSlopeErr()*dz2*dz2*fDataDwn[k]->XSlopeErr();
          const double yDwnVErrSq =  fDataDwn[k]->YOffsetErr()*fDataDwn[k]->YOffsetErr() + 
                                   dz2*dz2*fDataDwn[k]->YSlopeErr()*dz2*dz2*fDataDwn[k]->YSlopeErr();
          denomDwn = xDwnVErrSq + yDwnVErrSq;
	  if (fDebugIsOn) std::cerr << "  No covariance matrix.. , err Xv " << std::sqrt(xDwnVErrSq) << " y " << std::sqrt(yDwnVErrSq) << std::endl;
	} else {
          const double xDwnVErrSq =  fDataDwn[k]->CovMatrix(0,0) + dz2*dz2*fDataDwn[k]->CovMatrix(1,1) 
	                                   + dz2*fDataDwn[k]->CovMatrix(0,1);
          const double yDwnVErrSq =  fDataDwn[k]->CovMatrix(2,2) + dz2*dz2*fDataDwn[k]->CovMatrix(3,3) 
	                                   + dz2*fDataDwn[k]->CovMatrix(2,3);
          denomDwn = std::abs(xDwnVErrSq + yDwnVErrSq); // Ignore the weak correlation between X and Y. 
	  if (fDebugIsOn) std::cerr << "  With ovariance matrix.. , err Xv " << std::sqrt(xDwnVErrSq) 
	                            << " y " << std::sqrt(yDwnVErrSq) << std::endl;
	}
	chi2 += numDwn / denomDwn;
      } 
      if (fDebugIsOn) {
        std::cerr << " ...  Current ChiSq value " << chi2 << std::endl;
//	std::cerr << " ... ... And quit for now... " << std::endl; exit(2);
      }    
      return chi2;
    }
/*    
    void SSDVertexFitFCNAlgo1::OpenOutResids(const std::string &fNameStr) {
      if (fFOutResids.is_open()) return;
      fFOutResids.open(fNameStr.c_str());
      fFOutResids << " spill evt chiSq";
      for (size_t k=0; k != 4; k++)  fFOutResids << " residXSt" << k;
      for (size_t k=0; k != 4; k++)  fFOutResids << " residYSt" << k;
      fFOutResids << " " << std::endl;   
    } 
    void SSDVertexFitFCNAlgo1::SpitOutResids(int spill, int evtNum) {
      if (!fFOutResids.is_open()) return;
      fFOutResids << " " << spill << " " << evtNum << " " << fLastChi2;
      for (size_t k=0; k != fResids.size(); k++) 
         fFOutResids << " " << fResids[k];
      fFOutResids << " " << std::endl;   
    }
/*/
    void SSDVertexFitFCNAlgo1::printInputData() const { 
      std::cerr << " SSDVertexFitFCNAlgo1::printInputData, number of Dwonstream data points " << fDataDwn.size() << std::endl;
      std::cerr << " ... Usptream (Beam) track X, x' " << fDataUpstr->XOffset() << " +- " << fDataUpstr->XOffsetErr()
                                                       << fDataUpstr->XSlope() << " +- " << fDataUpstr->XSlopeErr() << std::endl;
      std::cerr << " ......................... Y, y' " << fDataUpstr->YOffset() << " +- " << fDataUpstr->YOffsetErr()
                                                       << fDataUpstr->YSlope() << " +- " << fDataUpstr->YSlopeErr() << std::endl;
      std::cerr << " .... Downstream.... " << std::endl;
      for(size_t k=0; k != fDataDwn.size(); k++) {
        std::cerr << " ....  Track Id " << fDataDwn[k]->ID() << " X = " << fDataDwn[k]->XOffset() << " +- " 
	                                << fDataDwn[k]->XOffsetErr() << " x' = " << fDataDwn[k]->XSlope() 
					<< " +- " << fDataDwn[k]->XSlopeErr() << std::endl; 
        std::cerr << " ....  ......... Y = " << fDataDwn[k]->YOffset() << " +- " 
	                                << fDataDwn[k]->YOffsetErr() << " x' = " << fDataDwn[k]->YSlope() 
					<< " +- " << fDataDwn[k]->YSlopeErr() << std::endl;
        if (fDataDwn[k]->CovMatrix(0, 0) != DBL_MAX) {
	  std::cerr << " ....  Covariance Matrix " << std::endl;
	  for (size_t i=0; i!=5; i++) {
	    std::cerr << " .... .............. " ; 
	    for (size_t j=0; j!=5; j++)  std::cerr << " " << fDataDwn[k]->CovMatrix(i,j);
	    std::cerr << std::endl;
	  }
	}
      }
      std::cerr << std::endl;
    }
  } // namespace 
} // Name space emph  
 
