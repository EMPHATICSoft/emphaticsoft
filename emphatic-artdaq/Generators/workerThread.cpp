#include <utility>
#include <chrono>
#include <list>
#include <vector>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <unistd.h>

#include "workerThread.hh"

bool share::WorkerThreadFunctor::call()
{
  return _functor();
}

share::WorkerThread::WorkerThread(WorkerThreadFunctorUPtr& functor, 
				       unsigned int usec_delay_if_failed,
				       unsigned int max_stop_tries)
  :_functor(std::move(functor)), 
   _stop_running( false ),
   _usec_delay_iffailed(usec_delay_if_failed),
   _maxStopAttempts(max_stop_tries) 
{
}

void share::WorkerThread::start(){
  std::cout << "WorkerThread: Starting " << _functor->name() << std::endl;
  _stop_running=false;
  _worker_thread_result = std::async ( std::launch::async, &WorkerThread::run, this );
  std::cout << "WorkerThread: Started " << _functor->name() << std::endl;
}

void share::WorkerThread::stop(){
  if ( _stop_running ) {
    std::cout << "WorkerThread: " << _functor->name() << " is not running." << std::endl;
    std::cout << "WorkerThread: Stopping " << _functor->name() << std::endl;
    return;
  }

  _stop_running = true;
    
  int availableRetryCount = _maxStopAttempts; // if the thread function is still runing give it another 10 seconds to complete

  bool isDone = _worker_thread_result.wait_for ( std::chrono::seconds ( 1 ) )  == std::future_status::ready;
  
  while ( ! isDone  && availableRetryCount-- )      
    isDone = _worker_thread_result.wait_for ( std::chrono::seconds ( 1 )) == std::future_status::ready ;    
  
  if ( !isDone ){
    std::cout << "ERROR! Cannot stop " << _functor->name() << std::endl;
  }
  //throw RuntimeErrorException ( "Unable to stop WorkerThread task:" +  _functor->name());
  _worker_thread_result.get();
  std::cout << "WorkerThread: " << _functor->name() 
    //<< " exit code=" << _worker_thread_result.get() 
	    << std::endl;
}

//uint64_t share::WorkerThread::run() {
void share::WorkerThread::run() {
    uint64_t iterationCount = 0;
    std::cout << "WorkerThread: " << _functor->name() << " started running." << std::endl;;

    try {
      while ( !_stop_running ) {
	if(_functor->call())
	  ++iterationCount;
	else
	  usleep(_usec_delay_iffailed);
      }    
    } catch ( ... ) {
      std::cout << "WorkerThread: " << "Caught exception while running "<< _functor->name() << ". Nested exception:Unknown" << std::endl;
    }

    std::cout << "WorkerThread: " << _functor->name() << " stopped; Iteration count:" << iterationCount << std::endl;
    
    _stop_running=true;
    
    //return iterationCount;
}

// kate: indent-mode cstyle; indent-width 1; replace-tabs on; ;






