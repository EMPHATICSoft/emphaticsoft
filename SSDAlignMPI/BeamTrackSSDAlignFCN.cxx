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

#include <mpi.h>
#include "BeamTrackSSDAlignFCN.h"
#include "myMPIUtils.h"

extern emph::rbal::SSDAlignParams* emph::rbal::SSDAlignParams::instancePtr;

namespace emph {
  namespace rbal {
    
    BeamTrackSSDAlignFCN::BeamTrackSSDAlignFCN( const std::string &aFitType, emph::rbal::BTAlignInput *DataIn) :
    fIsPhase1c(false),
    myGeo(emph::rbal::BTAlignGeom::getInstance()),
    myGeo1c(emph::rbal::BTAlignGeom1c::getInstance()),
    myParams(emph::rbal::SSDAlignParams::getInstance()),
    myBTIn(DataIn), 
    fFitType(aFitType), fIsMC(false), fNoMagnet(false),
    fStrictSt6(true), fBeamConstraint(false), fAlignMode(true), fDoAntiPencilBeam(false), fSelectedView('A'), 
    fDoAllowLongShiftByStation(false),
    fAssumedSlopeSigma(1.0),  
    fBeamBetaFunctionY(1357.), fBeamBetaFunctionX(377.), // in cm 
    fBeamAlphaFunctionY(-25.11), fBeamAlphaFunctionX(-8.63), // in cm 
    fBeamGammaX((1.0 + fBeamAlphaFunctionX*fBeamAlphaFunctionX)/fBeamBetaFunctionX), // See Twiss Param definition, TRANSPORT manual, p. 39 
    fBeamGammaY((1.0 + fBeamAlphaFunctionY*fBeamAlphaFunctionY)/fBeamBetaFunctionY),
    fSoftLimitDoubleSensorCrack( 1.0e-6),
    fNominalMomentum(29.6), // For the 30 GeV exercise.
    fSoftLimits(false),
    fUpLimForChiSq(1000.),
    fDebugIsOn(false), fDumpBeamTracksForR(false),
    fNCalls(0), 
    FCNBase(),
    fErrorDef(1.),
    fNameForBeamTracks("none"), fIsOK(0), fGapValues(6, DBL_MAX) 
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
      fResids.resize(parsM.size()); // Why ????? We could have changed our mind in defining residuals... 
      std::vector<double> pars(parsM); // need to copy, as the value could (should not, if all goes well, i.e., Minuit determines its next 
                                       // move on the same value of the chisq, which is broadcast from rank 0  
      //
      // Broadcast, make sure all the worker node have the same Parameters (different set of events.. )
      //
      emph::rbal::broadcastFCNParams(pars);
      if ((fDebugIsOn) && (myRank == 0)) std::cerr 
        << " BeamTrackSSDAlignFCN::operator, from rank 0 after broadcast params, numPrams " << pars.size() << std::endl;
//      if ((fDebugIsOn) && (myRank == 0)) std::cerr << " .... Geometry has been updated.. " << pars.size() << std::endl;
      //
      // Update the geometry. we do assume the number of Minuit and those from our own interface to the geometry 
      // are identical.  
      //
      int kP=0;
//      if (myRank == 0) std::cerr << " BeamTrackSSDAlignFCN::operator list of setting values.. nCalls " << fNCalls << std::endl;
      for (std::vector<SSDAlignParam>::iterator it = myParams->ItBegin(); it != myParams->ItEnd(); it++, kP++) {
//        if (myRank == 0) std::cerr << " param " << it->Name() << " new value " << pars[kP] << std::endl;  
	it->SetValue(pars[kP]);
      }
      
      if (fDoAllowLongShiftByStation) {
         if (!fIsPhase1c) myGeo1c->MoveZPosOfXUVByY();
	 else myGeo->MoveZPosOfXUVByY();
      }  
      double chiSoftLim = 0.;
      if (fSoftLimits) {
        chiSoftLim = this->SurveyConstraints(pars);
      }
      bool flagTracks = false;
      // If no information on the validity flags of the tracks, resize the associated vector. 
      if (fIsOK.size() == 0) {
        flagTracks = true; 
 	fIsOK = std::vector<bool>(myBTIn->GetNumEvts(), true);
      } 
      //
      // Loop over all the tracks. 
      //
//      if (!fIsPhase1c) { std::cerr << " BeamTrackSSDAlignFCN::operator Expecting phase1c, if not the case, stop here and now .. " << std::endl; exit(2); } 
//      if (fIsPhase1c) { std::cerr << " BeamTrackSSDAlignFCN::operator Expecting phase1c, We keep going .. " << std::endl; } 
     emph::rbal::BeamTracks myBTrs;
     myBTrs.SetForPhase1c(fIsPhase1c);
     myBTrs.SetSelectedView(fSelectedView); 
     size_t iEvt = 0;
     size_t kk=0;
     size_t nOKs = 0;
     if (fDebugIsOn) std::cerr << " BeamTrackSSDAlignFCN::operator, from rank " << myRank << " looping on " 
                               << myBTIn->GetNumEvts() << " events " << std::endl; 
     for (std::vector<emph::rbal::BeamTrackCluster>::const_iterator it = myBTIn->cbegin(); it != myBTIn->cend(); it++, kk++) {
       if (!it->Keep()) continue; 
       emph::rbal::BeamTrack aTr;
       aTr.SetSelectedView(fSelectedView);
       aTr.SetForPhase1c(fIsPhase1c);
       if ((!flagTracks) && (kk < fIsOK.size()) && (!fIsOK[kk])) continue;
       aTr.SetMCFlag(fIsMC);
       aTr.SetNoMagnet(fNoMagnet);
       aTr.SetDoMigrad(false); // Minuit Minimize will do .. 
       aTr.SetAlignMode(fAlignMode); 
       aTr.SetNominalMomentum(fNominalMomentum);
       aTr.SetNominalMomentumDisp(fNominalMomentumDisp);
//       aTr.SetDebug(fDebugIsOn && (iEvt < 5));
       aTr.SetDebug(false);
//       if ((kk % 100) == 0) std::cerr << " BeamTrackSSDAlignFCN::operator, at evt " << kk << std::endl;
//       aTr.SetDebug(true);
       if (fFitType == std::string("2DY")) { 
         aTr.doFit2D('Y', it); 
       } else if (fFitType == std::string("2DX")) aTr.doFit2D('X', it); 
       else if (fFitType == std::string("3D")) {
          aTr.SetDoMigrad(true); // a bit trickier if fitting momentum as well  
          aTr.doFit3D(it);
       }
       // if we are working on the sensors that are not covered by the 120 GeV pencil beam, let us not include 
       // the track that are too close to the pencil beam.  Rough cut.. 
        if (flagTracks) {
         if (fDoAntiPencilBeam && (fFitType == std::string("3D")) && (aTr.X0() < 0.) && (aTr.Y0() > 0.)) fIsOK[kk] = false;
         if (aTr.ChiSq() < 0.) {
           if ((myRank == 0) && (iEvt < 100) && fDebugIsOn)  {
              std::cerr << " Evt " << it->EvtNum() << " fit 3D failed, we will no longer try to fit this track. " << std::endl;
           }
           fIsOK[kk] = false;
	 } else if (aTr.ChiSq() > fUpLimForChiSq) { 
           if ((myRank < 10) && (iEvt < 100) && fDebugIsOn)  {
              std::cerr << " Rank " << myRank << " Evt/spill " << it->EvtNum() << " / " << it->Spill() << " fit 3D has too big of a chi-Sq, " 
	                <<  aTr.ChiSq() << " reject this track.  " << std::endl;
           }
	   fIsOK[kk] = false;
	 }
       } else {
         // a track become bad.. set a high value, but we will keep in the mean chiSq. 
         if (aTr.ChiSq() < 0.) continue; // we will not include this track in the tally, now.. 
       } 
       if ((myRank < 10) && (iEvt < 10) && fDebugIsOn)  {
          std::cerr << " spill " << it->Spill() << " Evt " << it->EvtNum() << " TrId " << it->TrId() 
	            << " x0 " << aTr.X0() << " x' " << aTr.Slx0() << 
	                        " y0 " << aTr.Y0() << " y' " << aTr.Sly0() <<  " chi2 " << aTr.ChiSq() << std::endl;
       }
       // stop as soon as we have the result for evt 5, spill 10 
//       if ((it->Spill() == 10) && (it->EvtNum() == 5)) {
//         std::cerr << " ... Did fit for evt 5, spill 10 , chi sq " << aTr.ChiSq() << " quit here and now .. " << std::endl; exit(2);
//       }
       if (!fIsOK[kk]) { iEvt++; continue; }
       nOKs++;
       myBTrs.AddBT(aTr);
       iEvt++;
     }
//      std::cerr << " BeamTrackSSDAlignFCN, operator(), on rank " << myRank << " MPI Barrier, nOKs " << nOKs << std::endl; 
      MPI_Barrier(MPI_COMM_WORLD);
//     if (fDebugIsOn && (myRank < 10))  std::cerr << " .... from rank " << myRank << " .. Did all the tracks fits.. " 
//                 << iEvt << " of them " << " successful " <<  nOKs << " check size of container " << myBTrs.size() << std::endl;
//     if ((myRank < 10))  std::cerr << " .... from rank " << myRank << " .. Did all the tracks fits.. " 
//                 << iEvt << " of them " << " successful " <<  nOKs << " check size of container " << myBTrs.size() << std::endl;
//
// Adding geometrical constraint for the double sensors
//
     double chiAddGeomCrack = 0.;
     const double maxStripPitch = 639*0.06; // Ugly should come from the geometry..  
     if ((!fIsPhase1c) && (fFitType == std::string("3D")) && (fSoftLimitDoubleSensorCrack > 1.0e-6)) {
       if (fDebugIsOn) std::cerr << " Starting SoftLimitDoubleSensorCrack, minimum gap " << fMinimumDoubleSensorCrack 
                                 << " chi sq fact " <<  fSoftLimitDoubleSensorCrack << std::endl; 
       const std::vector<char> views = {'X', 'Y', 'V' }; // the 3 view for Station for and 5; 
       for (size_t iView = 0; iView !=3; iView++) { 
         const char aView = views[iView];
         size_t numS = (!fIsPhase1c) ? myGeo->NumSensorsXorY() : myGeo1c->NumSensorsXorY();
	 if (aView == 'V') {
	    numS = myGeo->NumSensorsV(); 
	    numS = (!fIsPhase1c) ? myGeo->NumSensorsV() : myGeo1c->NumSensorsV();
	 }
	 const size_t kSeInit = (iView < 2) ? 4 : 0;
	 double tMin = 0; double tMax = 0.;
	 double trInner, trInnera, trInnerb, tr00, tr639;
         for (size_t kSe = kSeInit; kSe != numS; kSe++) {
	   const size_t kStDwn = (kSe - kSeInit) / 2 ;
	   const bool kSeEven = ((kSe % 2) == 0);
	   // Compute the position xt x = =y =0., the center of the detector.. 
	   if (aView == 'X') {
	    tr00 = myGeo->TrPos(aView, kSe);
	    if (!kSeEven) tr00 *= -1.0;
	    tr639 =  ( -1.0*maxStripPitch + myGeo->TrPos(aView, kSe));
	    if (!kSeEven) tr639 *= -1.0;
	  } else if (aView == 'Y') {
	    tr00 = myGeo->TrPos(aView, kSe);
	    if (kSeEven) tr00 *= -1.0; // kse ranges from 0 to 7, not the sensor index within the station. 
	    tr639 = ( -1.0*maxStripPitch + myGeo->TrPos(aView, kSe)); // Corrected, Sept 9, token NoTgt31Gev_ClSept_A1e_1o1_c10
	    if (kSeEven) tr639 *= -1.0; // kse ranges from 0 to 7, not the sensor index within the station. 
	  } else { // V is a.k.a. W 
	      if (kSeEven) tr00 =  -myGeo->TrPos(aView, kSe); // Tested, on run 1274, Oct 7
	      else  tr00 =  myGeo->TrPos(aView, kSe); 
	      if (kSeEven) tr639 = -maxStripPitch - myGeo->TrPos(aView, kSe); // Tested, on run 1274, Oct 7   
	      else  tr639 = maxStripPitch + myGeo->TrPos(aView, kSe);
	  }
	  trInner = (std::abs(tr00) < std::abs(tr639)) ? tr00 : tr639;
	  if (kSeEven) trInnera = trInner; else trInnerb = trInner;
	  if (!kSeEven) {
	    size_t gapIndex = iView + 3*kStDwn ;
	    fGapValues[gapIndex] = std::abs(trInnerb + trInnera);
	    if (fDebugIsOn) std::cerr << " View " << aView << " Station " <<  4 + (kSe - kSeInit)/2 
	                              << " trInnera " << trInnera << " opp. side " << trInnerb << " gap " << fGapValues[gapIndex] << std::endl;
	    if (fGapValues[gapIndex] < fMinimumDoubleSensorCrack) { 
	      const double badGap = fGapValues[gapIndex] - fMinimumDoubleSensorCrack;
	      chiAddGeomCrack += badGap*badGap*fSoftLimitDoubleSensorCrack;
	    }
	  }//    
	 } // on sensors
       } // on Views 
       if (fDebugIsOn) std::cerr << " Gap analysis,  final, adding " << chiAddGeomCrack << " to chiSq " << std::endl;
     } // Double Sensor geometrical constraint. 
     // 	
     // Adding beam Constraint
     //
     double chiAddBeam = 0.;
     if (fBeamConstraint) {
//       if ((fFitType == std::string("3D")) || (fFitType == std::string("2DX"))) chiAddBeam += this->BeamConstraintX(myBTrs);
//       if ((fFitType == std::string("3D")) || (fFitType == std::string("2DY"))) chiAddBeam += this->BeamConstraintY(myBTrs);
//       Only Y emittance.. 
         chiAddBeam += this->BeamConstraintEpsilY(myBTrs);
     }
//     chiAddBeam += this->SlopeConstraintAtStation0(myBTrs); To be reviewed.. 
//     if ((fDebugIsOn) && (myRank == 0)) 
//       std::cerr << " .... Applied  SlopeConstraintAtStation0, on rank 0, chiAd is   " << chiAddBeam << std::endl;
     
     // Collect the mean Chi-Sq, average...  
     //
     // 
//     double chi2 = emph::rbal::MeanChiSqFromBTracks(myBTrs, fUpLimForChiSq, (chiAddBeam + chiSoftLim) ); // We leave them be.. in the container.. 
      MPI_Barrier(MPI_COMM_WORLD);
     double chi2 = emph::rbal::MeanChiSqFromBTracks(myBTrs, DBL_MAX/2, (chiAddBeam + chiSoftLim + chiAddGeomCrack) ); // We include all the nasty contributions
      MPI_Barrier(MPI_COMM_WORLD);
     if ((fDebugIsOn) && (myRank == 0)) 
       std::cerr << " .... Did all the tracks fits.. on rank 0, at least.. chi2 is  " << chi2 << " fUpLimForChiSq " << fUpLimForChiSq << std::endl;
     
     if ((myRank == 0) && fFOutHistory.is_open())  {
       fFOutHistory << " " << fNCalls << " " << chi2;
       for (size_t k = 0; k != parsM.size(); k++) {
         fFOutHistory << " " << pars[k];
       } 
       for (size_t k = 0; k != fGapValues.size(); k++) {
         fFOutHistory << " " << fGapValues[k];
       } 
       fFOutHistory << std::endl;
     }
     if (fDumpBeamTracksForR) {
        emph::rbal::collectBeamTracks(myBTrs, false);
	if (myRank == 0) 
	  myBTrs.DumpForCVS(fNameForBeamTracks.c_str());
     }
      // We must synchronize here, as we will be collecting all the tracks..
      // Debugging the apparent infinite loop in Migrad.. 
      //
//      std::cerr << " BeamTrackSSDAlignFCN, operator(), on rank " << myRank << " before return, chiSq " << chi2 << std::endl; 
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
      std::vector<char> views = {'X', 'Y', 'V'};
      for (int kSt=4; kSt !=6; kSt++) {
        for (size_t iView=0; iView != views.size(); iView++) fFOutHistory << " Gap_" << kSt << "_" << views[iView];
      }
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
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      if (fDebugIsOn && (myRank == 0)) std::cerr << " BeamTrackSSDAlignFCN::SurveyConstraints.. on " << pars.size() << " parameters " << std::endl;
      //
      // Implement tying the double sensor together..Constraint on avoiding overlaps. The Gap has to be positive   
      //
      for (size_t kPar1=0; kPar1 != pars.size(); kPar1++) {
        std::vector<SSDAlignParam>::const_iterator it1 = myParams->It(kPar1);
	if (it1->Name() == std::string("TransShift_Y_4")) {
	  const double tryY4 = it1->Value();
	  for (size_t kPar2=0; kPar2 != pars.size(); kPar2++) {
            std::vector<SSDAlignParam>::const_iterator it2 = myParams->It(kPar2);
	     if (it2->Name() != std::string("TransShift_Y_5")) continue;
	     const double deltaTrY45 = tryY4 + it2->Value(); // check the sign here. 
	     if ((it2->Value() > 0.) && (tryY4 > 0.)) break; // no problem 
//	     std::cerr << " BeamTrackSSDAlignFCN::SurveyConstraints Gap Constraint, DeltaTr4   " 
//	               << tryY4 << " 5 " << it2->Value() << " Sum " << deltaTrY45 << std::endl;
	     if (deltaTrY45 < 0.) chiAdd +=  10.0*deltaTrY45*deltaTrY45; // arbitrary...  
	     break;
	  }
	  break;
	}
      }
//      std::cerr << " BeamTrackSSDAlignFCN::SurveyConstraints, Positivity of YGap, 45, adding  " << chiAdd << std::endl;
      for (size_t kPar1=0; kPar1 != pars.size(); kPar1++) {
        std::vector<SSDAlignParam>::const_iterator it1 = myParams->It(kPar1);
	if (it1->Name() == std::string("TransShift_Y_6")) {
	  const double tryY6 = it1->Value();
	  for (size_t kPar2=0; kPar2 != pars.size(); kPar2++) {
            std::vector<SSDAlignParam>::const_iterator it2 = myParams->It(kPar2);
	     if (it2->Name() != std::string("TransShift_Y_7")) continue;
	     const double deltaTrY67 = tryY6 + it2->Value(); // check the sign here. 
	     if (deltaTrY67 < 0.) chiAdd +=  10.0*deltaTrY67*deltaTrY67;  
	     break;
	  }
	  break;
	}
      }
       for (size_t kPar1=0; kPar1 != pars.size(); kPar1++) {
        std::vector<SSDAlignParam>::const_iterator it1 = myParams->It(kPar1);
	if (it1->Name() == std::string("TransShift_X_4")) {
	  const double tryX4 = it1->Value();
	  for (size_t kPar2=0; kPar2 != pars.size(); kPar2++) {
            std::vector<SSDAlignParam>::const_iterator it2 = myParams->It(kPar2);
	     if (it2->Name() != std::string("TransShift_X_5")) continue;
	     const double deltaTrX45 = tryX4 + it2->Value(); // check the sign here. 
	     if (deltaTrX45 < 0.) chiAdd +=  0.0001*deltaTrX45*deltaTrX45;  
	     break;
	  }
	  break;
	}
      }
//      std::cerr << " BeamTrackSSDAlignFCN::SurveyConstraints, Positivity of YGap, 45, adding  " << chiAdd << std::endl;
      for (size_t kPar1=0; kPar1 != pars.size(); kPar1++) {
        std::vector<SSDAlignParam>::const_iterator it1 = myParams->It(kPar1);
	if (it1->Name() == std::string("TransShift_X_6")) {
	  const double tryX6 = it1->Value();
	  for (size_t kPar2=0; kPar2 != pars.size(); kPar2++) {
            std::vector<SSDAlignParam>::const_iterator it2 = myParams->It(kPar2);
	     if (it2->Name() != std::string("TransShift_X_7")) continue;
	     const double deltaTrX67 = tryX6 + it2->Value(); // check the sign here. 
	     if (deltaTrX67 < 0.) chiAdd +=  0.0001*deltaTrX67*deltaTrX67;  
	     break;
	  }
	  break;
	}
      }
     // Survey constraints, as of February 2023. 
     // Comment this out, for MC studies.. 
     /*
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
      */
      if ((myRank == 0) && fDebugIsOn) std::cerr << " ........ Adding a total of " << chiAdd << " chi-square " << std::endl;
      return chiAdd;
    }
    double BeamTrackSSDAlignFCN::BeamConstraintY(const emph::rbal::BeamTracks &btrs) const {
     // Compute Sigma Y0 and sly0; 
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     double aaY0 = 0.; double aa2Y0=0.;  double aaSly0 = 0.; double aa2Sly0=0.;
     // Compute the mean slope first..  No cuts.. 
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) 
       aaSly0 +=it->Sly0();
     const double meanSly = aaSly0/static_cast<int>(btrs.size());
     aaSly0 = 0.;
     int nAcc = 0;
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
       const double yy = it->Y0(); const double slyy = it->Sly0();
       if (it->ChiSq() > 200.) continue;     // Added, for MC studies.. Also should be valid for data....   
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
     if (fDebugIsOn && (myRank == 0)) {
       std::cerr << " BeamTrackSSDAlignFCN::BeamConstraintY on " << btrs.size() << " tracks " << std::endl;
       std::cerr << " ............. sigmaY " << std::sqrt(sigmaYSq) << " sigmaSly " 
                 << sigmaSly <<  " betaY " << fBeamBetaFunctionY << " epsil " << epsil << " sigmaSlyPred " << sigmaSlyPred << " chiAdd " << chiAdd << std::endl;
     }
     return chiAdd;
    }
    // Same in the X view 
    double BeamTrackSSDAlignFCN::BeamConstraintX(const emph::rbal::BeamTracks  &btrs) const {
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     // Compute Sigma X0 and slx0; 
     /*
     * Winter 2023 code, O.K., but we don't have good values for the Twiss params of our beams!.. 
     double aaX0 = 0.; double aa2X0=0.;  double aaSlx0 = 0.; double aa2Slx0=0.;
     // Compute the mean slope first..  No cuts.. 
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) 
       aaSlx0 +=it->Slx0();
     const double meanSlx = aaSlx0/static_cast<int>(btrs.size());
     aaSlx0 = 0.;
     int nAcc = 0;
     double avMom = 0; double avMom2 = 0.;
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
       const double xx = it->X0(); const double slxx = it->Slx0();
       if (it->ChiSq() > 50.) continue;     // Added, for MC studies.. Also should be valid for data....   
       if (std::abs(slxx - meanSlx) > 0.0015) continue;  // we ignore multiple scattering or interaction in the Silcon wafer.  Valid onlx at 120 GeV 
       nAcc++;
       aaX0 += xx; aa2X0 += xx*xx; aaSlx0 += slxx; aa2Slx0 += slxx*slxx; 
       if ((std::abs(fNominalMomentum) > 0.1) && (it->Mom() != DBL_MAX))) {
         avMom += it->Mom(); avMom2 += it->Mom() * it->Mom(); 
       }
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
     */
     // Obsolete as well, we simply fix the momentum in the BT fits. 
     double chiAdd = 0.;
     if (std::abs(fNominalMomentum) > 0.1) { 
       double avMom = 0; double avMom2 = 0.; int nMoms = 0; double varMom = DBL_MAX;
       for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
         if (it->Mom() != DBL_MAX) {
           avMom += it->Mom(); avMom2 += it->Mom() * it->Mom(); nMoms++;
         }
       }
       if (nMoms > 5) {
         avMom /= nMoms; 
         varMom = (avMom2 - nMoms * avMom *avMom)/(nMoms *(nMoms-1));
	 //
	 // April 28 : we do not use the variance in the additional chi-square, because, if SSD sensors are misaligned, the 
	 // the momentum resolution will decrease.. So, let us a fixed variance 
	 const double varMomFixed = 0.01 * fNominalMomentum * fNominalMomentum;
         chiAdd += (fNominalMomentum - avMom) * (fNominalMomentum - avMom)/varMomFixed;
       }
//       if (fDebugIsOn && (myRank == 0)) {
       if ((myRank == 0)) {
         std::cerr << " BeamTrackSSDAlignFCN::BeamConstraintX on " << btrs.size() << " tracks " 
	           <<  " Nominal momentum " << fNominalMomentum << " average  " 
		    << avMom << " sigma " << std::sqrt(varMom) << " chiAdd " << chiAdd << std::endl;
       }
     }  
     return chiAdd;
    }
    //
    // However, we definitly want to avoid the broadening/split of average Beam Track slopes, which do occur if we have both Rolls 
    // transverse offsets. 
    double BeamTrackSSDAlignFCN::SlopeConstraintAtStation0(const emph::rbal::BeamTracks  &btrs) const {
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
      if (btrs.size() < 3) {
        if ((fDebugIsOn) && (myRank == 0)) {
           std::cerr << " BeamTrackSSDAlignFCN::SlopeConstraintAtStation0 on " << btrs.size() 
	             << " tracks Not enough tracks, nothing to add ... " << std::endl;
        }
        return 0.;
      }
      double slxAv = 0.; double slyAv = 0.; double slxSq = 0.; double slySq = 0.;
      for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
        const double slx = it->Slx0(); const double sly = it->Sly0(); 
        slxAv += slx; slyAv += sly;  slxSq += slx*slx; slySq += sly*sly;
      }
      slxAv /= btrs.size(); slyAv /= btrs.size();
      const double sigSlxSq = (slxSq - slxAv*slxAv*btrs.size())/(btrs.size() - 1);
      const double sigSlySq = (slySq - slyAv*slyAv*btrs.size())/(btrs.size() - 1);
      double chiAdd = (sigSlxSq + sigSlySq)/(fAssumedSlopeSigma*fAssumedSlopeSigma);
      if ((fDebugIsOn) && (myRank == 0)) {
         std::cerr << " BeamTrackSSDAlignFCN::SlopeConstraintAtStation0 on " << btrs.size() << " tracks " 
	           <<  " Expected Slope Sigma " << fAssumedSlopeSigma << " average  X sigma " 
		    << std::sqrt(std::abs(sigSlxSq)) << " in Y " << std::sqrt(std::abs(sigSlySq)) << " chiAdd " << chiAdd << std::endl;
      }
      return chiAdd;
    }
    double BeamTrackSSDAlignFCN::BeamConstraintEpsilY(const emph::rbal::BeamTracks  &btrs) const {
      // Not a true emittance, simply the variance of y*y' 
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
     double aaYYPrime = 0.; double aa2YYPrime=0.;    
     int nAcc = 0; 
     for (std::vector<emph::rbal::BeamTrack>::const_iterator it = btrs.cbegin(); it != btrs.cend(); it++) {
       if (std::isnan(it->ChiSq())) continue;
       const double yy = it->Y0(); const double slyy = 1.0e3*it->Sly0(); // now in miliradian. 
       if (it->ChiSq() > 5.0e6) continue;     // Phase1c empricial..   
       nAcc++; aaYYPrime +=  yy*slyy; aa2YYPrime += yy*yy*slyy*slyy; 
     }
     if (nAcc < 3) return 0.;
     const double avYYPrime = aaYYPrime/nAcc;
     const double epsilY = std::sqrt(std::abs(aa2YYPrime - nAcc*avYYPrime*avYYPrime)/(nAcc-1));
     const double chiAdd =  std::min(1.0e6, (epsilY - fExpectedEpsilY)*(epsilY - fExpectedEpsilY)/(fExpectedEpsilY*fExpectedEpsilY));
     if (fDebugIsOn) {
       std::cerr << " BeamTrackSSDAlignFCN::BeamConstraintEpsilY on " << btrs.size() << " tracks " << std::endl;
       std::cerr << " ............. epsil  " << epsilY  << " chiAdd " << chiAdd << std::endl;
     }
//
    return chiAdd;
    }
  }
}  
 
