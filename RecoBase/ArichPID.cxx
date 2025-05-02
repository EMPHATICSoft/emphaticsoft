#include "StandardRecord/SRPID.h"
#include "RecoBase/ArichPID.h"

namespace rb {

   ArichPID::ArichPID(){
	this->setdefault();	
   }

 /* ArichPID::ArichPID(float* scores[5]){
       pid = caf::SRPID(scores[5]);
   }
 */

   ArichPID::~ArichPID(){}

   void ArichPID::setdefault(){
        Track_ID = -1;
        ARing_ID = -1;
   }
  
  std::ostream& operator << (std::ostream& o, const ArichPID& h){
  return o; 	
  }

}


