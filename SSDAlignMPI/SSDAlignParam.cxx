////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Beam tracks, either 2D or 3D  
///          Requires Minuit2 
///          Used by BTFit2D and BTFit3D  classes  
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

#include "SSDAlignParam.h"

namespace emph {
  namespace rbal {
  
    SSDAlignParam::SSDAlignParam() : 
      myGeo(emph::rbal::BTAlignGeom::getInstance()),
      fMinNum(-1),  // The Minuit parameter number 
      fName("Undef"), // full name 
      fView('?'), //
      fType(NONE), // TrShift, PitchCorr.. see above.
      fValue(DBL_MAX), 
      fLimits(-1000., 1000.) // All them will have limits.. angle are less than 2Pi, for instance. In practice, much smaller, one hopes. 
      { ; } 
      
     std::string SSDAlignParam::CheckAndComposeName() {
       if (fView == '?') {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, the view  is not defined,.. Quit here and now " << std::endl;
	   exit(2);
       }
       if ((fView != 'X') && (fView != 'Y') && (fView != 'U') && (fView != 'V') && (fView != 'W')) {
  	   std::cerr << " SSDAlignParam::ComposeName, problem, the view  must be either X, Y ,U, V or W ,.. Quit here and now " << std::endl;
	   exit(2);
       }
       if (fSensor == -1) {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, the view  is not defined,.. Quit here and now " << std::endl;
	   exit(2);
       }
       if ((fSensor ==  0) && ((fView == 'X') || (fView == 'Y')) && ((fType == TRSHIFT) || (fType == ZSHIFT))) {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, The X or Y view for station 0, sensor 0 ...." << std::endl
	             << ".... is not a free parameter, as it defines our reference frame. So,  Quit here and now " << std::endl;
	   exit(2);
       }
       if ((fSensor ==  7) && ((fView == 'X') || (fView == 'Y')) && ((fType == TRSHIFT) || (fType == ZSHIFT))) {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, The X or Y view for station 5, sensor 7, in each view ...." << std::endl
	             << ".... is not a free parameter, as it defines our reference frame. So,  Quit here and now " << std::endl;
	   exit(2);
       }
       if ((fSensor >= myGeo->NumSensorsXorY() ) && ((fView == 'X') || (fView == 'Y'))) {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, The X or Y have a maximum of 8 sensors, current value is " << fSensor <<  std::endl
	             << "....  So,  Quit here and now " << std::endl;
	   exit(2);
       }
       if ((fSensor >= myGeo->NumSensorsU()  ) && (fView == 'U')) {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, The U view has a maximum of 2 sensors, current value is " << fSensor <<  std::endl
	             << "....  So,  Quit here and now " << std::endl;
	   exit(2);
       }
       if (fView == 'W') fView = 'V'; 
       if ((fSensor >= myGeo->NumSensorsV()  ) && (fView == 'V')) {
 	   std::cerr << " SSDAlignParam::ComposeName, problem, The V view has a maximum of 4 sensors, current value is " << fSensor <<  std::endl
	             << "....  So,  Quit here and now " << std::endl;
	   exit(2);
       }
     
       switch (fType) {
         case NONE:
	   std::cerr << " SSDAlignParam::ComposeName, problem, the type is not defined,.. Quit here and now " << std::endl;
	   exit(2);
	 case TRSHIFT:
	    { fName = std::string("TransShift"); break; }
	 case ZSHIFT:
	     { fName = std::string("LongShift");break; }
	 case PITCHCORR: 
	   { fName = std::string("Tilt"); break; }
	 case ROLL: 
	   { fName = std::string("DeltaRoll"); break; }
	 case ZMAGC:      
	   { fName = std::string("LongMagC"); return fName; }
	 case KICKMAGN:      
	   { fName = std::string("KickMag");   return fName; }
       
         default:
	   std::cerr << " SSDAlignParam::ComposeName, problem, Internal logic, no valid type  " << std::endl;
	   exit(2);
       }
       fName += std::string("_") + std::string(1, fView);
       std::ostringstream aSensorStrStr; aSensorStrStr << fSensor; 
       fName += std::string("_") + aSensorStrStr.str();
       return fName;
    } 
    void SSDAlignParam::UpdateGeom () const {
    
         switch (fType) {
         case NONE:
	   std::cerr << " SSDAlignParam::UpdateGeom, problem, the type is not defined,.. Quit here and now " << std::endl;
	   exit(2);
	 case TRSHIFT:
	    { 
//	        std::cerr << " SSDAlignParam::UpdateGeom, fSensor " << fSensor << " New value " << fValue << std::endl;
	        myGeo->SetDeltaTr(fView, fSensor, fValue); return;
            }
	 case ZSHIFT:
	     { myGeo->SetDeltaZ(fView, fSensor, fValue); return;}
	 case PITCHCORR: 
	    { myGeo->SetDeltaPitchCorr(fView, fSensor, fValue); return; }
	 case ROLL: 
	   { myGeo->SetRoll(fView, fSensor, fValue); return; }
	 case ZMAGC:      
	   { myGeo->SetZCoordsMagnetCenter(fValue); return; }
	 case KICKMAGN:      
	   { myGeo->SetMagnetKick120GeV(fValue);   return; }
       
         default:
	   std::cerr << " SSDAlignParam::ComposeName, problem, Internal logic, no valid type  " << std::endl;
	   exit(2);
       }
   
    
    }   
  } // name space.. 
}     
