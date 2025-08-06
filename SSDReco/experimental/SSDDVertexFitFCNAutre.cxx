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

#include "SSDReco/experimental/SSDVertexFitFCNAutre.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "Geometry/service/GeometryService.h"

// emph::ssdr::VolatileAlignmentParams* emph::ssdr::VolatileAlignmentParams::instancePtr; defined elsewhere 

namespace emph {
  namespace ssdr {
    
    SSDVertexFitFCNAutre::SSDVertexFitFCNAutre() :
    FCNBase(),
    fDebugIsOn(false), fErrorDef(1.0) {
     
//
    }
    SSDVertexFitFCNAutre::~SSDVertexFitFCNAutre() {
//      if (fFOutResids.is_open()) fFOutResids.close();
    } 
    double SSDVertexFitFCNAutre::operator()(const std::vector<double> &pars) const {
    
      assert(pars.size() == 3);
      if (fDebugIsOn) this->printInputData(); 
      assert(fZ2 != DBL_MAX);
      if (fDebugIsOn) std::cerr << std::endl <<  "SSDVertexFitFCNAutre::operator, Start, X = " << pars[0] << " Y " 
                                << pars[1] << " Z " <<  pars[2] << " Number of Dwn tracks " << fDataDwn.size() << " fZ2 " << fZ2 << std::endl;  
      if (fDataDwn.size() ==  0) return 2.0e10; // require at least 3 SSD Space Points 
      double chi2 = 0.;
      const double xv = pars[0]; const double yv = pars[1]; const double zv = pars[2];
      const double dz2 = zv - fZ2;
      const double xUpV =  fDataUpstr->XOffset() + zv*fDataUpstr->XSlope();
      const double yUpV =  fDataUpstr->YOffset() + zv*fDataUpstr->YSlope();
      const double xUpVErrSq =  fDataUpstr->XOffsetErr()*fDataUpstr->XOffsetErr() + 
                                   zv*fDataUpstr->XSlopeErr()*zv*fDataUpstr->XSlopeErr();
      const double yUpVErrSq =  fDataUpstr->YOffsetErr()*fDataUpstr->YOffsetErr() + 
                                   zv*fDataUpstr->YSlopeErr()*zv*fDataUpstr->YSlopeErr(); // over estimate, no covariance 
      if (fDebugIsOn) std::cerr << " xUpV " << 	xUpV << " +- " << std::sqrt(xUpVErrSq) << " yUp "
                                << yUpV << " +-  " << std::sqrt(yUpVErrSq) << std::endl;	   
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
                                   dz2*fDataDwn[k]->XSlopeErr()*dz2*fDataDwn[k]->XSlopeErr();
          const double yDwnVErrSq =  fDataDwn[k]->YOffsetErr()*fDataDwn[k]->YOffsetErr() + 
                                   dz2*fDataDwn[k]->YSlopeErr()*dz2*fDataDwn[k]->YSlopeErr();
          denomDwn = xDwnVErrSq + yDwnVErrSq;
	  if (fDebugIsOn) std::cerr << "  ... No covariance matrix.. , err Xv " << std::sqrt(xDwnVErrSq) << " y " << std::sqrt(yDwnVErrSq) << std::endl;
	} else {
          const double xDwnVErrSq =  fDataDwn[k]->CovMatrix(0,0) + dz2*dz2*fDataDwn[k]->CovMatrix(1,1) 
	                                   + dz2*fDataDwn[k]->CovMatrix(0,1);
	  if (fDebugIsOn) std::cerr << " ... ... Cov00 " << fDataDwn[k]->CovMatrix(0,0) << " Cov11 " 
	                            << fDataDwn[k]->CovMatrix(1,1) << " Cov01 " << fDataDwn[k]->CovMatrix(0,1) << std::endl;				   
          const double yDwnVErrSq =  fDataDwn[k]->CovMatrix(2,2) + dz2*dz2*fDataDwn[k]->CovMatrix(3,3) 
	                                   + dz2*fDataDwn[k]->CovMatrix(2,3);
	  if (fDebugIsOn) std::cerr << " ... ... Cov22 " << fDataDwn[k]->CovMatrix(2,2) << " Cov33 " 
	                            << fDataDwn[k]->CovMatrix(3,3) << " Cov23 " << fDataDwn[k]->CovMatrix(2,3) << std::endl;				   
          denomDwn = std::abs(xDwnVErrSq + yDwnVErrSq); // Ignore the weak correlation between X and Y. 
	  if (fDebugIsOn) std::cerr << "  ... With covariance matrix.. , err Xv " << std::sqrt(xDwnVErrSq) 
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
    void SSDVertexFitFCNAutre::OpenOutResids(const std::string &fNameStr) {
      if (fFOutResids.is_open()) return;
      fFOutResids.open(fNameStr.c_str());
      fFOutResids << " spill evt chiSq";
      for (size_t k=0; k != 4; k++)  fFOutResids << " residXSt" << k;
      for (size_t k=0; k != 4; k++)  fFOutResids << " residYSt" << k;
      fFOutResids << " " << std::endl;   
    } 
    void SSDVertexFitFCNAutre::SpitOutResids(int spill, int evtNum) {
      if (!fFOutResids.is_open()) return;
      fFOutResids << " " << spill << " " << evtNum << " " << fLastChi2;
      for (size_t k=0; k != fResids.size(); k++) 
         fFOutResids << " " << fResids[k];
      fFOutResids << " " << std::endl;   
    }
/*/
    void SSDVertexFitFCNAutre::printInputData() const { 
      std::cerr << " SSDVertexFitFCNAutre::printInputData, number of Dwonstream data points " << fDataDwn.size() << std::endl;
      std::cerr << " ... Usptream (Beam) track X, " << fDataUpstr->XOffset() << " +- " << fDataUpstr->XOffsetErr() << " x' " 
                                                       << 1.0e3*fDataUpstr->XSlope() << " +- " << 1.0e3*fDataUpstr->XSlopeErr() << " (mrad)" << std::endl;
      std::cerr << " ......................... Y, " << fDataUpstr->YOffset() << " +- " << fDataUpstr->YOffsetErr() << " y' " 
                                                       << 1.0e3*fDataUpstr->YSlope() << " +- " << 1.0e3*fDataUpstr->YSlopeErr() << " (mrad)" <<  std::endl;
      std::cerr << " .... Downstream.... " << std::endl;
      for(size_t k=0; k != fDataDwn.size(); k++) {
        std::cerr << " ....  Track Id " << fDataDwn[k]->ID() << " X = " << fDataDwn[k]->XOffset() << " +- " 
	                                << fDataDwn[k]->XOffsetErr() << " x' = " << 1.0e3*fDataDwn[k]->XSlope() 
					<< " +- " << 1.0e3*fDataDwn[k]->XSlopeErr() << std::endl; 
        std::cerr << " ....  ......... Y = " << fDataDwn[k]->YOffset() << " +- " 
	                                << fDataDwn[k]->YOffsetErr() << " y' = " << 1.0e3*fDataDwn[k]->YSlope() 
					<< " +- " << 1.0e3*fDataDwn[k]->YSlopeErr() << std::endl;
        if (fDataDwn[k]->CovMatrix(0, 0) != DBL_MAX) {
	  std::cerr << " ....  Covariance Matrix, numParams  " <<  fDataDwn[k]->NumParams() << std::endl;
	  for (size_t i=0; i != fDataDwn[k]->NumParams(); i++) {
	    std::cerr << " .... .............. " ; 
	    for (size_t j=0; j != fDataDwn[k]->NumParams(); j++)  std::cerr << " " << fDataDwn[k]->CovMatrix(i,j);
	    std::cerr << std::endl;
	  }
	}
      }
      std::cerr << std::endl;
    }
  } // namespace 
} // Name space emph  
 
