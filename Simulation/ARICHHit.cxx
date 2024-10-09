////////////////////////////////////////////////////////////////////////
/// \brief   Implemantation of ARICHHit
/// \author  wanly@bu.edu
/// \date
////////////////////////////////////////////////////////////////////////

#include <climits>
#include <cfloat>
#include <iostream>
#include <fstream>

#include "Simulation/ARICHHit.h"

namespace sim {

  ARICHHit::ARICHHit() :
  fBlock(INT_MAX), // See LeadGlass block numerology in the GDML file. 
  fAncestorTrackID(0), // A track id, set when a high track is entering the block  Not yet implemented. 
  fAncestorPDG(0),
  fAncestorMom(0.),
  fTime(0.),
  fEnergyDepo(0.),
  fdirx(0.), fdiry(0.),
  fposx(0.), fposy(0.)	
{

  }


  void ARICHHit::Reset() {
    fTime = 0.;
    fEnergyDepo = 0.; 
    fAncestorMom = 0.;
    fAncestorPDG = 0;
    fdirx=0.; fdiry=0.;
    fposx=0.; fposy=0.; 
}
 
	
  int ARICHHit::GetHiLo(){ // based on hilo-channel map found at EMPHATIC-doc-1407-v1 and later versions (V3)  
      
       std::vector<int> vnumber;
       std::string number = std::to_string(this->fBlock);
          
       for(auto& Integer: number){
       vnumber.push_back(static_cast<int>(Integer - '0'));
                }
       while(vnumber.size() < 4){
          vnumber.insert(vnumber.begin(),0); }

       int HiLo = 3*vnumber[0] + (-vnumber[1] + 2 ) ;     


      return HiLo;

    }

  int ARICHHit::GetChannel() // based on hilo-channel map found at EMPHATIC-doc-1407-v1 and later versions (V3)  
  {
       std::vector<int> vnumber;
       std::string number = std::to_string(this->fBlock);

       for(auto& Integer: number){
       vnumber.push_back(static_cast<int>(Integer - '0'));
                }
       while(vnumber.size() < 4){
          vnumber.insert(vnumber.begin(),0); }

	int channel = (7-vnumber[3]) + (vnumber[2] * 8);
	
	return channel; 
   } 


} //end namespace sim

