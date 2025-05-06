#include "StandardRecord/SRPID.h"
#include "RecoBase/PID.h"

namespace rb {

   PID::PID(){
	this->setdefault();	
   }

 /* ArichPID::ArichPID(float* scores[5]){
       pid = caf::SRPID(scores[5]);
   }
 */

   PID::~PID(){}

   void PID::setdefault(){
      nhits =-1; 
  }
  

}


