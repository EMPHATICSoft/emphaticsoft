#ifndef ARICHPID_H
#define ARICHPID_H

#include "StandardRecord/SRArichID.h"

namespace rb{

  class ArichID: public caf::SRArichID{

  public:
	ArichID();
	~ArichID();

	int nhit;
	virtual void setdefault();

   };

}

#endif
