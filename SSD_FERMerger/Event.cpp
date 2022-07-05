#include "Event.h"
#include <iostream>
#include <math.h>


using namespace std;


//-----------------------------------------------------------------------
EventMonicelli::EventMonicelli(void) : 
  beginTimestamp_       (0)
, currentTimestamp_     (0)
, triggerNumber_        (0)
, currentBCONumber_     (-1)
, reset_                (false)
{
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
}

//-----------------------------------------------------------------------
EventMonicelli::EventMonicelli(const EventMonicelli& event)
{
  *this=event;
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
}

//-----------------------------------------------------------------------
EventMonicelli& EventMonicelli::operator=(const EventMonicelli& event)
{
  reset();
  beginTimestamp_	  = event.beginTimestamp_;
  currentTimestamp_	= event.currentTimestamp_;
  currentBCONumber_ = event.currentBCONumber_;
  triggerNumber_	  = event.triggerNumber_;
  reset_		        = event.reset_;
  timestamps_ 		  = event.timestamps_;
 
  Hit* hitP;
  for(multimap<uint64_t, Hit*>::const_iterator it=event.timestampHits_.begin(); it!=event.timestampHits_.end(); it++){
    hitP = new Hit(*(it->second));
    hits_.push_back(hitP);
    timestampHits_.insert(pair<uint64_t,Hit*>(it->first, hitP));  
  }  
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
  return *this;
}

//-----------------------------------------------------------------------
EventMonicelli::~EventMonicelli(void)
{
//  cout << __PRETTY_FUNCTION__ << hits_.size() << endl;
  reset();
}

//-----------------------------------------------------------------------
void EventMonicelli::addHit(const Hit& hit){
  Hit* hitP = new Hit(hit);
  hits_.push_back(hitP);
  timestampHits_.insert(pair<unsigned long long,Hit*>(currentTimestamp_,hitP));
}

//-----------------------------------------------------------------------
Hit& EventMonicelli::getHit(unsigned int hitN){
  return *(hits_[hitN]);
}

//-----------------------------------------------------------------------
void EventMonicelli::reset(void)
{
  beginTimestamp_        = 0;
  currentTimestamp_      = 0;
  triggerNumber_         = 0;
  reset_                 = false;
  
  for(vector<Hit*>::iterator it = hits_.begin(); it != hits_.end(); it++)
    delete *it;
  hits_.clear();

  timestampHits_           .clear();
  timestamps_              .clear();
}

//-----------------------------------------------------------------------
EventMonicelli& EventMonicelli::operator+=(const EventMonicelli& event)
{
  Hit* hitP = 0;
  for(multimap<uint64_t,Hit*>::const_iterator it=event.timestampHits_.begin(); it!=event.timestampHits_.end(); it++)
  {
    hitP = new Hit(*(it->second));
    hits_.push_back(hitP);
    timestampHits_.insert(pair<uint64_t,Hit*>(it->first, hitP));
    if(!(timestampHits_.find(it->first) != timestampHits_.end())){
      timestamps_.push_back(it->first);
    }
  }

  beginTimestamp_ = timestampHits_.begin()->first;
  if(triggerNumber_ == 0)
  {
    triggerNumber_ = event.triggerNumber_;
  }
  return *this;
}
