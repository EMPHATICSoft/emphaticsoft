///
/// \file    TickerSubscriber.cxx
/// \brief   Base class for subscribers to ticker updates
/// \version $Id: TickerSubscriber.cxx,v 1.1 2012-01-03 17:58:50 messier Exp $
/// \author  messier@indiana.edu
///
#include "OnlineMonitoring/util/TickerSubscriber.h"
#include "OnlineMonitoring/util/Ticker.h"
using namespace om;

TickerSubscriber::TickerSubscriber(unsigned int which) 
{
  Ticker::Instance().Subscribe(this, which);
}

////////////////////////////////////////////////////////////////////////
