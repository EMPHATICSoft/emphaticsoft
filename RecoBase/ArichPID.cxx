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
      nhits =-1; 
  }
  

}


