#include "StandardRecord/SRArichID.h"
#include "RecoBase/ArichID.h"

namespace rb{


   ArichID::ArichID(){
	this->setdefault();	
   }

   ArichID::~ArichID(){}

   void ArichID::setdefault(){
	SRArichID::setdefault();
	nhit = 0;
	
   }

}


