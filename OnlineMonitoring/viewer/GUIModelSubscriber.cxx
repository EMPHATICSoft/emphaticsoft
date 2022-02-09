#include "OnlineMonitoring/viewer/GUIModelSubscriber.h"
#include <unistd.h>
#include "OnlineMonitoring/viewer/GUIModel.h"
using namespace emph::onmon;

static bool gsLock = false;

//......................................................................

GUIModelSubscriber::GUIModelSubscriber(unsigned int which)
{
  GUIModel::Instance().Subscribe(this, which);
}

//......................................................................

GUIModelSubscriber::~GUIModelSubscriber()
{
  GUIModel::Instance().Unsubscribe(this);
}

//......................................................................

bool GUIModelSubscriber::GetLock() 
{
  unsigned int i;
  const unsigned int kSleep = 1000;  // 1 msec
  const unsigned int kCount = 10000; // 10k tries at 1 msec = 10 seconds
  for (i=0; i<kCount; ++i) {
    if (gsLock==false) { 
      gsLock = true;
      return true;
    }
    usleep(kSleep);
  }
  return false;
}

//......................................................................

void GUIModelSubscriber::ReleaseLock() 
{
  gsLock = false;
}

////////////////////////////////////////////////////////////////////////
