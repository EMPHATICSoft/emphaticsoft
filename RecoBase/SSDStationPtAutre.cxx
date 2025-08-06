////////////////////////////////////////////////////////////////////////
/// \brief   Station Point, Autre class
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>
#include <cfloat>
#include "RecoBase/SSDStationPtAutre.h"

//
namespace rb {
  
  //----------------------------------------------------------------------
  
  SSDStationPtAutre::SSDStationPtAutre() :
     fType(rb::STNONE), fId(INT_MAX), fStationNum(INT_MAX), fUserFlag(INT_MAX), fX(DBL_MAX),  
     fY(DBL_MAX), fXErr(DBL_MAX), fYErr(DBL_MAX),
     fChiSq(DBL_MAX), fUorVPred(DBL_MAX), fUorVObsRaw(DBL_MAX), fUorVObsCorr(DBL_MAX),
     fHasXOverlap(false), fHasYOverlap(false), fHasUorVOverlap(false)
     { ; } 

  void SSDStationPtAutre::Add(std::vector<rb::SSDCluster>::const_iterator itCl, double aClCorrectedMean, double aClErr) {
	  if (itCl->Station() != fStationNum) {
	    if (fStationNum == INT_MAX) {
	      std::cerr << 
	        " SSDStationPtAutre::Add, undefined station, please set the station number first, fatal error for now " << std::endl;
	      exit(2);
	    }
	    std::cerr << " SSDStationPtAutre::Add, inconsistent station number, currently " 
	              << fStationNum << " from SSDCluster ptr " <<  itCl->Station() << " fatal.. " << std::endl;
	    exit(2);
	  }
	  fItClusters.push_back(itCl); 
	  fClIds.push_back(itCl->ID()); fClViews.push_back(itCl->View());
	  fClSensorIds.push_back(itCl->Sensor());
	  fClAvs.push_back(aClCorrectedMean);  fClSigmas.push_back(aClErr);
	//
	  if (fClViews.size() == 2) {
	    if (fClViews[0] == fClViews[1]) {
	      std::cerr << " SSDStationPtAutre::Add, inconsistent set of views, both being  " << fClViews[1] 
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
	      std::cerr << " SSDStationPtAutre::Add, inconsistent set of views, adding   " << fClViews[2] 
	              << " to " << fClViews[0] << " and " << fClViews[1] << " fatal.. " << std::endl; exit(2);
	    }
	    if ((fClViews[0] == emph::geo::U_VIEW) || (fClViews[1] == emph::geo::U_VIEW) 
	        || (fClViews[2] == emph::geo::U_VIEW)) fType = rb::STXYU; 
	    if ((fClViews[0] == emph::geo::W_VIEW) || (fClViews[1] == emph::geo::W_VIEW) 
	        || (fClViews[2] == emph::geo::W_VIEW)) fType = rb::STXYW; 
	 }
   }
   void SSDStationPtAutre::ReScaleMultUncert(double multScatt120, double pOld, double pNew) const {  
     // pseudo const, we don't change the position, just the uncertainty. 
//	  if (fStationNum == 5)
//	   std::cerr << " SSDStationPtAutre::ReScaleMultUncert, Station 5 multScatt120 " << multScatt120  
//	             << " xErr " << fXErr << std::endl;
	  const double ratioPOld120Sq = (120./pOld)*(120./pOld);
	  const double ratioPNew120Sq = (120./pNew)*(120./pNew);
	  const double xErrSQOther = fXErr*fXErr - multScatt120*multScatt120*ratioPOld120Sq;
	  const double aXErrOld = fXErr; 
	  if (xErrSQOther < 0.) {
	    std::cerr << " ReScaleMultUncert::SSDStationPtAutre, Station " << fStationNum<< " pOld " << pOld 
	             << " pNew " << pNew << " ratioPOldSq " << ratioPOld120Sq << " new " 
		     << ratioPNew120Sq << " xErrSQOther " << xErrSQOther << " mulScatt120 " <<  multScatt120
		     << " OlXErr " << aXErrOld << std::endl;
	    std::cerr << " SSDStationPtAutre::ReScaleMultUncert Problem, multScatt too large for X view !!! pOld = " 
	              << pOld << " X Err " << fXErr << " Fatal, quit now.. " << std::endl; 
	    exit(2); 
	  }
	  fXErr  = std::sqrt(xErrSQOther + multScatt120*multScatt120*ratioPNew120Sq); 
	  const double yErrSQOther = fYErr*fYErr - multScatt120*multScatt120*ratioPOld120Sq;
	  if (yErrSQOther < 0.) {
	    std::cerr << " SSDStationPtAutre::ReScaleMultUncert Problem, multScatt too large for Y view !!! pOld = " 
	              << pOld << " Y Err " << fYErr << " Fatal, quit now.. " << std::endl; 
	    exit(2); 
	  }
	  fYErr = std::sqrt(yErrSQOther + multScatt120*multScatt120*ratioPNew120Sq); 
   } 
   //
   //     
   std::ostream& operator<< (std::ostream& o, const rb::SSDStationPtAutre& h) {
     o << "SSD Station Point, Autre for Station  "<< h.Station() << ", type is ";
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
