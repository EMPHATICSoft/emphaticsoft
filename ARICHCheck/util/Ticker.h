///
/// \file    Ticker.h
/// \brief   Notify subscribers when certain time/event intervals have elapsed
/// \version $Id: Ticker.h,v 1.6 2012-11-12 20:48:10 mbaird42 Exp $
/// \author  messier@indiana.edu
///
#ifndef OM_TICKER_H
#define OM_TICKER_H
#include <climits>
#include <ctime>
#include <list>
namespace emph { namespace onmon { class TickerSubscriber; } }

namespace emph { 
  namespace onmon {
    class Ticker {
    public:
      static Ticker& Instance();
      void Update(unsigned int run, unsigned int subrun);
      void Subscribe(TickerSubscriber* s, unsigned int which);

    private:
      void PublishRun();
      void PublishSubrun();
      void Publish30Sec();
      void Publish1Min();
      void Publish5Min();
      void Publish10Min();
      void Publish30Min();
      void PublishHour();
      void Publish24Hr();
    
    private:
      /// Ticker is a singleton - constructor is private.
      Ticker();
      
      /// Define what an uninitialized run/subrun number looks like
      static const unsigned int kUninitRun = UINT_MAX;
      
      /// Define what an uninitialized time looks like
      static const int kUninitTime = -1;

    private:
      unsigned int fLastRun;    ///< Run number of last update
      unsigned int fLastSubrun; ///< Subrun number of last update
      int          fLast30Sec;  ///< Time of last 30 second update
      int          fLast1Min;   ///< Time of last 1 minute update
      int          fLast5Min;   ///< Time of last 5 minute update
      int          fLast10Min;  ///< Time of last 10 minute update
      int          fLast30Min;  ///< Time of last 30 minute update
      int          fLastHour;   ///< Time of last hour update
      int          fLast24Hr;   ///< Time of last 24 hour update
      std::list<TickerSubscriber*> f30SecSubscribers;
      std::list<TickerSubscriber*> f1MinSubscribers;
      std::list<TickerSubscriber*> f5MinSubscribers;
      std::list<TickerSubscriber*> f10MinSubscribers;
      std::list<TickerSubscriber*> f30MinSubscribers;
      std::list<TickerSubscriber*> fHourSubscribers;
      std::list<TickerSubscriber*> f24HrSubscribers;
      std::list<TickerSubscriber*> fRunSubscribers;
      std::list<TickerSubscriber*> fSubrunSubscribers;
    };
  } //end namespace onmon
} //end namespace emph
#endif
////////////////////////////////////////////////////////////////////////
