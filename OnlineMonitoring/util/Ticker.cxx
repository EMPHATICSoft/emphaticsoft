///
/// \file    Ticker.cxx
/// \brief   Notify subscribers when certain time/event intervals have elapsed
/// \version $Id: Ticker.cxx,v 1.5 2012-02-04 02:56:25 messier Exp $
/// \author  messier@indiana.edu
///
#include "OnlineMonitoring/util/Ticker.h"
#include "OnlineMonitoring/util/TickerSubscriber.h"
#include <iostream>
using namespace om;

Ticker::Ticker() :
  fLastRun   (kUninitRun),
  fLastSubrun(kUninitRun),
  fLast30Sec (kUninitTime),
  fLast1Min  (kUninitTime),
  fLast5Min  (kUninitTime),
  fLast10Min (kUninitTime),
  fLast30Min (kUninitTime),
  fLastHour  (kUninitTime),
  fLast24Hr  (kUninitTime)
{ }

//......................................................................

Ticker& Ticker::Instance() 
{
  static Ticker johnbonham;
  return johnbonham;
}

//......................................................................

void Ticker::Update(unsigned int run, unsigned int subrun) 
{
  // Get current UNIX time in seconds
  time_t    tt = time(0);
  struct tm t;
  gmtime_r(&tt, &t);
  
  //
  // Initialize the counters if we have to.
  //
  if (fLastRun   ==kUninitRun)  fLastRun    = run;
  if (fLastSubrun==kUninitRun)  fLastSubrun = subrun;
  if (fLast30Sec ==kUninitTime) fLast30Sec  = t.tm_sec;
  if (fLast1Min  ==kUninitTime) fLast1Min   = t.tm_min;
  if (fLast5Min  ==kUninitTime) fLast5Min   = t.tm_min;
  if (fLast10Min ==kUninitTime) fLast10Min  = t.tm_min;
  if (fLast30Min ==kUninitTime) fLast30Min  = t.tm_min;
  if (fLastHour  ==kUninitTime) fLastHour   = t.tm_hour;
  if (fLast24Hr  ==kUninitTime) fLast24Hr   = t.tm_yday;

  bool do30sec = (t.tm_sec !=fLast30Sec && (t.tm_sec%30==0));
  bool do1min  = (t.tm_min !=fLast1Min);
  bool do5min  = (t.tm_min !=fLast5Min  && (t.tm_min%5==0));
  bool do10min = (t.tm_min !=fLast10Min && (t.tm_min%10==0));
  bool do30min = (t.tm_min !=fLast30Min && (t.tm_min%30==0));
  bool dohour  = (t.tm_hour!=fLastHour);
  bool do24hr  = (t.tm_yday!=fLast24Hr);

  /*
  std::cout << "**" << std::endl;
  std::cout << "30s:\t"<<t.tm_sec<<"\t"<<fLast30Sec<<"\t"<<do30sec<<std::endl;
  std::cout << "1m:\t" <<t.tm_min<<"\t"<<fLast1Min <<"\t"<<do1min <<std::endl;
  std::cout << "5m:\t" <<t.tm_min<<"\t"<<fLast5Min <<"\t"<<do5min <<std::endl;
  std::cout << "10m:\t"<<t.tm_min<<"\t"<<fLast10Min<<"\t"<<do10min<<std::endl;
  std::cout << "30m:\t"<<t.tm_min<<"\t"<<fLast30Min<<"\t"<<do30min<<std::endl;
  std::cout << "1h:\t" <<t.tm_hour<<"\t"<<fLastHour<<"\t"<<dohour<<std::endl;
  std::cout << "24h:\t"<<t.tm_yday<<"\t"<<fLast24Hr<<"\t"<<do24hr<<std::endl;
  */
  
  if (do30sec) { fLast30Sec = t.tm_sec;  this->Publish30Sec(); }
  if (do1min)  { fLast1Min  = t.tm_min;  this->Publish1Min();  }
  if (do5min)  { fLast5Min  = t.tm_min;  this->Publish5Min();  }
  if (do10min) { fLast10Min = t.tm_min;  this->Publish10Min(); }
  if (do30min) { fLast30Min = t.tm_min;  this->Publish30Min(); }
  if (dohour)  { fLastHour  = t.tm_hour; this->PublishHour();  }
  if (do24hr)  { fLast24Hr  = t.tm_yday; this->Publish24Hr();  }

  //
  // Check the run/subrun tickers and publish if we need to. Note that
  // the "run update" should also trigger a "subrun update" since
  // logically the last subrun of a run ends just prior to the end of
  // the run (try saying that ten times fast!). When publishing, be
  // sure to update the run and subrun numbers of the last update.
  //
  if (fLastRun!=run) { 
    fLastRun    = run;
    fLastSubrun = subrun;
    this->PublishSubrun();
    this->PublishRun();
  }
  if (fLastSubrun!=subrun) {
    fLastSubrun = subrun;
    this->PublishSubrun();
  }
}

//......................................................................

void Ticker::PublishRun() 
{
  std::list<TickerSubscriber*>::iterator i(fRunSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(fRunSubscribers.end());
  for (; i!=e; ++i) (*i)->RunTicker();
}

//......................................................................

void Ticker::PublishSubrun() 
{
  std::list<TickerSubscriber*>::iterator i(fSubrunSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(fSubrunSubscribers.end());
  for (; i!=e; ++i) (*i)->SubrunTicker();
}

//......................................................................

void Ticker::Publish30Sec() 
{
  std::list<TickerSubscriber*>::iterator i(f30SecSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(f30SecSubscribers.end());
  for (; i!=e; ++i) (*i)->ThirtySecTicker();
}

//......................................................................

void Ticker::Publish1Min() 
{
  std::list<TickerSubscriber*>::iterator i(f1MinSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(f1MinSubscribers.end());
  for (; i!=e; ++i) (*i)->OneMinTicker();
}

//......................................................................

void Ticker::Publish5Min() 
{
  std::list<TickerSubscriber*>::iterator i(f5MinSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(f5MinSubscribers.end());
  for (; i!=e; ++i) (*i)->FiveMinTicker();
}

//......................................................................

void Ticker::Publish10Min() 
{
  std::list<TickerSubscriber*>::iterator i(f10MinSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(f10MinSubscribers.end());
  for (; i!=e; ++i) (*i)->TenMinTicker();
}

//......................................................................

void Ticker::Publish30Min() 
{
  std::list<TickerSubscriber*>::iterator i(f30MinSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(f30MinSubscribers.end());
  for (; i!=e; ++i) (*i)->ThirtyMinTicker();
}

//......................................................................

void Ticker::PublishHour() 
{
  std::list<TickerSubscriber*>::iterator i(fHourSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(fHourSubscribers.end());
  for (; i!=e; ++i) (*i)->HourTicker();
}

//......................................................................

void Ticker::Publish24Hr() 
{
  std::list<TickerSubscriber*>::iterator i(f24HrSubscribers.begin());
  std::list<TickerSubscriber*>::iterator e(f24HrSubscribers.end());
  for (; i!=e; ++i) (*i)->TwentyFourHrTicker();
}

//......................................................................

void Ticker::Subscribe(TickerSubscriber* s, unsigned int which) 
{
  if ((which&TickerSubscriber::kRun)!=0) {
    fRunSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::kSubrun)!=0) {
    fSubrunSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::k30sec)!=0) {
    f30SecSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::k1min)!=0) {
    f1MinSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::k5min)!=0) {
    f5MinSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::k10min)!=0) {
    f10MinSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::k30min)!=0) {
    f30MinSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::kHour)!=0) {
    fHourSubscribers.push_back(s);
  }
  if ((which&TickerSubscriber::k24hr)!=0) {
    f24HrSubscribers.push_back(s);
  }
}

////////////////////////////////////////////////////////////////////////
