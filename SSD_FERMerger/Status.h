#ifndef _Status_h_
#define _Status_h_

#include <iostream>
#include <iomanip>
  // error(62) | rep_err(61) | tok_in_err(60) | tok_out_err(59) | st_err(58) | plaq(57:56) | tokin(55:42) | tokout(41:28) | trig(27:14) | ub(13:0)

class Status{
 public:
  unsigned long long data;
  int error;
  int repErr;
  int tokenInError;
  int tokenOutError;
  int stError;
  int plaquette;
  int tokenIn;
  int tokenOut;
  int trigger;
  int ultraBlack;
  friend std::ostream& operator<<(std::ostream& os, const Status& status)
  {
//    os << "[Status::operator<<()]\t" 
    os 
       << "Data: "           << std::hex << (status.data >> 32) << (status.data & 0xffffffff) << std::dec
       << " error : "	     << status.error
       << " repErr: "        << status.repErr
       << " tokenInError: "  << status.tokenInError
       << " tokenOutError: " << status.tokenOutError
       << " stError: "	     << status.stError
       << " plaquette: "     << status.plaquette
       << " tokenIn: "	     << status.tokenIn
       << " tokenOut: "      << status.tokenOut
       << " trigger: "       << status.trigger
       << " ultraBlack: "    << status.ultraBlack;
    return os;
  } 
};


#endif
