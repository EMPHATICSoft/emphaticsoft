#ifndef _UBOONE_SHARE_WORKERTHREAD_H
#define _UBOONE_SHARE_WORKERTHREAD_H 1

#include <memory>
#include <atomic>
#include <future>

#include <utility>
#include <list>
#include <vector>
#include <sstream>
#include <type_traits>

namespace share
{

class WorkerThread;

typedef std::unique_ptr<WorkerThread> WorkerThreadUPtr;
typedef std::shared_ptr<WorkerThread> WorkerThreadSPtr;


typedef std::function < bool ( void ) >   ThreadFunctor;
typedef std::unique_ptr<ThreadFunctor> ThreadFunctorUPtr;

class WorkerThreadFunctor final
{
public:
    explicit WorkerThreadFunctor ( ThreadFunctor& functor, std::string name="WorkerThread" )
        : _functor ( functor ),_name ( name ) {}

    bool call();

    const std::string& name() const {
        return _name;
    }

private:
    ThreadFunctor       _functor;
    const std::string   _name;
};

typedef std::unique_ptr<WorkerThreadFunctor> WorkerThreadFunctorUPtr;

class WorkerThread
{

public:
    static WorkerThreadUPtr
    createWorkerThread ( WorkerThreadFunctorUPtr& functor,
                         unsigned int usec_delay_if_failed=10e3,
                         unsigned int max_stop_tries=10 ) {
        return WorkerThreadUPtr ( new WorkerThread ( functor,usec_delay_if_failed,max_stop_tries ) );
    }

    void start();
    void stop();

private:
    explicit WorkerThread ( WorkerThreadFunctorUPtr& functor,
                            unsigned int usec_delay_if_failed=10e3,
                            unsigned int max_stop_tries=10 );

    //uint64_t run();
    void run();

    WorkerThread() = delete;
    WorkerThread ( WorkerThread const & ) = delete;
    WorkerThread& operator= ( WorkerThread const & ) = delete;
    WorkerThread ( WorkerThread && ) = delete;
    WorkerThread& operator= ( WorkerThread && ) = delete;

private:
    WorkerThreadFunctorUPtr _functor;
    const std::string       _name;
    std::atomic_bool        _stop_running;
    const unsigned int      _usec_delay_iffailed;
    const unsigned int      _maxStopAttempts;
    //std::future<uint64_t>   _worker_thread_result;
    std::future<void>   _worker_thread_result;

};


}  // end of namespace share

#endif //_UBOONE_SHARE_WORKERTHREAD_H






