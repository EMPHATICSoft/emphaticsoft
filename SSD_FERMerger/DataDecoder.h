#ifndef _DataDecoder_h_
#define _DataDecoder_h_

#include "Hit.h"
#include "Status.h"
#include "stib.h"
#include <string>

struct PxDecodedHit
{
   unsigned int chan; 
   unsigned int chipid;
   unsigned int set;  
   unsigned int strip;
   unsigned int bco;  
   unsigned int adc;  
   unsigned int istrip;
};

class DataDecoder{
 public:
  DataDecoder (void);
  DataDecoder (const std::string& dataString, bool isTimestamp=false, int station=-1);
  DataDecoder (unsigned long long data,       bool isTimestamp=false, int station=-1);
  ~DataDecoder(void);

  //Setters
  void setData     (const std::string& dataString, int station=-1);
  void setData     (unsigned long long data,       int station=-1);
  void setTimestamp(const std::string& dataString);
  void setTimestamp(unsigned long long data);
  void setTrigger  (unsigned int currentTrigger);

  //Getters
  const unsigned long long& getData        (void){return data_;}
  unsigned long long        getTimestamp   (void);
  unsigned long             getPacketNumber(void);
 
  bool               isData       (void);
  unsigned long long order        (void);
  unsigned long long convert      (const std::string& dataString);
  Hit                decodeHit    (const std::string& dataString, int station=-1);
  Hit                decodeHit    (unsigned long long data,       int station=-1);
  Hit                decodeHit    (void);
  Status             decodeStatus (void);
  int                decodeTrigger(const std::string& dataString);
  int                decodeTrigger(unsigned long long data);
  int                decodeTrigger(void);
  void               addStation   (int station);
 private:
  unsigned long long unOrderedData(void);
  unsigned long long data_;
  unsigned long long unOrderedData_;
  bool               ordered_;
};

#endif
