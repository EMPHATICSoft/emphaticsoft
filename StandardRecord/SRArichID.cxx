#include "StandardRecord/SRPID.h"
#include "StandardRecord/SRArichID.h"
namespace caf{


   SRArichID::SRArichID(){
	this->setdefault();	
   }

   SRArichID::~SRArichID(){}

   void SRArichID::setdefault(){
	
        Track_ID = -1;
        ARing_ID = -1;
	pid.Reset();	
   }

}


