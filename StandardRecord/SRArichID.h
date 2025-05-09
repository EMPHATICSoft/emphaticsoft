#ifndef SRARICHID_H
#define SRARICHID_H

#include "StandardRecord/SRPID.h"

namespace caf{

  class SRArichID: public SRPID{

  public:
	SRArichID();
	virtual	~SRArichID();

	int nhit;
	virtual void setdefault();

   };

}

#endif
