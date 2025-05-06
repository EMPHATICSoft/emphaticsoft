#ifndef ARICHPID_H
#define ARICHPID_H

#include "StandardRecord/SRPID.h"

namespace rb {

  class ArichPID: public caf::SRPID{

  public:
	ArichPID();
	//ArichPID(float *scores[5]);
	~ArichPID();

	int nhits;
	virtual void setdefault();
   };
  
}

#endif
