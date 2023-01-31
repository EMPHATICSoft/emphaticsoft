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
    fStrictSt6(true), fBeamConstraint(false), 
    fBeamBetaFunctionY(1357.), fBeamBetaFunctionX(377.), // in cm 
    fBeamAlphaFunctionY(-25.11), fBeamAlphaFunctionX(-8.63), // in cm 
    fBeamGammaX((1.0 + fBeamAlphaFunctionX*fBeamAlphaFunctionX)/fBeamBetaFunctionX), // See Twiss Param definition, TRANSPORT manual, p. 39 
    fBeamGammaY((1.0 + fBeamAlphaFunctionY*fBeamAlphaFunctionY)/fBeamBetaFunctionY),
    fSoftLimits(false),
    fUpLimForChiSq(1000.),
    fDebugIsOn(false), 
    fNCalls(0), 
    FCNBase(),
    fErrorDef(1.) 
    {
      // We overwrite the Twiss parameters, based on e-mail from Mike Olander, Jan 31 
      const double l172 = 172.1455; // meters  
      const double lMT6WC2 = 179.1555; // meters  
      const double lEndM05 = 169.459; // meters  
      const double lMT61a = lEndM05 + 7.01; // where we think we are...  
      const double deltaL_MT6WC2_172 =  lMT6WC2 - l172; 
      const double dlRatio = (lMT61a - l172)/deltaL_MT6WC2_172;
      const double alphaX172 = -0.982; // Units? No units, pure number. 
      const double alphaY172 = -8.57208; 
      const double betaX172 = 16.514; // Mike says beta is in m 
      const double betaY172 = 160.1907; 
      const double alphaXMT6WC2 = -2.918992; // Units? 
      const double alphaYMT6WC2 = -11.83138; // Units? 
      const double betaXMT6WC2 = 47.61328; // meters
      const double betaYMT6WC2 = 303.21906; // m 
      //
      fBeamAlphaFunctionX = alphaX172 + dlRatio * (alphaXMT6WC2 -  alphaX172);
      fBeamAlphaFunctionY = alphaY172 + dlRatio * (alphaYMT6WC2 -  alphaY172);
      fBeamBetaFunctionX = betaX172 + dlRatio * (betaXMT6WC2 -  betaX172);
      fBeamBetaFunctionY = betaY172 + dlRatio * (betaYMT6WC2 -  betaY172);
      fBeamGammaX = (1.0 + fBeamAlphaFunctionX*fBeamAlphaFunctionX)/fBeamBetaFunctionX; // See Twiss Param definition, TRANSPORT manual, p. 39 
      fBeamGammaY = (1.0 + fBeamAlphaFunctionY*fBeamAlphaFunctionY)/fBeamBetaFunctionY;
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      if (myRank == 0) std::cerr << " Default Twiss parameters alphaX " 
         <<  fBeamAlphaFunctionX << " betaX " << fBeamBetaFunctionX << " alphaY "<< fBeamAlphaFunctionY << " betaY " << fBeamBetaFunctionY << std::endl;
    }  
      
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
      // Update the geometry. we do assume the number of Minuit and those from our own interface to the geometry 
      // are identical.  
      //
      int kP=0;
      for (std::vector<SSDAlignParam>::iterator it = myParams->ItBegin(); it != myParams->ItEnd(); it++, kP++) {
	it->SetValue(pars[kP]);
      }
      double chiSoftLim = 0.;
      if (fSoftLimits) {
        chiSoftLim = this->SurveyConstraints(pars);
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
     // Adding beam Constraint
     //
     double chiAddBeam = 0.;
     if (fBeamConstraint) {
       if ((fFitType == std::string("3D")) || (fFitType == std::string("2DX"))) chiAddBeam += this->BeamConstraintX(myBTrs);
       if ((fFitType == std::string("3D")) || (fFitType == std::string("2DY"))) chiAddBeam += this->BeamConstraintY(myBTrs);
     }
     
     // Collect the mean Chi-Sq, average...  
     //
     double chi2 = emph::rbal::MeanChiSqFromBTracks(myBTrs, fUpLimForChiSq, (chiAddBeam + chiSoftLim) ); // We leave them be.. in the container.. 
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
    
    double BeamTrackSSDAlignFCN::SurveyConstraints(const std::vector<double> &pars) const { // Using Soft Limits. 
      double chiAdd = 0.;
      if (fDebugIsOn) std::cerr << " BeamTrackSSDAlignFCN::SurveyConstraints.. on " << pars.size() << " parameters " << std::endl;
      for (size_t kPar=0; kPar != pars.size(); kPar++) {
        std::vector<SSDAlignParam>::const_iterator it = myParams->It(kPar);
        const double deltaUp = std::abs(it->UpLimit() - pars[kPar]);
        const double deltaDown = std::abs(it->DownLimit() - pars[kPar]);
        const double sigma = 0.5*std::abs(it->UpLimit() - it->DownLimit());
        const double delta = (deltaUp > deltaDown) ? deltaDown : deltaUp;
        const double chiA = std::min(1.0e9, std::abs(std::exp(delta*delta/(sigma*sigma)) -1.)); // Gaussian probability, pseuso chi-square... Black magic.. 
        if (fDebugIsOn) std::cerr << " ........ for param " << it->Name() << " delta " << delta 
	   << " sigma " << sigma <<  " chi " << chiA << std::endl;
	chiAdd += chiA;
      }
      if (fDebugIsOn) std::cerr << " ........ Adding a total of " << chiAdd << " chi-square " << std::endl;
      return chiAdd;
    }
    double BeamTrackSSDAlignFCN::BeamConstraintY(const emph::rbal::BeamTracks &btrs) const {
     // Compute Sigma Y0 and sly0; 
     double aaY0 = 0.; double aa2Y0=0.;  double aaSly0 = 0.; double aa2Sly0=0.;
     // Compute the mean slope first..  No cuts.. 
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) 
       aaSly0 +=it->Sly0();
     const double meanSly = aaSly0/static_cast<int>(btrs.size());
     aaSly0 = 0.;
     int nAcc = 0;
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
       const double yy = it->Y0(); const double slyy = it->Sly0();
       if (std::abs(slyy - meanSly) > 0.0015) continue;  // we ignore multiple scattering or interaction in the Silcon wafer.  Valid only at 120 GeV 
       nAcc++;
       aaY0 += yy; aa2Y0 += yy*yy; aaSly0 += slyy; aa2Sly0 += slyy*slyy; 
     }
     if (nAcc < 3) return 1.0e9;
     const double meanSly0Acc = aaSly0/nAcc;
     const double meanY0Acc = aaY0/nAcc;
     const double sigmaYSq = std::abs(aa2Y0 - nAcc*meanY0Acc*meanY0Acc)/(nAcc-1);
     const double sigmaSly = 1.0e3 * std::sqrt(std::abs(aa2Sly0 - nAcc*meanSly0Acc*meanSly0Acc)/(nAcc-1)); // in mrad 
     const double epsil =  sigmaYSq / fBeamBetaFunctionY; // in mm mrad, as beta is in m. (1000 mm) 
     const double sigmaSlyPred = std::sqrt(std::abs(epsil*fBeamGammaY)); // also in mrad
     const double delta = sigmaSlyPred - sigmaSly;
     const double chiAdd =  std::min(1.0e9, std::abs(std::exp(delta*delta/(sigmaSlyPred*sigmaSlyPred)) -1.));
     if (fDebugIsOn) {
       std::cerr << " BeamTrackSSDAlignFCN::BeamConstraintY on " << btrs.size() << " tracks " << std::endl;
       std::cerr << " ............. sigmaY " << std::sqrt(sigmaYSq) << " sigmaSly " 
                 << sigmaSly <<  " betaY " << fBeamBetaFunctionY << " epsil " << epsil << " sigmaSlyPred " << sigmaSlyPred << " chiAdd " << chiAdd << std::endl;
     }
     return chiAdd;
    }
    // Same in the X view 
    double BeamTrackSSDAlignFCN::BeamConstraintX(const emph::rbal::BeamTracks  &btrs) const {
     // Compute Sigma X0 and slx0; 
     double aaX0 = 0.; double aa2X0=0.;  double aaSlx0 = 0.; double aa2Slx0=0.;
     // Compute the mean slope first..  No cuts.. 
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) 
       aaSlx0 +=it->Slx0();
     const double meanSlx = aaSlx0/static_cast<int>(btrs.size());
     aaSlx0 = 0.;
     int nAcc = 0;
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
       const double xx = it->X0(); const double slxx = it->Slx0();
       if (std::abs(slxx - meanSlx) > 0.0015) continue;  // we ignore multiple scattering or interaction in the Silcon wafer.  Valid onlx at 120 GeV 
       nAcc++;
       aaX0 += xx; aa2X0 += xx*xx; aaSlx0 += slxx; aa2Slx0 += slxx*slxx; 
     }
     if (nAcc < 3) return 1.0e9;
     const double meanSlx0Acc = aaSlx0/nAcc;
     const double meanX0Acc = aaX0/nAcc;
     const double sigmaXSq = std::abs(aa2X0 - nAcc*meanX0Acc*meanX0Acc)/(nAcc-1);
     const double sigmaSlx = 1.0e3*std::sqrt(std::abs(aa2Slx0 - nAcc*meanSlx0Acc*meanSlx0Acc)/(nAcc-1));
     const double epsil =  sigmaXSq / fBeamBetaFunctionX;
     const double sigmaSlxPred = std::sqrt(std::abs(epsil*fBeamGammaX));
     const double delta = sigmaSlxPred - sigmaSlx;
     const double chiAdd =  std::min(1.0e9, std::abs(std::exp(delta*delta/(sigmaSlxPred*sigmaSlxPred)) -1.));
     if (fDebugIsOn) {
       std::cerr << " BeamTrackSSDAlignFCN::BeamConstraintX on " << btrs.size() << " tracks " << std::endl;
       std::cerr << " ............. sigmaX " << std::sqrt(sigmaXSq) << " sigmaSlx " 
                 << sigmaSlx << " epsil " << epsil << " sigmaSlxPred " << sigmaSlxPred << " chiAdd " << chiAdd << std::endl;
     }
     return chiAdd;
    }
    
  }
}  
 
