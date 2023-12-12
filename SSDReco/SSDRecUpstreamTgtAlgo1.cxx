////////////////////////////////////////////////////////////////////////
/// \brief   Reconstruction of the beam track upstream of the magnet
///          For Phase1b, no fitting at all, as we have on 4 plane, 2View. 
///          Rely on the results of SSDAlign, via the volatileAligmentParams class 
///          
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
//

#include "SSDReco/SSDHotChannelList.h" 
#include "RecoBase/SSDCluster.h" 
#include "SSDReco/SSDRecUpstreamTgtAlgo1.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"

 using namespace emph;

namespace emph {
  namespace ssdr {
    SSDRecUpstreamTgtAlgo1::SSDRecUpstreamTgtAlgo1() :
      fOneOSqrt12(1.0/std::sqrt(12.)),
      fRunNum(0), fSubRunNum(0), fEvtNum(-1), fNEvents(0), fIsMC(false), fFilesAreOpen(false), 
      fPitch(0.06), fHalfWaferWidth(0.5*static_cast<int>(fNumStrips)*fPitch), fNominalMomentum(120.),
      fTokenJob("undef"),
      fEmgeo(nullptr), fEmVolAlP(emph::ssdr::VolatileAlignmentParams::getInstance()), fConvertCoord('A')
    
    {
     	  
	;   
    }
    
    SSDRecUpstreamTgtAlgo1::~SSDRecUpstreamTgtAlgo1()  {
      if (fFOut3D.is_open()) fFOut3D.close();
    } 
     
    size_t SSDRecUpstreamTgtAlgo1::recoXY(const art::Handle<std::vector<rb::SSDCluster> > aSSDClsPtr) {
      if (fEmVolAlP == nullptr) {
        std::cerr << " SSDRecUpstreamTgtAlgo1::recoXY, we require (for now, famous last word) the Volatile Alignment data. " << std::endl; 
	std::cerr << " .........  Fix that in the module itself, fatal, quit here and now.. " << std::endl; 
	exit(2); 
      }
      if (fEmgeo == nullptr) fEmgeo = fGeoService->Geo();

//      const bool debugIsOn = ((fEvtNum == 5) && (fSubRunNum == 10)); 
      const bool debugIsOn = ((fEvtNum == 1199) && (fSubRunNum == 10)); 
      fConvertCoord.SetDebugOn(debugIsOn); 
      if (debugIsOn) std::cerr << " SSDRecUpstreamTgtAlgo1::recoXY, spill " << fSubRunNum << " evt " << fEvtNum << std::endl;
      fTrXYs.clear();
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsX0;
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsY0;
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsX1;
      std::vector< std::vector<rb::SSDCluster>::const_iterator > mySSDClsPtrsY1;
      for(std::vector<rb::SSDCluster>::const_iterator itCl = aSSDClsPtr->cbegin(); itCl != aSSDClsPtr->cend(); itCl++) {
        if (itCl->Station() > 1) continue;
        if (itCl->View() == emph::geo::X_VIEW) {
	  if (itCl->Station() == 0) mySSDClsPtrsX0.push_back(itCl);
	  if (itCl->Station() == 1) mySSDClsPtrsX1.push_back(itCl);
	} else if (itCl->View() == emph::geo::Y_VIEW) {
	  if (itCl->Station() == 0) mySSDClsPtrsY0.push_back(itCl);
	  if (itCl->Station() == 1) mySSDClsPtrsY1.push_back(itCl);
        }
      }
      //
      // Require at least one cluster in each of the 2 X 2 view. 
      //
      if ((mySSDClsPtrsX0.size() == 0) || (mySSDClsPtrsY0.size() == 0) || 
          (mySSDClsPtrsY1.size() == 0) || (mySSDClsPtrsY1.size() == 0)) return 0;  
      //
      // Could be stored in this clas..in some initialization, 
      // post delclaration of the Volatile Alignment data.  For sake of clarity, we leave it here.. 
      // There is no dependence on Monte-Carlo vs data for the first two stations.. 
      //  
      const double angleRollX0 = fEmVolAlP->Roll(emph::geo::X_VIEW, 0, 0);
      const double angleRollCenterX0 = fEmVolAlP->RollCenter(emph::geo::X_VIEW, 0, 0);
      const double angleRollY0 = fEmVolAlP->Roll(emph::geo::Y_VIEW, 0, 0);
      const double angleRollCenterY0 = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 0, 0);
      const double angleRollX1 = fEmVolAlP->Roll(emph::geo::X_VIEW, 1, 1);
      const double angleRollCenterX1 = fEmVolAlP->RollCenter(emph::geo::X_VIEW, 1, 1);
      const double angleRollY1 = fEmVolAlP->Roll(emph::geo::Y_VIEW, 1, 1);
      const double angleRollCenterY1 = fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 1, 1);
      if (debugIsOn) std::cerr << " angleRollY1 " << angleRollY1 << " Center " << angleRollCenterY1 << std::endl; 
      const double pitchX0 = fEmVolAlP->Pitch(emph::geo::X_VIEW, 0, 0);
      const double pitchY0 = fEmVolAlP->Pitch(emph::geo::Y_VIEW, 0, 0);
      const double pitchX1 = fEmVolAlP->Pitch(emph::geo::X_VIEW, 1, 1);
      const double pitchY1 = fEmVolAlP->Pitch(emph::geo::Y_VIEW, 1, 1);
      const double deltaZX = (fEmVolAlP->ZPos(emph::geo::X_VIEW, 1) - fEmVolAlP->ZPos(emph::geo::X_VIEW, 0));
      const double deltaZY = (fEmVolAlP->ZPos(emph::geo::Y_VIEW, 1) - fEmVolAlP->ZPos(emph::geo::Y_VIEW, 0));
      const double errMultScatStation1 = fEmVolAlP->MultScatUncert(emph::geo::X_VIEW, 1, 0) * 120./fNominalMomentum;
      const double errMultScatStation1Sq = errMultScatStation1 * errMultScatStation1;
      const double dZErrRel = fEmVolAlP->ZPosErr()/deltaZX; // same for Y view.. 
      if (debugIsOn) std::cerr << " ... deltaZX " << deltaZX << " Y " << deltaZY << " +- " << dZErrRel << std::endl; 
      for(std::vector< std::vector<rb::SSDCluster>::const_iterator >::const_iterator ittClX0=mySSDClsPtrsX0.cbegin(); 
            ittClX0 != mySSDClsPtrsX0.cend(); ittClX0++) {
	std::vector<rb::SSDCluster>::const_iterator itClX0 = *ittClX0;  
	const double stripX0 = itClX0->WgtAvgStrip();
        const double rmsStrNX0 = std::max(0.1, itClX0->WgtRmsStrip())/fOneOSqrt12; // protect against some zero values for the RMS 
	const double stripX0ErrSq = (1.0/rmsStrNX0*rmsStrNX0)/12.; // Too convoluted..  just a guess!!!  Suspicious.. 
	const double X0Raw = fConvertCoord.getTrCoord(itClX0, fNominalMomentum).first;
	const double X0RawR = fConvertCoord.getTrCoordRoot(itClX0);
	if (debugIsOn) {
	    std::cerr << " .. At station 0, X view strip0 " << stripX0 << "  X0 Raw " << X0Raw << " X0-GeoMap " << X0RawR 
	              << " Average strip " << itClX0->AvgStrip() << " Min Strip " 
	              << itClX0->MinStrip() << " Max Strip " << itClX0->MaxStrip() <<  std::endl;
	    
	}
        for(std::vector< std::vector<rb::SSDCluster>::const_iterator >::const_iterator ittClY0=mySSDClsPtrsY0.cbegin(); 
            ittClY0 != mySSDClsPtrsY0.cend(); ittClY0++) {
	  std::vector<rb::SSDCluster>::const_iterator itClY0 = *ittClY0;  
	  const double stripY0 = itClY0->WgtAvgStrip();
          const double rmsStrNY0 = std::max(0.1, itClY0->WgtRmsStrip())/fOneOSqrt12; // protect against some zero values for the RMS 
	  const double stripY0ErrSq = (1.0/rmsStrNY0*rmsStrNY0)/12.; 
	  const double Y0Raw = fConvertCoord.getTrCoord(itClY0, fNominalMomentum).first; 
	  const double Y0RawR = fConvertCoord.getTrCoordRoot(itClY0); 
	  const double X0 = X0Raw - (Y0Raw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, 0, 0))*fEmVolAlP->Roll(emph::geo::X_VIEW, 0, 0);
	  const double Y0 = Y0Raw - (X0Raw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 0, 0))*fEmVolAlP->Roll(emph::geo::Y_VIEW, 0, 0);
	  if (debugIsOn) std::cerr << " .. At station 0, Y View strip0 " << stripY0 << "  Y0 Raw " 
	                           << Y0Raw << " X0-GeoMap " << Y0RawR << " X0 " << X0 << " Y0 " << Y0 << std::endl;
          for(std::vector< std::vector<rb::SSDCluster>::const_iterator >::const_iterator ittClX1=mySSDClsPtrsX1.cbegin(); 
            ittClX1 != mySSDClsPtrsX1.cend(); ittClX1++) {
	    std::vector<rb::SSDCluster>::const_iterator itClX1 = *ittClX1;  
	    const double stripX1 = itClX1->WgtAvgStrip();
            const double rmsStrNX1 = std::max(0.1, itClX1->WgtRmsStrip())/fOneOSqrt12; // protect against some zero values for the RMS 
	    const double stripX1ErrSq = (1.0/rmsStrNX1*rmsStrNX1)/12.; // Too convoluted..  just a guess!!!  Suspicious.. 
	    const double X1Raw = fConvertCoord.getTrCoord(itClX1, fNominalMomentum).first; 
	    const double X1RawR = fConvertCoord.getTrCoordRoot(itClX1);
	    if (debugIsOn) std::cerr << " .. At station 1, X view strip1 " << stripX1 
	                             << "  X1 Raw " << X1Raw << " X1-GeoMap " <<  X1RawR << std::endl;
            for(std::vector< std::vector<rb::SSDCluster>::const_iterator >::const_iterator ittClY1=mySSDClsPtrsY1.cbegin(); 
              ittClY1 != mySSDClsPtrsY1.cend(); ittClY1++) {
	      std::vector<rb::SSDCluster>::const_iterator itClY1 = *ittClY1;  
	      const double stripY1 = itClY1->WgtAvgStrip();
              const double rmsStrNY1 = std::max(0.1, itClY1->WgtRmsStrip())/fOneOSqrt12; // protect against some zero values for the RMS 
	      const double stripY1ErrSq = (1.0/rmsStrNY1*rmsStrNY1)/12.; // Too convoluted..  just a guess!!!  Suspicious.. 
	      const double Y1Raw = fConvertCoord.getTrCoord(itClY1, fNominalMomentum).first; 
	      const double Y1RawR = fConvertCoord.getTrCoordRoot(itClY1); 
	      const double X1 = X1Raw - (Y1Raw - fEmVolAlP->RollCenter(emph::geo::X_VIEW, 1, 0))*fEmVolAlP->Roll(emph::geo::X_VIEW, 1, 0);
	      const double Y1 = Y1Raw - (X1Raw - fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 1, 0))*fEmVolAlP->Roll(emph::geo::Y_VIEW, 1, 0);
	     if (debugIsOn) std::cerr << " .. At station 1, Y View strip1 " << stripY1 << "  Y1 Raw "  
	                           << Y1Raw << " Y1-GeoMap " <<  Y1RawR << " X1 " << X1 << " Y1 " << Y1 << " RollCenter Y " 
				   << fEmVolAlP->RollCenter(emph::geo::Y_VIEW, 1, 0) << " Roll X " 
				   << fEmVolAlP->Roll(emph::geo::X_VIEW, 1, 0) << std::endl;
	      // track params, slope and uncertainties. 
	      
	      const double XSlope = (X1 - X0) / deltaZX;
	      const double YSlope = (Y1 - Y0) /deltaZY;
	      const double XSlopeErrSq = ( stripX0ErrSq + stripX1ErrSq + errMultScatStation1Sq ) / (deltaZX * deltaZX) +
	                                   ( dZErrRel * dZErrRel); 
	      const double YSlopeErrSq = ( stripY0ErrSq + stripY1ErrSq + errMultScatStation1Sq ) / (deltaZY * deltaZY) +
	                                   ( dZErrRel * dZErrRel); 
	      
	      rb::BeamTrackAlgo1 aTr;
	      aTr.SetType(rb::tBeamTrType::XYONLY);
	      aTr.SetTrParams(X0, Y0,  XSlope, YSlope);
	      aTr.SetTrParamsErrs(std::sqrt(std::abs(stripX0ErrSq)), std::sqrt(std::abs(stripY0ErrSq)), 
	                         std::sqrt(std::abs(XSlopeErrSq)), std::sqrt(std::abs(YSlopeErrSq)), 0., 0.);
              aTr.SetTrItCls(itClX0, itClX1, itClY0, itClY1); 		 
	      fTrXYs.push_back(aTr);
	    } // on Clusters from Station 1, Y view
	  } // on Clusters from Station 1, X view
	} // on Clusters from Station 0, Y view
      } // on Clusters from Station 0, X view
//      if (debugIsOn) { std::cerr << " .... And quit enough for now, got one upstream track, enough! " << std::endl; exit(2); }
      return fTrXYs.size();
    }
    void SSDRecUpstreamTgtAlgo1::dumpXYInforR(int uFlag) const {
      if (!fFOut3D.is_open()) {  
        std::ostringstream aNameStrStr;
        aNameStrStr << "./SSDRecUpstreamTgtAlgo1_" << fTokenJob << "_" << fRunNum << "_V1.txt"; 
        std::string aNameStr(aNameStrStr.str());
        fFOut3D.open(aNameStr.c_str());
        fFOut3D << " spill evt nTrs iTr x0 x0Err y0 y0Err slx0 slx0Err sly0 sly0Err " << std::endl;
      }
      std::ostringstream headerStrStr; 
      headerStrStr << " " << fSubRunNum <<  " " << fEvtNum << " " << fTrXYs.size();
      int kTr=0;  
      for (std::vector<rb::BeamTrackAlgo1>::const_iterator  it = fTrXYs.cbegin();  it != fTrXYs.cend(); it++, kTr++ ) {
        if ((uFlag != INT_MAX) && (it->UserFlag() != uFlag )) continue;
        fFOut3D << headerStrStr.str() << " " << kTr << " " << it->XOffset() << " " << it->XOffsetErr();
	fFOut3D <<  " " << it->YOffset() << " " << it->YOffsetErr() << " " << it->XSlope() << " " << it->XSlopeErr();
	fFOut3D <<  " " << it->YSlope() << " " << it->YSlopeErr() << std::endl;
      }
    }
  } // ssdr
} // emph
