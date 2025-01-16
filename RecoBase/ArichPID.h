#ifndef ARICHPID_H
#define ARICHPID_H

#include "StandardRecord/SRPID.h"

namespace rb {

  class ArichPID: public caf::SRPID{

  public:
	ArichPID();
	ArichPID(float *scores[5]);
	ArichPID(std::vector<double> scores);
	~ArichPID();

	int Track_ID;
	int ARing_ID;
	caf::SRPID pid;

	virtual void setdefault();
	friend std::ostream& operator << (std::ostream& o, const ArichPID& h);
   };
  
}

#endif
