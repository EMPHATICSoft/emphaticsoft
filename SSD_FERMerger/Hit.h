 #ifndef _Hit_h_
#define _Hit_h_

#include <iostream>
#include <iomanip>

//#include "DataDecoder.h"
//#include "HitTranslator.h"

class Hit{
 public:
  Hit();
  ~Hit();
  unsigned long long data;
  short detId  ;
  short station;
  short plaq   ;
  short cid    ;
  short row    ;
  short col    ;
  short adc    ;
  int 	trig   ;
  short	bco   ;
  const unsigned long long& getData(void) const {return data;}
 private:
  //static DataDecoder   theDataDecoder_;
  //static HitTranslator theHitTranslator_;

  friend std::ostream& operator<<(std::ostream& os, const Hit& hit){
    os << "Data: "     << std::hex << (hit.data >> 32) << (hit.data & 0xffffffff) << std::dec
       << " detId: "   << hit.detId
       << " station: " << hit.station
       << " plaq: "    << hit.plaq
       << " cid: "     << hit.cid
       << " row: "     << hit.row
       << " col: "     << hit.col
       << " adc: "     << hit.adc;
    return os;		      
  } 
};


#endif
