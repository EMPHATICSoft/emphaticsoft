////////////////////////////////////////////////////////////////////////
/// \brief   Station Point, Algo1 class
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>
#include "RecoBase/SSDStationPtAlgo1.h"

//
namespace rb {
  
  //----------------------------------------------------------------------
  
  SSDStationPtAlgo1::SSDStationPtAlgo1() :
     fType(rb::STNONE), fStationNum(INT_MAX), fUserFlag(INT_MAX), fX(DBL_MAX),  
     fY(DBL_MAX), fXErr(DBL_MAX), fYErr(DBL_MAX),
     fChiSq(DBL_MAX), fUorVPred(DBL_MAX), fUorVObsRaw(DBL_MAX), fUorVObsCorr(DBL_MAX)
     { ; } 

  void SSDStationPtAlgo1::Add(std::vector<rb::SSDCluster>::const_iterator itCl, double aClCorrectedMean, double aClErr) {
	  if (itCl->Station() != fStationNum) {
	    if (fStationNum == INT_MAX) {
	      std::cerr << 
	        " SSDStationPtAlgo1::Add, undefined station, please set the station number first, fatal error for now " << std::endl;
	      exit(2);
	    }
	    std::cerr << " SSDStationPtAlgo1::Add, inconsistent station number, currently " 
	              << fStationNum << " from SSDCluster ptr " <<  itCl->Station() << " fatal.. " << std::endl;
	    exit(2);
	  }
	  fClIds.push_back(itCl->ID()); fClViews.push_back(itCl->View());
	  fClAvs.push_back(aClCorrectedMean);  fClSigmas.push_back(aClErr);
	//
	  if (fClViews.size() == 2) {
	    if (fClViews[0] == fClViews[1]) {
	      std::cerr << " SSDStationPtAlgo1::Add, inconsistent set of views, both being  " << fClViews[1] 
	              << " fatal.. " << std::endl; exit(2);
	    }
	    if (((fClViews[0] == emph::geo::X_VIEW) && (fClViews[1] == emph::geo::Y_VIEW)) || 
	        ((fClViews[0] == emph::geo::Y_VIEW) && (fClViews[1] == emph::geo::X_VIEW))) fType = rb::STXYONLY; 
	    if (((fClViews[0] == emph::geo::X_VIEW) && (fClViews[1] == emph::geo::U_VIEW)) || 
	        ((fClViews[0] == emph::geo::U_VIEW) && (fClViews[1] == emph::geo::X_VIEW))) fType = rb::STXUONLY; 
	    if (((fClViews[0] == emph::geo::X_VIEW) && (fClViews[1] == emph::geo::W_VIEW)) || 
	        ((fClViews[0] == emph::geo::W_VIEW) && (fClViews[1] == emph::geo::X_VIEW))) fType = rb::STXWONLY; 
	    if (((fClViews[0] == emph::geo::Y_VIEW) && (fClViews[1] == emph::geo::U_VIEW)) || 
	        ((fClViews[0] == emph::geo::U_VIEW) && (fClViews[1] == emph::geo::Y_VIEW))) fType = rb::STYUONLY; 
	    if (((fClViews[0] == emph::geo::Y_VIEW) && (fClViews[1] == emph::geo::W_VIEW)) || 
	        ((fClViews[0] == emph::geo::W_VIEW) && (fClViews[1] == emph::geo::Y_VIEW))) fType = rb::STYWONLY; 
	 } else if (fClViews.size() == 3) {
	    if ((fClViews[0] == fClViews[2]) || (fClViews[1] == fClViews[2]))  {
	      std::cerr << " SSDStationPtAlgo1::Add, inconsistent set of views, adding   " << fClViews[2] 
	              << " to " << fClViews[0] << " and " << fClViews[1] << " fatal.. " << std::endl; exit(2);
	    }
	    if ((fClViews[0] == emph::geo::U_VIEW) || (fClViews[1] == emph::geo::U_VIEW) 
	        || (fClViews[2] == emph::geo::U_VIEW)) fType = rb::STXYU; 
	    if ((fClViews[0] == emph::geo::W_VIEW) || (fClViews[1] == emph::geo::W_VIEW) 
	        || (fClViews[2] == emph::geo::W_VIEW)) fType = rb::STXYW; 
	 }
   }
   std::ostream& operator<< (std::ostream& o, const rb::SSDStationPtAlgo1& h) {
     o << "SSD Station Point, Algo1 for Station  "<< h.Station() << ", type is ";
     switch (h.Type()) {
       case rb::STNONE : { o <<" undefined "; break; } 
       case rb::STXYU: { o << " 3 Clusters, X, Y, and U "; break; } 
       case rb::STXYW: { o << " 3 Clusters, X, Y, and W "; break; } 
       case rb::STXYONLY: { o << " 2 Clusters,  X and Y "; break; } 
       case rb::STXUONLY: { o << " 2 Clusters,  X and U "; break; } 
       case rb::STYUONLY: { o << " 2 Clusters,  Y and U "; break; } 
       case rb::STXWONLY: { o << " 2 Clusters,  X and W "; break; } 
       case rb::STYWONLY: { o << " 2 Clusters,  Y and W "; break; } 
     } 
     o << std::endl;
     o <<  " X = " << h.X() << " +- " << h.XErr() << " Y = " << h.Y() << " +- " <<  h.YErr();
     if (h.UorWPred() != DBL_MAX)
        o << " U or W info , pred. " << h.UorWPred() << " raw, obs " << h.UorWObsRaw() 
	  << " corr, obs " << h.UorWObsCorr() << " chiSq = " << h.ChiSq() << std::endl;
     o <<  " ...Based on " <<  h.NumClusters() << " SSD Clusters, which are: " << std::endl;
     for (size_t k=0; k != h.NumClusters(); k++) { 
       o << " Cluster ID " << h.ClusterID(k) << " View " << h.ClusterView(k)  
         << " mean strip coord " << h.ClusterCorrMeasurement(k) << " +- " << h.ClusterMeasurementError(k) << std::endl;
     }   
     o << std::endl;
     return o;
  }
}  // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
