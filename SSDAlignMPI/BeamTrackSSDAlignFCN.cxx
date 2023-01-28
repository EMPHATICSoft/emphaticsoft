////////////////////////////////////////////////////////////////////////
/// \brief   Fitting for the SSD Sensors 
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

#include "BeamTrackSSDAlignFCN.h"
#include "myMPIUtils.h"

extern emph::rbal::SSDAlignParams* emph::rbal::SSDAlignParams::instancePtr;

namespace emph {
  namespace rbal {
    
    BeamTrackSSDAlignFCN::BeamTrackSSDAlignFCN( const std::string &aFitType, emph::rbal::BTAlignInput *DataIn) :
    myGeo(emph::rbal::BTAlignGeom::getInstance()),
    myParams(emph::rbal::SSDAlignParams::getInstance()),
    myBTIn(DataIn), 
    fFitType(aFitType),
    fUpLimForChiSq(1000.),
    fDebugIsOn(false), 
    fNCalls(0), 
    FCNBase(),
    fErrorDef(1.) 
    { ; }  
      
    double BeamTrackSSDAlignFCN::operator()(const std::vector<double> &parsM) const {
    
          // Get the rank of the process
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      fNCalls++;

      
      fResids.resize(parsM.size());
      std::vector<double> pars(parsM); // need to copy, as the value could (should not, if all goes well, i.e., Minuit determines its next 
                                       // move on the same value of the chisq, which is broadcast from rank 0  
      //
      // Broadcast, make sure all the worker node have the same Parameters (different set of events.. )
      //
      emph::rbal::broadcastFCNParams(pars);
      if ((fDebugIsOn) && (myRank == 1)) std::cerr << " BeamTrackSSDAlignFCN::operator, from rank " 
              << myRank << " after broadcast params, numPrams " << pars.size() << std::endl;
      if ((fDebugIsOn) && (myRank == 1)) std::cerr << " .... Geometry has been updated.. " << pars.size() << std::endl;
      //
      // Update the geometry 
      //
      size_t kP=0;
      for (std::vector<SSDAlignParam>::iterator it = myParams->ItBegin(); it != myParams->ItEnd(); it++, kP++) {
	it->SetValue(pars[kP]);
// 	itMyParam->UpdateGeom(); done in the SetValue method. 
      }
      //
      // Loop over all the tracks. 
      //
     emph::rbal::BeamTracks myBTrs; 
     size_t iEvt = 0; 
     for (std::vector<emph::rbal::BeamTrackCluster>::const_iterator it = myBTIn->cbegin(); it != myBTIn->cend(); it++) {
       if (!it->Keep()) continue; 
       emph::rbal::BeamTrack aTr;
       aTr.SetDoMigrad(false); // Minuit Minimize will do .. 
//       aTr.SetDebug((iEvt < 5));
       aTr.SetDebug(false);
       if (fFitType == std::string("2DY")) { 
         aTr.doFit2D('Y', it); 
       } else if (fFitType == std::string("2DX")) aTr.doFit2D('X', it); 
       else if (fFitType == std::string("3D")) aTr.doFit3D(it);
       myBTrs.AddBT(aTr);
       iEvt++;
     }
      if (fDebugIsOn) std::cerr << " .... from rank " 
        << myRank << " Did all the tracks fits.. " << iEvt << " of them " << std::endl;
     
     //
     // Collect the mean Chi-Sq, average...  
     //
     double chi2 = emph::rbal::MeanChiSqFromBTracks(myBTrs, fUpLimForChiSq); // We leave them be.. 
     if ((fDebugIsOn) && (myRank == 0)) 
       std::cerr << " .... Did all the tracks fits.. on rank 0, at least.. chi2 is  " << chi2 << std::endl;
     
     if ((myRank == 0) && fFOutHistory.is_open())  {
       fFOutHistory << " " << fNCalls << " " << chi2;
       for (size_t k = 0; k != parsM.size(); k++) {
         fFOutHistory << " " << pars[k];
       } 
       fFOutHistory << std::endl;
     }  
     return chi2;
    }
    void BeamTrackSSDAlignFCN::OpenChiSqHistoryFile(const std::string &token) {
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      if (myRank != 0) return;
      std::string aName("./BeamTrackSSDAlignFCN_"); aName += token; aName += std::string("_V1.txt");
      fFOutHistory.open(aName.c_str());
      fFOutHistory << " nCalls chi2";
      for (std::vector<SSDAlignParam>::iterator it = myParams->ItBegin(); it != myParams->ItEnd(); it++)
        fFOutHistory << " " << it->Name();
      fFOutHistory << std::endl;
    }
    void BeamTrackSSDAlignFCN::CloseChiSqHistoryFile() {
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      if (myRank != 0) return;
      if (fFOutHistory.is_open()) fFOutHistory.close();
    }
  }
}  
 
