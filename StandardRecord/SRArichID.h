#ifndef SRARICHID_H
#define SRARICHID_H

#include "StandardRecord/SRPID.h"

namespace caf{

  class SRArichID: public SRPID{

  public:
	SRArichID();
	~SRArichID();

	int Track_ID;
	int ARing_ID;
	SRPID pid;
	virtual void setdefault();

   };

}

#endif
