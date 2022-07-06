#ifndef _EventMonicelli_h_
#define _EventMonicelli_h_

#include "SSD_FERMerger/Hit.h"
#include <vector>
#include <map>
#include <iostream>

class EventMonicelli{
public:
  EventMonicelli();
  EventMonicelli(const EventMonicelli& event);
  ~EventMonicelli();

  //Setters
  void setTimestamp        (uint64_t time            ){beginTimestamp_ = time; currentTimestamp_ = time;timestamps_.push_back(time);}
  void addTimestamp        (uint64_t time            ){currentTimestamp_ = time;timestamps_.push_back(time);}
  void setTriggerNumber    (uint32_t trigger         ){triggerNumber_ = trigger;}
  void setReset            (bool reset               ){reset_ = reset;}
  void setCurrentBCONumber (uint64_t currentBCONumber){currentBCONumber_ = currentBCONumber;}

  //Getters
  std::vector<uint64_t> getTimestamps       (void){return timestamps_;}
  const uint64_t&       getTimestamp        (void){return beginTimestamp_;}
  uint64_t              getCurrentBCONumber (void){return currentBCONumber_;}
  uint64_t              getCurrentTimestamp (void){return currentTimestamp_;}
  uint32_t              getTriggerNumber    (void){return triggerNumber_;}
  unsigned int  	getNumberOfHits	    (void){return hits_.size();}
  ssd::Hit&          	getHit		    (unsigned int hitN);

  //Other Methods
  bool isReset   (void){return reset_;}
  void addHit	   (const ssd::Hit& hit);
  void reset	   (void);
  
  EventMonicelli& operator= (const EventMonicelli& event);
  EventMonicelli& operator+=(const EventMonicelli& event);
  
  friend std::ostream& operator<<(std::ostream& os, EventMonicelli& event){
    //    os << "[EventMonicelli::operator<<()]\t"
    os
      << "Trigger: "   << event.triggerNumber_
      << " Bco: "      << event.currentBCONumber_
      << " Time: "     << event.beginTimestamp_
      << " N times: "  << event.timestamps_.size() 
      << " N hits: "   << event.hits_.size()
      << " Reset: "    << event.reset_
      << std::endl;
    return os;
  }
  
private:
  uint64_t	                  	beginTimestamp_;
  uint64_t                      	currentTimestamp_;
  uint64_t                              currentBCONumber_;
  uint32_t		                triggerNumber_;
  bool        		                reset_;
  std::vector<ssd::Hit*>                hits_;
  std::multimap<uint64_t,ssd::Hit*>   	timestampHits_;
  std::vector<uint64_t>         	timestamps_;
};


#endif
