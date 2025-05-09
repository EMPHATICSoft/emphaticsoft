#ifndef PID_H
#define PID_H

#include "StandardRecord/SRPID.h"

namespace rb {

  class PID: public caf::SRPID{

  public:
	PID();
	//ArichPID(float *scores[5]);
	~PID();

	int nhits;
	virtual void setdefault();
   };
  
}

#endif
