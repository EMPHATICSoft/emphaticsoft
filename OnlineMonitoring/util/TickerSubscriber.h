///
/// \file    TickerSubscriber.h
/// \brief   Base class for subscribers to ticker updates
/// \version $Id: TickerSubscriber.h,v 1.4 2012-02-04 02:56:25 messier Exp $
/// \author  messier@indiana.edu
///
#ifndef OM_TICKERSUBSCRIBER_H
#define OM_TICKERSUBSCRIBER_H
#include <cstdlib>

namespace om {
  class TickerSubscriber
  {
  public:
    enum updates_ {
      kRun    = 1<<0,
      kSubrun = 1<<1,
      k30sec  = 1<<2,
      k1min   = 1<<3,
      k5min   = 1<<4,
      k10min  = 1<<5,
      k30min  = 1<<6,
      kHour   = 1<<7,
      k24hr   = 1<<8,
      kUTC    = 1<<9,
      kAll    = 0xFFFF
    };

  public:
    TickerSubscriber(unsigned int which);
    virtual ~TickerSubscriber(){};

    virtual void RunTicker()          { abort(); }
    virtual void SubrunTicker()       { abort(); }
    virtual void ThirtySecTicker()    { abort(); }
    virtual void OneMinTicker()       { abort(); }
    virtual void FiveMinTicker()      { abort(); }
    virtual void TenMinTicker()       { abort(); }
    virtual void ThirtyMinTicker()    { abort(); }
    virtual void HourTicker()         { abort(); }
    virtual void TwentyFourHrTicker() { abort(); }
  };
}
#endif
////////////////////////////////////////////////////////////////////////
